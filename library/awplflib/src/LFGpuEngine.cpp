#include "LFGpuEngine.h"
#include "LF.h"
#include "LFEngine.h"
#include "LFFileUtils.h"
#include "LFFeatures.h"
#include "LFWeak.h"

TGpuEngine::TGpuEngine(int num_threads, int num_transforms, int min_stages) :
    engine_(accel_rects::Engine::Create())
    , num_threads_(num_threads)
    , num_transforms_(num_transforms)
    , min_stages_(min_stages) {

    /*pool_ = std::make_unique<pool::ObjectPool<accel_rects::Worker>>([&]() {
        return engine_.CreateWorker();

        }, 4);*/
    
}

TGpuDetector TGpuEngine::CreateDetector(TSCObjectDetector* detector) {
    accel_rects::DetectorBuilder builder;

    TLFObjectList* strongs = detector->GetStrongs();
    if (strongs == NULL)
        return {};

    builder.Begin();

    for (size_t s = 0; s < strongs->GetCount(); ++s) {
        ILFStrong* classifier = (ILFStrong*)strongs->Get(s);
        if (classifier == NULL)
            return {};

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

    auto view = engine_.CreateDetector(std::move(builder.Consume()));

    return { detector, view };

}

TGpuIntegral TGpuEngine::CreateIntegral(TLFImage* image) {
    auto integral_image = image->GetIntegralImage();

    std::vector<uint64_t> integral(integral_image->sSizeX * integral_image->sSizeY, 0);

    //HACK:
    for (size_t p = 0; p < integral.size(); ++p) {
        integral[p] = ((double*)integral_image->pPixels)[p];
    }

    auto view = engine_.CreateIntegral(integral.data(), integral_image->sSizeX, integral_image->sSizeY, integral_image->sSizeX * sizeof(uint64_t));

    return { image, view, std::move(integral)};
}


std::vector<TLFDetectedItem> TGpuEngine::Run(TGpuDetector detector, TGpuIntegral image, awpRect in_rect, TCallback cb) {

    std::vector<TLFDetectedItem> objects;

    int num_threads_proc = 0;
#ifdef _OMP_
    num_threads_proc = std::min<int>(num_threads_, omp_get_max_threads());
#endif 
    auto scanner = detector.detector()->GetScanner();
    auto det_source = detector.detector();

    //Transforms per thread

    if (result_.size() < num_threads_proc) {
        workers_.reserve(num_threads_proc);
        for (int i = 0; i < num_threads_proc; ++i) {
            workers_.emplace_back(engine_.CreateWorker());
            result_.emplace_back();
            auto& [triggered, features] = result_.back();

            triggered.resize(num_transforms_, -1);
            features.resize(num_transforms_ * detector.view().stages().positions().back(), 0);
        }
    }

    if (rect_.left != in_rect.left || rect_.right != in_rect.right || 
        rect_.top != in_rect.top || rect_.bottom != in_rect.bottom) {

        std::cout << "Creating transforms " << std::endl;

        scanner->ScanRect(in_rect);
        // Make transforms and send to gpu
        accel_rects::transforms_t trans;

        trans.reserve(scanner->GetFragmentsCount());
        
        // TODO: parallelize
        for (size_t frag = 0; frag < scanner->GetFragmentsCount(); ++frag) {
            awpRect rect = scanner->GetFragmentRect(frag);

            float scale_x = (rect.right - rect.left) / float(scanner->GetBaseWidth());
            float scale_y = (rect.bottom - rect.top) / float(scanner->GetBaseHeight());
            float scale = std::min<float>(scale_x, scale_y);

            trans.push_back({ scale, scale, float(rect.left), float(rect.top) });

        }

        transforms_ = engine_.CreateTransforms(std::move(trans));
        rect_ = in_rect;

    }

    auto fragments_count = scanner->GetFragmentsCount();

    if (fragments_count == 0)
        return {};


    size_t batches_count = size_t(std::ceil(fragments_count / num_transforms_));
        
#ifdef _OMP_
#pragma omp parallel for num_threads(num_threads_proc)
#endif 
    for ( int i = 0; i < batches_count; i++) {

        int current_thread = 0;

#ifdef _OMP_
        current_thread = omp_get_thread_num();
#endif 
        
        // Init features builder
        size_t batch_begin = num_transforms_ * i;
        size_t batch_end = batch_begin + num_transforms_;
        batch_end = batch_end < fragments_count ? batch_end : fragments_count;
        auto& [triggered, features] = result_[current_thread];

        //auto res = pool_->Get()->Run(image.view(), detector.view(), transforms_, min_stages_, batch_begin, batch_end, triggered, features);

        auto res = workers_[current_thread]->Run(image.view(), detector.view(), transforms_, min_stages_, batch_begin, batch_end, triggered, features);

        if (!res) {
            std::cout << "Error in Thread: " << current_thread << std::endl;
        }

        for (size_t t = 0; t < triggered.size(); ++t) {
          
            auto rect = scanner->GetFragmentRect(t + batch_begin);

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
                    objects.emplace_back(rect, 1.0, det_source->GetObjectType(), det_source->GetAngle(), det_source->GetRacurs()
                        , det_source->GetBaseWidth(), det_source->GetBaseHeight(), det_source->GetName(), id);
                    objects.back().SetHasObject(true);

                }
            }
            // std::cout << std::endl;

            if (cb) {

                cb(rect, triggered[t], std::max<int>(min_stages_, triggered[t]), features.data() + t * detector.view().stages().positions().back());
            }

        }
              
    }
        

    return objects;

}
