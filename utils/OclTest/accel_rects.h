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
    //Sizes of cascades
    using stages_t = std::vector<int>;
    //Weights of features
    using weights_t = std::vector<float>;
    //Threshold for each cascade;
    using thres_t = std::vector<float>;
    //Responces of weak features. Each weak sc feature have 64 bytes (512 bits) of responces {0, 1}
    using sc_table_t = std::array<uint8_t, 64>;
    using sc_resps_t = std::vector<sc_table_t>;
    

    using callback_t = std::function<void(size_t trans_size, size_t stages_size, size_t feats_size, detections_t&& dets, features_t&& feats)>;

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

    

    class Detector {
    public:
        void        Begin() {
            rects_.clear();
            weights_.clear();
            thres_.clear();
            stages_.clear();
        }

        int         BeginStage() {
            assert(stages_.size() == thres_.size());
            stages_.push_back(int(weights_.size()));

            return int(stages_.size());
            
        }
        int         AddSCWeak(float weight, const ScTable& resp, float x, float y, float w, float h) {
            //Add Total rect (MUST be first);
            rects_.push_back({ x, y, x + 3 * w, y + 3 * h });

            for (int j = 0; j < 3; ++j) 
                for (int i = 0; i < 3; ++i) 
                    rects_.push_back({ x + i * w, y + j * h, x + (i + 1) * w, y + (j + 1) * h });
                
            weights_.push_back(weight);
            resps_.push_back(resp);
            return int(weights_.size());
        }

        void        EndStage(float threshold = 0) {
            assert(stages_.size() == thres_.size() + 1);
            thres_.push_back(threshold);
        }


        void        End() {
            stages_.push_back(int(weights_.size()));
        }

    public:
        const rects_t& rects() const { return rects_; }
        const weights_t& weights() const { return weights_; }
        const thres_t& thres() const { return thres_; }
        const stages_t& stages() const { return stages_; }
        const sc_resps_t& resps() const { return resps_; }

        int feats_size() const { return int(weights_.size()); }
        int stages_size() const { return int(thres_.size()); }

    private:
        rects_t                         rects_;
        weights_t                       weights_;
        thres_t                         thres_;
        stages_t                        stages_;
        sc_resps_t                      resps_;
    };



    class Kernels;
    class Core;

     

    
    class Worker {
    public:
        Worker(std::shared_ptr<Core> core);
        Worker(Worker&& other);


        ~Worker();

    public:
        bool        Enqueue(transforms_t&& trans, callback_t cb);

   

    private:
        std::shared_ptr<Core>           core_;
        std::unique_ptr<Kernels>        kernels_;
    };

    class Engine {
    private:
        Engine(std::shared_ptr<Core> core) : core_(core) {}

    public:
        static Engine       Create(Detector&& detector);

        bool                is_valid() const { return bool(core_); }

    public:
        Worker              CreateWorker();

        bool                SetIntegral(const uint64_t*, size_t width, size_t height, size_t stride);
     

    private:
        std::shared_ptr<Core>               core_;
        
    };



}