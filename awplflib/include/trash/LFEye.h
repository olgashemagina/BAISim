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
//		File: LFEye.h
//		Purpose: Eye detection mplementation 
//
//      CopyRight 2004-2020 (c) NN-Videolab.net
//M*/
#ifndef LFEyeH
#define LFEyeH
/** \addtogroup LFDescriptors
*	(obsolete) Eye detectors 
*   @{
*/

/**
@brief obsolete
*/
enum LFEyes {LFLeftEye, LFRightEye};
/**
*@brief obsolete 
*/
enum LFPreciseMethod { LFAverage, LFExtremum, LFMean };
/**
*@brief obsolete
*class interface that performs eye positioning
*/
class ILFEye : public TLFObject
{
protected:
   bool m_IsInitialized;
   virtual bool CheckEye(awpRect& r, awpImage* pImage, double& err, LFEyes eye) = 0;
public:
   ILFEye(){m_IsInitialized = false;};
   virtual ~ILFEye(){};

   virtual bool Init(const char* lpInitString)= 0;
   virtual bool SetImage(awpImage* pImage) = 0;
   /** @brief
   * Refines the eye position [roi] in the image [image]
   * in case of an error returns NULL
   */
   virtual TLFRoi* Precise(TLFImage& image, TLFRoi& roi, LFPreciseMethod Method = LFAverage);
};

/**
*@brief obsolete
an ILFRoi implementation that uses a correlation comparison algorithm
*/

class TLFEyeCorrelation : public ILFEye
{
private:

   awpImage*  m_pLeftEye;        // эталонное изображение левого глаза
   awpImage*  m_pRightEye;       // эталонное изображение правого глаза

   double      m_fMathLeft;       // математическое ожидание для эталона левого глаза
   double      m_fDispLeft;       // корень из дисперсии для левого глаза

   double      m_fMathRight;      // математическое ожидание для эталона правого глаза
   double      m_fDispRight;      // корень из дисперсии для правого глаза


   double Correlation(double* TestVct1, LFEyes eye);

protected:
   virtual bool CheckEye(awpRect& r, awpImage* pImage, double& err, LFEyes eye);
public:
   TLFEyeCorrelation();
   virtual ~TLFEyeCorrelation();
   virtual bool Init(const char* lpInitString);
   virtual bool SetImage(awpImage* pImage){return true;};
};
/** @} */ 
#endif
