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
//		File: LFFeatures.cpp
//		Purpose: implements features
//
//      CopyRight 2004-2019 (c) NN-Videolab.net
//M*/
#include "LFFeatures.h"

/*
	A-Feature 
*/

TLFAFeature::TLFAFeature() : ILFFeature()
{

}

TLFAFeature::TLFAFeature(ILFFeature* feature) : ILFFeature(feature)
{

}
TLFAFeature::TLFAFeature(int sxbase, int sybase, int wbase, int hbase) : ILFFeature(sxbase, sybase, wbase, hbase)
{

}

unsigned int      TLFAFeature::uCalcValue(TLFImage* pImage, const TLFAlignedTransform& transform) const
{
	return  (unsigned int)this->fCalcValue(pImage, transform)/2;
}

double            TLFAFeature::fCalcValue(TLFImage* pImage, const TLFAlignedTransform& transform) const
{
	if (pImage == NULL)
		return 0;

	TLFRect rect = transform.Apply(m_base);

	int w = rect.Width();
	int h = rect.Height();

	double s = 1. / (double)(w * h);
	double v = pImage->CalcLnSum(rect.Left(), rect.Top(), w, h);

	return v * s;
}

/*
	S - feature
*/

TLFSFeature::TLFSFeature() : ILFFeature()
{
}
TLFSFeature::TLFSFeature(ILFFeature* feature) : ILFFeature(feature)
{
}
TLFSFeature::TLFSFeature(int sxbase, int sybase, int wbase, int hbase) : ILFFeature(sxbase, sybase, wbase, hbase)
{

}


unsigned int      TLFSFeature::uCalcValue(TLFImage* pImage, const TLFAlignedTransform& transform) const
{
	return (unsigned int)fCalcValue(pImage, transform);
}
double            TLFSFeature::fCalcValue(TLFImage* pImage, const TLFAlignedTransform& transform) const {
	if (pImage == NULL)
		return 0;
	TLFRect rect = transform.Apply(m_base);

	int w = rect.Width();
	int h = rect.Height();
	int x = rect.Left();
	int y = rect.Top();
	
	double v1, v2, s, v;
	s = w * h;
	if (s == 0)
		return 0;
	

	v1 = pImage->CalcLnSum(x, y, w, h);
	v2 = pImage->CalcSqSum(x, y, w, h);
	v1 /= s;
	v2 /= s;
	v = v2 - v1 * v1;
	return v >= 0 ? sqrt(v) : 0.0;
}

TLFSAFeature::TLFSAFeature() : ILFFeature()
{}
TLFSAFeature::TLFSAFeature(int sxbase, int sybase, int wbase, int hbase) : ILFFeature(sxbase, sybase, wbase, hbase)
{}
TLFSAFeature::TLFSAFeature(ILFFeature* feature) : ILFFeature(feature)
{}

unsigned int      TLFSAFeature::uCalcValue(TLFImage* pImage, const TLFAlignedTransform& transform) const
{
	if (pImage == NULL)
		return 0;
	TLFRect rect = transform.Apply(m_base);

	int w = rect.Width();
	int h = rect.Height();
	int x = rect.Left();
	int y = rect.Top();

	double v1, v2, s, v;
	s = w * h;
	if (s == 0)
		return 0;


	v1 = pImage->CalcLnSum(x, y, w, h);
	v2 = pImage->CalcSqSum(x, y, w, h);
	v1 /= s;
	v2 /= s;
	v = v2 - v1 * v1;
	return v >= 0 ? (unsigned int)(floor(16 * floor((floor(sqrt(v) / 16. + 0.5))) + v1 / 16. + 0.5)) : 0;
}

double            TLFSAFeature::fCalcValue(TLFImage* pImage, const TLFAlignedTransform& transform) const
{
	double v = (double)uCalcValue(pImage, transform);
	return v;
}


/*
	H - feature 
*/

TLFHFeature::TLFHFeature() : ILFFeature()
{

}
TLFHFeature::TLFHFeature(ILFFeature* feature) : ILFFeature(feature)
{

}
TLFHFeature::TLFHFeature(int sxbase, int sybase, int wbase, int hbase) : ILFFeature(sxbase, sybase, wbase, hbase)
{

}

unsigned int      TLFHFeature::uCalcValue(TLFImage* pImage, const TLFAlignedTransform& transform) const
{
	double v = fCalcValue(pImage, transform);
	return (unsigned int)((v + 2)*32);
}

