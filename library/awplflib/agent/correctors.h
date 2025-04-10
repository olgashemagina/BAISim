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

		TCorrectorBase(ECorrectorType type = kCorrectorType_Unknown) : type_(type)  {}

		virtual ~TCorrectorBase() {}

		// Correct result of detector using features.
		virtual void Correct(const TFeatures& features, std::vector<int>& corrections) const override {
			if (type_ != kCorrectorType_Unknown) {

				int expected_result = (type_ == kCorrectorType_FN) ? 1 : 0;

				auto IsProcessNeeded = [&](const size_t& row) {
					const auto res = features.GetDetectorResult(row + features.frags_begin());
					return (expected_result != res) && (corrections.at(row) == kNoCorrection);
				};

				if (corrections.size() < features.batch_size())
					corrections.resize(features.batch_size(), kNoCorrection);
											
				// Copy Result
				for (size_t row = 0; row < corrections.size(); ++row) {
					if (IsProcessNeeded(row)) {

						auto corr = Process(features.matrix().GetRow(row), features.feats_count());

						if (corr > 0) {
							corrections[row] = expected_result;
						}
						else if (corr < 0) {
							std::cout << "Error while trying to correct fragment " << row + features.frags_begin() << std::endl;
						}
						
					}
				}

			}

		}

		virtual int	Process(const float* features, size_t size) const {
			// Process corrections and push [0, 1] to corrections.
			// 0 - No Corrections Needed
			// 1 - Correct result
			return 0;
		}

		// Serializing methods.
		bool LoadXML(TiXmlElement* node) {
			
			auto type = node->Attribute("type");
			if (type != nullptr)
			{
				std::string str(type);
				if (str == "FN")
					type_ = kCorrectorType_FN;
				else if (str == "FP")
					type_ = kCorrectorType_FP;
				else
					type_ = kCorrectorType_Unknown;
			}
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

		

	public:

		TBaselineCorrector(ECorrectorType type = kCorrectorType_Unknown) : TCorrectorBase(type) {}

		virtual ~TBaselineCorrector() {}
	

		virtual int	Process(const float* features, size_t size) const override;

		// Serializing methods.
		bool LoadXML(TiXmlElement* node);

		TiXmlElement* SaveXML() override;
	};
}



