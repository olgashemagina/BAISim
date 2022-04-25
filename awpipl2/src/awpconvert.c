/*M
//
//      awpipl2 image processing and image analysis library
//		File: awpconvert.c
//		Purpose: data type convert functions 
//		Autors: Alex A.Telnykh (tlnykha@yahoo.com)
//      CopyRight 2004-2020 (c) NN-Videolab.net
//M*/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include "_awpipl.h"
#include <limits.h>


AWPRESULT _awpConvertByteToAWPAWPDOUBLE(awpImage* Image)
{
    AWPRESULT res;
    AWPDOUBLE* pD;
    AWPINT i;
    AWPBYTE* pB;
    long lSize;

    res = AWP_OK;/*initialize result */

    lSize=Image->sSizeX*Image->sSizeY*Image->bChannels;

    /* allocate memory for AWPDOUBLE data */
    pD = (AWPDOUBLE*)malloc(sizeof(AWPDOUBLE)*lSize);

    if(pD==NULL){
        res = AWP_BADMEMORY;
        _ERR_EXIT_
    }

    pB=(AWPBYTE*)Image->pPixels;


    if(res!=AWP_OK){
        free((void*)pD);
        _ERR_EXIT_
    }

    for(i=0;i<lSize;i++){
        pD[i]=pB[i];
    }

    free((void*)pB);
    Image->dwType=AWP_DOUBLE;
    Image->pPixels=(void*)pD;

CLEANUP:

    return res;
}

AWPRESULT _awpConvertByteToFloat(awpImage* Image)
{
	AWPRESULT res;
	AWPFLOAT* pD;
	AWPINT i;
	AWPBYTE* pB;
	long lSize;

	res = AWP_OK;/*initialize result */

	lSize=Image->sSizeX*Image->sSizeY*Image->bChannels;

	/* allocate memory for AWPFLOAT data */
	pD = (AWPFLOAT*)malloc(sizeof(AWPFLOAT)*lSize);

	if(pD==NULL){
		res = AWP_BADMEMORY;
		_ERR_EXIT_
	}

	pB=(AWPBYTE*)Image->pPixels;


	if(res!=AWP_OK){
		free((void*)pD);
		_ERR_EXIT_
	}

	for(i=0;i<lSize;i++){
		pD[i]=pB[i];
	}

	free((void*)pB);
	Image->dwType=AWP_FLOAT;
	Image->pPixels=(void*)pD;

CLEANUP:

	return res;
}

AWPRESULT _awpConvertDoubleToFloat(awpImage* Image)
{
	AWPRESULT res;
	AWPFLOAT* pD;
	AWPINT i;
	AWPDOUBLE* pB;
	long lSize;

	res = AWP_OK;/*initialize result */

	lSize=Image->sSizeX*Image->sSizeY*Image->bChannels;

	/* allocate memory for AWPFLOAT data */
	pD = (AWPFLOAT*)malloc(sizeof(AWPFLOAT)*lSize);

	if(pD==NULL){
		res = AWP_BADMEMORY;
		_ERR_EXIT_
	}

	pB=(AWPDOUBLE*)Image->pPixels;


	if(res!=AWP_OK){
		free((void*)pD);
		_ERR_EXIT_
	}

	for(i=0;i<lSize;i++){
		pD[i]=(AWPFLOAT)pB[i];
	}

	free((void*)pB);
	Image->dwType=AWP_FLOAT;
	Image->pPixels=(void*)pD;

CLEANUP:

	return res;
}


AWPRESULT _awpConvertFloatToDouble(awpImage* Image)
{
	AWPRESULT res;
	AWPDOUBLE* pD;
	AWPINT i;
	AWPFLOAT* pB;
	long lSize;

	res = AWP_OK;/*initialize result */

	lSize=Image->sSizeX*Image->sSizeY*Image->bChannels;

	/* allocate memory for AWPDOUBLE data */
	pD = (AWPDOUBLE*)malloc(sizeof(AWPDOUBLE)*lSize);

	if(pD==NULL){
		res = AWP_BADMEMORY;
		_ERR_EXIT_
	}

	pB=(AWPFLOAT*)Image->pPixels;


	if(res!=AWP_OK){
		free((void*)pD);
		_ERR_EXIT_
	}

	for(i=0;i<lSize;i++){
		pD[i]=(AWPDOUBLE)pB[i];
	}

	free((void*)pB);
	Image->dwType=AWP_DOUBLE;
	Image->pPixels=(void*)pD;

CLEANUP:

	return res;
}


