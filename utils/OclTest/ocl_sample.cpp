#include <iostream>
#include <CL/cl.hpp>

#include <vector>
#include <array>

#include <windows.h>
#include <random>

#include <thread>


#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <numeric>

#include "tinyxml.h"

#include "LF.h"
#include "LFEngine.h"
#include "LFFileUtils.h"
#include "LFFeatures.h"
#include "LFWeak.h"

#include "LFGpuEngine.h"

#include "accel_rects.h"

#include <mutex>
#include <optional>
#include <chrono>

#ifdef _OMP_
#include <omp.h>
#endif

std::unique_ptr<TLFDetectEngine> load_detector(const TLFString& det_path)
{
    // TODO: for some reason neither vector of instances nor vector of smart
    // pointers can be created, so extra error handling should be added later

    std::unique_ptr<TLFDetectEngine> det = std::make_unique<TLFDetectEngine>();
    if (!det->Load(det_path.c_str())) {
        std::cerr << "TLFDetectEngine couldn't parse file" <<
            det_path << std::endl;
        return nullptr;
    }

    return det;
}


std::unique_ptr<TLFImage> load_image(const TLFString& path)
{
    std::unique_ptr<TLFImage> img = std::make_unique<TLFImage>();
    if (!img->LoadFromFile(path.c_str())) {
        std::cerr << "Can't load image " << path << std::endl;
        return nullptr;
    }

    return img;
}

