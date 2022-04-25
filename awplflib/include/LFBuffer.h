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
//		File: LFBuffer.h
//		Purpose: Contains double buffers 
//
//      CopyRight 2004-2020 (c) NN-Videolab.net
//M*/
#ifndef _lf_buffer_h_
#define _lf_buffer_h_

/** \defgroup LFBuffers
*	Implementation fast buffer of doubles 
*   @{
*/
class TLFBuffer : public TLFObject
{
protected:
	int m_size;
	double* m_data;
	double m_sum;
public:
	TLFBuffer(unsigned int size = 256, double value = 0);
	virtual ~TLFBuffer();
	void Push(double value);
	void  SetData(double* data);
	double* GetData();
	double GetSum();
	double GetAvg();
	double GetValue(int index);
	int    GetSize();
	void   Clear();
	virtual const char* GetName(){ return "TLFBuffer"; }
};

class TLFRingBuffer : public TLFObject
{
protected:
	int m_size;
	double* m_data;
	double m_sum;
	int   m_current;
public:
	TLFRingBuffer(unsigned int size, double value);
	virtual ~TLFRingBuffer();
	void Push(double value);
	double GetSum();
	double GetAvg();
	double GetSigma();
	double GetDisp();
	double GetValue(int index);
	int    GetSize();
	void   Clear();
	virtual const char* GetName(){ return "TLFRingBuffer"; }
};
/** @} */ /*  end of LFBuffers group */
#endif //_lf_buffer_h_