AWPRESULT _awpConvertByte3To1(awpImage* Image)
{
    AWPRESULT res;
    long lSize;
    AWPBYTE* pB=NULL;
    AWPBYTE* pBSrc=NULL;
    AWPINT i,ii;

    res = AWP_OK;/*initialize result */
	if (Image->dwType == AWP_BYTE && Image->bChannels == 1)
		return res;
    if(Image->dwType!=AWP_BYTE || Image->bChannels!=3){
	res = AWP_NOTSUPPORT;
        _ERR_EXIT_
    }

    lSize=Image->sSizeX*Image->sSizeY;

	pBSrc=(AWPBYTE*)Image->pPixels;

    /* allocate memory for AWPDOUBLE data */
    pB = (AWPBYTE*)malloc(sizeof(AWPBYTE)*lSize);
    if (pB == NULL)
    {
	res = AWP_BADMEMORY;
        _ERR_EXIT_
    }

    for(i=0;i<lSize;i++){
	ii=3*i;
	pB[i]=(pBSrc[ii]+pBSrc[ii+1]+pBSrc[ii+2])/3;
    }
    
    free(pBSrc);
    
    Image->bChannels=1;
    Image->pPixels=pB;
    
CLEANUP:

    return res;
    
}

AWPRESULT _awpConvertAWPAWPDOUBLEToByte(awpImage* Image)
{
    AWPRESULT res;
    AWPDOUBLE* pD;
    AWPINT i;
    AWPBYTE* pB;
    long lSize;

    res = AWP_OK;/*initialize result */

    lSize=Image->sSizeX*Image->sSizeY*Image->bChannels;

    /* allocate memory for byte data */
	pB = (AWPBYTE*)malloc(sizeof(AWPBYTE)*lSize);

    if(pB==NULL){
        res = AWP_BADMEMORY;
        _ERR_EXIT_
    }

    pD=(AWPDOUBLE*)Image->pPixels;

    for(i=0;i<lSize;i++){
        pB[i]=(AWPBYTE)(pD[i]<0)?0:(pD[i]>255?255:(AWPBYTE)pD[i]);
    }

    free((void*)pD);
    Image->dwType=AWP_BYTE;
    Image->pPixels=(void*)pB;

CLEANUP:

    return res;
}
AWPRESULT _awpConvertFloatToByte(awpImage* Image)
{
    AWPRESULT res;
    AWPFLOAT* pD;
    AWPINT i;
    AWPBYTE* pB;
    long lSize;

    res = AWP_OK;/*initialize result */

    lSize=Image->sSizeX*Image->sSizeY*Image->bChannels;

    /* allocate memory for byte data */
    pB = (AWPBYTE*)malloc(sizeof(AWPBYTE)*lSize);

    if(pB==NULL){
        res = AWP_BADMEMORY;
        _ERR_EXIT_
	}

    pD=(AWPFLOAT*)Image->pPixels;

    for(i=0;i<lSize;i++){
        pB[i]=(AWPBYTE)(pD[i]<0)?0:(pD[i]>255?255:(AWPBYTE)pD[i]);
    }

    free((void*)pD);
    Image->dwType=AWP_BYTE;
    Image->pPixels=(void*)pB;

CLEANUP:

    return res;
}


AWPRESULT _awpConvertAWPAWPDOUBLEToByteWithNorm(awpImage* Image)
{
    AWPRESULT res;
    AWPDOUBLE* pD;
    AWPINT i;
    AWPBYTE ch;
    AWPBYTE* pB;
    long lSize;
    AWPDOUBLE* pMax,*pMin;

    res = AWP_OK;/*initialize result */
    pMax=NULL;pMin=NULL;

    lSize=Image->sSizeX*Image->sSizeY*Image->bChannels;

    /* allocate memory for byte data */
    pB = (AWPBYTE*)malloc(sizeof(AWPBYTE)*lSize);

    if(pB == NULL){
        res = AWP_BADMEMORY;
        _ERR_EXIT_
	}

    /* allocate memory for min and max  data */
    pMax = (AWPDOUBLE*)malloc(sizeof(AWPDOUBLE)*Image->bChannels);
    if(pMax == NULL){
        res = AWP_BADMEMORY;
        _ERR_EXIT_
    }

    pMin = (AWPDOUBLE*)malloc(sizeof(AWPDOUBLE)*Image->bChannels);
    if(pMin == NULL){
        res = AWP_BADMEMORY;
        _ERR_EXIT_
    }

    /* full max and min data default value */
    for(ch=0;ch<Image->bChannels;ch++){
        pMax[ch]=-1e30;
        pMin[ch]=1e30;
    }

    pD=(AWPDOUBLE*)Image->pPixels;

    /* found min and max in image data */
    for(i=0;i<lSize;i+=Image->bChannels){
        for(ch=0;ch<Image->bChannels;ch++){
            if(pMax[ch]<pD[i+ch]){
                pMax[ch]=pD[i+ch];
            }
            if(pMin[ch]>pD[i+ch]){
                pMin[ch]=pD[i+ch];
            }
        }
    }

    /* normalize data */
    for(i=0;i<lSize;i+=Image->bChannels){
        for(ch=0;ch<Image->bChannels;ch++){
            pB[i+ch]=(AWPBYTE)(255.0*(pD[i+ch]-pMin[ch])/(pMax[ch]-pMin[ch]+0.0001));
		}
    }

    free((void*)pD);
    Image->dwType=AWP_BYTE;
    Image->pPixels=(void*)pB;

CLEANUP:
    if(pMax!=NULL){
	free(pMax);
    }

    if(pMin!=NULL){
	free(pMin);
    }

    return res;
}


