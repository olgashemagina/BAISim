#include "LFAgent.h"
#include "LFScanners.h"

#include <random>
#include <syncstream>

#ifdef _OMP_
#include <omp.h>
#endif

TLFFragments::TLFFragments(std::vector<awpRect>& rects, ILFScanner* scanner) {
	rects_ = std::move(rects);
	scanner_ = scanner;
}
const std::vector<awpRect>& TLFFragments::GetFragments() const {
	return rects_;
}

size_t TLFFragments::GetNearestIndex(const awpRect & rect) {
	// naidi mne blizhaishii awpRect
	// TODO from scanner
	return 0;
}

TLFFragmentBuilder::TLFFragmentBuilder(ILFScanner* scanner) {
	scanner_ = scanner;
}

std::shared_ptr<TLFFragments> TLFFragmentBuilder::Build(std::shared_ptr<TLFImage> img) {
	// ??? std::shared_ptr<vector<awprect>>
	if (height_ == img->height() && width_ == img->width()) { // razmer img ne pomenyalsya to
		return fragments_;
	}
	else { // skaniruem		
		scanner_->ScanImage(img.get());

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

void DetectorCallback(const std::vector<TLFResult>& results, std::shared_ptr<TFeatures> features, size_t frags_begin, size_t frags_end) {
	std::thread::id this_id = std::this_thread::get_id();
	std::cout << "thread " << this_id << "\nfrags_begin=" << frags_begin << "\nfrags_end=" << frags_end << "\nfeatures=" << features->GetValue(frags_begin, 0) << std::endl;
	//TODO
}

std::unique_ptr<TLFSemanticImageDescriptor> TLFAgent::Detect(std::shared_ptr<TLFImage> img) {
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

std::shared_ptr<TLFDetections> TLFSupervisor::Detect(std::shared_ptr<TLFImage> img) {
	//TODO implement python call
	std::vector<awpRect> rects;
	std::shared_ptr<TLFDetections> detections = std::make_shared<TLFDetections>(rects);
	return detections;
}
/*
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
*/
TLFDetector::TLFDetector() {
	Init();
}

void TLFDetector::Init() {
	// karta detectora odin raz
	size_t cascade_count = 3;  // rand ot 3 do 10
	//const size_t sizeof_one_feature = 100;
	for (auto i = 0; i < cascade_count; i++) {
		size_t feature_count = 10; // rand ot 10 do 100
		feature_count_list_.push_back(feature_count);
	}
	// cascade_count = 3: 10 + 19 + 21 = 50 features 
	// cascade_count = 4: 10 + 19 + 21 + 10 = 60 features
	//discription_features_ = std::make_shared<TLFDiscriptionFeatures>(feature_count_list);
}

void TLFDetector::Detect(std::shared_ptr<TLFImage> img, std::shared_ptr<TLFFragments> fragments) {
	TLFResult result;
	const auto& frags = fragments->GetFragments();
	int count = 0;
	//size_t count_all_feature = 0;
	const size_t batch_fragments = 1000;
	
	if (frags.size() > batch_fragments)
		count = frags.size() / batch_fragments;
	size_t remainder_fragments = frags.size() - count * batch_fragments;

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> distr(0, 1);
	TFeatures::TMapPtr tmap_ptr = std::make_shared<TFeatures::TMap>(feature_count_list_);
	TFeaturesPool features_pool(tmap_ptr);

#ifdef _OMP_
#pragma omp parallel for num_threads(omp_get_max_threads())
#endif
	for (auto i = 0; i <= count; i++) {
		size_t batch_fragments_for_callback = batch_fragments;
		const size_t frags_begin = i * batch_fragments;
		if (i == count) {
			if (remainder_fragments == 0) break;
			batch_fragments_for_callback = remainder_fragments;
			
		}
		const size_t frags_end = frags_begin + batch_fragments_for_callback;
		
		std::vector<TLFResult> results;
		results.reserve(batch_fragments);
		auto features_builder = features_pool.Alloc(batch_fragments);
		features_builder->Reset(frags_begin, frags_end);
#ifdef _OMP_
#pragma omp critical 
#endif  
		{
			callback_(results, features_builder, frags_begin, frags_end);
		}
	}	
}

void TLFDetector::SetCallback(std::function<void(const std::vector<TLFResult>& results, std::shared_ptr<TFeatures> features, size_t frags_begin, size_t frags_end)> callback) {
	callback_ = callback;
}

TCorrector::TCorrector() {

}

TLFResult TCorrector::Correct(const TFeatures& list) {
	TLFResult result;
	return result;
}

void TCorrector::LoadXML(TiXmlElement* parent) {

}

TiXmlElement* TCorrector::SaveXML() {
	return nullptr;
}

TLFTrainerCorrectors::TLFTrainerCorrectors() {

}
/*
void TLFTrainerCorrectors::addSamples(TLFResult result, std::shared_ptr<TLFDetections> detections, TLFDiscriptionFeatures features) {

}
*/
