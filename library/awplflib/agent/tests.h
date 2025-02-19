#pragma once

#include <numeric>
#include <memory>

#include <iostream>
#include <fstream>

#include "utils/object_pool.h"
#include "utils/matrix.h"
#include "utils/xml.h"
#include "utils/time.h"
#include "agent/agent.h"
#include "agent/corrector_trainer.h"
#include "agent/correctors.h"
#include "agent/detectors.h"

#include "LFAgent.h"
#include "LFScanners.h"
#include "LFImage.h"
#include "LFDetector.h"
#include "LFStrong.h"

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

		// Type of detected objects
		virtual std::string_view GetType() const {
			return "RandomObject";
		}

		// Detector name
		virtual std::string_view GetName() const {
			return "TRandomDetector";
		}

		const TFragments& Setup(std::shared_ptr<TLFImage> img, const std::vector<TLFRect>* rois = nullptr) override;

		// Run detector and fill features and result of detections for fragments in range [begin, end]
		// Can be called in parallel for different fragments;
		bool Detect(TFeaturesBuilder& builder) const override;

		// Finalize processing image
		void Release() override {}

		virtual bool LoadXML(TiXmlElement* node);

		virtual TiXmlElement* SaveXML()  override;


	private:
		static void SetRandData(float* data, size_t size);

	private:
		TFragmentsBuilder				fragments_;

		std::vector<size_t>				feature_count_list_;

		std::unique_ptr<ILFScanner>		scanner_;

	};

}


namespace tests {
	static size_t GetRand(size_t low_dist, size_t high_dist) {
		return low_dist + std::rand() % (high_dist - low_dist);
	}

	std::unique_ptr<TLFAgent> build_random_agent();

	static std::shared_ptr<TLFImage>	create_empty_image(int width, int height, bool init_random = false) {
		awpImage* image = nullptr;
		awpCreateImage(&image, width, height, 3, AWP_BYTE);

		if (init_random) {
			AWPBYTE* pp = (AWPBYTE*)image->pPixels;
			size_t count = image->bChannels * image->sSizeX * image->sSizeY;
			for (size_t i = 0; i < count; ++i) {
				pp[i] = GetRand(0, 256);
			}
		}

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
		std::vector<int> corrections(vectors.size(), agent::kNoCorrection);

		for (int i = 0; i < data.rows(); i++) {
			corrections[i] = corrector->Process(data.GetRow(i), data.cols());
		}
		

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


		std::string test_xml_path = "../../test_corrector/out_data.xml";
		if (!save_xml(test_xml_path, corrector->SaveXML())) {
			return false;
		}
		auto corrector_test = load_xml(test_xml_path, [](TiXmlElement* node) {
			auto corrector = std::make_unique<agent::TBaselineCorrector>();
			if (node && corrector->LoadXML(node))
				return corrector;
			return std::unique_ptr<agent::TBaselineCorrector>{};
			});

		return true;
	}
	static bool test_build_correctors(const std::string& path)
	{
		std::string fppath = path + "FP_dron.bin";
		std::string tppath = path + "TP_dron.bin";
		// Open the binary file
		std::ifstream fpfile(fppath, std::ios::binary);

		// Check if the file is open
		if (!fpfile.is_open()) {
			std::cerr << "Error opening file!" << std::endl;
			return 1;
		}
		int w, h;
		fpfile.read(reinterpret_cast<char*>(&w), sizeof(w));
		fpfile.read(reinterpret_cast<char*>(&h), sizeof(h));

		std::vector<float> fp(w*h);

		// Read the entire binary data into the vector
		fpfile.read(reinterpret_cast<char*>(fp.data()), w*h * sizeof(float));

		// Close the file
		fpfile.close();
		const TMatrix fp_matrix(h, w, fp);


		std::ifstream tpfile(tppath, std::ios::binary);

		// Check if the file is open
		if (!tpfile.is_open()) {
			std::cerr << "Error opening file!" << std::endl;
			return 1;
		}
		int wb, hb;
		tpfile.read(reinterpret_cast<char*>(&wb), sizeof(wb));
		tpfile.read(reinterpret_cast<char*>(&hb), sizeof(hb));

		std::vector<float> tp(wb * hb);

		// Read the entire binary data into the vector
		tpfile.read(reinterpret_cast<char*>(tp.data()), wb * hb * sizeof(float));

		// Close the file
		tpfile.close();
		const TMatrix tp_matrix(hb, wb, tp);

		auto corrector = agent::CreateBaselineTrainer("create_class_corr1.py", "");
		
		corrector->TrainFpCorrector(fp_matrix, tp_matrix);
	}


