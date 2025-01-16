#include "LFAgent.h"
#include "LFScanners.h"

#include <random>
#include <syncstream>

#ifdef _OMP_
#include <omp.h>
#endif



namespace agent
{
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

	

	class TFeaturesPool {
	public:
		TFeaturesPool(TFeatures::TMapPtr map) : map_(map) {}

		std::shared_ptr<TFeaturesBuilder> Alloc(size_t batch_size) {
			std::lock_guard lg(mutex_);
			for (const auto& fb : all_) {
				if (fb.unique()) {
					return fb;
				}
			}
			auto fb = std::make_shared<TFeaturesBuilder>(map_, batch_size);
			all_.emplace_back(fb);
			return fb;
		}

	private:
		std::vector<std::shared_ptr<TFeaturesBuilder>>			all_;
		TFeatures::TMapPtr						map_;
		std::mutex mutex_;
	};

	

	class TRandomDetector : public IDetector
	{
	public:
		TRandomDetector() {
			Init();
		}

		void Init() {
			// karta detectora odin raz
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_int_distribution<> distr(3, 10);
			size_t cascade_count = distr(gen);  // rand ot 3 do 10
			//const size_t sizeof_one_feature = 100;
			for (auto i = 0; i < cascade_count; i++) {
				std::mt19937 gen2(rd());
				std::uniform_int_distribution<> distr2(10, 100);
				size_t feature_count = distr2(gen2); // rand ot 10 do 100
				feature_count_list_.push_back(feature_count);
			}
			tmap_ptr_ = std::make_shared<TFeatures::TMap>(feature_count_list_);
			// cascade_count = 3: 10 + 19 + 21 = 50 features 
			// cascade_count = 4: 10 + 19 + 21 + 10 = 60 features
			//discription_features_ = std::make_shared<TLFDiscriptionFeatures>(feature_count_list);
		}

		void Detect(std::shared_ptr<TLFImage> img, std::shared_ptr<ILFScanner> scanner) override {
			scanner->ScanImage(img.get());
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
		}

		void SetCallback(std::function<void(std::shared_ptr<TFeatures> features)> callback) override {
			callback_ = callback;
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

	
	
}

std::shared_ptr<IAgent> CreateAgent() {
	return std::make_shared<Agent::TRandomAgent>();
}

inline std::unique_ptr<TLFSemanticImageDescriptor> TLFAgent::Detect(std::shared_ptr<TLFImage> img) {
	// TODO: 1) Run detector in parallel


}
