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
//		File: _LFParameters.cpp
//		Purpose: implementation of ILFParameter, ILFParameters interfaces
//
//      CopyRight 2004-2022 (c) NN-Videolab.net
//M*/
#include "_LFParameters.h"

_TLFParameter::_TLFParameter(const char* name, const char* comment, const char* value)
{
	m_strName = name;
	m_strComment = comment;
	m_strValue = value;
}
_TLFParameter::~_TLFParameter()
{

}
const char* _TLFParameter::name()
{
	return m_strName.c_str();
}
const char* _TLFParameter::comment()
{
	return m_strComment.c_str();
}

const char* _TLFParameter::value()
{
	return m_strValue.c_str();
}
const double _TLFParameter::doubleValue()
{
	return atof(m_strValue.c_str());
}
const double _TLFParameter::intValue()
{
	return atoi(m_strValue.c_str());
}

void _TLFParameter::setValue(const char* value)
{
	m_strValue = value;
}
void _TLFParameter::setDouble(const double value)
{
	char buf[256];
	sprintf(buf, "%lf", value);
	setValue(buf);
}
void _TLFParameter::setInt(const int value)
{
	char buf[256];
	sprintf(buf, "%d", value);
	setValue(buf);
}

_TLFParameters::_TLFParameters()
{

}

int _TLFParameters::Count()
{
	return m_list.GetCount();
}
ILFParameter* _TLFParameters::Parameter(unsigned int index)
{
	if (index >= m_list.GetCount())
		return NULL;
	return (ILFParameter*)m_list.Get(index);
}
ILFParameter* _TLFParameters::Parameter(const char* value)
{
	unsigned int index = 0;
	TLFString strName = value;
	for (int i = 0; i < m_list.GetCount(); i++)
	{
		ILFParameter* current = (ILFParameter*)m_list.Get(i);
		TLFString strCurrent = current->name();
		if (strName == strCurrent)
			return current;
	}
	return NULL;
}

void _TLFParameters::AddParameter(const char* name, const char* value, const char* comment)
{
	_TLFParameter* p = new _TLFParameter(name, comment, value);
	m_list.Add(p);
}


