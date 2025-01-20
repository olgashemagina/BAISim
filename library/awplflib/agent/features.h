#pragma once

#include <numeric>
#include <memory>

#include "utils/matrix.h"

namespace agent {

	// No correction marker
	static const int kNoCorrection = -1;


	// Features that collected from detector. May be represented as matrix with rows describing fragments
	// and cols describing features of detector.
	// Detector has structure of stages, size of each stage is described by Map.
	// Triggered vector describing number of stage that return 0 as no object detection.
	// Corrections is a vector of triplets {NoCorrection, 0, 1}
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


	// Provide access to protected fields of TFeatures
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

	
}


