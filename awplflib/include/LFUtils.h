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
//		File: LFUtils.h
//		Purpose: different utils to work with ROI
//
//      CopyRight 2004-2018 (c) NN-Videolab.net
//M*/
//---------------------------------------------------------------------------
#ifndef RoiUtilsH
#define RoiUtilsH
//---------------------------------------------------------------------------
/** \defgroup LFRoiUtils
*	 (obsolete) Different utils to work with ROI
*   @{
*/
awp2DPoint  awpGetPointsCentroid(awp2DPoint* p, int count);
awpPoint    awpGetPointsCentroid(awpPoint* p, int count);
awpRect     awpResizeRect(awpRect r, double scale);
awpPoint    awpGetLeftTopRectCorner(awpRect r);
awpPoint    awpGetRightBottomRectCorner(awpRect r);
awpRect     awpTranslateRect(awpRect r, awpPoint p);
awpRect     awpTranslateRect(awpRect r, awp2DPoint p);
awpPoint    awpTranslatePoint (awpPoint p, int dx, int dy);
awpPoint    awpTranslatePoint (awp2DPoint p, int dx, int dy);
int awpRectWidth(awpRect r);
int awpRectHeight(awpRect r);
awpRect  awpCreateRect(awpPoint lt, awpPoint rb);
AWPBOOL IsPointInRect(awpPoint p, awpRect r);
awpPoint RectCenter (awpRect r);

/** @} */ /*  end of LFRoiUtils group */
#endif
