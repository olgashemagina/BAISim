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
#include "_LF.h"

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

unsigned int      TLFAFeature::uCalcValue(TLFImage* pImage)
{
	m_uValue =  (unsigned int)this->fCalcValue(pImage)/2;
	return m_uValue;
}

double            TLFAFeature::fCalcValue(TLFImage* pImage)
{
	if (pImage == NULL)
		return 0;
	else
	{
		double s = 1./(double)m_w*m_h;
		double v = pImage->CalcLnSum(this->m_sx, this->m_sy, this->m_w, this->m_h);
		m_fValue = v*s;
		return m_fValue;

    }
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


unsigned int      TLFSFeature::uCalcValue(TLFImage* pImage)
{

	m_uValue = (unsigned int)fCalcValue(pImage);
	return   m_uValue;
}
double            TLFSFeature::fCalcValue(TLFImage* pImage)
{
	if (pImage == NULL)
	{
		m_fValue = 0;
		return m_fValue;
	}
	else
	{
		double v1, v2, s, v;
		s = m_w*m_h;
		if (s == 0)
		{
			m_fValue = 0;
			return m_fValue;
		}

		v1 = pImage->CalcLnSum(this->m_sx, this->m_sy, this->m_w, this->m_h);
		v2 = pImage->CalcSqSum(this->m_sx, this->m_sy, this->m_w, this->m_h);
		v1 /= s;
		v2 /= s;
		v = v2 - v1*v1;
		if (v >= 0)
		{
			m_fValue = sqrt(v);
			return m_fValue;
		}
		else
		{
			m_fValue = 0;
			return m_fValue;
		}
	}
}

TLFSAFeature::TLFSAFeature() : ILFFeature()
{}
TLFSAFeature::TLFSAFeature(int sxbase, int sybase, int wbase, int hbase) : ILFFeature(sxbase, sybase, wbase, hbase)
{}
TLFSAFeature::TLFSAFeature(ILFFeature* feature) : ILFFeature(feature)
{}

unsigned int      TLFSAFeature::uCalcValue(TLFImage* pImage)
{
	if (pImage == NULL)
	{
		m_fValue = 0;
		return m_fValue;
	}
	else
	{
		double v1, v2, s, v;
		s = m_w*m_h;
		if (s == 0)
		{
			m_fValue = 0;
			return m_fValue;
		}

		v1 = pImage->CalcLnSum(this->m_sx, this->m_sy, this->m_w, this->m_h);
		v2 = pImage->CalcSqSum(this->m_sx, this->m_sy, this->m_w, this->m_h);
		v1 /= s;
		v2 /= s;
		v = v2 - v1*v1;
		if (v >= 0)
		{
			m_fValue = floor(16*floor((floor(sqrt(v) / 16. +0.5))) + v1 / 16. +0.5);

			return (unsigned int)m_fValue;
		}
		else
		{
			m_fValue = 0;
			return m_fValue;
		}
	}
}

double            TLFSAFeature::fCalcValue(TLFImage* pImage)
{
	double v = (double)uCalcValue(pImage);
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

unsigned int      TLFHFeature::uCalcValue(TLFImage* pImage)
{
	double v = fCalcValue(pImage);
	m_uValue = (unsigned int)((v + 2)*32);
	return   m_uValue;
}
double            TLFHFeature::fCalcValue(TLFImage* pImage)
{
	if (pImage == NULL)
	{
		m_fValue = 0;
		return m_fValue;
	}
	else
	{
		double s, v1, v2, sigma, value;
		s = 2*m_w*m_h;
		value = pImage->CalcLnSum(this->m_sx, this->m_sy, this->m_w, 2*this->m_h);
		sigma = pImage->CalcSqSum(this->m_sx, this->m_sy, this->m_w, 2*this->m_h);
		value /=s;
		sigma /= s;
		sigma =  sigma - value*value;

		if (sigma == 0)
			return 0;
		s /= 2;
		v1 = pImage->CalcLnSum(m_sx, m_sy, m_w, m_h) / s;
		v2 = pImage->CalcLnSum(m_sx, m_sy + m_h, m_w, m_h) / s;
		m_fValue= (v1 - v2) / sqrt(sigma);
		return m_fValue;
	}
}

awpRect TLFHFeature::GetRect()
{
	awpRect result;
	result.left = this->m_sxBase;
	result.top = this->m_syBase;
	result.right = result.left + this->m_wBase;
	result.bottom = result.top + 2 * this->m_hBase;
	return result;
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

unsigned int      TLFVFeature::uCalcValue(TLFImage* pImage)
{
	double v = fCalcValue(pImage);
	m_uValue = (unsigned int)((v + 2)*32);
	return   m_uValue;
}
double            TLFVFeature::fCalcValue(TLFImage* pImage)
{
	if (pImage == NULL)
	{
		m_fValue = 0;
		return m_fValue;
	}
	else
	{
		double s, v1, v2, sigma, value;
		s = 2*m_w*m_h;
		value = pImage->CalcLnSum(this->m_sx, this->m_sy, 2*this->m_w, this->m_h);
		sigma = pImage->CalcSqSum(this->m_sx, this->m_sy, 2*this->m_w, this->m_h);
		value /=s;
		sigma /= s;
		sigma =  sigma - value*value;

		if (sigma == 0)
		{
			m_fValue = 0;
			return m_fValue;
		}
		s /= 2;
		v1 = pImage->CalcLnSum(m_sx, m_sy, m_w, m_h )/s;
		v2 = pImage->CalcLnSum(m_sx + m_w, m_sy, m_w, m_h)/s;
		m_fValue = (v1 - v2) / sqrt(sigma);
		return   m_fValue;
	}
}
awpRect TLFVFeature::GetRect()
{
	awpRect result;
	result.left = this->m_sxBase;
	result.top = this->m_syBase;
	result.right = result.left + 2*this->m_wBase;
	result.bottom = result.top + this->m_hBase;
	return result;
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

unsigned int      TLFDFeature::uCalcValue(TLFImage* pImage)
{
	double v = fCalcValue(pImage);
	m_uValue = (unsigned int)((v + 2)*32);
	return   m_uValue;
}
double            TLFDFeature::fCalcValue(TLFImage* pImage)
{
	if (pImage == NULL)
	{
		m_fValue = 0;
		return m_fValue;
	}
	else
	{
		double s, v1, v2, v3, v4, sigma, value;
		s = 4*m_w*m_h;
		value = pImage->CalcLnSum(this->m_sx, this->m_sy, 2*this->m_w, 2*this->m_h);
		sigma = pImage->CalcSqSum(this->m_sx, this->m_sy, 2*this->m_w, 2*this->m_h);
		value /=s;
		sigma /= s;
		sigma =  sigma - value*value;

		if (sigma == 0)
		{
			m_fValue = 0;
			return m_fValue;
		}
		v1 = pImage->CalcLnSum(m_sx, m_sy, m_w, m_h) / s;
		v2 = pImage->CalcLnSum(m_sx + m_w, m_sy, m_w, m_h) / s;
		v3 = pImage->CalcLnSum(m_sx, m_sy + m_h, m_w, m_h) / s;
		v4 = pImage->CalcLnSum(m_sx + m_w, m_sy + m_h, m_w, m_h) / s;
		m_fValue = (v1 + v4 - v2 - v3) / sqrt(sigma);
		return m_fValue;
	}
}

awpRect TLFDFeature::GetRect()
{
	awpRect result;
	result.left = this->m_sxBase;
	result.top = this->m_syBase;
	result.right = result.left + 2 * this->m_wBase;
	result.bottom = result.top + 2 * this->m_hBase;
	return result;
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

unsigned int      TLFCFeature::uCalcValue(TLFImage* pImage)
{
	double v = fCalcValue(pImage);
	m_uValue = (unsigned int)((v + 30)*2);
	return m_uValue;
}
double            TLFCFeature::fCalcValue(TLFImage* pImage)
{
	if (pImage == NULL)
	{
		m_fValue = 0;
		return m_fValue;
	}
	else
	{
		double s, v1, v2, v3, v4, v5, v6, v7, v8, v9, value, sigma;
		s = m_w*m_h;
		int w = m_w /3;
		int h = m_h/3;

		value = pImage->CalcLnSum(this->m_sx, this->m_sy, this->m_w, this->m_h);
		sigma = pImage->CalcSqSum(this->m_sx, this->m_sy, this->m_w, this->m_h);
		value /= s;
		sigma /= s;
		sigma =  sigma - value*value;

		if (sigma == 0)
		{
			m_fValue = 0;
			return m_fValue;
		}
		s *=sqrt(sigma);
		s /= 9;
		v1 =  pImage->CalcLnSum(m_sx, m_sy, w, h) / s;
		v2 =  pImage->CalcLnSum(m_sx + w, m_sy, w, h) / s;
		v3 =  pImage->CalcLnSum(m_sx + 2*w, m_sy, w, h) / s;

		v4 =  pImage->CalcLnSum(m_sx, m_sy + h, w, h) / s;
		v5 =  pImage->CalcLnSum(m_sx + w, m_sy + h, w, h) / s;
		v6 =  pImage->CalcLnSum(m_sx + 2*w, m_sy + h, w, h) / s;

		v7 =  pImage->CalcLnSum(m_sx, m_sy + 2*h, w, h) / s;
		v8 =  pImage->CalcLnSum(m_sx + w, m_sy + 2*h, w, h) / s;
		v9 =  pImage->CalcLnSum(m_sx + 2*w, m_sy + 2*h, w, h) / s;
		m_fValue  =  (8*v5 - v1 - v2 - v3- v4 - v6 - v7 - v8 - v9);
		return m_fValue;
	}
}

awpRect TLFCFeature::GetRect()
{
	awpRect result;
	result.left = this->m_sxBase;
	result.top = this->m_syBase;
	result.right = result.left + 3 * this->m_wBase;
	result.bottom = result.top + 3 * this->m_hBase;
	return result;
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

int  TLFLBPFeature::CalcValue(awpImage* pImage, double avg)
{
	if (m_w*m_h == 0 || pImage == NULL)
		return 0;
	double tarea = 9.0*m_w*m_h;
	double area = 1.0 / (double)(m_w*m_h);

	double v[9];
	memset(v, 0, sizeof(v));
	int    iv[9];
	memset(iv, 0, sizeof(iv));

	double* pix = (double*)pImage->pPixels;
	int i = 0;

	v[0] = CalcSum(pix, m_sx, m_sy, m_w, m_h, pImage->sSizeX) * area;
	v[1] = CalcSum(pix, m_sx + m_w, m_sy, m_w, m_h, pImage->sSizeX) * area;
	v[2] = CalcSum(pix, m_sx + 2 * m_w, m_sy, m_w, m_h, pImage->sSizeX) * area;
	v[3] = CalcSum(pix, m_sx, m_sy + m_h, m_w, m_h, pImage->sSizeX) * area;

	v[4] = CalcSum(pix, m_sx + 2 * m_w, m_sy + m_h, m_w, m_h, pImage->sSizeX) * area;
	v[5] = CalcSum(pix, m_sx, m_sy + 2 * m_h, m_w, m_h, pImage->sSizeX) * area;
	v[6] = CalcSum(pix, m_sx + m_w, m_sy + 2 * m_h, m_w, m_h, pImage->sSizeX) * area;
	v[7] = CalcSum(pix, m_sx + 2 * m_w, m_sy + 2 * m_h, m_w, m_h, pImage->sSizeX) * area;

	v[8] = CalcSum(pix, m_sx + m_w, m_sy + m_h, m_w, m_h, pImage->sSizeX) * area;

	for (i = 0; i < 8; i++)
		if (v[i] > v[8])
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
	//idx = idx << 1;
	//idx |= iv[8];
	m_uValue = idx;
	m_fValue = idx;
	return m_uValue;
}

unsigned int      TLFLBPFeature::uCalcValue(TLFImage* pImage)
{
	return this->CalcValue(pImage->GetIntegralImage(), 0);
}
double            TLFLBPFeature::fCalcValue(TLFImage* pImage)
{
	return (double)this->uCalcValue(pImage);
}

awpRect TLFLBPFeature::GetRect()
{
	awpRect result;
	result.left = this->m_sxBase;
	result.top = this->m_syBase;
	result.right = result.left + 3 * this->m_wBase;
	result.bottom = result.top + 3 * this->m_hBase;
	return result;
}

TLFColorSensor9Bit::TLFColorSensor9Bit() : ILFFeature()
{

}

TLFColorSensor9Bit::TLFColorSensor9Bit(TLFColorSensor9Bit* sensor) : ILFFeature(sensor)
{

}
TLFColorSensor9Bit::TLFColorSensor9Bit(AWPWORD sx, AWPWORD sy, AWPWORD xbase, AWPWORD ybase) : ILFFeature(sx, sy, xbase, ybase)
{
	m_sxBase = sx;
	m_syBase = sy;
	m_wBase = xbase;
	m_hBase = ybase;
	m_w = m_wBase;
    m_h = m_hBase;
}
/*
calc features value
*/
unsigned int      TLFColorSensor9Bit::uCalcValue(TLFImage* pImage)
{
	if (pImage == NULL || pImage->GetBlueIntegral() == NULL ||
		pImage->GetGreenIntegral() == NULL ||
		pImage->GetRedIntegral() == NULL)
		return 0;

	unsigned int code = 0.;
	double s = 1/ (double)(m_w*m_h);
	double coef = 1 / 32.;

	double rvalue =pImage->CalcLnSum(this->m_sx, this->m_sy, this->m_w, this->m_h) * s;
	double gvalue = pImage->CalcLnSum(this->m_sx, this->m_sy, this->m_w, this->m_h) * s;
	double bvalue = pImage->CalcLnSum(this->m_sx, this->m_sy, this->m_w, this->m_h) * s;

	unsigned int sr = (unsigned int)(rvalue *coef );
	unsigned int sg = (unsigned int)(gvalue *coef );
	unsigned int sb = (unsigned int)(bvalue *coef );
	unsigned int result = 0;

	result |= sr;
	result = result << 3;
	result |= sg;
	result = result << 3;
	result |= sb;
	return result;
}
double            TLFColorSensor9Bit::fCalcValue(TLFImage* pImage)
{
	return (double)uCalcValue(pImage);
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

unsigned int      TLFLHFeature::uCalcValue(TLFImage* pImage)
{
	double v = fCalcValue(pImage);
	m_uValue = (unsigned int)((v + 4.2426406871192848) * 60.33977866125207);
	return   m_uValue;
}
double            TLFLHFeature::fCalcValue(TLFImage* pImage)
{
	if (pImage == NULL)
	{
		m_fValue = 0;
		return m_fValue;
	}
	else
	{
		double s, v1, v2, v3, sigma, value;
		s = 3 * m_w * m_h;
		value = pImage->CalcLnSum(this->m_sx, this->m_sy, this->m_w, 3 * this->m_h);
		sigma = pImage->CalcSqSum(this->m_sx, this->m_sy, this->m_w, 3 * this->m_h);
		value /= s;
		sigma /= s;
		sigma = sigma - value * value;

		if (sigma == 0)
			return 0;
		s /= 3;//?!
		v1 = pImage->CalcLnSum(m_sx, m_sy, m_w, m_h) / s;
		v2 = pImage->CalcLnSum(m_sx, m_sy + m_h, m_w, m_h) / s;
		v3 = pImage->CalcLnSum(m_sx, m_sy + 2 * m_h, m_w, m_h) / s;
		m_fValue = (v1 - 2*v2 + v3) / sqrt(sigma);
		return m_fValue;
	}
}

awpRect TLFLHFeature::GetRect()
{
	awpRect result;
	result.left = this->m_sxBase;
	result.top = this->m_syBase;
	result.right = result.left + this->m_wBase;
	result.bottom = result.top + 3 * this->m_hBase;
	return result;
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

unsigned int      TLFLVFeature::uCalcValue(TLFImage* pImage)
{
	double v = fCalcValue(pImage);
	m_uValue = (unsigned int)((v + 4.2426406871192848) * 60.33977866125207);
	return   m_uValue;
}
double            TLFLVFeature::fCalcValue(TLFImage* pImage)
{
	if (pImage == NULL)
	{
		m_fValue = 0;
		return m_fValue;
	}
	else
	{
		double s, v1, v2, v3, sigma, value;
		s = 3 * m_w * m_h;
		value = pImage->CalcLnSum(this->m_sx, this->m_sy, 3 * this->m_w, this->m_h);
		sigma = pImage->CalcSqSum(this->m_sx, this->m_sy, 3 * this->m_w, this->m_h);
		value /= s;
		sigma /= s;
		sigma = sigma - value * value;

		if (sigma == 0)
		{
			m_fValue = 0;
			return m_fValue;
		}
		s /= 3;//?!
		v1 = pImage->CalcLnSum(m_sx, m_sy, m_w, m_h) / s;
		v2 = pImage->CalcLnSum(m_sx + m_w, m_sy, m_w, m_h) / s;
		v3 = pImage->CalcLnSum(m_sx + 2 * m_w, m_sy, m_w, m_h) / s;
		m_fValue = (v1 - 2*v2 + v3) / sqrt(sigma);
		return   m_fValue;
	}
}
awpRect TLFLVFeature::GetRect()
{
	awpRect result;
	result.left = this->m_sxBase;
	result.top = this->m_syBase;
	result.right = result.left + 3 * this->m_wBase;
	result.bottom = result.top + this->m_hBase;
	return result;
}

//---------------------------------------------------------------------------
//
TCSSensor::TCSSensor()
{
  m_sxBase = 0;
  m_syBase = 0;
  m_wBase  = 0;
  m_hBase  = 0;

  m_sx     = 0;
  m_sy     = 0;
  m_w      = 0;
  m_h      = 0;
}

TCSSensor::TCSSensor(int sx, int sy, int uw, int uh)
{
  m_sxBase = sx;
  m_syBase = sy;
  m_wBase  = uw;
  m_hBase  = uh;

  m_sx = m_sxBase;
  m_sy = m_syBase;
  m_w  = m_wBase;
  m_h  = m_hBase;
}

TCSSensor::TCSSensor(const TCSSensor& Sensor)
{
  m_sxBase = Sensor.m_sxBase;
  m_syBase = Sensor.m_syBase;
  m_wBase  = Sensor.m_wBase;
  m_hBase  = Sensor.m_hBase;

  m_sx = Sensor.m_sx;
  m_sy = Sensor.m_sy;
  m_w  = Sensor.m_w;
  m_h  = Sensor.m_h;
}

TCSSensor::TCSSensor(TCSSensor* sensor) : ILFFeature(sensor)
{

}


// вычисление значения признака.
// возвращает значение [0..511]

int  TCSSensor::CalcValue(awpImage* pImage, double avg)
{
   if (m_w*m_h == 0 || pImage == NULL)
	return 0;
   double tarea = 9.0*m_w*m_h;
   double area =  1.0/(double)(m_w*m_h);

   double v[9];
   memset(v, 0, sizeof(v));
   int    iv[9];
   memset(iv, 0, sizeof(iv));

   double* pix = (double*)pImage->pPixels;
   double total = avg;
   if (avg > 0)
	total = avg;
   else
	total = CalcSum(pix, m_sx, m_sy, 3*m_w, 3*m_h, pImage->sSizeX) / tarea;
   int i = 0;

   v[0] = CalcSum(pix, m_sx, m_sy, m_w, m_h, pImage->sSizeX) * area;
   v[1] = CalcSum(pix, m_sx + m_w, m_sy, m_w, m_h, pImage->sSizeX) * area;
   v[2] = CalcSum(pix, m_sx + 2*m_w, m_sy, m_w, m_h, pImage->sSizeX) * area;
   v[3] = CalcSum(pix, m_sx, m_sy + m_h, m_w, m_h, pImage->sSizeX) * area;
   v[4] = CalcSum(pix, m_sx + m_w, m_sy + m_h, m_w, m_h, pImage->sSizeX) * area;
   v[5] = CalcSum(pix, m_sx + 2*m_w, m_sy + m_h, m_w, m_h, pImage->sSizeX) * area;
   v[6] = CalcSum(pix, m_sx, m_sy + 2*m_h, m_w, m_h, pImage->sSizeX) * area;
   v[7] = CalcSum(pix, m_sx + m_w, m_sy + 2*m_h, m_w, m_h, pImage->sSizeX) * area;
   v[8] = CalcSum(pix, m_sx + 2*m_w, m_sy + 2*m_h, m_w, m_h, pImage->sSizeX) * area;

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
	m_uValue = idx;
	m_fValue = idx;
   return m_uValue;
}

unsigned int     TCSSensor::uCalcValue(TLFImage* pImage)
{
	return this->CalcValue(pImage->GetIntegralImage(), 0);
}
double            TCSSensor::fCalcValue(TLFImage* pImage)
{
	return (double)this->CalcValue(pImage->GetIntegralImage(), 0);
}

awpRect TCSSensor::GetRect()
{
	awpRect result;
	result.left = this->m_sxBase;
	result.top = this->m_syBase;
	result.right = result.left + 3 * this->m_wBase;
	result.bottom = result.top + 3 * this->m_hBase;
	return result;
}

TCSSensor& TCSSensor::operator = (TCSSensor& Sensor)
{
  m_sxBase = Sensor.m_sxBase;
  m_syBase = Sensor.m_syBase;
  m_wBase  = Sensor.m_wBase;
  m_hBase  = Sensor.m_hBase;

  m_sx = Sensor.m_sx;
  m_sy = Sensor.m_sy;
  m_w  = Sensor.m_w;
  m_h  = Sensor.m_h;

  return *this;
}

void TCSSensor::shift(LFPoint delta)
{
	int x = (int)delta.x;
	int y = (int)delta.y;
	Shift(x, y);
}

void TCSSensor::scale(double factor)
{
	Scale(factor);
}
void TCSSensor::setup(double factor, LFPoint shift)
{
	int x = (int)shift.x;
	int y = (int)shift.y;
	Setup(factor, factor, x,y);
}
unsigned int TCSSensor::uValue(ILFImage* image)
{
	TLFImage* img = (TLFImage*)image;
	return uCalcValue(img);
}
double TCSSensor::dValue(ILFImage* image)
{
	TLFImage* img = (TLFImage*)image;
	return fCalcValue(img);
}
LFPoint TCSSensor::corner()
{
	LFPoint result;
	result.x = this->m_sx;
	result.y = this->m_sy;
	return result;
}
int TCSSensor::width()
{
	return 3 * this->m_w;
}
int TCSSensor::height()
{
	return 3 * this->m_h;
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


