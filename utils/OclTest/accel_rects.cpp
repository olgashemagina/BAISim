#include "accel_rects.h"

#include <CL/cl.hpp>
#include <atomic>
#include <thread>
#include <mutex>



static const char cl_source_code[] =
" const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;\n"

"float calc_rect_norm(const image2d_t integral, const float4 rect, const float4 transform) {\n"
"    int4 transformed;\n"
"    ulong abcd[4];\n"
//Transform rect
"    transformed.lo = convert_int2_rte(rect.lo * transform.lo + transform.hi);\n"
"    transformed.hi = convert_int2_rte(rect.hi * transform.lo + transform.hi);\n"

"    abcd[0] = as_ulong(convert_ushort4(read_imageui(integral, sampler, transformed.lo)));\n"
"    abcd[1] = as_ulong(convert_ushort4(read_imageui(integral, sampler, transformed.hi)));\n"
"    abcd[2] = as_ulong(convert_ushort4(read_imageui(integral, sampler, transformed.zy)));\n"
"    abcd[3] = as_ulong(convert_ushort4(read_imageui(integral, sampler, transformed.xw)));\n"
//"    printf(\"ABCD: %llu %llu %llu %llu \\n\", abcd[0], abcd[1], abcd[2], abcd[3]);\n"

"    ulong sum = abcd[0] + abcd[1] - abcd[2] - abcd[3];\n"

//Normalization by area
//TODO: improve, use modulo
"    float2 rect_size = convert_float2(transformed.hi - transformed.lo);\n"
//"    float area = rect_size.x * rect_size.y;\n"
//"    printf(\"Rect Size: %f %f %f\\n\", rect_size[0], rect_size[1], area);\n"
"    float result = convert_float(sum) / (rect_size.x * rect_size.y);\n"
//"    printf(\"RESULT: %f %llu\\n\", result, sum);\n"
"    return result; \n"
"}\n"
/*
* integral - 2d image of uint16 * 4 channels (RGBA) = 512bits = 2e9. Image of 4 channel of 16 bit integers contains splitted uint64 value as (R, G, B, A) == (0 1, 2 3, 4 5, 6 7) bytes sequence. Little-endian.
* feat_size - count of features for one transform
* rects - rectangles (xmin, ymin, xmax, ymax) with count 10*feat_size
* transforms - list of transforms as (xscale, yscale, xtranslate, ytranslate).
* features - list of feature result (indices) count = feat_size * trans_size
* NOTE: run as global kernel of size (feat_size, trans_size)
*/
"kernel void calc_sc_features(read_only const image2d_t integral, const int feats_size, global const float4 * rects, global const float4 * transforms, global ushort * features) {\n"

"    int feat_id = get_global_id(0);\n"
"    int trans_id = get_global_id(1);\n"
//"    printf(\"Starting calc features: grid %dx%d %d \",  feat_id, trans_id, feats_size);\n"
//Use only for SC features
"    const float4 * feat_rects = rects + feat_id * 10;\n"

"    float base = calc_rect_norm(integral, feat_rects[0], transforms[trans_id]);\n"
"    ushort feat_value = 0;\n"

"    size_t rect = 1;\n"
"    for (; rect < 9; ++rect) {\n"
"       float sum = calc_rect_norm(integral, feat_rects[rect], transforms[trans_id]);\n"
"       if (sum > base)\n"
"           feat_value |= 1;\n"

"       feat_value = feat_value << 1;\n"
"    }\n"

"    float sum = calc_rect_norm(integral, feat_rects[rect], transforms[trans_id]);\n"
"    if (sum > base)\n"
"       feat_value |= 1;\n"

"    features[feat_id + trans_id * feats_size] = feat_value;\n"
"}\n"
/*
* feats_size - count of features.
* stages_size - count of stages in detector
* features - precalculated values of features (0-512) (feats_size * trans_size)
* stages - start feature indices of stage [0, ..., feats_size] (stages_size + 1)
* resps - code table of responces. 512 bits per weak feature. (64 * feats_size)
* weights - weights of weaks. (feats_size).
* thres - thresholds of stages (stages_size)
* dets - result of detections (stages_size * thres_size)
*/
"kernel void calc_sc_detector(int feats_size, int stages_size, global const ushort* features, global const int* stages, global const uchar* resps, global const float* weights, global const float* thres, global uchar* dets) {\n"
    //Features have had (feats_size x trans_size) size