double            TLFHFeature::fCalcValue(TLFImage* pImage, const TLFAlignedTransform& transform) const
{
	if (pImage == NULL)
		return 0;
	TLFRect rect = transform.Apply(m_base);

	int w = rect.Width();
	int h = rect.Height();
	int x = rect.Left();
	int y = rect.Top();

	double s, v1, v2, sigma, value;
	s = 2 * w * h;
	value = pImage->CalcLnSum(x, y, w, 2 * h);
	sigma = pImage->CalcSqSum(x, y, w, 2 * h);
	value /= s;
	sigma /= s;
	sigma = sigma - value * value;

	if (sigma == 0)
		return 0;
	s /= 2;
	v1 = pImage->CalcLnSum(x, y, w, h) / s;
	v2 = pImage->CalcLnSum(x, y + h, w, h) / s;
	return (v1 - v2) / sqrt(sigma);
}

/*
	V - feature
*/

TLFVFeature::TLFVFeature() : ILFFeature()
{

}
TLFVFeature::TLFVFeature(ILFFeature* feature) : ILFFeature(feature)
{

}
TLFVFeature::TLFVFeature(int sxbase, int sybase, int wbase, int hbase) : ILFFeature(sxbase, sybase, wbase, hbase)
{

}

unsigned int      TLFVFeature::uCalcValue(TLFImage* pImage, const TLFAlignedTransform& transform) const
{
	double v = fCalcValue(pImage, transform);
	return (unsigned int)((v + 2)*32);
}
double            TLFVFeature::fCalcValue(TLFImage* pImage, const TLFAlignedTransform& transform) const
{
	if (pImage == NULL)
		return 0;
	TLFRect rect = transform.Apply(m_base);

	int w = rect.Width();
	int h = rect.Height();
	int x = rect.Left();
	int y = rect.Top();

	double s, v1, v2, sigma, value;
	s = 2 * w * h;
	value = pImage->CalcLnSum(x, y, 2 * w, h);
	sigma = pImage->CalcSqSum(x, y, 2 * w, h);
	value /= s;
	sigma /= s;
	sigma = sigma - value * value;

	if (sigma == 0)
		return 0;
	
	s /= 2;
	v1 = pImage->CalcLnSum(x, y, w, h) / s;
	v2 = pImage->CalcLnSum(x + w, y, w, h) / s;
	return (v1 - v2) / sqrt(sigma);
}

/*
	D - feature
*/

TLFDFeature::TLFDFeature() : ILFFeature()
{
}
TLFDFeature::TLFDFeature(ILFFeature* feature) : ILFFeature(feature)
{
}
TLFDFeature::TLFDFeature(int sxbase, int sybase, int wbase, int hbase) : ILFFeature(sxbase, sybase, wbase, hbase)
{

}

unsigned int      TLFDFeature::uCalcValue(TLFImage* pImage, const TLFAlignedTransform& transform) const
{
	double v = fCalcValue(pImage, transform);
	return (unsigned int)((v + 2)*32);
}
double            TLFDFeature::fCalcValue(TLFImage* pImage, const TLFAlignedTransform& transform) const
{
	if (pImage == NULL)
		return 0;

	TLFRect rect = transform.Apply(m_base);

	int w = rect.Width();
	int h = rect.Height();
	int x = rect.Left();
	int y = rect.Top();

	double s, v1, v2, v3, v4, sigma, value;
	s = 4 * w * h;
	value = pImage->CalcLnSum(x, y, 2 * w, 2 * h);
	sigma = pImage->CalcSqSum(x, y, 2 * w, 2 * h);
	value /= s;
	sigma /= s;
	sigma = sigma - value * value;

	if (sigma == 0)
		return 0;

	v1 = pImage->CalcLnSum(x, y, w, h) / s;
	v2 = pImage->CalcLnSum(x + w, y, w, h) / s;
	v3 = pImage->CalcLnSum(x, y + h, w, h) / s;
	v4 = pImage->CalcLnSum(x + w, y + h, w, h) / s;
	return (v1 + v4 - v2 - v3) / sqrt(sigma);
}


/*
	C - feature
*/

