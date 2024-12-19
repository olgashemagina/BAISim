#include "LFAgent.h"
#include "LFScanners.h"

TLFFragments::TLFFragments(std::vector<awpRect>& rects, ILFScanner* scanner) {
	rects_ = std::move(rects);
	scanner_ = scanner;
}
const std::vector<awpRect>& TLFFragments::GetFragments() const {
	return rects_;
}
size_t/*index*/ TLFFragments::GetNearestIndex(awpRect) { //todo kakoito est
	// naidi mne blizhaishii awpRect
	// TODO from scanner
	return 0;
}

TLFFragmentBuilder::TLFFragmentBuilder(ILFScanner* scanner) {
	scanner_ = scanner;
}

std::shared_ptr<TLFFragments> TLFFragmentBuilder::Build(TLFImage* img) {
	// ??? std::shared_ptr<vector<awprect>>
	if (height_ == img->height() && width_ == img->width()) { // razmer img ne pomenyalsya to
		return fragments_;
	}
	else { // skaniruem		
		scanner_->ScanImage(img);

		height_ = img->height();
		width_ = img->width();
		
		const auto& bounds = scanner_->GetFragments();
		std::vector<awpRect> rects;
		rects.reserve(bounds.size());
		for (int i = 0; i < bounds.size(); i++)
			rects.push_back(bounds[i].Rect);
		fragments_ = std::make_shared<TLFFragments>(rects, scanner_);
		return fragments_;
	}
}

TLFAgent::TLFAgent() {
    fragment_builder_ = std::make_unique <TLFFragmentBuilder>(scanner_);
}

std::unique_ptr<TLFSemanticImageDescriptor> TLFAgent::Detect(TLFImage* img) {
	fragment_builder_->Build(img);
	//todo nms algoritm return ILFDescriptor
	std::unique_ptr<TLFSemanticImageDescriptor> ret = std::make_unique<TLFSemanticImageDescriptor>();
	return ret;
}

void TLFAgent::LoadXML() {
	//TODO
}
void TLFAgent::SaveXML() {
	//TODO
}

TLFDetections::TLFDetections(std::vector<awpRect>& rects) {
	rects_ = std::move(rects);
}

const std::vector<awpRect>& TLFDetections::GetDetections() const {
	return rects_;
}

TLFSupervisor::TLFSupervisor(){}

std::shared_ptr<TLFDetections> TLFSupervisor::Detect(TLFImage* img) {
	//TODO implement python call
	std::vector<awpRect> rects;
	std::shared_ptr<TLFDetections> detections = std::make_shared<TLFDetections>(rects);
	return detections;
}

TLFFeature::TLFFeature(float* data, std::vector<size_t>& kascade_size) {
	data_ = data;
	kascade_size_ = std::move(kascade_size);
}

float* TLFFeature::GetData(size_t index) {
	size_t count = 0;
	for (size_t i = 0; i < index; i++)
		count += kascade_size_[i];
	return (data_ + count);
}

size_t TLFFeature::GetSizeData(size_t index) {
	return kascade_size_[index];
}

TLFFeatures::TLFFeatures(const std::vector<TLFFeature>& features, size_t start_pos) {
	features_ = std::move(features);
	start_pos_ = start_pos;
}

size_t TLFFeatures::GetLength() {
	return features_.size();
}

size_t TLFFeatures::GetStartPosition() {
	return start_pos_;
}

void TLFDetector::Detect(TLFImage* img, TLFFragments, void(*callback)(TLFResult result, TLFFeature feature, TLFFeatures features)) {
	TLFResult result;
	std::vector<size_t> kascade_size;
	TLFFeature feature(nullptr, kascade_size);
	std::vector<TLFFeature> features;
	size_t start_pos;
	TLFFeatures tlffeatures(features, start_pos);
	callback(result, feature, tlffeatures);
}

TLFCorrectors::TLFCorrectors() {

}

TLFResult TLFCorrectors::correct(std::vector<TLFFeature> list) {
	TLFResult result;
	return result;
}

void TLFCorrectors::LoadXML() {

}

void TLFCorrectors::SaveXML() {

}

TLFTrainerCorrectors::TLFTrainerCorrectors() {

}

void TLFTrainerCorrectors::addSamples(TLFResult result, std::shared_ptr<TLFDetections> detections, TLFFeatures features) {

}