"    int stage_id = get_global_id(0);\n"
"    int trans_id = get_global_id(1);\n"

//"    printf(\"Starting calc features: grid %dx%d %d \",  stage_id, trans_id, feats_size);\n"

    //stages - number of feature to start stage. size of stages  - stages_size + 1;
    //dets - (stages_size x trans_size)
"    int stage_start = stages[stage_id];\n"
"    int stage_end = stages[stage_id + 1];\n"

"    const ushort* feats = features + feats_size * trans_id + stage_start;\n"
"    const uchar * code_table = resps + stage_start * 64;\n"
"    const float* w = weights + stage_start;\n"
"    uchar * d = dets + stages_size * trans_id;\n"

"    float activation = 0;\n"

"    for (int feat_id = stage_start; feat_id < stage_end; ++feat_id, ++feats, ++w, code_table += 64) {\n"
        //CS feature
"       ushort index = *feats;\n"

"       ushort byte_index = index >> 3;\n"
"       ushort byte_offset = index % 8;\n"

"       uchar value = (code_table[byte_index] >> byte_offset) & 1;\n"
"       activation += *w * value;\n"
"   }\n"
"   d[stage_id] = activation >= thres[stage_id] ? 1 : 0;\n"
"}\n"
;


#define CL_CHECK(expr, value)                                                    \
{     cl_int _err = (expr);                                            \
     if (_err != CL_SUCCESS) {                                                  \
       std::cout << "OpenCL Error: '" << #expr << "' returned " << _err << std::endl; \
       return value;                                                                 \
     }  \
}

#define CL_CHECK_BOOL(expr)  CL_CHECK(expr, false)

namespace accel_rects {

    template<typename T>
    class Pool {
    public:
        Pool(size_t limits = 8) : limits_(limits) {}

        template<typename ...A>
        T* Create(A &&... args) {
            std::unique_lock<std::mutex>     locker(mtx_);

            if (limits_ > 0 && objects_.size() - pool_.size() > limits_ ) {
                cv_.wait(locker);
            }

            if (pool_.empty()) {
                objects_.emplace_back(std::make_unique<T>(std::forward<A...>(args...)));
                pool_.push_back(objects_.back().get());
            }

            T* obj = pool_.back();
            pool_.pop_back();
            return obj;
        }

        void Release(T* obj) {
            std::lock_guard<std::mutex>     locker(mtx_);
            pool_.push_back(obj);
            cv_.notify_one();
        }

    private:
        std::vector<std::unique_ptr<T>> objects_;

        size_t                          limits_ = 0;

        std::vector<T*>                 pool_;
        std::mutex                      mtx_;
        std::condition_variable         cv_;
    };

    class Task;
    
    
    class Core : public std::enable_shared_from_this<Core> {
    public:
        Core(size_t tasks_limits = 8) : pool_(tasks_limits) {}

        ~Core() {
            queue_.finish();

            std::cout << "Tasks count " << tasks_count() << std::endl;
        }

        bool            Initialize(Detector&& detector) {
            detector_ = std::move(detector);

            std::vector<cl::Platform> all_platforms;
            cl::Platform::get(&all_platforms);
            if (all_platforms.empty()) {
                std::cout << "No platform found" << std::endl;
                return false;
            }

            for (auto platform : all_platforms) {
                std::cout << "Platform: " << platform.getInfo<CL_PLATFORM_NAME>() << std::endl;
            }

            cl::Platform default_platform = all_platforms[0];
            std::cout << "Using platform: " << default_platform.getInfo<CL_PLATFORM_NAME>() << std::endl;

            std::vector<cl::Device> all_devices;
            default_platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);
            if (all_devices.empty()) {
                std::cout << "No device found" << std::endl;
                return false;
            }

            device_ = all_devices[0];
            std::cout << "Using device: " << device_.getInfo<CL_DEVICE_NAME>() << std::endl;

            context_ = cl::Context({ device_ });

            cl::Program::Sources sources;