AWPRESULT _awpConvertFloatToByteWithNorm(awpImage* Image)
{
	AWPRESULT res;
	AWPFLOAT* pD;
	AWPINT i;
	AWPBYTE ch;
	AWPBYTE* pB;
	long lSize;
	AWPFLOAT* pMax,*pMin;

	res = AWP_OK;/*initialize result */
	pMax=NULL;pMin=NULL;

	lSize=Image->sSizeX*Image->sSizeY*Image->bChannels;

	/* allocate memory for byte data */
	pB = (AWPBYTE*)malloc(sizeof(AWPBYTE)*lSize);

	if(pB == NULL){
		res = AWP_BADMEMORY;
		_ERR_EXIT_
	}

	/* allocate memory for min and max  data */
	pMax = (AWPFLOAT*)malloc(sizeof(AWPFLOAT)*Image->bChannels);
	if(pMax == NULL){
		res = AWP_BADMEMORY;
		_ERR_EXIT_
	}

	pMin = (AWPFLOAT*)malloc(sizeof(AWPFLOAT)*Image->bChannels);
	if(pMin == NULL){
        res = AWP_BADMEMORY;
        _ERR_EXIT_
    }

    /* full max and min data default value */
    for(ch=0;ch<Image->bChannels;ch++){
        pMax[ch]=-1e-30f;
        pMin[ch]=1e30f;
    }

    pD=(AWPFLOAT*)Image->pPixels;

    /* found min and max in image data */
    for(i=0;i<lSize;i+=Image->bChannels){
        for(ch=0;ch<Image->bChannels;ch++){
            if(pMax[ch]<pD[i+ch]){
                pMax[ch]=pD[i+ch];
            }
            if(pMin[ch]>pD[i+ch]){
                pMin[ch]=pD[i+ch];
            }
        }
    }

    /* normalize data */
    for(i=0;i<lSize;i+=Image->bChannels){
		for(ch=0;ch<Image->bChannels;ch++){
            pB[i+ch]=(AWPBYTE)(255.0*(pD[i+ch]-pMin[ch])/(pMax[ch]-pMin[ch]+0.0000000000001));
        }
    }

    free((void*)pD);
    Image->dwType=AWP_BYTE;
    Image->pPixels=(void*)pB;

CLEANUP:
    if(pMax!=NULL){
	free(pMax);
    }

    if(pMin!=NULL){
	free(pMin);
    }

    return res;
}

