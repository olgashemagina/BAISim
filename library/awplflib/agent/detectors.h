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
    class TStagesDetectorBase : public IDetector {

    public:
     

        virtual ~TStagesDetectorBase() = default;
                        
        // Supported only SC detector
        bool		Initialize(std::unique_ptr<TSCObjectDetector> detector) {
            detector_ = std::move(detector);
            
            if (detector_ && InitializeStructure()) {
                type_ = detector_->GetObjectType();
                name_ = detector_->GetName();
                return true;
            }
            else {
                type_ = "unknown";
                name_ = "none";
            }
            return false;
        }

        void SetMinStages(size_t min_stages) { min_stages_ = min_stages; }

        void SetMinFragmentFactor(float min_fragment_factor) { min_fragment_factor_ = min_fragment_factor; }


        // Type of detected objects
        virtual std::string_view GetType() const { return type_; };

        // Detector name
        virtual std::string_view GetName() const { return name_; };

        const TFragments& Setup(std::shared_ptr<TLFImage> img, const std::vector<TLFRect>* rois) override {
            bool update_needed = false;

            if (!detector_ || !detector_->GetScanner()) {
                fragments_.Clear();
                return fragments_;
            }

            if (rois && !rois->empty())
                update_needed = fragments_.Scan(detector_->GetScanner(), img, min_fragment_factor_, *rois);
            else
                update_needed = fragments_.Scan(detector_->GetScanner(), img);
                                  
            SetupImage(img, update_needed);

            img_ = img;
            

            return fragments_;
        }

        // Run detector and fill features and result of detections for fragments in range [begin, end]
        // Can be called in parallel for different fragments;
        //bool Detect(TFeaturesBuilder& builder) const override = 0;

        // Finalize processing image
        void Release() override {
            ReleaseImage();
            img_.reset();
        }


        // Serializing methods.
     // Serializing methods.
        virtual bool LoadXML(TiXmlElement* stages_node) {
            stages_node->QueryValueAttribute("min_stages", &min_stages_);
            stages_node->QueryValueAttribute("min_fragment_factor", &min_fragment_factor_);

            auto detector_node = stages_node->FirstChildElement();

            decltype(detector_) det;

            if (detector_node && detector_node->ValueStr() == "TSCObjectDetector") {

                auto detector = std::make_unique<TSCObjectDetector>();
                //TiXmlElement* e = elem->FirstChildElement();
                if (!detector->LoadXML(detector_node))
                    return false;

                det = std::move(detector);
            }

            return Initialize(std::move(det));

        };


        virtual TiXmlElement* SaveXML() {
            TiXmlElement* stages_node = new TiXmlElement("TStagesDetectorBase");

            stages_node->SetAttribute("min_stages", min_stages_);
            stages_node->SetAttribute("min_fragment_factor", min_fragment_factor_);

            if (detector_) {
                stages_node->LinkEndChild(detector_->SaveXML());
            }

            return stages_node;
        }

    public:
        TSCObjectDetector* detector() const { return detector_.get(); }
       
    protected:
        // Load features count
        virtual bool            InitializeStructure() { return false; }

        virtual void            SetupImage(std::shared_ptr<TLFImage> img, bool update_needed) {}
        virtual void            ReleaseImage() {}

    protected:
        std::string                                         type_;
        std::string                                         name_;
        // Structure of detector
        TLayout                                             layout_;
        // Detector pointer
        std::unique_ptr<TSCObjectDetector>					detector_;

    protected:

        TFragmentsBuilder                                   fragments_;

        std::shared_ptr<TLFImage>                           img_;

        // Minimal stages count for processing;
        size_t                                              min_stages_ = 0;

        float                                               min_fragment_factor_ = 0.7;

    };


    std::unique_ptr<TStagesDetectorBase>      CreateCpuDetector();
    std::unique_ptr<TStagesDetectorBase>      CreateGpuDetector();
    
        
    


}
