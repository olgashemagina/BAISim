#include "corrector_trainer.h"
#include "correctors.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

#include <boost/python.hpp>
#include <boost/python/numpy.hpp>

namespace bp = boost::python;
namespace np = boost::python::numpy;
namespace fs = std::filesystem;
using namespace agent;

struct PyLockGIL
{

	PyLockGIL(): gstate(PyGILState_Ensure()) { }

	~PyLockGIL() { PyGILState_Release(gstate); }

	PyLockGIL(const PyLockGIL&) = delete;
	PyLockGIL& operator=(const PyLockGIL&) = delete;

	PyGILState_STATE gstate;
};

struct PyRelinquishGIL
{
	PyRelinquishGIL()
		: _thread_state(PyEval_SaveThread()) {}

	~PyRelinquishGIL() { PyEval_RestoreThread(_thread_state); }

	PyRelinquishGIL(const PyLockGIL&) = delete;
	PyRelinquishGIL& operator=(const PyLockGIL&) = delete;

	PyThreadState* _thread_state;
};

class TBaselineCorrectorTrainer : public TCorrectorTrainerBase {

public:

	TBaselineCorrectorTrainer() {
		Py_Initialize();
		np::initialize();
		// Initialize threading support
		PyEval_InitThreads();
		// Release GIL after initialization
		PyEval_SaveThread();
	}
	~TBaselineCorrectorTrainer() {
		
		main_ = {};
		corrector_ = {};
				
		Py_Finalize();
	}
	bool Initialize(const std::string& script_path, const std::string& state_path);


	virtual std::unique_ptr<TCorrectorBase>	TrainFnCorrector(const TMatrix& fn, const TMatrix& tn) override;

	// Train False Positive Corrector;
	virtual std::unique_ptr<TCorrectorBase>	TrainFpCorrector(const TMatrix& fp, const TMatrix& tp) override;


	std::string	TrainCorrector(const std::string& type, const TMatrix& f_feats, const TMatrix& t_feats);


private:
	std::string			state_path_;
	
	bp::object			main_;
	bp::object			corrector_;
};


	bool TBaselineCorrectorTrainer::Initialize(const std::string& script_path, const std::string& state_path) {
	
		// Acquire GIL before any Python operations
		PyLockGIL  gil_locker;

		state_path_ = state_path;
						
		main_ = bp::import("__main__");

		std::ifstream file(script_path);
		if (!file.is_open())
			return false;
		
		std::string script_content((std::istreambuf_iterator<char>(file)),
			std::istreambuf_iterator<char>());
				
		bp::exec(script_content.c_str(), main_.attr("__dict__"));

		corrector_ = main_.attr("BaselineCorrector")();

		// Release GIL after initialization
		//PyEval_SaveThread();
		
		return corrector_;
	}


	std::unique_ptr<TCorrectorBase>	TBaselineCorrectorTrainer::TrainFnCorrector(const TMatrix& fn, const TMatrix& tn) {
		auto path = TrainCorrector("FN", fn, tn);
		if (!path.empty()) {
			auto corrector = load_xml(path, [](TiXmlElement* node) {
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
	std::unique_ptr<TCorrectorBase>	TBaselineCorrectorTrainer::TrainFpCorrector(const TMatrix& fp, const TMatrix& tp) {
		auto path = TrainCorrector("FP", fp, tp);
		if (!path.empty()) {

			auto corrector = load_xml(path, [](TiXmlElement* node) {
				auto  agent = std::make_unique<TBaselineCorrector>(kCorrectorType_FP);
				if (node && agent->LoadXML(node))
					return agent;
				return std::unique_ptr<TBaselineCorrector>{};
				});

			return corrector;

		}

		return nullptr;
	}

	std::string	TBaselineCorrectorTrainer::TrainCorrector(const std::string& type, const TMatrix& f_feats, const TMatrix& t_feats) {
		// Acquire GIL before any Python operations
		PyLockGIL  gil_locker;

		np::ndarray f_array = np::from_data(f_feats.GetRow(0), np::dtype::get_builtin<float>(),
			bp::make_tuple(f_feats.rows(), f_feats.cols()),
			bp::make_tuple(f_feats.cols() * sizeof(float), sizeof(float)),
			bp::object());


		np::ndarray t_array = np::from_data(t_feats.GetRow(0), np::dtype::get_builtin<float>(),
			bp::make_tuple(t_feats.rows(), t_feats.cols()),
			bp::make_tuple(t_feats.cols() * sizeof(float), sizeof(float)),
			bp::object());
						

		try {
			auto callable = corrector_.attr("fit");
			callable(t_array, f_array);
		}
		catch (bp::error_already_set const&) {
			PyErr_Print();
		}
				
		
		int index = 1;
		fs::path path = state_path_ + "corrector0_" + type + ".xml";
		while (fs::exists(path))
		{
			path = state_path_ + "corrector" + std::to_string(index) + "_" + type + ".xml";
			index++;
		}

		
		try {
			corrector_.attr("save_state")(path.generic_string());
		}
		catch (bp::error_already_set const&) {
			PyErr_Print();
		}
		

		return path.generic_string();
	}


	std::unique_ptr<TCorrectorTrainerBase> agent::CreateBaselineTrainer(const std::string& script_path, const std::string& state_path)
	{
		auto trainer = std::make_unique<TBaselineCorrectorTrainer>();
		if (trainer->Initialize(script_path, state_path))
			return trainer;
		
		return {};
	}
