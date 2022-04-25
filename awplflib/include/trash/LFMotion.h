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
//		File: LFMotion.h
//		Purpose: Declares all parts of Motion Detection and Tracking engine 
//		Declasres:
//		TLFMotionWeak				- motion weak classifier 
//		TLFMotionDetector			- motion detector
//		TLFMotionEngine				- motion detector and tracker 
//
//      CopyRight 2004-2018 (c) NN-Videolab.net
//M*/

#ifndef _LFMOTION_H_
#define _LFMOTION_H_
/** \defgroup LFMotion 
*	Implementation of Motion Detection Engine 
*	@{
*/
class TLFMotionWeak : public ILFWeak
{
protected:
	double m_sum;
	int    m_counter;
	double m_threshold;
public:
	TLFMotionWeak(double t);
	TLFMotionWeak(ILFFeature* feature, double t);
	TLFMotionWeak(TLFMotionWeak& Weak);
	TLFMotionWeak(TLFMotionWeak* weak);
	virtual ~TLFMotionWeak();

	virtual int Classify(TLFImage* pImage, double* value = NULL);
	virtual void SaveXML(TiXmlElement* parent);
	virtual bool LoadXML(TiXmlElement* parent);

	double GetThreshold();
	void SetThreshold(double v);

	virtual const char* GetName()
	{
		return "TLFMotionWeak";
	}
};

class TLFMotionDetector : public ILFObjectDetector
{
protected:
	int m_counter;
	TLFObjectList m_weaks_a;
	TLFObjectList m_weaks_s;
	TLFObjectList m_weaks_h;
	TLFObjectList m_weaks_v;
	TLFObjectList m_weaks_d;
	double m_threshold;
	int    m_delay;
public:
	TLFMotionDetector();

	virtual bool Init(awpImage* pImage, bool scan = false);
	// classification
	virtual int  ClassifyRect(awpRect Fragmnet, double* err, int* vect);
	virtual int  Detect();
	// properties
	virtual int	  GetStagesCount();
	virtual bool  AddStrong(ILFStrong* strong);
	virtual  double GetThreshold();
	virtual  void SetThreshold(double Value);


	virtual TiXmlElement* SaveXML();
	virtual bool          LoadXML(TiXmlElement* parent);

	virtual const char* GetName()
	{
		return "TLFMotionDetector";
	}
};

class TLFMotionEngine : public ILFDetectEngine
{
protected:
	double m_minSize;
	double m_maxSize;

	TLFObjectList m_tmpList;
	virtual void OverlapsFilter(TLFSemanticImageDescriptor* descriptor);
	virtual void InitDetectors();

	bool 	SquareFilter(awpRect& rect);
	void    BuildForeground();

	TLFImage 			m_foreground;
	TLFThresholdProc    m_processor;

public:
	TLFMotionEngine();
	virtual void Clear();
	virtual bool LoadXML(TiXmlElement* parent);
	virtual bool FindObjects();
	virtual TiXmlElement*  SaveXML();

	double GetMinSize();
	double GetMaxSize();

	void SetMinSize(double value);
	void SetMaxSize(double value);

	awpImage* GetForeground();
	TLFImage* GetForegroundImage();

	TLFThresholdProc* GetThresholdProc();

	virtual const char* GetName()
	{
		return "TLFMotionEngine";
	}
};

/** @} */ /*  end of LFMotion group */

#endif // _LFMOTION_H_