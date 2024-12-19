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


class TLFAgent
{
public:
	TLFAgent();
	//ILFDescriptor* detect(TLFImage* img);
	std::unique_ptr<TLFSemanticImageDescriptor> Detect(TLFImage* img);
	void LoadXML();
	void SaveXML();
private:
	std::unique_ptr<TLFFragmentBuilder> fragment_builder_;
	ILFScanner* scanner_;
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

class TLFResult {
public:
	//TODO
	bool result;
};

class TLFFeature {
public:
	TLFFeature(float* data, std::vector<size_t>& kascade_size);
	float* GetData(size_t index);
	size_t GetSizeData(size_t index);
private:
	float* data_;
	std::vector<size_t> kascade_size_;
};

class TLFFeatures {
public:
	TLFFeatures(const std::vector<TLFFeature>& features, size_t start_pos);
	size_t GetLength();
	size_t GetStartPosition();
private:
	size_t start_pos_;
	size_t length_;
	std::vector<TLFFeature> features_;
};

class TLFDetector
{
public:
	void Detect(TLFImage* img, TLFFragments, void(*callback)(TLFResult result, TLFFeature feature, TLFFeatures features) );
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
	void addSamples(TLFResult result, std::shared_ptr<TLFDetections> detections, TLFFeatures features);
};

#endif //__lf_builder_h__