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
		TRandomSupervisor();
		virtual std::shared_ptr<TLFDetections> Detect(std::shared_ptr<TLFImage> img) override;
	};


	class TLFFragments
	{
	public:
		TLFFragments(std::vector<awpRect>& rects, ILFScanner* scanner);
		const std::vector<awpRect>& GetFragments() const;
		size_t GetNearestIndex(const awpRect& rect);
	private:
		std::vector<awpRect>			rects_;
		ILFScanner* scanner_ = nullptr;
	};

	class TLFFragmentBuilder
	{
	public:
		TLFFragmentBuilder(ILFScanner* scanner);
		std::shared_ptr<TLFFragments> Build(std::shared_ptr<TLFImage> img);
	private:
		ILFScanner* scanner_ = nullptr;
		std::shared_ptr<TLFFragments> fragments_ = nullptr;
		int height_ = 0;
		int width_ = 0;
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

	class IDetector
	{
	public:
		virtual void Detect(std::shared_ptr<TLFImage> img, std::shared_ptr<TLFFragments> fragments) = 0;
		virtual void SetCallback(std::function<void(std::shared_ptr<TFeatures>)> callback) = 0;
	};

	class TRandomDetector : public IDetector
	{
	public:
		TRandomDetector();
		void Init();
		void Detect(std::shared_ptr<TLFImage> img, std::shared_ptr<TLFFragments> fragments) override;
		void SetCallback(std::function<void(std::shared_ptr<TFeatures>)> callback) override;

	private:
		std::function<void(std::shared_ptr<TFeatures>)> callback_;
		//std::shared_ptr<TLFDiscriptionFeatures> discription_features_;
		std::vector<size_t> feature_count_list_;
		TFeatures::TMapPtr tmap_ptr_;
	};

	class TRandomAgent : public IAgent
	{
	public:
		TRandomAgent();

	public:
		virtual void				SetSupervisor(ILFSupervisor* supervisor) override;
		virtual std::unique_ptr<TLFSemanticImageDescriptor> Detect(std::shared_ptr<TLFImage> img) override;
		virtual bool				LoadXML(const char* lpFileName) override;
		virtual bool				SaveXML(const char* lpFileName) override;

	public:
		bool				LoadXML(TiXmlElement* parent);

		TiXmlElement* SaveXML();
	private:
		std::unique_ptr<TLFFragmentBuilder> fragment_builder_;
		ILFScanner* scanner_;
		ILFSupervisor* supervisor_;
		std::unique_ptr<TRandomDetector> detector_;
	};

	class TLFResult {
	public:
		//TODO
		bool result;
	};

	class TFeaturesBuilder : public TFeatures {
	public:
		TFeaturesBuilder(TMapPtr map, size_t batch_size) : TFeatures(map, batch_size) {}

	public:
		// Set fragments and 
		void Reset(size_t frags_begin, size_t frags_end) {
			frags_begin_ = frags_begin; // 1000   1001 GetValue(1000, 0);
			frags_end_ = frags_end; // 2000
			data_[0] = frags_begin_;
			triggered_.assign(triggered_.size(), -1);
		}

		std::vector<size_t>& GetTriggered() { return triggered_; }

		//Get memory to fill it;
		float* GetMutation() { return data_.get(); }
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

	class TCorrector
	{
	public:
		TCorrector();
		TLFResult Correct(const TFeatures& list);
		void LoadXML(TiXmlElement* parent);
		TiXmlElement* SaveXML();
	private:

	};

	class TLFTrainerCorrectors
	{
	public:
		TLFTrainerCorrectors();
		//void addSamples(TLFResult/*ot detectora*/, return supervisor->detect(TLFImage * img), vector<TLFFeature>(size = nskolko), size_t(ot kuda), size_t nskolko)
		//void addSamples(TLFResult result, std::shared_ptr<TLFDetections> detections, TLFDiscriptionFeatures features);
	};

	TLFFragments::TLFFragments(std::vector<awpRect>& rects, ILFScanner* scanner) {
		rects_ = std::move(rects);
		scanner_ = scanner;
	}
	const std::vector<awpRect>& TLFFragments::GetFragments() const {
		return rects_;
	}

	size_t TLFFragments::GetNearestIndex(const awpRect& rect) {
		// naidi mne blizhaishii awpRect
		// TODO from scanner
		return 0;
	}

	TLFFragmentBuilder::TLFFragmentBuilder(ILFScanner* scanner) {
		scanner_ = scanner;
	}

	std::shared_ptr<TLFFragments> TLFFragmentBuilder::Build(std::shared_ptr<TLFImage> img) {
		// ??? std::shared_ptr<vector<awprect>>
		if (height_ == img->height() && width_ == img->width()) { // razmer img ne pomenyalsya to
			return fragments_;
		}
		else { // skaniruem		
			scanner_->ScanImage(img.get());

			height_ = img->height();
			width_ = img->width();

			const auto& bounds = scanner_->GetFragments();
			std::vector<awpRect> rects;
			rects.reserve(bounds.size());
			for (int i = 0; i < bounds.size(); i++)
				rects.push_back(bounds[i].Rect);
			fragments_ = std::make_shared<TLFFragments>(rects, scanner_);
			return fragments_;
		}
	}

	TRandomAgent::TRandomAgent() {
	}

	void DetectorCallback(std::shared_ptr<TFeatures> features) {
		std::thread::id this_id = std::this_thread::get_id();
		//std::cout << "thread " << this_id << "\nfrags_begin=" << frags_begin << "\nfrags_end=" << frags_end << "\nfeatures=" << features->GetValue(frags_begin, 0) << std::endl;
		//TODO
	}

	std::unique_ptr<TLFSemanticImageDescriptor> TRandomAgent::Detect(std::shared_ptr<TLFImage> img) {
		std::shared_ptr<TLFFragments> fragments = fragment_builder_->Build(img);
		detector_->SetCallback(DetectorCallback);
		detector_->Detect(img, fragments);
		//todo call supervisor_
		//todo call trainer
		//todo nms algoritm return ILFDescriptor
		std::unique_ptr<TLFSemanticImageDescriptor> ret = std::make_unique<TLFSemanticImageDescriptor>();
		return ret;
	}

	void TRandomAgent::SetSupervisor(ILFSupervisor* supervisor) {
		supervisor_ = supervisor;
	}

	bool TRandomAgent::LoadXML(const char* lpFileName) {
		//TODO
		detector_ = std::make_unique<TRandomDetector>();
		scanner_ = new TLFScanner();
		fragment_builder_ = std::make_unique <TLFFragmentBuilder>(scanner_);
		return true;
	}

	bool TRandomAgent::LoadXML(TiXmlElement* parent)
	{
		if (parent == NULL)
			return false;
	}

	bool TRandomAgent::SaveXML(const char* lpFileName) {
		//TODO
		return true;
	}

	TiXmlElement* TRandomAgent::SaveXML()
	{
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

	TRandomSupervisor::TRandomSupervisor() {}

	std::shared_ptr<TLFDetections> TRandomSupervisor::Detect(std::shared_ptr<TLFImage> img) {
		//TODO implement python call
		std::vector<awpRect> rects;
		std::shared_ptr<TLFDetections> detections = std::make_shared<TLFDetections>(rects);
		return detections;
	}
	/*
	TLFFeature::TLFFeature(float* data, std::shared_ptr<TLFDiscriptionFeatures> batch_discription_features, size_t triggered_cascade) :
		batch_discription_features_(batch_discription_features),
		data_(data),
		triggered_cascade_(triggered_cascade)
	{}

	float* TLFFeature::GetData(size_t fragment_index, size_t feature_index) {
		return (data_ + feature_index * triggered_cascade_ + fragment_index);
	}


	TLFDiscriptionFeatures::TLFDiscriptionFeatures(std::vector<size_t>& feature_count_list) :
		feature_count_list_(std::move(feature_count_list))
	{}

	size_t TLFDiscriptionFeatures::GetCountFeature(size_t cascade_index) const {
		if (feature_count_list_.size() <= cascade_index)
			return 0;
		return feature_count_list_[cascade_index];
	}

	size_t TLFDiscriptionFeatures::GetCascadeCount() const {
		return feature_count_list_.size();
	}
	*/
	TRandomDetector::TRandomDetector() {
		Init();
	}

	void TRandomDetector::Init() {
		// karta detectora odin raz
		size_t cascade_count = 3;  // rand ot 3 do 10
		//const size_t sizeof_one_feature = 100;
		for (auto i = 0; i < cascade_count; i++) {
			size_t feature_count = 10; // rand ot 10 do 100
			feature_count_list_.push_back(feature_count);
		}
		tmap_ptr_ = std::make_shared<TFeatures::TMap>(feature_count_list_);
		// cascade_count = 3: 10 + 19 + 21 = 50 features 
		// cascade_count = 4: 10 + 19 + 21 + 10 = 60 features
		//discription_features_ = std::make_shared<TLFDiscriptionFeatures>(feature_count_list);
	}

	void TRandomDetector::Detect(std::shared_ptr<TLFImage> img, std::shared_ptr<TLFFragments> fragments) {
		TLFResult result;
		const auto& frags = fragments->GetFragments();
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

			std::vector<TLFResult> results;
			results.reserve(batch_fragments);
			auto features_builder = features_pool.Alloc(batch_fragments);
			features_builder->Reset(frags_begin, frags_end);

			callback_(features_builder);
		}
	}

	void TRandomDetector::SetCallback(std::function<void(std::shared_ptr<TFeatures> features)> callback) {
		callback_ = callback;
	}

	TCorrector::TCorrector() {

	}

	TLFResult TCorrector::Correct(const TFeatures& list) {
		TLFResult result;
		return result;
	}

	void TCorrector::LoadXML(TiXmlElement* parent) {

	}

	TiXmlElement* TCorrector::SaveXML() {
		return nullptr;
	}

	TLFTrainerCorrectors::TLFTrainerCorrectors() {

	}
	/*
	void TLFTrainerCorrectors::addSamples(TLFResult result, std::shared_ptr<TLFDetections> detections, TLFDiscriptionFeatures features) {

	}
	*/

}

std::shared_ptr<IAgent> CreateAgent() {
	return std::make_shared<Agent::TRandomAgent>();
}

