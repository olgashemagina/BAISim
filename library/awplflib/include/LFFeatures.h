﻿/*M///////////////////////////////////////////////////////////////////////////////////////
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
//		File: LFFeatures.h
//		Purpose: Declares features:
//
//		TLFAFeature			- simply brigness feature
//		TLFSFeature			- simply dispertion feature
//		TLFHFeature			- horizontal gradient feature with norm
//		TLFVFeature			- vertical gradient feature with norm
//		TLFDFeature			- diagonal gradient feature with norm
//		TLFCFeature			- center feature with norm
//		TLFLBPFeature		- 256 pin binary pattern
//      TLFColorSensor9Bit  - color feature 9 bit
//		TLFLHFeature		- horizontal line feature with norm
//		TLFLVFeature		- vertical line feature with norm
//		TCSSensor			- 512 pin binary pattern (census feature)
//
//      CopyRight 2004-2018 (c) NN-Videolab.net
//M*/
#ifndef _HLFEATURES_H
#define _HLFEATURES_H

#include "LF.h"
#include "LFImage.h"
#include "LFInterface.h"

/** \defgroup LFFeatures 
*	Implementation set of features of the Locate Framework
*   @{
*/

/*
	average feature. 
*/
class TLFAFeature : public ILFFeature
{
public:
	TLFAFeature();
	TLFAFeature(int sxbase, int sybase, int wbase, int hbase);
	TLFAFeature(ILFFeature* feature);


	TResult           CalcValue(TLFImage* pImage, const TLFAlignedTransform& transform) const override;

	virtual TLFRect GetRect() const  {
		TLFRect r( m_base.Left(), m_base.Top(), m_base.Width(), m_base.Height() );
		return r;
	}

	virtual const char* GetName()
	{
		return "TLFAFeature";
	}
};

/*
	sigma feature 
*/
class TLFSFeature : public ILFFeature
{
public:
	TLFSFeature();
	TLFSFeature(int sxbase, int sybase, int wbase, int hbase);
	TLFSFeature(ILFFeature* feature);

	
	TResult           CalcValue(TLFImage* pImage, const TLFAlignedTransform& transform) const override;

	virtual TLFRect GetRect() const  {
		TLFRect r( m_base.Left(), m_base.Top(), m_base.Width(), m_base.Height() );
		return r;
	}

	virtual const char* GetName()
	{
		return "TLFSFeature";
	}
};

/*
	SA - feature 
*/
class TLFSAFeature : public ILFFeature
{
public:
	TLFSAFeature();
	TLFSAFeature(int sxbase, int sybase, int wbase, int hbase);
	TLFSAFeature(ILFFeature* feature);

	TResult           CalcValue(TLFImage* pImage, const TLFAlignedTransform& transform) const override;
	
	virtual TLFRect GetRect() const  {
		TLFRect r( m_base.Left(), m_base.Top(), m_base.Width(), m_base.Height() );
		return r;
	}

	virtual const char* GetName()
	{
		return "TLFSAFeature";
	}
};



/*
	horizontal feature with norm 
*/
class TLFHFeature : public ILFFeature
{
public:
	TLFHFeature();
	TLFHFeature(ILFFeature* feature);
	TLFHFeature(int sxbase, int sybase, int wbase, int hbase);

	TResult           CalcValue(TLFImage* pImage, const TLFAlignedTransform& transform) const override;
	
	virtual TLFRect GetRect() const  {
		TLFRect r( m_base.Left(), m_base.Top(), m_base.Width(), 2 * m_base.Height() );
		return r;
	}

	virtual const char* GetName()
	{
		return "TLFHFeature";
	}
};

/*
	vertical feature 
*/
class TLFVFeature : public ILFFeature
{
public:
	TLFVFeature();
	TLFVFeature(ILFFeature* feature);
	TLFVFeature(int sxbase, int sybase, int wbase, int hbase);

	TResult           CalcValue(TLFImage* pImage, const TLFAlignedTransform& transform) const override;
	
	virtual TLFRect GetRect() const  {
		TLFRect r( m_base.Left(), m_base.Top(), 2* m_base.Width(), m_base.Height()  );
		return r;
	}

	virtual const char* GetName()
	{
		return "TLFVFeature";
	}
};
/*
	Diagonal feature 
*/
class TLFDFeature : public ILFFeature
{
public:
	TLFDFeature();
	TLFDFeature(ILFFeature* feature);
	TLFDFeature(int sxbase, int sybase, int wbase, int hbase);

	TResult           CalcValue(TLFImage* pImage, const TLFAlignedTransform& transform) const override;

	virtual TLFRect GetRect() const  {
		TLFRect r( m_base.Left(), m_base.Top(), 2 * m_base.Width(), 2*m_base.Height() );
		return r;
	}

	virtual const char* GetName()
	{
		return "TLFDFeature";
	}
};


/*
	Center frature 
*/
class TLFCFeature : public ILFFeature
{
public:
	TLFCFeature();
	TLFCFeature(ILFFeature* feature);
	TLFCFeature(int sxbase, int sybase, int wbase, int hbase);

