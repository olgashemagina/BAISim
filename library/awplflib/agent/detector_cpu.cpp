#include "detectors.h"

using namespace agent;
namespace agent {
   


    // Supported only SC detector

    bool TStagesDetector::Initialize(std::unique_ptr<ILFObjectDetector> detector, int min_stages) {

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
                features_count += 10;
            }
            layout_.push_back(features_count);
        }

        return true;
    }

    const TFragments& TStagesDetector::Setup(std::shared_ptr<TLFImage> img, const std::vector<TLFRect>* rois) {
        fragments_.Scan(detector_->GetScanner(), img, rois);
        img_ = img;
        return fragments_;
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
            awpRect rect = detector_->GetScanner()->GetFragmentRect(frag);

            double scale_x = (rect.right - rect.left) / double(detector_->GetBaseWidth());
            double scale_y = (rect.bottom - rect.top) / double(detector_->GetBaseHeight());
            double scale = std::min<double>(scale_x, scale_y);

            TLFAlignedTransform transform(scale, scale, rect.left, rect.top);
            float score = FLT_MAX;

            TLFObjectList* strongs = detector_->GetStrongs();

            size_t triggered = kNoTriggered;

            for (int j = 0; j < strongs->GetCount(); j++)
            {
                ILFStrong* strong = static_cast<ILFStrong*>(strongs->Get(j));

                // TODO: improve it, set pointer to store features;
                auto desc = strong->Classify(img_.get(), transform);

                for (const auto& weak : desc.weaks) {
                    if (features_size < weak.feature.features.size())
                        throw std::out_of_range("Features memory size out of range");

                    std::memcpy(features, weak.feature.features.data(), weak.feature.features.size() * sizeof(float));
                    features_size -= weak.feature.features.size();
                    features += weak.feature.features.size();
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

    void TStagesDetector::Release() {
        img_.reset();
    }

    // Serializing methods.
    bool TStagesDetector::LoadXML(TiXmlElement* stages_node) {
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


    TiXmlElement* TStagesDetector::SaveXML() {
        TiXmlElement* stages_node = new TiXmlElement("TStagesDetector");

        stages_node->SetAttribute("min_stages", min_stages_);

        if (detector_) {
            stages_node->LinkEndChild(detector_->SaveXML());
        }

        return stages_node;
    }

 


}


