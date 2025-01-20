#pragma once

#include "LFImage.h"
#include "LF.h"

#include <numeric>
#include <memory>
#include <mutex>
#include <shared_mutex>

#include "utils/object_pool.h"
#include "utils/matrix.h"


class ILFSupervisor {
public:

	using TDetections = std::vector<TLFRect>;

public:
	virtual ~ILFSupervisor() = default;
	virtual TDetections Detect(std::shared_ptr<TLFImage> img) = 0;
};

namespace agent {

	static const int kNoCorrection = -1;

	class TFeatures {
	public:
		// Map is vector of summing current stage count of features and all previous features;
		using TMap = std::vector<size_t>;
		using TMapPtr = std::shared_ptr<TMap>;

	public:
		TFeatures() {}

		virtual ~TFeatures() {}

	public:
		
		int GetDetectorResult(size_t fragment_index) const { return (triggered_[fragment_index - frags_begin_] < -1) ? 1 : 0; }

		int GetCorrectedResult(size_t fragment_index) const {
			return (corrections_[fragment_index - frags_begin_] == kNoCorrection) ?
				GetDetectorResult(fragment_index) : corrections_[fragment_index - frags_begin_];
		}

		size_t	GetTriggeredStage(size_t fragment_index) const { return triggered_.at(fragment_index - frags_begin_); }

		
		size_t frags_begin() const { return frags_begin_; }
		size_t frags_end() const { return frags_end_; }

		size_t	frags_count() const { return frags_end_ - frags_begin_; }

		size_t	feats_count() const { return data_.cols(); }

		size_t	batch_size() const { return data_.rows(); }

		const std::vector<int>& corrections() const { return corrections_; }

		TMapPtr map() const { return map_; }

		const float* GetFeats(size_t fragment_index) const {
			return data_.GetRow(fragment_index - frags_begin_);
		}

		const TMatrix& matrix() const { return data_; }

		
	protected:
		//Matrix of data there rows is fragments and cols is features
		TMatrix								data_;

		// Map of detector
		TMapPtr								map_;
		
		size_t								frags_begin_ = 0;
		size_t								frags_end_ = 0;
		//Number triggered stage or -1 if noone triggered;

		std::vector<size_t>					triggered_;

		std::vector<int>					corrections_;
	};

	class TFeaturesBuilder : public TFeatures {
	public:
		TFeaturesBuilder() = default;

	public:
		// Set fragments and 
		void Setup(TMapPtr map, size_t frags_begin, size_t frags_end, size_t batch_size) {
			frags_begin_ = frags_begin;
			frags_end_ = frags_end;

			size_t features_count = data_.cols();

			if (map_ != map) {
				features_count = map->back();
				map_ = map;
			}

			if (data_.rows() != batch_size) {
				data_.Resize(batch_size, features_count);
				triggered_.resize(batch_size, -1);
				corrections_.resize(batch_size, kNoCorrection);
			}
			else {
				triggered_.assign(triggered_.size(), -1);
				corrections_.assign(corrections_.size(), kNoCorrection);
			}
		}

		std::vector<size_t>& GetTriggered() { return triggered_; }

		//Get memory to fill it;
		TMatrix& GetMutableData() { return data_; }


		std::vector<int>& GetMutableCorrections() { return corrections_; }


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
				
		// Process new samples
		virtual void CollectSamples(ILFScanner* scanner, const TFeatures& feats, const ILFSupervisor::TDetections&) = 0;
		// Start training of samples if needed.
		virtual std::vector<std::unique_ptr<ICorrector>> Train() = 0;
	};


	class TCorrectorTrainerBase : public ICorrectorTrainer {
	public:
		virtual ~TCorrectorTrainerBase() = default;


		// Process new samples for each batch in separate thread.
		virtual void CollectSamples(ILFScanner* scanner, const TFeatures& feats, const ILFSupervisor::TDetections& detections) override {

			for (size_t frag = feats.frags_begin(); frag < feats.frags_end(); ++frag) {
				// Find frag indices that overlap proper detections
				float overlap = 0;
				for (const auto& det : detections) {
					overlap = std::max<float>(overlap, float(det.RectOverlap(scanner->GetFragmentRect(frag))));
				}
				GatherFeatures(feats, frag, overlap);
			}

		}
		// Notify that image finished
		virtual std::vector<std::unique_ptr<ICorrector>> Train() override {
			std::vector<std::unique_ptr<ICorrector>> correctors;
			if (fn_.rows() >= min_fn_samples_) {
				// Run training of FN corrector;
				std::unique_lock<std::mutex> lock(mtx_);
				auto result = TrainFnCorrector(fn_, tn_);
				if (result)
					correctors.emplace_back(std::move(result));
			}

			if (fp_.rows() >= min_fp_samples_) {
				// Run training of FP corrector;
				std::unique_lock<std::mutex> lock(mtx_);
				auto result = TrainFpCorrector(fp_, tp_);
				if (result)
					correctors.emplace_back(std::move(result));
			}

			return correctors;
		}

