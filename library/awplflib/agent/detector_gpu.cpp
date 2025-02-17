#include "detectors.h"
#include "accel_rects/accel_rects.h"
#include "LFGpuEngine.h"
#include "utils/object_pool.h"



namespace agent {

    static const int kDefaultWorkersCount = 2;

    
    class TAccelStagesDetector : public IDetector {

    public:

        TAccelStagesDetector() {}

        virtual ~TAccelStagesDetector() = default;

        // Type of detected objects
        virtual std::string_view GetType() const { return type_; };

        // Detector name
        virtual std::string_view GetName() const { return name_; };

        // Supported only SC detector
        bool		Initialize(std::unique_ptr<TSCObjectDetector> detector, size_t min_stages) {
            detector_ = std::move(detector);
            min_stages_ = min_stages;
            if (InitializeStructure()) {
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

    
        const TFragments& Setup(std::shared_ptr<TLFImage> img, const std::vector<TLFRect>* rois) {
            bool update_needed = false;

            if (rois && !rois->empty()) {
                if (rois)
                    update_needed = fragments_.Scan(detector_->GetScanner(), img, min_fragment_factor_, *rois);
                else
                    update_needed = fragments_.Scan(detector_->GetScanner(), img);
            }


            if (update_needed) {
                gpu_transforms_.Clear();
                gpu_transforms_.AddFragments(fragments_.rects(), 
                    detector_->GetScanner()->GetBaseWidth(), 
                    detector_->GetScanner()->GetBaseHeight());

                if (!gpu_transforms_.Update()) {
                    std::cout << "Error while update transforms " << std::endl;
                    fragments_.Clear();
                }
            }

            //TODO: create cache;
            if (!integral_.Update(img.get())) {
                std::cout << "Error while creating image " << std::endl;
                fragments_.Clear();
            }
                        
            return fragments_;
        }

        // Run detector and fill features and result of detections for fragments in range [begin, end]
        virtual bool Detect(TFeaturesBuilder& builder) const override {

            builder.Reset(layout_);
                       
            return GetWorker()->Run( integral_.view(), gpu_detector_.view(), gpu_transforms_.view(), min_stages_,
                    builder.frags_begin(),
                builder.frags_end(),
                builder.triggered(),
                &builder.GetMutableData().data());

            // TODO: add scores
            
        }

        // Finalize processing image
        void Release() override {}



        // Serializing methods.
        bool LoadXML(TiXmlElement* stages_node) {
            stages_node->QueryValueAttribute("min_stages", &min_stages_);

            auto detector_node = stages_node->FirstChildElement();

            decltype(detector_) det;

            if (detector_node && detector_node->ValueStr() == "TSCObjectDetector") {

                auto detector = std::make_unique<TSCObjectDetector>();
                //TiXmlElement* e = elem->FirstChildElement();
                if (!detector->LoadXML(detector_node))
                    return false;

                det = std::move(detector);
            }

            return Initialize(std::move(det), min_stages_);

        };


        TiXmlElement* SaveXML() {
            TiXmlElement* stages_node = new TiXmlElement("TAccelStagesDetector");

            stages_node->SetAttribute("min_stages", min_stages_);

            if (detector_) {
                stages_node->LinkEndChild(detector_->SaveXML());
            }

            return stages_node;
        }


    private:
        // Load features count
        bool            InitializeStructure() {
                        
            if (!gpu_detector_.Build(detector_.get())) {
                return false;
            }
            layout_.clear();
            for (size_t i = 1; i < gpu_detector_.view().detector().positions().size(); ++i) {
                layout_.push_back(gpu_detector_.view().detector().positions()[i]);
            }
            
            return true;
        }

    private:

        static std::shared_ptr<accel_rects::WorkerView> GetWorker() {

            static pool::ObjectPool<accel_rects::WorkerView>             pool([]() {

                return std::make_unique<accel_rects::WorkerView>(
                    std::move(accel_rects::GetDefault()->CreateWorker()));

                }, kDefaultWorkersCount);

            return pool.Get();
        }


    private:
        std::string                                         type_;
        std::string                                         name_;

        // Structure of detector
        TLayout                                             layout_;

        TGpuDetector                                        gpu_detector_;
        TGpuTransforms                                      gpu_transforms_;

        std::unique_ptr<TSCObjectDetector>					detector_;

        // Minimal stages count for processing;
        size_t                                              min_stages_ = 3;

        float                                               min_fragment_factor_ = 0.7f;

        TFragmentsBuilder                                   fragments_;

    private:
        // TODO: move to image container;
        std::shared_ptr<TLFImage>                           cached_image_;

        TGpuIntegral                                        integral_;
    };


    std::unique_ptr<IDetector> agent::CreateGpuDetector(std::unique_ptr<TSCObjectDetector> detector, size_t min_stages)
    {
        auto det = std::make_unique<TAccelStagesDetector>();

        if (det->Initialize(std::move(detector), min_stages)) {
            return det;
        }

        return {};
    }

    std::unique_ptr<IDetector> agent::CreateGpuDetector(TiXmlElement* stages_node)
    {
        auto det = std::make_unique<TAccelStagesDetector>();

        if (det->LoadXML(stages_node)) {
            return det;
        }

        return {};
    }


}