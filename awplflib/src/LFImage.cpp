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
//		File: LFImage.cpp
//		Purpose: implementation of TLFImage
//
//      CopyRight 2004-2018 (c) NN-Videolab.net
//M*/
#include "_LF.h"

#ifndef MIN
#define MIN(a,b)  ((a) > (b) ? (b) : (a))
#endif

#ifndef MAX
#define MAX(a,b)  ((a) < (b) ? (b) : (a))
#endif

static long filesize(FILE *stream)
{
   long curpos, length;

   curpos = ftell(stream);
   fseek(stream, 0L, SEEK_END);
   length = ftell(stream);
   fseek(stream, curpos, SEEK_SET);
   return length;

}

TLFImage::TLFImage()
{
    m_pImage = NULL;
	m_pIntegralImage = NULL;  
	m_pIntegralSquareImage= NULL;

	m_pRed = NULL;
	m_pGreen = NULL;
	m_pBlue = NULL;

	m_pIntegralRed = NULL;
	m_pIntegralGreen = NULL;
	m_pIntegralBlue = NULL;

	m_pSqIntegralRed = NULL;
	m_pSqIntegralGreen = NULL;
	m_pSqIntegralBlue = NULL;

	m_lnpix = NULL;
	m_sqpix = NULL;
	m_rlnpix = NULL;
	m_glnpix = NULL;
	m_blnpix = NULL;
	m_rsqlnpix = NULL;
	m_gsqlnpix = NULL;
	m_bsqlnpix = NULL;
}

TLFImage::~TLFImage()
{
	FreeImages();
}

void TLFImage::FreeImages()
{
	_AWP_SAFE_RELEASE_(m_pImage);
	_AWP_SAFE_RELEASE_(m_pIntegralImage);	
	_AWP_SAFE_RELEASE_(m_pIntegralSquareImage);

	_AWP_SAFE_RELEASE_(m_pRed);
	_AWP_SAFE_RELEASE_(m_pGreen);
	_AWP_SAFE_RELEASE_(m_pBlue);

	_AWP_SAFE_RELEASE_(m_pIntegralRed);
	_AWP_SAFE_RELEASE_(m_pIntegralGreen);
	_AWP_SAFE_RELEASE_(m_pIntegralBlue);

	_AWP_SAFE_RELEASE_(m_pSqIntegralRed);
	_AWP_SAFE_RELEASE_(m_pSqIntegralGreen);
	_AWP_SAFE_RELEASE_(m_pSqIntegralBlue);

	m_lnpix = NULL;
	m_sqpix = NULL;
	m_rlnpix = NULL;
	m_glnpix = NULL;
	m_blnpix = NULL;

	m_rsqlnpix = NULL;
	m_gsqlnpix = NULL;
	m_bsqlnpix = NULL;


	m_pRed = NULL;
	m_pGreen = NULL;
	m_pBlue = NULL;

	m_pIntegralRed = NULL;
	m_pIntegralGreen = NULL;
	m_pIntegralBlue = NULL;

	m_pSqIntegralRed = NULL;
	m_pSqIntegralGreen = NULL;
	m_pSqIntegralBlue = NULL;

}

bool TLFImage::LoadFromFile(const char* szFileName)
{
    bool res = true;
	FreeImages();
    try
    {
	//printf("Loading %s ... ", szFileName);		
	if (awpLoadImage(szFileName, &m_pImage) != AWP_OK)
           throw 0;
	//printf(" done. \n", szFileName);
   	//GetBlueIntegral();
	//GetGreenIntegral();
	//GetRedIntegral();
    }
    catch(...)
    {
	printf("TLFImage::LoadFromFile exception!\n");	
        res = false;
    }
    return res;
}

bool TLFImage::SaveToFile(const char* szFileName)
{
    bool res = true;
    if (m_pImage == NULL)
        return false;
    try
    {
        if (awpSaveImage(szFileName, m_pImage) != AWP_OK)
            throw 0;
    }
    catch(...)
    {
        res = false;
    }
    return res;
}