AWPRESULT awpConvert(awpImage* Image, long lOptions)
{
    AWPRESULT res;

    res = AWP_OK;/*initialize result */

    /* check image */
	_CHECK_RESULT_((res = awpCheckImage(Image)))

    /* select type image */
    switch(Image->dwType){
    case AWP_BYTE:
        switch(lOptions)
		{
		case AWP_CONVERT_TO_DOUBLE:
            _CHECK_RESULT_((res=_awpConvertByteToAWPAWPDOUBLE(Image)))
            break;
		case AWP_CONVERT_3TO1_BYTE:
			_CHECK_RESULT_((res=_awpConvertByte3To1(Image)))
			break;
        case AWP_CONVERT_TO_FLOAT:
            _CHECK_RESULT_((res=_awpConvertByteToFloat(Image)))
            break;
        default:
            res = AWP_NOTSUPPORT;
            _ERR_EXIT_
        }
        break;

    case AWP_DOUBLE:
        switch(lOptions)
		{
        case AWP_CONVERT_TO_BYTE:
            _CHECK_RESULT_((res=_awpConvertAWPAWPDOUBLEToByte(Image)))
            break;
        case AWP_CONVERT_TO_BYTE_WITH_NORM:
            _CHECK_RESULT_((res=_awpConvertAWPAWPDOUBLEToByteWithNorm(Image)))
			break;
        case AWP_CONVERT_TO_FLOAT:
			_CHECK_RESULT_((res=_awpConvertDoubleToFloat(Image)))
			break;
        default:
            res = AWP_NOTSUPPORT;
            _ERR_EXIT_
        }
        break;
    case AWP_FLOAT:
        switch(lOptions)
        {
            case AWP_CONVERT_TO_BYTE:
				_CHECK_RESULT_((res=_awpConvertFloatToByte(Image)))
			break;
			case AWP_CONVERT_TO_BYTE_WITH_NORM:
				_CHECK_RESULT_((res=_awpConvertFloatToByteWithNorm(Image)))
			break;
			case AWP_CONVERT_TO_DOUBLE:
				_CHECK_RESULT_((res=_awpConvertFloatToDouble(Image)))
			break;
		default:
			res = AWP_NOTSUPPORT;
            _ERR_EXIT_
        }
    break;
    default:
        res = AWP_NOTSUPPORT;
        _ERR_EXIT_
    }

CLEANUP:
    return res;
}


// converts Image of type AWP_BYTE with HSV color space to ppProb of AWP_GRAYSCALE
// Histogramm - normal awpHistogramm
AWPRESULT awpBackProjection2D(awpImage* Image, awpImage** ppProb, awpImage* pPreset, AWPBYTE min_v, AWPBYTE max_v)
{
    AWPRESULT res;
    /*pointers to the pixels*/
    AWPDOUBLE*         dblPresetPix;
    awpColor*       cPix;
    AWPBYTE*           bPix;
    AWPBYTE            h,s,v;
    AWPINT i;
    AWPDOUBLE          max_preset;
    AWPDOUBLE          min_preset;
    res = AWP_OK;/*initialize result */

    if (ppProb == NULL)
    {
        res = AWP_BADARG;
        _ERR_EXIT_
    }

    _CHECK_RESULT_((res = awpCheckImage(Image)))
    _CHECK_RESULT_((res = awpCheckImage(pPreset)))

    /*Image must have 3 bytes per point
     To get the ppProb image, use the
     two channels Image*/
    if (Image->dwType != AWP_BYTE && Image->bChannels != 3)
    {
        res = AWP_BADARG;
        _ERR_EXIT_
    }

    /*pPreset should be of type AWP_DOUBLE, contain
     one data channel and be normalized to one, the dimensions
     pPreset images should be 256 x 256*/
    if (pPreset->dwType != AWP_DOUBLE || pPreset->bChannels != 1
    && pPreset->sSizeX != 256 || pPreset->sSizeY != 256)
    {
        res = AWP_BADARG;
        _ERR_EXIT_
    }

    /*we get pointers to pixels of the image pPreset*/
    dblPresetPix = (AWPDOUBLE*)pPreset->pPixels;
    max_preset = dblPresetPix[0];
    min_preset = dblPresetPix[0];
    for (i = 1; i < pPreset->sSizeX*pPreset->sSizeY; i++)
    {
        if (dblPresetPix[i] > max_preset)
            max_preset = dblPresetPix[i];
        if (dblPresetPix[i] < min_preset)
            min_preset = dblPresetPix[i];
    }
	if (max_preset - min_preset == 0)
    {
        /*The histogram must be non-zero*/
        res = AWP_BADARG;
        _ERR_EXIT_

    }
    /*create an image for BackProjection*/
    _CHECK_RESULT_((res = awpCreateImage(ppProb, Image->sSizeX, Image->sSizeY, 1, AWP_BYTE)))

    cPix = (awpColor*)Image->pPixels;
    bPix = (AWPBYTE*)(*ppProb)->pPixels;

    for (i = 0; i < Image->sSizeX*Image->sSizeY;i++)
    {
       h = cPix[i].bRed;
       s = cPix[i].bGreen;
       v = cPix[i].bBlue;
       if (v >= min_v && v <= max_v)
	       bPix[i] = (AWPBYTE)(255*((max_preset - dblPresetPix[s * 256 + h]) / (max_preset - min_preset)));
		//   bPix[i] = (AWPBYTE)(255*dblPresetPix[s * 256 + h]/max_preset);//(1-(max_preset - dblPresetPix[s * 256 + h]) / (max_preset - min_preset)));
	   else
           bPix[i] = 0;
    }
CLEANUP:
    return res;
}

