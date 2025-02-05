#include "LFAgent.h"
#include "LFScanners.h"
#include "LFEngine.h"

#include <random>

#include "agent/detectors.h"
#include "agent/correctors.h"
#include "agent/corrector_trainer.h"
#include "agent/tests.h"

#ifdef _OMP_
#include <omp.h>
#endif

using namespace agent;


struct TItemAttributes {
	// Detected object type
	std::string_view	type;
	int					base_width = 24;
	int					base_height = 24;
	int					angle = 0;
	int					racurs = 0;
	// Name of detector
	std::string_view	name;
};

static std::vector<TLFDetectedItem> NonMaximumSuppression(
	std::vector<std::pair<TLFRect, float>>& rects,
	float overlap_threshold, const TItemAttributes& attrs);

///////////////////////////////////////////////////////////////////////////////

TLFAgent::TLFAgent() 
	: pool_([this]() { return std::make_unique<agent::TFeaturesBuilder>(); }) {
}

 std::vector<TLFDetectedItem> TLFAgent::Detect(std::shared_ptr<TLFImage> img) {

	auto scanner = detector_->GetScanner();
	
	if (!scanner)
		return {};

	agent::TDetections	gt_detections;

	scanner->ScanImage(img.get());
// We can process objects without supervisor also
	if (supervisor_ && trainer_) {
		// Process image and get ground truth bounds
		gt_detections = supervisor_->Detect(img);
		auto correctors = trainer_->ConsumeCorrectors();
		if (!correctors.empty())
			correctors_.AddCorrectors(std::move(correctors));
	}

	auto fragments_count = scanner->GetFragmentsCount();

	if (fragments_count == 0)
		return {};


	size_t batches_count = size_t(std::ceil(fragments_count / batch_size_));
			
	int threads = 1;

#ifdef _OMP_
	threads = std::max<int>(omp_get_max_threads(), max_threads_);
#endif 

	// Acquire workers for processing;
	if (workers_.size() < threads) {
		workers_.resize(threads);
		for (auto& worker : workers_) {
			worker = std::move(detector_->CreateWorker());
		}
	}

	prior_detections_.clear();
	

#ifdef _OMP_
#pragma omp parallel for num_threads(threads)
#endif
	for (auto batch = 0; batch <= batches_count; batch++) {

		int current_thread = 0;

#ifdef _OMP_
		current_thread = omp_get_thread_num();
#endif 
		// Acquire object from pool or create new one
		auto features = pool_.GetObject();

		// Init features builder
		size_t batch_begin = batch_size_ * batch;
		size_t batch_end = batch_begin + batch_size_;
		batch_end = batch_end < fragments_count ? batch_end : fragments_count;

		// Setting up bounds of batch and alloc memory if needed
		features->SetBounds(batch_begin, batch_end);

		// Process batch by detector
		workers_[current_thread]->Detect(img, *features.get());

		// Apply corrections and calc corrections vector
		correctors_.Apply(*features.get(), features->GetMutableCorrections());

		// No new correctors without supervisor
		if (supervisor_ && trainer_) {
			// Process features and check if we can train new corrector(s)
			trainer_->CollectSamples(scanner, *features.get(), gt_detections);
			
		}
		
		// Prepare for NMS
		for (size_t n = features->frags_begin(); n < features->frags_end(); ++n) {
			if (features->GetCorrectedResult(n)) {
#ifdef _OMP_
#pragma omp critical 
#endif
				{
					prior_detections_.emplace_back(	scanner->GetFragmentRect(n), 
													features->GetScore(n));
				}
			}
		}
	}

	if (supervisor_ && trainer_) {
		trainer_->Train();
	}

	TItemAttributes attr = {	detector_->GetType(), 
								scanner->GetBaseWidth(), 
								scanner->GetBaseHeight(), 0, 0, 
								detector_->GetName() };

	return NonMaximumSuppression(prior_detections_, nms_threshold_, attr);
	//return {};
}

bool TLFAgent::LoadXML(TiXmlElement* agent_node) {

	agent_node->QueryValueAttribute("batch_size", &batch_size_);
	agent_node->QueryValueAttribute("max_threads", &max_threads_);
	agent_node->QueryValueAttribute("nms_threshold", &nms_threshold_);

	auto detector_interface_node = agent_node->FirstChildElement("IDetector");
	if (!detector_interface_node) {
		return false;
	}

	auto detector_node = detector_interface_node->FirstChildElement();

	if (!detector_node) return false;

	decltype(detector_) det;
	
	if (detector_node->ValueStr() == "TStagesDetector") {

		auto detector = std::make_unique<TStagesDetector>();
		
		if (!detector->LoadXML(detector_node))
			return false;

		det = std::move(detector);
	}
	else if (detector_node->ValueStr() == "TRandomDetector") {
		auto detector = std::make_unique<TRandomDetector>();
		
		if (!detector->LoadXML(detector_node))
			return false;

		det = std::move(detector);
	}

	auto trainer_interface_node = agent_node->FirstChildElement("ICorrectorTrainer");

	if (!trainer_interface_node) {
		return false;
	}

	auto trainer_node = trainer_interface_node->FirstChildElement();

	decltype(trainer_)	tr;

	if (trainer_node) {
		if (trainer_node->ValueStr() == "TCorrectorTrainerBase") {

			auto trainer = std::make_unique<TCorrectorTrainerBase>();
			
			if (!trainer->LoadXML(trainer_node))
				return false;
			tr = std::move(trainer);
		}
	}

	Initialize(std::move(det), std::move(tr));

	auto correctors_node = agent_node->FirstChildElement("TCorrectorsCollection");

	if (correctors_node && !correctors_.LoadXML(correctors_node) ) {
		return false;
	}
		
	return true;

}

