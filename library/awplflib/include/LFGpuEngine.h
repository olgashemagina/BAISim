#pragma once


#include "LFCore.h"
#include "LF.h"
#include "LFDetector.h"
#include "accel_rects/accel_rects.h"
#include "utils/object_pool.h"


#include <omp.h>

class TGpuDetector {
public:
    TGpuDetector() {}

    TGpuDetector(TGpuDetector&&) = default;
    

    bool Build(TSCObjectDetector* detector);

    const auto& view() const { return view_; }
    size_t features_count() const { return view_.detector().rects().size(); }


private:
    accel_rects::DetectorView		                view_;
};

class TGpuIntegral {
public:
    TGpuIntegral() = default;

    TGpuIntegral(TGpuIntegral&&) = default;


    bool    Update(TLFImage* image);

	
	const auto& view() const { return view_; }

    int width() const { return width_; }
    int height() const { return height_; }
        

private:
    int                             width_ = 0;
    int                             height_ = 0;
            
    std::vector<uint64_t>           integral_;
        
    accel_rects::IntegralView		view_;
};


class TGpuTransforms {
public:
    TGpuTransforms() = default;

    void    AddFragments(const std::vector<TLFRect>& rects, int base_width, int base_height);
    void    AddFragments(ILFScanner* scanner);

    void    Clear() {
        transforms_.clear();
    }

    bool    Update() {
        view_ = accel_rects::GetDefault()->CreateTransforms(transforms_);
        return view_.is_valid();
    }

    const auto& view() const { return view_; }

    int     size() const { return int(transforms_.size()); }

private:
    accel_rects::transforms_t                           transforms_;
    accel_rects::TransformsView		                    view_;
};


class TGpuEngine{
    friend class TGpuTransforms;
    friend class TGpuDetector;
    friend class TGpuIntegral;

public:

    TGpuEngine(int num_threads = 4, int num_workers = 4, int num_transforms = 16192, int min_stages = 3);

    bool    Initialize(std::unique_ptr<TSCObjectDetector> detector);
                    

	std::vector<TLFDetectedItem>		Detect(const TGpuIntegral& image);
    std::vector<TLFDetectedItem>		Detect(TLFImage* image) {
        TGpuIntegral integral;

        if (!integral.Update(image))
            return {};

        return Detect(integral);

    }

private:
    std::unique_ptr<TSCObjectDetector>      object_detector_;
    TGpuTransforms                          transforms_;
    TGpuDetector                            detector_;

    std::unique_ptr<pool::ObjectPool<accel_rects::WorkerView>>             pool_;

    int                             num_threads_ = 4;
    int                             num_transforms_ = 16192;

    int                             min_stages_ = 3;


    std::vector<std::pair<accel_rects::triggered_t, accel_rects::features_t>>        result_;
    
    int                             trans_width_ = 0;
    int                             trans_height_ = 0;
};


