#ifndef __lf_builder_h__
#define __lf_builder_h__

#include "LFImage.h"
#include "LF.h"

class TLFFragments
{
public:
	TLFFragments(std::vector<awpRect>& rects, ILFScanner* scanner);
	const std::vector<awpRect>& GetFragments() const;
	size_t GetNearestIndex(awpRect);
private:
	std::vector<awpRect> rects_;
	ILFScanner* scanner_ = nullptr;
};


class TLFFragmentBuilder
{
public:
	TLFFragmentBuilder(ILFScanner* scanner);
	std::shared_ptr<TLFFragments> Build(TLFImage* img);
private:
	ILFScanner* scanner_ = nullptr;
	std::shared_ptr<TLFFragments> fragments_ = nullptr;
	int height_ = 0;
	int width_ = 0;
};

class TLFDetections {
public:
	TLFDetections(std::vector<awpRect>& rects);
	const std::vector<awpRect>& GetDetections() const;
private:
	std::vector<awpRect> rects_;
};

class TLFSupervisor
{
public:
	TLFSupervisor();
	std::shared_ptr<TLFDetections> Detect(TLFImage* img);
};

class TLFAgent
{
public:
	TLFAgent();
	void SetSupervisor(TLFSupervisor* supervisor);
	std::unique_ptr<TLFSemanticImageDescriptor> Detect(TLFImage* img);
	bool LoadXML(const char* lpFileName);
	bool LoadXML(TiXmlElement* parent);
	bool SaveXML(const char* lpFileName);
	TiXmlElement* SaveXML();
private:
	std::unique_ptr<TLFFragmentBuilder> fragment_builder_;
	ILFScanner* scanner_;
	TLFSupervisor* supervisor_;
};


class TLFResult {
public:
	//TODO
	bool result;
};


class TLFDiscriptionFeatures {
public:
	TLFDiscriptionFeatures(std::vector<size_t>& feature_count_list);
	size_t GetCountFeature(size_t cascade_index) const;
	size_t GetCascadeCount() const;
private:
	const std::vector<size_t> feature_count_list_;
}; 

class TLFFeature {
public:
	TLFFeature(float* data, std::shared_ptr<TLFDiscriptionFeatures>, size_t triggered_cascade);
	float* GetData(size_t fragment_index, size_t feature_index);
private:
	float* data_;
	std::shared_ptr<TLFDiscriptionFeatures> batch_discription_features_;
	size_t triggered_cascade_;
};
/*
class TLFFeature {
public:
	TLFFeature(float* data, const TLFDiscriptionFeatures&, const std::vector<size_t>& triggered_cascade);
	float* GetData(size_t fragment_index, size_t feature_index);
private:
	float* data_;
	const TLFDiscriptionFeatures& batch_discription_features_;
	std::vector<size_t> triggered_cascade_; //kolichestvo srabotavshih kaskadov = kolichestvo features(1000)
};
*/


class TLFDetector
{
public:
	TLFDetector();
	void Init();
	void Detect(TLFImage* img, std::shared_ptr<TLFFragments> fragments);
	void SetCallback(std::function<void(const std::vector<TLFResult>&, const std::vector<std::shared_ptr<TLFFeature>>&)> callback);

private:
	std::function<void(const std::vector<TLFResult>&, const std::vector<std::shared_ptr<TLFFeature>>&)> callback_;
	std::shared_ptr<TLFDiscriptionFeatures> discription_features_;
};

class TLFCorrectors
{
public:
	TLFCorrectors();
	TLFResult correct(std::vector<TLFFeature> list);
	void LoadXML();
	void SaveXML();
private:

};

class TLFTrainerCorrectors
{
public:
	TLFTrainerCorrectors();
	//void addSamples(TLFResult/*ot detectora*/, return supervisor->detect(TLFImage * img), vector<TLFFeature>(size = nskolko), size_t(ot kuda), size_t nskolko)
	void addSamples(TLFResult result, std::shared_ptr<TLFDetections> detections, TLFDiscriptionFeatures features);
};

#endif //__lf_builder_h__