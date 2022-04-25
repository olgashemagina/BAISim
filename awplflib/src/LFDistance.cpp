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
//		File: LFDistance.cpp
//		Purpose: implements TLFL2Distance, TLFL2NormDistance, TLFIEFSDistance
//
//      CopyRight 2004-2018 (c) NN-Videolab.net
//M*/
#include "_LF.h"
double TLFL2Distance::ro(TLFDblVector* v1, TLFDblVector* v2)
{
	if (v1 == NULL || v2 == NULL)
		return -1;
	if (v1->GetCount() != v2->GetCount())
		return -1;
	double d = 0;
	for (int i = 0; i < v1->GetCount(); i++)
	{
		d += (v1->GetValue(i) - v2->GetValue(i))*(v1->GetValue(i) - v2->GetValue(i));
	}
	return sqrt(d);
}

double TLFL2NormDistance::ro(TLFDblVector* v1, TLFDblVector* v2)
{
	if (v1 == NULL || v2 == NULL)
		return -1;
	if (v1->GetCount() != v2->GetCount())
		return -1;

	double norm1 = v1->NormL2();
	double norm2 = v2->NormL2();
	if (norm1 == 0 || norm2 == 0)
		return -1;
	double d = 0;
	for (int i = 0; i < v1->GetCount(); i++)
	{
		double d1 = v1->GetValue(i) / norm1;
		double d2 = v2->GetValue(i) / norm2;
		d += (d1 - d2)*(d1 - d2);
	}
	return sqrt(d);
}
// forward declaration
class TIEFSStrong;
class TIEFSSample;
TLFIEFSDistance::TLFIEFSDistance(TLFObject* strong)
{
	this->m_strong = NULL;
	if (strcmp(strong->GetName(), "TIEFSStrong") != 0)
		return;
	m_strong = strong;
}
double TLFIEFSDistance::ro(TLFDblVector* v1, TLFDblVector* v2)
{
	if (m_strong == NULL)
		return -1;
	if (v1 == NULL || v2 == NULL)
		return -1;
	if (v1->GetCount() != v2->GetCount())
		return -1;
	double* diff = NULL;
	diff = new double[v1->GetCount()];

	for (int i = 0; i < v1->GetCount(); i++)
	{
		diff[i] = v1->GetValue(i) - v2->GetValue(i);
	}
	TIEFSSample* sample = new TIEFSSample(diff, v1->GetCount(), 0);
	delete diff;
	double _ro = ((TIEFSStrong*)m_strong)->Raiting(sample);
	delete sample;
	return _ro;
}
