#pragma once
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
//		File: _LFParameters.h
//		Purpose: implementation of ILFParameter, ILFParameters interfaces
//
//      CopyRight 2004-2022 (c) NN-Videolab.net
//M*/

#include "LFCore.h"
#include "LFInterface.h"

class _TLFParameter : public TLFObject, ILFParameter
{
protected:
	TLFString m_strName;
	TLFString m_strComment;
	TLFString m_strValue;
public:
	_TLFParameter(const char* name, const char* comment, const char* value);
	virtual ~_TLFParameter();
	virtual const char* name();
	virtual const char* comment();

	virtual const char* value();
	virtual const double doubleValue();
	virtual const double intValue();

	virtual void setValue(const char* value);
	virtual void setDouble(const double value);
	virtual void setInt(const int value);
};

class _TLFParameters : public TLFObject, ILFParameters
{
protected:
	TLFObjectList m_list;
public:
	_TLFParameters();
	virtual int Count();
	virtual ILFParameter* Parameter(unsigned int index);
	virtual ILFParameter* Parameter(const char* value);
	void AddParameter(const char* name, const char* value, const char* comment);

};
