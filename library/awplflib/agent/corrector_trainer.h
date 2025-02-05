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

		TCorrectorTrainerBase() : thread_([=]() { RunTrain(); }) {

		}

		virtual ~TCorrectorTrainerBase() {
			cancel_ = true;
			if (thread_.joinable()) {
				cv_.notify_one();
				thread_.join();
			}
		}


		virtual std::vector<std::unique_ptr<ICorrector>> ConsumeCorrectors() {
			std::unique_lock<std::mutex> lock(mtx_);
			return std::move(correctors_);
		}

		// Process new samples for each batch in separate thread.
		virtual void CollectSamples(ILFScanner* scanner, const TFeatures& feats, const TDetections& detections) override {

			ResizeIfNeeded(feats.layout());

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
		virtual void Train() override {
			cv_.notify_one();
		}

		// Serializing methods.
		virtual bool LoadXML(TiXmlElement* parent) {
			parent->QueryFloatAttribute("positive_threshold", &positive_threshold_);
			parent->QueryFloatAttribute("negative_threshold", &negative_threshold_);
			parent->QueryFloatAttribute("tn_prob", &use_tn_prob_);
			parent->QueryValueAttribute("min_stages", &min_features_stages_);

			parent->QueryValueAttribute("min_fn_samples", &min_fn_samples_);
			parent->QueryValueAttribute("min_fp_samples", &min_fp_samples_);
			
			return true;
		}
		virtual TiXmlElement* SaveXML() {
			TiXmlElement* trainer_node = new TiXmlElement("TCorrectorTrainerBase");

			trainer_node->SetDoubleAttribute("positive_threshold", positive_threshold_);
			trainer_node->SetDoubleAttribute("negative_threshold", negative_threshold_);
			trainer_node->SetDoubleAttribute("tn_prob", use_tn_prob_);
			trainer_node->SetAttribute("min_stages", min_features_stages_);

			trainer_node->SetAttribute("min_fn_samples", min_fn_samples_);
			trainer_node->SetAttribute("min_fp_samples", min_fp_samples_);

			//trainer_node->SetValue("NewName");

			return trainer_node;
		}

	protected:
		void		RunTrain() {
			TMatrix fn, tn;
			TMatrix fp, tp;
			
			while (!cancel_) {
				{
					std::unique_lock<std::mutex> lock(mtx_);

					cv_.wait(lock);

					if (cancel_)
						break;

					// Move data for training
					if (fn_.rows() >= min_fn_samples_) {
						fn = std::move(fn_);
						tn = std::move(tn_);
					}

					if (fp_.rows() >= min_fp_samples_) {
						fp = std::move(fp_);
						tp = std::move(tp_);
					}
				}


				std::vector<std::unique_ptr<ICorrector>> correctors;
				if (fn.rows() >= min_fn_samples_) {
					// Run training of FN corrector;
					auto result = TrainFnCorrector(fn, tn);
					if (result) {
						std::unique_lock<std::mutex> lock(mtx_);
						correctors_.emplace_back(std::move(result));
					}
					fn.Clear();
					tn.Clear();
				}

				if (fp.rows() >= min_fp_samples_) {
					// Run training of FP corrector;
					auto result = TrainFpCorrector(fp, tp);
					if (result) {
						std::unique_lock<std::mutex> lock(mtx_);
						correctors.emplace_back(std::move(result));
					}
					fp.Clear();
					tp.Clear();
				}
			}
		}
	public:
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
				if (gt_overlap < positive_threshold_) {
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
				auto stage = min_features_stages_ - 1;
				auto feats_count = feats.layout().at(stage);
				// No object detected
				if (gt_overlap >= positive_threshold_) {
					// FN
					// Add to training fn-corrector;
					fn_.AddRow(feats.GetFeats(index), feats_count);
				}
				else if (gt_overlap > std::min<float>(positive_threshold_, negative_threshold_)) {
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

		void		ResizeIfNeeded(const TLayout& layout) {
			size_t features_count = layout.back();

			std::unique_lock<std::mutex> lock(mtx_);
			if (fp_.cols() < features_count)
				fp_.Resize(fp_.rows(), features_count);

			if (tp_.cols() < features_count)
				tp_.Resize(tp_.rows(), features_count);

			features_count = layout.at(min_features_stages_ - 1);

			if (fn_.cols() < features_count)
				fn_.Resize(fn_.rows(), features_count);

			if (tn_.cols() < features_count)
				tn_.Resize(tn_.rows(), features_count);

		}

		// Calculate normal probability for selecting data;
		static float		GetNormalProb() {
			return float(std::rand() / double(RAND_MAX));
		}



	private:
		float								positive_threshold_ = 0.4f;
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
		std::condition_variable				cv_;

		std::thread							thread_;

		std::atomic<bool>					cancel_ = false;

		std::vector<std::unique_ptr<ICorrector>>  correctors_;
	};


	std::unique_ptr<TCorrectorTrainerBase> CreateBaselineTrainer(const std::string& script_path, const std::string& state_path);
}
