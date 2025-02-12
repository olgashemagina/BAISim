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
	}
	~TBaselineCorrectorTrainer() {
		
		main_ = {};
		corrector_ = {};
		Py_Finalize();
	}
	bool Initialize(const std::string& script_path, const std::string& state_path);


	virtual std::unique_ptr<ICorrector>	TrainFnCorrector(const TMatrix& fn, const TMatrix& tn);

	// Train False Positive Corrector;
	virtual std::unique_ptr<ICorrector>	TrainFpCorrector(const TMatrix& fp, const TMatrix& tp);

	bool	TrainCorrector_fn(const TMatrix& fn, const TMatrix& tn);

	bool	TrainCorrector_fp(const TMatrix& fp, const TMatrix& tp);


private:
	std::string			state_path_;
	std::string			state_path_fn_;
	std::string			state_path_fp_;
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


	std::unique_ptr<ICorrector>	TBaselineCorrectorTrainer::TrainFnCorrector(const TMatrix& fn, const TMatrix& tn) {

		if (TrainCorrector_fn(fn, tn)) {
			auto corrector = load_xml(state_path_fn_, [](TiXmlElement* node) {
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
	std::unique_ptr<ICorrector>	TBaselineCorrectorTrainer::TrainFpCorrector(const TMatrix& fp, const TMatrix& tp) {
		if (TrainCorrector_fp(fp, tp)) {

			auto corrector = load_xml(state_path_fp_, [](TiXmlElement* node) {
				auto  agent = std::make_unique<TBaselineCorrector>(kCorrectorType_FP);
				if (node && agent->LoadXML(node))
					return agent;
				return std::unique_ptr<TBaselineCorrector>{};
				});

			return corrector;

		}

		return nullptr;
	}

	bool	TBaselineCorrectorTrainer::TrainCorrector_fn(const TMatrix& fn, const TMatrix& tn) {
		// Acquire GIL before any Python operations
		PyLockGIL  gil_locker;

		np::ndarray fn_array = np::from_data(fn.GetRow(0), np::dtype::get_builtin<float>(),
			bp::make_tuple(fn.rows(), fn.cols()),
			bp::make_tuple(fn.cols() * sizeof(float), sizeof(float)),
			bp::object());


		np::ndarray tn_array = np::from_data(tn.GetRow(0), np::dtype::get_builtin<float>(),
			bp::make_tuple(tn.rows(), tn.cols()),
			bp::make_tuple(tn.cols() * sizeof(float), sizeof(float)),
			bp::object());

		//np::ndarray fn_array = vector_to_ndarray(fn.GetRow(0), fn.rows(), fn.cols());

		corrector_.attr("fit")(fn_array, tn_array);

		int index = 1;
		fs::path path = state_path_ + "corrector0_fn.xml";
		while (fs::exists(path))
		{
			path = state_path_ + "corrector" + std::to_string(index) + "_fn.xml";
			index++;
		}
		state_path_fn_ = path.generic_string();

		corrector_.attr("save_state")(state_path_fn_);

		return true;
	}

	bool	TBaselineCorrectorTrainer::TrainCorrector_fp(const TMatrix& fp, const TMatrix& tp) {
		// Acquire GIL before any Python operations
		PyLockGIL  gil_locker;

		np::ndarray fp_array = np::from_data(fp.GetRow(0), np::dtype::get_builtin<float>(),
			bp::make_tuple(fp.rows(), fp.cols()),
			bp::make_tuple(fp.cols() * sizeof(float), sizeof(float)),
			bp::object());


		np::ndarray tp_array = np::from_data(tp.GetRow(0), np::dtype::get_builtin<float>(),
			bp::make_tuple(tp.rows(), tp.cols()),
			bp::make_tuple(tp.cols() * sizeof(float), sizeof(float)),
			bp::object());

		corrector_.attr("fit")(fp_array, tp_array);

		int index = 1;
		fs::path path = state_path_ + "corrector0_fp.xml";
		while (fs::exists(path))
		{
			path = state_path_ + "corrector" + std::to_string(index) + "_fp.xml";
			index++;
		}
		state_path_fp_ = path.generic_string();

		corrector_.attr("save_state")(state_path_fp_);

		return true;
	}

	std::unique_ptr<TCorrectorTrainerBase> agent::CreateBaselineTrainer(const std::string& script_path, const std::string& state_path)
	{
		auto trainer = std::make_unique<TBaselineCorrectorTrainer>();
		if (trainer->Initialize(script_path, state_path))
			return trainer;
		
		return {};
	}