TLFCFeature::TLFCFeature() : ILFFeature()
{

}
TLFCFeature::TLFCFeature(ILFFeature* feature) : ILFFeature(feature)
{
}
TLFCFeature::TLFCFeature(int sxbase, int sybase, int wbase, int hbase) : ILFFeature(sxbase, sybase, wbase,hbase)
{

}

unsigned int      TLFCFeature::uCalcValue(TLFImage* pImage, const TLFAlignedTransform& transform) const
{
	double v = fCalcValue(pImage, transform);
	return (unsigned int)((v + 30)*2);
}
double            TLFCFeature::fCalcValue(TLFImage* pImage, const TLFAlignedTransform& transform) const
{
	if (pImage == NULL)
		return 0;

	TLFRect rect = transform.Apply(m_base);

	int w = rect.Width();
	int h = rect.Height();
	int x = rect.Left();
	int y = rect.Top();

	double s, v1, v2, v3, v4, v5, v6, v7, v8, v9, value, sigma;
	s = w * h;
	//int ww = w /3;
	//int hh = h/3;

	value = pImage->CalcLnSum(x, y, 3 * w, 3 * h);
	sigma = pImage->CalcSqSum(x, y, 3 * w, 3 * h);
	value /= s;
	sigma /= s;
	sigma = sigma - value * value;


	if (sigma == 0)
		return 0;

	s *= sqrt(sigma);
	s /= 9;
	v1 = pImage->CalcLnSum(x, y, w, h) / s;
	v2 = pImage->CalcLnSum(x + w, y, w, h) / s;
	v3 = pImage->CalcLnSum(x + 2 * w, y, w, h) / s;

	v4 = pImage->CalcLnSum(x, y + h, w, h) / s;
	v5 = pImage->CalcLnSum(x + w, y + h, w, h) / s;
	v6 = pImage->CalcLnSum(x + 2 * w, y + h, w, h) / s;

	v7 = pImage->CalcLnSum(x, y + 2 * h, w, h) / s;
	v8 = pImage->CalcLnSum(x + w, y + 2 * h, w, h) / s;
	v9 = pImage->CalcLnSum(x + 2 * w, y + 2 * h, w, h) / s;
	return  (8 * v5 - v1 - v2 - v3 - v4 - v6 - v7 - v8 - v9);

}


/*
	LBP - feature
*/
TLFLBPFeature::TLFLBPFeature() : ILFFeature()
{

}
TLFLBPFeature::TLFLBPFeature(ILFFeature* feature) : ILFFeature(feature)
{

}
TLFLBPFeature::TLFLBPFeature(int sxbase, int sybase, int wbase, int hbase) : ILFFeature(sxbase, sybase, wbase, hbase)
{

}


int  TLFLBPFeature::CalcValue(awpImage* pImage, double avg, const TLFAlignedTransform& transform) const {


	TLFRect transformed = transform.Apply(m_base);

	int w = transformed.Width();
	int h = transformed.Height();
	auto trans_area = w * h;

	if (trans_area == 0 || pImage == NULL)
		return 0;

	double tarea = 9.0 * trans_area;
	double area = 1.0 / (double)(trans_area);

	double v[9];
	memset(v, 0, sizeof(v));
	int    iv[9];
	memset(iv, 0, sizeof(iv));

	double* pix = (double*)pImage->pPixels;
	
	int x = transformed.Left();
	int y = transformed.Top();

	v[0] = CalcSum(pix, x, y, w, h, pImage->sSizeX) * area;
	v[1] = CalcSum(pix, x + w, y, w, h, pImage->sSizeX) * area;
	v[2] = CalcSum(pix, x + 2 * w, y, w, h, pImage->sSizeX) * area;
	v[3] = CalcSum(pix, x, y + h, w, h, pImage->sSizeX) * area;
	
	v[4] = CalcSum(pix, x + 2 * w, y + h, w, h, pImage->sSizeX) * area;
	v[5] = CalcSum(pix, x, y + 2 * h, w, h, pImage->sSizeX) * area;
	v[6] = CalcSum(pix, x + w, y + 2 * h, w, h, pImage->sSizeX) * area;
	v[7] = CalcSum(pix, x + 2 * w, y + 2 * h, w, h, pImage->sSizeX) * area;

	v[8] = CalcSum(pix, x + w, y + h, w, h, pImage->sSizeX) * area;

	double total = avg > 0 ? avg : v[8];

	for ( int i = 0; i < 8; i++)
		if (v[i] > total)
			iv[i] = 1;

	int idx = 0;
	idx |= iv[0];
	idx = idx << 1;
	idx |= iv[1];
	idx = idx << 1;
	idx |= iv[2];
	idx = idx << 1;
	idx |= iv[3];
	idx = idx << 1;
	idx |= iv[4];
	idx = idx << 1;
	idx |= iv[5];
	idx = idx << 1;
	idx |= iv[6];
	idx = idx << 1;
	idx |= iv[7];
	
	return idx;
}

