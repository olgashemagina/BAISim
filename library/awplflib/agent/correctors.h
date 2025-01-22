#pragma once

#include <numeric>
#include <memory>
#include <mutex>
#include <shared_mutex>

#include "agent/agent.h"

// BOOST_UBLAS_SHALLOW_ARRAY_ADAPTOR must be defined before include vector.hpp
#define BOOST_UBLAS_SHALLOW_ARRAY_ADAPTOR

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>

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

	namespace ublas = boost::numeric::ublas;
	using Vector = ublas::vector<float>;
	using Matrix = ublas::matrix<float>;
	using ArrayAdaptor = ublas::shallow_array_adaptor<float>;
	using MatrixView = ublas::matrix<float, ublas::row_major, ublas::shallow_array_adaptor<float> >;

	

	class TBaselineCorrector : public TCorrectorBase {
		Matrix	centre_;	// shape (1, rows)
		Matrix	pca_proj_;	// shape (rows, proj_size)

		Matrix centroids_;	// shape (c_num, proj_size)


	public:
		virtual void	Process(const TMatrix& data, std::vector<int>& corrections) {
			// Process corrections and push [0, 1] to corrections.
			// 0 - No Corrections Needed
			// 1 - Correct result
			

			MatrixView features(data.rows(), data.cols(), ArrayAdaptor(data.rows(), const_cast<float*>(data.GetRow(0))));

			Matrix pca_space = prod(features - centre_, pca_proj_);

			Matrix distance(data.rows(), centroids_.size1(), 0);

			for (size_t c = 0; c < centroids_.size1(); ++c) {
				for (size_t r = 0; r < data.rows(); ++r) {
					distance.at_element(r, c) = inner_prod(row(centroids_, c), row(pca_space, r));
				}
			}
			// TODO:

		}

		// Serializing methods.
		bool LoadXML(TiXmlElement* node) {
			if (TCorrectorBase::LoadXML(node)) {
				// TODO:


				return true;
			}

			return false;
		}

		virtual TiXmlElement* SaveXML() {

			auto node = TCorrectorBase::SaveXML();

			node->SetValue("TBaselineCorrector");

			// TODO: save data

			return node;
		}
	};
}



