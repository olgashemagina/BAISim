#pragma once

#include "agent.h"

#include "LF.h"
#include "LFEngine.h"
#include "LFFileUtils.h"


namespace agent {

	
	class TDBSupervisor : public ILFSupervisor
	{
	public:

		struct FileInfo {
			TLFString name;
			TDetections detections;

		};

		TDBSupervisor() {}
		virtual TDetections Detect(std::shared_ptr<TLFImage> img) override {
			return img_files_[cur_index_].detections;
		}
	
		std::shared_ptr<TLFImage> LoadImg(size_t index) {
			cur_index_ = index;
			std::shared_ptr<TLFImage> img = std::make_shared<TLFImage>();
			if (!img->LoadFromFile(img_files_[index].name.c_str())) {
				std::cout << "LoadFromFile return false for " << img_files_[index].name << std::endl;
			}
			return img;
		}

		int LoadDB(const TLFString& db_folder) {

			TLFStrings folder_files;
			if (!LFGetDirFiles(db_folder.c_str(), folder_files)) {
				std::cerr << "Could not read contents from " << db_folder << std::endl;
				return -101;
			}
			else {
				for (size_t i = 0; i < folder_files.size(); i++) {
					if (LFIsImageFile(folder_files[i].c_str())) {
						FileInfo fi;
						fi.name = folder_files[i];
						img_files_.push_back(fi);
					}
				}
			}
			folder_files.clear();
			std::cout << "Found " << img_files_.size() << " images. Loading... ";

			for (int i = 0; i < img_files_.size(); i++) {
						

				std::unique_ptr<TLFSemanticImageDescriptor> image_descriptor = std::make_unique<TLFSemanticImageDescriptor>();

				
				std::string xml_desc_name = LFChangeFileExt(img_files_[i].name, ".xml");
				if (LFFileExists(xml_desc_name)) {
					//We need to load image description
					if (!image_descriptor->LoadXML(xml_desc_name.c_str())) {
						std::cerr << "Cant load image description from " << xml_desc_name << std::endl;
						continue;
					}
					for (int j = 0; j < image_descriptor->GetCount(); j++) {
						auto obj = image_descriptor->Get(j);
						TLFDetectedItem* item = static_cast<TLFDetectedItem*>(obj);
						if (item != nullptr) {
							img_files_[i].detections.push_back(item->GetBounds());
						}
					}					
				}
			}
			std::cout << "Done." << std::endl;

			return img_files_.size();
		}
	private:
		std::vector<FileInfo> img_files_;
		size_t cur_index_ = 0;
	};
}
