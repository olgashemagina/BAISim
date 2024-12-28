#include "LFAgent.h"
#include "LFScanners.h"

#include <random>

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
}

void DetectorCallback(const std::vector<TLFResult>& results, const std::vector<std::shared_ptr<TLFFeature>>& features) {
	//TODO
}

std::unique_ptr<TLFSemanticImageDescriptor> TLFAgent::Detect(TLFImage* img) {
	std::shared_ptr<TLFFragments> fragments = fragment_builder_->Build(img);
	TLFDetector detector;
	detector.SetCallback(DetectorCallback);
	detector.Detect(img, fragments);
	//todo nms algoritm return ILFDescriptor
	std::unique_ptr<TLFSemanticImageDescriptor> ret = std::make_unique<TLFSemanticImageDescriptor>();
	return ret;
}

void TLFAgent::SetSupervisor(TLFSupervisor* supervisor) {
	supervisor_ = supervisor;
}

bool TLFAgent::LoadXML(const char* lpFileName) {
	//TODO
	scanner_ = new TLFScanner();
	fragment_builder_ = std::make_unique <TLFFragmentBuilder>(scanner_);
	return true;
}

bool TLFAgent::LoadXML(TiXmlElement* parent)
{
	if (parent == NULL)
		return false;
}

bool TLFAgent::SaveXML(const char* lpFileName) {
	//TODO
	return true;
}

TiXmlElement* TLFAgent::SaveXML()
{	
	TiXmlElement* node1 = NULL;//new TiXmlElement(this->GetName());
	if (node1 == NULL)
		return NULL;

	//node1->SetAttribute("detector", this->m_strDetName.c_str());
	if (scanner_ != NULL)
	{
		TiXmlElement* e = scanner_->SaveXML();
		//node1->LinkEndChild(e);
	}
	else
	{
		//delete node1;
		return NULL;
	}

	return node1;
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

TLFFeature::TLFFeature(float* data, std::shared_ptr<TLFDiscriptionFeatures> batch_discription_features, size_t triggered_cascade) :
	batch_discription_features_(batch_discription_features),
	data_(data),
	triggered_cascade_(triggered_cascade)
{}

float* TLFFeature::GetData(size_t fragment_index, size_t feature_index) {
	return (data_ + feature_index * triggered_cascade_ + fragment_index);
}


TLFDiscriptionFeatures::TLFDiscriptionFeatures(std::vector<size_t>& feature_count_list) :
	feature_count_list_(std::move(feature_count_list))
{}

size_t TLFDiscriptionFeatures::GetCountFeature(size_t cascade_index) const {
	if (feature_count_list_.size() <= cascade_index)
		return 0;
	return feature_count_list_[cascade_index];
}

size_t TLFDiscriptionFeatures::GetCascadeCount() const {
	return feature_count_list_.size();
}

TLFDetector::TLFDetector() {
	Init();
}

void TLFDetector::Init() {
	// karta detectora odin raz
	std::vector<size_t> feature_count_list;
	size_t cascade_count = 3;  // rand ot 3 do 10
	//const size_t sizeof_one_feature = 100;
	for (auto i = 0; i < cascade_count; i++) {
		size_t feature_count = 10; // rand ot 10 do 100
		feature_count_list.push_back(feature_count);
	}
	// cascade_count = 3: 10 + 19 + 21 = 50 features 
	// cascade_count = 4: 10 + 19 + 21 + 10 = 60 features
	discription_features_ = std::make_shared<TLFDiscriptionFeatures>(feature_count_list);
}

void TLFDetector::Detect(TLFImage* img, std::shared_ptr<TLFFragments> fragments) {
	TLFResult result;
	const auto& frags = fragments->GetFragments();
	int count = 0;
	size_t count_all_feature = 0;
	for (auto i = 0; i < discription_features_->GetCascadeCount(); i++) {
		count_all_feature += discription_features_->GetCountFeature(i);
	}	
	size_t batch_fragments = 1000;
	float* p = new float[count_all_feature * batch_fragments];
	
	if (frags.size() > batch_fragments)
		count = frags.size() / batch_fragments;
	size_t remainder_fragments = frags.size() - count * batch_fragments;

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> distr(0, 1);
	auto call_callback = [&](size_t batch_fragments_for_callback) {
		std::vector<TLFResult> results;
		results.reserve(batch_fragments_for_callback);
		std::vector<std::shared_ptr<TLFFeature>> features;
		features.reserve(batch_fragments_for_callback);
		size_t triggered_cascade = 0;
		for (auto a = 0; a < batch_fragments_for_callback; a++) {
			TLFResult result;
			result.result = distr(gen) == 1;
			results.push_back(result);
			for (size_t b = 0; b < count_all_feature; b++) {
				p[a * count_all_feature + b] = distr(gen);
			}
			std::shared_ptr<TLFFeature> feature = std::make_shared<TLFFeature>((p + a * count_all_feature), discription_features_, triggered_cascade);
			features.emplace_back(feature);
		}
		callback_(results, features);
	};
	for (auto i = 0; i < count; i++) {
		call_callback(batch_fragments);
	}
	if (remainder_fragments > 0) {
		call_callback(remainder_fragments);
	}
	
}

void TLFDetector::SetCallback(std::function<void(const std::vector<TLFResult>& results, const std::vector<std::shared_ptr<TLFFeature>>& features)> callback) {
	callback_ = callback;
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

void TLFTrainerCorrectors::addSamples(TLFResult result, std::shared_ptr<TLFDetections> detections, TLFDiscriptionFeatures features) {

}

