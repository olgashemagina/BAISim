#include "tests.h"
#include "agent.h"

using namespace agent;

static size_t GetRand(size_t low_dist, size_t high_dist) {
	return low_dist + std::rand() % (high_dist + 1 - low_dist);
}


bool agent::TRandomDetector::Initialize() {


	size_t cascade_count = GetRand(3, 10);  // rand ot 3 do 10
	size_t count = 0;
	for (auto i = 0; i < cascade_count; i++) {
		size_t feature_count = GetRand(10, 100); // rand ot 10 do 100
		feature_count_list_.push_back(feature_count + count);
		count = feature_count_list_.back();
	}

	return true;
}

void agent::TRandomDetector::Detect(std::shared_ptr<TLFImage> img, TFeaturesBuilder& builder) {

	builder.Reset(feature_count_list_);

	float* data = builder.GetMutableData().GetRow(0);
	size_t stride = builder.feats_count();

	size_t detected = 0;

	for (size_t frag = builder.frags_begin(); frag < builder.frags_end(); ++frag) {
		auto row_data = builder.GetFeats(frag);
		// With probability 0.99 it is negative result
		if (GetRand(0, 100) > 1) {
			size_t cascade_number = GetRand(0, feature_count_list_.size() - 1);

			builder.SetTriggered(frag, cascade_number, 1);

			SetRandData(row_data, feature_count_list_.at(cascade_number));
		}
		else { // fill all cascade
			SetRandData(row_data, stride);
			detected++;
		}
	}

	std::thread::id this_id = std::this_thread::get_id();

	std::stringstream ss;

	ss << "Detection thread " << this_id
		<< " frags_begin=" << builder.frags_begin()
		<< " frags_end=" << builder.frags_end()
		<< " detected=" << detected << std::endl;

	//(std::cout << ss.str()).flush();

}

std::unique_ptr<IWorker> TRandomDetector::CreateWorker() {
	return std::make_unique<TRandomWorker>(this);
}

bool agent::TRandomDetector::LoadXML(TiXmlElement* node) {

	auto features = node->GetText();
	if (features) {
		std::istringstream ss(features);
		size_t value = 0;
		size_t count = 0;
		while (ss >> value) {
			count += value;
			feature_count_list_.push_back(count);
		}
	}
	return !feature_count_list_.empty();

}

TiXmlElement* agent::TRandomDetector::SaveXML() {
	TiXmlElement* node = new TiXmlElement("TRandomDetector");

	std::stringstream ss;

	size_t prev = 0;

	for (size_t i = 0; i < feature_count_list_.size() - 1; ++i) {
		ss << feature_count_list_[i] - prev << ' ';
		prev = feature_count_list_[i];
	}
	ss << feature_count_list_.back() - prev;


	node->LinkEndChild(new TiXmlText(ss.str()));

	return node;
}

void agent::TRandomDetector::SetRandData(float* data, size_t size) {
	for (size_t i = 0; i < size; i++)
		data[i] = float(GetRand(1, 100));
}

 

 TDetections agent::TRandomSupervisor::Detect(std::shared_ptr<TLFImage> img) {

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

 void agent::TRandomWorker::Detect(std::shared_ptr<TLFImage> img, TFeaturesBuilder& builder) {
	 assert(detector_);
	 detector_->Detect(img, builder);
 }

 namespace tests {
	 std::unique_ptr<TLFAgent> build_random_agent() {
		 auto detector = std::make_unique<TRandomDetector>();

		 detector->Initialize();

		 auto trainer = std::make_unique<agent::TCorrectorTrainerBase>();

		 auto sv = std::make_shared<TRandomSupervisor>();

		 std::unique_ptr<TLFAgent>  agent = std::make_unique<TLFAgent>();
		 agent->Initialize(std::move(detector), std::move(trainer));
		 agent->SetSupervisor(sv);
		 return std::move(agent);
	 }
 }
