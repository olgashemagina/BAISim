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
//	File: LFSample.h
//	Purpose: Samples for training 
//	Copyright (C) 2004-2020, NN-Videolab.net, all rights reserved.
//M*/

#ifndef LFSAMPLE
#define LFSAMPLE
#include "LF.h"
/** \defgroup LFSample
*	Implementation sample data for AdaBoost trainig  of the Locate Framework
*   @{
*/

/*
A sample for training a regular object detector.
It is an integrated image and additional
information. Weight, rating and so on.
*/
class TCSSample : public TLFImage
{
protected:
    int             m_Flag;			// признак принадлежности к одному из двух классов. 1 - свои, 0 - чужие. 
    double          m_Weight;		// вес. Нужен для работы алгоритма AdaBoost 
	double			m_rating;		// нужен для сохранения ошибочно классифицированных образцов. 	
public:
    TCSSample();
	TCSSample(TCSSample& sample);
	TCSSample(TCSSample* sample);
	virtual ~TCSSample();

	int GetFlag();
    void SetFlag(int Value);
    
	double GetWeight();
	void   SetWeight(double Value);

	
	bool   GetIsProcessed();
	double  GetRating();
	void    SetRating(double rating); // 
};

// образец, используемый в системе обучения по исходным изображениям
class TCSSoftSample :  public TLFObject
{
protected:
        // указатель на внешнее интегральное изображение
        awpImage* m_pImage;
        // фрагмент изображения
        awpRect   m_Fragment;
        // флаг образца 0 - фон, 1- объект
        int       m_Flag;
        // "вес" образца
        double    m_Weight;
        int       m_Classifyed;
public:
        TCSSoftSample(awpImage* pImage, awpRect& Rect, int Flag, double Weight);
        //атрибуты
		awpImage* GetImage();
		awpRect GetFragment();
		int GetFlag();
		double GetWeight();
		void SetWeight(double Value);
		int GetClassifyed();
		void SetClassifyed(int Value);

};
/*
	Образец для обучения. 
*/
class TIEFSSample : public TLFObject
{
protected:
	double* m_deltas;
	double  m_wight;
	int     m_flag;
public:
	TIEFSSample(double* data, int num, int flag);
	virtual ~TIEFSSample();
	int GetFlag();
	void SetFlag(int Value);

	double GetWeight();
	void   SetWeight(double Value);

	double GetValue(int index);
};
/** @} */ /*  end of LFSSample group */
#endif //LFSAMPLE 