unsigned int     TLFLBPFeature::uCalcValue(TLFImage* pImage, const TLFAlignedTransform& transform) const
{
	return this->CalcValue(pImage->GetIntegralImage(), 0, transform);
}
double            TLFLBPFeature::fCalcValue(TLFImage* pImage, const TLFAlignedTransform& transform) const
{
	return (double)this->CalcValue(pImage->GetIntegralImage(), 0, transform);
}

TLFColorSensor9Bit::TLFColorSensor9Bit() : ILFFeature()
{

}

TLFColorSensor9Bit::TLFColorSensor9Bit(TLFColorSensor9Bit* sensor) : ILFFeature(sensor)
{

}
TLFColorSensor9Bit::TLFColorSensor9Bit(AWPWORD sx, AWPWORD sy, AWPWORD xbase, AWPWORD ybase) : ILFFeature(sx, sy, xbase, ybase)
{

}
/*
calc features value
*/


unsigned int      TLFColorSensor9Bit::uCalcValue(TLFImage* pImage, const TLFAlignedTransform& transform) const {
	if (pImage == NULL || pImage->GetBlueIntegral() == NULL ||
		pImage->GetGreenIntegral() == NULL ||
		pImage->GetRedIntegral() == NULL)
		return 0;

	TLFRect rect = transform.Apply(m_base);

	int w = rect.Width();
	int h = rect.Height();
	int x = rect.Left();
	int y = rect.Top();

	unsigned int code = 0;
	double s = 1 / (double)(w * h);
	double coef = 1 / 32.;

	double rvalue = pImage->CalcLnSum(x, y, w, h) * s;
	double gvalue = pImage->CalcLnSum(x, y, w, h) * s;
	double bvalue = pImage->CalcLnSum(x, y, w, h) * s;

	unsigned int sr = (unsigned int)(rvalue * coef);
	unsigned int sg = (unsigned int)(gvalue * coef);
	unsigned int sb = (unsigned int)(bvalue * coef);
	unsigned int result = 0;

	result |= sr;
	result = result << 3;
	result |= sg;
	result = result << 3;
	result |= sb;
	return result;
}

double            TLFColorSensor9Bit::fCalcValue(TLFImage* pImage, const TLFAlignedTransform& transform) const {
	return uCalcValue(pImage, transform);
}



/*
	H line - feature
*/
TLFLHFeature::TLFLHFeature() : ILFFeature()
{

}
TLFLHFeature::TLFLHFeature(ILFFeature* feature) : ILFFeature(feature)
{

}
TLFLHFeature::TLFLHFeature(int sxbase, int sybase, int wbase, int hbase) : ILFFeature(sxbase, sybase, wbase, hbase)
{

}

unsigned int      TLFLHFeature::uCalcValue(TLFImage* pImage, const TLFAlignedTransform& transform) const {
	double v = fCalcValue(pImage, transform);
	return (unsigned int)((v + 4.2426406871192848) * 60.33977866125207);
}
double            TLFLHFeature::fCalcValue(TLFImage* pImage, const TLFAlignedTransform& transform) const {
	if (pImage == NULL)
		return 0;

	TLFRect rect = transform.Apply(m_base);

	int w = rect.Width();
	int h = rect.Height();
	int x = rect.Left();
	int y = rect.Top();
	

	double s, v1, v2, v3, sigma, value;
	s = 3 * w * h;
	value = pImage->CalcLnSum(x, y, w, 3*h);
	sigma = pImage->CalcSqSum(x, y, w, 3*h);
	value /= s;
	sigma /= s;
	sigma = sigma - value * value;

	if (sigma == 0)
		return 0;

	s /= 3;//?!
	v1 = pImage->CalcLnSum(x, y, w, h) / s;
	v2 = pImage->CalcLnSum(x, y + h, w, h) / s;
	v3 = pImage->CalcLnSum(x, y + 2 * h, w, h) / s;
	return (v1 - 2 * v2 + v3) / sqrt(sigma);
}