            sources.push_back({ cl_source_code, sizeof(cl_source_code) });

            program_ = cl::Program(context_, sources);

            if (program_.build({ device_ }) != CL_SUCCESS) {
                std::cout << "Error building: " << program_.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device_) << std::endl;
                return false;
            }


            const cl_queue_properties properties[] = { CL_QUEUE_PROPERTIES, CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE /*| CL_QUEUE_PROFILING_ENABLE */ , 0 };

            cl_int err = 0;

            queue_ = cl::CommandQueue(context_, device_, properties, &err);
            CL_CHECK_BOOL(err);

            rects_ = cl::Buffer(context_, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, 
                            sizeof(cl_float4) * detector_.rects().size(), (void*)detector_.rects().data(), &err);
            CL_CHECK_BOOL(err);

            stages_ = cl::Buffer(context_, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
                sizeof(cl_int) * detector_.stages().size(), (void*)detector_.stages().data(), &err);
            CL_CHECK_BOOL(err);

            weights_ = cl::Buffer(context_, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
                sizeof(cl_float) * detector_.weights().size(), (void*)detector_.weights().data(), &err);
            CL_CHECK_BOOL(err);

            thres_ = cl::Buffer(context_, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
                sizeof(cl_float) * detector_.thres().size(), (void*)detector_.thres().data(), &err);
            CL_CHECK_BOOL(err);
                        
            resps_ = cl::Buffer(context_, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
                sizeof(cl_uchar) * sizeof(sc_resps_t::value_type) * detector_.resps().size(), (void*)detector_.resps().data(), &err);
            CL_CHECK_BOOL(err);

