#pragma once

#include <numeric>
#include <memory>
#include <mutex>
#include <shared_mutex>

#include "utils/object_pool.h"
#include "utils/matrix.h"
#include "agent/features.h"

#include "agent/agent.h"
#include "agent/correctors.h"
#include "agent/samples_map.h"

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

		// Setup trainer internal state for start image processing
		virtual void Setup(const TFragments& fragments, const TDetections& detections) override {
			std::unique_lock<std::mutex> lock(mtx_);
			fn_map_.Setup(detections.size());
		}
		
		virtual std::vector<std::unique_ptr<ICorrector>> Consume() override {
			std::unique_lock<std::mutex> lock(mtx_);
			return std::move(correctors_);
		}

		// Process new samples for each batch in separate thread.
		virtual void Collect(const TFragments& fragments, const TFeatures& feats, const TDetections& detections) override {

			ResizeIfNeeded(feats.layout());

			for (size_t frag = feats.frags_begin(); frag < feats.frags_end(); ++frag) {
				// Find frag indices that overlap proper detections
				float best_overlap = 0;
				size_t best_id = -1;
				for (size_t id = 0; id < detections.size(); ++id) {

					float overlap = float(detections[id].RectOverlap(fragments.get(frag)));
					if (overlap > best_overlap) {
						best_overlap = overlap;
						best_id = id;
					}
				}
				GatherFeatures(feats, frag, best_id, best_overlap);
			}

		}

		// Finish collecting samples and start training
		virtual void Train() override {
			std::unique_lock<std::mutex> lock(mtx_);
			Erase(fn_, fn_map_.GetFree());
			cv_.notify_one();
		}

		virtual void Finish() override {
			
			cv_.notify_one();
		}
				
		// Serializing methods.
		virtual bool LoadXML(TiXmlElement* parent) {
			parent->QueryFloatAttribute("hi_threshold", &hi_overlap_);
			parent->QueryFloatAttribute("low_threshold", &low_overlap_);
			parent->QueryFloatAttribute("tn_prob", &use_tn_prob_);
			parent->QueryValueAttribute("min_stages", &min_features_stages_);

			parent->QueryValueAttribute("min_fn_samples", &min_fn_samples_);
			parent->QueryValueAttribute("min_fp_samples", &min_fp_samples_);
			
			return true;
		}
		virtual TiXmlElement* SaveXML() {
			TiXmlElement* trainer_node = new TiXmlElement("TCorrectorTrainerBase");

			trainer_node->SetDoubleAttribute("hi_threshold", hi_overlap_);
			trainer_node->SetDoubleAttribute("low_threshold", low_overlap_);
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
					if (fn_.rows() >= min_fn_samples_ || flush_) {
						fn = fn_;
						tn = tn_;
						fn_.Clear();
						tn_.Clear();
					}

					if (fp_.rows() >= min_fp_samples_ || flush_) {
						fp = fp_;
						tp = tp_;
						fp_.Clear();
						tp_.Clear();
					}
					flush_ = false;
				}


				std::vector<std::unique_ptr<ICorrector>> correctors;
				if (fn.rows() > 0) {
					// Run training of FN corrector;
					auto result = TrainFnCorrector(fn, tn);
					if (result) {
						if (self_test_) {
							SelfTestCorrector("FN", result.get(), fn, tn);
						}

						std::unique_lock<std::mutex> lock(mtx_);
						correctors_.emplace_back(std::move(result));
					}
					fn.Clear();
					tn.Clear();
				}

				if (fp.rows() > 0) {
					// Run training of FP corrector;
					auto result = TrainFpCorrector(fp, tp);
					if (result) {
						if (self_test_) {
							SelfTestCorrector("FP", result.get(), fp, tp);
						}

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
		virtual std::unique_ptr<TCorrectorBase>	TrainFnCorrector(const TMatrix& fn, const TMatrix& tn) {
			return nullptr;
		}

		// Train False Positive Corrector;
		virtual std::unique_ptr<TCorrectorBase>	TrainFpCorrector(const TMatrix& fp, const TMatrix& tp) {
			return nullptr;
		}

	private:
		static void SelfTestCorrector(const std::string& type, TCorrectorBase* corrector, const TMatrix& false_features, const TMatrix& true_features) {
			int frr_value = false_features.rows() - TestCorrector(corrector, false_features);
			int far_value = TestCorrector(corrector, true_features);

			std::stringstream ss;
			ss << "Self test of " << type << " corrector: FRR=" << frr_value / float(false_features.rows()) << " FAR=" << far_value / float(true_features.rows()) << std::endl;
			std::cout << ss.str();
		}

		static int	TestCorrector(TCorrectorBase* corrector, const TMatrix& mat) {
			int corrections = 0;
			for (int row = 0; row < mat.rows(); ++row) {
				auto result = corrector->Process(mat.GetRow(row), mat.rows());
				corrections += result;
			}
			return corrections;
		}

		// frag - index of fragment
		// det_id - id of detection nearest to
		void		GatherFeatures(const TFeatures& feats, size_t frag, size_t det_id, float gt_overlap) {
			// Gather samples for data.
			// 1) There are 2 overlap threads hi and low. 
			// 2) If detection overlap is < low threshold then it will be fp
			// 3) If overlap is > hi threshold and no detections here then it ll be fn. 
			// 4) All fn errors for detection will be removed if there is one or more detections.

			std::unique_lock<std::mutex> lock(mtx_);

			if (feats.GetDetectorResult(frag)) {
			//if (0) {
				// Object Detected
				if (gt_overlap < low_overlap_) {
					// FP
					// Add to training fp-corrector;
					fp_.AddRow(feats.GetFeats(frag), feats.feats_count());
				}
				else {
					// Stop gathering data for fn corrector cos there is one detection for this id;
					fn_map_.Stop(det_id);
					
					if (gt_overlap > hi_overlap_) {
						// TP
						tp_.AddRow(feats.GetFeats(frag), feats.feats_count());
					}
				}
			}
			else {
				auto stage = min_features_stages_ - 1;
				auto feats_count = feats.layout().at(stage);
				// No object detected
				if (gt_overlap >= hi_overlap_) {
					// FN
					// Add to training fn-corrector;
					if (!fn_map_.IsStopped(det_id)) {
						// Get place for fn;
						
						size_t place = std::min<size_t>(fn_map_.Pop(), fn_.rows());
						fn_map_.Push(det_id, place);

						if (place < fn_.rows()) {
							fn_.CopyRow(place, feats.matrix(), feats.matrix_index(frag), feats_count);

						} else
							fn_.AddRow(feats.GetFeats(frag), feats_count);
					}
				}
				else if (gt_overlap < low_overlap_) {
					// TN
					// Test probability to use it;
					// TODO: make balancing algorithm that changing probability to reduce amount of TN rows.
					auto prob = GetNormalProb();
					if (use_tn_prob_ >= prob)
						tn_.AddRow(feats.GetFeats(frag), feats_count);
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
	
		float								hi_overlap_ = 0.75f;
		float								low_overlap_ = 0.15f;

		// Probability to add tn for futher usage;
		float								use_tn_prob_ = 0.01f;

		size_t								min_features_stages_ = 3;

		size_t								min_fn_samples_ = 500;
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
		SamplesMap							fn_map_;
		
		bool								self_test_ = true;
	private:

		std::mutex							mtx_;
		std::condition_variable				cv_;

		std::thread							thread_;

		std::atomic<bool>					cancel_ = false;

		// Flash collected data;
		std::atomic<bool>					flush_ = false;

		std::vector<std::unique_ptr<ICorrector>>  correctors_;
	};


	std::unique_ptr<TCorrectorTrainerBase> CreateBaselineTrainer(const std::string& script_path, const std::string& state_path);
}