std::optional<accel_rects::detector_t>   convert_detector(ILFObjectDetector* detector) {
    accel_rects::DetectorBuilder builder;

    TLFObjectList* strongs = detector->GetStrongs();
    if (strongs == NULL)
        return std::nullopt;

    builder.Begin();

    for (size_t s = 0; s < strongs->GetCount(); ++s) {
        ILFStrong* classifier = (ILFStrong*)strongs->Get(s);
        if (classifier == NULL)
            return std::nullopt;

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

    return builder.Consume();
}

accel_rects::transforms_t   convert_transforms(ILFScanner* scanner, int width, int height) {

    //TODO: use omp;
    accel_rects::transforms_t transforms;
    
    awpRect rect = { 0, 0, (short)width, (short)height };
    scanner->ScanRect(rect);

    for (int i = 0; i < scanner->GetFragmentsCount(); i++) {

        awpRect rect = scanner->GetFragmentRect(i);
                
        float scale_x = (rect.right - rect.left) / float(scanner->GetBaseWidth());
        float scale_y = (rect.bottom - rect.top) / float(scanner->GetBaseHeight());
        float scale = std::min<float>(scale_x, scale_y);

        transforms.push_back({ scale, scale, float(rect.left), float(rect.top) });
    }
    return transforms;
}


int apply_detector(TLFDetectEngine* engine,
    TLFImage* image)
{

    //Mutex if OMP threading used.
    auto mtx = std::make_shared<std::mutex>();
    engine->GetDetector(0)->SetDescCallback([=](size_t index,
        const auto& bounds,
        const auto& desc) {
            if (desc.result) {
                std::cout << "Detected! Bound: " << bounds.Rect.left << " " << bounds.Rect.top << " " << bounds.Rect.right << " " << bounds.Rect.bottom << std::endl;
            }
            /*std::cout << "Start Detector " << std::endl;
            for (size_t s = 0; s < desc.descs.size(); ++s) {
                const auto& strong = desc.descs[s];
                std::cout << "Start Strong " << s << std::endl;
                for (const auto& weak : strong.weaks) {

                    const auto& feature = weak.feature;

                    std::cout << feature.value << " : [";

                    for (auto v : feature.features) {
                        std::cout << int(v) << ", ";

                    }
                    std::cout << "]" << std::endl;

                }

                std::cout << std::endl;

            }
            std::cout << "End Detector " << std::endl;*/

        });

    if (!engine->SetSourceImage(image, true)) {
        return -103;
    }
   
    return 0;
}



int test_detector(const std::string& detector_path, const std::string& image_path) {
    auto det = load_detector(detector_path);
    auto img = load_image(image_path);
    auto acc_det = convert_detector(det->GetDetector(0));

    //apply_detector(det.get(), img.get());
    
    auto integral_image = img->GetIntegralImage();

    std::vector<uint64_t> integral(integral_image->sSizeX * integral_image->sSizeY, 0);

    for (size_t p = 0; p < integral.size(); ++p) {
        integral[p] = ((double*)integral_image->pPixels)[p];
    }

    auto engine = accel_rects::Engine::Create(16);
    
    auto integral_view = engine.CreateIntegral(integral.data(), integral_image->sSizeX, integral_image->sSizeY, integral_image->sSizeX * sizeof(uint64_t));

    if (!integral_view.is_valid()) {
        return -1;
    }

    
   // transforms.resize(1);
    auto det_view = engine.CreateDetector(std::move(*acc_det));

    auto worker = engine.CreateWorker();
   

    int num_transforms_per_task = 4096; //1024;
    int num_threads_proc = 0;
#ifdef _OMP_
    num_threads_proc = std::min<int>(4, omp_get_max_threads());
#endif 
    auto scanner = det->GetDetector(0)->GetScanner();

    //Transforms per thread
    std::vector<accel_rects::transforms_t>  multi_transforms(num_threads_proc);
    std::vector<std::vector<awpRect>>       multi_fragments(num_threads_proc);
    std::vector<accel_rects::Worker>        workers;

    for (int i = 0; i < num_threads_proc; ++i) {
        multi_transforms[i].reserve(num_transforms_per_task);
        multi_fragments[i].reserve(num_transforms_per_task);
        workers.push_back(engine.CreateWorker());
    }
        
    awpRect rect = { 0, 0, (short)integral_image->sSizeX, (short)integral_image->sSizeY };
    scanner->ScanRect(rect);
       
    auto processing = [](const accel_rects::dims_t& dims, const accel_rects::detections_t& dets, const accel_rects::features_t& feats, const std::vector<awpRect>& frags) {
        //dims: transforms, stages, features
        // std::cout << "Callback " << dims[0] << " : " << dims[1] << " : " << dims[2] << ". " << std::this_thread::get_id() << std::endl;

        for (size_t t = 0; t < dims[0]; ++t) {
            /*       std::cout << "Features " << dims[2] << "." << std::endl;
                   for (size_t f = 0; f < dims[2]; ++f) {
                       std::cout << feats[t * dims[2] + f] << "\t";
                   }

                   std::cout << std::endl << "Detections " << dims[1] << "." << std::endl;
                   */bool detected = true;
                   for (size_t s = 0; s < dims[1]; ++s) {
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
                   }
                   // std::cout << std::endl;
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


        trans.push_back({scale, scale, float(rect.left), float(rect.top)});
        frags.push_back(rect);


        if (trans.size() >= num_transforms_per_task) {
                        
            auto res = workers[current_thread].Enqueue(integral_view, det_view, trans,
                std::bind(processing, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::move(frags)));

            frags.reserve(num_threads_proc);

            trans.resize(0);
            frags.resize(0);

        }

        
    }

    for (int i = 0; i < num_threads_proc; ++i) {
        
        auto res = workers[i].Enqueue(integral_view, det_view, multi_transforms[i],
            std::bind(processing, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::move(multi_fragments[i])));

    }
    
    return 0;

}




std::vector<uint8_t>            generate_image(int width, int height) {
    // First create an instance of an engine.
    std::random_device rnd_device;
    // Specify the engine and distribution.
    std::mt19937 mersenne_engine{ rnd_device() };  // Generates random integers
    std::uniform_int_distribution<int> dist{ 0, 255 };

    auto gen = [&]() {
        return dist(mersenne_engine);
        };

    std::vector<uint8_t> vec(width * height);
    std::generate(vec.begin(), vec.end(), gen);
    return vec;
}

std::vector<uint64_t>            integral_image(const std::vector<uint8_t>& image, int width, int height) {
    std::vector<uint64_t> integral(width * height);
    integral[0] = image[0];
    for (int i = 1; i < width; ++i) {
        integral[i] = integral[i - 1] + image[i];
    }

    for (int j = 1; j < height; ++j) {
        int top = (j - 1) * width;
        int current = top + width;
        integral[current] = integral[top] + image[current];
        for (int i = 1; i < width; ++i) {
            integral[current + i] = integral[current + i - 1] + integral[top + i] + image[current + i] - integral[top + i - 1];
        }
    }
    
    return integral;
}

int test1() {
    int width = 1280;
    int height = 720;
    int feats_size = 10;
    int trans_size = 1024;
    int stages_size = 5000;


    auto img = generate_image(width, height);
    auto integral = integral_image(img, width, height);


    accel_rects::ScTable table;

    for (int i = 0; i < 64; ++i)
        table[i] = 0xff;

    accel_rects::DetectorBuilder   det_builder;
    det_builder.Begin();

    for (int s = 0; s < stages_size; s++) {
        det_builder.BeginStage();

        for (int f = 0; f < feats_size; ++f) {
            det_builder.AddSCWeak(1, table, 0, 0, 0.23f, 0.23f);
            det_builder.AddSCWeak(1, table, 0.1, 0.1, 0.27f, 0.27f);
            det_builder.AddSCWeak(1, table, 0.05, 0.05, 0.2f, 0.2f);
        }
        det_builder.EndStage(1);
    }
    det_builder.End();

    auto engine = accel_rects::Engine::Create(16);


    if (!engine.is_valid()) {
        return -1;
    }

    auto det_view = engine.CreateDetector(det_builder.Consume());

    std::cout << "Detector created!" << std::endl;

    auto integral_view = engine.CreateIntegral(integral.data(), width, height, width * sizeof(uint64_t));

    if (!integral_view.is_valid()) {
        return -2;
    }


    accel_rects::callback_t cb = [](const accel_rects::dims_t& dims, const accel_rects::detections_t& dets, const accel_rects::features_t& feats) {

        std::cout << "Callback " << dims[0] << " : " << dims[1] << " : " << dims[2] << ". " << std::this_thread::get_id() << std::endl;
        /*for (size_t t = 0; t < trans_size; ++t) {
            std::cout << "Features " << feats_size << "." << std::endl;
            for (size_t f = 0; f < std::min<size_t>(feats_size, 10); ++f) {
                std::cout << feats[t * feats_size + f] << "\t";
            }
            std::cout << std::endl << "Detections " << stages_size << "." << std::endl;
            for (size_t s = 0; s < std::min<size_t>(stages_size, 10); ++s) {
                std::cout << int(dets[t * stages_size + s]) << "\t";
            }
            std::cout << std::endl;
        }*/


        };

    std::vector<std::thread>        threads;

    int threads_count = 8;

    for (int t = 0; t < threads_count; ++t) {
        threads.emplace_back([&]() {
            auto worker = engine.CreateWorker();
            for (int z = 0; z < 100; ++z) {
                accel_rects::transforms_t trans(trans_size, { float(width), float(height), 0, 0 });

                for (int i = 0; i < trans_size; ++i) {
                    trans[i][2] = 0;
                    trans[i][3] = 0;
                }

                auto res = worker.Enqueue(integral_view, det_view.features_view(), std::move(trans), cb);
                std::cout << "Added " << z << " res=" << res << std::endl;
            }

            });
        //threads.back().detach();
    }

    for (int t = 0; t < threads_count; ++t) {
        threads[t].join();
    }


    std::cout << "FINISHED!" << std::endl;

    //::Sleep(90000);

    return 0;
}


int test_gpu_engine(const std::string& detector_path, const std::string& image_path) {
    auto det = load_detector(detector_path);
    auto img = load_image(image_path);

    TGpuEngine engine;

    auto detector = engine.CreateDetector((TSCObjectDetector* /*HACK*/)det->GetDetector(0));

    auto integral = engine.CreateIntegral(img.get());

    auto begin = std::chrono::high_resolution_clock::now();

    int count = 100;

    for (int i = 0; i < count; ++i) {

        auto result = engine.Run(detector, integral, { 0, 0, (AWPSHORT)integral.width(), (AWPSHORT)integral.height() });

        std::cout << "Detected " << result.size() << " items." << std::endl;
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto dur = end - begin;
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
    std::cout << "Execution duration " << ms / count << " ms." << std::endl;

    return 0;

}

int test_ñpu_engine(const std::string& detector_path, const std::string& image_path) {
    auto det = load_detector(detector_path);
    auto img = load_image(image_path);
        
    auto begin = std::chrono::high_resolution_clock::now();

    apply_detector(det.get(), img.get());
    

    auto end = std::chrono::high_resolution_clock::now();
    auto dur = end - begin;
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
    std::cout << "Execution duration " << ms  << " ms." << std::endl;

    return 0;

}


int main()
{
    //return test_detector("test/detector.xml", "test/test.awp");
    //return test_gpu_engine("test/detector.xml", "test/test.awp");
    return test_ñpu_engine("test/detector.xml", "test/test.awp");
   
  
}