// операции с образом памяти
bool TLFImage::SaveDump(const char* szFileName)
{
    bool res = true;
    unsigned char* pDump = NULL;
    int DumpLen = 0;
    FILE* F = NULL;
    if (m_pImage == NULL)
        return false;
    try
    {
        if (awpImagePack(m_pImage, &pDump, &DumpLen) != AWP_OK)
            throw 0;
        F = fopen (szFileName, "w+b");
        if (F == NULL)
            throw 0;
        if (fwrite(pDump, DumpLen, 1, F) != 1)
            throw 0;
    }
    catch(...)
    {
        res = false;
    }
    if (pDump != NULL)
        free (pDump);
    if (F != NULL)
        fclose(F);
    return res;
}

bool TLFImage::LoadDump(const char* szFileName)
{
    bool res = true;
    FILE* F = NULL;
    unsigned char* pDump = NULL;
    int nDumpSize = 0;
	FreeImages();
    try
    {
         F = fopen(szFileName, "r+b");
         if (F == NULL)
            throw 0;
         nDumpSize = filesize(F);
         if (nDumpSize == 0)
            throw 0;
         pDump = (unsigned char*)malloc(nDumpSize);
         if( pDump == NULL )
            throw 0;
         if (fread(pDump, nDumpSize, 1, F) != 1)
            throw 0;
         if (awpImageUnpack(pDump, nDumpSize, &m_pImage) != AWP_OK)
            throw 0;
    }
    catch(...)
    {
        res = false;
    }
    if (pDump != NULL)
        free(pDump);
    if (F != NULL)
        fclose(F);    
    return res;
}

bool TLFImage::LoadMemDump(const unsigned char* pDump, int nDumpSize)
{
	bool res = true;
	FreeImages();
	try
	{
		if (awpImageUnpack(pDump, nDumpSize, &m_pImage) != AWP_OK)
			throw 0;
	}
	catch(...)
	{
		res = false;
	}
	return res;
}

//свойства изображения
awpImage* TLFImage::GetImage()
{
    return m_pImage;
}

awpImage* TLFImage::GetRedImage()
{
	if (this->m_pRed == NULL)
	{
		if (this->m_pImage->bChannels == 3)
		{
			awpGetChannel(this->m_pImage, &this->m_pRed, 2);
			return this->m_pRed;
		}
		else if (this->m_pImage->bChannels == 1)
		{
			return m_pImage;
		}
		else
			return NULL;
	}
	else
		return this->m_pRed;
}

awpImage* TLFImage::GetGreenImage()
{
	if (this->m_pGreen == NULL)
	{
		if (this->m_pImage->bChannels == 3)
		{
			awpGetChannel(this->m_pImage, &this->m_pGreen, 1);
			return this->m_pGreen;
		}
		else if (this->m_pImage->bChannels == 1)
		{
			return m_pImage;
		}
		else
			return NULL;
	}
	else
		return this->m_pGreen;
}

awpImage* TLFImage::GetBlueImage()
{
	if (this->m_pBlue == NULL)
	{
		if (this->m_pImage->bChannels == 3)
		{
			awpGetChannel(this->m_pImage, &this->m_pBlue, 0);
			return this->m_pBlue;
		}
		else if (this->m_pImage->bChannels == 1)
		{
			return m_pImage;
		}
		else
			return NULL;
	}
	else
		return this->m_pBlue;
}



awpImage* TLFImage::GetIntegralImage()
{
	// получение интегрального изображения. 
	if (this->m_pIntegralImage != NULL)
		return this->m_pIntegralImage;
	if (this->m_pImage == NULL)
		return NULL;

	awpImage* dbl = NULL;
	awpCopyImage(m_pImage, &dbl);
	if (m_pImage->bChannels == 3)
		awpConvert(dbl, AWP_CONVERT_3TO1_BYTE);
	awpIntegral(dbl, &this->m_pIntegralImage, AWP_LINEAR);
	awpReleaseImage(&dbl);
	this->m_lnpix = (double*)this->m_pIntegralImage->pPixels;
	return this->m_pIntegralImage;
}

awpImage* TLFImage::GetSqIntegralImage()
{
	if (this->m_pIntegralSquareImage != NULL)
		return this->m_pIntegralSquareImage;
	if (this->m_pImage == NULL)
		return NULL;
	awpImage* dbl = NULL;
	awpCopyImage(m_pImage, &dbl);
	if (m_pImage->bChannels == 3)
		awpConvert(dbl, AWP_CONVERT_3TO1_BYTE);
	awpIntegral(dbl, &this->m_pIntegralSquareImage, AWP_SQUARE);
	awpReleaseImage(&dbl);
	this->m_sqpix = (double*)m_pIntegralSquareImage->pPixels;
	return this->m_pIntegralSquareImage;
}

