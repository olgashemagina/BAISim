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
//		File: LFAvgFeature.h
//		Purpose: Contains double buffers
//
//      CopyRight 2004-2020 (c) NN-Videolab.net
//M*/

#ifndef _LF_AVG_FEATURE_
#define _LF_AVG_FEATURE_
/** \addtogroup LFFeatures
*   @{
*/
#define FEATURE_AVG_SIZE 5 

class TLFAvgFeature : public ILFFeature
{
protected:
	int    m_CalcCount;
	double m_buffer[FEATURE_AVG_SIZE];
public:

	TLFAvgFeature();
	TLFAvgFeature(int sxbase, int sybase, int wbase, int hbase);
	TLFAvgFeature(TLFAvgFeature* sensor);
	virtual unsigned int     uCalcValue(TLFImage* pImage);
	virtual double           fCalcValue(TLFImage* pImage);


	/*
	XML io operations
	*/
	virtual bool LoadXML(TiXmlElement* parent);
	virtual bool SaveXML(TiXmlElement* parent);

	virtual awpRect GetRect();
	void Clear();

	virtual const char* GetName()
	{
		return "TLFAvgFeature";
	}
};

class TLFSigmaFeature : public TLFAvgFeature
{
public:
	TLFSigmaFeature();
	TLFSigmaFeature(int sxbase, int sybase, int wbase, int hbase);
	TLFSigmaFeature(TLFSigmaFeature* feature);
	virtual double           fCalcValue(TLFImage* pImage);
	virtual const char* GetName()
	{
		return "TLFSigmaFeature";
	}
};
/** @} */ /*  end of LFAvgFeature group */
#endif //_LF_AVG_FEATURE_