#pragma once

#include <numeric>
#include <memory>

#include <iostream>
#include <fstream>

#include "utils/object_pool.h"
#include "utils/matrix.h"
#include "utils/xml.h"
#include "agent/agent.h"
#include "agent/corrector_trainer.h"
#include "agent/correctors.h"

#include "LFAgent.h"
#include "LFScanners.h"
#include "LFImage.h"

namespace agent {

	class TRandomSupervisor : public ILFSupervisor
	{
	public:
		TRandomSupervisor() {
			std::srand((unsigned int)std::time(nullptr));
		}
		virtual TDetections Detect(std::shared_ptr<TLFImage> img) override;

	};


	class TRandomDetector : public IDetector
	{
	public:
		TRandomDetector() {
			std::srand((unsigned int)std::time(nullptr));
			scanner_ = std::make_unique<TLFScanner>();
		}

		bool Initialize();

		void Detect(std::shared_ptr<TLFImage> img, TFeaturesBuilder& builder);

		// Type of detected objects
		virtual std::string_view GetType() const {
			return "RandomObject";
		}

		// Detector name
		virtual std::string_view GetName() const {
			return "TRandomDetector";
		}

		virtual ILFScanner* GetScanner()  override {
			return scanner_.get();
		}

		std::unique_ptr<IWorker> CreateWorker() override;

		virtual bool LoadXML(TiXmlElement* node);

		virtual TiXmlElement* SaveXML()  override;


	private:
		static void SetRandData(float* data, size_t size);

	private:
		std::vector<size_t>				feature_count_list_;

		std::unique_ptr<ILFScanner>		scanner_;

	};


	class TRandomWorker : public IWorker {
	public:
		TRandomWorker(TRandomDetector* detector) : detector_(detector) {}

		virtual void Detect(std::shared_ptr<TLFImage> img, TFeaturesBuilder& builder) override;

	private:
		TRandomDetector* detector_ = nullptr;
	};
}


namespace tests {

	std::unique_ptr<TLFAgent> build_random_agent();

	static std::shared_ptr<TLFImage>	create_empty_image(int width, int height) {
		awpImage* image = nullptr;
		awpCreateImage(&image, 1280, 720, 3, AWP_BYTE);

		std::shared_ptr<TLFImage> img = std::make_shared<TLFImage>();
		img->SetImage(image);
		return img;

	}
		

	static bool test_serialization() {
		auto agent = build_random_agent();

		save_xml("test_agent.xml", agent->SaveXML());

		auto agent_loaded = load_xml("test_agent.xml", [](TiXmlElement* node) {
			auto  agent = std::make_unique<TLFAgent>();
			if (node && agent->LoadXML(node))
				return agent;
			return std::unique_ptr<TLFAgent>{};
			});

		save_xml("test_agent_2.xml", agent_loaded->SaveXML());

		auto agent_1 = agent->SaveXML();
		auto agent_2 = agent_loaded->SaveXML();

		auto result = compare_xml_elements(agent_1, agent_2);

		delete agent_1;
		delete agent_2;
		return result;

	}

	static bool test_random_agent() {
		auto agent = build_random_agent();

		if (!agent)
			return false;

		// Setup random image
		auto img = create_empty_image(1280, 720);

		for (int i = 0; i < 100; ++i) {
			auto dets = agent->Detect(img);
		}
		return true;
	}

	// Tests importing external detector
	static bool test_import_detector(const std::string& path) {

		auto agent = LoadAgentFromEngine(path);

		if (!agent)
			return false;

		auto image = create_empty_image(1280, 720);

		for (int i = 0; i < 100; ++i) {
			auto dets = agent->Detect(image);
			std::cout << "Detections count " << dets.size() << std::endl;
		}
		return true;

	}

	static bool test_baseline_corrector() {
		auto corrector = load_xml("../../test_corrector/data.xml", [](TiXmlElement* node) {
			auto corrector = std::make_unique<agent::TBaselineCorrector>();
			if (node && corrector->LoadXML(node))
				return corrector;
			return std::unique_ptr<agent::TBaselineCorrector>{};
			});
		std::ifstream file("../../test_corrector/features.txt");
		if (!file) {
			return false;
		}
		
		std::vector<std::vector<float>> vectors;
		std::string line;
		while (std::getline(file, line))
		{
			std::istringstream ss(line);
			std::vector<float> new_vec;
			float v;
			while (ss >> v)
			{
				new_vec.push_back(v);
			}
			vectors.push_back(new_vec);
		}
		file.close();
		TMatrix data(0, vectors[0].size());
		for (int i = 0; i < vectors.size(); i++) {
			data.AddRow(&(vectors[i][0]), vectors[i].size());
		}
		std::vector<int> corrections;
		corrector->Process(data, corrections);

		std::ifstream file_prediction("../../test_corrector/prediction.txt");
		if (!file_prediction) {
			return false;
		}

		std::vector<std::vector<float>> vectors_prediction;
		std::string line_prediction;
		int i = 0;
		while (std::getline(file_prediction, line_prediction))
		{
			std::istringstream ss(line_prediction);
			std::vector<float> new_vec;
			int v;
			while (ss >> v)
			{
				if (corrections[i] != v) {
					return false;
				}
			}
			i++;
			vectors_prediction.push_back(new_vec);
		}
		file_prediction.close();


		if (!save_xml("../../test_corrector/out_data.xml", corrector->SaveXML())) {
			return false;
		}
		return true;
	}
}



