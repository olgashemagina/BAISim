#include "LFGpuEngine.h"
#include "LF.h"
#include "LFEngine.h"
#include "LFFileUtils.h"
#include "LFFeatures.h"
#include "LFWeak.h"

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
    std::vector<accel_rects::transforms_t>  multi_transforms(num_threads_proc);
    std::vector<std::vector<awpRect>>       multi_fragments(num_threads_proc);
    std::vector<accel_rects::Worker>        workers;

    for (int i = 0; i < num_threads_proc; ++i) {
        multi_transforms[i].reserve(num_transforms_);
        multi_fragments[i].reserve(num_transforms_);
        workers.push_back(engine_.CreateWorker());
    }

    scanner->ScanRect(in_rect);

    auto processing = [=, &objects](const accel_rects::dims_t& dims, const accel_rects::detections_t& dets, const accel_rects::features_t& feats, const std::vector<awpRect>& frags) {
        //dims: transforms, stages, features
        // std::cout << "Callback " << dims[0] << " : " << dims[1] << " : " << dims[2] << ". " << std::this_thread::get_id() << std::endl;

        auto [num_trans, num_stages, num_feats] = dims;

        for (size_t t = 0; t < num_trans; ++t) {
            bool detected = true;
            for (size_t s = 0; s < num_stages; ++s) {
                //std::cout << int(dets[t * dims[1] + s]) << " ";
                if (dets[t * dims[1] + s] == 0) {
                    detected = false;
                    break;
                }
                //std::cout << int(dets[t * dims[1] + s]) << "\t";
            }

            if (detected) {
                auto rect = frags[t];
                std::cout << "Detected! Bounds: " << rect.left << " " << rect.top << " " << rect.right << " " << rect.bottom << std::endl;

                if (detected)
                {
                    //object detected
                    UUID id;
                    LF_NULL_UUID_CREATE(id);
                    // записываем результат в лист.
                    //TODO: get score

#ifdef _OMP_
#pragma omp critical 
#endif
                    {
                        objects.emplace_back(&rect, 1.0, det_source->GetObjectType(), det_source->GetAngle(), det_source->GetRacurs()
                            , det_source->GetBaseWidth(), det_source->GetBaseHeight(), det_source->GetName(), id);
                        objects.back().SetHasObject(true);

                    }
                }

            }
            // std::cout << std::endl;

            if (cb) {
                cb(frags[t], dets.data() + t * num_stages, feats.data() + t * num_feats);
            }

        }
    };



#ifdef _OMP_
#pragma omp parallel for num_threads(num_threads_proc)
#endif 
    for (int i = 0; i < scanner->GetFragmentsCount(); i++) {

        int current_thread = 0;

#ifdef _OMP_
        current_thread = omp_get_thread_num();
#endif 
        //std::cout << "Thread: " << current_thread << std::endl;


        auto& trans = multi_transforms[current_thread];
        auto& frags = multi_fragments[current_thread];

        awpRect rect = scanner->GetFragmentRect(i);

        float scale_x = (rect.right - rect.left) / float(scanner->GetBaseWidth());
        float scale_y = (rect.bottom - rect.top) / float(scanner->GetBaseHeight());
        float scale = std::min<float>(scale_x, scale_y);


        trans.push_back({ scale, scale, float(rect.left), float(rect.top) });
        frags.push_back(rect);


        if (trans.size() >= num_transforms_) {

            auto res = workers[current_thread].Enqueue(image.view(), detector.view(), trans,
                std::bind(processing, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::move(frags)));

            frags.reserve(num_threads_proc);

            trans.resize(0);
            frags.resize(0);

        }


    }

    for (int i = 0; i < num_threads_proc; ++i) {

        auto res = workers[i].Enqueue(image.view(), detector.view(), multi_transforms[i],
            std::bind(processing, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::move(multi_fragments[i])));

    }

    return objects;

}
