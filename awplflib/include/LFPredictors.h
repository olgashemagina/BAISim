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
//	File: LFPredictors.h
//	Purpose: Different predictors 
//	Copyright (C) 2004-2018, NN-Videolab.net, all rights reserved.
//M*/

#ifndef _LF_PREDICTORS_
#define _LF_PREDICTORS_

/** \defgroup LFPredictors 
*	Implementation of predictors and tracking engine 
*	@{
*/

// average area, based on last stage of the engines first detector
class TLFAverageNNPredictor : public ILFPredictor
{
protected:
	TLFObjectList  m_list;
	awpRect		   m_rect;
	virtual TLFDblVector* Features(ILFDetectEngine* engine, TLFRect* rect, int id);
	virtual bool Classify(TLFDblVector* d, double* err);
public:
	TLFAverageNNPredictor();
	virtual void Learn(ILFDetectEngine* engine, TLFRect* rect);
	virtual TLFRect* Predict(ILFDetectEngine* engine);
	virtual const char* GetName()
	{
		return "TLFAverageNNPredictor";
	}
};
// average area with empty detector 
class TLFEmptyAverageNNPredictor : public TLFAverageNNPredictor
{
protected:
	virtual TLFDblVector* Features(ILFDetectEngine* engine, TLFRect* rect, int id);
};
//
class TLFNullPredictor : public ILFPredictor
{
protected:
//	TLFRect* m_pPredicted;
	virtual TLFDblVector* Features(ILFDetectEngine* engine, TLFRect* rect, int id);
	virtual bool Classify(TLFDblVector* d, double* err);
public:
	TLFRect* GetPredicted();
	virtual void Learn(ILFDetectEngine* engine, TLFRect* rect);
	virtual TLFRect* Predict(ILFDetectEngine* engine);
};
// estimates brigthness of tracking box over
// scanned fragments
class TLFIntegralImagePredictor : public ILFPredictor
{
protected:
	TLFTileScaleScanner* m_scanner;
	TLFRect    m_rect;
	virtual TLFDblVector* Features(ILFDetectEngine* engine, TLFRect* rect, int id);
	virtual bool Classify(TLFDblVector* d, double* err);
public:
	TLFIntegralImagePredictor();
	virtual ~TLFIntegralImagePredictor();
	virtual void Learn(ILFDetectEngine* engine, TLFRect* rect);
	virtual TLFRect* Predict(ILFDetectEngine* engine);
	virtual const char* GetName()
	{
		return "TLFIntegralImagePredictor";
	}
};

// moments predictor
class TLFMomentsPredictor : public ILFPredictor
{
protected:
	TLFRect    m_rect;
	virtual TLFDblVector* Features(ILFDetectEngine* engine, TLFRect* rect, int id);
	virtual bool Classify(TLFDblVector* d, double* err);
public:
	TLFMomentsPredictor();
	virtual ~TLFMomentsPredictor();
	virtual void Learn(ILFDetectEngine* engine, TLFRect* rect);
	virtual TLFRect* Predict(ILFDetectEngine* engine);
	virtual const char* GetName()
	{
		return "TLFMomentsPredictor";
	}
};

//car predictor 
class TLFCarPredictor : public ILFPredictor
{
protected:
	TLFRect    m_rect;
	awpImage*  m_image;
	virtual TLFDblVector* Features(ILFDetectEngine* engine, TLFRect* rect, int id);
	virtual bool Classify(TLFDblVector* d, double* err);
public:
	TLFCarPredictor();
	virtual ~TLFCarPredictor();
	virtual void Learn(ILFDetectEngine* engine, TLFRect* rect);
	virtual TLFRect* Predict(ILFDetectEngine* engine);
	virtual const char* GetName()
	{
		return "TLFCarPredictor";
	}
};
/** @} */ /*  end of LFPredictors group */
#endif