/*
	V line - feature
*/
TLFLVFeature::TLFLVFeature() : ILFFeature()
{

}
TLFLVFeature::TLFLVFeature(ILFFeature* feature) : ILFFeature(feature)
{

}
TLFLVFeature::TLFLVFeature(int sxbase, int sybase, int wbase, int hbase) : ILFFeature(sxbase, sybase, wbase, hbase)
{

}

unsigned int      TLFLVFeature::uCalcValue(TLFImage* pImage, const TLFAlignedTransform& transform) const {
	double v = fCalcValue(pImage, transform);
	return (unsigned int)((v + 4.2426406871192848) * 60.33977866125207);
}
double            TLFLVFeature::fCalcValue(TLFImage* pImage, const TLFAlignedTransform& transform) const {
	if (pImage == NULL)
		return 0;

	TLFRect rect = transform.Apply(m_base);

	int w = rect.Width();
	int h = rect.Height();
	int x = rect.Left();
	int y = rect.Top();
	
	double s, v1, v2, v3, sigma, value;
	s = 3 * w * h;
	value = pImage->CalcLnSum(x, y, 3 * w, h);
	sigma = pImage->CalcSqSum(x, y, 3 * w, h);
	value /= s;
	sigma /= s;
	sigma = sigma - value * value;

	if (sigma == 0)
		return 0;
	
	s /= 3;//?!
	v1 = pImage->CalcLnSum(x, y, w, h) / s;
	v2 = pImage->CalcLnSum(x + w, y, w, h) / s;
	v3 = pImage->CalcLnSum(x + 2 * w, y, w, h) / s;
	return (v1 - 2 * v2 + v3) / sqrt(sigma);
}



//---------------------------------------------------------------------------
//
TCSSensor::TCSSensor() {}

TCSSensor::TCSSensor(int sx, int sy, int uw, int uh) : ILFFeature(sx, sy, uw, uh) {}

TCSSensor::TCSSensor(TCSSensor* sensor) : ILFFeature(sensor) {}


// вычисление значения признака.
// возвращает значение [0..511]

int  TCSSensor::CalcValue(awpImage* pImage, double avg, const TLFAlignedTransform& transform) const {


	TLFRect transformed = transform.Apply(m_base);

	int w = transformed.Width();
	int h = transformed.Height();
	auto trans_area = w*h;

	if (trans_area == 0 || pImage == NULL)
		return 0;

   double tarea = 9.0 * trans_area;
   double area =  1.0/(double)(trans_area);

   double v[9];
   memset(v, 0, sizeof(v));
   int    iv[9];
   memset(iv, 0, sizeof(iv));

   double* pix = (double*)pImage->pPixels;
   double total = avg;

   int x = transformed.Left();
   int y = transformed.Top();
   

   total = avg > 0 ? avg : CalcSum(pix, x, y, 3 * w, 3 * h, pImage->sSizeX) / tarea;
   
   int i = 0;

   v[0] = CalcSum(pix, x, y, w, h, pImage->sSizeX) * area;
   v[1] = CalcSum(pix, x + w, y, w, h, pImage->sSizeX) * area;
   v[2] = CalcSum(pix, x + 2* w, y, w, h, pImage->sSizeX) * area;
   v[3] = CalcSum(pix, x, y + h, w, h, pImage->sSizeX) * area;
   v[4] = CalcSum(pix, x + w, y + h, w, h, pImage->sSizeX) * area;
   v[5] = CalcSum(pix, x + 2* w, y + h, w, h, pImage->sSizeX) * area;
   v[6] = CalcSum(pix, x, y + 2* h, w, h, pImage->sSizeX) * area;
   v[7] = CalcSum(pix, x + w, y + 2* h, w, h, pImage->sSizeX) * area;
   v[8] = CalcSum(pix, x + 2* w, y + 2* h, w, h, pImage->sSizeX) * area;

   for (i = 0; i < 9; i++)
	  if (v[i] > total)
		iv[i] = 1;

	int idx = 0;
	idx |= iv[0];
	idx  =idx << 1;
	idx |= iv[1];
	idx  =idx << 1;
	idx |= iv[2];
	idx  =idx << 1;
	idx |= iv[3];
	idx  =idx << 1;
	idx |= iv[4];
	idx  =idx << 1;
	idx |= iv[5];
	idx  =idx << 1;
	idx |= iv[6];
	idx  =idx << 1;
	idx |= iv[7];
	idx  =idx << 1;
	idx |= iv[8];
	
   return idx;
}

