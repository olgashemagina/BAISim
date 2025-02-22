/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                           License Agreement
//                  Locate Framework  Computer Vision Library
//
// Copyright (C) 2004-2018, NN-Videolab.net, all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the NN-Videolab.net or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//      Locate Framework  (LF) Computer Vision Library.
//		File: LFEngine.h
//		Purpose: Different engines to convert image to semantic descriprion
//		 
//      CopyRight 2004-2018 (c) NN-Videolab.net
//M*/

#include "LFThresholdProc.h"
#ifndef __LFENGINE_H__
#define __LFENGINE_H__

#include "LF.h"
#include "LFThresholdProc.h"
#include "LFAgent.h"

#include <vector>
#include <map>
#include <string>
#include <optional>
#include <memory>


class TLFTreeEngine : public TLFObject {
	struct tree_t : public std::map<std::string, std::pair<std::shared_ptr<ILFObjectDetector>, tree_t>> {};
	struct agent_tree_t : public std::map<std::string, std::pair<std::shared_ptr<TLFAgent>, agent_tree_t>> {};

	using detectors_t = std::vector<std::shared_ptr<ILFObjectDetector>>;
	using agents_t = std::vector<std::shared_ptr<TLFAgent>>;

public:
	/*Loading from-to xml files*/
	virtual bool Load(const std::string& filename);
	virtual bool Save(const std::string& filename);

	//Loading/Saving stuff
	virtual bool LoadXML(TiXmlElement* parent);
	virtual TiXmlElement* SaveXML();

	virtual void SetAgentMode();

private:
	static TiXmlElement* SaveXML(const tree_t& tree, detectors_t& detectors);
	static tree_t LoadXML(TiXmlElement* node, const detectors_t& detectors);

	static TiXmlElement* SaveXML(const agent_tree_t& tree, agents_t& agents);
	static agent_tree_t LoadXML(TiXmlElement* node, const agents_t& agents);
	

	static std::shared_ptr<ILFObjectDetector> LoadDetector(TiXmlElement* node);
	static std::shared_ptr<TLFAgent> LoadAgent(TiXmlElement* node);


public:
	//Run hierarchy on image
	std::optional<std::vector<detected_item_ptr>> Detect(TLFImage* pImage) {
		return DetectInRect(tree_, pImage, nullptr);
	}
	std::optional<std::vector<detected_item_ptr>> Detect(std::shared_ptr<TLFImage> pImage) {
		return DetectInRect(tree_, pImage.get(), nullptr);
	}

	//Set detector to the node specified by path;
	bool		SetDetector(const std::string& path, std::shared_ptr<ILFObjectDetector> detector);

	//Get detector specified by path;
	std::shared_ptr<ILFObjectDetector>		GetDetector(const std::string& path) const;

	//Set agent to the node specified by path;
	bool SetAgent(const std::string& path, std::shared_ptr<TLFAgent> agent);
	//Get agent specified by path;
	std::shared_ptr<TLFAgent> GetAgent(const std::string& path) const;

	virtual const char* GetName()
	{
		return "TLFTreeEngine";
	}

public:
	//Split path to the vector
	static std::vector<std::string> SplitPath(const std::string& s);

private:
	std::optional<std::vector<detected_item_ptr>>		
		DetectInRect( const tree_t& tree, TLFImage* image, awpRect* rect);

private:
	tree_t				tree_;
	agent_tree_t		agent_tree_;
	bool				use_agent = false;
};


/** \defgroup LFEngines
*	Implementation general semantic engines of the Locate Framework
*   @{
*/
// general object detection engine 
class TLFDetectEngine : public ILFDetectEngine
{
protected:
	TLFObjectList m_tmpList;
	ILFObjectDetector* LoadDetector(TiXmlElement* parent);

	virtual void OverlapsFilter(TLFSemanticImageDescriptor* descriptor) override;
	virtual void InitDetectors() override;

public:
	TLFDetectEngine();
	virtual ~TLFDetectEngine();
	
	bool LoadXML(TiXmlElement* parent) override;
    bool FindObjects() override;
	void Clear() override;
	TiXmlElement*  SaveXML() override;

	virtual const char* GetName()
    {
        return "TLFDetectEngine";
    }
};

// foreground detector
class TLFFGEngine : public ILFDetectEngine
{
protected:
	bool m_relax;
	int m_relax_count;
	double m_minSize;
	double m_maxSize;
	double m_minAR;
	double m_maxAR;
	TLFObjectList m_tmpList;
	virtual void OverlapsFilter(TLFSemanticImageDescriptor* descriptor);
	virtual void InitDetectors();

	bool 	SquareFilter(awpRect& rect);
	bool 	ARFilter(awpRect& rect);
	void    BuildForeground();

	TLFImage 			m_foreground;
	TLFThresholdProc    m_processor;

public:
	TLFFGEngine();
	virtual void Clear();
	virtual bool LoadXML(TiXmlElement* parent);
	virtual bool FindObjects();
	virtual TiXmlElement*  SaveXML();
	double GetMinSize();
	double GetMaxSize();
	double GetMinAR();
	double GetMaxAR();

	void SetMinSize(double value);
	void SetMaxSize(double value);
	void SetMinAR(double value);
	void SetMaxAR(double value);

	int GetAverageBufferSize();
	void SetAverageBufferSize(int value);

	int GetBgStability();
	void SetBgStability(int value);

	int GetDelay();
	void SetDelay(int value);

	awpImage* GetForeground();
	TLFImage* GetForegroundImage();

	TLFThresholdProc* GetThresholdProc();

	virtual const char* GetName()
	{
		return "TLFFGEngine";
	}
};
/** @} */ /*  end of LFEngines group */
#endif //__LFENGINE_H__
