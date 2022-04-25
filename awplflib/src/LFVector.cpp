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
//		File: LFVector.cpp
//		Purpose: Implements TLFDouble, TLFDblVector
//
//      CopyRight 2004-2018 (c) NN-Videolab.net
//M*/
#include "_LF.h"

// double object 
TLFDouble::TLFDouble(double value)
{
	m_value = value;
}
TLFDouble::TLFDouble(TLFDouble& v)
{
	m_value = v.m_value;
}
TLFDouble::TLFDouble(TLFDouble* v)
{
	m_value = v->m_value;
}
double TLFDouble::Get()
{
	return m_value;
}
void   TLFDouble::Set(double v)
{
	m_value = v;
}
bool TLFDouble::LoadXML(TiXmlElement* parent)
{
	this->m_value = 0;
	if (parent == NULL)
		return false;
	if (strcmp(parent->Value(), this->GetName()) != 0)
		return false;
	if (parent->QueryDoubleAttribute("value", &this->m_value) == TIXML_NO_ATTRIBUTE)
		return false;
	return true;
}
TiXmlElement* TLFDouble::SaveXML()
{
	TiXmlElement* e = new TiXmlElement(GetName());
	e->SetDoubleAttribute("value", this->m_value);
    return e;
}

// 
TLFDblVector::TLFDblVector(int id)
{
	m_id = id;
}
double TLFDblVector::GetValue(int index)
{
	if (index < 0 || index >= this->GetCount())
		return DBL_MAX;
	TLFDouble* dbl = (TLFDouble*)Get(index);
	return dbl->Get();
}
void   TLFDblVector::SetValue(int index, double value)
{
	if (index < 0 || index >= this->GetCount())
		return;
	TLFDouble* dbl = (TLFDouble*)Get(index);
	dbl->Set(value);
}
void   TLFDblVector::AddValue(double value)
{
	TLFDouble* o = new TLFDouble(value);
	Add(o);
}

double TLFDblVector::NormL2()
{
	double norm = 0;
	for (int i = 0; i < GetCount(); i++)
		norm += GetValue(i);
	return sqrt(norm);
}

int TLFDblVector::id()
{
	return m_id;
}