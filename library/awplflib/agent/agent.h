#pragma once

#include "LFImage.h"
#include "LF.h"

#include <numeric>
#include <memory>
#include <mutex>
#include <shared_mutex>

#include "utils/object_pool.h"
#include "utils/matrix.h"
#include "agent/features.h"
#include "agent/fragments.h"


// Forward declaration of supervisor interface defainde in TLFAgent
class ILFSupervisor;


namespace agent {

	// Detections
	using TDetections = std::vector<TLFRect>;

	// Basic interfaces of Agent
	class ICorrector
	{
	public:
		virtual ~ICorrector() = default;

		// Correct result of detector using features.
		virtual void Correct(const TFeatures& features, std::vector<int>& corrections) const = 0;
		
		// Serializing methods.
		//virtual bool LoadXML(TiXmlElement* parent) = 0;
		virtual TiXmlElement* SaveXML() = 0;
	};

	class ICorrectorTrainer	{
	public:
		virtual ~ICorrectorTrainer() = default;

		virtual std::vector<std::unique_ptr<ICorrector>> Consume() = 0;

		// Setup trainer internal state for start image processing
		virtual void Setup(const TFragments& fragments, const TDetections& detections) = 0;
		// Process new samples
		virtual void Collect(const TFragments& fragments, const TFeatures& feats, const TDetections& detections) = 0;
		// Finish collecting samples and start training
		virtual void Train() = 0;
				
		virtual void Finish() = 0;

		// Serializing methods.
		//virtual bool LoadXML(TiXmlElement* parent) = 0;
		virtual TiXmlElement* SaveXML() = 0;
	};


	// Interface of any Detector that can be corrected;
	class IDetector {
	public:
		virtual ~IDetector() = default;
		// Type of detected objects
		virtual std::string_view GetType() const = 0;

		// Detector name
		virtual std::string_view GetName() const = 0;
				
		// Initialize detector for the image
		virtual const TFragments& Setup(std::shared_ptr<TLFImage> img, const std::vector<TLFRect>* rois = nullptr) = 0;
			
		// Run detector and fill features and result of detections for fragments in range [begin, end]
		// Can be called in parallel for different fragments;
		virtual bool Detect(TFeaturesBuilder& builder) const = 0;

		// Finalize processing image
		virtual void Release() = 0;

		// Serializing methods.
		//virtual bool LoadXML(TiXmlElement* parent) = 0;
		virtual TiXmlElement* SaveXML() = 0;
	};

}

