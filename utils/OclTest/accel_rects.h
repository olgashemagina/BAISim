#pragma once

#include <iostream>


#include <vector>
#include <array>
#include <functional>
#include <assert.h>



namespace accel_rects {


    using transforms_t = std::vector<std::array<float, 4>>;
    //Base rects of features. For example SC feature has 10 rects
    using rects_t = std::vector<std::array<float, 4>>;
    //Features values
    using features_t = std::vector<uint16_t>;
    //Result of detections
    using detections_t = std::vector<uint8_t>;
    //Stages positions
    using positions_t = std::vector<int>;
    //Weights of features
    using weights_t = std::vector<float>;
    //Threshold for each cascade;
    using thres_t = std::vector<float>;
    //Responces of weak features. Each weak sc feature have 64 bytes (512 bits) of responces {0, 1}
    using sc_table_t = std::array<uint8_t, 64>;
    using sc_resps_t = std::vector<sc_table_t>;

    //transform_size, stages_size, features_size
    using dims_t = std::array<int, 3>; 
    
    using callback_t = std::function<void(const dims_t& dimensions, const detections_t& dets, const features_t& feats)>;

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

            uint8_t mask = (1 << byte_index);

            if (value)
                (*this)[byte_index] |= mask;       //Set bit
            else
                (*this)[byte_index] &= mask;       //Clear bit
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
        int         AddSCWeak(float x, float y, float w, float h) {
            //Add Total rect (MUST be first);
            rects_.push_back({ x, y, x + 3 * w, y + 3 * h });

            for (int j = 0; j < 3; ++j)
                for (int i = 0; i < 3; ++i)
                    rects_.push_back({ x + i * w, y + j * h, x + (i + 1) * w, y + (j + 1) * h });

            return count_++;
        }

        void    Clear() { rects_.clear(); count_ = 0; }

        Features Consume() { return std::move(*this); }

           
    };

    

    class Stages {
    public:
        const weights_t& weights() const { return weights_; }
        const thres_t& thres() const { return thres_; }
        const positions_t& positions() const { return positions_; }
        const sc_resps_t& resps() const { return resps_; }
        
        int count() const { return int(thres_.size()); }

    protected:
        weights_t                       weights_;
        thres_t                         thres_;
        positions_t                     positions_;
        sc_resps_t                      resps_;
    };


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

        std::pair<Stages, Features> Consume() {
            return { std::move(*this), std::move(features_.Consume()) };
        }

    
    private:
        FeaturesBuilder                 features_;
             
    };



    class Kernels;
    class Core;
    class FeaturesData;
    class DetectorData;
    class IntegralData;

    //Represent features data in GPU;
    class FeaturesView {
    public:
        FeaturesView(const std::shared_ptr<FeaturesData>& data) : data_(data) {}
        FeaturesView(const FeaturesView&) = default;
        FeaturesView(FeaturesView&&) = default;

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
        Worker(std::shared_ptr<Core> core);
        Worker(Worker&& other);


        ~Worker();

    public:
        bool        Enqueue(IntegralView integral, DetectorView detector, transforms_t&& trans, callback_t cb);
        bool        Enqueue(IntegralView integral, FeaturesView features, transforms_t&& trans, callback_t cb);

   

    private:
        std::shared_ptr<Core>           core_;
        std::unique_ptr<Kernels>        kernels_;
    };

    class Engine {
    private:
        Engine(std::shared_ptr<Core> core) : core_(core) {}

    public:
        static Engine       Create(size_t tasks_limits = 8);

        bool                is_valid() const { return bool(core_); }

    public:
        //Workers used in context one thread;
        Worker              CreateWorker();
                
        IntegralView        CreateIntegral(const uint64_t*, size_t width, size_t height, size_t stride);
        DetectorView        CreateDetector(std::pair<Stages, Features>&& detector);
        FeaturesView        CreateFeatures(Features&& features);
     

    private:
        std::shared_ptr<Core>               core_;
        
    };



}