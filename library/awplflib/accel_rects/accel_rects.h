#pragma once

#include <iostream>


#include <vector>
#include <array>
#include <functional>
#include <assert.h>


namespace accel_rects {
    using features_t = std::vector<float>;
    using triggered_t = std::vector<int>;

    using transforms_t = std::vector<std::array<float, 4>>;
    //Base rects of features. For example SC feature has 10 rects
    using rects_t = std::vector<std::array<float, 4>>;
    //Features values
    using measures_t = std::vector<uint16_t>;
    //Result of detections
    using detections_t = std::vector<int>;
    //Stages positions
    using positions_t = std::vector<int>;
    //Weights of features
    using weights_t = std::vector<float>;
    //Threshold for each cascade;
    using thres_t = std::vector<float>;
    //Responces of weak features. Each weak sc feature have 64 bytes (512 bits) of responces {0, 1}
    using sc_table_t = std::array<uint8_t, 64>;
    using sc_resps_t = std::vector<sc_table_t>;

    //transform_begin, transform_end, stages_size, features_size
    using dims_t = std::array<int, 4>; 
    
    using callback_t = std::function<void(const dims_t& dimensions, const detections_t& dets, const measures_t& feats)>;

    struct ScTable : public sc_table_t {
        ScTable() {
            for (size_t i = 0; i < 64; ++i) {
                (*this)[i] = 0;
            }
        }

        ScTable(const ScTable& tb) = default;
        ScTable(ScTable&& tb) = default;

        void SetBit(int index, bool value = true) {
            int byte_index = index >> 3;
            int byte_offset = index % 8;

            uint8_t mask = (1 << byte_offset);

            if (value)
                (*this)[byte_index] |= mask;       //Set bit
            else
                (*this)[byte_index] &= ~mask;       //Clear bit
        }

    };

    class TransformsBuilder : public transforms_t {
    public:
        transforms_t&& Consume() { return std::move(*this); }

        void Add(float scale = 1, float transition = 0) {
            this->push_back({ scale, scale, transition, transition });
        }

        void Add(float scale, float tx, float ty) {
            this->push_back({ scale, scale, tx, ty });
        }

        void Add(float sx, float sy, float tx, float ty) {
            this->push_back({ sx, sy, tx, ty });
        }

    };

    class Features {
    public:
        const rects_t& rects() const { return rects_; }
        int     count() const { return count_; }

    protected:
        int                             count_ = 0;
        rects_t                         rects_;
    };

    class FeaturesBuilder : public Features {
    public:
        /*int         AddSCWeak(float x, float y, float w, float h) {
            //Add Total rect (MUST be first);
            rects_.push_back({ x, y, x + w, y + h });

            w = w / 3.f;
            h = h / 3.f;

            for (int j = 0; j < 3; ++j)
                for (int i = 0; i < 3; ++i)
                    rects_.push_back({ x + i * w, y + j * h, x + (i + 1) * w, y + (j + 1) * h });

            return count_++;
        }*/

        int         AddSCWeak(float x, float y, float w, float h) {
            //Add Total rect (MUST be first);
            rects_.push_back({ x, y, w / 3.f, h / 3.f });

            return count_++;
        }

        void    Clear() { rects_.clear(); count_ = 0; }

        Features Consume() { return std::move(*this); }

           
    };

    

    class Stages {
    public:
        inline const weights_t& weights() const { return weights_; }
        inline const thres_t& thres() const { return thres_; }
        inline const positions_t& positions() const { return positions_; }
        inline const sc_resps_t& resps() const { return resps_; }
        
        inline int count() const { return int(thres_.size()); }

        // Count of features for stage i;
        inline int count(int i) const { return positions_[i+1] - positions_[i]; }

    protected:
        weights_t                       weights_;
        thres_t                         thres_;
        positions_t                     positions_;
        sc_resps_t                      resps_;
    };

    using detector_t = std::pair<Stages, Features>;

    class DetectorBuilder : public Stages {
    public:
        void        Begin() {
            features_.Clear();
            weights_.clear();
            thres_.clear();
            positions_.clear();
        }

        int         BeginStage() {
            assert(positions_.size() == thres_.size());
            positions_.push_back(int(weights_.size()));

            return int(positions_.size());

        }
        int         AddSCWeak(float weight, const ScTable& resp, float x, float y, float w, float h) {
            int count = features_.AddSCWeak(x, y, w, h);

            weights_.push_back(weight);
            resps_.push_back(resp);
            return count;
        }

