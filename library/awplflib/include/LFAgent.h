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
#include "agent/corrector_collection.h"
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


public:
	void		Initialize(std::unique_ptr<agent::IDetector> detector,
		std::unique_ptr<agent::ICorrectorTrainer> trainer) {
		detector_ = std::move(detector);
		trainer_ = std::move(trainer);

	}


	virtual void SetSupervisor(std::shared_ptr<ILFSupervisor> sv) {
		supervisor_ = sv;
	}

	virtual std::vector<TLFDetectedItem> Detect(std::shared_ptr<TLFImage> img);

	virtual bool LoadXML(TiXmlElement* agent_node);

	virtual TiXmlElement* SaveXML();

protected:
	std::shared_ptr<ILFSupervisor>					supervisor_;
	std::unique_ptr<agent::IDetector>				detector_;

	std::unique_ptr<agent::ICorrectorTrainer>		trainer_;


	std::vector<std::unique_ptr<agent::IWorker>>	workers_;

	pool::ObjectPool<agent::TFeaturesBuilder>		pool_;

	agent::TCorrectorCollection						correctors_;

	// Prior detections
	std::vector<std::pair<TLFRect, float>>		    prior_detections_;

	// Size of Batch of fragments
	size_t		batch_size_ = 2048;

	// MUST be equal to count of batches used simultaneously
	int			max_threads_ = 32;

private:
	float											nms_threshold_ = 0.1f;
	
};

// Load Agent from Object Detection Engine;
std::unique_ptr<TLFAgent>       LoadAgentFromEngine(const std::string& engine_path);

// Compare ground truths (gt) and detections (dets)
// Returns FP and FN pair
static std::pair<int, int>		CalcStat(const agent::TDetections& gt, const std::vector<TLFDetectedItem>& dets, float overlap) {
	//Calc overlap matrix 
	int FP = dets.size();
	int FN = 0;
		
	unsigned char gt_mask[64] = {0};

	for (size_t d = 0; d < dets.size(); ++d) {
		unsigned char det_found = 0;
		FN = gt.size();
		for (size_t r = 0; r < gt.size(); ++r) {
			if (dets[d].GetBounds().RectOverlap(gt[r]) > overlap) {
				det_found = 1;
				gt_mask[r] = 1;
			}

			if (gt_mask[r])
				FN--;
		}

		if (det_found)
			FP--;

	}

	return { FP, FN };
}
