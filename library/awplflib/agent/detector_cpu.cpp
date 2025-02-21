#include "detectors.h"

using namespace agent;
namespace agent {

    // Detector of objects using CPU;
    class TStagesDetector : public TStagesDetectorBase {

    public:
        TStagesDetector() {}

        virtual ~TStagesDetector() = default;

        // Run detector and fill features and result of detections for fragments in range [begin, end]
        // Can be called in parallel for different fragments;
        bool Detect(TFeaturesBuilder& builder) const override;
        virtual TiXmlElement* SaveXML();

    private:
        // Load features count
        bool            InitializeStructure();


    };

    // Load features count
    bool            TStagesDetector::InitializeStructure() {
        layout_.clear();

        TLFObjectList* strongs = detector_->GetStrongs();
        if (strongs == NULL)
            return false;

        size_t features_count = 0;

        for (size_t s = 0; s < strongs->GetCount(); ++s) {
            ILFStrong* classifier = (ILFStrong*)strongs->Get(s);
            if (classifier == NULL)
                return false;

            auto threshold = classifier->GetThreshold();

            for (int w = 0; w < classifier->GetCount(); ++w) {

                TCSWeak* weak = (TCSWeak*)classifier->GetWeak(w);
                auto weight = weak->Weight();

                // Centus sensor
                auto feature = (TCSSensor*)weak->Fetaure();
                auto rect = feature->GetRect();

                // 9 values + 1 of full rect;
                features_count += 1;
            }
            layout_.push_back(features_count);
        }

        return true;
    }

    // Run detector and fill features and result of detections for fragments in range [begin, end]
    bool TStagesDetector::Detect(TFeaturesBuilder& builder) const {

        if (!img_) {
            std::cout << "It is needed to setup image before detect call" << std::endl;
            return false;
        }

        builder.Reset(layout_);

        for (size_t frag = builder.frags_begin(); frag < builder.frags_end(); ++frag) {
            float* features = builder.GetFeats(frag);
            size_t features_size = builder.feats_count();

            auto rect = fragments_.get(frag);
            
            double scale_x = (rect.Right() - rect.Left()) / double(detector_->GetBaseWidth());
            double scale_y = (rect.Bottom() - rect.Top()) / double(detector_->GetBaseHeight());
            double scale = std::min<double>(scale_x, scale_y);

            TLFAlignedTransform transform(scale, scale, rect.Left(), rect.Top());
            float score = FLT_MAX;

            TLFObjectList* strongs = detector_->GetStrongs();

            size_t triggered = kNoTriggered;

            for (int j = 0; j < strongs->GetCount(); j++)
            {
                ILFStrong* strong = static_cast<ILFStrong*>(strongs->Get(j));

                // TODO: improve it, set pointer to store features;
                auto desc = strong->Classify(img_.get(), transform);

                if (min_stages_ > 0) {

                    for (const auto& weak : desc.weaks) {
                        //if (features_size < weak.feature.features.size())
                            //throw std::out_of_range("Features memory size out of range");

                        // Copy only total value from CS feature
                        features[0] = weak.feature.features.back();
                        features_size -= 1;
                        features += 1;
                    }
                }

                //Check If object detected;
                if (triggered == kNoTriggered) {
                    if (desc.result != 0) {
                        score = std::min<float>(desc.score, score);
                    }
                    else {
                        // No object detected.
                        score = desc.score;
                        triggered = j;
                    }
                }
                if ((j + 1 >= min_stages_) && triggered != kNoTriggered)
                    // Stop checking;
                    break;
            }

            builder.SetTriggered(frag, triggered, score);
        }
        return true;
    }

    inline TiXmlElement* TStagesDetector::SaveXML() {
        auto node = TStagesDetectorBase::SaveXML();

        node->SetValue("TStagesDetector");
        return node;
    }
     
    std::unique_ptr<TStagesDetectorBase> agent::CreateCpuDetector() {
        return std::make_unique<TStagesDetector>();
    }

}


