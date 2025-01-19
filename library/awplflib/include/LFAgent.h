#pragma once

#include "LFImage.h"
#include "LF.h"

#include <numeric>
#include <memory>
#include <mutex>
#include <shared_mutex>

//#include "../utils/tasks.h"
#include "../utils/object_pool.h"



namespace agent {

	static const int kNoCorrection = -1;

	class TFeatures {
	public:
		using TMap = std::vector<size_t>;
		using TMapPtr = std::shared_ptr<TMap>;

	public:
		TFeatures(TMapPtr map) : map_(map), batch_size_(0) {
			stride_ = std::accumulate(map_->begin(), map_->end(), 0);
			
		}

		virtual ~TFeatures() {}

	public:
		float GetValue(size_t fragment_index, size_t feature_index) const {
			assert(fragment_index < frags_end_ && fragment_index >= frags_begin_);
			assert(feature_index < stride_);

			size_t index = (fragment_index - frags_begin_) * stride_ + feature_index;
			return data_[index];
		}

		int GetDetectorResult(size_t fragment_index) const { return (triggered_[fragment_index - frags_begin_] < -1) ? 1 : 0; }

		int GetCorrectedResult(size_t fragment_index) const { 
			return (corrections_[fragment_index - frags_begin_] == kNoCorrection) ? 
				GetDetectorResult(fragment_index) : corrections_[fragment_index - frags_begin_];
		}

		size_t frags_begin() const { return frags_begin_; }
		size_t frags_end() const { return frags_end_; }

		size_t	count() const { return frags_end_ - frags_begin_; }

		const std::vector<int>& corrections() const { return corrections_; }

		TMapPtr GetMap() { return map_; }

		template<typename F>
		bool	CopyIf(F&& binary, float* mem, size_t size, size_t num_features = 0) {

			float* mem_end = mem + size;
			float* row = data_.get();

			num_features = num_features == 0 ? stride_ : num_features;

			for (size_t pos = 0; pos < count() && mem < mem_end; ++pos, row += stride_) {
				if (F(pos)) {
					std::memcpy(mem, row, num_features * sizeof(float));
					mem += stride_;
				}
			}
			return mem < mem_end;
		}

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

		std::vector<int>					corrections_;
	};

	class TFeaturesBuilder : public TFeatures {
	public:
		TFeaturesBuilder(TMapPtr map) : TFeatures(map) {}

	public:
		// Set fragments and 
		void Setup(size_t frags_begin, size_t frags_end, size_t batch_size) {
			frags_begin_ = frags_begin;
			frags_end_ = frags_end;
			
			if (batch_size_ != batch_size) {
				data_ = std::make_unique<float[]>(stride_ * batch_size);
				triggered_.resize(batch_size, -1);
				corrections_.resize(batch_size, kNoCorrection);
				batch_size_ = batch_size;
			}
			else {
				triggered_.assign(triggered_.size(), -1);
				corrections_.assign(corrections_.size(), kNoCorrection);
			}
		}

		std::vector<size_t>& GetTriggered() { return triggered_; }

		//Get memory to fill it;
		float* GetMutableData() {

			return data_.get();
		}


		std::vector<int>& GetMutableCorrections() { return corrections_; }

		size_t GetStride() { return stride_; }

	};

	class ILFSupervisor {
	public:

		using TDetections = std::vector<awpRect>;

	public:
		virtual ~ILFSupervisor() = default;
		virtual TDetections Detect(std::shared_ptr<TLFImage> img) = 0;
	};

	class ICorrector
	{
	public:
		virtual ~ICorrector() = default;

		virtual bool Correct(const TFeatures& features, std::vector<int>& corrections) = 0;

		virtual bool LoadXML(TiXmlElement* parent) = 0;
		virtual TiXmlElement* SaveXML() = 0;
	};

	class ICorrectorTrainer
	{
	public:
		virtual ~ICorrectorTrainer() = default;

		// Setup new image samples extraction
		virtual void BeginImage(ILFScanner* scanner, const ILFSupervisor::TDetections&) = 0;
		// Process new samples
		//start for each batchs
		virtual std::vector<std::unique_ptr<ICorrector>> ProcessSamples(const std::shared_ptr<TFeatures>& feats ) = 0;
		// Notify that image finished
		virtual void EndImage() = 0;
	};

	class IDetector;

	class IWorker {
	public:
		virtual ~IWorker() = default;
		virtual void Detect(std::shared_ptr<TLFImage> img, TFeaturesBuilder& builder) = 0;
	};


	class IDetector {
	public:
		virtual void Detect(std::shared_ptr<TLFImage> img, TFeaturesBuilder& builder) = 0;
		virtual ~IDetector() = default;
		virtual ILFScanner* GetScanner() = 0;
		virtual std::unique_ptr<IWorker> CreateWorker() = 0;
		
		virtual bool LoadXML(TiXmlElement* parent) = 0;
		virtual TiXmlElement* SaveXML() = 0;
		virtual TFeatures::TMapPtr GetMap() = 0;
	};


	class TCorrectors {
	public:/*
		virtual bool LoadXML(TiXmlElement* parent) {
			// TODO: load detector and all correctors
			return false;
		}

		virtual TiXmlElement* SaveXML() {
			// TODO: save detector and all correctors
			return nullptr;
		}

		void			Apply(const TFeatures& features, std::vector<int>& corrections) {

			std::shared_lock<std::shared_mutex> lock(mutex_);
			//todo sele
			
			std::fill(corrections.begin(), corrections.end(), kNoCorrection);
			for (const auto& corrector : correctors_) {
				corrector->Correct(features, corrections);
			}
		}
				

		void			AddCorrector(std::unique_ptr<ICorrector> corr) {
			std::lock_guard<std::shared_mutex> lock(mutex_);
			//todo sele
			//correctors_.emplace_back(std::move(corr));
		}
		void			AddCorrectors(std::vector<std::unique_ptr<ICorrector>> corrs) {
			std::lock_guard<std::shared_mutex> lock(mutex_);
			//todo sele
			//correctors_.insert(correctors_.end(), corrs.begin(), corrs.end());
		}


	private:
		std::shared_mutex									mutex_;

		std::vector<std::unique_ptr<ICorrector>>			correctors_;
*/
	};

	// TODO: move to separate header



	class TLFAgent {
	public:
		TLFAgent(std::unique_ptr<agent::IDetector> detector)
			: detector_(std::move(detector)),
				pool_([this]() { return std::make_unique<TFeaturesBuilder>(detector_->GetMap()); }) {}
		virtual ~TLFAgent() = default;

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
		TLFAgent() = delete;
	protected:
		std::shared_ptr<ILFSupervisor>					supervisor_;
		std::unique_ptr<agent::IDetector>				detector_;

		std::unique_ptr<agent::ICorrectorTrainer>		trainer_;


		std::vector<std::unique_ptr<IWorker>>			workers_;

		pool::ObjectPool<agent::TFeaturesBuilder>		pool_;

		std::unique_ptr<agent::TCorrectors>				correctors_;

		// Indices of detected fragments
		std::vector<size_t>								detected_indices_;

		size_t		batch_size_ = 2048;

		// MUST be equal to count of batches used simultaneously
		size_t		max_threads_ = 32;

	};

	std::shared_ptr<TLFAgent> CreateAgent();


}

