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

	struct TItem {
		TLFDetectedItem			detected;
		bool					is_corrected = false;
	};

	using item_t = TItem;


public:
	TLFAgent() noexcept;

	TLFAgent(const TLFAgent&) = delete;

	TLFAgent(TLFAgent&& other) noexcept = default;
	
	virtual ~TLFAgent() = default;

	const TLFAgent& operator=(const TLFAgent&) = delete;

	TLFAgent& operator= (TLFAgent&&) noexcept = default;

public:
	void		Initialize(std::unique_ptr<agent::IDetector> detector,
		std::unique_ptr<agent::ICorrectorTrainer> trainer) {
		detector_ = std::move(detector);
		trainer_ = std::move(trainer);

	}

	void	SetNmsThreshold(float threshold) { nms_threshold_ = threshold; }

	void	SetBatchSize(size_t size) { batch_size_ = size; }
	void	SetMaxThreads(size_t size) { max_threads_ = size; }


	virtual void SetSupervisor(std::shared_ptr<ILFSupervisor> sv) {
		supervisor_ = sv;
	}

	// Detect objects on image using agent;
	virtual std::vector<item_t> Detect(std::shared_ptr<TLFImage> img, const std::vector<TLFRect>* rois = nullptr);

	// Advanced method that used rois  for fragments and supervised detections;
	// If supervised is nullptr then do not using trainer for new correctors;\
	// If rois is nullptr then all image used for detections
	//virtual std::vector<TLFDetectedItem> Detect(TLFImageContainer img, const std::vector<TLFRect>* supervised = nullptr, const std::vector<TLFRect>*rois = nullptr );

	virtual bool LoadXML(TiXmlElement* agent_node);

	virtual TiXmlElement* SaveXML();

private:
	struct TPrior {
		// Rect of prior
		TLFRect			rect;
		// Score of prior
		float			score;
		// Correceted flag
		bool			is_corrected = false;
	};

	struct TItemAttributes;

	static std::vector<item_t> NonMaximumSuppression(
		std::vector<TPrior>& rects,
		float overlap_threshold, const TItemAttributes& attrs);


protected:
	std::shared_ptr<ILFSupervisor>					supervisor_;
	std::unique_ptr<agent::IDetector>				detector_;

	std::unique_ptr<agent::ICorrectorTrainer>		trainer_;
			

	pool::ObjectPool<agent::TFeaturesBuilder>		pool_;

	agent::TCorrectorCollection						correctors_;

	

	// Prior detections
	std::vector<TPrior>		    prior_detections_;

	// Size of Batch of fragments
	size_t		batch_size_ = 2048;

	// MUST be equal to count of batches used simultaneously
	int			max_threads_ = 32;

private:
	float											nms_threshold_ = 0.1f;
	
};

// Load Agent from Object Detection Engine;
std::unique_ptr<TLFAgent>       LoadAgentFromEngine(const std::string& engine_path, bool use_gpu = false);

// Compare ground truths (gt) and detections (dets)
// Returns FP and FN pair
std::pair<int, int>		CalcStat(const agent::TDetections& gt, const std::vector<TLFAgent::item_t>& dets, float overlap);
