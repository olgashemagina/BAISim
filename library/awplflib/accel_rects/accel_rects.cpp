#include "accel_rects.h"

#include <CL/cl.hpp>
#include <atomic>
#include <thread>
#include <mutex>
#include <variant>


#define MEASURE_COMPRESS_FACTOR         100.f


#define VALUE(string) #string
#define TO_LITERAL(string) VALUE(string)

static const char cl_source_code[] =
" const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;\n"
//"#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics : enable\n"

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

"    ulong sum = abcd[0] + abcd[1] - abcd[2] - abcd[3];\n"
//"    printf(\"ID: %d SUM=%llu ABCD: %llu %llu %llu %llu COORD: %d %d  %d %d\\n\", id, sum, abcd[0], abcd[1], abcd[2], abcd[3], transformed[0], transformed[1], transformed[2], transformed[3]);\n"

//Normalization by area
//TODO: improve, use modulo
"    float2 rect_size = convert_float2(transformed.hi - transformed.lo);\n"
//"    float area = rect_size.x * rect_size.y;\n"
//"    printf(\"Rect Size: %f %f %f\\n\", rect_size[0], rect_size[1], area);\n"
"    float result = convert_float(sum);"// / (rect_size.x * rect_size.y);\n"
//"    printf(\"RESULT: %f %llu\\n\", result, sum);\n"
"    return result; \n"
"}\n"

"float sum_rect_norm(const image2d_t integral, const int4 rect) {\n"
"    ulong abcd[4];\n"

"    abcd[0] = as_ulong(convert_ushort4(read_imageui(integral, sampler, rect.lo)));\n"
"    abcd[1] = as_ulong(convert_ushort4(read_imageui(integral, sampler, rect.hi)));\n"
"    abcd[2] = as_ulong(convert_ushort4(read_imageui(integral, sampler, rect.zy)));\n"
"    abcd[3] = as_ulong(convert_ushort4(read_imageui(integral, sampler, rect.xw)));\n"

"    ulong sum = abcd[0] + abcd[1] - abcd[2] - abcd[3];\n"
//"    printf(\"ID: %d SUM=%llu ABCD: %llu %llu %llu %llu COORD: %d %d  %d %d\\n\", id, sum, abcd[0], abcd[1], abcd[2], abcd[3], rect[0], rect[1], rect[2], rect[3]);\n"

//Normalization by area
//TODO: improve, use modulo
"    float2 rect_size = convert_float2(rect.hi - rect.lo);\n"
//"    float area = rect_size.x * rect_size.y;\n"
//"    printf(\"Rect Size: %f %f %f\\n\", rect_size[0], rect_size[1], area);\n"
"    float result = convert_float(sum) / (rect_size.x * rect_size.y);\n"
//"    printf(\"RESULT: %f %llu\\n\", result, sum);\n"
"    return result; \n"
"}\n"

// Store obly two signs after the dot;
"ushort pack_measure(float value) {\n"
"   return convert_ushort(value * "
TO_LITERAL(MEASURE_COMPRESS_FACTOR)
"); \n"
"}\n"

/*
* integral - 2d image of uint16 * 4 channels (RGBA) = 512bits = 2e9. Image of 4 channel of 16 bit integers contains splitted uint64 value as (R, G, B, A) == (0 1, 2 3, 4 5, 6 7) bytes sequence. Little-endian.
* feat_size - count of features for one transform
* rects - rectangles (xmin, ymin, width, height) - base unit of rect
* transforms - list of transforms as (xscale, yscale, xtranslate, ytranslate).
* feats - one value of feature coded as ushort(float * 100 )list of feature result (indices) count = feat_size * trans_size
* NOTE: run as global kernel of size (feat_size, trans_size)
*/
"ushort calc_census_feature(read_only const image2d_t integral, global const float4* feat_rect, global const float4* transform, global ushort * feats) {\n"

//Use only for SC features
//"    float4 feat_rect = rects[feat_id];\n"
//"    float4 transform = transforms[trans_id];\n"

"    float4 transformed;\n"

//Transform rect
"   transformed.lo = feat_rect->lo * transform->lo + transform->hi;\n"
"   transformed.hi = feat_rect->hi * transform->lo;\n"
//"    int4 base_rect = convert_int4_rte(transformed);\n"
"   int4 base_rect = convert_int4_rtz(transformed + 0.5f);\n"
"   int2 rect_size = base_rect.hi;\n"
"   int2 full_size = base_rect.hi * 3;\n"
"   float total = 0;\n"

//"   float* feats = features + feat_id + trans_id * feats_size;\n"  

"   float base = sum_rect_norm(integral, (int4)(base_rect.lo, base_rect.lo + full_size));\n"
//"   *(feats++) = base;\n"  
"   feats[0] = pack_measure(base);\n"
"   ushort feat_value = 0;\n"
"   int2 pos = base_rect.lo;\n"
"   for (int j = 0; j < 2; ++j, pos.y += rect_size.y, pos.x = base_rect.x) {\n"

"       for (int i = 0; i < 3; ++i, pos.x += rect_size.x) {\n"
"           float sum = sum_rect_norm(integral, (int4)(pos, pos + rect_size));\n"
"           if (sum > base)\n"
"               feat_value |= 1;\n"
"           total += sum;\n"
"           feat_value = feat_value << 1;\n"
//"           *(feats++)=sum;\n"

