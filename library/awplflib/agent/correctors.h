#pragma once

#include <numeric>
#include <memory>
#include <mutex>
#include <shared_mutex>

#include "agent/agent.h"
#include "utils/xml.h"


namespace agent {

	enum ECorrectorType {
		kCorrectorType_Unknown = 0,
		kCorrectorType_FN,
		kCorrectorType_FP,
	};

	class TCorrectorBase : public ICorrector {
	public:

		// Correct result of detector using features.
		virtual void Correct(const TFeatures& features, std::vector<int>& corrections) override {
			if (type_ != kCorrectorType_Unknown) {

				int expected_result = (type_ == kCorrectorType_FN) ? 1 : 0;

				auto IsProcessNeeded = [&](const size_t& row) {
					const auto res = features.GetDetectorResult(row + features.frags_begin());
					return (expected_result != res) && (corrections.at(row) == kNoCorrection);
				};

				
				//Filter rows to temporary matrix;
				filtered_data_.CopyRowsIf(IsProcessNeeded, features.matrix());

				corrections_.clear();
				corrections_.resize(filtered_data_.rows(), 0);

				Process(filtered_data_, corrections_);

				// Copy Result
				for (size_t row = 0, cur = 0; row < corrections.size(); ++row) {
					if (IsProcessNeeded(row)) {
						if (corrections_[cur]) {
							corrections[row] = expected_result;
						}
						cur++;
					}
				}

			}

		}

		virtual void	Process(const TMatrix& data, std::vector<int>& corrections) {
			// Process corrections and push [0, 1] to corrections.
			// 0 - No Corrections Needed
			// 1 - Correct result
		}

		// Serializing methods.
		bool LoadXML(TiXmlElement* node) {
			std::string type = node->Attribute("type");

			if (type == "FN")
				type_ = kCorrectorType_FN;
			else if (type == "FP")
				type_ = kCorrectorType_FP;
			else
				type_ = kCorrectorType_Unknown;

			return true;
		}
		
		virtual TiXmlElement* SaveXML() {
			TiXmlElement* corrector_node = new TiXmlElement("TCorrectorBase");

			corrector_node->SetAttribute("type", type_ == kCorrectorType_FN ? "FN" : "FP");

			return corrector_node;
		}

	private:
		ECorrectorType							type_;
		TMatrix									filtered_data_;
		std::vector<int>						corrections_;
	};

	

	class TBaselineCorrector : public TCorrectorBase {
		std::vector<float>	center_;	// shape (feats, 1)
		TMatrix	pca_proj_;	// shape (feats, proj_size)

		//TMatrix centroids_;	// shape (c_num, proj_size)

		std::vector<std::vector<float>>	centroids_;

		TMatrix fisher_;	// shape (proj_size, c_num)
		std::vector<float> thres_;		// shape (c_num, 1)

		// Temporary
		TMatrix	projected_;


	public:
	

		virtual void	Process(const TMatrix& data, std::vector<int>& corrections);

		// Serializing methods.
		bool LoadXML(TiXmlElement* node);

		TiXmlElement* SaveXML() override;
	};
}