awpImage* TLFImage::GetRedIntegral()
{
	if (this->m_pIntegralRed != NULL)
		return this->m_pIntegralRed;
	if (this->m_pImage == NULL)
		return NULL;
	awpImage* img = this->GetRedImage();
	if (img == NULL)
		return NULL;
	awpIntegral(img, &this->m_pIntegralRed, AWP_LINEAR);
	if (this->m_pIntegralRed != NULL)
		this->m_rlnpix = (double*)this->m_pIntegralRed->pPixels;
	return this->m_pIntegralRed;
}

awpImage* TLFImage::GetGreenIntegral()
{
	if (this->m_pIntegralGreen != NULL)
		return this->m_pIntegralGreen;
	if (this->m_pImage == NULL)
		return NULL;
	awpImage* img = this->GetGreenImage();
	if (img == NULL)
		return NULL;
	awpIntegral(img, &this->m_pIntegralGreen, AWP_LINEAR);
	if (this->m_pIntegralGreen != NULL)
		this->m_glnpix = (double*)this->m_pIntegralGreen->pPixels;
	return this->m_pIntegralGreen;
}
awpImage* TLFImage::GetBlueIntegral()
{
	if (this->m_pIntegralBlue != NULL)
		return this->m_pIntegralBlue;
	if (this->m_pImage == NULL)
		return NULL;
	awpImage* img = this->GetBlueImage();
	if (img == NULL)
		return NULL;
	awpIntegral(img, &this->m_pIntegralBlue, AWP_LINEAR);
	if (this->m_pIntegralBlue != NULL)
		this->m_blnpix = (double*)this->m_pIntegralBlue->pPixels;
	return this->m_pIntegralBlue;
}

awpImage* TLFImage::GetSqRedIntegral()
{
	if (this->m_pSqIntegralRed != NULL)
		return this->m_pSqIntegralRed;
	if (this->m_pImage == NULL)
		return NULL;
	awpImage* img = this->GetRedImage();
	if (img == NULL)
		return NULL;
	awpIntegral(img, &this->m_pSqIntegralRed, AWP_SQUARE);
	if (this->m_pSqIntegralRed != NULL)
		this->m_rsqlnpix = (double*)this->m_pSqIntegralRed->pPixels;

	return this->m_pSqIntegralRed;
}
awpImage* TLFImage::GetSqGreenIntegral()
{
	if (this->m_pSqIntegralGreen != NULL)
		return this->m_pSqIntegralGreen;
	if (this->m_pImage == NULL)
		return NULL;
	awpImage* img = this->GetGreenImage();
	if (img == NULL)
		return NULL;
	awpIntegral(img, &this->m_pSqIntegralGreen, AWP_SQUARE);
	if (this->m_pSqIntegralGreen != NULL)
		this->m_gsqlnpix = (double*)this->m_pSqIntegralGreen->pPixels;

	return this->m_pSqIntegralGreen;

}
awpImage* TLFImage::GetSqBlueIntegral()
{
	if (this->m_pSqIntegralBlue != NULL)
		return this->m_pSqIntegralBlue;
	if (this->m_pImage == NULL)
		return NULL;
	awpImage* img = this->GetBlueImage();
	if (img == NULL)
		return NULL;
	awpIntegral(img, &this->m_pSqIntegralBlue, AWP_SQUARE);
	if (this->m_pSqIntegralBlue != NULL)
		this->m_bsqlnpix = (double*)this->m_pSqIntegralBlue->pPixels;

	return this->m_pSqIntegralBlue;

}



bool TLFImage::SetImage(awpImage* pImage)
{
    bool res = true;
    AWPRESULT awpres;
	FreeImages();
    try
    {
        awpres = awpCopyImage(pImage, &m_pImage);
        if ( awpres != AWP_OK)
             throw 0;
    }
    catch(...)
    {
        res = false;
        if (awpres == AWP_BADMEMORY)
        {
            throw 0;
        }
    }
    return res;
}