"       }\n"

"   }\n"

"   for (int i = 0; i < 2; ++i, pos.x += rect_size.x) {\n"
"       float sum = sum_rect_norm(integral, (int4)(pos, pos + rect_size));\n"
"       if (sum > base)\n"
"           feat_value |= 1;\n"
"       total += sum;\n"
//"       *(feats++)=sum;\n"
"       feat_value = feat_value << 1;\n"
"   }\n"

"   float sum = sum_rect_norm(integral, (int4)(pos, pos + rect_size));\n"
"   if (sum > base)\n"
"       feat_value |= 1;\n"
//"   *(feats++)=sum;\n"
"   total += sum;\n"

//"    printf(\"Feature: id: %d v: %d R: %f %f %f %f %f %f %f %f %f %f\\n \",  feat_id, feat_value, f[0], f[1], f[2], f[3], f[4], f[5], f[6], f[7], f[8], f[9]);\n"
//"    printf(\"Feature: id: %d v: %d . B= %f , Total= %f; TRANS: %f %f %f %f\\n \",  feat_id, feat_value, base, total / 9.f, transformed[0], transformed[1], transformed[2], transformed[3]);\n"
"    return feat_value;\n"
"}\n"

"kernel void init_dets_table(global int* dets_table) {dets_table[0] = 0;}\n"

/*
* feats_size - count of features.
* stages_size - count of stages in detector
* stages - start feature indices of stage [0, ..., feats_size] (stages_size + 1)
* resps - code table of responces. 512 bits per weak feature. (64 * feats_size)
* weights - weights of weaks. (feats_size).
* thres - thresholds of stages (stages_size)
* dets_table - index of returned stage ( stages_size*trans_size + 1). First int is top bounds of features.
* features - output of features  (feats_size * 10 * trans_size)
*/
"kernel void detect_frag(read_only const image2d_t integral, int min_stages, global const float4 * rects, global const float4 * transforms, "
"int stages_size, global const int* stages, global const uchar* resps, global const float* weights, global const float* thres, global int* dets_table, global ushort* features ) {\n"
 
//Features have had (feats_size x trans_size) size
"    int trans_id = get_global_id(0);\n"
"    int trans_offset = trans_id - get_global_offset(0);\n"




    //stages - number of feature to start stage. size of stages  - stages_size + 1;
    //dets - (stages_size x trans_size)
    // Mark that detector not triggered;

"   int feats_size = stages[stages_size];\n"
"   global int* dets = dets_table + (stages_size + 1)* trans_offset + 1;\n"
"   dets[0] = -1;\n"

// HACK:
//"dets[trans_id] = 0;return;"


"   for (int stage_id = 0; stage_id < stages_size; ++stage_id) {\n"


"       int stage_start = stages[stage_id];\n"
"       int stage_end = stages[stage_id + 1];\n"

"       const uchar * code_table = resps + stage_start * 64;\n"
"       const float* w = weights + stage_start;\n"

"       float activation = 0;\n"

//"    if (trans_id == 0) "
//"printf(\"Feat stage=%d place: pos %d \\n\",  stage_id, dets_table[0]);\n"
"       int feats_place = atomic_add(dets_table, (stage_end - stage_start));\n"

"       if (feats_place > feats_size * get_global_size(0)) {printf(\"Error in placing features. Place=%d, Count=%d.\", feats_place, get_global_size(0));}\n"      
//"       printf(\"Feat stage=%d place: %d size %d \\n\",  stage_id, feats_place, dets_table[0]);\n"
 
//"    if (trans_id == 0) printf(\"Feat stage=%d place: %d size %d \\n\",  stage_id, feats_place, dets_table[0]);\n"
"       global ushort* feats = features + feats_place;\n"
"       dets[stage_id+1] = feats_place;\n"

"       for (int feat_id = stage_start; feat_id < stage_end; ++feat_id, feats += 1, ++w, code_table += 64) {\n"
//CS feature
"           ushort index = calc_census_feature(integral, rects + feat_id, transforms + trans_id, feats); \n"
//"           ushort index = 0; \n"

"           ushort byte_index = index >> 3;\n"
"           ushort byte_offset = index % 8;\n"

"           uchar value = (code_table[byte_index] >> byte_offset) & 1;\n"
"           activation += *w * value;\n"
"       }\n"

"       if (activation < thres[stage_id] && dets[0] < 0)\n {"
"           dets[0] = stage_id; }\n"

