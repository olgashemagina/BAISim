#ifndef __lf_builder_h__
#define __lf_builder_h__

#include "LFImage.h"
#include "LF.h"

#include <numeric>
#include <memory>
#include <mutex>

class TLFFragments
{
public:
	TLFFragments(std::vector<awpRect>& rects, ILFScanner* scanner);
	const std::vector<awpRect>& GetFragments() const;
	size_t GetNearestIndex(const awpRect& rect);
private:
	std::vector<awpRect>			rects_;
	ILFScanner*						scanner_ = nullptr;
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

class TLFDetections {
public:
	TLFDetections(std::vector<awpRect>& rects);
	const std::vector<awpRect>& GetDetections() const;
private:
	std::vector<awpRect> rects_;
};

class TLFSupervisor
{
public:
	TLFSupervisor();
	std::shared_ptr<TLFDetections> Detect(std::shared_ptr<TLFImage> img);
};

class TLFAgent
{
public:
	TLFAgent();

public:
	void				SetSupervisor(TLFSupervisor* supervisor);
	std::unique_ptr<TLFSemanticImageDescriptor> Detect(std::shared_ptr<TLFImage> img);

public:
	bool				LoadXML(const char* lpFileName);
	bool				SaveXML(const char* lpFileName);

public:
	bool				LoadXML(TiXmlElement* parent);
	
	TiXmlElement*		SaveXML();
private:
	std::unique_ptr<TLFFragmentBuilder> fragment_builder_;
	ILFScanner* scanner_;
	TLFSupervisor* supervisor_;
};


class TLFResult {
public:
	//TODO
	bool result;
};



class TFeatures {
public:
	using TMap = std::vector<size_t>;
	using TMapPtr = std::shared_ptr<TMap>;

public:
	TFeatures(TMapPtr map, size_t batch_size) : map_(map), batch_size_(batch_size) {
		stride_ = std::accumulate(map_->begin(), map_->end(), 0);
		data_ = std::make_unique<float[]>(stride_*batch_size_);
		triggered_.resize(batch_size_, -1);
	}

	virtual ~TFeatures() {}

public:
	float GetValue(size_t fragment_index, size_t feature_index) const {
		assert(fragment_index < frags_end_ && fragment_index >= frags_begin_);
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


class TFeaturesBuilder : public TFeatures {
public:
	TFeaturesBuilder(TMapPtr map, size_t batch_size) : TFeatures(map, batch_size) {}

public:
	// Set fragments and 
	void Reset(size_t frags_begin, size_t frags_end) {		
		frags_begin_ = frags_begin; // 1000   1001 GetValue(1000, 0);
		frags_end_ = frags_end; // 2000
		triggered_.assign(triggered_.size(), -1);
	}

	std::vector<size_t>& GetTriggered() { return triggered_; }

	//Get memory to fill it;
	float*		GetMutation() { return data_.get(); }
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

/*
class TFeaturesPool : public std::enable_shared_from_this<TFeaturesPool> {
public:
	TFeaturesPool(TFeatures::TMapPtr map, size_t batch_size) : map_(map), batch_size_(batch_size) {}

	std::shared_ptr<TFeaturesBuilder>		Alloc() {
		//TODO: add mutex

		if (free_.empty()) {
					
			free_.emplace_back(std::make_unique<TFeaturesBuilder>(map_, batch_size_));
		}
		
		//Move Object to shared_ptr;
		auto fb = std::shared_ptr<TFeaturesBuilder>(free_.back().release(), 
			// Hold Pool and 
			[=, pool = this->shared_from_this()](TFeaturesBuilder* ptr) {
			free_.emplace_back(ptr);
		});

		free_.pop_back();

		return fb;
	}


private:
	
	std::vector<std::unique_ptr<TFeaturesBuilder>>			free_;
	TFeatures::TMapPtr						map_;
	size_t			batch_size_;
};
*/



/*
class TLFFeature {
public:
	TLFFeature(float* data, const TLFDiscriptionFeatures&, const std::vector<size_t>& triggered_cascade);
	float* GetData(size_t fragment_index, size_t feature_index);
private:
	float* data_;
	const TLFDiscriptionFeatures& batch_discription_features_;
	std::vector<size_t> triggered_cascade_; //kolichestvo srabotavshih kaskadov = kolichestvo features(1000)
};
*/


class TLFDetector
{
public:
	TLFDetector();
	void Init();
	void Detect(std::shared_ptr<TLFImage> img, std::shared_ptr<TLFFragments> fragments);
	void SetCallback(std::function<void(const std::vector<TLFResult>&, std::shared_ptr<TFeatures>, size_t, size_t)> callback);

private:
	std::function<void(const std::vector<TLFResult>&, std::shared_ptr<TFeatures>, size_t, size_t)> callback_;
	//std::shared_ptr<TLFDiscriptionFeatures> discription_features_;
	std::vector<size_t> feature_count_list_;
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

#endif //__lf_builder_h__