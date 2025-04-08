#include "corrector_trainer.h"
#include "correctors.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <cstdlib>

#include "utils/time.h"

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

class PythonEnvironment {
public:
	PythonEnvironment() {
		if (!Py_IsInitialized()) {
			Py_Initialize();
			// Initialize numpy array support
			np::initialize();
			// Initialize threading support
			PyEval_InitThreads();
			// Release GIL after initialization
			thread_state_ = PyEval_SaveThread();
		}
	}

	~PythonEnvironment() {
		PyEval_RestoreThread(thread_state_);

		if (Py_IsInitialized()) {
			PyGILState_Ensure();
			Py_Finalize();
		}
	}

	PyThreadState* thread_state_;
};

static void InitializePython() {
	static PythonEnvironment env;
}


class TBaselineCorrectorTrainer : public TCorrectorTrainerBase {

public:

	TBaselineCorrectorTrainer() {

		InitializePython();
		//Py_Initialize();
		//np::initialize();
		// Initialize threading support
		//PyEval_InitThreads();
		// Release GIL after initialization
		//PyEval_SaveThread();
	}
	~TBaselineCorrectorTrainer() {
		
		PyLockGIL  gil_locker;

		main_ = {};
		corrector_ = {};
				
		//Py_Finalize();
	}
	bool Initialize(const std::string& script_path, const std::string& state_path);
	
	bool LoadXML(TiXmlElement* parent);
	TiXmlElement* TBaselineCorrectorTrainer::SaveXML();

	virtual std::unique_ptr<TCorrectorBase>	TrainFnCorrector(const TMatrix& fn, const TMatrix& tn) override;

	// Train False Positive Corrector;
	virtual std::unique_ptr<TCorrectorBase>	TrainFpCorrector(const TMatrix& fp, const TMatrix& tp) override;


	std::string	TrainCorrector(const std::string& type, const TMatrix& f_feats, const TMatrix& t_feats);



private:
	std::string			state_path_;
	
	bp::object			main_;
	bp::object			corrector_;
	int					numPCA_;
	int					numClusters_;
};


	bool TBaselineCorrectorTrainer::Initialize(const std::string& script_path, const std::string& state_path) {
	
		// Acquire GIL before any Python operations
		numPCA_ = 100;
		numClusters_ = 10;
		PyLockGIL  gil_locker;

		state_path_ = state_path;
						
		main_ = bp::import("__main__");
		std::ifstream file;
		const char* temp = std::getenv("pyscript_path");
		if (temp != NULL)
		{
			std::string env_path(temp);

			file.open(env_path + "\\" + script_path);
		}
		else
			file.open(script_path);
		
		if (!file.is_open()) {
			std::cout << "Cant open file " << script_path << std::endl;
			return false;
		}
		
		std::string script_content((std::istreambuf_iterator<char>(file)),
			std::istreambuf_iterator<char>());
	

		try {
			bp::exec(script_content.c_str(), main_.attr("__dict__"));
		}
		catch (bp::error_already_set const&) {
			PyErr_Print();
		}

		corrector_ = main_.attr("BaselineCorrector")();

		// Release GIL after initialization
		//PyEval_SaveThread();
		
		return corrector_;
	}

	bool TBaselineCorrectorTrainer::LoadXML(TiXmlElement* parent) {
		if (TCorrectorTrainerBase::LoadXML(parent)) {

			if (parent->ValueStr() != "TBaselineCorrectorTrainer") {
				return false;
			}

			parent->QueryValueAttribute("numPCA", &numPCA_);
			parent->QueryValueAttribute("numClusters", &numClusters_);
		}	

		return true;
	}
	TiXmlElement* TBaselineCorrectorTrainer::SaveXML() {
		
		TiXmlElement* trainer_node = TCorrectorTrainerBase::SaveXML();
		trainer_node->SetValue("TBaselineCorrectorTrainer");

		trainer_node->SetAttribute("numPCA", numPCA_);
		trainer_node->SetAttribute("numClusters", numClusters_);

		return trainer_node;
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
				
		if (f_feats.rows() == 0 || t_feats.rows() == 0) {
			std::cout << "Error in size input matrix for " << type << " corrector: f_size=" << f_feats.rows() << ", t_size=" << t_feats.rows() << std::endl;
			return {};
		}

		np::ndarray f_array = np::from_data(f_feats.GetRow(0), np::dtype::get_builtin<float>(),
			bp::make_tuple(f_feats.rows(), f_feats.cols()),
			bp::make_tuple(f_feats.cols() * sizeof(float), sizeof(float)),
			bp::object());


		np::ndarray t_array = np::from_data(t_feats.GetRow(0), np::dtype::get_builtin<float>(),
			bp::make_tuple(t_feats.rows(), t_feats.cols()),
			bp::make_tuple(t_feats.cols() * sizeof(float), sizeof(float)),
			bp::object());
		/*/-------------------DEBUG------------------------------------
		std::ofstream outFile_fn("fn.bin", std::ios::binary);
		if (!outFile_fn) {
			std::cerr << "Error opening file for writing!" << std::endl;
		}
		// Write the vector's raw binary data to the file
		int rows = f_feats.rows();
		int cols = f_feats.cols();
		outFile_fn.write(reinterpret_cast<const char*>(&rows), sizeof(rows));
		outFile_fn.write(reinterpret_cast<const char*>(&cols), sizeof(cols));
		outFile_fn.write(reinterpret_cast<const char*>(f_feats.GetRow(0)), f_feats.cols() * f_feats.rows() * sizeof(float));
		outFile_fn.close();

		std::ofstream outFile_tn("tn.bin", std::ios::binary);
		if (!outFile_tn) {
			std::cerr << "Error opening file for writing!" << std::endl;
		}
		// Write the vector's raw binary data to the file
		int tnrows = t_feats.rows();
		int tncols = t_feats.cols();
		outFile_tn.write(reinterpret_cast<const char*>(&tnrows), sizeof(tnrows));
		outFile_tn.write(reinterpret_cast<const char*>(&tncols), sizeof(tncols));
		outFile_tn.write(reinterpret_cast<const char*>(t_feats.GetRow(0)), t_feats.cols() * t_feats.rows() * sizeof(float));
		outFile_tn.close();
		//------------------------------------------------------------*/						

		try {
			TimeDiff	td;
			auto callable = corrector_.attr("fit");
			callable(t_array, f_array, numPCA_, numClusters_);
			std::cout << "Corrector train time: " << td.GetDiffMs() << " ms." << std::endl;
			//auto callable_correct = corrector_.attr("correct");

			//int value_far = bp::extract<int>(callable_correct(t_array));
			//int value_frr = bp::extract<int>(callable_correct(f_array));
			//std::cout << "Self python test for " << type << " corrector: FAR = " << value_far / (float)t_feats.rows() << "   FRR = " << 1 - (value_frr / (float)f_feats.rows()) << std::endl;
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