"       if (dets[0] >= 0 && (stage_id + 1) >= min_stages)\n"
"           break;\n"
"   }\n"
//"    if (trans_id == 0) printf(\"Feat place: %d \",  dets_table[0]);\n"
"}\n"
// Packed detector due to limits of args in opencl kernel;
// |num_stages|thres_s1|feats_s1|rect_s1_1|weight_s1_1|code_table_s1_1|rect_s1_2|weights_s1_2|ct_s1_2|....
// |thres_s2|feats_s2|rect_s2_1 |ct_s2_1|...
/*"kernel void detect_frags_packed(read_only const image2d_t integral, int min_stages, global const float4 * transforms, global const uchar* detector_packed, "
"int feats_size, global int* dets, global float* features ) {\n"
//Features have had (feats_size x trans_size) size
"    int trans_id = get_global_id(0);\n"

//"    printf(\"Starting calc features: grid %dx%d %d \",  stage_id, trans_id, feats_size);\n"

    //stages - number of feature to start stage. size of stages  - stages_size + 1;
    //dets - (stages_size x trans_size)
    // Mark that detector not triggered;
    "   dets[trains_id] = -1;\n"
    "   float* feats = features + feats_size * 10 * trans_id;"
    "   int num_stages = *(global const int*)detector_packed; detector_packed+=sizeof(int);\n"
        
    "   for (int stage_id = 0; stage_id < stages_size; ++stage_id) {\n"

    "       int stage_start = stages[stage_id];\n"
    "       int stage_end = stages[stage_id + 1];\n"

    "       const uchar * code_table = resps + stage_start * 64;\n"
    "       const float* w = weights + stage_start;\n"

    "       float activation = 0;\n"

    "       for (int feat_id = stage_start; feat_id < stage_end; ++feat_id, feats += 10, ++w, code_table += 64) {\n"
    //CS feature
    "           ushort index = calc_census_feature(integral, rects[feat_id], transforms[trans_id], feats); \n"

    "           ushort byte_index = index >> 3;\n"
    "           ushort byte_offset = index % 8;\n"

    "           uchar value = (code_table[byte_index] >> byte_offset) & 1;\n"
    "           activation += *w * value;\n"
    "       }\n"
    "       if (activation < thres[stage_id] && dets[trans_id] < 0)\n"
    "           dets[trains_id] = stage_id;\n"

    "       if (dets[trans_id] >= 0 && (stage_id + 1) >= min_stages)\n"
    "           break;\n"
    "   }\n"
    "}\n"*/
;

#define CL_CHECK(expr, value)                                                    \
{     cl_int _err = (expr);                                            \
     if (_err != CL_SUCCESS) {                                                  \
       std::cout << "OpenCL Error: '" << #expr << "' returned " << _err << std::endl; \
       return value;                                                                 \
     }  \
}

#define CL_CHECK_RET(expr) CL_CHECK(expr, ;)

#define CL_CHECK_BOOL(expr)  CL_CHECK(expr, false)

namespace accel_rects {

    template<typename T>
    class Pool {
    public:
        Pool(size_t limits = 0) : limits_(limits) {}

        template<typename ...A>
        T* Create(A &&... args) {
            std::unique_lock<std::mutex>     locker(mtx_);

            while (limits_ > 0 && objects_.size() >= limits_ && pool_.size() == 0) {
                //                std::cout << "Waiting task..." << std::endl;
                cv_.wait(locker);
            }

            if (pool_.empty()) {
                objects_.emplace_back(std::make_unique<T>(args...));
                pool_.push_back(objects_.back().get());
            }

            tasks_count_.fetch_add(1, std::memory_order_relaxed);

            T* obj = pool_.back();
            pool_.pop_back();
            return obj;
        }

        void Release(T* obj) {
            std::lock_guard<std::mutex>     locker(mtx_);
            pool_.push_back(obj);
            tasks_count_.fetch_sub(1, std::memory_order_relaxed);
            cv_.notify_one();
        }

        int     tasks_count() const { return tasks_count_.load(std::memory_order_acquire); }

        void    WaitTask() {
            std::unique_lock<std::mutex>     locker(mtx_);

            if (objects_.size() > pool_.size()) {
                cv_.wait(locker);
            }
        }

        void    WaitAllTasks() {
            std::unique_lock<std::mutex>     locker(mtx_);

            while (objects_.size() > pool_.size()) {
                cv_.wait(locker);
            }
        }

    private:
        std::vector<std::unique_ptr<T>> objects_;

        size_t                          limits_ = 0;

        std::vector<T*>                 pool_;
        std::mutex                      mtx_;
        std::condition_variable         cv_;

        std::atomic_int32_t             tasks_count_ = 0;
    };

    class Task;

    /// <summary>
    /// IntegralData
    /// </summary>
    class IntegralData {
    public:
        IntegralData(const cl::Context& context, const uint64_t* ptr, size_t width, size_t height, size_t stride) {
            //stride must be equal or more than width * 8 bytes;
            integral_ = cl::Image2D(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, { CL_RGBA, CL_UNSIGNED_INT16 }, width, height, stride, (void*)ptr, &err_);
        }

        bool    is_valid() const { return err_ == CL_SUCCESS; }

        size_t  width() const {
            return integral_.getImageInfo<CL_IMAGE_WIDTH>();
        }

        size_t  height() const {
            return integral_.getImageInfo<CL_IMAGE_HEIGHT>();
        }

        const cl::Image2D& integral() const { return integral_; }

    private:
        cl_int                      err_ = CL_SUCCESS;

        //Integral image
        cl::Image2D                 integral_;

    };

    bool IntegralView::is_valid() const
    {
        return data_ ? data_->is_valid() : false;
    }

    size_t IntegralView::width() const {
        return data_ ? data_->width() : 0;

    }

    size_t IntegralView::height() const {
        return data_ ? data_->height() : 0;
    }

