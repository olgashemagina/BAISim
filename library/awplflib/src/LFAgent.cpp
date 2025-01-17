#include "LFAgent.h"
#include "LFScanners.h"

#include <random>
#include <syncstream>

#ifdef _OMP_
#include <omp.h>
#endif

using namespace agent;


class TRandomSupervisor : public ILFSupervisor
{
public:
	TRandomSupervisor() {}
	virtual std::shared_ptr<TLFDetections> Detect(std::shared_ptr<TLFImage> img) override {
		//TODO implement python call
		std::vector<awpRect> rects;
		std::shared_ptr<TLFDetections> detections = std::make_shared<TLFDetections>(rects);
		return detections;
	}
};

class TRandomTrainerCorrectors : public ITrainerCorrectors
{
public:
	virtual void SetScanner(ILFScanner* scanner) override {

	}
	virtual void AddSamples(std::shared_ptr<TFeatures> features) override {
		count_ += features->count();
		if (count_ >= max_count_) {
			ICorrector* corrector = nullptr;
			//todo send corrector
		}
	}
private:
	size_t count_ = 0;
	const size_t max_count_ = 10000;
};

	
class TRandomWorker : public IWorker {
public:
	TRandomWorker(std::shared_ptr<IDetector> detector) : detector_(detector) {}
	virtual void Detect(std::shared_ptr<TLFImage> img, TFeaturesBuilder& builder) override {
		mutex_.lock();
		detector_->Detect(img, builder);
		mutex_.unlock();
	}

private:
	std::mutex mutex_;
	std::shared_ptr<IDetector> detector_;
};
	

class TRandomDetector : public IDetector
{
public:
	TRandomDetector() {
		Init();
	}

	void Init() {
		size_t cascade_count = GetRand(3, 10);  // rand ot 3 do 10
		for (auto i = 0; i < cascade_count; i++) {
			size_t feature_count = GetRand(10, 100); // rand ot 10 do 100
			feature_count_list_.push_back(feature_count);
		}
		tmap_ptr_ = std::make_shared<TFeatures::TMap>(feature_count_list_);
	}

	virtual void Detect(std::shared_ptr<TLFImage> img, TFeaturesBuilder& builder) override {
		float* data = builder.GetMutableData();
		size_t stride = builder.GetStride();
		auto triggered = builder.GetTriggered();
		auto map = builder.GetMap();

		for (auto i = 0; i < triggered.size(); i++) {
			if (GetRand(0, 1)) {
				size_t cascade_number = GetRand(0, stride - 1);
				triggered[i] = cascade_number;
				size_t count = 0;
				for (auto j = 0; j < stride; j++) {
					if (j > cascade_number) break;					
					size_t cur_cascade_lenght = map.get()->at(j);
					SetRandData(data + (i * stride) + count, cur_cascade_lenght);
					count += cur_cascade_lenght;
				}
			}
			else { // fill all cascade
				SetRandData(data + (i * stride), stride);
			}
		}
	}
	/*	scanner->ScanImage(img.get());
		const auto& frags = scanner->GetFragments();
		int count = 0;
		//size_t count_all_feature = 0;
		const size_t batch_fragments = 1000;

		if (frags.size() > batch_fragments)
			count = frags.size() / batch_fragments;
		size_t remainder_fragments = frags.size() - count * batch_fragments;

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> distr(0, 1);

		TFeaturesPool features_pool(tmap_ptr_);

#ifdef _OMP_
#pragma omp parallel for num_threads(omp_get_max_threads())
#endif
		for (auto i = 0; i <= count; i++) {
			size_t batch_fragments_for_callback = batch_fragments;
			const size_t frags_begin = i * batch_fragments;
			if (i == count) {
				if (remainder_fragments == 0) break;
				batch_fragments_for_callback = remainder_fragments;

			}
			const size_t frags_end = frags_begin + batch_fragments_for_callback;
			auto features_builder = features_pool.Alloc(batch_fragments);
			features_builder->Reset(frags_begin, frags_end);
			features_builder->GetMutation();
			callback_(features_builder);
		}
	}*/

