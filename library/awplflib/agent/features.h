#pragma once

#include <numeric>
#include <memory>
#include <limits>

#include "utils/matrix.h"

namespace agent {

	// No correction marker
	static constexpr int kNoCorrection = -1;

	static const int	kNoTriggered = -1;

	// Layout is vector of summing current stage count of features and all previous features;
	using TLayout = std::vector<size_t>;
	
	// Features that collected from detector. May be represented as matrix with rows describing fragments
	// and cols describing features of detector.
	// Detector has structure of stages, size of each stage is described by Map.
	// Triggered vector describing number of stage that return 0 as no object detection.
	// Corrections is a vector of triplets {NoCorrection, 0, 1}
	class TFeatures {
	public:
		
	public:
		TFeatures() {}

		virtual ~TFeatures() {}

	public:

		int GetDetectorResult(size_t fragment_index) const { return (triggered_[fragment_index - frags_begin_] == kNoTriggered) ? 1 : 0; }

		int GetCorrectedResult(size_t fragment_index) const {
			return (corrections_[fragment_index - frags_begin_] == kNoCorrection) ?
				GetDetectorResult(fragment_index) : corrections_[fragment_index - frags_begin_];
		}

		int	GetTriggeredStage(size_t fragment_index) const { return triggered_.at(fragment_index - frags_begin_); }

		float	GetScore(size_t fragment_index) const { return scores_.at(fragment_index - frags_begin_); }


		size_t frags_begin() const { return frags_begin_; }
		size_t frags_end() const { return frags_end_; }

		size_t	frags_count() const { return frags_end_ - frags_begin_; }

		size_t	feats_count() const { return data_.cols(); }

		size_t	batch_size() const { return data_.rows(); }

		const std::vector<int>& corrections() const { return corrections_; }

		const TLayout& layout() const { return layout_; }

		const float* GetFeats(size_t fragment_index) const {
			return data_.GetRow(fragment_index - frags_begin_);
		}

		const TMatrix& matrix() const { return data_; }


	protected:
		//Matrix of data there rows is fragments and cols is features
		TMatrix								data_;

		// Layout of detector
		TLayout								layout_;

		// Fragments bounds
		size_t								frags_begin_ = 0;
		size_t								frags_end_ = 0;
		
		// Number triggered stage or -1 if none triggered
		std::vector<int>					triggered_;

		// Scores of poor stage
		std::vector<float>					scores_;

		// TODO: move corrections to builder
		std::vector<int>					corrections_;
	};


	// Provide access to protected fields of TFeatures
	class TFeaturesBuilder : public TFeatures {
	public:
		TFeaturesBuilder() = default;

	public:
		void SetBounds(size_t frags_begin, size_t frags_end) {
			frags_begin_ = frags_begin;
			frags_end_ = frags_end;
		}

		// Reset memory for layout;
		void Reset( const TLayout& layout) {
			
			size_t features_count = data_.cols();

			if (layout_.size() != layout.size() || 
				layout_.back() != layout.back()) {
				features_count = layout.back();
				layout_ = layout;
			}

			data_.Clear();
			data_.Resize(frags_count(), features_count);

			triggered_.clear();
			corrections_.clear();

			triggered_.resize(frags_count(), kNoTriggered);
			scores_.resize(frags_count(), 0.f);
			corrections_.resize(frags_count(), kNoCorrection);

		}

		
		//Get memory to fill it;
		TMatrix& GetMutableData() { return data_; }

		float* GetFeats(size_t fragment_index) {
			return data_.GetRow(fragment_index - frags_begin_);
		}

		auto& triggered() { return triggered_; }
		auto& scores() { return scores_; }

		void SetTriggered(size_t fragment_index, int triggered, float score) {
			triggered_[fragment_index - frags_begin_] = triggered;
			scores_[fragment_index - frags_begin_] = score;
		}


		std::vector<int>& GetMutableCorrections() { return corrections_; }


	};

	
}


