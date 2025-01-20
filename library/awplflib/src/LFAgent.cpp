#include "LFAgent.h"
#include "LFScanners.h"

#include <random>

#ifdef _OMP_
#include <omp.h>
#endif

using namespace agent;

static size_t GetRand(size_t low_dist, size_t high_dist) {
	static bool init_srand = false;
	if (!init_srand) {
		std::srand((unsigned int)std::time(nullptr));
		init_srand = true;
	}
	return low_dist + std::rand() % (high_dist + 1 - low_dist);
}

class TRandomSupervisor : public ILFSupervisor
{
public:
	TRandomSupervisor() {}
	virtual TDetections Detect(std::shared_ptr<TLFImage> img) override {
		//TODO implement python call
		auto width = img->GetImage()->sSizeX;
		auto height = img->GetImage()->sSizeY;
		
		TDetections detections;
		for (size_t i = 0; i < 10; i++) {
			awpRect rect;
			rect.left = GetRand(0, width - 5);
			rect.right = GetRand(rect.left, width);
			rect.top = GetRand(0, height - 5);
			rect.bottom = GetRand(rect.top, height);
			detections.push_back(rect);
		}
		
		return detections;
	}

};

/*
class TRandomCorrectorTrainer : public ICorrectorTrainer
{
public:
	// Setup new image samples extraction
	virtual void BeginImage(ILFScanner* scanner, const ILFSupervisor::TDetections&) override {
	}
	// Process new samples
	//start for each batchs
	virtual std::vector<std::unique_ptr<ICorrector>> ProcessSamples(const std::shared_ptr<TFeatures>& features) override {
		count_ += features->count();
		std::vector<std::unique_ptr<ICorrector>> vec;
		if (count_ >= max_count_) {
			//ICorrector* corrector = nullptr;
			//todo add corrector
			//vec.push()
		}
		return vec;
	}
	// Notify that image finished
	virtual void EndImage() override {
	}

private:
	size_t count_ = 0;
	const size_t max_count_ = 10000;
};*/

	
class TRandomWorker : public IWorker {
public:
	TRandomWorker(IDetector* detector) : detector_(detector) {}


	virtual void Detect(std::shared_ptr<TLFImage> img, TFeaturesBuilder& builder) override {
	
		assert(detector_);
		detector_->Detect(img, builder);
	
	}

private:
	
	IDetector* detector_ = nullptr;
};
	

class TRandomDetector : public IDetector
{
public:
	TRandomDetector() = default;

	bool Initialize() {

		scanner_ = new TLFScanner();

		size_t cascade_count = GetRand(3, 10);  // rand ot 3 do 10
		size_t count = 0;
		for (auto i = 0; i < cascade_count; i++) {
			size_t feature_count = GetRand(10, 100); // rand ot 10 do 100
			feature_count_list_.push_back(feature_count + count);
			count = feature_count_list_.back();
		}
		tmap_ptr_ = std::make_shared<TFeatures::TMap>(feature_count_list_);
		return (bool)tmap_ptr_;
	}

	virtual void Detect(std::shared_ptr<TLFImage> img, TFeaturesBuilder& builder) override {
		float* data = builder.GetMutableData().GetRow(0);
		size_t stride = builder.feats_count();
		auto triggered = builder.GetTriggered();
		auto map = builder.map();

		size_t detected = 0;

		for (auto i = 0; i < triggered.size(); i++) {
			// With probability 0.99 it is negative result
			if (GetRand(0, 100) > 1) {
				size_t cascade_number = GetRand(0, map->size() - 1);
				triggered[i] = cascade_number;
				size_t count = 0;
				for (auto j = 0; j < map->size(); j++) {
					if (j > cascade_number) break;					
					size_t cur_cascade_lenght = map.get()->at(j);
					SetRandData(data + (i * stride) + count, cur_cascade_lenght);
					count += cur_cascade_lenght;
				}
			}
			else { // fill all cascade
				SetRandData(data + (i * stride), stride);
				detected++;
			}
		}

		std::thread::id this_id = std::this_thread::get_id();

		std::stringstream ss;

		ss << "Detection thread " << this_id
			<< " frags_begin=" << builder.frags_begin()
			<< " frags_end=" << builder.frags_end()
			<< " detected=" << detected << std::endl;

		(std::cout << ss.str()).flush();

	}

	virtual ILFScanner* GetScanner()  override {
		return scanner_;
	}
	virtual std::unique_ptr<IWorker> CreateWorker() override {
		return std::make_unique<TRandomWorker>(this);
	}

	virtual bool LoadXML(TiXmlElement* parent)  override {
		return false;
	}
	virtual TiXmlElement* SaveXML()  override {
		return nullptr;
	}

	virtual TFeatures::TMapPtr GetMap() override { return tmap_ptr_; }

	

private:
	static void SetRandData(float* data, size_t size) {
		for (size_t i = 0; i < size; i++)
			data[i] = float(GetRand(1, 100));
	}

private:
	std::function<void(std::shared_ptr<TFeatures>)> callback_;
	std::vector<size_t> feature_count_list_;
	TFeatures::TMapPtr tmap_ptr_;
	ILFScanner* scanner_;

};


class TRandomAgent : public TLFAgent
{
public:
	TRandomAgent() : TLFAgent() {
	}

	bool Initialize() {
		auto detector = std::make_unique<TRandomDetector>();
		if (detector->Initialize())
			detector_ = std::move(detector);

		return bool(detector_);

	}

};
	
///////////////////////////////////////////////////////////////////////////////

TLFAgent::TLFAgent() 
	: pool_([this]() { return std::make_unique<agent::TFeaturesBuilder>(); }) {
}

inline std::unique_ptr<TLFSemanticImageDescriptor> 
TLFAgent::Detect(std::shared_ptr<TLFImage> img) {

	auto scanner = detector_->GetScanner();
	
	if (!scanner)
		return nullptr;

	ILFSupervisor::TDetections	gt_detections;

	scanner->ScanImage(img.get());
// We can process objects without supervisor also
	if (supervisor_ && trainer_) {
		// Process image and get ground truth bounds
		gt_detections = supervisor_->Detect(img);
	}

	auto fragments_count = scanner->GetFragmentsCount();

	if (fragments_count == 0)
		return nullptr;


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

	detected_indices_.clear();
	

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
		features->Setup(detector_->GetMap(), batch_begin, batch_end, batch_size_);

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
					detected_indices_.push_back(n);
				}
			}
		}

	}

	//TODO: run NMS;

	if (supervisor_ && trainer_) {
		auto correctors = trainer_->Train();
		correctors_.AddCorrectors(std::move(correctors));
	}

	return std::make_unique<TLFSemanticImageDescriptor>();
}

std::shared_ptr<TLFAgent> CreateAgent() {
	auto agent = std::make_shared<TRandomAgent>();
	if (agent->Initialize())
		return agent;

	return nullptr;
}

std::shared_ptr<ILFSupervisor> CreateSupervisor() {
	return std::make_shared<TRandomSupervisor>();
}
