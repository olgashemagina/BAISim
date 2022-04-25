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
//	Locate Framework  4.0
//	File: LFSabotage.h
//	Purpose: Camera Sabotage Engine Implementation 
//	Copyright (C) 2004-2020, NN-Videolab.net, all rights reserved.
//M*/

/** \defgroup LFSabotage
*	Implementation of sabotage engine 
*	@{
*/

#ifndef LFSabotageH
#define LFSabotageH
// forwrd 
#define SABOTAGE_BUF_SIZE 12 
class TLFSabotageWeak : public ILFWeak
{
protected:
	int		m_numFramesToAvg;
	int		m_bufSize;
	double* m_story_buffer;
	double  m_sens; 
	double  m_minValue;
	double  m_maxValue;
	double  m_diffValue;
	int     m_classifyCount;
	int		m_state;
    AWPDWORD   m_dt[SABOTAGE_BUF_SIZE];
    AWPDWORD   m_ct;
	int	    m_bf; // number frame in the buffer 
public:
	TLFSabotageWeak(ILFFeature* feature, double sens);
	virtual ~TLFSabotageWeak();
	/*
		Классифицирует участок изображения, связанный с TLFAvgFeature
		0 - все ок
		1 - слишком темно
		2 - слишком светло 
		3 - резкие изменения 
	*/
	virtual int Classify(TLFImage* pImage,  double* value = NULL);

	virtual void SaveXML(TiXmlElement* parent);
	virtual bool LoadXML(TiXmlElement* parent);

	void Clear();
	int  GetState();

	virtual const char* GetName()
	{
		return "TLFSabotageWeak";
	}
};
class TLFSabotageStrong : public ILFStrong
{
protected:
	
public: 
	/*
	XML io operations
	*/
	virtual void SaveXML(TiXmlElement* parent);
	virtual bool LoadXML(TiXmlElement* parent);
	/*
	Classification
	0 - object present
	1 - object absence
	*/
	virtual int Classify(TLFImage* pImage, double& err);
	void Clear();
};

//
class TLFSabotage : public TLFObject
{
protected:
	TLFSabotageStrong m_classifyer;
	TLFImage*		  m_CurrentImage;
	int				  m_state;
	AWPWORD			  m_srcWidth;
	AWPWORD			  m_srcHeight;
	AWPBYTE			  m_srcBPP;

    AWPDWORD			  m_event_duration;
    AWPDWORD			  m_event_start;

	double			  m_time_thr;

	void Clear();
	void Init(TLFImage* img);

	bool Preprocess(awpImage* img);

	
public:
	TLFSabotage();
	TLFSabotage(double s, double t);
	virtual ~TLFSabotage();

	bool PorocessImage(awpImage* img, bool* result);
	int GetState();
	virtual const char* GetName()
	{
		return "TLFSabotage";
	}
};
/** @} */ /*  end of LFSabotage group */
#endif 
