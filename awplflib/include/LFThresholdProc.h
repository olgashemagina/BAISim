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
//	File: LFThresholdProc.h
//	Purpose: Threshold Image Porcessor 
//
//	Copyright (C) 2004-2020, NN-Videolab.net, all rights reserved.
//M*/
#ifndef _LF_THRESHOLD_PROC_
#define _LF_THRESHOLD_PROC_
/** \defgroup LFImageProcessors 
*	(obsolete)Implementation threshold image processor 
*   @{
*/

typedef struct tagSLFBinaryMoments
{
	double area;
	double cx;
	double cy;
	double mxx;
	double mxy;
	double myy;
	double major;
	double minor;
	double angle;
	awp2DPoint major_p1;
	awp2DPoint major_p2;
	awp2DPoint minor_p1;
	awp2DPoint minor_p2;
}SLFBinaryMoments;

double GetMinMomnetsX(SLFBinaryMoments& m);
double GetMaxMomnetsX(SLFBinaryMoments& m);
double GetMinMomnetsY(SLFBinaryMoments& m);
double GetMaxMomnetsY(SLFBinaryMoments& m);


/* преобразует изображение из полутонового
   в бинарное.
   входное изображение должно быть типа AWP_BYTE
   и иметь один канал.
   Параметры преобразования:
   Значение порога [0..255], по умолчанию устанавливается - 128
*/
class TLFThresholdProc : public ILFImageProcessor
{
protected:
	virtual bool Process();
public:
	TLFThresholdProc ();
	virtual ~TLFThresholdProc ();

	bool ProcessMoments(TLFRect& rect, SLFBinaryMoments& moments);
};
/** @} */ /*  end of LFThreshold group */
#endif //_LF_THRESHOLD_PROC_