            return true;

        }

        bool            SetIntegral(const uint64_t* ptr, size_t width, size_t height, size_t stride) {
            cl_int err = 0;

            integral_ = cl::Image2D(context_, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, { CL_RGBA, CL_UNSIGNED_INT16 }, width, height, stride, (void*)ptr, &err);

            CL_CHECK_BOOL(err);

            return true;
        }

        
        //cl::Context& context() { return context_; }
        const cl::Context& context() const { return context_; }

        const cl::Device& device() const { return device_; }

        cl::CommandQueue& queue() { return queue_; }
        const cl::CommandQueue& queue() const { return queue_; }

        cl::Program& program() { return program_; }
        const cl::Program& program() const { return program_; }

        //cl::Image2D& integral() { return integral_; }
        const cl::Image2D& integral() const { return integral_; }

        //cl::Buffer& rects() { return rects_; }
        const cl::Buffer& rects() const { return rects_; }

        int      feats_size() const { return detector_.feats_size(); }
        int      stages_size() const { return detector_.stages_size(); }


        const cl::Buffer& stages() const { return stages_; }
        const cl::Buffer& weights() const { return weights_; }
        const cl::Buffer& thres() const { return thres_; }
        const cl::Buffer& resps() const { return resps_; }

    public:
        Task*     CreateTask() {
            return pool_.Create(shared_from_this());
        }

        void     ReleaseTask(Task* task) {
            pool_.Release(task);
        }



        void     AddTask() {
            tasks_count_.fetch_add(1, std::memory_order_relaxed);
        }

        void     ReleaseTask() {
            tasks_count_.fetch_sub(1, std::memory_order_relaxed);
        }

        int     tasks_count() const { return tasks_count_.load(std::memory_order_acquire); }

    public:
        const Detector& detector() const { return detector_; }


    private:
        cl::Context                 context_;
        cl::Device                  device_;
        //Source code of cl program
        cl::Program                 program_;
        cl::CommandQueue            queue_;

        //Integral image
        cl::Image2D                 integral_;
        //Rects list
        cl::Buffer                  rects_;
        //Cascades of features;
        cl::Buffer                  stages_;
        //Weights of features
        cl::Buffer                  weights_;
        //Threshold for each cascade;
        cl::Buffer                  thres_;
        //Responces of weak features;
        cl::Buffer                  resps_;

        int                         feats_size_ = 0;
        int                         stages_size_ = 0;

        Detector                    detector_;

        std::atomic_int32_t         tasks_count_ = 0;

        Pool<Task>                  pool_;
    };

    class Kernels {
    public:
        struct Data {
            //Transforms
            cl::Buffer      trans;
            //Features result
            cl::Buffer      feats;
            //Detections;
            cl::Buffer      dets;
        };
    public:
        Kernels(const Core& core) {
            cl_int err = 0;
            feats_kernel_ = cl::Kernel(core.program(), "calc_sc_features", &err);
            dets_kernel_ = cl::Kernel(core.program(), "calc_sc_detector", &err);

            const cl_queue_properties properties[] = { CL_QUEUE_PROPERTIES, CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE /*| CL_QUEUE_PROFILING_ENABLE */ , 0 };
                        
            queue_ = cl::CommandQueue(core.context(), core.device(), properties, &err);
        }


        cl::Kernel& feats_kernel() { return feats_kernel_; }
        const cl::Kernel& feats_kernel() const { return feats_kernel_; }

        cl::Kernel& dets_kernel() { return dets_kernel_; }
        const cl::Kernel& dets_kernel() const { return dets_kernel_; }

        const cl::CommandQueue& queue() const { return queue_; }
        cl::CommandQueue& queue() { return queue_; }



    private:
        cl::Kernel          feats_kernel_;
        cl::Kernel          dets_kernel_;

        cl::CommandQueue            queue_;
    };

    class Task {

    public:
        Task(std::shared_ptr<Core> core)
            : core_(core)
        {   }

        void        OnComplete() {
            auto core = core_.lock();
            
            if (cb_)
                cb_(trans_.size(), core->stages_size(), core->feats_size(), std::move(dets_), std::move(feats_));
            core->ReleaseTask(this);
        }

        
        bool      Enqueue(const Kernels& kernels, transforms_t&& transforms, callback_t callback) {
            cl_int err = 0;

            auto core = core_.lock();

            if (!core) return false;

            cb_ = std::move(callback);

            if (transforms.size() > trans_.size()) {
                trans_mem_ = cl::Buffer(core->context(), CL_MEM_READ_ONLY, sizeof(cl_float4) * transforms.size(), 0, &err);
                CL_CHECK_BOOL(err);
            }
            trans_ = std::move(transforms);

            if (core->feats_size() * trans_.size() > feats_.size()) {
                feats_.resize(core->feats_size() * trans_.size(), 0);
                feats_mem_ = cl::Buffer(core->context(), CL_MEM_READ_WRITE, sizeof(cl_ushort) * feats_.size(), 0, &err);
                CL_CHECK_BOOL(err);
            }

            if (core->stages_size() * trans_.size() > dets_.size()) {
                dets_.resize(core->stages_size() * trans_.size(), 0);
                dets_mem_ = cl::Buffer(core->context(), CL_MEM_WRITE_ONLY, sizeof(cl_uchar) * dets_.size(), 0, &err);
                CL_CHECK_BOOL(err);
            }
            

            std::vector<cl::Event>     waiting_events;

            cl::Event feats_evt, dets_evt, read_feats_evt, read_dets_evt, evt_write;
                        

            CL_CHECK_BOOL(core->queue().enqueueWriteBuffer(trans_mem_, CL_FALSE, 0, sizeof(cl_float4) * trans_.size(), trans_.data(), 0, &evt_write));

            waiting_events.push_back(evt_write);

            cl::Kernel feats_kernel = kernels.feats_kernel();
            CL_CHECK_BOOL(feats_kernel.setArg(0, core->integral()));
            CL_CHECK_BOOL(feats_kernel.setArg<int>(1, int(core->feats_size())));
            CL_CHECK_BOOL(feats_kernel.setArg(2, core->rects()));
            CL_CHECK_BOOL(feats_kernel.setArg(3, trans_mem_));
            CL_CHECK_BOOL(feats_kernel.setArg(4, feats_mem_));

            CL_CHECK_BOOL(core->queue().enqueueNDRangeKernel(feats_kernel, cl::NullRange, cl::NDRange(core->feats_size(), trans_.size()), cl::NullRange, &waiting_events, &feats_evt));
        
            waiting_events.clear();
            waiting_events.push_back(feats_evt);

            CL_CHECK_BOOL(core->queue().enqueueReadBuffer(feats_mem_, CL_FALSE, 0, sizeof(cl_ushort) * feats_.size(), feats_.data(), &waiting_events, &read_feats_evt));

            cl::Kernel dets_kernel = kernels.dets_kernel();

            CL_CHECK_BOOL(dets_kernel.setArg<int>(0, core->feats_size()));
            CL_CHECK_BOOL(dets_kernel.setArg<int>(1, core->stages_size()));
            CL_CHECK_BOOL(dets_kernel.setArg(2, feats_mem_));
            CL_CHECK_BOOL(dets_kernel.setArg(3, core->stages()));
            CL_CHECK_BOOL(dets_kernel.setArg(4, core->resps()));
            CL_CHECK_BOOL(dets_kernel.setArg(5, core->weights()));
            CL_CHECK_BOOL(dets_kernel.setArg(6, core->thres()));
            CL_CHECK_BOOL(dets_kernel.setArg(7, dets_mem_));

            
            waiting_events.clear();
            waiting_events.push_back(feats_evt);

            CL_CHECK_BOOL(core->queue().enqueueNDRangeKernel(dets_kernel, cl::NullRange, cl::NDRange(core->stages_size(), trans_.size()), cl::NullRange, &waiting_events, &dets_evt));

            waiting_events.clear();
            waiting_events.push_back(dets_evt);

            CL_CHECK_BOOL(core->queue().enqueueReadBuffer(dets_mem_, CL_FALSE, 0, sizeof(cl_uchar) * dets_.size(), dets_.data(), &waiting_events, &read_dets_evt));

            waiting_events.clear();
            waiting_events.push_back(read_dets_evt);
            waiting_events.push_back(read_feats_evt);
            CL_CHECK_BOOL(core->queue().enqueueMarkerWithWaitList(&waiting_events, &this->evt_complete_));
            
            evt_complete_ = read_feats_evt;

            CL_CHECK_BOOL(evt_complete_.setCallback(CL_COMPLETE, &Task::EvtCallback, this));

            //core_->AddTask();

            return true;
        }

        /*bool      Enqueue(const Kernels& kernels, transforms_t&& transforms) {
            trans_ = std::move(transforms);
            feats_.resize(core_->feats_size() * trans_.size(), 0);
            dets_.resize(core_->stages_size() * trans_.size(), 0);

            std::vector<cl::Event>     waiting_events;

            cl::Event feats_evt, dets_evt, read_feats_evt, read_dets_evt, evt_write;

            cl_int err = 0;

            //trans_mem_ = cl::Buffer(core_->context(), CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(cl_float4) * trans_.size(), trans_.data(), &err);
            trans_mem_ = cl::Buffer(core_->context(), CL_MEM_READ_ONLY, sizeof(cl_float4) * trans_.size(), 0, &err);
            CL_CHECK_BOOL(err);
            feats_mem_ = cl::Buffer(core_->context(), CL_MEM_READ_WRITE, sizeof(cl_ushort) * feats_.size(), 0, &err);
            CL_CHECK_BOOL(err);
            dets_mem_ = cl::Buffer(core_->context(), CL_MEM_WRITE_ONLY, sizeof(cl_uchar) * dets_.size(), 0, &err);
            CL_CHECK_BOOL(err);


            CL_CHECK_BOOL(kernels.queue().enqueueWriteBuffer(trans_mem_, CL_FALSE, 0, sizeof(cl_float4) * trans_.size(), trans_.data(), 0, &evt_write));

            waiting_events.push_back(evt_write);

            cl::Kernel feats_kernel = kernels.feats_kernel();
            CL_CHECK_BOOL(feats_kernel.setArg(0, core_->integral()));
            CL_CHECK_BOOL(feats_kernel.setArg<int>(1, int(core_->feats_size())));
            CL_CHECK_BOOL(feats_kernel.setArg(2, core_->rects()));
            CL_CHECK_BOOL(feats_kernel.setArg(3, trans_mem_));
            CL_CHECK_BOOL(feats_kernel.setArg(4, feats_mem_));

            CL_CHECK_BOOL(kernels.queue().enqueueNDRangeKernel(feats_kernel, cl::NullRange, cl::NDRange(core_->feats_size(), trans_.size()), cl::NullRange, &waiting_events, &feats_evt));
            //CL_CHECK_BOOL(core_->queue().enqueueNDRangeKernel(feats_kernel, cl::NullRange, cl::NDRange(core_->feats_size(), trans_.size()), cl::NullRange, 0, &feats_evt));

            waiting_events.clear();
            waiting_events.push_back(feats_evt);

            CL_CHECK_BOOL(kernels.queue().enqueueReadBuffer(feats_mem_, CL_FALSE, 0, sizeof(cl_ushort) * feats_.size(), feats_.data(), &waiting_events, &read_feats_evt));

            cl::Kernel dets_kernel = kernels.dets_kernel();

            CL_CHECK_BOOL(dets_kernel.setArg<int>(0, core_->feats_size()));
            CL_CHECK_BOOL(dets_kernel.setArg<int>(1, core_->stages_size()));
            CL_CHECK_BOOL(dets_kernel.setArg(2, feats_mem_));
            CL_CHECK_BOOL(dets_kernel.setArg(3, core_->stages()));
            CL_CHECK_BOOL(dets_kernel.setArg(4, core_->resps()));
            CL_CHECK_BOOL(dets_kernel.setArg(5, core_->weights()));
            CL_CHECK_BOOL(dets_kernel.setArg(6, core_->thres()));
            CL_CHECK_BOOL(dets_kernel.setArg(7, dets_mem_));


            waiting_events.clear();
            waiting_events.push_back(feats_evt);

            CL_CHECK_BOOL(kernels.queue().enqueueNDRangeKernel(dets_kernel, cl::NullRange, cl::NDRange(core_->stages_size(), trans_.size()), cl::NullRange, &waiting_events, &dets_evt));

            waiting_events.clear();
            waiting_events.push_back(dets_evt);

            CL_CHECK_BOOL(kernels.queue().enqueueReadBuffer(dets_mem_, CL_FALSE, 0, sizeof(cl_uchar) * dets_.size(), dets_.data(), &waiting_events, &read_dets_evt));

            waiting_events.clear();
            waiting_events.push_back(read_dets_evt);
            waiting_events.push_back(read_feats_evt);
            CL_CHECK_BOOL(const_cast<cl::CommandQueue&>(kernels.queue()).enqueueMarkerWithWaitList(&waiting_events, &this->evt_complete_));

            CL_CHECK_BOOL(evt_complete_.setCallback(CL_COMPLETE, &Task::EvtCallback, this));

            core_->AddTask();

            return true;
        }*/

    private:

        static void __stdcall EvtCallback(cl_event event, cl_int status, void* user_data) {
            auto task = static_cast<Task*>(user_data);
            task->OnComplete();
        }

    public:
        std::weak_ptr<Core>             core_;

        transforms_t                    trans_;

        features_t                      feats_;
        detections_t                    dets_;

        callback_t                      cb_;

        cl::Buffer                      trans_mem_;
        cl::Buffer                      feats_mem_;
        cl::Buffer                      dets_mem_;

        cl::Event                       evt_complete_;

    };

    Engine Engine::Create(Detector&& detector, size_t tasks_limits = 8)
    {
        auto core = std::make_shared<Core>(tasks_limits);
        if (core->Initialize(std::move(detector)))
            return Engine(core);
        return Engine(nullptr);
    }

    Worker Engine::CreateWorker() { return core_; }

       

    Worker::Worker(std::shared_ptr<Core> core) : core_(core) {
        kernels_ = std::make_unique<Kernels>(*core_);
    }

    Worker::Worker(Worker&& other) {
        core_ = std::move(other.core_);
        kernels_ = std::move(other.kernels_);
    }

    Worker::~Worker() { }

    bool Worker::Enqueue(transforms_t&& trans, callback_t cb) {
        auto task = core_->CreateTask();
        return task->Enqueue(*kernels_, std::move(trans), cb);
        //std::unique_ptr<Task> task = std::make_unique<Task>(core_);

        //return task.release()->Enqueue(*kernels_, std::move(trans), cb);

    }


    bool Engine::SetIntegral(const uint64_t* ptr, size_t width, size_t height, size_t stride)
    {
        return core_->SetIntegral(ptr, width, height, stride);
    }
      
}