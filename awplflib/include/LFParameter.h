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
//		File: LFParameter.h
//		Purpose: Paramters implemetetion 
//
//      CopyRight 2004-2020 (c) NN-Videolab.net
//M*/

#ifndef _LF_PARAMETER_H_
#define _LF_PARAMETER_H_
/** \defgroup LFParameter
*	Implementation paramenter im the Locate Framework 
*	@{
*/
#define LF_MAX_NAME_LEN		128
#define LF_MAX_DESCR_LEN	256
class TLFParameter : public TLFObject
{
protected:
	char m_Name[LF_MAX_NAME_LEN];
	char m_Descr[LF_MAX_DESCR_LEN];
	double m_Value;
	double m_MaxValue;
	double m_MinValue;
public:
	TLFParameter();
	virtual ~TLFParameter();

	char* GetPName();
	char* GetDescription();
	double GetValue();
	double GetMaxValue();
	double GetMinValue();
	void  SetPName(const char* Value);
	void  SetDescription(const char* Value);
	bool  SetValue(double Value);
	bool  SetMaxValue(double Value);
	bool  SetMinValue(double Value);

	bool LoadXML(TiXmlElement* parent);
	TiXmlElement* SaveXML();

    virtual const char* GetName()
    {
        return "TLFParameter";
    }
};
/** @} */ /*  end of LFParameter group */
#endif //_LF_PARAMETER_H_