bool TLFImage::CopyImage(awpImage** ppImage)
{
    bool res = true;
    if (m_pImage == NULL)
        return false;
    try
    {
        if (awpCopyImage(m_pImage, ppImage))
            throw 0;
    }
    catch(...)
    {
        res = false;
    }
    return res;
}
#ifdef WIN32
/*bool TLFImage::LoadResourceImage(char* lpName)
{
    bool    res = true;         // результат работы функци
    HRSRC   hRC = NULL;         //
    HGLOBAL hgTempl = NULL;     //
    int     DumpSize = 0;       //
    unsigned char*   pDump = NULL;
    HMODULE hModule = NULL;
	try
	{
        hModule = GetModuleHandle("awplf.dll");
        if (hModule == NULL)
            throw 0;
        hRC = FindResource(hModule, lpName, "AWPIMAGE");
        if (hRC == NULL)
            throw 0;
        DumpSize = SizeofResource(hModule, hRC);
        if (DumpSize == 0)
           throw 0;
        hgTempl = LoadResource(hModule, hRC);
        if (hgTempl == NULL)
           throw 0;
        pDump = (unsigned char*)LockResource(hgTempl);
        if (pDump == NULL)
           throw 0;
        if (!LoadMemDump(pDump, DumpSize))
           throw 0;
	}
	catch(...)
	{
		res = false;
	}
	return res;
} */


void  TLFImage::Show( HDC dc, int x, int y, int w, int h, int from_x, int from_y )
{
    if (m_pImage == NULL) return;
	if( m_pImage->sSizeX > 0 )
    {
        unsigned char buffer[sizeof(BITMAPINFOHEADER) + 1024];
        BITMAPINFO* bmi = (BITMAPINFO*)buffer;
        int bmp_w = m_pImage->sSizeX;
        int bmp_h = m_pImage->sSizeY;


		BITMAPINFOHEADER* bmih = &(bmi->bmiHeader);

		memset( bmih, 0, sizeof(*bmih));
		bmih->biSize   = sizeof(BITMAPINFOHEADER); 
		bmih->biWidth  = bmp_w;
		bmih->biHeight = -abs(bmp_h);
		bmih->biPlanes = 1; 
		bmih->biCompression = BI_RGB;

		if (m_pImage->dwType == AWP_BYTE && m_pImage->bChannels == 3)
			bmih->biBitCount = 24;
		else
		{
			bmih->biBitCount = 8;
			RGBQUAD* palette = bmi->bmiColors;
			int i;
			for( i = 0; i < 256; i++ )
			{
				palette[i].rgbBlue = palette[i].rgbGreen = palette[i].rgbRed = (BYTE)i;
				palette[i].rgbReserved = 0;
			}
			
		}
        int sw = MAX( MIN( bmp_w - from_x, w ), 0 );
        int sh = MAX( MIN( bmp_h - from_y, h ), 0 );

        SetDIBitsToDevice(
              dc,                        // handle to DC
              x,                         // x-coord of destination upper-left corner
              y,                         // y-coord of destination upper-left corner 
              sw,                        // source rectangle width
              sh,                        // source rectangle height
              from_x,                    // x-coord of source lower-left corner
              from_y,                    // y-coord of source lower-left corner
              from_y,                    // first scan line in array
              sh,                        // number of scan lines
              m_pImage->pPixels ,    // array of DIB bits
              (BITMAPINFO*)bmi,          // bitmap information
              DIB_RGB_COLORS );          // RGB or palette indexes
    }
}