#ifdef WIN32
/*

*/
AWPRESULT awpImageToDIB( const awpImage* pImage, BITMAPINFO* pBMI, unsigned char** pBMData, AWPBOOL replace )
{
	AWPBYTE *dib_pixels  = NULL;
	AWPBYTE *awp_row     = NULL;
	AWPBYTE *dib_row     = NULL;
	AWPBYTE *tmp_row, *top_row, *bottom_row;
	AWPINT row, col, dib_width, awp_width;
    AWPINT ind_awp, ind_bm;

	if (pBMI == NULL)
		return AWP_BADARG;

	if (awpCheckImage(pImage) != AWP_OK)
		return AWP_BADARG;

	if (pImage->dwType != AWP_BYTE)
		return AWP_BADARG;

	if (pImage->bChannels != 3 && pImage->bChannels != 1)
		return AWP_BADARG;

	pBMI->bmiHeader.biSize          = sizeof(BITMAPINFOHEADER);
	pBMI->bmiHeader.biWidth         = pImage->sSizeX;
	pBMI->bmiHeader.biHeight        = pImage->sSizeY;
	pBMI->bmiHeader.biPlanes        = 1;
        pBMI->bmiHeader.biBitCount      = 24;
	pBMI->bmiHeader.biCompression   = BI_RGB;
	pBMI->bmiHeader.biSizeImage     = 0;
	pBMI->bmiHeader.biXPelsPerMeter = 0;
	pBMI->bmiHeader.biYPelsPerMeter = 0;
	pBMI->bmiHeader.biClrUsed       = 0;
	pBMI->bmiHeader.biClrImportant  = 0;

	awp_width  = 3*pImage->sSizeX;
    dib_width  = ((pImage->sSizeX*24 + 31)/32) * 4; /* aligned to 4 bytes */
    if (replace)
        dib_pixels = (AWPBYTE*)*pBMData;
    else
        dib_pixels = (AWPBYTE *)malloc(dib_width * pImage->sSizeY * sizeof(AWPBYTE));

    if (dib_pixels == NULL)
      	return AWP_NOTENOUGH_MEM;
    memset(dib_pixels, 0, dib_width*pImage->sSizeY*sizeof(AWPBYTE));

    switch (pImage->bChannels)
    {
    case 1: /* Grayscale */
  	    for (row=0; row<pImage->sSizeY; row++)
		    for (col=0; col<pImage->sSizeX; col++)
		    {
			    ind_bm  = row*dib_width + 3*col;
				ind_awp = (pImage->sSizeY - 1 - row)*pImage->sSizeX + col;

     			dib_pixels[ind_bm]     = ((AWPBYTE *)pImage->pPixels)[ind_awp];
	    		dib_pixels[ind_bm + 1] = dib_pixels[ind_bm];
		    	dib_pixels[ind_bm + 2] = dib_pixels[ind_bm];
			}
        break;
    case 3: /* Color */
        /* AWP width aligned to 4 bytes - simple copy pixels */
    	if (pImage->sSizeX % 4 == 0)
		    memcpy(dib_pixels, pImage->pPixels, awp_width*pImage->sSizeY*sizeof(AWPBYTE));
    	else
	   	    for (row=0; row<pImage->sSizeY; row++)
			{
				dib_row = dib_pixels + row*dib_width;
				awp_row = (AWPBYTE *)pImage->pPixels + row*awp_width;
				memcpy(dib_row, awp_row, awp_width*sizeof(AWPBYTE));
			}

        /* Flip DIB image around horizontal axis */
	    tmp_row = (AWPBYTE*)malloc(dib_width*sizeof(AWPBYTE));
    	if (tmp_row == NULL)
	    {
    		free(dib_pixels);
	    	return AWP_NOTENOUGH_MEM;
    	}

    	bottom_row = dib_pixels;
	    top_row    = bottom_row + (pImage->sSizeY - 1)*dib_width;
    	for (row=0; row<pImage->sSizeY/2; row++)
	    {
		    memcpy(tmp_row, top_row, dib_width*sizeof(AWPBYTE));
    		memcpy(top_row, bottom_row, dib_width*sizeof(AWPBYTE));
	    	memcpy(bottom_row, tmp_row, dib_width*sizeof(AWPBYTE));

		    bottom_row += dib_width*sizeof(AWPBYTE);
    		top_row    -= dib_width*sizeof(AWPBYTE);
	    }
        free(tmp_row);
        break;
	}
    if (!replace)
    	*pBMData = dib_pixels;

	return AWP_OK;
}
/*
    Windows DIB to awpImage convertor
*/

