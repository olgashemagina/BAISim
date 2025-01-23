#include "corrector_trainer.h"
#include "correctors.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <boost/python.hpp>
#include <boost/python/numpy.hpp>

namespace bp = boost::python;
namespace np = boost::python::numpy;

using namespace agent;


class TBaselineCorrectorTrainer : public TCorrectorTrainerBase {

public:

	TBaselineCorrectorTrainer() {
		
	}

	bool Initialize(const std::string& script_path, const std::string& state_path) {
		
		Py_Initialize();

		state_path_ = state_path;
						
		main_ = bp::import("__main__");
		bp::exec_file(script_path.c_str(), main_.attr("__dict__"));

		corrector_ = main_.attr("BaselineCorrector")();
		return corrector_;
	}


	virtual std::unique_ptr<ICorrector>	TrainFnCorrector(const TMatrix& fn, const TMatrix& tn) {

		if (TrainCorrector(fn, tn)) {
			
			auto corrector = load_xml(state_path_, [](TiXmlElement* node) {
				auto  agent = std::make_unique<TBaselineCorrector>(kCorrectorType_FN);
				if (node && agent->LoadXML(node))
					return agent;
				return std::unique_ptr<TBaselineCorrector>{};
				});

			return corrector;

		}

		return nullptr;
	}

	// Train False Positive Corrector;
	virtual std::unique_ptr<ICorrector>	TrainFpCorrector(const TMatrix& fp, const TMatrix& tp) {
		if (TrainCorrector(fp, tp)) {

			auto corrector = load_xml(state_path_, [](TiXmlElement* node) {
				auto  agent = std::make_unique<TBaselineCorrector>(kCorrectorType_FP);
				if (node && agent->LoadXML(node))
					return agent;
				return std::unique_ptr<TBaselineCorrector>{};
				});

			return corrector;

		}

		return nullptr;
	}

	bool	TrainCorrector(const TMatrix& fn, const TMatrix& tn) {
		np::ndarray fn_array = np::from_data(fn.GetRow(0), np::dtype::get_builtin<float>(),
			bp::make_tuple(fn.rows(), fn.cols()),
			bp::make_tuple(sizeof(float)),
			bp::object());


		np::ndarray tn_array = np::from_data(tn.GetRow(0), np::dtype::get_builtin<float>(),
			bp::make_tuple(tn.rows(), tn.cols()),
			bp::make_tuple(sizeof(float)),
			bp::object());

		corrector_.attr("fit")(fn_array, tn_array);
		corrector_.attr("save_state")(state_path_);

		return true;
	}


private:
	std::string			state_path_;
	bp::object			main_;
	bp::object			corrector_;
};