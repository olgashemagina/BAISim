#pragma once


#include "LFCore.h"
#include "LF.h"
#include "LFDetector.h"
#include "accel_rects/accel_rects.h"
#include "utils/object_pool.h"


#include <omp.h>

class TGpuDetector {
public:
    TGpuDetector(ILFObjectDetector* source, accel_rects::DetectorView view) 
    : detector_(source)
    , view_(std::move(view)) {}

    TGpuDetector() : detector_(nullptr), view_(nullptr) {}

	ILFObjectDetector* detector() const { return detector_; }

	auto view() const { return view_; }

    bool is_valid() const { return view_.is_valid(); }

private:
	//TODO: memory handler
	ILFObjectDetector*				detector_;
	accel_rects::DetectorView	    view_;
	
};

class TGpuIntegral {
public:
    TGpuIntegral(TLFImage* source, accel_rects::IntegralView view, std::vector<uint64_t>&& integral)
        : image_(source)
        , view_(std::move(view))
        , integral_(std::move(integral)){}

    TGpuIntegral() : image_(nullptr), view_(nullptr) {}

	TLFImage*		image() const { return image_; }
	auto view() const { return view_; }

    int     width() const { return view_.width(); }
    int     height() const { return view_.height(); }

    bool is_valid() const { return view_.is_valid(); }

private:

	TLFImage*						image_;
	accel_rects::IntegralView		view_;

    //TODO: remove this
    std::vector<uint64_t>           integral_;
};

class TGpuEngine {
public:
	using TCallback = std::function<void(const awpRect& fragment, int triggered, int stages, const float* features)>;
public:

    TGpuEngine(int num_threads = 4, int num_transforms = 2048, int min_stages = 3);

    TGpuDetector						CreateDetector(TSCObjectDetector* detector /*TODO: move ownership*/);
    TGpuIntegral						CreateIntegral(TLFImage* image /*TODO: move ownership*/);


	std::vector<TLFDetectedItem>		Run(TGpuDetector detector, TGpuIntegral image, awpRect in_rect, TCallback cb = nullptr);

public:


private:
    int                             num_transforms_ = 2048;
    int                             num_threads_ = 8;

    int                             min_stages_ = 3;

	accel_rects::Engine				engine_;

    std::vector<std::unique_ptr<accel_rects::Worker>>        workers_;

    std::vector<std::pair<accel_rects::triggered_t, accel_rects::features_t>>        result_;



    accel_rects::TransformsView                 transforms_;

    //std::unique_ptr<pool::ObjectPool<accel_rects::Worker>>             pool_;

private:
    awpRect                         rect_;
};
