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
#include <vector>
#include <map>
#include <string>
#include <optional>
#include <memory>


class TLFTreeEngine : public TLFObject {
	struct tree_t : public std::map<std::string, std::pair<std::shared_ptr<ILFObjectDetector>, tree_t>> {};

	using detectors_t = std::vector<std::shared_ptr<ILFObjectDetector>>;

public:
	/*Loading from-to xml files*/
	virtual bool Load(const std::string& filename);

	virtual bool Save(const std::string& filename);

	virtual bool LoadXML(TiXmlElement* parent);

	virtual TiXmlElement* SaveXML();

private:
	static TiXmlElement* SaveXML(const tree_t& tree, detectors_t& detectors);

	static tree_t LoadXML(TiXmlElement* node, const detectors_t& detectors);

	static std::shared_ptr<ILFObjectDetector> LoadDetector(TiXmlElement* node);


public:
	std::optional<std::vector<detected_item_ptr>> Detect(TLFImage* pImage) {
		return DetectInRect(tree_, pImage, nullptr);
	}


	bool		SetDetector(const std::string& path, std::shared_ptr<ILFObjectDetector> detector) {
		tree_t& tree = tree_;
		auto splitted = SplitPath(path);

		auto detector_name = splitted.back();
		splitted.pop_back();

		for (const auto& name : splitted) {
			auto it = tree.find(name);
			if (it != tree.end()) {
				tree = it->second.second;
			}
			else {
				//Cant find node in tree;
				return false;
			}

		}
		tree[detector_name] = { std::move(detector), tree_t{} };
		return true;
	}

	std::shared_ptr<ILFObjectDetector>		GetDetector(const std::string& path) const {
		tree_t const* tree = &tree_;
		std::shared_ptr<ILFObjectDetector> detector;

		auto splitted = SplitPath(path);
		for (const auto& name : splitted) {
			auto it = tree->find(name);
			if (it != tree->end()) {
				tree = &it->second.second;
				detector = it->second.first;
			}
			else {
				//Cant find node in tree;
				return nullptr;
			}

		}
		
		return detector;
	}

	virtual const char* GetName()
	{
		return "TLFTreeEngine";
	}

public:
	static std::vector<std::string> SplitPath(const std::string& s) {
		std::vector<std::string> result;
		std::stringstream ss(s);
		std::string item;

		while (std::getline(ss, item, '.')) {
			result.push_back(item);
		}

		return result;
	}


private:
	std::optional<std::vector<detected_item_ptr>>		
		DetectInRect( const tree_t& tree, TLFImage* image, awpRect* rect) {
		std::vector<detected_item_ptr>		detections;

		for (const auto& [name, node] : tree) {

			if (!node.first->Init(image, rect))
				//Cant initialize scanner or image;
				return std::nullopt;

			//TODO: add NMS
			//TODO: move Non Maximum Suppression to detector;
			if (node.first->Detect() > 0) {
				//Objects detected, run tree
				for (int i = 0; i < node.first->GetNumItems(); i++) {
					TLFDetectedItem* item = node.first->GetItem(i);
					auto leaf_rect = item->GetBounds()->GetRect();

					auto parent = std::make_shared<TLFDetectedItem>(*item);

					detections.push_back(parent);

					auto detected = DetectInRect(node.second, image, &leaf_rect);

					if (!detected)
						return std::nullopt;

					for (auto child : *detected) {
						//TODO: move to TLFSemanticImageDescriptor
						if (!child->parent())
							child->set_parent(parent);

						detections.push_back(child);
					}

				}

			}
		}

		return detections;
	}

private:
		
	tree_t				tree_;

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
	virtual void OverlapsFilter(TLFSemanticImageDescriptor* descriptor);
	virtual void InitDetectors();

public:
	TLFDetectEngine();
	virtual ~TLFDetectEngine();
	virtual bool LoadXML(TiXmlElement* parent);
    virtual bool FindObjects();
	virtual void Clear();
	virtual TiXmlElement*  SaveXML();

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
