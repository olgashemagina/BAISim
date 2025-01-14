#include "LFAgent.h"
#include "LFScanners.h"

#include <random>
#include <syncstream>

#ifdef _OMP_
#include <omp.h>
#endif



namespace Agent
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

	class TFeatures {
	public:
		using TMap = std::vector<size_t>;
		using TMapPtr = std::shared_ptr<TMap>;

	public:
		TFeatures(TMapPtr map, size_t batch_size) : map_(map), batch_size_(batch_size) {
			stride_ = std::accumulate(map_->begin(), map_->end(), 0);
			data_ = std::make_unique<float[]>(stride_ * batch_size_);
			triggered_.resize(batch_size_, -1);
		}

		virtual ~TFeatures() {}

	public:
		float GetValue(size_t fragment_index, size_t feature_index) const {
			assert(fragment_index < frags_end_&& fragment_index >= frags_begin_);
			assert(feature_index < stride_);

			size_t index = (fragment_index - frags_begin_) * stride_ + feature_index;
			return data_[index];
		}

		bool GetResult(size_t fragment_index) const { return (triggered_[fragment_index] < -1); }

		size_t GetFragsBegin() { return frags_begin_; }
		size_t GetFragsEnd() { return frags_end_; }

	protected:
		//Block of data
		std::unique_ptr<float[]>			data_;
		TMapPtr								map_;
		//Size of row of features of one fragment;
		size_t								stride_ = 0;
		// Size of fragments;
		size_t								batch_size_ = 0;
		size_t								frags_begin_ = 0;
		size_t								frags_end_ = 0;
		//Number triggered stage or -1 if noone triggered;

		std::vector<size_t>					triggered_;
	};

	class TFeaturesBuilder : public TFeatures {
	public:
		TFeaturesBuilder(TMapPtr map, size_t batch_size) : TFeatures(map, batch_size) {}

	public:
		// Set fragments and 
		void Reset(size_t frags_begin, size_t frags_end) {
			frags_begin_ = frags_begin;
			frags_end_ = frags_end;
			triggered_.assign(triggered_.size(), -1);
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_int_distribution<> distr(0, 1);
			std::mt19937 gen2(rd());
			std::uniform_int_distribution<> distr2(0, map_->size() - 1);
			std::mt19937 gen3(rd());
			std::uniform_real_distribution<float> distr_float(1, 2);
			for (auto i = 0; i < triggered_.size(); i++) {
				if (distr(gen)) {
					size_t cascade_number = distr2(gen2);
					triggered_[i] = cascade_number;
					size_t count = 0;
					for (auto j = 0; j < map_->size(); j++) {
						for (auto k = 0; k < map_.get()->at(j); k++) {
							float rand_value = 0;
							if (j <= cascade_number) rand_value = distr_float(gen3);
							data_[i * stride_ + count] = rand_value;
							count++;
						}
					}
				}
				else { // fill all cascade
					for (auto j = 0; j < stride_; j++) {
						data_[i * stride_ + j] = distr_float(gen3);
					}
				}
			}
			assert(CheckData());
		}

		std::vector<size_t>& GetTriggered() { return triggered_; }

		//Get memory to fill it;
		float* GetMutation() { return data_.get(); }

	private:
		bool CheckData() {
			for (auto i = 0; i < triggered_.size(); i++) {
				if (triggered_[i] != -1 && triggered_[i] >= map_->size())
					return false;
				size_t cascade_number = triggered_[i];
				if (cascade_number == -1) {
					cascade_number = map_->size();
				}
				size_t count = 0;
				for (auto j = 0; j < map_->size(); j++) {
					if (j <= cascade_number) {
						for (auto k = 0; k < map_.get()->at(j); k++) {
							if (data_[i * stride_ + count] == 0)
								return false;
							count++;
						}
					}
					else {
						for (auto k = 0; k < map_.get()->at(j); k++) {
							if (data_[i * stride_ + count] != 0)
								return false;
							count++;
						}
					}
				}
			}
			return true;
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

	class IDetector
	{
	public:
		virtual void Detect(std::shared_ptr<TLFImage> img, std::shared_ptr<ILFScanner> scanner) = 0;
		virtual void SetCallback(std::function<void(std::shared_ptr<TFeatures>)> callback) = 0;
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

	class ICorrector
	{
	public:
		virtual bool Correct(const TFeatures& list) = 0;
	};
	
	class TCorrector : public ICorrector
	{
	public:
		TCorrector() {}

		virtual bool Correct(const TFeatures& list) override {
			bool result;
			return result;
		}

		void LoadXML(TiXmlElement* parent) {

		}

		TiXmlElement* SaveXML() {
			return nullptr;
		}
	};

	class ITrainerCorrectors
	{
	public:
		virtual void addSamples(std::shared_ptr<TFeatures>) = 0;
	};

	class TTrainerCorrectors : public ITrainerCorrectors
	{
	public:
		TTrainerCorrectors() {}
		virtual void addSamples(std::shared_ptr<TFeatures>) override {

		}
		//void addSamples(TLFResult/*ot detectora*/, return supervisor->detect(TLFImage * img), vector<TLFFeature>(size = nskolko), size_t(ot kuda), size_t nskolko)
		//void addSamples(TLFResult result, std::shared_ptr<TLFDetections> detections, TLFDiscriptionFeatures features);
	};
}

std::shared_ptr<IAgent> CreateAgent() {
	return std::make_shared<Agent::TRandomAgent>();
}

