#pragma hdrstop
#pragma argsused

#ifdef _WIN32
#include <tchar.h>
#else
  typedef char _TCHAR;
  #define _tmain main
#endif

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <numeric>

#include "tinyxml.h"

#include "LF.h"
#include "LFEngine.h"
#include "LFFileUtils.h"

#include <memory>
#include <mutex>

//merge --tree="../../../../models/railway/rw_tree.xml" --det="../../../../models/railway/rtank_1224.xml" --tree_path="RailwayTank" --tree_path="RailwayTank2"

//export --tree="../../../../models/railway/rw_tree.xml" --det="../../../../models/railway/rtank_exported.xml" --tree_path="RailwayTank"

  //merge --tree="../../../../models/railway/rw_tree.xml" --det="../../../../models/railway/rtank_1224.xml" --tree_path="RailwayTank"
  //merge --tree="../../../../models/railway/rw_tree.xml" --det="../../../../models/railway/Rtank_Num_Digits/rnum_cs.xml" --tree_path="RailwayTank.Numbers"
  //merge --tree="../../../../models/railway/rw_tree.xml" --det="../../../../models/railway/Rtank_Num_Digits/0_075_v3.xml" --tree_path="RailwayTank.Numbers.Digit0"
  //merge --tree="../../../../models/railway/rw_tree.xml" --det="../../../../models/railway/Rtank_Num_Digits/1_075_v3.xml" --tree_path="RailwayTank.Numbers.Digit1"
  //merge --tree="../../../../models/railway/rw_tree.xml" --det="../../../../models/railway/Rtank_Num_Digits/2_075_v3.xml" --tree_path="RailwayTank.Numbers.Digit2"
  //merge --tree="../../../../models/railway/rw_tree.xml" --det="../../../../models/railway/Rtank_Num_Digits/3_075_v3.xml" --tree_path="RailwayTank.Numbers.Digit3"
  //merge --tree="../../../../models/railway/rw_tree.xml" --det="../../../../models/railway/Rtank_Num_Digits/4_075_v3.xml" --tree_path="RailwayTank.Numbers.Digit4"
  //merge --tree="../../../../models/railway/rw_tree.xml" --det="../../../../models/railway/Rtank_Num_Digits/5_075_v3.xml" --tree_path="RailwayTank.Numbers.Digit5"
  //merge --tree="../../../../models/railway/rw_tree.xml" --det="../../../../models/railway/Rtank_Num_Digits/6_075_v3.xml" --tree_path="RailwayTank.Numbers.Digit6"
  //merge --tree="../../../../models/railway/rw_tree.xml" --det="../../../../models/railway/Rtank_Num_Digits/7_075_v3.xml" --tree_path="RailwayTank.Numbers.Digit7"
  //merge --tree="../../../../models/railway/rw_tree.xml" --det="../../../../models/railway/Rtank_Num_Digits/8_075_v3.xml" --tree_path="RailwayTank.Numbers.Digit8"
  //merge --tree="../../../../models/railway/rw_tree.xml" --det="../../../../models/railway/Rtank_Num_Digits/9_075_v3.xml" --tree_path="RailwayTank.Numbers.Digit9"


enum EStatus {
	kStatus_Success = 0,
	kStatus_WrongPath,
	kStatus_WrongArgument,

};


void usage()
{
	std::cout << "Tree Builder v1.0" << std::endl <<
		"Usage:" << std::endl << "<tree_builder.exe> " <<
		"export|merge  " <<
		"--tree=<path_to_tree_engine> " <<
		"--det=<path_to_detector> " <<
		"--tree_path=<path_in_tree> [--tree_path=<path_in_tree> ...] " <<
		std::endl;

	std::cout << "\t--tree\t\tPath to XML with Tree Engine (TLFTreeEngine)" <<
		std::endl << "\t--det\t\tPath to XML with Engine (TLFDetectEngine)" <<
		std::endl << "\t--tree_path\t\tPath in Tree Engine as Name1.Name2..." <<
		std::endl << std::endl;
}
	

