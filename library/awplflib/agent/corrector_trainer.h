#pragma once

#include <numeric>
#include <memory>
#include <mutex>
#include <shared_mutex>

#include "utils/object_pool.h"
#include "utils/matrix.h"
#include "agent/features.h"

#include "agent/agent.h"

namespace agent {


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
					if (use_tn_prob_ >= GetNormalProb())
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


}