unsigned int     TCSSensor::uCalcValue(TLFImage* pImage, const TLFAlignedTransform& transform) const
{
	return this->CalcValue(pImage->GetIntegralImage(), 0, transform);
}
double            TCSSensor::fCalcValue(TLFImage* pImage, const TLFAlignedTransform& transform) const
{
	return (double)this->CalcValue(pImage->GetIntegralImage(), 0, transform);
}

/*
	Features facory
*/
ILFFeature*  LFCreateFeature(ILFFeature* feature)
{
	if (feature == NULL)
		return NULL;
	const char* name = feature->GetName();

	if (strcmp(name, "TLFColorSensor9Bit") == 0)
	{
		TLFColorSensor9Bit* cw = dynamic_cast<TLFColorSensor9Bit*>(feature);
		return new TLFColorSensor9Bit(cw);
	}
	else if (strcmp(name, "CSFeature") == 0)
	{
		TCSSensor* cw = dynamic_cast<TCSSensor*>(feature);
		return new TCSSensor(cw);
	}
	else if (strcmp(name, "TLFAFeature") == 0)
	{
		TLFAFeature* cw = dynamic_cast<TLFAFeature*>(feature);
		return new TLFAFeature(cw);
	}
	else if (strcmp(name, "TLFSFeature") == 0)
	{
		TLFSFeature* cw = dynamic_cast<TLFSFeature*>(feature);
		return new TLFSFeature(cw);
	}
	else if (strcmp(name, "TLFHFeature") == 0)
	{
		TLFHFeature* cw = dynamic_cast<TLFHFeature*>(feature);
		return new TLFHFeature(cw);
	}
	else if (strcmp(name, "TLFVFeature") == 0)
	{
		TLFVFeature* cw = dynamic_cast<TLFVFeature*>(feature);
		return new TLFVFeature(cw);
	}
	else if (strcmp(name, "TLFDFeature") == 0)
	{
		TLFDFeature* cw = dynamic_cast<TLFDFeature*>(feature);
		return new TLFDFeature(cw);
	}
	else if (strcmp(name, "TLFCFeature") == 0)
	{
		TLFCFeature* cw = dynamic_cast<TLFCFeature*>(feature);
		return new TLFCFeature(cw);
	}
	else if (strcmp(name, "TLFLBPFeature") == 0)
	{
		TLFLBPFeature* cw = dynamic_cast<TLFLBPFeature*>(feature);
		return new TLFLBPFeature(cw);
	}
	else
		return NULL;
}

ILFFeature* LFCreateFeature(const char* lpName, int sx, int sy, int w, int h)
{
	if (strcmp(lpName, "TLFColorSensor9Bit") == 0)
	{
		return new TLFColorSensor9Bit(sx,sy,w,h);
	}
	else if (strcmp(lpName, "CSFeature") == 0)
	{
		return new TCSSensor(sx,sy,w,h);
	}
	else if (strcmp(lpName, "TLFAFeature") == 0)
	{
		return new TLFAFeature(sx, sy, w, h);
	}
	else if (strcmp(lpName, "TLFSFeature") == 0)
	{
		return new TLFSFeature(sx, sy, w, h);
	}
	else if (strcmp(lpName, "TLFHFeature") == 0)
	{
		return new TLFHFeature(sx, sy, w, h);
	}
	else if (strcmp(lpName, "TLFVFeature") == 0)
	{
		return new TLFVFeature(sx, sy, w, h);
	}
	else if (strcmp(lpName, "TLFDFeature") == 0)
	{
		return new TLFDFeature(sx, sy, w, h);
	}
	else if (strcmp(lpName, "TLFCFeature") == 0)
	{
		return new TLFCFeature(sx, sy, w, h);
	}
	else if (strcmp(lpName, "TLFLBPFeature") == 0)
	{
		return new TLFLBPFeature(sx, sy, w, h);
	}
	else
		return NULL;
}

