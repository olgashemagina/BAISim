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

void setup_callback(ILFObjectDetector* detector, TLFSemanticImageDescriptor* descriptor, TiXmlNode* node, double overlap = 0.5, double negative_threshold = 0.25) {
	//Mutex if OMP threading used.
	auto mtx = std::make_shared<std::mutex>();
	detector->SetDescCallback([=](size_t index,
		const auto& bounds,
		const auto& desc) {
			double iou = 0;
			for (int i = 0; i < descriptor->GetCount(); i++)
			{
				TLFDetectedItem* item = descriptor->GetDetectedItem(i);
				TLFRect* rect = item->GetBounds();

				//TODO: detector untyped
				//if (detector->GetObjectType() == item->GetType()) 
				{

					iou = rect->RectOverlap(bounds.Rect);

					if (iou >= overlap) {
						break;
					}
				}
			}

			std::unique_ptr<TiXmlElement> sample(desc.SaveXML());

			sample->SetAttribute("left", bounds.Rect.left);
			sample->SetAttribute("top", bounds.Rect.top);
			sample->SetAttribute("right", bounds.Rect.right);
			sample->SetAttribute("bottom", bounds.Rect.bottom);
			sample->SetDoubleAttribute("IOU", iou);

			if (desc.result) {
				if (iou < overlap) {
					//False Positive
					sample->SetAttribute("SampleTest", "FP");
				}
				else {
					//True Positive
					sample->SetAttribute("SampleTest", "TP");
				}
			}
			else {
				if (iou > overlap) {
					//False Negative
					sample->SetAttribute("SampleTest", "FN");
				}
				else if (iou > negative_threshold) {
					//True Negatives
					sample->SetAttribute("SampleTest", "TN");
				}
				else {
					sample.reset();
				}
			}

			
			if (sample) {
				std::lock_guard<std::mutex> locker(*mtx);
				node->LinkEndChild(sample.release());
			}
			
						
		});
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


	TLFStrings folder_files, img_files;
	if (!LFGetDirFiles(db_folder.c_str(), folder_files)) {
		std::cerr << "Could not read contents from " << db_folder << std::endl;
		return -101;
	} else {
		for (size_t i = 0; i < folder_files.size(); i++) {
			if (LFIsImageFile(folder_files[i].c_str())) {
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
		if (!img.LoadFromFile(img_files[i].c_str())) {
			std::cerr << "Can't load image " << img_files[i] << std::endl;
			return -102;
		}

		std::unique_ptr<TLFSemanticImageDescriptor> image_descriptor = std::make_unique<TLFSemanticImageDescriptor>();

		TiXmlDocument samples_doc;
		samples_doc.LinkEndChild(new TiXmlDeclaration("1.0", "", ""));

		std::string xml_desc_name = LFChangeFileExt(img_files[i], ".xml");
		if (LFFileExists(xml_desc_name)) {
			//We need to load image description
			if (!image_descriptor->LoadXML(xml_desc_name.c_str())) {
				std::cerr << "Cant load image description from " << xml_desc_name << std::endl;
				continue;
			}
			//Setup callback of descriptor
			auto detector = det->GetDetector(0);

			TiXmlElement* samples = new TiXmlElement("LFSamples");
					
			//samples->SetAttribute("ImagePath", img_files[i]);

			setup_callback(detector, image_descriptor.get(), samples);

			samples_doc.LinkEndChild(samples);
		}

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

		//Clear Callback;
		det->GetDetector(0)->SetDescCallback(nullptr);

		//No description of image;
		if (image_descriptor->GetCount() == 0) {
			TiXmlDocument img_res_doc;
			TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
			img_res_doc.LinkEndChild(decl);

			auto semantic = det->GetSemantic();

			if (semantic) {
				img_res_doc.LinkEndChild(semantic->SaveXML());
			}

			TLFString results_file = LFChangeFileExt(img_files[i], ".xml");

			if (!img_res_doc.SaveFile(results_file)) {
				std::cerr << "Can't save results to " << results_file << std::endl;
				return -105;
			}
		}
		else {
			std::string xml_sample_name = LFChangeFileExt(img_files[i], "_samples.xml");
			if (!samples_doc.SaveFile(xml_sample_name)) {
				std::cerr << "Can't save results to " << xml_sample_name << std::endl;
				return -106;
			}
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
