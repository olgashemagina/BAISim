#include "detectors.h"
#include "accel_rects/accel_rects.h"
#include "LFGpuEngine.h"
#include "utils/object_pool.h"



namespace agent {

    static const int kDefaultWorkersCount = 4;

    
    class TAccelStagesDetector : public TStagesDetectorBase {

    public:

        TAccelStagesDetector() {}

        virtual ~TAccelStagesDetector() = default;

        virtual bool            InitializeStructure() { 
            if (!gpu_detector_.Build(detector_.get())) {
                return false;
            }
            layout_.clear();
            for (size_t i = 1; i < gpu_detector_.view().detector().positions().size(); ++i) {
                layout_.push_back(gpu_detector_.view().detector().positions()[i]);
            }

            return true;

        }

        virtual void            SetupImage(std::shared_ptr<TLFImage> img, bool update_needed) {
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
            if (img_ != img) {
                if (!integral_.Update(img.get())) {
                    std::cout << "Error while creating image " << std::endl;
                    fragments_.Clear();

                }
            }
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



        virtual TiXmlElement* SaveXML() {
            auto node = TStagesDetectorBase::SaveXML();

            node->SetValue("TAccelStagesDetector");
            return node;
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
       
        TGpuDetector                                        gpu_detector_;
        TGpuTransforms                                      gpu_transforms_;

       
    private:
        // TODO: move to image container;
        std::shared_ptr<TLFImage>                           cached_image_;

        TGpuIntegral                                        integral_;
    };


    std::unique_ptr<TStagesDetectorBase> agent::CreateGpuDetector() {
        return std::make_unique<TAccelStagesDetector>();
    }



}