	void SetCallback(std::function<void(std::shared_ptr<TFeatures> features)> callback) override {
		callback_ = callback;
	}

private:
	void SetRandData(float* data, size_t size) {
		for (size_t i = 0; i < size; i++)
			data[i] = GetRand(10, 100);
	}
	size_t GetRand(size_t low_dist, size_t high_dist) {
		static bool init_srand = false;
		if (!init_srand) {
			std::srand((unsigned int)std::time(nullptr));
			init_srand = true;
		}
		return low_dist + std::rand() % (high_dist + 1 - low_dist);
	}

private:
	std::function<void(std::shared_ptr<TFeatures>)> callback_;
	std::vector<size_t> feature_count_list_;
	TFeatures::TMapPtr tmap_ptr_;

};

void DetectorCallback(std::shared_ptr<TFeatures> features) {
	std::thread::id this_id = std::this_thread::get_id();
	std::cout << "thread " << this_id << "\nfrags_begin=" << features->GetFragsBegin() << "\nfrags_end=" << features->GetFragsEnd() << "\nfeatures=" << features->GetValue(features->GetFragsBegin(), 0) << std::endl;
	//TODO
}

class TRandomAgent : public IAgent
{
public:
	TRandomAgent() {}

public:
	virtual void				SetSupervisor(std::shared_ptr<ILFSupervisor> supervisor) override {
		supervisor_ = supervisor;
	}
	virtual std::unique_ptr<TLFSemanticImageDescriptor> Detect(std::shared_ptr<TLFImage> img) override {			
		detector_->SetCallback(DetectorCallback);
		detector_->Detect(img, scanner_);
		//todo call supervisor_
		//todo call trainer
		//todo nms algoritm return ILFDescriptor
		std::unique_ptr<TLFSemanticImageDescriptor> ret = std::make_unique<TLFSemanticImageDescriptor>();
		return ret;
	}
	virtual bool				LoadXML(const char* lpFileName) override {
		//TODO
		detector_ = std::make_unique<TRandomDetector>();
		scanner_ = std::make_shared<TLFScanner>();
		return true;
	}
	virtual bool				SaveXML(const char* lpFileName) override {
		//TODO
		return true;
	}

public:
	bool				LoadXML(TiXmlElement* parent) {
		if (parent == NULL)
			return false;
	}

	TiXmlElement* SaveXML() {
		TiXmlElement* node1 = NULL;//new TiXmlElement(this->GetName());
		if (node1 == NULL)
			return NULL;

		//node1->SetAttribute("detector", this->m_strDetName.c_str());
		if (scanner_ != NULL)
		{
			TiXmlElement* e = scanner_->SaveXML();
			//node1->LinkEndChild(e);
		}
		else
		{
			//delete node1;
			return NULL;
		}

		return node1;
	}
private:
	std::shared_ptr<ILFScanner> scanner_;
	std::shared_ptr<ILFSupervisor> supervisor_;
	std::unique_ptr<TRandomDetector> detector_;
};


	

inline std::unique_ptr<TLFSemanticImageDescriptor> TLFAgent::Detect(std::shared_ptr<TLFImage> img) {

	auto scanner = detector_->GetScanner();

	auto fragments_count = scanner->GetFragmentsCount();

	size_t batches_count = std::ceil(fragments_count / batch_size_);

	// TODO: 1) Run detector in parallel
	// 2) Run correctors

	
	size_t threads = 1;

#ifdef _OMP_
	threads = std::max<size_t>(omp_get_max_threads(), max_threads_);
#endif 

	if (workers_.size() != threads) {
		workers_.resize(threads);
		for (auto& worker : workers_) {
			worker = std::move(detector_->CreateWorker());
		}
	}


#ifdef _OMP_
#pragma omp parallel for num_threads(threads)
#endif
	for (auto batch = 0; batch <= batches_count; batch++) {

		int current_thread = 0;

#ifdef _OMP_
		current_thread = omp_get_thread_num();
#endif 
		auto features = pool_.GetObject();


		workers_[current_thread]->Detect(img, *features);

		//Apply correctors
		
	}





}

