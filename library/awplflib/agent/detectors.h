#pragma once

#include <numeric>
#include <memory>
#include "agent/agent.h"

#include "LF.h"
#include "LFEngine.h"
#include "LFFileUtils.h"
#include "LFFeatures.h"
#include "LFWeak.h"
#include "LFDetector.h"

namespace agent {

    // Detector of objects using CPU;
	class TStagesDetector : public IDetector {
                
	public:
        TStagesDetector() {}

        virtual ~TStagesDetector() = default;


		// Supported only SC detector
		bool		Initialize(std::unique_ptr<ILFObjectDetector> detector, int min_stages = 3);
        
        
        // Type of detected objects
        virtual std::string_view GetType() const { return type_; };

        // Detector name
        virtual std::string_view GetName() const { return name_; };
                
        const TFragments& Setup(std::shared_ptr<TLFImage> img, const std::vector<TLFRect>* rois = nullptr) override;

        // Run detector and fill features and result of detections for fragments in range [begin, end]
        // Can be called in parallel for different fragments;
        bool Detect(TFeaturesBuilder& builder) const override;

        // Finalize processing image
        void Release() override;


        // Serializing methods.
        bool LoadXML(TiXmlElement* parent);
        TiXmlElement* SaveXML() override;

    private:
        // Load features count
        bool            InitializeStructure();

	private:
        std::string                                         type_;
        std::string                                         name_;
        // Structure of detector
        TLayout                                             layout_;
        // Detector pointer
        std::unique_ptr<ILFObjectDetector>					detector_;

        TFragmentsBuilder                                   fragments_;

        std::shared_ptr<TLFImage>                           img_;

        // Minimal stages count for processing;
        size_t                                              min_stages_ = 0;

	};

    std::unique_ptr<IDetector>      CreateGpuDetector(std::unique_ptr<TSCObjectDetector> detector, size_t min_stages);
    std::unique_ptr<IDetector>      CreateGpuDetector(TiXmlElement* parent);
        
    


}
