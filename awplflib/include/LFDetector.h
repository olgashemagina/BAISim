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
//		File: LFDetector.h
//		Purpose: Declares TSCObjectDetector and TLFFGBGDetector 
//
//      CopyRight 2004-2018 (c) NN-Videolab.net
//M*/

#ifndef HLDETECTORH
#define HLDETECTORH

/** \defgroup LFDetectors
*	Implementation general object and events detectors of the Locate Framework
*   @{
*/
//---------------------------------------------------------------------------
// cascade detecor with TLFCSStrong and ILFWeak classifiers 
class TSCObjectDetector : public ILFObjectDetector
{
public:
	TSCObjectDetector();
	virtual ~TSCObjectDetector();

   // init detector with image
	virtual bool Init(awpImage* pImage, bool scan = false);
   // classification
   virtual int  ClassifyRect(awpRect Fragmnet, double* err, int* vect);
   virtual int  Detect();
   int DetectInRect(awpRect roi);
   // properties
	virtual  double GetThreshold();
    virtual  void	SetThreshold(double Value);
    // xml support
    virtual TiXmlElement* SaveXML();
	virtual bool          LoadXML(TiXmlElement* parent);
	// file io 
	bool SaveDetector(const char* lpFileName);
	bool LoadDetector(const char* lpFileName);
	// stages
	virtual bool	AddStrong(ILFStrong* strong);


	virtual const char* GetName()
	{
		return "TSCObjectDetector";
	}
};

// foreground detector is used for 
// tracking, package, crowd and counters
class TLFFGBGDetector : public ILFObjectDetector
{
protected:
	int m_counter;
	TLFObjectList m_weaks_a;
	TLFObjectList m_weaks_s;
	TLFObjectList m_weaks_h;
	TLFObjectList m_weaks_v;
	TLFObjectList m_weaks_d;
	double m_threshold;
	int    m_buf_size;
    int    m_bg_stability;
    int    m_delay;
public:
	TLFFGBGDetector();
	virtual ~TLFFGBGDetector();

	virtual bool Init(awpImage* pImage, bool scan = false);
	// classification
	virtual int  ClassifyRect(awpRect Fragmnet, double* err, int* vect);
	virtual int  Detect();
	// properties
	virtual int	  GetStagesCount();
	virtual bool  AddStrong(ILFStrong* strong);
	virtual  double GetThreshold();
	virtual  void SetThreshold(double Value);


	int GetAverageBufferSize();
	void SetAverageBufferSize(int value);

	int GetBgStability();
	void SetBgStability(int value);

	int GetDelay();
	void SetDelay(int value);

	virtual TiXmlElement* SaveXML();
	virtual bool          LoadXML(TiXmlElement* parent);

	virtual const char* GetName()
	{
		return "TLFFGBGDetector";
	}
};

// smoke detector 
class TLFSmokeDetector :public ILFObjectDetector
{
protected:
	double m_threshold;
	TLFObjectList m_weaks_s;
	TLFObjectList m_weaks_a;
public:
	TLFSmokeDetector();
	virtual ~TLFSmokeDetector();

	// init detector with image
	virtual bool Init(awpImage* pImage, bool scan = true);
	// classification
	virtual int  ClassifyRect(awpRect Fragmnet, double* err, int* vect);
	virtual int  Detect();
	// properties
	int GetStagesCount();
	int		GetSensorsCount(int Stage);
	double  GetStageWeight(int Stage);
	double	GetStageThreshold(int Stage);
	TLFObjectList* GetStrongs();
	virtual bool	AddStrong(ILFStrong* strong);
	virtual  double GetThreshold();
	virtual  void SetThreshold(double Value);

	// xml support
	virtual TiXmlElement* SaveXML();
	virtual bool          LoadXML(TiXmlElement* parent);

	virtual const char* GetName()
	{
		return "TLFSmokeDetector";
	}
};

// fire detector 
class TLFFireDetector : public ILFObjectDetector
{
protected:
	double m_threshold;
	TLFObjectList m_weaks_a;
#ifdef _DEBUG
	FILE* m_log;
#endif
public:
	TLFFireDetector();
	virtual ~TLFFireDetector();

	// init detector with image
	virtual bool Init(awpImage* pImage, bool scan = true);
	// classification
	virtual int  ClassifyRect(awpRect Fragmnet, double* err, int* vect);
	virtual int  Detect();
	// properties
	int GetStagesCount();
	int		GetSensorsCount(int Stage);
	double  GetStageWeight(int Stage);
	double	GetStageThreshold(int Stage);
	TLFObjectList* GetStrongs();
	virtual bool	AddStrong(ILFStrong* strong);
	virtual  double GetThreshold();
	virtual  void SetThreshold(double Value);
	virtual TiXmlElement* SaveXML();
	virtual bool          LoadXML(TiXmlElement* parent);

	virtual const char* GetName()
	{
		return "TLFFireDetector";
	}
};
/** @} */ /*  end of LFDetectors group */
#endif