    /// <summary>
    /// TransformsData
    /// </summary>
    class TransformsData {
    public:
        TransformsData(const cl::Context& context, transforms_t&& trans)
            : transforms_(std::move(trans)) {
            trans_ = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
                sizeof(cl_float4) * transforms_.size(), (void*)transforms_.data(), &err_);
        }

        bool    is_valid() const { return err_ == CL_SUCCESS; }

        const transforms_t& transforms() const { return transforms_; }

        const cl::Buffer& trans_buffer() const { return trans_; }


    private:
        cl_int                      err_ = CL_SUCCESS;

        cl::Buffer                  trans_;
        transforms_t                transforms_;
    };

    const transforms_t& TransformsView::transforms() const
    {
        return data_->transforms();
    }

    bool TransformsView::is_valid() const
    {
        return data_ && data_->is_valid();
    }



    /// <summary>
    /// FeaturesData
    /// </summary>
    class FeaturesData {
    public:
        FeaturesData(const cl::Context& context, Features&& features) : features_(std::move(features)) {
            rects_ = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
                sizeof(cl_float4) * features_.rects().size(), (void*)features_.rects().data(), &err_);
        }

        bool    is_valid() const { return err_ == CL_SUCCESS; }

        const Features& features() const { return features_; }

        const cl::Buffer& rects() const { return rects_; }


    private:
        cl_int                      err_ = CL_SUCCESS;

        cl::Buffer                  rects_;
        Features                    features_;
    };


    const Features& FeaturesView::features() const {
        return data_->features();
    }

    bool FeaturesView::is_valid() const {
        return data_->is_valid();
    }

    /// <summary>
    /// StagesData
    /// </summary>
    class StagesData {
    public:
        StagesData(const cl::Context& context, Stages&& stages)
            : stages_(std::move(stages)) {

            positions_ = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
                sizeof(cl_int) * stages_.positions().size(), (void*)stages_.positions().data(), &err_);
            CL_CHECK_RET(err_);

            weights_ = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
                sizeof(cl_float) * stages_.weights().size(), (void*)stages_.weights().data(), &err_);
            CL_CHECK_RET(err_);

            thres_ = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
                sizeof(cl_float) * stages_.thres().size(), (void*)stages_.thres().data(), &err_);
            CL_CHECK_RET(err_);

            resps_ = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
                sizeof(cl_uchar) * sizeof(sc_resps_t::value_type) * stages_.resps().size(), (void*)stages_.resps().data(), &err_);
            CL_CHECK_RET(err_);
        }

        bool    is_valid() const { return err_ == CL_SUCCESS; }

        const Stages& stages() const { return stages_; }

        const cl::Buffer& positions() const { return positions_; }
        const cl::Buffer& thresholds() const { return thres_; }
        const cl::Buffer& weights() const { return weights_; }
        const cl::Buffer& responses() const { return resps_; }


    private:
        cl_int                      err_ = CL_SUCCESS;

        //Cascades of features;
        cl::Buffer                  positions_;
        //Weights of features
        cl::Buffer                  weights_;
        //Threshold for each cascade;
        cl::Buffer                  thres_;
        //Responces of weak features;
        cl::Buffer                  resps_;

        Stages                      stages_;
    };

    /// <summary>
    /// DetectorData
    /// </summary>
    class DetectorData {
    public:
        DetectorData(const cl::Context& context, std::pair<Stages, Features>&& detector)
            : stages_data_(context, std::move(detector.first))
            , features_data_(context, std::move(detector.second)) {
        }

        bool    is_valid() const { return stages_data_.is_valid() && features_data_.is_valid(); }

        const Stages& stages() const { return stages_data_.stages(); }
        const Features& features() const { return features_data_.features(); }

        const FeaturesData& features_data() const { return features_data_; }
        FeaturesData& features_data() { return features_data_; }

        const StagesData& stages_data() const { return stages_data_; }
        StagesData& stages_data() { return stages_data_; }


    private:

        FeaturesData                features_data_;
        StagesData                  stages_data_;
    };

    const Stages& DetectorView::stages() const {
        return data_->stages();
    }

    FeaturesView DetectorView::features_view() {
        return std::shared_ptr<FeaturesData>(data_, &data_->features_data());

    }

    bool DetectorView::is_valid() const
    {
        return data_ ? data_->is_valid() : false;
    }



    class Core : public std::enable_shared_from_this<Core> {
    public:
        Core() = default;

        ~Core() = default;

        void            Finish() {
            if (kernel_queue_.finish() != CL_SUCCESS && read_queue_.finish() != CL_SUCCESS && write_queue_.finish() != CL_SUCCESS) {
                
                std::cout << "Error while finishing Engine." << std::endl;
            }
        }


        bool            Initialize() {

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

            std::cout << "MaxArgs: " << device_.getInfo<CL_DEVICE_MAX_CONSTANT_ARGS>() << std::endl;
            std::cout << "Max Const Buffer Size: " << device_.getInfo<CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE>() << std::endl;

            std::cout << "Max Global mem Size: " << device_.getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>() << std::endl;

            std::cout << "Max Alloc Size: " << device_.getInfo<CL_DEVICE_MAX_MEM_ALLOC_SIZE>() << std::endl;
                        
            std::cout << "Max Compute Units: " << device_.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>() << std::endl;

            std::cout << "Max WARP_SIZE: " << device_.getInfo<CL_DEVICE_WARP_SIZE_NV>() << std::endl;

                       


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

            kernel_queue_ = cl::CommandQueue(context_, device_, properties, &err);
            CL_CHECK_BOOL(err);

            read_queue_ = cl::CommandQueue(context_, device_, properties, &err);
            CL_CHECK_BOOL(err);

            write_queue_ = cl::CommandQueue(context_, device_, properties, &err);
            CL_CHECK_BOOL(err);


            return true;

        }


        //cl::Context& context() { return context_; }
        const cl::Context& context() const { return context_; }

        const cl::Device& device() const { return device_; }

        cl::CommandQueue& kernel_queue() { return kernel_queue_; }
        const cl::CommandQueue& kernel_queue() const { return kernel_queue_; }

        cl::CommandQueue& read_queue() { return read_queue_; }
        const cl::CommandQueue& read_queue() const { return read_queue_; }

        cl::CommandQueue& write_queue() { return write_queue_; }
        const cl::CommandQueue& write_queue() const { return write_queue_; }

        cl::Program& program() { return program_; }
        const cl::Program& program() const { return program_; }

    public:
        bool ResizeBufferIfNeeded(cl::Buffer& buffer, cl_mem_flags flags, size_t bytes) {
            cl_int err = 0;
            auto size = buffer.getInfo<CL_MEM_SIZE>(&err);
            if (err != CL_SUCCESS || size < bytes) {

                buffer = cl::Buffer(context_, flags, bytes, 0, &err);
                CL_CHECK_BOOL(err);
                //if (err == CL_SUCCESS)
                //    std::cout << "Resizing from " << size << " to " << bytes << std::endl;
            }
            return true;

        }


        bool CreateKernel(const std::string& name, cl::Kernel& kernel) {
            cl_int err = 0;
            kernel = cl::Kernel(program_, name.c_str(), &err);

            if (err != CL_SUCCESS) {
                std::cout << "Error while creating kernel " << name << std::endl;
                return false;
            }
            return true;
        }



    private:
        cl::Context                 context_;
        cl::Device                  device_;
        //Source code of cl program
        cl::Program                 program_;
        cl::CommandQueue            kernel_queue_;

        cl::CommandQueue            read_queue_;
        cl::CommandQueue            write_queue_;
    };


    class TaskWorker : public Worker {


    public:
        TaskWorker() {}


        virtual ~TaskWorker() {}


        bool    Initialize(const std::shared_ptr<Core>& core) {
            core_ = core;
            return core_->CreateKernel("detect_frag", kernel_);
        }


    public:
        bool        Run(IntegralView integral, DetectorView detector, TransformsView trans, int min_stages, int begin_index, int end_index, triggered_t& triggered, features_t& features) override {
            cl_int err = 0;


            int stages_count = detector.stages().count();
            int measures_count = detector.data()->features_data().features().count();

            int trans_size = end_index - begin_index;

            int measures_size = measures_count * trans_size;
            size_t table_size = trans_size * (stages_count + 1) + 1;

            measures_.resize(measures_size, 0);
            dets_table_.resize(table_size, 0);

            
            if (!core_->ResizeBufferIfNeeded(measures_mem_, CL_MEM_WRITE_ONLY, sizeof(cl_float) * measures_.size()))
                return false;

            if (!core_->ResizeBufferIfNeeded(dets_table_mem_, CL_MEM_WRITE_ONLY, sizeof(cl_int) * dets_table_.size()))
                return false;

            cl::Event proc_evt, dets_evt, read_feats_evt, read_dets_evt, evt_write;


            // Reset index of packet features;
            CL_CHECK_BOOL(core_->write_queue().enqueueFillBuffer<int>(dets_table_mem_, 0, 0, sizeof(cl_int), 0, &evt_write));
            waiting_evts_.push_back(evt_write);

           
            //read_only const image2d_t integral, int min_stages, global const float4 * rects, global const float4 * transforms, 
            // int stages_size, global const int* stages, global const uchar* resps, global const float* weights, global const float* thres, global int* dets, global float* features


            CL_CHECK_BOOL(kernel_.setArg(0, integral.data()->integral()));
            CL_CHECK_BOOL(kernel_.setArg<int>(1, min_stages));
            CL_CHECK_BOOL(kernel_.setArg(2, detector.data()->features_data().rects()));
            CL_CHECK_BOOL(kernel_.setArg(3, trans.data()->trans_buffer()));
            CL_CHECK_BOOL(kernel_.setArg<int>(4, stages_count));
            CL_CHECK_BOOL(kernel_.setArg(5, detector.data()->stages_data().positions()));
            CL_CHECK_BOOL(kernel_.setArg(6, detector.data()->stages_data().responses()));
            CL_CHECK_BOOL(kernel_.setArg(7, detector.data()->stages_data().weights()));
            CL_CHECK_BOOL(kernel_.setArg(8, detector.data()->stages_data().thresholds()));
            CL_CHECK_BOOL(kernel_.setArg(9, dets_table_mem_));
            CL_CHECK_BOOL(kernel_.setArg(10, measures_mem_));


            CL_CHECK_BOOL(core_->kernel_queue().enqueueNDRangeKernel(kernel_, cl::NDRange(begin_index), cl::NDRange(end_index - begin_index), cl::NullRange, &waiting_evts_, &proc_evt));
            //CL_CHECK_BOOL(kernels.queue().enqueueNDRangeKernel(detect_kernel, cl::NDRange(begin_index), cl::NDRange(end_index - begin_index), cl::NullRange, &waiting_evts_, &proc_evt));

            waiting_evts_.clear();
            waiting_evts_.push_back(proc_evt);


            //CL_CHECK_BOOL(core->queue().enqueueReadBuffer(feats_mem_, CL_FALSE, 0, sizeof(cl_float) * feats_ptr_->size(), feats_ptr_->data(), &waiting_evts_, &read_feats_evt));
            //CL_CHECK_BOOL(core->queue().enqueueReadBuffer(feats_mem_, CL_FALSE, 0, sizeof(cl_half) * feats_ptr_->size() / 10, feats_ptr_->data(), &waiting_evts_, &read_feats_evt));
            //CL_CHECK_BOOL(core->queue().enqueueReadBuffer(dets_mem_, CL_FALSE, 0, sizeof(cl_int) * dets_ptr_->size(), dets_ptr_->data(), &waiting_evts_, &read_dets_evt));
            //CL_CHECK_BOOL(core->queue().enqueueReadBuffer(dets_mem_, CL_FALSE, 0, sizeof(cl_int) * dets_ptr_->size(), dets_ptr_->data(), &waiting_evts_, &read_dets_evt));

            // TODO: make big buffer and add all data here;


            CL_CHECK_BOOL(core_->read_queue().enqueueReadBuffer(dets_table_mem_, CL_TRUE, 0, sizeof(cl_int) * dets_table_.size(), dets_table_.data(), &waiting_evts_));

            int measures_pack_size = dets_table_.at(0);


            CL_CHECK_BOOL(core_->read_queue().enqueueReadBuffer(measures_mem_, CL_TRUE, 0, sizeof(cl_ushort) * measures_pack_size, measures_.data()));
            // const int* table = &dets_table_[1];
             //const uint16_t* measures_ptr = measures_.data();


             /*
             const int* dets_table_ptr = (const int*)core->read_queue().enqueueMapBuffer(dets_table_mem_, CL_TRUE, CL_MAP_READ, 0, sizeof(cl_int) * table_size, &waiting_evts_);

             int measures_pack_size = dets_table_ptr[0];
             const int* table = &dets_table_ptr[1];

             const uint16_t* measures_ptr = (const uint16_t*)core->read_queue().enqueueMapBuffer(measures_mem_, CL_TRUE, CL_MAP_READ, 0, sizeof(cl_ushort) * measures_pack_size);
             */

            DecodeMeasures(detector.stages().positions(), &dets_table_[1], measures_.data(), trans_size, min_stages, triggered, features);


            /*

            CL_CHECK_BOOL(core->read_queue().enqueueUnmapMemObject(measures_mem_, (void*)measures_ptr));

            // Cancel mapping of detection table
            CL_CHECK_BOOL(core->read_queue().enqueueUnmapMemObject(dets_table_mem_, (void*)dets_table_ptr));
            */


            return true;
        }

        private:
            void            DecodeMeasures(const positions_t& stages_positions,
                const int* table,
                const uint16_t* measures,
                int table_size,
                int min_stages,
                triggered_t& triggered,
                features_t& features) {

                triggered.resize(table_size, -1);
                features.resize(stages_positions.back() * table_size, 0);
                float* measures_output = features.data();
                size_t measures_size = features.size();
                size_t stages_plus_1 = stages_positions.size();



                for (int t = 0; t < table_size; ++t, table += stages_plus_1) {
                    // First number is number of stage triggered or -1;
                    int triggered_stage = table[0];
                    const int* stages_offset = table + 1;
                    int num_stages = triggered_stage < 0 ? stages_plus_1 - 1 : std::max<int>(triggered_stage + 1, min_stages);
                    triggered[t] = triggered_stage;


                    for (int s = 0; s < num_stages; ++s) {
                        int stage_size = stages_positions[s + 1] - stages_positions[s];

                        const uint16_t* stage_measures = measures + stages_offset[s];
                        for (int i = 0; i < stage_size; i++) {
                            measures_output[i] = float(stage_measures[i] / MEASURE_COMPRESS_FACTOR);
                        }
                    }
                    measures_output += stages_positions.back();
                    measures_size -= stages_positions.back();
                }
            }


    private:
        std::shared_ptr<Core>           core_;
        
        measures_t                      measures_;
        detections_t                    dets_table_;

        cl::Kernel                      kernel_;
        cl::Buffer                      measures_mem_;
        cl::Buffer                      dets_table_mem_;

        
        std::vector<cl::Event>          waiting_evts_;
    };
    /*

    class Task {
    public:
        using integral_ptr = std::shared_ptr<IntegralData>;
        using detector_ptr = std::shared_ptr<DetectorData>;
        using features_ptr = std::shared_ptr<FeaturesData>;
        using data_ptr_var = std::variant<std::shared_ptr<DetectorData>, std::shared_ptr<FeaturesData>>;

    public:
        Task() {   }

        static bool ResizeBufferIfNeeded(const cl::Context& context, cl::Buffer& buffer, cl_mem_flags flags, size_t bytes) {
            cl_int err = 0;
            auto size = buffer.getInfo<CL_MEM_SIZE>(&err);
            if (err != CL_SUCCESS || size < bytes) {

                buffer = cl::Buffer(context, flags, bytes, 0, &err);
                CL_CHECK_BOOL(err);
                //if (err == CL_SUCCESS)
                //    std::cout << "Resizing from " << size << " to " << bytes << std::endl;
            }
            return true;

        }

        // TODO: use one block of data for different calls and use atomics to split each call
        bool      EnqueueDetect(const std::shared_ptr<Core>& core,
            const IntegralData& integral,
            const DetectorData& detector,
            const TransformsData& transforms,
            const Kernels& kernels,
            int min_stages,
            int begin_index,
            int end_index,
            triggered_t& triggered,
            features_t& features) {
            cl_int err = 0;

          
            int stages_count = detector.stages().count();
            int measures_count = detector.features_data().features().count();
            
            int trans_size = end_index - begin_index;

            int measures_size = measures_count * trans_size;
            size_t table_size = trans_size * (stages_count + 1) + 1;
                        
            measures_.resize(measures_size, 0);
            //feats_ptr_->resize(feats_count * trans_.size(), 0);
                        
            dets_table_.resize(table_size, 0);

            //if (!ResizeBufferIfNeeded(core_->context(), trans_mem_, CL_MEM_READ_ONLY, sizeof(cl_float4) * trans_.size())) 
            //    return false;

            if (!ResizeBufferIfNeeded(core->context(), measures_mem_, CL_MEM_WRITE_ONLY, sizeof(cl_float) * measures_.size()))
                return false;

            if (!ResizeBufferIfNeeded(core->context(), dets_table_mem_, CL_MEM_WRITE_ONLY, sizeof(cl_int) * dets_table_.size()))
                return false;

            cl::Event proc_evt, dets_evt, read_feats_evt, read_dets_evt, evt_write;


            //CL_CHECK_BOOL(core->queue().enqueueWriteBuffer(trans_mem_, CL_FALSE, 0, sizeof(cl_float4) * trans_.size(), trans_.data(), 0, &evt_write));

            //core->queue().enqueueMapBuffer(dets_mem_, CL_FALSE, )

            //waiting_evts_.push_back(evt_write);

            //dets_ptr_->at(0) = 0;

            // Reset index of packet features;
            CL_CHECK_BOOL(core->write_queue().enqueueFillBuffer<int>(dets_table_mem_, 0, 0, sizeof(cl_int), 0, &evt_write));
            waiting_evts_.push_back(evt_write);

            cl::Kernel detect_kernel = kernels.detect_kernel();

            //read_only const image2d_t integral, int min_stages, global const float4 * rects, global const float4 * transforms, 
            // int stages_size, global const int* stages, global const uchar* resps, global const float* weights, global const float* thres, global int* dets, global float* features



            CL_CHECK_BOOL(detect_kernel.setArg(0, integral.integral()));
            CL_CHECK_BOOL(detect_kernel.setArg<int>(1, min_stages));
            CL_CHECK_BOOL(detect_kernel.setArg(2, detector.features_data().rects()));
            CL_CHECK_BOOL(detect_kernel.setArg(3, transforms.trans_buffer()));
            CL_CHECK_BOOL(detect_kernel.setArg<int>(4, stages_count));
            CL_CHECK_BOOL(detect_kernel.setArg(5, detector.stages_data().positions()));
            CL_CHECK_BOOL(detect_kernel.setArg(6, detector.stages_data().responses()));
            CL_CHECK_BOOL(detect_kernel.setArg(7, detector.stages_data().weights()));
            CL_CHECK_BOOL(detect_kernel.setArg(8, detector.stages_data().thresholds()));
            CL_CHECK_BOOL(detect_kernel.setArg(9, dets_table_mem_));
            CL_CHECK_BOOL(detect_kernel.setArg(10, measures_mem_));

            
            CL_CHECK_BOOL(core->kernel_queue().enqueueNDRangeKernel(detect_kernel, cl::NDRange(begin_index), cl::NDRange(end_index - begin_index), cl::NullRange, &waiting_evts_, &proc_evt));
            //CL_CHECK_BOOL(kernels.queue().enqueueNDRangeKernel(detect_kernel, cl::NDRange(begin_index), cl::NDRange(end_index - begin_index), cl::NullRange, &waiting_evts_, &proc_evt));

            waiting_evts_.clear();
            waiting_evts_.push_back(proc_evt);
            

            //CL_CHECK_BOOL(core->queue().enqueueReadBuffer(feats_mem_, CL_FALSE, 0, sizeof(cl_float) * feats_ptr_->size(), feats_ptr_->data(), &waiting_evts_, &read_feats_evt));
            //CL_CHECK_BOOL(core->queue().enqueueReadBuffer(feats_mem_, CL_FALSE, 0, sizeof(cl_half) * feats_ptr_->size() / 10, feats_ptr_->data(), &waiting_evts_, &read_feats_evt));
            //CL_CHECK_BOOL(core->queue().enqueueReadBuffer(dets_mem_, CL_FALSE, 0, sizeof(cl_int) * dets_ptr_->size(), dets_ptr_->data(), &waiting_evts_, &read_dets_evt));
            //CL_CHECK_BOOL(core->queue().enqueueReadBuffer(dets_mem_, CL_FALSE, 0, sizeof(cl_int) * dets_ptr_->size(), dets_ptr_->data(), &waiting_evts_, &read_dets_evt));

            // TODO: make big buffer and add all data here;

            
            CL_CHECK_BOOL(core->read_queue().enqueueReadBuffer(dets_table_mem_, CL_TRUE, 0, sizeof(cl_int) * dets_table_.size(), dets_table_.data(), &waiting_evts_));

            int measures_pack_size = dets_table_.at(0);
            

            CL_CHECK_BOOL(core->read_queue().enqueueReadBuffer(measures_mem_, CL_TRUE, 0, sizeof(cl_ushort) * measures_pack_size, measures_.data()));
           // const int* table = &dets_table_[1];
            //const uint16_t* measures_ptr = measures_.data();
            

            /*
            const int* dets_table_ptr = (const int*)core->read_queue().enqueueMapBuffer(dets_table_mem_, CL_TRUE, CL_MAP_READ, 0, sizeof(cl_int) * table_size, &waiting_evts_);

            int measures_pack_size = dets_table_ptr[0];
            const int* table = &dets_table_ptr[1];

            const uint16_t* measures_ptr = (const uint16_t*)core->read_queue().enqueueMapBuffer(measures_mem_, CL_TRUE, CL_MAP_READ, 0, sizeof(cl_ushort) * measures_pack_size);
            */
            
            //DecodeMeasures(detector.stages().positions(), & dets_table_[1], measures_.data(), trans_size, min_stages, triggered, features);
                                 
            
            //core->ReleaseTask(this);

            
            /*

            CL_CHECK_BOOL(core->read_queue().enqueueUnmapMemObject(measures_mem_, (void*)measures_ptr));

            // Cancel mapping of detection table
            CL_CHECK_BOOL(core->read_queue().enqueueUnmapMemObject(dets_table_mem_, (void*)dets_table_ptr));
            */

