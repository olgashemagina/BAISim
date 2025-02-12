#pragma once

#include <numeric>
#include <memory>
#include <limits>

#include "LF.h"

#include "utils/matrix.h"

namespace agent {

	// Fragments of image to be processed
	class TFragments {
	public:
		TFragments(const TFragments&) = default;
		TFragments(TFragments&&) = default;


		const size_t		count() const { return fragments_.size(); }
		const TLFRect&		get(size_t index) const {
			return fragments_[index];
		}

		const auto& rects() const { return fragments_; }

	protected:
		TFragments() {}
		
	protected:
				
		std::vector<TLFRect>			fragments_;
	};


	// Builder of fragments
	class TFragmentsBuilder : public TFragments {
	public:
		
		bool Scan(ILFScanner* scanner, std::shared_ptr<TLFImage> img, const std::vector<TLFRect>* rois = nullptr) {

			bool scan_needed = false;

			scan_needed = cached_width_ != img->width() || cached_height_ != img->height();
			cached_width_ = img->width();
			cached_height_ = img->height();
						
			if (rois) {

				// TODO: compare rects;
				//scan_needed = scan_needed || (rois_ == *rois);

				rois_ = *rois;
				
				if (scan_needed) {
					fragments_.clear();
					for (const auto& roi : *rois) {
						scanner->Scan(roi.Width(), roi.Height());

						float min_width = roi.Width() * min_size_factor_;
						float min_height = roi.Height() * min_size_factor_;

						fragments_.reserve(fragments_.size() + scanner->GetFragmentsCount());

						for (int f = 0; f < scanner->GetFragmentsCount(); ++f) {
							TLFRect rect = scanner->GetFragmentRect(f);
							if (rect.Width() >= min_width || rect.Height() >= min_height) {
								rect.Shift(roi.Left(), roi.Top());
								fragments_.push_back(scanner->GetFragmentRect(f));
							}
						}
					}
				}
			} else {
				rois_.clear();
				if (scan_needed) {
					scanner->Scan(img->width(), img->height());
					fragments_.resize(scanner->GetFragmentsCount());
					for (int f = 0; f < scanner->GetFragmentsCount(); ++f) {
						fragments_[f].SetRect(scanner->GetFragmentRect(f));
					}
				}
			}
			return scan_needed;

		}

		void Clear() { fragments_.clear(); }

	private:
		
		float								min_size_factor_ = 0.5;
		
		
		std::vector<TLFRect>				rois_;



		int									cached_width_ = 0;
		int									cached_height_ = 0;

	};

}
