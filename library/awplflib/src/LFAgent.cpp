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
	virtual TDetections Detect(std::shared_ptr<TLFImage> img) override {
		//TODO implement python call
		return TDetections();
	}
};

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
};

	
class TRandomWorker : public IWorker {
public:
	//TRandomWorker(std::shared_ptr<IDetector> detector) : detector_(detector) {}
	void SetDetector(std::shared_ptr<IDetector> detector) { detector_ = detector; }
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

	virtual ILFScanner* GetScanner()  override {
		return nullptr;
	}
	virtual std::unique_ptr<IWorker> CreateWorker()  override {
		return std::make_unique<TRandomWorker>();
	}

	virtual bool LoadXML(TiXmlElement* parent)  override {
		return false;
	}
	virtual TiXmlElement* SaveXML()  override {
		return nullptr;
	}

	virtual TFeatures::TMapPtr GetMap() override { return tmap_ptr_; }

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
	/*
	void SetCallback(std::function<void(std::shared_ptr<TFeatures> features)> callback) override {
		callback_ = callback;
	}*/

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
	//std::cout << "thread " << this_id << "\nfrags_begin=" << features->GetFragsBegin() << "\nfrags_end=" << features->GetFragsEnd() << "\nfeatures=" << features->GetValue(features->GetFragsBegin(), 0) << std::endl;
	//TODO
}

class TRandomAgent : public TLFAgent
{
public:
	TRandomAgent() : TLFAgent(std::make_unique<TRandomDetector>()) {
	}
};
	

inline std::unique_ptr<TLFSemanticImageDescriptor> TLFAgent::Detect(std::shared_ptr<TLFImage> img) {

	auto scanner = detector_->GetScanner();
	scanner->ScanImage(img.get());
// We can process objects without supervisor also
	if (supervisor_ && trainer_) {
		// Process image and get ground truth bounds
		auto supervised = supervisor_->Detect(img);

		// Prepare corrector trainer for building correctors
		trainer_->BeginImage(scanner, supervised);
	}

	auto fragments_count = scanner->GetFragmentsCount();

	size_t batches_count = std::ceil(fragments_count / batch_size_);
			
	size_t threads = 1;

#ifdef _OMP_
	threads = std::max<size_t>(omp_get_max_threads(), max_threads_);
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
		features->Setup(batch_begin, batch_end, batch_size_);

		// Process batch by detector
		workers_[current_thread]->Detect(img, *features.get());

		// Apply corrections and calc corrections vector
		//todo sele
		//correctors_->Apply(*features.get(), features->GetMutableCorrections());

		// No new correctors without supervisor
		if (supervisor_ && trainer_) {
			// Process features and check if we can train new corrector(s)
			auto new_correctors = trainer_->ProcessSamples(features);
			if (!new_correctors.empty()) {
				//todo sele
				//correctors_->AddCorrectors(new_correctors);
			}
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

	if (supervisor_ && trainer_)
		trainer_->EndImage();

	
}

std::shared_ptr<TLFAgent> agent::CreateAgent() {
	return std::make_shared<TRandomAgent>();
}