void  TLFImage::StretchShow(HDC dc, int x, int y, int w, int h, int from_x, int from_y)
{
    if (m_pImage == NULL) 
	{
		
		ExtFloodFill(dc, 0,0, RGB(128, 128,128), FLOODFILLBORDER   );	
		return;
	}
	if( m_pImage->sSizeX > 0 )
    {
		BITMAPINFO bi; 
		unsigned char* data = NULL;
		::awpImageToDIB(m_pImage, &bi, &data, false);
		::SetStretchBltMode(dc, COLORONCOLOR);
		//ExtFloodFill(dc, 0,0, RGB(128, 128,128), FLOODFILLBORDER);	
        StretchDIBits(
              dc,                        // handle to DC
              x,                         // x-coord of destination upper-left corner
              y,                         // y-coord of destination upper-left corner
			  w,
			  h,
              from_x,                        // source rectangle width
              from_y,                        // source rectangle height
              m_pImage->sSizeX,          // x-coord of source lower-left corner
              m_pImage->sSizeY,         // y-coord of source lower-left corner
              data ,					// array of DIB bits
              &bi,          // bitmap information
              DIB_RGB_COLORS,
			  SRCCOPY);          // RGB or palette indexes
		free(data);
    }
}
#endif

 double TLFImage::CalcLnSum(int x, int y, int w, int h)
{
	if (this->GetIntegralImage() == NULL)
		return 0;

	double* p = m_lnpix + x + y*m_pIntegralImage->sSizeX;

	h = h*m_pIntegralImage->sSizeX;

	return (p[0] + p[w + h] - p[h] - p[w]);
}

double TLFImage::CalcSqSum(int x, int y, int w, int h)
{
	if (this->GetSqIntegralImage() == NULL)
		return 0;

	double* p = this->m_sqpix + x + y*m_pIntegralSquareImage->sSizeX;

	h = h*m_pIntegralSquareImage->sSizeX;

	return (p[0] + p[w + h] - p[h] - p[w]);

}

double TLFImage::CalcRLnSum(int x, int y, int w, int h)
{
	if (GetRedIntegral() == NULL)
		return 0;
	double* p = m_rlnpix + x + y*m_pIntegralRed->sSizeX;

	h = h*m_pIntegralRed->sSizeX;

	return (p[0] + p[w + h] - p[h] - p[w]);

}
double TLFImage::CalcGLnSum(int x, int y, int w, int h)
{
	if (GetGreenIntegral() == NULL)
		return 0;
	double* p = m_glnpix + x + y*m_pIntegralGreen->sSizeX;

	h = h*m_pIntegralGreen->sSizeX;

	return (p[0] + p[w + h] - p[h] - p[w]);
}
double TLFImage::CalcBLnSum(int x, int y, int w, int h)
{
	if (GetBlueIntegral() == NULL)
		return 0;
	double* p = m_blnpix + x + y*m_pIntegralBlue->sSizeX;

	h = h*m_pIntegralBlue->sSizeX;

	return (p[0] + p[w + h] - p[h] - p[w]);
}


void TLFImageList::AddImage(TLFImage* img)
{
	this->Add(img);
}
TLFImage* TLFImageList::GetImage(int index)
{
	return (TLFImage*)this->Get(index);
}


double* TLFImage::integral(int channel)
{
	if (channel < 0 || m_pImage == NULL)
		return NULL;
	if (channel >= m_pImage->bChannels)
		return NULL;
	if (m_pImage->bChannels == 1 && channel == 0)
		return (double*)(GetIntegralImage()->pPixels);
	if (m_pImage->bChannels == 3)
	{
		if (channel == 0)
			return (double*)(GetRedIntegral()->pPixels);
		else if (channel == 1)
			return (double*)(GetGreenIntegral()->pPixels);
		else if (channel == 2)
			return (double*)(GetBlueIntegral()->pPixels);
		else
			return NULL;
	}
	else
		return NULL;

}
double* TLFImage::integral2(int channel)
{
	if (channel < 0 || m_pImage == NULL)
		return NULL;
	if (channel >= m_pImage->bChannels)
		return NULL;
	if (m_pImage->bChannels == 1 && channel == 0)
		return (double*)(this->GetSqIntegralImage()->pPixels);
	if (m_pImage->bChannels == 3)
	{
		if (channel == 0)
			return (double*)(GetSqRedIntegral()->pPixels);
		else if (channel == 1)
			return (double*)(GetSqGreenIntegral()->pPixels);
		else if (channel == 2)
			return (double*)(GetSqBlueIntegral()->pPixels);
		else
			return NULL;
	}
	else
		return NULL;
}
int TLFImage::width()
{
	if (this->m_pImage == NULL)
		return 0;
	return m_pImage->sSizeX;
}
int TLFImage::height()
{
	if (this->m_pImage == NULL)
		return 0;
	return m_pImage->sSizeY;

}
int TLFImage::channels()
{
	if (this->m_pImage == NULL)
		return 0;
	return (int)m_pImage->bChannels;

}
