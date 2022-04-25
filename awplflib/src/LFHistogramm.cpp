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
//		File: LFHistogramm.cpp
//		Purpose: implements TLFHistogramm
//
//      CopyRight 2004-2018 (c) NN-Videolab.net
//M*/
#include "_LF.h"

TLFHistogramm::TLFHistogramm()
{
	this->m_max = 256;
	this->m_min = 0;
	this->m_num_bins = 256;
	m_data = NULL;
	m_data = (double*)malloc(this->m_num_bins*sizeof(double));
	memset(m_data, 0, this->m_num_bins*sizeof(double));
}

TLFHistogramm::TLFHistogramm(int num_bins, double min, double max)
{
	this->m_max = max;
	this->m_min = min;
	this->m_num_bins = num_bins;
	m_data = NULL;
	m_data = (double*)malloc(this->m_num_bins*sizeof(double));
	memset(m_data, 0, this->m_num_bins*sizeof(double));
}
TLFHistogramm::~TLFHistogramm()
{
	if (this->m_data != NULL)
		free(m_data);
}

void TLFHistogramm::Setup(int num_bins, double min, double max)
{
	if (m_data != NULL)
		free(m_data);

	this->m_max = max;
	this->m_min = min;
	this->m_num_bins = num_bins;
	m_data = NULL;
	m_data = (double*)malloc(this->m_num_bins*sizeof(double));
	memset(m_data, 0, this->m_num_bins*sizeof(double));
}
void TLFHistogramm::AddElememt(double v)
{
	if (v < this->m_min)
	{
		m_data[0]++;
		return;
	}
	if (v > this->m_max)
	{
		m_data[this->m_num_bins - 1]++;
		return;
	}
	int index = 0;
	index = (int)floor((this->m_num_bins - 1)*(v - m_min) / (m_max - m_min));
	m_data[index]++;
}
bool TLFHistogramm::Save(const char* lpFileName)
{
	FILE* f = fopen(lpFileName, "w+t");
	if (f != NULL)
	{
		for (int i = 0; i < this->m_num_bins; i++)
			fprintf(f, "%f\n", m_data[i]);
		fclose(f);
		return true;
	}
	else
		return false;
}
bool TLFHistogramm::Load(const char* lpFileName)
{
	return false;
}

double TLFHistogramm::GetSum()
{
	double result = 0;
	for (int i = 0; i < m_num_bins; i++)
		result += m_data[i];
	return result;
}

double TLFHistogramm::GetSumToBinForward(int bin)
{
	if (bin < 0 || bin > this->m_num_bins)
		return -1;
	double sum = 0;
	double sum1 = 0;
	for (int i = 0; i < this->m_num_bins; i++)
		sum += this->m_data[i];
	if (sum == 0)
		return -1;
	for (int i = 0; i < bin; i++)
		sum1 += this->m_data[i];
	return 100 * sum1 / sum;
}
double TLFHistogramm::GetSumToBinBackward(int bin)
{
	if (bin < 0 || bin > this->m_num_bins)
		return -1;
	double sum = 0;
	double sum1 = 0;
	for (int i = 0; i < this->m_num_bins; i++)
		sum += this->m_data[i];
	if (sum == 0)
		return -1;
	for (int i = this->m_num_bins - 1; i >= bin; i--)
		sum1 += this->m_data[i];
	return 100 * sum1 / sum;
}

int TLFHistogramm::GetNumBins()
{
	return this->m_num_bins;
}
double TLFHistogramm::GetThrBin(int bin)
{
	double delta = (m_max - m_min) / (double)this->m_num_bins;
	return m_min + bin*delta;
}

#define _CHECK_NULL_ \
	if (m_data == NULL)\
		return -1;\


double TLFHistogramm::GetBin(int bin)
{
	_CHECK_NULL_

	return m_data[bin];
}

double TLFHistogramm::GetMax()
{
	_CHECK_NULL_

	double max = m_data[0];
	for (int i = 1; i < m_num_bins; i++)
	{
		if (m_data[i] > max)
			max = m_data[i];
	}
	return max;
}
double TLFHistogramm::GetMin()
{
	_CHECK_NULL_

	double min = m_data[0];
	for (int i = 1; i < m_num_bins; i++)
	{
		if (m_data[i] < min)
			min = m_data[i];
	}
	return min;
}
double TLFHistogramm::GetMaxBin(int& bin)
{
	bin = -1;
	_CHECK_NULL_
    double max = m_data[0];
	bin = 0;
	for (int i = 1; i < m_num_bins; i++)
	{
		if (m_data[i] > max)
		{
			max = m_data[i];
			bin = i;
		}
	}
	return max;
}

double TLFHistogramm::GetMinBin(int& bin)
{
	bin = -1;
	_CHECK_NULL_
	double min = m_data[0];
	bin = 0;
	for (int i = 1; i < m_num_bins; i++)
	{
		if (m_data[i] < min)
		{
			min = m_data[i];
			bin = i;
		}
	}
	return min;
}

bool TLFHistogramm::LoadXML(TiXmlElement* parent)
{
	if (parent == NULL)
		return false;
	if (strcmp(parent->Value(), GetName()) != 0)
		return false;
	parent->QueryDoubleAttribute("min", &this->m_min);
	parent->QueryDoubleAttribute("max", &this->m_max);
	parent->QueryIntAttribute("num", &this->m_num_bins);
	if (m_num_bins < 0)
		return false;
	if (m_data != NULL)
		free(m_data);
	if (m_num_bins == 0)
		return true;
	m_data = (double*)malloc(m_num_bins*sizeof(double));
	int c = 0;
	TLFDouble d(0.);
	for(TiXmlNode* child = parent->FirstChild(); child; child = child->NextSibling() )
	{
		TiXmlElement* e = child->ToElement();
		if(!d.LoadXML(e))
			return false;
		m_data[c] = d.Get();
		c++;
	}

	return true;
}

TiXmlElement* TLFHistogramm::SaveXML()
{
	TiXmlElement* hst = new TiXmlElement(this->GetName());
	hst->SetDoubleAttribute("min", this->m_min);
	hst->SetDoubleAttribute("max", this->m_max);
	hst->SetAttribute("num", this->m_num_bins);
	TLFDouble value(0.);
	for (int i = 0; i < this->m_num_bins; i++)
	{
		value.Set(m_data[i]);
		TiXmlElement* v =  value.SaveXML();
		hst->LinkEndChild(v);
	}
	return hst;
}
