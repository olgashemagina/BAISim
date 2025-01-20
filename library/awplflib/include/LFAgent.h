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

class TLFAgent {
public:
	TLFAgent();
	
	virtual ~TLFAgent() = default;

	virtual void SetSupervisor(std::shared_ptr<ILFSupervisor> sv) {
		supervisor_ = sv;
	}

	virtual std::unique_ptr<TLFSemanticImageDescriptor> Detect(std::shared_ptr<TLFImage> img);

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

	// Indices of detected fragments
	std::vector<size_t>								detected_indices_;

	size_t		batch_size_ = 2048;

	// MUST be equal to count of batches used simultaneously
	int			max_threads_ = 32;

};
