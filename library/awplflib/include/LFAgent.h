#pragma once

#include "LFImage.h"
#include "LF.h"

#include <numeric>
#include <memory>
#include <mutex>


class TLFDetections {
public:
	TLFDetections(std::vector<awpRect>& rects) {
		rects_ = std::move(rects);
	}
	const std::vector<awpRect>& GetDetections() const {
		return rects_;
	}
private:
	std::vector<awpRect> rects_;
};

class ILFSupervisor {
public:
	virtual std::shared_ptr<TLFDetections> Detect(std::shared_ptr<TLFImage> img) = 0;
};

class TLFAgent {
public:
	virtual void SetSupervisor(std::shared_ptr<ILFSupervisor> sv) {
		supervisor_ = sv;
	}

	virtual std::unique_ptr<TLFSemanticImageDescriptor> Detect(std::shared_ptr<TLFImage> img);

	virtual bool LoadXML(TiXmlElement* parent) {
		// TODO: load detector and all correctors
		return false;
	}

	virtual TiXmlElement* SaveXML() {
		// TODO: save detector and all correctors
		return nullptr;
	}

private:
	ILFScanner*								scanner_ = nullptr;
	std::shared_ptr<ILFSupervisor>			supervisor_;
	std::unique_ptr<agent::IDetector>		detector_;

};



namespace agent {

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
			assert(fragment_index < frags_end_ && fragment_index >= frags_begin_);
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
		}

		std::vector<size_t>& GetTriggered() { return triggered_; }

		//Get memory to fill it;
		float* GetMutation() {

			return data_.get();
		}

	};

	class ICorrector
	{
	public:
		virtual bool Correct(const TFeatures& list) = 0;
		virtual bool LoadXML(TiXmlElement* parent) = 0;

		virtual TiXmlElement* SaveXML() = 0;
	};

	class ITrainerCorrectors
	{
	public:
		virtual void SetScanner(ILFScanner* scanner) = 0;
		virtual void AddSamples(std::shared_ptr<TFeatures>) = 0;
	};


	class IDetector {
	public:
		virtual ILFScanner* GetScanner() = 0;
		virtual void Detect(std::shared_ptr<TLFImage> img, TFeaturesBuilder& builder) = 0;
		//virtual void SetCallback(std::function<void(std::shared_ptr<TFeatures>)> callback) = 0;
		virtual bool LoadXML(TiXmlElement* parent) = 0;

		virtual TiXmlElement* SaveXML() = 0;
	};


	


}

