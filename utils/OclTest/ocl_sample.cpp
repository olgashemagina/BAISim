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

#include "accel_rects/accel_rects.h"

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


int apply_detector(TLFDetectEngine* engine,
    TLFImage* image)
{

    //Mutex if OMP threading used.
    auto mtx = std::make_shared<std::mutex>();
    /*engine->GetDetector(0)->SetDescCallback([=](size_t index,
        const auto& bounds,
        const auto& desc) {
            if (desc.result) {
                std::cout << "Detected! Bound: " << bounds.Rect.left << " " << bounds.Rect.top << " " << bounds.Rect.right << " " << bounds.Rect.bottom << std::endl;
            }
            //std::cout << "Start Detector " << std::endl;
            //for (size_t s = 0; s < desc.descs.size(); ++s) {
                //const auto& strong = desc.descs[s];
                //std::cout << "Start Strong " << s << std::endl;
                f//or (const auto& weak : strong.weaks) {

//                    const auto& feature = weak.feature;

  //                  std::cout << feature.value << " : [";

    //                for (auto v : feature.features) {
      //                  std::cout << int(v) << ", ";

        //            }
          //          std::cout << "]" << std::endl;

            //    }

              //  std::cout << std::endl;

            //}
            //std::cout << "End Detector " << std::endl;

        });*/

    if (!engine->SetSourceImage(image, true)) {
        return -103;
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
/*
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
/*

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
*/

int test_gpu_engine(const std::string& detector_path, const std::string& image_path) {
    auto det = load_detector(detector_path);
    auto img = load_image(image_path);

    std::unique_ptr<TSCObjectDetector>  internal_detector((TSCObjectDetector* /*HACK*/)det->GetDetector(0));

    // Detach detector from list.
    det->RemoveDetector(0);
    
    //TGpuEngine engine(2, 8, 16192);
    TGpuEngine engine(4, 2, 16192, 3);
    //TGpuEngine engine(4, 32, 64);

    if (!engine.Initialize(std::move(internal_detector))) {
        std::cout << "Cant initialize GPU detector!" << std::endl;
    }

    TGpuIntegral integral;

    if (!integral.Update(img.get())) {
        std::cout << "Cant initialize GPU image!" << std::endl;
    }

    auto begin = std::chrono::high_resolution_clock::now();

    int count = 100;

    for (int i = 0; i < count; ++i) {

        auto result = engine.Detect(integral);

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
    int count = 10;
    for (int i = 0; i < count; ++i) {
        apply_detector(det.get(), img.get());
    }
    

    auto end = std::chrono::high_resolution_clock::now();
    auto dur = end - begin;
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
    std::cout << "Execution duration " << ms / count  << " ms." << std::endl;

    return 0;

}


int main()
{    
    return test_gpu_engine("test/detector.xml", "test/test.awp");
    //return test_ñpu_engine("test/detector.xml", "test/test.awp");
   
  
}