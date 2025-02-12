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

       

    class Detector {
    public:
        inline const weights_t& weights() const { return weights_; }
        inline const thres_t& thres() const { return thres_; }
        inline const positions_t& positions() const { return positions_; }
        inline const sc_resps_t& resps() const { return resps_; }
        inline const rects_t& rects() const { return rects_; }
        
        inline int stages_count() const { return int(thres_.size()); }

        inline int features_count() const { return int(rects_.size()); }

        // Count of features for stage i;
        inline int count(int i) const { return positions_[i+1] - positions_[i]; }

    protected:
        weights_t                       weights_;
        thres_t                         thres_;
        positions_t                     positions_;
        sc_resps_t                      resps_;

        rects_t                         rects_;
    };

   
    class DetectorBuilder : public Detector {
    public:
        void        Begin() {
            weights_.clear();
            thres_.clear();
            positions_.clear();
            rects_.clear();
        }

        int         BeginStage() {
            assert(positions_.size() == thres_.size());
            positions_.push_back(int(weights_.size()));

            return int(positions_.size());

        }
        int         AddSCWeak(float weight, const ScTable& resp, float x, float y, float w, float h) {
            //int count = features_.AddSCWeak(x, y, w, h);
            int count = int(rects_.size());
            rects_.push_back({ x, y, w / 3.f, h / 3.f });

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
                    
             
    };



    class Kernels;
    class Core;

    class IntegralData;

    class IntegralView {
    public:
        IntegralView(std::unique_ptr<IntegralData> data);
        IntegralView();

        IntegralView(IntegralView&&);

        IntegralView& operator = (IntegralView&&) ;

        ~IntegralView();


        auto data() const { return data_.get(); }
        bool    is_valid() const;

    public:

        int         width() const;
        int         height() const;

    private:
        std::unique_ptr<IntegralData> data_;
            
    };


    class DetectorData;

    // View of detector;
    class DetectorView {
    public:
        DetectorView();
     
        DetectorView(std::unique_ptr<DetectorData> data);
                
        DetectorView(DetectorView&&);

        DetectorView& operator = (DetectorView&&) ;
        ~DetectorView();

        bool    is_valid() const;

    public:
        const Detector& detector() const;

        auto data() const { return data_.get(); }

    protected:
        
    
        std::unique_ptr<DetectorData> data_;
    };


    /// <summary>
    /// TransformsView transforms data must exclusive for it.
    /// </summary>
    class TransformsData;

    class TransformsView {
    public: 
        TransformsView() ;
        TransformsView(std::unique_ptr<TransformsData> data);

        TransformsView(TransformsView&&);
                
        TransformsView& operator = (TransformsView&&);

        ~TransformsView();

    public:
        bool    is_valid() const;

        auto data() const { return data_.get(); }

    private:

        std::unique_ptr<TransformsData> data_;
    };
    
    class Worker;
    class WorkerView {

    public:
        WorkerView();
        WorkerView(std::unique_ptr<Worker> data);

        WorkerView(WorkerView&&);

        WorkerView& operator = (WorkerView&&);

        ~WorkerView();

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

        bool        Run(    const IntegralView& integral,  
                                    const DetectorView& detector, 
                                    const TransformsView& trans, 
                                    int min_stages, 
                                    int begin_index, 
                                    int end_index, 
                                    triggered_t& triggered, 
                                    features_t* features = nullptr);

    private:
        std::unique_ptr<Worker> data_;
    };

    class Engine {
           
    public:
        virtual ~Engine() = default;

        //Workers used in context one thread;
        virtual WorkerView      CreateWorker() = 0;
               
        
        virtual IntegralView    CreateIntegral(const uint64_t*, size_t width, size_t height, size_t stride) = 0;
        virtual DetectorView    CreateDetector(const Detector& detector) = 0;
        
        // Create transforms view from constant data
        virtual TransformsView  CreateTransforms(const transforms_t& transforms) = 0;
     
        
    };

    
    // Default Accelerating engine 
    std::shared_ptr<Engine>     GetDefault();


}