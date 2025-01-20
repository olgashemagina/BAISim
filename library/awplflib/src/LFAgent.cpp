#include "LFAgent.h"
#include "LFScanners.h"

#include <random>

#ifdef _OMP_
#include <omp.h>
#endif

using namespace agent;

static std::vector<TLFDetectedItem> NonMaximumSuppression(
	std::vector<std::pair<TLFRect, float>>& rects,
	float overlap_threshold, const TItemAttributes& attrs);

///////////////////////////////////////////////////////////////////////////////

TLFAgent::TLFAgent() 
	: pool_([this]() { return std::make_unique<agent::TFeaturesBuilder>(); }) {
}

 std::vector<TLFDetectedItem> TLFAgent::Detect(std::shared_ptr<TLFImage> img) {

	auto scanner = detector_->GetScanner();
	
	if (!scanner)
		return {};

	agent::TDetections	gt_detections;

	scanner->ScanImage(img.get());
// We can process objects without supervisor also
	if (supervisor_ && trainer_) {
		// Process image and get ground truth bounds
		gt_detections = supervisor_->Detect(img);
	}

	auto fragments_count = scanner->GetFragmentsCount();

	if (fragments_count == 0)
		return {};


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

	prior_detections_.clear();
	

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
		features->SetBounds(batch_begin, batch_end);

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
					prior_detections_.emplace_back(	scanner->GetFragmentRect(n), 
													features->GetScore(n));
				}
			}
		}
	}

	if (supervisor_ && trainer_) {
		auto correctors = trainer_->Train();
		correctors_.AddCorrectors(std::move(correctors));
	}

	return NonMaximumSuppression(prior_detections_, overlap_threshold_, item_attrs_);
}


// Filter detections
static std::vector<TLFDetectedItem> NonMaximumSuppression(
	std::vector<std::pair<TLFRect, float>>& rects, 
	float overlap_threshold, const TItemAttributes& attrs ) {
	
	std::vector<TLFDetectedItem>	items;

	// Sort by score
	std::sort(rects.begin(), rects.end(), [](const auto& a, const auto& b) {
			return a.second > b.second;
	});

#ifdef _OMP_
#pragma omp parallel for num_threads(std::min<int>(omp_get_max_threads(), 4))
#endif
	for (int i = 0; i < rects.size(); ++i) {
		const auto& [base_rect, base_score] = rects[i];

		// Averaging position
		awpPoint p = base_rect.Center();
		AWPSHORT w = base_rect.Width();
		AWPSHORT h = base_rect.Height();

		AWPSHORT similar_count = 1;

		for (size_t j = i + 1; j < rects.size(); ++j) {
			auto& [rect, score] = rects[j];

			if (score > 0 && rect.RectOverlap(base_rect) > overlap_threshold) {
				p.X += rect.Center().X;
				p.Y += rect.Center().Y;
				w += rect.Width();
				h += rect.Height();

				similar_count++;
#ifdef _OMP_
#pragma omp critical 
#endif
				// Skip from using in futher tests
				score = 0;
			}

		}
		p.X /= similar_count;
		p.Y /= similar_count;
		w /= similar_count;
		h /= similar_count;

		awpRect r{ p.X - w / 2, p.Y - h / 2, p.X + w / 2, p.Y + h / 2 };

		UUID id;
		// memset(id, 0, sizeof(UUID));
		LF_NULL_UUID_CREATE(id);
		// add object to result list
#ifdef _OMP_
#pragma omp critical 
#endif
		{
			items.emplace_back(std::move(r), base_score, attrs.type,
				attrs.angle, attrs.racurs, attrs.base_width, attrs.base_height, attrs.name, id);
		}

	}

	return items;
}