/*
            return true;
        }


    private:
        void            DecodeMeasures( const positions_t& stages_positions, 
                                        const int* table, 
                                        const uint16_t* measures, 
                                        int table_size, 
                                        int min_stages, 
                                        triggered_t& triggered, 
                                        features_t& features) {

            triggered.resize(table_size, -1);
            features.resize(stages_positions.back() * table_size, 0);
            float* measures_output = features.data();
            size_t measures_size = features.size();
            size_t stages_plus_1 = stages_positions.size();

           

            for (int t = 0; t < table_size; ++t, table += stages_plus_1) {
                // First number is number of stage triggered or -1;
                int triggered_stage = table[0];
                const int* stages_offset = table + 1;
                int num_stages = triggered_stage < 0 ? stages_plus_1 - 1 : std::max<int>(triggered_stage + 1, min_stages);
                triggered[t] = triggered_stage;

                
                for (int s = 0; s < num_stages; ++s) {
                    int stage_size = stages_positions[s + 1] - stages_positions[s];
                   
                    const uint16_t* stage_measures = measures + stages_offset[s];
                    for (int i = 0; i < stage_size; i++) {
                        measures_output[i] = float(stage_measures[i] / MEASURE_COMPRESS_FACTOR);
                    }
                }
                measures_output += stages_positions.back();
                measures_size -= stages_positions.back();
            }
        }


    private:
          
        measures_t                      measures_;
        detections_t                    dets_table_;

       
        cl::Buffer                      measures_mem_;
        cl::Buffer                      dets_table_mem_;

        cl::Event                       evt_complete_;
              
        std::vector<cl::Event>          waiting_evts_;


    };*/

    Engine::~Engine() {
        core_->Finish();

       // std::cout << "Overall tasks count " << core_->tasks_count() << std::endl;
    }

    Engine Engine::Create()
    {
        auto core = std::make_shared<Core>();
        if (core->Initialize())
            return Engine(core);
        return Engine(nullptr);
    }

    std::unique_ptr<Worker> Engine::CreateWorker() { 
        auto worker = std::make_unique<TaskWorker>();
        if (worker->Initialize(core_))
            return worker;
        return {};
    }

    IntegralView Engine::CreateIntegral(const uint64_t* ptr, size_t width, size_t height, size_t stride) {
        return std::make_shared<IntegralData>(core_->context(), ptr, width, height, stride);
    }

    DetectorView Engine::CreateDetector(detector_t&& detector) {
        return std::make_shared<DetectorData>(core_->context(), std::forward<detector_t>(detector));
    }

    FeaturesView Engine::CreateFeatures(Features&& features) {
        return std::make_shared<FeaturesData>(core_->context(), std::forward<Features>(features));
    }

    TransformsView Engine::CreateTransforms(transforms_t&& transforms)
    {
        return std::make_shared<TransformsData>(core_->context(), std::forward<transforms_t>(transforms));
    }


}
   

    