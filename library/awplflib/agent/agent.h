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
		virtual void Correct(const TFeatures& features, std::vector<int>& corrections) = 0;

		// Serializing methods.
		//virtual bool LoadXML(TiXmlElement* parent) = 0;
		virtual TiXmlElement* SaveXML() = 0;
	};

	class ICorrectorTrainer
	{
	public:
		virtual ~ICorrectorTrainer() = default;

		// Process new samples
		virtual void CollectSamples(ILFScanner* scanner, const TFeatures& feats, const TDetections&) = 0;
		// Start training of samples if needed.
		virtual std::vector<std::unique_ptr<ICorrector>> Train() = 0;

		// Serializing methods.
		//virtual bool LoadXML(TiXmlElement* parent) = 0;
		virtual TiXmlElement* SaveXML() = 0;
	};


	// Worker of detector that support parallel processing in different threads.
	// Garanteed that Detect can be called from one thread simultaneously	
	class IWorker {
	public:
		virtual ~IWorker() = default;

		// Run detector and fill features and result of detections for fragments in range [begin, end]
		virtual void Detect(std::shared_ptr<TLFImage> img, TFeaturesBuilder& builder) = 0;
	};

	// Interface of any Detector that can be corrected;
	class IDetector {
	public:
		virtual ~IDetector() = default;


		// Type of detected objects
		virtual std::string_view GetType() const = 0;

		// Detector name
		virtual std::string_view GetName() const = 0;

		// Scanner of Detector;
		virtual ILFScanner* GetScanner() = 0;
				
		// Creating Worker for parallel processing.
		virtual std::unique_ptr<IWorker> CreateWorker() = 0;

		// Serializing methods.
		//virtual bool LoadXML(TiXmlElement* parent) = 0;
		virtual TiXmlElement* SaveXML() = 0;

	};

}

