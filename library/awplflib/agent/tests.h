#pragma once

#include <numeric>
#include <memory>

#include "utils/object_pool.h"
#include "utils/matrix.h"
#include "agent/agent.h"

#include "LFAgent.h"
#include "LFScanners.h"

namespace agent {

	static size_t GetRand(size_t low_dist, size_t high_dist) {
		static bool init_srand = false;
		if (!init_srand) {
			std::srand((unsigned int)std::time(nullptr));
			init_srand = true;
		}
		return low_dist + std::rand() % (high_dist + 1 - low_dist);
	}

	class TRandomSupervisor : public ILFSupervisor
	{
	public:
		TRandomSupervisor() {}
		virtual TDetections Detect(std::shared_ptr<TLFImage> img) override {
			
			auto width = img->GetImage()->sSizeX;
			auto height = img->GetImage()->sSizeY;

			TDetections detections;
			for (size_t i = 0; i < 10; i++) {
				awpRect rect;
				rect.left = GetRand(0, width - 5);
				rect.right = GetRand(rect.left, width);
				rect.top = GetRand(0, height - 5);
				rect.bottom = GetRand(rect.top, height);
				detections.push_back(rect);
			}

			return detections;
		}

	};


	class TRandomDetector : public IDetector
	{
	public:
		TRandomDetector() = default;

		bool Initialize() {

			scanner_ = std::make_unique<TLFScanner>();

			size_t cascade_count = GetRand(3, 10);  // rand ot 3 do 10
			size_t count = 0;
			for (auto i = 0; i < cascade_count; i++) {
				size_t feature_count = GetRand(10, 100); // rand ot 10 do 100
				feature_count_list_.push_back(feature_count + count);
				count = feature_count_list_.back();
			}
			
			return true;
		}

		void Detect(std::shared_ptr<TLFImage> img, TFeaturesBuilder& builder) {

			builder.Reset(feature_count_list_);

			float* data = builder.GetMutableData().GetRow(0);
			size_t stride = builder.feats_count();
						
			size_t detected = 0;

			for (size_t frag = builder.frags_begin(); frag < builder.frags_end(); ++frag) {
				auto row_data = builder.GetFeats(frag);
				// With probability 0.99 it is negative result
				if (GetRand(0, 100) > 1) {
					size_t cascade_number = GetRand(0, feature_count_list_.size() - 1);

					builder.SetTriggered(frag, cascade_number, 1);
															
					SetRandData(row_data, feature_count_list_.at(cascade_number));
				}
				else { // fill all cascade
					SetRandData(row_data, stride);
					detected++;
				}
			}

			std::thread::id this_id = std::this_thread::get_id();

			std::stringstream ss;

			ss << "Detection thread " << this_id
				<< " frags_begin=" << builder.frags_begin()
				<< " frags_end=" << builder.frags_end()
				<< " detected=" << detected << std::endl;

			(std::cout << ss.str()).flush();

		}

		virtual ILFScanner* GetScanner()  override {
			return scanner_.get();
		}

		virtual std::unique_ptr<IWorker> CreateWorker() override;

		virtual bool LoadXML(TiXmlElement* parent)  override {
			return false;
		}
		virtual TiXmlElement* SaveXML()  override {
			return nullptr;
		}


	private:
		static void SetRandData(float* data, size_t size) {
			for (size_t i = 0; i < size; i++)
				data[i] = float(GetRand(1, 100));
		}

	private:
		std::vector<size_t>				feature_count_list_;

		std::unique_ptr<ILFScanner>		scanner_;

	};


	class TRandomWorker : public IWorker {
	public:
		TRandomWorker(TRandomDetector* detector) : detector_(detector) {}

		virtual void Detect(std::shared_ptr<TLFImage> img, TFeaturesBuilder& builder) override {
			assert(detector_);
			detector_->Detect(img, builder);
		}

	private:
		TRandomDetector* detector_ = nullptr;
	};

	std::unique_ptr<IWorker> TRandomDetector::CreateWorker() {
		return std::make_unique<TRandomWorker>(this);
	}


	class TRandomAgent : public TLFAgent
	{
	public:
		TRandomAgent() : TLFAgent() {
		}

		bool Initialize() {
			auto detector = std::make_unique<TRandomDetector>();
			if (detector->Initialize())
				detector_ = std::move(detector);

			return bool(detector_);

		}

	};

}