        void        EndStage(float threshold = 0) {
            assert(positions_.size() == thres_.size() + 1);
            thres_.push_back(threshold);
        }


        void        End() {
            positions_.push_back(int(weights_.size()));
        }

        detector_t Consume() {
            return { std::move(*this), std::move(features_.Consume()) };
        }

    
    private:
        FeaturesBuilder                 features_;
             
    };



    class Kernels;
    class Core;
    class FeaturesData;
    class TransformsData;
    class DetectorData;
    class IntegralData;

    //Represent transforms data in GPU;
    class TransformsView {
    public:
        TransformsView() {}
        TransformsView(const std::shared_ptr<TransformsData>& data) : data_(data) {}
        TransformsView(const TransformsView&) = default;
        TransformsView(TransformsView&&) = default;


        TransformsView& operator = (const TransformsView&) = default;
        TransformsView& operator = (TransformsView&&) = default;

        const std::shared_ptr<TransformsData>& data() const { return data_; }

        const transforms_t& transforms() const;

        bool    is_valid() const;

    private:
        std::shared_ptr<TransformsData>       data_;
    };

    //Represent features data in GPU;
    class FeaturesView {
    public:
        FeaturesView(const std::shared_ptr<FeaturesData>& data) : data_(data) {}
        FeaturesView(const FeaturesView&) = default;
        FeaturesView(FeaturesView&&) = default;

        FeaturesView& operator = (const FeaturesView&) = default;
        FeaturesView& operator = (FeaturesView&&) = default;
        const std::shared_ptr<FeaturesData>& data() const { return data_; }

        const Features& features() const;

        bool    is_valid() const;

    private:
        std::shared_ptr<FeaturesData>       data_;
    };

    //Represent detector data in GPU;
    class DetectorView {
    public:
        DetectorView(const std::shared_ptr<DetectorData>& data) : data_(data) {}
        DetectorView(const DetectorView&) = default;
        DetectorView(DetectorView&&) = default;

        DetectorView& operator = (const DetectorView&) = default;
        DetectorView& operator = (DetectorView&&) = default;
        const std::shared_ptr<DetectorData>& data() const { return data_; }

        const Stages& stages() const;

        FeaturesView    features_view();

        bool    is_valid() const;

    private:
        std::shared_ptr<DetectorData>       data_;
    };

    class IntegralView {
    public:
        IntegralView(const std::shared_ptr<IntegralData>& data) : data_(data) {}
        IntegralView(const IntegralView&) = default;
        IntegralView(IntegralView&&) = default;

        IntegralView& operator = (const IntegralView&) = default;
        IntegralView& operator = (IntegralView&&) = default;
        const std::shared_ptr<IntegralData>& data() const { return data_; }

        bool    is_valid() const;

    public:

        size_t         width() const;
        size_t         height() const;

    private:
        std::shared_ptr<IntegralData>       data_;
    };

    
    
    class Worker {
    public:
        
        virtual ~Worker() {}

    public:

        // Process transforms for detector;
        // integral - integral image,
        // detector - processed detector,
        // trans - transforms for fragments,
        // min_stages - minimal count of stages to run for features,
        // begin_index - start index of transforms,
        // end_index of transforms,
        // triggered - number of stage that return 0, or -1 if not triggered,
        // features - measured features, count of them not less than min_stages features and not more than count of features.
        // For each transform requered count_of_features space and features from max(triggered, min_stages) to count_of_features are equal to 0;

        virtual bool        Run(    IntegralView integral,  
                                        DetectorView detector, 
                                        TransformsView trans, 
                                        int min_stages, 
                                        int begin_index, 
                                        int end_index, 
                                        triggered_t& triggered, 
                                        features_t& features) = 0;
    };

    class Engine {
    private:
        Engine(std::shared_ptr<Core> core) : core_(core) {}
        
    public:
        ~Engine();

        static Engine       Create();

        bool                is_valid() const { return bool(core_); }

    public:
        //Workers used in context one thread;
        std::unique_ptr<Worker>              CreateWorker();
                
        IntegralView        CreateIntegral(const uint64_t*, size_t width, size_t height, size_t stride);
        DetectorView        CreateDetector(detector_t&& detector);
        FeaturesView        CreateFeatures(Features&& features);

        TransformsView      CreateTransforms(transforms_t&& transforms);
     

    private:
        std::shared_ptr<Core>               core_;
        
    };



}