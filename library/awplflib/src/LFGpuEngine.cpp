#include "LFGpuEngine.h"
#include "LF.h"
#include "LFEngine.h"
#include "LFFileUtils.h"
#include "LFFeatures.h"
#include "LFWeak.h"

TGpuEngine::TGpuEngine(int num_threads, int num_workers, int num_transforms, int min_stages)
    : num_threads_(num_threads)
    , num_transforms_(num_transforms)
    , min_stages_(min_stages) {
    
    pool_ = std::make_unique<pool::ObjectPool<accel_rects::WorkerView>>([this]() {
                
        return std::make_unique<accel_rects::WorkerView>(
                std::move(accel_rects::GetDefault()->CreateWorker()));

        }, num_workers);
}

bool TGpuEngine::Initialize(std::unique_ptr<TSCObjectDetector> detector) {
    
    if (detector_.Build(detector.get())) {
        object_detector_ = std::move(detector);
        return true;
    }
    return false;
}

std::vector<TLFDetectedItem> TGpuEngine::Detect(const TGpuIntegral& image) {
    
    std::vector<TLFDetectedItem> objects;

    int num_threads_proc = 0;
#ifdef _OMP_
    num_threads_proc = std::min<int>(num_threads_, omp_get_max_threads());
#endif 
    
    //Transforms per thread

    if (result_.size() < num_threads_proc) {
    
        for (int i = 0; i < num_threads_proc; ++i) {
    
            result_.emplace_back();
            auto& [triggered, features] = result_.back();

            triggered.resize(num_transforms_, -1);
            features.resize(num_transforms_ * detector_.features_count(), 0);
        }
    }

    if (trans_width_ != image.width() || trans_height_ != image.height()) {

        std::cout << "Creating transforms " << std::endl;

        object_detector_->GetScanner()->Scan(image.width(), image.height());

        transforms_.Clear();
        transforms_.AddFragments(object_detector_->GetScanner());
        transforms_.Update();

        trans_width_ = image.width();
        trans_height_ = image.height();

    }

    auto fragments_count = transforms_.size();

    if (fragments_count == 0)
        return {};


    size_t batches_count = size_t(std::ceil(transforms_.size() / num_transforms_));

#ifdef _OMP_
#pragma omp parallel for num_threads(num_threads_proc)
#endif 
    for (int i = 0; i < batches_count; i++) {

        int current_thread = 0;

#ifdef _OMP_
        current_thread = omp_get_thread_num();
#endif 

        int batch_begin = num_transforms_ * i;
        int batch_end = batch_begin + num_transforms_;
        batch_end = batch_end < transforms_.size() ? batch_end : transforms_.size();

        auto& [triggered, features] = result_[current_thread];

        auto res = pool_->Get()->Run(image.view(), detector_.view(), transforms_.view(), min_stages_, batch_begin, batch_end, triggered, &features);
        
        if (!res) {
            std::cout << "Error in Thread: " << current_thread << std::endl;
        }

        for (size_t t = 0; t < triggered.size(); ++t) {

            auto rect = object_detector_->GetScanner()->GetFragmentRect(t + batch_begin);

            if (triggered[t] < 0) {

                std::stringstream ss;
                ss << "Detected! Bounds: " << rect.left << " " << rect.top << " " << rect.right << " " << rect.bottom << std::endl;

                std::cout << ss.str();

                //object detected
                UUID id;
                LF_NULL_UUID_CREATE(id);
                // записываем результат в лист.
                //TODO: get score

#ifdef _OMP_
#pragma omp critical 
#endif
                {
                    objects.emplace_back(rect, 1.0, object_detector_->GetObjectType(), object_detector_->GetAngle(), object_detector_->GetRacurs()
                        , object_detector_->GetBaseWidth(), object_detector_->GetBaseHeight(), object_detector_->GetName(), id);
                    objects.back().SetHasObject(true);

                }
            }
            // std::cout << std::endl;

           /* if (cb) {

                cb(rect, triggered[t], std::max<int>(min_stages_, triggered[t]), features.data() + t * detector.view().stages().positions().back());
            }*/

        }

    }
    return objects;
}


bool TGpuDetector::Build(TSCObjectDetector* detector) {
    accel_rects::DetectorBuilder builder;

    TLFObjectList* strongs = detector->GetStrongs();
    if (strongs == NULL)
        return false;

    builder.Begin();

    for (size_t s = 0; s < strongs->GetCount(); ++s) {
        ILFStrong* classifier = (ILFStrong*)strongs->Get(s);
        if (classifier == NULL)
            return false;

        builder.BeginStage();

        auto threshold = classifier->GetThreshold();

        for (size_t w = 0; w < classifier->GetCount(); ++w) {


            TCSWeak* weak = (TCSWeak*)classifier->GetWeak(w);
            auto weight = weak->Weight();

            auto feature = (TCSSensor*)weak->Fetaure();
            auto rect = feature->GetRect();

            accel_rects::ScTable table;

            for (int i = 0; i < 512; ++i) {
                table.SetBit(i, weak->Classificator(i));
            }

            builder.AddSCWeak(weight, table, rect.Left(), rect.Top(), rect.Width(), rect.Height());
        }

        builder.EndStage(threshold);

    }
    builder.End();
        
    view_ = accel_rects::GetDefault()->CreateDetector(builder);

    return view_.is_valid();

}

bool TGpuIntegral::Update(TLFImage* image) {
    auto integral_image = image->GetIntegralImage();

    integral_.resize(integral_image->sSizeX * integral_image->sSizeY, 0);

    //TODO: improve this;
    for (size_t p = 0; p < integral_.size(); ++p) {
        integral_[p] = ((double*)integral_image->pPixels)[p];
    }

    view_ = accel_rects::GetDefault()->CreateIntegral(integral_.data(), integral_image->sSizeX, integral_image->sSizeY, integral_image->sSizeX * sizeof(uint64_t));

    width_ = image->width();
    height_ = image->height();

    return view_.is_valid();

}

void TGpuTransforms::AddFragments(const std::vector<TLFRect>& rects, int base_width, int base_height) {


    transforms_.reserve(transforms_.size() + rects.size());

    // TODO: parallelize
    for (const auto& rect : rects) {
    //for (size_t frag = 0; frag < scanner->GetFragmentsCount(); ++frag) {
        //awpRect rect = scanner->GetFragmentRect(frag);

        float scale_x = (rect.Right() - rect.Left()) / float(base_width);
        float scale_y = (rect.Bottom() - rect.Top()) / float(base_height);
        float scale = std::min<float>(scale_x, scale_y);

        transforms_.push_back({ scale, scale, float(rect.Left()), float(rect.Top()) });

    }
}

void TGpuTransforms::AddFragments(ILFScanner* scanner) {


    transforms_.reserve(transforms_.size() + scanner->GetFragmentsCount());

    // TODO: parallelize
    
    for (size_t frag = 0; frag < scanner->GetFragmentsCount(); ++frag) {
        awpRect rect = scanner->GetFragmentRect(frag);

        float scale_x = (rect.right - rect.left) / float(scanner->GetBaseWidth());
        float scale_y = (rect.bottom - rect.top) / float(scanner->GetBaseHeight());
        float scale = std::min<float>(scale_x, scale_y);

        transforms_.push_back({ scale, scale, float(rect.left), float(rect.top) });

    }
}