static EStatus merge_detector(const TLFString& tree_path, const TLFString& det_path,
	const std::vector<TLFString>& tree_path_det) {

	TLFTreeEngine			tree_engine;

	if (!tree_engine.Load(tree_path)){
		std::cerr << "Cant load Tree Engine. Creating empty one." << std::endl;
	}

	std::shared_ptr<TLFDetectEngine> det = std::make_shared<TLFDetectEngine>();
	if (!det->Load(det_path.c_str())) {
		std::cerr << "TLFDetectEngine couldn't parse file" <<
			det_path << std::endl;
		return kStatus_WrongPath;
	}

	for (const auto& path : tree_path_det) {
		if (!tree_engine.SetDetector(path, std::shared_ptr<ILFObjectDetector>(det, det->GetDetector(0))))
			return kStatus_WrongPath;
	}

	if (!tree_engine.Save(tree_path))
		return kStatus_WrongPath;

	return kStatus_Success;
}

static EStatus export_detector(const TLFString& tree_path, const TLFString& det_path,
	const TLFString& tree_path_det) {

	auto tree_engine = std::make_unique<TLFTreeEngine>();

	if (!tree_engine->Load(tree_path))
		return kStatus_WrongPath;


	auto det = tree_engine->GetDetector(tree_path_det);

	if (!det) {
		std::cerr << "TLFTreeEngine cant find path in tree " <<
			tree_path_det << std::endl;
		return kStatus_WrongPath;
	}

	//Delete engine to hold pointer to detector det;
	tree_engine.reset();

	auto det_engine = std::make_unique<TLFDetectEngine>();


	det_engine->AddDetector(det.get());
		
	if (!det_engine->Save(det_path.c_str())) {
		std::cerr << "TLFDetectEngine couldn't write file" <<
			det_path << std::endl;
		return kStatus_WrongPath;
	}

	det_engine->RemoveDetector(0);

	return kStatus_Success;
}


int main(int argc, char* argv[])
{
	TLFString det_path;
	TLFString tree_path;
	std::vector<TLFString> tree_path_det;

	if (argc < 3) {
		usage();
		return -1;
	}

	TLFString command(argv[1]);
	// argv[0] is the launcher command, not parameter
	for (int i = 2; i < argc; i++) {
		TLFString key(argv[i]);
		if (key.substr(0, 6) == "--help" || key.substr(0, 2) == "-h") {
			usage();
			return kStatus_Success;
		}
		if (key.substr(0, 6) == "--det=") {
			if (key.length() < 7 || key[5] != '=') {
				std::cerr << "Parsing error: use \"det=<path>\", "
					"no extra spaces" << std::endl;
				usage();
				return kStatus_WrongArgument;
			}
			if (!det_path.empty()) {
				std::cerr << "Engine file was already specified "
					"in parameters" << std::endl;
				usage();
				return kStatus_WrongArgument;
			}
			det_path = key.substr(6);
		} else if (key.substr(0, 7) == "--tree=") {
			if (key.length() < 8 || key[6] != '=') {
				std::cerr << "Parsing error: use \"tree=<path>\", "
					"no extra spaces" << std::endl;
				usage();
				return kStatus_WrongArgument;
			}
			if (!tree_path.empty()) {
				std::cerr << "Engine file was already specified "
					"in parameters" << std::endl;
				usage();
				return kStatus_WrongArgument;
			}
			tree_path = key.substr(7);
		}
		else if (key.substr(0, 12) == "--tree_path=") {
			if (key.length() < 13 || key[11] != '=') {
				std::cerr << "Parsing error: use \"tree_path=<path>\", "
					"no extra spaces" << std::endl;
				usage();
				return kStatus_WrongArgument;
			}

			tree_path_det.push_back(key.substr(12));
		
		} else {
			std::cerr << "Unknown key: " << key << std::endl;
			usage();
			return kStatus_WrongArgument;
		}
	}
	if (det_path.size() == 0) {
		std::cerr << "No detector provided" << std::endl;
		usage();
		return kStatus_WrongArgument;
	} else {
		std::cout << "Using detector: " << det_path << std::endl;
	}
	
	EStatus res =	(command == "merge") ? merge_detector(tree_path, det_path, tree_path_det) : 
				(command == "export") ? export_detector(tree_path, det_path, tree_path_det[0]) : kStatus_WrongArgument;
		

	if (res == kStatus_Success) {
		std::cout << "Done" << std::endl;
	} else {
		std::cerr << "Stopped due to error" << std::endl;
    }

	return res;
}
