#ifndef __lf_builder_h__
#define __lf_builder_h__

#include "LFImage.h"
#include "LF.h"

#include <numeric>
#include <memory>
#include <mutex>

class TLFDetections {
public:
	TLFDetections(std::vector<awpRect>& rects) {
		rects_ = std::move(rects);
	}
	const std::vector<awpRect>& GetDetections() const {
		return rects_;
	}
private:
	std::vector<awpRect> rects_;
};

class ILFSupervisor
{
public:
	virtual std::shared_ptr<TLFDetections> Detect(std::shared_ptr<TLFImage> img) = 0;
};


class IAgent {
public:
	virtual void SetSupervisor(ILFSupervisor* supervisor) = 0;
	virtual std::unique_ptr<TLFSemanticImageDescriptor> Detect(std::shared_ptr<TLFImage> img) = 0;
	virtual bool LoadXML(const char* lpFileName) = 0;
	virtual bool SaveXML(const char* lpFileName) = 0;
};

std::shared_ptr<IAgent> CreateAgent();

#endif //__lf_builder_h__