	protected:
		// Train False Negative Corrector;
		virtual std::unique_ptr<ICorrector>	TrainFnCorrector(const TMatrix& fn, const TMatrix& tn) {
			return nullptr;
		}

		// Train False Positive Corrector;
		virtual std::unique_ptr<ICorrector>	TrainFpCorrector(const TMatrix& fp, const TMatrix& tp) {
			return nullptr;
		}

	private:
		void		GatherFeatures(const TFeatures& feats, size_t index, float gt_overlap) {
			std::unique_lock<std::mutex> lock(mtx_);

			if (feats.GetDetectorResult(index)) {
				// Object Detected
				if (gt_overlap < detection_threshold_) {
					// FP
					// Add to training fp-corrector;
					fp_.AddRow(feats.GetFeats(index), feats.feats_count());
				}
				else {
					// TP
					// 
					// Add to training fn-corrector;
					// Add to training fp-corrector;
					tp_.AddRow(feats.GetFeats(index), feats.feats_count());
				}
			}
			else {
				auto stage = std::min<size_t>(feats.GetTriggeredStage(index), min_features_stages_ - 1);
				auto feats_count = feats.map()->at(stage);
				// No object detected
				if (gt_overlap >= detection_threshold_) {
					// FN
					// Add to training fn-corrector;
					fn_.AddRow(feats.GetFeats(index), feats_count);
				}
				else if (gt_overlap > std::min<float>(detection_threshold_, negative_threshold_)) {
					// TN
					// Test probability to use it;
					// TODO: make balancing algorithm that changing probability to reduce amount of TN rows.
					if (use_tn_prob_ >= GetNormalProb() )
						tn_.AddRow(feats.GetFeats(index), feats_count);
				}
				else {
					// Skip other TN
				}
			}
		}

		// Calculate normal probability for selecting data;
		static float		GetNormalProb() {
			return float(std::rand() / double(RAND_MAX));
		}

		

	private:
		float								detection_threshold_ = 0.4f;
		float								negative_threshold_ = 0.1f;

		// Probability to add tn for futher usage;
		float								use_tn_prob_ = 0.3f;

		size_t								min_features_stages_ = 3;

		size_t								min_fn_samples_ = 1000;
		size_t								min_fp_samples_ = 500;

	private:
		// False Negatives
		TMatrix								fn_;
		// True Negatives
		TMatrix								tn_;

		// False Positives
		TMatrix								fp_;

		// True Positives
		TMatrix								tp_;

	private:

		std::mutex							mtx_;



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
		virtual TFeatures::TMapPtr GetMap() = 0;

		virtual std::unique_ptr<IWorker> CreateWorker() = 0;

		virtual bool LoadXML(TiXmlElement* parent) = 0;
		virtual TiXmlElement* SaveXML() = 0;
		
	};


	class TCorrectors {
	public:
		bool LoadXML(TiXmlElement* parent) {
			// TODO: load detector and all correctors
			return false;
		}

		TiXmlElement* SaveXML() {
			// TODO: save detector and all correctors
			return nullptr;
		}

		void			Apply(const TFeatures& features, std::vector<int>& corrections) {
			std::shared_lock<std::shared_mutex> lock(mutex_);

			std::fill(corrections.begin(), corrections.end(), kNoCorrection);
			for (const auto& corrector : correctors_) {
				corrector->Correct(features, corrections);
			}
		}


		void			AddCorrector(std::unique_ptr<ICorrector> corr) {
			std::unique_lock<std::shared_mutex> lock(mutex_);

			correctors_.emplace_back(std::move(corr));
		}
		void			AddCorrectors(std::vector<std::unique_ptr<ICorrector>> corrs) {
			std::unique_lock<std::shared_mutex> lock(mutex_);

			auto it = std::next(correctors_.begin(), correctors_.size());
			std::move(corrs.begin(), it, std::back_inserter(correctors_));
			//correctors_.insert(correctors_.end(), corrs.begin(), corrs.end());
		}


	private:
		std::shared_mutex									mutex_;
		std::vector<std::unique_ptr<ICorrector>>			correctors_;

	};
}

	// TODO: move to separate header



class TLFAgent {
public:
	TLFAgent();
	
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

protected:
	std::shared_ptr<ILFSupervisor>					supervisor_;
	std::unique_ptr<agent::IDetector>				detector_;

	std::unique_ptr<agent::ICorrectorTrainer>		trainer_;


	std::vector<std::unique_ptr<agent::IWorker>>	workers_;

	pool::ObjectPool<agent::TFeaturesBuilder>		pool_;

	agent::TCorrectors								correctors_;

	// Indices of detected fragments
	std::vector<size_t>								detected_indices_;

	size_t		batch_size_ = 2048;

	// MUST be equal to count of batches used simultaneously
	int			max_threads_ = 32;

};



std::shared_ptr<TLFAgent> CreateAgent();

std::shared_ptr<ILFSupervisor> CreateSupervisor();
