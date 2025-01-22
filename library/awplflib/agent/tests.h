#pragma once

#include <numeric>
#include <memory>

#include "utils/object_pool.h"
#include "utils/matrix.h"
#include "utils/xml.h"
#include "agent/agent.h"
#include "agent/corrector_trainer.h"

#include "LFAgent.h"
#include "LFScanners.h"

namespace agent {

	static size_t GetRand(size_t low_dist, size_t high_dist) {
		return low_dist + std::rand() % (high_dist + 1 - low_dist);
	}

	class TRandomSupervisor : public ILFSupervisor
	{
	public:
		TRandomSupervisor() {
			std::srand((unsigned int)std::time(nullptr));
		}
		virtual TDetections Detect(std::shared_ptr<TLFImage> img) override {
			
			auto width = img->GetImage()->sSizeX;
			auto height = img->GetImage()->sSizeY;

			TDetections detections;
			for (size_t i = 0; i < 10; i++) {
				awpRect rect;
				rect.left = GetRand(0, width - 5);
				rect.right = GetRand(rect.left, width);
				rect.top = GetRand(0, height - 5);
				rect.bottom = GetRand(rect.top, height);
				detections.push_back(rect);
			}

			return detections;
		}

	};


	class TRandomDetector : public IDetector
	{
	public:
		TRandomDetector() {
			std::srand((unsigned int)std::time(nullptr));
			scanner_ = std::make_unique<TLFScanner>();
		}

		bool Initialize() {
						

			size_t cascade_count = GetRand(3, 10);  // rand ot 3 do 10
			size_t count = 0;
			for (auto i = 0; i < cascade_count; i++) {
				size_t feature_count = GetRand(10, 100); // rand ot 10 do 100
				feature_count_list_.push_back(feature_count + count);
				count = feature_count_list_.back();
			}
			
			return true;
		}

		void Detect(std::shared_ptr<TLFImage> img, TFeaturesBuilder& builder) {

			builder.Reset(feature_count_list_);

			float* data = builder.GetMutableData().GetRow(0);
			size_t stride = builder.feats_count();
						
			size_t detected = 0;

			for (size_t frag = builder.frags_begin(); frag < builder.frags_end(); ++frag) {
				auto row_data = builder.GetFeats(frag);
				// With probability 0.99 it is negative result
				if (GetRand(0, 100) > 1) {
					size_t cascade_number = GetRand(0, feature_count_list_.size() - 1);

					builder.SetTriggered(frag, cascade_number, 1);
															
					SetRandData(row_data, feature_count_list_.at(cascade_number));
				}
				else { // fill all cascade
					SetRandData(row_data, stride);
					detected++;
				}
			}

			std::thread::id this_id = std::this_thread::get_id();

			std::stringstream ss;

			ss << "Detection thread " << this_id
				<< " frags_begin=" << builder.frags_begin()
				<< " frags_end=" << builder.frags_end()
				<< " detected=" << detected << std::endl;

			(std::cout << ss.str()).flush();

		}

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

		virtual bool LoadXML(TiXmlElement* node)  {
			
			auto features = node->GetText();
			if (features) {
				std::istringstream ss(features);
				size_t value = 0;
				size_t count = 0;
				while (ss >> value) {
					count += value;
					feature_count_list_.push_back(count);
				}
			}
			return !feature_count_list_.empty();

		}

		virtual TiXmlElement* SaveXML()  override {
			TiXmlElement* node = new TiXmlElement("TRandomDetector");

			std::stringstream ss;

			size_t prev = 0;

			for (size_t i = 0; i < feature_count_list_.size() - 1; ++i) {
				ss << feature_count_list_[i] - prev << ' ';
				prev = feature_count_list_[i];
			}
			ss << feature_count_list_.back() - prev;

						
			node->LinkEndChild(new TiXmlText(ss.str()));

			return node;
		}


	private:
		static void SetRandData(float* data, size_t size) {
			for (size_t i = 0; i < size; i++)
				data[i] = float(GetRand(1, 100));
		}

	private:
		std::vector<size_t>				feature_count_list_;

		std::unique_ptr<ILFScanner>		scanner_;

	};


	class TRandomWorker : public IWorker {
	public:
		TRandomWorker(TRandomDetector* detector) : detector_(detector) {}

		virtual void Detect(std::shared_ptr<TLFImage> img, TFeaturesBuilder& builder) override {
			assert(detector_);
			detector_->Detect(img, builder);
		}

	private:
		TRandomDetector* detector_ = nullptr;
	};

	
}

namespace tests {

	static std::unique_ptr<TLFAgent> build_random_agent() {
		auto detector = std::make_unique<agent::TRandomDetector>();
				
		detector->Initialize();

		auto trainer = std::make_unique<agent::TCorrectorTrainerBase>();

		auto sv = std::make_shared<agent::TRandomSupervisor>();

		std::unique_ptr<TLFAgent>  agent = std::make_unique<TLFAgent>();
		agent->Initialize(std::move(detector), std::move(trainer));
		agent->SetSupervisor(sv);
		return std::move(agent);
	}

	

	static bool make_serialization_tests() {
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
}