	static std::unique_ptr<TSCObjectDetector>			build_random_detector(float activate_prob = 0.1f, int num_features = 10, int num_stages = 1, int base_width = 24, int base_height = 24) {
		auto detector = std::make_unique<TSCObjectDetector>();

		TLFObjectList* strongs = detector->GetStrongs();

		detector->SetBaseWidht(base_width);
		detector->SetBaseHeight(base_height);
		auto scanner = new TLFScanner();

		scanner->SetBaseWidth(base_width);
		scanner->SetBaseHeight(base_height);

		detector->SetScanner(scanner);

		//float weak_prob = std::powf(activate_prob, 1.f / float(num_features));

		for (int s = 0; s < num_stages; ++s) {

			TCSStrong* strong = new TCSStrong;
			strongs->Add(strong);

			strong->SetThreshold(1.0);

			for (int f = 0; f < num_features; ++f) {

				
				int w = GetRand(1, base_width / 3);
				int h = GetRand(1, base_height / 3);

				int x = GetRand(0, base_width - w * 3);
				int y = GetRand(0, base_height - h * 3);


				auto sensor = LFCreateFeature("CSFeature", x, y, w, h);

				TCSWeak* weak = new TCSWeak(sensor);
				strong->AddWeak(weak);
				weak->SetWeight(1.0 / double(num_features));
				int activated = 0;
				for (int i = 0; i < 512; ++i) {
					AWPBYTE activation = GetRand(0, 100) < activate_prob * 100 ? 1 : 0;
					if (activation)
						activated++;
					weak->SetClassificator(i, activation);
				}
				
				//std::cout << "Activated patterns: " << activated << std::endl;

			}
			
		}
		return detector;
	}

	static bool test_agents_sequence() {
		// Make random HD image
		auto img = create_empty_image(1280, 720, true);
		//auto img = create_empty_image(320, 240, true);

		//static_assert(noexcept(TLFAgent()), "!");

		std::vector<std::unique_ptr<TLFAgent>>			agents;

		for (size_t i = 0; i < 100; ++i) {
			agents.emplace_back(std::make_unique<TLFAgent>());

			auto detector_base = build_random_detector(0.6, 10);

			auto detector = std::make_unique<agent::TStagesDetector>();
			if (!detector->Initialize(std::move(detector_base), 0)) {
				std::cout << "Cant initialize TStagesDetector" << std::endl;
			}

			agents.back()->Initialize(std::move(detector), nullptr);
			agents.back()->SetNmsThreshold(0.7);
		}

		TimeDiff	td_total(true);

		agent::TDetections		detections;
		for (size_t a = 0; a < agents.size(); ++a) {

			TimeDiff	td;
			auto items = agents[a]->Detect(img, &detections);

			std::cout << "Agent " << a + 1 << " detected " << items.size() << " items in " << detections.size() << " rois for " << td.GetDiffMs()  << " ms." << std::endl;

			//if (items.size() > 5) {
			if (detections.empty()) {
				detections.clear();

				for (const auto& item : items) {
					auto rect = item.GetBounds();
					// Increase size of rect;
					//rect.Inflate(rect.Width() * 0.1, rect.Height() * 0.1);
					detections.push_back(rect);
				}
			}

		}

		return true;
	}

	/// <summary>
	///  Agent Tree test
	/// </summary>

	struct TNode {
		size_t					num_nodes = 0;
		TLFAgent				agent;

		std::unique_ptr<TNode>	left;
		std::unique_ptr<TNode>	right;


		void Detect(std::shared_ptr<TLFImage> img, const std::vector<TLFRect>* rois = nullptr) {
			TimeDiff	td;
			agent::TDetections		detections;

			auto items = agent.Detect(img, rois);

			std::cout << "Agent " << num_nodes << " detected " << items.size() << " items for " << td.GetDiffMs() << " ms." << std::endl;

			if (items.size()) {

				for (const auto& item : items) {
					auto rect = item.GetBounds();
					// Increase size of rect;
					//rect.Inflate(rect.Width() * 0.1, rect.Height() * 0.1);
					detections.push_back(rect);
				}

				if (left)
					left->Detect(img, &detections);

				if (right)
					right->Detect(img, &detections);
			}
		}
	};

	static std::unique_ptr<TNode>	build_agents_tree(int levels, float activate_prob = 0.7f, int num_features = 10) {
		if (levels <= 0)
			return nullptr;

		auto root = std::make_unique<TNode>();

		auto detector_base = build_random_detector(activate_prob, num_features);

		auto detector = std::make_unique<agent::TStagesDetector>();
		if (!detector->Initialize(std::move(detector_base), 0)) {
			std::cout << "Cant initialize TStagesDetector" << std::endl;
		}

		root->agent.Initialize(std::move(detector), nullptr);
		root->agent.SetNmsThreshold(0.5);

		root->left = build_agents_tree(levels - 1);
		root->right = build_agents_tree(levels - 1);

		if (root->left)
			root->num_nodes += root->left->num_nodes;

		if (root->right)
			root->num_nodes += root->right->num_nodes;

		root->num_nodes++;

		return std::move(root);
	}


	static bool test_agents_tree() {
		// Make random HD image
		auto img = create_empty_image(1280, 720, true);
		//auto img = create_empty_image(320, 240, true);

		std::cout << "Input image size " << img->width() << "x" << img->height() << std::endl;
		
		auto root = build_agents_tree(13);

		std::cout << "Agents count " << root->num_nodes << std::endl;

		TimeDiff	td_total(true);

		root->Detect(img);

		return true;
	}
}