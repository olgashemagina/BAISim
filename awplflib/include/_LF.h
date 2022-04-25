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
//		File: _LF.h
//		Purpose: Contains internals for Locate Face 
//
//      CopyRight 2004-2018 (c) NN-Videolab.net
//M*/

#ifndef ___LF_H__
#define ___LF_H__

#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <fstream>
#include <list>
#include <algorithm>
#include <string>
#include <sstream>
#include <assert.h>
#include <float.h>
#include <iostream>
#ifdef WIN32
 #include <strstream>
#endif
#include <map>
#include <vector>
#include <cctype>
#include <string>

#ifdef WIN32
    #include <io.h>
    #include <direct.h>
#else
    #include <dirent.h>
#endif
#ifdef _OMP_
	#include <omp.h>
#endif

extern "C"
{
#include "awpipl.h"  
}
#include "LF.h"

typedef struct tagawpColor
{
	/** red channel */
	AWPBYTE bRed;
	/** green channel */
	AWPBYTE bGreen;
	/** blue channel */
	AWPBYTE bBlue;
} awpColor;


#include "LFVector.h"
#include "LFDistance.h"
#include "LFHistogramm.h"
#include "LFBuffer.h"
#include "LFCSFeature.h"

#include "LFUtils.h"
#include "LFFeatures.h"
#include "LFSample.h"
#include "LFCluster.h"
#include "LFWeak.h"
#include "LFStrong.h"
#include "LFDetector.h"
#include "LFAttrFilter.h"
#include "LFScanners.h"
#include "LFGeometry.h"
#include "LFImageProcessor.h"
#include "LFThresholdProc.h"
#include "LFEngine.h"
#include "LFParameter.h"
#include "LFPredictors.h"
#include "LFZone.h"
#include "LFFileUtils.h"
#include "LFSemantic.h"
#include "LFDatabase.h"
#include "LFCSBoost.h"
#include "LFBuilder.h"
#include "LFMCDetector.h"
#include "LFLandmarks.h"
/** \defgroup LFFactory
*	Locate Framework factory routines 
*   @{
*/
ILFPredictor* LFCreatePredictor(const char* lpName, ILFDetectEngine* engine);
ILFFeature*   LFCreateFeature(ILFFeature* feature);
ILFFeature*   LFCreateFeature(const char* lpName, int sx, int sy, int w, int h);
ILFFeature*   LFCreateFeature(const char* lpName, TiXmlElement* parent);
ILFFeature*   LFCreateFeature(TiXmlElement* parent);

ILFWeak*      LFCreateWeak(ILFWeak* weak);
ILFWeak*      LFCreateWeak(const char* lpName);
ILFStrong*    LFCreateStrong(const char* lpName);
awpRect		  LFRectToFeatureBlock(const char* lpName, awpRect& rect);

/** @} */ /*  end of LFFactory group */

template <class T>
std::string TypeToStr( const T & t )
{
std::ostringstream oss; // create a stream
oss << t; // insert value to stream
return oss.str(); // extract string and copy
}

template <class T>
T StrToType( std::string const& val )
{
std::stringstream ss; // create a stream
ss << val; // insert value to stream
T t;
ss >> t;
return t;
}

#endif// __LF_H__
