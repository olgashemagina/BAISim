
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
//		File: LFBuffer.cpp
//		Purpose: Implements TLFBuffer, TLFRingBuffer
//
//      CopyRight 2004-2018 (c) NN-Videolab.net
//M*/
#include "_LF.h"
/////////// buffer /////////////////////////////////
TLFBuffer::TLFBuffer(unsigned int size, double value)
{
	if (size == 0)
		size = 1;
	m_size = size;
	m_data = new double[m_size];
	m_sum = 0;
	for (int i = 0; i < m_size; i++)
	{
		m_data[i] = value;
		m_sum += value;
	}

}
TLFBuffer::~TLFBuffer()
{
	delete[] m_data;
}
void TLFBuffer::Push(double value)
{
	m_sum -= m_data[0];
	m_sum += value;
	double* p1 = m_data + 1;
	memcpy(m_data, p1, (m_size - 1)*sizeof(double));
	m_data[m_size - 1] = value;
}
void  TLFBuffer::SetData(double* data)
{
	m_sum = 0;
	for (int i = 0; i < m_size; i++)
	{
		m_data[i] = data[i];
		m_sum += data[i];
	}
}

double* TLFBuffer::GetData()
{
	return m_data;
}


double TLFBuffer::GetSum()
{
	return m_sum;
}
double TLFBuffer::GetAvg()
{
	return m_sum / (double)m_size;
}
double TLFBuffer::GetValue(int index)
{
	if (index >= m_size)
		return 0;
	return m_data[index];
}
int    TLFBuffer::GetSize()
{
	return this->m_size;
}

void   TLFBuffer::Clear()
{
	m_sum = 0;
	memset(m_data, 0, m_size*sizeof(double));
}
//////////////////// ring buffer ////////////////////////////
TLFRingBuffer::TLFRingBuffer(unsigned int size, double value)
{
	if (size == 0)
		size = 1;
	m_size = size;
	m_data = new double[m_size];
	m_sum = 0;
	for (int i = 0; i < m_size; i++)
	{
		m_data[i] = value;
		m_sum += value;
	}
	m_current = 0;
}

TLFRingBuffer::~TLFRingBuffer()
{
	delete[] m_data;
}
void TLFRingBuffer::Push(double value)
{
	m_sum -= m_data[m_current];
	m_sum += value;
	m_data[m_current] = value;
	m_current++;
	if (m_current >= this->m_size)
		m_current = 0;
}
double TLFRingBuffer::GetSum()
{
	return m_sum;
}
double TLFRingBuffer::GetAvg()
{
	return m_sum / (double)m_size;
}

double TLFRingBuffer::GetSigma()
{
	double s = GetDisp();
	return sqrt(s);
}
double TLFRingBuffer::GetDisp()
{
	double result = 0;
	for (int i = 0; i < m_size; i++)
		result += (m_data[i] * m_data[i]);
	result /= m_size;
	double a = GetAvg();
	return result - a*a;
}


double TLFRingBuffer::GetValue(int index)
{
	if (index >= m_size)
		return 0;
	return m_data[index];
}
int    TLFRingBuffer::GetSize()
{
	return this->m_size;
}
void   TLFRingBuffer::Clear()
{
	m_sum = 0;
	memset(m_data, 0, m_size*sizeof(double));
}

