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
	

int merge_detector(const TLFString& tree_path, const TLFString& det_path,
	const std::vector<TLFString>& tree_path_det) {

	TLFTreeEngine			tree_engine;

	tree_engine.Load(tree_path);

	std::shared_ptr<TLFDetectEngine> det = std::make_shared<TLFDetectEngine>();
	if (!det->Load(det_path.c_str())) {
		std::cerr << "TLFDetectEngine couldn't parse file" <<
			det_path << std::endl;
		return -100;
	}

	for (const auto& path : tree_path_det) {
		if (!tree_engine.SetDetector(path, std::shared_ptr<ILFObjectDetector>(det, det->GetDetector(0))))
			return -101;
	}

	if (!tree_engine.Save(tree_path))
		return -102;

	return 0;
}

int export_detector(const TLFString& tree_path, const TLFString& det_path,
	const TLFString& tree_path_det) {

	TLFTreeEngine			tree_engine;

	if (!tree_engine.Load(tree_path))
		return -100;


	auto det = tree_engine.GetDetector(tree_path_det);

	if (det) {
		std::cerr << "TLFTreeEngine cant find path in tree " <<
			tree_path_det << std::endl;
		return -101;
	}

	std::shared_ptr<TLFDetectEngine> det_engine = std::make_shared<TLFDetectEngine>();

	det_engine->AddDetector(det.get());

	if (!det_engine->Save(det_path.c_str())) {
		std::cerr << "TLFDetectEngine couldn't write file" <<
			det_path << std::endl;
		return -102;
	}

	return 0;
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
			return 0;
		}
		if (key.substr(0, 5) == "--det") {
			if (key.length() < 7 || key[5] != '=') {
				std::cerr << "Parsing error: use \"det=<path>\", "
					"no extra spaces" << std::endl;
				usage();
				return -3;
			}
			if (!det_path.empty()) {
				std::cerr << "Engine file was already specified "
					"in parameters" << std::endl;
				usage();
				return -4;
			}
			det_path = key.substr(6);
		} else if (key.substr(0, 6) == "--tree") {
			if (key.length() < 8 || key[6] != '=') {
				std::cerr << "Parsing error: use \"tree=<path>\", "
					"no extra spaces" << std::endl;
				usage();
				return -3;
			}
			if (!tree_path.empty()) {
				std::cerr << "Engine file was already specified "
					"in parameters" << std::endl;
				usage();
				return -4;
			}
			tree_path = key.substr(7);
		}
		else if (key.substr(0, 11) == "--tree_path") {
			if (key.length() < 13 || key[11] != '=') {
				std::cerr << "Parsing error: use \"tree_path=<path>\", "
					"no extra spaces" << std::endl;
				usage();
				return -4;
			}

			tree_path_det.push_back(key.substr(12));
		
		} else {
			std::cerr << "Unknown key: " << key << std::endl;
			usage();
			return -2;
		}
	}
	if (det_path.size() == 0) {
		std::cerr << "No detector provided" << std::endl;
		usage();
		return -3;
	} else {
		std::cout << "Using detector: " << det_path << std::endl;
	}
	
	int res =	(command == "merge") ? merge_detector(tree_path, det_path, tree_path_det) : 
				(command == "export") ? export_detector(tree_path, det_path, tree_path_det[0]) : -1;
		

	if (res == 0) {
		std::cout << "Done" << std::endl;
	} else {
		std::cerr << "Stopped due to error" << std::endl;
    }

	return res;
}