static AWPBOOL _awp24DibToAwp(const BITMAPINFO* pBMI, const void* pBMData, awpImage** ppImage)
{
	awpImage *pImage  = NULL;
	AWPBYTE  *pBytes      = (AWPBYTE *)pBMData;
	AWPBYTE  *dib_row, *awp_row;
	WORD     awp_width, awp_height, dib_width;
    AWPRESULT res;
	WORD     row;

	awp_width  = (WORD)pBMI->bmiHeader.biWidth;
	awp_height = (WORD)pBMI->bmiHeader.biHeight;
	dib_width  = ((awp_width*24 + 31)/32 ) * 4;

	res = awpCreateImage(&pImage, awp_width, awp_height, 3, AWP_BYTE);
	if (res != AWP_OK)
		return FALSE;

	if (awp_width % 4 == 0)
	{
		memcpy(pImage->pPixels, pBytes, 3*pImage->sSizeX*pImage->sSizeY*sizeof(AWPBYTE));
	}
	else
		for (row=0; row<pImage->sSizeY; row++)
		{
			dib_row = pBytes + row*dib_width;
			awp_row = (AWPBYTE *)pImage->pPixels + 3*row*awp_width;
			memcpy(awp_row, dib_row, 3*awp_width*sizeof(AWPBYTE));
		}
	awpFlip(&pImage, AWP_FLIP_VERT);

	*ppImage = pImage;

    return TRUE;
}

static AWPBOOL _awp32DibToAwp(const BITMAPINFO* pBMI, const void* pBMData, awpImage** ppImage )
{
    awpImage *pImage  = NULL;
    AWPBYTE  *pBytes      = (AWPBYTE *)pBMData;
    AWPBYTE  *awp_row, *dib_row;
    AWPWORD  w,h;
    WORD     awp_width, awp_height, dib_width;
//    WORD     row;
    DWORD    *dib_dw, d, i ,j;

	awp_width  = (WORD)pBMI->bmiHeader.biWidth;
	awp_height = (WORD)pBMI->bmiHeader.biHeight;
	dib_width  = (WORD)pBMI->bmiHeader.biWidth*4;

	if (awpCreateImage(&pImage, awp_width, awp_height, 3, AWP_BYTE) != AWP_OK)
		return FALSE;


    if (pBMI->bmiHeader.biCompression == BI_RGB)
    {
        //todo: decompress 
        awp_row = (AWPBYTE*)pImage->pPixels;
        dib_dw  = (DWORD*)pBMData;
        i=0;
        for (h = 0; h < awp_height; h++)
        {
            for (w = 0; w < awp_width; w++)
            {
                j = 3*h*awp_width + w*3;
                d = dib_dw[i];
                awp_row[j + 0] = d & 0x000000FF;
                awp_row[j + 1] = (d & 0x0000FF00) >> 8;
                awp_row[j + 2] = (d & 0x00FF0000) >> 16;
                i++;
            }
        }
    }
    else if (pBMI->bmiHeader.biCompression == BI_BITFIELDS)
    {
        // decompress
        awp_row = (AWPBYTE*)pImage->pPixels;
        dib_row = (AWPBYTE*)pBMData;
        for (h = 0; h < awp_height; h++)
        {
            for (w = 0; w < awp_width; w++)
            {
                i = h*dib_width + w*4;
                j = 3*h*awp_width + w*3;

                awp_row[j + 0] = dib_row[i + 0];
                awp_row[j + 1] = dib_row[i + 1];
                awp_row[j + 2] = dib_row[i + 2];

            }
        }
    }
    else
        return FALSE;
 	awpFlip(&pImage, AWP_FLIP_VERT);

	*ppImage = pImage;
    return TRUE;
}

AWPRESULT awpDIBToImage( const BITMAPINFO* pBMI, const void* pBMData, awpImage** ppImage )
{
	if (pBMI == NULL)
		return AWP_BADARG;

	if (pBMI->bmiHeader.biSize != sizeof(BITMAPINFOHEADER))
		return AWP_BADARG;

	if (pBMI->bmiHeader.biBitCount == 24)
    {
        if (_awp24DibToAwp(pBMI, pBMData, ppImage))
            return AWP_OK;
    }
    else if (pBMI->bmiHeader.biBitCount == 32)
    {
        if (_awp32DibToAwp(pBMI, pBMData, ppImage))
            return AWP_OK;
    }
	return AWP_BADARG;
}
#endif //WIN32

