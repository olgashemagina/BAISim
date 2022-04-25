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
//	File: LFScanners.h
//	Purpose: Different scanners declaration 
//	Copyright (C) 2004-2018, NN-Videolab.net, all rights reserved.
//M*/
#ifndef ScannersH
#define ScannersH

/** \defgroup LFScanners
*	Implementation classes for splitting the input image into rectangular areas in the Locate Framework 
*   @{
*/

/*
	standart multiscale scanner 
*/
class TLFScanner : public ILFScanner
{
protected:
public:
	TLFScanner();  
	virtual ~TLFScanner();
	virtual bool Scan(int w, int h);
    virtual const char* GetName()
    {
        return  "TLFScanner";
    }
};
/*
	tile scanner 
*/
class TLFTileScanner : public ILFScanner
{
protected:
	int m_numX;
	int m_numY;
public:
	TLFTileScanner();
	virtual bool Scan(int w, int h);
	int GetNumX();
	int GetNumY();
	virtual const char* GetName()
	{
		return  "TLFTileScanner";
	}
};

class TLFTileScaleScanner : public ILFScanner
{
protected:
	unsigned int    m_overlap;
	double m_min_scale;
	double m_max_scale;
	double m_grow;

public:
   TLFTileScaleScanner();
   TLFTileScaleScanner(unsigned int bw, unsigned int bh);
   TLFTileScaleScanner(unsigned int bw, unsigned int bh, double min_scale, double max_scale, double grow, unsigned int overlap);

	virtual bool Scan(int w, int h);

	virtual const char* GetName()
	{
		return  "TLFTileScaleScanner";
	}

};
/*
	
*/
class TLFAllScanner : public ILFScanner
{
protected:
public:
	TLFAllScanner();
	virtual bool Scan(int w, int h);
	virtual const char* GetName()
	{
		return  "TLFAllScanner";
	}
};

class TLFWholeImageScanner : public ILFScanner
{
protected:
public:
	TLFWholeImageScanner();
	virtual bool Scan(int w, int h);
	virtual const char* GetName()
	{
		return  "TLFWholeImageScanner";
	}
};

/** @} */ /*  end of LFSSemantic group */
#endif
