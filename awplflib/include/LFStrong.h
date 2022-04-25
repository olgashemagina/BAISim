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
//	Locate Framework  4.0
//	File: LFStrong.h
//	Purpose: Strong classifiers. Implements simply artificial neurons, 
//	such as the McCullochЦPitts model, based on the list of ILFWeak input elements.
//
//	Copyright (C) 2004-2020, NN-Videolab.net, all rights reserved.
//M*/

#ifndef LFSTRONGH
#define LFSTRONGH
/** \defgroup LFStrongs
*	Implementation the strong classifiers in the Locate Framework
*   @{
*/
//---------------------------------------------------------------------------
// сильный классификатор, основанный на преобразовании Census
class TCSStrong : public ILFStrong
{
protected:
	double m_sumWeakWeight;
public:
    TCSStrong();
    // позиционирование
    void Setup( awpRect const& window, int det_width );
    // классификафи€
	virtual int Classify(TLFImage* pImage, double& err);

	virtual void SaveXML(TiXmlElement* parent);
	virtual bool LoadXML(TiXmlElement* parent);

	double GetSumWeakWeight();
};
// сильный классификатор, основанный на MCT и TCSWeakSign
class TCSStrongSign : public TLFObject
{
protected:
    TLFObjectList  m_WeakList;      // список слабых класификаторов
    double         m_Threshold;      // порог прин€ти€ решени€.
    double         m_sumWeakWeight;  // сумма всех весов слабых классификаторов
public:
    TCSStrongSign();
    // позиционирование
    void Setup( awpRect const& window, int det_width );
    // классификафи€
    int Classify( awpImage* pImage, double& err, double avg = 0 );
    //ввод - вывод
    void SaveXML(TiXmlElement* parent);
    bool LoadXML(TiXmlElement* parent);

    // порог прин€ти€ решени€
    double GetThreshold();
    void   SetThreshold(double Value);

    double GetSumWeakWeight()
    {
        return m_sumWeakWeight;
    }

    TCSWeakSign* GetWeak(int index);
    void AddWeak(TCSWeakSign* Weak);

    void Clear();

    int WeakCount()
    {
        return   m_WeakList.GetCount();
    }

};
class TIEFSStrong : public TLFObjectList
{
protected:
	double m_threshold;
	TiXmlElement* SaveXML();
public:
	TIEFSStrong();
	double Raiting(TIEFSSample* sample);
	int Classify(TIEFSSample* sample);
	double GetSumWeakWeight();
	double GetThreshold();
	void SetThreshold(double value);
	//io
	bool SaveXML(const char* lpFileName);
	bool LoadXML(const char* lpFileName);

	virtual const char* GetName()
	{
		return "TIEFSStrong";
	}
};
/** @} */ /*  end of LFStrongs group */
#endif
