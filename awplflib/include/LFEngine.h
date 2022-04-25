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

#ifndef __LFENGINE_H__
#define __LFENGINE_H__
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