inline TiXmlElement* TLFAgent::SaveXML() {
	TiXmlElement* agent_node = new TiXmlElement("TLFAgent");

	agent_node->SetAttribute("batch_size", batch_size_);
	agent_node->SetAttribute("max_threads", max_threads_);
	agent_node->SetDoubleAttribute("nms_threshold", nms_threshold_);

	TiXmlElement* detector_node = new TiXmlElement("IDetector");

	if (auto node = detector_->SaveXML()) {
		detector_node->LinkEndChild(node);
	}
	
	agent_node->LinkEndChild(detector_node);

	if (trainer_) {
		TiXmlElement* trainer_node = new TiXmlElement("ICorrectorTrainer");

		if (auto node = trainer_->SaveXML()) {
			trainer_node->LinkEndChild(node);
		}
		agent_node->LinkEndChild(trainer_node);
	}

	TiXmlElement* correctors_node = correctors_.SaveXML();
	agent_node->LinkEndChild(correctors_node);

	return agent_node;
}


// Filter detections
static std::vector<TLFDetectedItem> NonMaximumSuppression(
	std::vector<std::pair<TLFRect, float>>& rects, 
	float overlap_threshold, const TItemAttributes& attrs ) {
	
	std::vector<TLFDetectedItem>	items;

	// Sort by score
	std::sort(rects.begin(), rects.end(), [](const auto& a, const auto& b) {
			return a.second > b.second;
	});

	for (int i = 0; i < rects.size(); ++i) {
		const auto& [base_rect, base_score] = rects[i];

		if (base_score == 0)
			continue;

		// Averaging position
	
		float x = base_rect.Center().X, y = base_rect.Center().Y;
		float w = base_rect.Width(), h = base_rect.Height();

		int similar_count = 1;

		for (size_t j = i + 1; j < rects.size(); ++j) {
			auto& [rect, score] = rects[j];

			if (score > 0 && rect.RectOverlap(base_rect) > overlap_threshold) {
				x += rect.Center().X;
				y += rect.Center().Y;
				w += rect.Width();
				h += rect.Height();

				similar_count++;


				// Skip from using in futher tests
				score = 0;
			}

		}
		x /= float(similar_count);
		y /= float(similar_count);
		w /= float(similar_count);
		h /= float(similar_count);

		awpRect r{ x - w / 2.f, y - h / 2.f, x + w / 2.f, y + h / 2.f };

		UUID id;
		// memset(id, 0, sizeof(UUID));
		LF_NULL_UUID_CREATE(id);
		// add object to result list

		{
			items.emplace_back(std::move(r), base_score, std::string(attrs.type),
				attrs.angle, attrs.racurs, attrs.base_width, attrs.base_height, std::string(attrs.name), id);
		}

	}

	std::cout << "Detected objects: " << rects.size() << " NMS: " << items.size() << std::endl;

	return items;
}

std::unique_ptr<TLFAgent>       LoadAgentFromEngine(const std::string& engine_path) {

	auto engine = std::make_unique<TLFDetectEngine>();
	if (!engine->Load(engine_path.c_str())) {
		std::cerr << "TLFDetectEngine couldn't parse file" <<
			engine_path << std::endl;
		return nullptr;
	}

	std::unique_ptr<ILFObjectDetector>  internal_detector(engine->GetDetector(0));

	// Detach detector from list.
	engine->RemoveDetector(0);

	auto detector = std::make_unique<agent::TStagesDetector>();

	if (!detector->Initialize(std::move(internal_detector))) {
		std::cerr << "Cant initialize detector from file" <<
			engine_path << std::endl;
	}

	auto trainer = agent::CreateBaselineTrainer("C:\\Users\\olgas\\Desktop\\python_mipt\\bsiiroko\\create_class_corr1.py", "C:\\Users\\olgas\\Desktop\\python_mipt\\bsiiroko\\");

	std::unique_ptr<TLFAgent>  agent = std::make_unique<TLFAgent>();
	agent->Initialize(std::move(detector), std::move(trainer));

	return agent;

}