awpRect		  LFRectToFeatureBlock(const char* lpName, awpRect& rect)
{
	awpRect result;
	result.left = rect.left;
	result.top = rect.top;
	result.right = rect.left + 1;
	result.bottom = rect.top + 1;
	int w, h;
	w = rect.right - rect.left;
	h = rect.bottom - rect.top;
	if (strcmp(lpName, "TLFColorSensor") == 0 || strcmp(lpName, "TLFColorSensor9Bit") == 0)
	{
		result = rect;
		return result;
	}
	else if (strcmp(lpName, "CSFeature") == 0)
	{
		result.right = result.left + w / 3;
		result.bottom = result.top + h / 3;
		return result;
	}
	else if (strcmp(lpName, "TLFAvgFeature") == 0)
	{
		result = rect;
		return result;
	}
	else if (strcmp(lpName, "TLFSigmaFeature") == 0)
	{
		result = rect;
		return result;
	}
	else if (strcmp(lpName, "TLFAFeature") == 0)
	{
		result = rect;
		return result;
	}
	else if (strcmp(lpName, "TLFSFeature") == 0)
	{
		result = rect;
		return result;
	}
	else if (strcmp(lpName, "TLFHFeature") == 0)
	{
		result.right = result.left + w;
		result.bottom = result.top + h / 2;
		return result;
	}
	else if (strcmp(lpName, "TLFHAFeature") == 0)
	{
		result.right = result.left + w;
		result.bottom = result.top + h / 2;
		return result;
	}
	else if (strcmp(lpName, "TLFVFeature") == 0)
	{
		result.right = result.left + w /2;
		result.bottom = result.top + h;
		return result;
	}
	else if (strcmp(lpName, "TLFVAFeature") == 0)
	{
		result.right = result.left + w / 2;
		result.bottom = result.top + h;
		return result;
	}
	else if (strcmp(lpName, "TLFDFeature") == 0)
	{
		result.right = result.left + w / 2;
		result.bottom = result.top + h / 2;
		return result;
	}
	else if (strcmp(lpName, "TLFDAFeature") == 0)
	{
		result.right = result.left + w / 2;
		result.bottom = result.top + h / 2;
		return result;
	}
	else if (strcmp(lpName, "TLFCFeature") == 0)
	{
		result.right = result.left + w / 3;
		result.bottom = result.top + h / 3;
		return result;
	}
	else if (strcmp(lpName, "TLFCAFeature") == 0)
	{
		result.right = result.left + w / 3;
		result.bottom = result.top + h / 3;
		return result;
	}
	else if (strcmp(lpName, "TLFLBPFeature") == 0)
	{
		result.right = result.left + w / 3;
		result.bottom = result.top + h / 3;
		return result;
	}
	else
		return result;
}

TLFFeatureList::TLFFeatureList()
{
	int sx = 0;
	int sy = 0;
	int w = 8;
	int h = 8;
	ILFFeature* f = NULL;
	f = LFCreateFeature("TLFColorSensor9Bit", sx, sy, w, h);
	Add(f);
	f = LFCreateFeature("CSFeature", sx, sy, w, h);
	Add(f);
	f = LFCreateFeature("TLFAFeature", sx, sy, w, h);
	Add(f);
	f = LFCreateFeature("TLFSFeature", sx, sy, w, h);
	Add(f);
	f = LFCreateFeature("TLFHFeature", sx, sy, w, h);
	Add(f);
	f = LFCreateFeature("TLFVFeature", sx, sy, w, h);
	Add(f);
	f = LFCreateFeature("TLFDFeature", sx, sy, w, h);
	Add(f);
	f = LFCreateFeature("TLFCFeature", sx, sy, w, h);
	Add(f);
	f = LFCreateFeature("TLFLBPFeature", sx, sy, w, h);
	Add(f);

}
const char* TLFFeatureList::FeatureName(int index)
{
	if (index >= 0 && index < this->GetCount())
		return Get(index)->GetName();
	else
		return "";
}

ILFFeature*   LFCreateFeature(const char* lpName, TiXmlElement* parent)
{
    ILFFeature* result = LFCreateFeature(lpName, 0,0,6,6);
    if (result == NULL)
        return result;
    if (!result->LoadXML(parent))
    {
        delete result;
        return NULL;
    }
    return result;
}

ILFFeature*   LFCreateFeature(TiXmlElement* parent)
{
    if (parent == NULL)
        return NULL;
    return LFCreateFeature(parent->Value(), parent);
}


