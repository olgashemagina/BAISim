#include "LFAgent.h"
#include "LFScanners.h"

#include <random>

#ifdef _OMP_
#include <omp.h>
#endif

using namespace agent;

///////////////////////////////////////////////////////////////////////////////

TLFAgent::TLFAgent() 
	: pool_([this]() { return std::make_unique<agent::TFeaturesBuilder>(); }) {
}

inline std::unique_ptr<TLFSemanticImageDescriptor> 
TLFAgent::Detect(std::shared_ptr<TLFImage> img) {

	auto scanner = detector_->GetScanner();
	
	if (!scanner)
		return nullptr;

	agent::TDetections	gt_detections;

	scanner->ScanImage(img.get());
// We can process objects without supervisor also
	if (supervisor_ && trainer_) {
		// Process image and get ground truth bounds
		gt_detections = supervisor_->Detect(img);
	}

	auto fragments_count = scanner->GetFragmentsCount();

	if (fragments_count == 0)
		return nullptr;


	size_t batches_count = size_t(std::ceil(fragments_count / batch_size_));
			
	int threads = 1;

#ifdef _OMP_
	threads = std::max<int>(omp_get_max_threads(), max_threads_);
#endif 

	// Acquire workers for processing;
	if (workers_.size() < threads) {
		workers_.resize(threads);
		for (auto& worker : workers_) {
			worker = std::move(detector_->CreateWorker());
		}
	}

	detected_indices_.clear();
	

#ifdef _OMP_
#pragma omp parallel for num_threads(threads)
#endif
	for (auto batch = 0; batch <= batches_count; batch++) {

		int current_thread = 0;

#ifdef _OMP_
		current_thread = omp_get_thread_num();
#endif 
		// Acquire object from pool or create new one
		auto features = pool_.GetObject();

		// Init features builder
		size_t batch_begin = batch_size_ * batch;
		size_t batch_end = batch_begin + batch_size_;
		batch_end = batch_end < fragments_count ? batch_end : fragments_count;

		// Setting up bounds of batch and alloc memory if needed
		features->Setup(detector_->GetMap(), batch_begin, batch_end, batch_size_);

		// Process batch by detector
		workers_[current_thread]->Detect(img, *features.get());

		// Apply corrections and calc corrections vector
		correctors_.Apply(*features.get(), features->GetMutableCorrections());

		// No new correctors without supervisor
		if (supervisor_ && trainer_) {
			// Process features and check if we can train new corrector(s)
			trainer_->CollectSamples(scanner, *features.get(), gt_detections);
			
		}
		
		// Prepare for NMS
		for (size_t n = features->frags_begin(); n < features->frags_end(); ++n) {
			if (features->GetCorrectedResult(n)) {
#ifdef _OMP_
#pragma omp critical 
#endif
				{
					detected_indices_.push_back(n);
				}
			}
		}

	}

	//TODO: run NMS;

	if (supervisor_ && trainer_) {
		auto correctors = trainer_->Train();
		correctors_.AddCorrectors(std::move(correctors));
	}

	return std::make_unique<TLFSemanticImageDescriptor>();
}