	TResult           CalcValue(TLFImage* pImage, const TLFAlignedTransform& transform) const override;

	virtual TLFRect GetRect() const  {
		TLFRect r( m_base.Left(), m_base.Top(), 3 * m_base.Width(), 3 * m_base.Height() );
		return r;
	}

	virtual const char* GetName()
	{
		return "TLFCFeature";
	}
};


/*
	Local Binary Pattern (LBP) feature 
*/
class TLFLBPFeature : public ILFFeature
{
protected:
	int  CalcValue(awpImage* pImage, double avg, const TLFAlignedTransform& transform) const;
public:
	TLFLBPFeature();
	TLFLBPFeature(ILFFeature* feature);
	TLFLBPFeature(int sxbase, int sybase, int wbase, int hbase);

	TResult           CalcValue(TLFImage* pImage, const TLFAlignedTransform& transform) const override;

	virtual TLFRect GetRect() const  {
		TLFRect r( m_base.Left(), m_base.Top(), 3*m_base.Width(), 3*m_base.Height() );
		return r;
	}

	virtual const char* GetName()
	{
		return "TLFLBPFeature";
	}
};



class TLFColorSensor9Bit : public ILFFeature
{
public:
	TLFColorSensor9Bit();
	TLFColorSensor9Bit(TLFColorSensor9Bit* sensor);
	TLFColorSensor9Bit(AWPWORD sx, AWPWORD sy, AWPWORD xbase, AWPWORD ybase);
	/*
	calc features value
	*/
	TResult           CalcValue(TLFImage* pImage, const TLFAlignedTransform& transform) const override;

	virtual TLFRect GetRect() const  {
		TLFRect r( m_base.Left(), m_base.Top(), m_base.Width(), m_base.Height() );
		return r;
	}

	virtual const char* GetName()
	{
		return "TLFColorSensor9Bit";
	}
};

/*
	horizontal line feature with norm
*/
class TLFLHFeature : public ILFFeature
{
public:
	TLFLHFeature();
	TLFLHFeature(ILFFeature* feature);
	TLFLHFeature(int sxbase, int sybase, int wbase, int hbase);


	TResult           CalcValue(TLFImage* pImage, const TLFAlignedTransform& transform) const override;

	virtual TLFRect GetRect() const  {
		TLFRect r( m_base.Left(), m_base.Top(), m_base.Width(), 3 * m_base.Height() );
		return r;
	}

	virtual const char* GetName()
	{
		return "TLFLHFeature";
	}
};

/*
	vertical line feature
*/
class TLFLVFeature : public ILFFeature
{
public:
	TLFLVFeature();
	TLFLVFeature(ILFFeature* feature);
	TLFLVFeature(int sxbase, int sybase, int wbase, int hbase);

	TResult           CalcValue(TLFImage* pImage, const TLFAlignedTransform& transform) const override;

	virtual TLFRect GetRect() const  {
		TLFRect r( m_base.Left(), m_base.Top(), 3 * m_base.Width(), m_base.Height() );
		return r;
	}



	virtual const char* GetName()
	{
		return "TLFLVFeature";
	}
};
//---------------------------------------------------------------------------
// ������ �������������� Census ��� ������������� �����������
class TCSSensor : public ILFFeature
{
protected:
	// ���������� �������� ��������.
	// ���������� �������� [0..511]
	int  CalcValue(awpImage* pImage, double avg, const TLFAlignedTransform& transform) const;
public:
	TCSSensor();
	TCSSensor(TCSSensor* sensor);
	TCSSensor(const TCSSensor& Sensor);
	TCSSensor(int sx, int sy, int uw, int uh);

	
	virtual TLFRect GetRect() const  {
		TLFRect r( m_base.Left(), m_base.Top(), 3 * m_base.Width(), 3 * m_base.Height() );
		return r;
	}

	virtual TResult CalcValue(TLFImage* pImage, const TLFAlignedTransform& transform) const override;


	TCSSensor& operator = ( TCSSensor& Sensor);

	
	//Base Unit of feature
	virtual  _LFRect baseUnit()  {
		_LFRect r = { (double)m_base.Left(), (double)m_base.Top(), (double)(m_base.Width()), (double)(m_base.Height()) };
		return r;
	}

	//Rect of aperture in unit coordinates
	virtual  _LFRect baseRect()  {
		_LFRect r = { (double)m_base.Left(), (double)m_base.Top(), (double)(3 * m_base.Width()), (double)(3 * m_base.Height()) };
		return r;
	}

 
	virtual const char* GetName()
	{
		return "CSFeature";
	}
};
//todo: ����� ���� ������� � �������, ������ ����� ����� �� ������� ���������� ������.
 inline double CalcSum(double* pix, int x, int y, int w, int h, int ww )
{
	double* p = pix + x + (y)*ww;

	//h = (h-1)*ww;
	//w = w - 1;
	h = h*ww;

	return (p[0] + p[w + h] - p[h] - p[w]);
}

 class TLFFeatureList : public TLFObjectList
 {
 public:
	 TLFFeatureList();
	 const char* FeatureName(int index);
 };

 /** @} */ /*  end of LFFeatures group */
#endif
