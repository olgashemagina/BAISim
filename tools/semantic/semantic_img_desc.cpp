#pragma hdrstop
#pragma argsused

#ifdef _WIN32
#include <tchar.h>
#else
  typedef char _TCHAR;
  #define _tmain main
#endif

#include <iostream>
#include <string>
#include <vector>

#include "tinyxml.h"

#include "LF.h"
#include "LFEngine.h"
#include "LFFileUtils.h"

#include <memory>


void usage()
{
	std::cout << "Semantic Image Descriptor v1.0" << std::endl <<
		"Usage:" << std::endl << "<semantic_img_descriptor.exe> " <<
		"--det=<path_to_detector1> [--det=<path_to_detector2> ...] " <<
		"--db_folder=<path_to_img_directory>" << std::endl;
	std::cout << "\t--det\t\tPath to XML with detector (TLFDetectEngine)" <<
		std::endl << "\t--db_path\t\tPath to directory with images" <<
		std::endl << std::endl;
	std::cout << "Each detector will be executed on each image file in the " <<
		"directory and detected objects will be written to XML file " <<
		"(which is created in the same directory with images)." << std::endl;
}

int apply_detectors_to_folder(const TLFString& det_path,
	const TLFString &db_folder)
{
	// TODO: for some reason neither vector of instances nor vector of smart
	// pointers can be created, so extra error handling should be added later

	std::unique_ptr<TLFDetectEngine> det = std::make_unique<TLFDetectEngine>();
	if (!det->Load(det_path.c_str())) {
		std::cerr << "TLFDetectEngine couldn't parse file" <<
			det_path << std::endl;
		return -100;
	}


	std::vector<std::filesystem::path> folder_files, img_files;
	if (!LFGetDirFiles(db_folder.c_str(), folder_files)) {
		std::cerr << "Could not read contents from " << db_folder << std::endl;
		return -101;
	} else {
		for (size_t i = 0; i < folder_files.size(); i++) {
			if (LFIsImageFile(folder_files[i].u8string().c_str())) {
				img_files.push_back(folder_files[i]);
			}
		}
	}
	folder_files.clear();
	std::cout << "Found " << img_files.size() << " images" << std::endl;

	for (int i = 0; i < img_files.size(); i++) {
		std::cout << "Processing " << i + 1 << " out of " <<
			img_files.size() << std::endl;

		TLFImage img;
		if (!img.LoadFromFile(img_files[i].u8string().c_str())) {
			std::cerr << "Can't load image " << img_files[i] << std::endl;
			return -102;
		}

		TiXmlDocument img_res_doc;
		TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
		img_res_doc.LinkEndChild(decl);
		
		
		if (!det->SetSourceImage(&img, true)) {
			std::cerr << "Can't import image " << img_files[i] <<
				" to detector " << det->GetPredictorName() <<
				std::endl;
			return -103;
		}
		if (!det->FindObjects()) {
			std::cerr << "Eror when finding objects in " << img_files[i] <<
				" by detector " << det->GetPredictorName() <<
				std::endl;

			return -104;
		}

		auto semantic = det->GetSemantic();

		if (semantic) {
			img_res_doc.LinkEndChild(semantic->SaveXML());
		}
						
		
		TLFString results_file = img_files[i].replace_extension(".xml").u8string();

		if (!img_res_doc.SaveFile(results_file)) {
			std::cerr << "Can't save results to " << results_file << std::endl;
			return -105;
		}
	}

	return 0;
}

int main(int argc, char* argv[])
{
	TLFString det_path;
	TLFString db_folder_path("");

	// argv[0] is the launcher command, not parameter
	for (int i = 1; i < argc; i++) {
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
		} else if (key.substr(0, 11) == "--db_folder") {
			if (key.length() < 13 || key[11] != '=') {
				std::cerr << "Parsing error: use \"db_folder=<path>\", "
					"no extra spaces" << std::endl;
				usage();
				return -4;
			}
			if (db_folder_path.length() > 0) {
				std::cerr << "Database folder was already specified "
					"in parameters" << std::endl;
				usage();
				return -4;
			}
			db_folder_path = key.substr(12);
			std::cout << "Using database folder: " << db_folder_path <<
				std::endl;
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
	if (db_folder_path.length() == 0) {
		std::cerr << "Database folder was not provided" << std::endl;
        usage();
		return -4;
	}

	int res = apply_detectors_to_folder(det_path, db_folder_path);

	if (res == 0) {
		std::cout << "Done" << std::endl;
	} else {
		std::cerr << "Stopped due to error" << std::endl;
    }

	return res;
}