AWPRESULT _awpConvertByteToShort(awpImage* Image)
{
    AWPRESULT res;
    AWPSHORT* pS;
    AWPINT i;
    AWPBYTE* pB;
    long lSize;

    res = AWP_OK;/*initialize result */

    lSize = Image->sSizeX * Image->sSizeY * Image->bChannels;

    /* allocate memory forAWPSHORT data */
    pS = (AWPSHORT*)malloc(sizeof(AWPSHORT) * lSize);

    if(pS == NULL)
	{
        res = AWP_BADMEMORY;
        _ERR_EXIT_
    }

    pB = (AWPBYTE*)Image->pPixels;

    for(i = 0; i < lSize; i++)
	{
        pS[i] = pB[i];
    }

    free((void*)pB);
    Image->dwType = AWP_SHORT;
    Image->pPixels = (void*)pS;

CLEANUP:
    return res;
}

AWPRESULT _awpConvertDoubleToShort(awpImage* Image)
{
    AWPRESULT res;
    AWPSHORT* pS;
    AWPINT i;
    AWPDOUBLE* pD;
    long lSize;

    res = AWP_OK;/*initialize result */

    lSize = Image->sSizeX * Image->sSizeY * Image->bChannels;

    /* allocate memory forAWPSHORT data */
    pS = (AWPSHORT*)malloc(sizeof(AWPSHORT) * lSize);

    if(pS == NULL)
	{
        res = AWP_BADMEMORY;
        _ERR_EXIT_
    }

    pD = (AWPDOUBLE*)Image->pPixels;

    for(i = 0; i < lSize; i++)
	{
		pS[i] = (pD[i]<SHRT_MIN)?SHRT_MIN:(pD[i]>SHRT_MAX?SHRT_MAX:(AWPSHORT)pD[i]);
    }

    free((void*)pD);
    Image->dwType = AWP_SHORT;
    Image->pPixels = (void*)pS;

CLEANUP:
    return res;
}

AWPRESULT _awpConvertShortToByte(awpImage* Image)
{
    AWPRESULT res;
    AWPBYTE* pB;
    AWPINT i;
    AWPSHORT* pS;
    long lSize;

    res = AWP_OK;/*initialize result */

    lSize = Image->sSizeX * Image->sSizeY * Image->bChannels;

    /* allocate memory for byte data */
    pB = (AWPBYTE*)malloc(sizeof(AWPBYTE) * lSize);

    if(pB == NULL)
	{
        res = AWP_BADMEMORY;
        _ERR_EXIT_
    }

    pS = (AWPSHORT*)Image->pPixels;

    for(i = 0; i < lSize; i++)
	{
		pB[i] = (pS[i]<0)?0:(pS[i]>255?255:pS[i]);
    }

    free((void*)pS);
    Image->dwType = AWP_BYTE;
    Image->pPixels = (void*)pB;

CLEANUP:
    return res;
}

AWPRESULT _awpConvertShortToByteWithNorm(awpImage* Image)
{
    AWPRESULT res;
    AWPBYTE* pB;
    AWPINT i;
	AWPBYTE ch;
    AWPSHORT* pS;
    long lSize;
	AWPSHORT* pMax = NULL, *pMin = NULL;

    res = AWP_OK;/*initialize result */

    lSize = Image->sSizeX * Image->sSizeY * Image->bChannels;

    /* allocate memory for byte data */
    pB = (AWPBYTE*)malloc(sizeof(AWPBYTE) * lSize);

    if(pB == NULL)
	{
        res = AWP_BADMEMORY;
        _ERR_EXIT_
    }

    /* allocate memory for min and max  data */
    pMax = (AWPSHORT*)malloc(sizeof(AWPSHORT) * Image->bChannels);
    if(pMax == NULL)
	{
        res = AWP_BADMEMORY;
        _ERR_EXIT_
    }

    pMin = (AWPSHORT*)malloc(sizeof(AWPSHORT) * Image->bChannels);
    if(pMin == NULL)
	{
        res = AWP_BADMEMORY;
        _ERR_EXIT_
    }

    /* full max and min data default value */
    for(ch = 0; ch < Image->bChannels; ch++)
	{
        pMax[ch] = SHRT_MIN;
        pMin[ch] = SHRT_MAX;
    }

    pS = (AWPSHORT*)Image->pPixels;

    /* found min and max in image data */
    for(i=0;i<lSize;i+=Image->bChannels){
        for(ch=0;ch<Image->bChannels;ch++){
            if(pMax[ch]<pS[i+ch]){
                pMax[ch]=pS[i+ch];
            }
            if(pMin[ch]>pS[i+ch]){
                pMin[ch]=pS[i+ch];
            }
        }
    }

    /* normalize data */
    for(i=0;i<lSize;i+=Image->bChannels){
        for(ch=0;ch<Image->bChannels;ch++){
            pB[i+ch]=(AWPBYTE)(255.0*(pS[i+ch]-pMin[ch])/(pMax[ch]-pMin[ch]+0.0001));
        }
    }

    free((void*)pS);
    Image->dwType = AWP_BYTE;
    Image->pPixels = (void*)pB;

CLEANUP:
    if(pMax!=NULL){
	free(pMax);
    }
    
    if(pMin!=NULL){
	free(pMin);
    }

    return res;
}

