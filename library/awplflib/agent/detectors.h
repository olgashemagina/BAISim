#pragma once

#include <numeric>
#include <memory>
#include "agent/agent.h"

#include "LF.h"
#include "LFEngine.h"
#include "LFFileUtils.h"
#include "LFFeatures.h"
#include "LFWeak.h"

namespace agent {


    class TStagesDetectorImpl {

    public:

        virtual ~TStagesDetectorImpl() = default;


        // Supported only SC detector
        bool		Initialize(std::unique_ptr<ILFObjectDetector> detector, size_t min_stages) {
            detector_ = std::move(detector);
            min_stages_ = min_stages;
            return InitializeStructure();
        }

        // Scanner of Detector;
        ILFScanner* GetScanner() { return detector_->GetScanner(); }
              
        
        // Run detector and fill features and result of detections for fragments in range [begin, end]
        virtual void Detect(std::shared_ptr<TLFImage> img, TFeaturesBuilder& builder) {

            builder.Reset(layout_);

            for (size_t frag = builder.frags_begin(); frag < builder.frags_end(); ++frag) {
                float* features = builder.GetFeats(frag);
                size_t features_size = builder.feats_count();
                awpRect rect = detector_->GetScanner()->GetFragmentRect(frag);

                double scale_x = (rect.right - rect.left) / double(detector_->GetBaseWidth());
                double scale_y = (rect.bottom - rect.top) / double(detector_->GetBaseHeight());
                double scale = std::min<double>(scale_x, scale_y);

                TLFAlignedTransform transform(scale, scale, rect.left, rect.top);
                double score = FLT_MAX;

                TLFObjectList* strongs = detector_->GetStrongs();

                size_t triggered = -1;

                for (int j = 0; j < strongs->GetCount(); j++)
                {
                    ILFStrong* strong = static_cast<ILFStrong*>(strongs->Get(j));

                    // TODO: improve it, set pointer to store features;
                    auto desc = strong->Classify(img.get(), transform);

                    for (const auto& weak : desc.weaks) {
                        if (features_size < weak.feature.features.size())
                            throw std::out_of_range("Features memory size out of range");

                        std::memcpy(features, weak.feature.features.data(), weak.feature.features.size() * sizeof(float));
                        features_size -= weak.feature.features.size();
                        features += weak.feature.features.size();
                    }

                    //Check If object detected;
                    if (triggered != -1) {
                        if (desc.result != 0) {
                            score = std::min<double>(desc.score, score);
                        }
                        else {
                            // No object detected.
                            score = desc.score;
                            triggered = j;
                        }
                    }
                    else if (j + 1 >= min_stages_)
                        // Stop checking;
                        break;
                }

                builder.SetTriggered(frag, triggered);

                // TODO: store score;

            }
        }


    private:
        // Load features count
        bool            InitializeStructure() {
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


    private:

        // Structure of detector
        TLayout                                             layout_;
        // Detector pointer
        std::unique_ptr<ILFObjectDetector>					detector_;

        // Minimal stages count for processing;
        size_t                                              min_stages_ = 3;
    };


    class TStagesWorker : public IWorker {
    public:
        TStagesWorker(const std::shared_ptr<TStagesDetectorImpl>& impl)
            : impl_(impl) {

        }

        virtual ~TStagesWorker() = default;

        // Run detector and fill features and result of detections for fragments in range [begin, end]
        virtual void Detect(std::shared_ptr<TLFImage> img, TFeaturesBuilder& builder) {
            impl_->Detect(img, builder);
        }

    private:
        // Implementation
        std::shared_ptr<TStagesDetectorImpl>				impl_;
    };


	class TStagesDetector : IDetector {

        friend class TStagesWorker;
	public:
        TStagesDetector() {
            impl_ = std::make_shared<TStagesDetectorImpl>();
        }

        virtual ~TStagesDetector() = default;


		// Supported only SC detector
		bool		Initialize(std::unique_ptr<ILFObjectDetector> detector) {
            return impl_->Initialize(std::move(detector), 3);
		}
                
        // Scanner of Detector;
        virtual ILFScanner* GetScanner() { return impl_->GetScanner(); }
                
        // Creating Worker for parallel processing.
        virtual std::unique_ptr<IWorker> CreateWorker() {
            return std::make_unique<TStagesWorker>(impl_);
        }

        // Serializing methods.
        virtual bool LoadXML(TiXmlElement* parent) {
            return false;
        };
        virtual TiXmlElement* SaveXML() {
            return nullptr;
        }

	private:

        // Detector pointer
		std::shared_ptr<TStagesDetectorImpl>				impl_;

	};

    


}
