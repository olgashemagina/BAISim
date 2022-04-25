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
//		File: LFHistogramm.h
//		Purpose: Contains Histogramm 
//
//      CopyRight 2004-2018 (c) NN-Videolab.net
//M*/
#ifndef _lf_histogramm_h_
#define _lf_histogramm_h_

/** \addtogroup LFBuffers
*   @{
*/

/**
*	\brief Histogramm
*/
class TLFHistogramm : public TLFObject
{
protected:
	double* m_data;
	double  m_min;
	double  m_max;
	int     m_num_bins;
public:
	TLFHistogramm();
	TLFHistogramm(int num_bins, double min, double max);
	virtual ~TLFHistogramm();

	void Setup(int num_bins, double min, double max);

	void AddElememt(double v);
	double GetSumToBinForward(int bin);
	double GetSumToBinBackward(int bin);
	double GetThrBin(int bin);
	double GetBin(int bin);
	double GetSum();
	double GetMax();
	double GetMin();
	double GetMaxBin(int& bin);
	double GetMinBin(int& bin);


	bool Save(const char* lpFileName);
	bool Load(const char* lpFileName);

	bool LoadXML(TiXmlElement* parent);
	TiXmlElement* SaveXML();

	// PROPS
	int GetNumBins();
	virtual const char* GetName(){ return "TLFHistogramm"; }
};
/** @} */ /*  end of LFHistogramm group */
#endif //_lf_histogramm_h_