AWPRESULT _awpConvertShortToAWPAWPDOUBLE(awpImage* Image)
{
    AWPRESULT res;
    AWPDOUBLE* pD;
    AWPINT i;
    AWPSHORT* pS;
    long lSize;

    res = AWP_OK;/*initialize result */

    lSize = Image->sSizeX * Image->sSizeY * Image->bChannels;

    /* allocate memory for AWPDOUBLE data */
    pD = (AWPDOUBLE*)malloc(sizeof(AWPDOUBLE) * lSize);

    if(pD == NULL)
	{
        res = AWP_BADMEMORY;
        _ERR_EXIT_
    }

    pS = (AWPSHORT*)Image->pPixels;

    for(i = 0; i < lSize; i++)
	{
        pD[i] = pS[i];
    }

    free((void*)pS);
    Image->dwType = AWP_DOUBLE;
    Image->pPixels = (void*)pD;

CLEANUP:
    return res;
}

AWPRESULT awpConvertV2(awpImage* Image, long lOptions)
{
    AWPRESULT res;

    res = AWP_OK;/*initialize result */

    /* check image */
    _CHECK_RESULT_((res = awpCheckImage(Image)))

    /* select type image */
    switch(Image->dwType){
    case AWP_BYTE:
        switch(lOptions){
		case AWP_CONVERT_TO_SHORT:
			_CHECK_RESULT_((res = _awpConvertByteToShort(Image)))
			break;
        case AWP_CONVERT_TO_DOUBLE:
//			_CHECK_RESULT_((res=_awpConvertByteToAWPAWPDOUBLE(Image)))
            break;
		case AWP_CONVERT_3TO1_BYTE:
//			_CHECK_RESULT_((res=_awpConvertByte3To1(Image)))
			break;
        default:
            res = AWP_NOTSUPPORT;
            _ERR_EXIT_
        }
        break;
    case AWP_DOUBLE:
        switch(lOptions){
        case AWP_CONVERT_TO_BYTE:
//			_CHECK_RESULT_((res=_awpConvertAWPAWPDOUBLEToByte(Image)))
            break;
		case AWP_CONVERT_TO_SHORT:
			_CHECK_RESULT_((res = _awpConvertDoubleToShort(Image)))
			break;
        case AWP_CONVERT_TO_BYTE_WITH_NORM:
//			_CHECK_RESULT_((res=_awpConvertAWPAWPDOUBLEToByteWithNorm(Image)))
            break;
        default:
            res = AWP_NOTSUPPORT;
            _ERR_EXIT_
        }
        break;
	case AWP_SHORT:
		switch(lOptions){
		case AWP_CONVERT_TO_BYTE:
			_CHECK_RESULT_((res = _awpConvertShortToByte(Image)))
			break;
		case AWP_CONVERT_TO_BYTE_WITH_NORM:
			_CHECK_RESULT_((res = _awpConvertShortToByteWithNorm(Image)))
			break;
		case AWP_CONVERT_TO_DOUBLE:
			_CHECK_RESULT_((res = _awpConvertShortToAWPAWPDOUBLE(Image)))
			break;
		default:
			res = AWP_NOTSUPPORT;
			_ERR_EXIT_
		}
		break;
    default:
        res = AWP_NOTSUPPORT;
        _ERR_EXIT_
    }

CLEANUP:
    return res;
}

