#pragma once

#include <numeric>
#include <memory>
#include <mutex>
#include <shared_mutex>

#include "agent/agent.h"



namespace agent {


	class TCorrectorCollection {
	public:
		bool LoadXML(TiXmlElement* collection_node) {
			
			for (TiXmlElement* child = collection_node->FirstChildElement(); child; child = child->NextSiblingElement()) {

				auto corrector = LoadCorrector(child);
				if (!corrector)
					return false;

				correctors_.emplace_back(std::move(corrector));
			}

			return false;
		}

		std::unique_ptr<ICorrector> LoadCorrector(TiXmlElement* corrector_node);

		TiXmlElement* SaveXML() {
			
			TiXmlElement* collection_node = new TiXmlElement("TCorrectorCollection");

			for (auto& corrector : correctors_) {
				auto corrector_node = corrector->SaveXML();
				collection_node->LinkEndChild(corrector_node);
			}

			return collection_node;
		}



		void			Apply(const TFeatures& features, std::vector<int>& corrections) {
			std::shared_lock<std::shared_mutex> lock(mutex_);

			std::fill(corrections.begin(), corrections.end(), kNoCorrection);
			for (const auto& corrector : correctors_) {
				corrector->Correct(features, corrections);
			}
		}


		void			AddCorrector(std::unique_ptr<ICorrector> corr) {
			std::unique_lock<std::shared_mutex> lock(mutex_);

			correctors_.emplace_back(std::move(corr));
		}
		void			AddCorrectors(std::vector<std::unique_ptr<ICorrector>> corrs) {
			std::unique_lock<std::shared_mutex> lock(mutex_);

			auto it = std::next(correctors_.begin(), correctors_.size());
			std::move(corrs.begin(), it, std::back_inserter(correctors_));
			//correctors_.insert(correctors_.end(), corrs.begin(), corrs.end());
		}


	private:
		std::shared_mutex									mutex_;
		std::vector<std::unique_ptr<ICorrector>>			correctors_;

	};
}


