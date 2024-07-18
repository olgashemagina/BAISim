#include <iostream>
#include <CL/cl.hpp>

#include <vector>
#include <array>

#include <windows.h>
#include <random>

#include <thread>

#include "accel_rects.h"


/*
void __stdcall Callback(cl_event event, cl_int status, void* user_data) {
    std::cout << "Callback status=" << status << std::endl;
    int* C = (int*)user_data;
    for (int i = 0; i < 10; ++i)
    {
        std::cout << C[i] << "\t";
    }
    std::cout << std::endl;
}

int test1() {
    std::vector<cl::Platform> all_platforms;
    cl::Platform::get(&all_platforms);
    if (all_platforms.empty())
    {
        std::cout << "No platform found" << std::endl;
        exit(1);
    }

    for (auto platform : all_platforms) {
        std::cout << "Platform: " << platform.getInfo<CL_PLATFORM_NAME>() << std::endl;
    }

    cl::Platform default_platform = all_platforms[0];
    std::cout << "Using platform: " << default_platform.getInfo<CL_PLATFORM_NAME>() << std::endl;

    std::vector<cl::Device> all_devices;
    default_platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);
    if (all_devices.empty())
    {
        std::cout << "No device found" << std::endl;
        exit(1);
    }

    cl::Device default_device = all_devices[0];
    std::cout << "Using device: " << default_device.getInfo<CL_DEVICE_NAME>() << std::endl;

    cl::Context context({ default_device });

    cl::Program::Sources sources;

    sources.push_back({ cl_source_code, sizeof(cl_source_code) });

    cl::Program program(context, sources);

    if (program.build({ default_device }) != CL_SUCCESS)
    {
        std::cout << "Error building: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(default_device) << std::endl;
        exit(1);
    }


    cl::Image2D integral_image(context, CL_MEM_READ_ONLY, { CL_RGBA, CL_UNSIGNED_INT16 }, 1280, 720);

    int feats_size = 1000;
    int trans_size = 100;
    std::vector<cl_float4> rects_host(feats_size * 10, { 0, 0, 0.5, 0.5 });
    std::vector<cl_float4> trans_host(trans_size, { 640, 480, 0, 0 });
    std::vector<cl_float> feats_host(feats_size, 0.f);

    cl::Buffer rects_buf(context, CL_MEM_READ_ONLY, sizeof(cl_float4) * rects_host.size());
    cl::Buffer trans_buf(context, CL_MEM_READ_ONLY, sizeof(cl_float4) * trans_host.size());
    cl::Buffer feats_buf(context, CL_MEM_WRITE_ONLY, sizeof(cl_float) * feats_host.size());


    const cl_queue_properties properties[] = { CL_QUEUE_PROPERTIES, CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE , 0 };

    cl_int err = 0;

    cl::CommandQueue queue(context, default_device, properties, &err);


    auto props2 = queue.getInfo<CL_QUEUE_PROPERTIES>();

    //Setup integral image

    std::array<size_t, 3> origin = { 0, 0, 0 };
    std::array<size_t, 3> region = { 1280, 720, 1 };


    err = queue.enqueueFillImage(integral_image, cl_uint4({ 0,0,0,0 }), (const cl::size_t< 3>&)origin, (const cl::size_t<3>&)region);

    err = queue.enqueueWriteBuffer(rects_buf, CL_TRUE, 0, sizeof(cl_float4) * rects_host.size(), rects_host.data());



    std::vector<cl::Event>     waiting_events;

    cl::Event evt_write, evt_read, evt_kernel;

    err = queue.enqueueWriteBuffer(trans_buf, CL_FALSE, 0, sizeof(cl_float4) * trans_host.size(), trans_host.data(), 0, &evt_write);

    waiting_events.push_back(evt_write);

    cl::Kernel kernel_calc_sc_features(program, "calc_sc_features", &err);
    err = kernel_calc_sc_features.setArg(0, integral_image);
    err = kernel_calc_sc_features.setArg(1, feats_size);
    err = kernel_calc_sc_features.setArg(2, rects_buf);
    err = kernel_calc_sc_features.setArg(3, trans_buf);
    err = kernel_calc_sc_features.setArg(4, feats_buf);

    err = queue.enqueueNDRangeKernel(kernel_calc_sc_features, cl::NullRange, cl::NDRange(feats_size, trans_size), cl::NullRange, &waiting_events, &evt_kernel);

    waiting_events.push_back(evt_kernel);


    err = queue.enqueueReadBuffer(feats_buf, CL_FALSE, 0, sizeof(cl_float) * feats_host.size(), feats_host.data(), &waiting_events, &evt_read);

    err = evt_read.setCallback(CL_COMPLETE, &Callback, feats_host.data());

    //queue.flush();
    queue.finish();

    ::Sleep(2000);

    //evt_read.wait();

    

    //exit(0);
    return 0;

}*/

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


int main()
{
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

    accel_rects::Detector   det; 
    det.Begin();

    for (int s = 0; s < stages_size; s++) {
        det.BeginStage();

        for (int f = 0; f < feats_size; ++f) {
            det.AddSCWeak(1, table, 0, 0, 0.23f, 0.23f);
            det.AddSCWeak(1, table, 0.1, 0.1, 0.27f, 0.27f);
            det.AddSCWeak(1, table, 0.05, 0.05, 0.2f, 0.2f);
        }
        det.EndStage(1);
    }
    det.End();
    
    auto engine = accel_rects::Engine::Create(std::move(det));


    if (!engine.is_valid()) {
        return -1;
    }

    std::cout << "Detector created!" << std::endl;

    if (!engine.SetIntegral(integral.data(), width, height, width*sizeof(uint64_t))) {
        return -2;
    }

    auto worker = engine.CreateWorker();

    accel_rects::callback_t cb = [](size_t trans_size, size_t stages_size, size_t feats_size, accel_rects::detections_t&& dets, accel_rects::features_t&& feats) {
        
        std::cout << "Callback " << trans_size << " : " << stages_size << " : " << feats_size << ". " << std::this_thread::get_id() << std::endl;
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

    for (int t = 0; t < 8; ++t) {
        threads.emplace_back([&]() {
            auto worker = engine.CreateWorker();
            for (int z = 0; z < 25; ++z) {
                accel_rects::transforms_t trans(trans_size, { float(width), float(height), 0, 0 });

                for (int i = 0; i < trans_size; ++i) {
                    trans[i][2] = 0;
                    trans[i][3] = 0;
                }

                worker.Enqueue(std::move(trans), cb);
                std::cout << "Added " << z << std::endl;
            }

        });
        threads.back().detach();
    }
    /*
    for (int z = 0; z < 100; ++z) {
        accel_rects::transforms_t trans(trans_size, { float(width), float(height), 0, 0 });

        for (int i = 0; i < trans_size; ++i) {
            trans[i][2] = i;
            trans[i][3] = i;
        }

        worker.Enqueue(std::move(trans), cb);
        std::cout << "Added " << z << std::endl;
    }*/

    std::cout << "FINISHED!" << std::endl;

    ::Sleep(10000);

    return 0;
  
}