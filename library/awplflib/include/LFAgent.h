#pragma once

#include "LFImage.h"
#include "LF.h"

#include <numeric>
#include <memory>
#include <mutex>
#include <shared_mutex>

#include "utils/object_pool.h"
#include "utils/matrix.h"
#include "agent/agent.h"
#include "agent/correctors.h"
#include "agent/features.h"

class ILFSupervisor {

public:
	virtual ~ILFSupervisor() = default;

	// Detect supervised objects on image.
	virtual agent::TDetections Detect(std::shared_ptr<TLFImage> img) = 0;
};


struct TItemAttributes {
	// Detected object type
	std::string			type;
	int					base_width = 24;
	int					base_height = 24;
	int					angle = 0;
	int					racurs = 0;
	// Name of detector
	std::string			name;
};

class TLFAgent {

public:
	TLFAgent();
	
	virtual ~TLFAgent() = default;

	virtual void SetSupervisor(std::shared_ptr<ILFSupervisor> sv) {
		supervisor_ = sv;
	}

	virtual std::vector<TLFDetectedItem> Detect(std::shared_ptr<TLFImage> img);

	virtual bool LoadXML(TiXmlElement* parent) {
		// TODO: load detector and all correctors
		return false;
	}

	virtual TiXmlElement* SaveXML() {
		// TODO: save detector and all correctors
		return nullptr;
	}

protected:
	std::shared_ptr<ILFSupervisor>					supervisor_;
	std::unique_ptr<agent::IDetector>				detector_;

	std::unique_ptr<agent::ICorrectorTrainer>		trainer_;


	std::vector<std::unique_ptr<agent::IWorker>>	workers_;

	pool::ObjectPool<agent::TFeaturesBuilder>		pool_;

	agent::TCorrectors								correctors_;

	// Prior detections
	std::vector<std::pair<TLFRect, float>>		    prior_detections_;

	size_t		batch_size_ = 2048;

	// MUST be equal to count of batches used simultaneously
	int			max_threads_ = 32;

private:
	float											overlap_threshold_ = 0.1f;
	TItemAttributes									item_attrs_;

};
