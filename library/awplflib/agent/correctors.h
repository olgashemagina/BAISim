#pragma once

#include <numeric>
#include <memory>
#include <mutex>
#include <shared_mutex>

#include "agent/agent.h"



namespace agent {


	class TCorrectorBaseline : public ICorrector {
	public:
		bool LoadXML(TiXmlElement* parent) {
			// TODO: load detector and all correctors
			return false;
		}


		// Correct result of detector using features.
		virtual void Correct(const TFeatures& features, std::vector<int>& corrections) {

		}

		// Serializing methods.
		//virtual bool LoadXML(TiXmlElement* parent) = 0;
		virtual TiXmlElement* SaveXML() {
			return nullptr;
		}

	private:
		

	};
}



