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
//		File: LFVector.h
//		Purpose: Contains double labeled vectors 
//
//      CopyRight 2004-2018 (c) NN-Videolab.net
//M*/
#ifndef _lf_vector_h_
#define _lf_vector_h_
/** \defgroup LFVectors
*	Implementation lebeled lists of numbers in the Locate Framework
*   @{
*/
class TLFDouble : public TLFObject
{
protected:
	double m_value;
public:
	TLFDouble(double value);
	TLFDouble(TLFDouble& v);
	TLFDouble(TLFDouble* v);
	double Get();
	void   Set(double v);
	bool LoadXML(TiXmlElement* parent);
	TiXmlElement* SaveXML();
	virtual const char* GetName(){ return "TLFDouble"; }
};

// vector of double values with label
class TLFDblVector : public TLFObjectList
{
protected:
	int m_id; // id
public:

	TLFDblVector(int id);
	double GetValue(int index);
	void   SetValue(int index, double value);
	void   AddValue(double value);
	double NormL2();
	// id
	int id();
	virtual const char* GetName(){ return "TLFDblVector"; }
};
/** @} */ /*  end of LFVectors group */
#endif //_lf_vector_h_