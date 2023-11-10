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
//		File: LFLabColor.h
//		Purpose: Declares Color Convertor class TLFLabColor
//
//      CopyRight 2004-2018 (c) NN-Videolab.net
//M*/

#ifndef _lf_lab_color_h_
#define _lf_lab_color_h_

/** \defgroup LFColorConveror 
*	Implementation of color convertor and color database
*   @{
*/

/*
	LF's color space conveerter
*/
class TLFLabColor : public TLFObject
{
protected:
	AWPDOUBLE m_valueL;
	AWPDOUBLE m_valueA;
	AWPDOUBLE m_valueB;
public:
	TLFLabColor();
	TLFLabColor(const AWPDOUBLE l, const  AWPDOUBLE a, const  AWPDOUBLE b);
	TLFLabColor(TLFLabColor& color);

	void GetLab(AWPDOUBLE& l, AWPDOUBLE& a, AWPDOUBLE& b);
	void GetXyz(AWPDOUBLE& x, AWPDOUBLE& y, AWPDOUBLE& z);
	void GetRGB(AWPDOUBLE& r, AWPDOUBLE& g, AWPDOUBLE& b);
	void GetHSV(AWPDOUBLE& h, AWPDOUBLE& s, AWPDOUBLE& v);
	void GetHSL(AWPDOUBLE& h, AWPDOUBLE& s, AWPDOUBLE& l);
	void GetWeb(AWPINT& web);

	void SetLab(const AWPDOUBLE l, const AWPDOUBLE a, const AWPDOUBLE b);
	void SetXyz(const AWPDOUBLE x, const AWPDOUBLE y, const AWPDOUBLE z);
	void SetRGB(const AWPDOUBLE r, const AWPDOUBLE g, const AWPDOUBLE b);
	void SetHSV(const AWPDOUBLE h, const AWPDOUBLE s, const AWPDOUBLE v);
	void SetHSL(const AWPDOUBLE h, const AWPDOUBLE s, const AWPDOUBLE l);
	void SetWeb(const AWPINT web);

	AWPDOUBLE CIE76(TLFLabColor& color);

	TiXmlElement* SaveXml();
	bool LoadXml(TiXmlElement* element);

    AWPDOUBLE _L()
    {
        return m_valueL;
    }

    AWPDOUBLE _A()
    {
        return m_valueA;
    }

    AWPDOUBLE _B()
    {
        return m_valueB;
    }

	virtual const char* GetName()
	{
		return "TLFLabColor";
	}
};

class TLFLabColorDatabase : public TLFObjectList
{
public:
	TLFLabColorDatabase();
	TLFLabColorDatabase(TLFLabColorDatabase& _db);
    TLFLabColorDatabase& operator=(TLFLabColorDatabase& _db);

	void AddColor(TLFLabColor& color);
	TLFLabColor* GetColor(int index);

	bool SaveToXml(const char* lpFileName);
	bool LoadFromXml(const char* lpFileName);

	TLFLabColor* Center();
	TLFLabColor* Nearlest(TLFLabColor& color, double& dist, int& index);
    bool SortFromColor(TLFLabColor& color, TLFLabColorDatabase& db);
    double Distance(TLFLabColor& color, int idx);
    bool  Copy(TLFLabColorDatabase& _db, int start, int end);

	virtual const char* GetName()
	{
		return "TLFLabColorDatabase";
	}
};

/** @} */ /*  end of LFColorConveror group */

#endif // _lf_lab_color_h_