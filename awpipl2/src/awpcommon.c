/*M
//
//      awpipl2 image processing and image analysis library
//		File: awpcommon.c
//		Purpose: awpipl2 commen functions 
//		Autors: Alex A.Telnykh (tlnykha@yahoo.com)
//      CopyRight 2004-2018 (c) NN-Videolab.net
//M*/
#include "_awpipl.h"


AWPRESULT awpGetVersion(AWPINT* pVersion)
{
    if (pVersion == NULL)
        return AWP_BADARG;
    *pVersion = AWP_VERSION;
    return AWP_OK;
}


/*chack image*/
AWPRESULT awpCheckImage(const awpImage* pImage)
{
    /*check pointer*/
    if (pImage == NULL)
        return AWP_BADMEMORY;
    /*check magic number*/
    if (pImage->nMagic != AWP_MAGIC2)
        return AWP_NOTAWPIMAGE;
    /*check channles*/
    if (pImage->bChannels == 0)
        return AWP_BADIMAGE_FORMAT;
    /*check sizes*/
    if (pImage->sSizeX == 0 ||pImage->sSizeY == 0)
        return AWP_BADSIZES;
    /*check format*/
    if (pImage->dwType != AWP_BYTE &&
            pImage->dwType != AWP_SHORT		&&
            pImage->dwType != AWP_FLOAT 	&&
            pImage->dwType != AWP_DOUBLE )
        return AWP_BADIMAGE_FORMAT;
    /*check pixels*/
    if (pImage->pPixels == NULL)
        return AWP_BADMEMORY;

    return AWP_OK;

}

AWPRESULT awpCheckRect(const awpRect* pRect)
{
    /* check pointer*/
    if(pRect==NULL)
        return AWP_BADMEMORY;
    /* check coordinate rect*/
    if(pRect->left>=pRect->right){
        return AWP_BADAWPRECT;
    }
    if(pRect->top>=pRect->bottom){
        return AWP_BADAWPRECT;
    }
    return AWP_OK;
}

/*create image*/
AWPRESULT awpCreateImage(awpImage** ppImage,AWPWORD width,AWPWORD height,AWPBYTE channels, AWPDWORD dwType)
{
    awpImage* pImage;
    AWPINT size;
    if( ppImage == NULL )
        return AWP_BADARG;
    if (channels == 0 || width == 0 || height == 0)
        return AWP_BADARG;
    *ppImage = NULL;
    /*todo: this code is subject to change
    dwType may be not only just a set of channels....*/	
    switch(dwType)
    {
    case AWP_BYTE: size = channels*sizeof(AWPBYTE)*width*height;break;
    case AWP_SHORT:size = channels*sizeof(AWPSHORT)*width*height;break;
    case AWP_FLOAT:size = channels*sizeof(AWPFLOAT)*width*height;break;
    case AWP_DOUBLE:size = channels*sizeof(AWPDOUBLE)*width*height;break;
    default:
        return AWP_BADARG;
    }
    /*alloc memory for header*/
    pImage = NULL;
    pImage = (awpImage*)malloc(sizeof(awpImage));
    if (pImage == NULL)
        return 	AWP_BADMEMORY;

    pImage->nMagic = AWP_MAGIC2;
    pImage->bChannels = channels;
    pImage->sSizeX  = width;
    pImage->sSizeY  = height;
    pImage->dwType  = dwType;
    pImage->pPixels = NULL;

    pImage->pPixels = malloc(size);
    if (pImage->pPixels == NULL)
    {
        free(pImage);
        return 	AWP_BADMEMORY;
    }
    /*fill memory*/
    memset((AWPBYTE*)pImage->pPixels,0,size);
    *ppImage = pImage;
    return AWP_OK;
}

AWPRESULT awpCreateGrayImage(awpImage** ppImage, AWPWORD width, AWPWORD height, AWPBYTE channels, AWPBYTE* data)
{
	AWPRESULT res = AWP_OK;
	awpImage* pImage = NULL;
	AWPINT size = 0;
	AWPINT i = 0; 
	AWPINT k = 0;
	AWPINT c = 0;
	AWPINT sum = 0;
	AWPINT bpp = 0;
	AWPBYTE* pix = NULL;
	if (ppImage == NULL)
		return AWP_BADARG;
	if (channels == 0 || width == 0 || height == 0 || data == NULL)
		return AWP_BADARG;
	*ppImage = NULL;
	res = awpCreateImage(ppImage, width, height, 1, AWP_BYTE);
	if (res != AWP_OK)
		return res;
	pImage = *ppImage;
	pix = (AWPBYTE*)pImage->pPixels;


	bpp = channels > 3 ? 3 : channels;
	for (i = 0; i < width*height*channels; i+= channels)
	{
		sum = 0;
		for (k = 0; k < bpp; k++)
			sum += data[i + k];
		sum /= bpp;
		pix[c] = (AWPBYTE)sum;
		c++;
	}

	return res;
}

AWPRESULT awpCreateMultiImage(awpImage** ppImage, AWPWORD width, AWPWORD height, AWPBYTE channels, AWPBYTE bpp, AWPBYTE* data)
{
	AWPRESULT res = AWP_OK;
	awpImage* pImage = NULL;
	AWPINT size = 0;
	AWPINT i = 0;
	AWPINT k = 0;
	AWPINT c = 0;
	AWPINT sum = 0;
	AWPBYTE* pix = NULL;
	if (ppImage == NULL)
		return AWP_BADARG;
	if (channels == 0 || width == 0 || height == 0 || data == NULL)
		return AWP_BADARG;
	if (channels > bpp)
		return AWP_BADARG;

	*ppImage = NULL;
	res = awpCreateImage(ppImage, width, height, channels, AWP_BYTE);
	if (res != AWP_OK)
		return res;
	pImage = *ppImage;
	pix = (AWPBYTE*)pImage->pPixels;

	for (i = 0; i < width*height*bpp; i += bpp)
	{
		for (k = 0; k < channels; k++)
		{
			pix[c] = data[i + k];
			c++;

		}
	}

	return res;

}



/*release image*/
AWPRESULT awpReleaseImage(awpImage** ppImage)
{
    AWPRESULT res;
    res = AWP_OK;
    
    if(*ppImage==NULL) return AWP_OK;
    
    res = awpCheckImage(*ppImage);
    if (res != AWP_OK)
        return res;

    /*remove pixels*/
    free((*ppImage)->pPixels);
    free(*ppImage);
    *ppImage = NULL;
    return AWP_OK;
}

AWPRESULT awpGetImagePixelType(const awpImage* pImage, AWPINT* pType)
{
    AWPRESULT res;
    _CHECK_RESULT_((res = awpCheckImage(pImage)))

    /*todo: this code is subject to change
    the field "image type" may contain and some additional information*/
    switch(pImage->dwType)
    {
    case AWP_BYTE:	*pType = AWP_BYTE;	break;
    case AWP_SHORT:	*pType = AWP_SHORT;	break;
    case AWP_FLOAT:	*pType = AWP_FLOAT;	break;
    case AWP_DOUBLE:*pType = AWP_DOUBLE;break;
    default:
        {
            *pType = 0;
            res = AWP_BADARG;
            _ERR_EXIT_
        }
    }
    res = AWP_OK;
CLEANUP:
    return res;
}


/*
	returns size of image pixel array 
	total size of image = pSize + sizeof(awpImage)
*/
AWPRESULT awpGetImageSize(const awpImage* pImage, AWPINT* pSize)
{
    AWPRESULT res;
    AWPINT isize;
    AWPINT itype;
    /*init variables */
    res = AWP_OK;
    isize = 0;

    _CHECK_RESULT_((res = awpCheckImage(pImage)))

    isize = pImage->sSizeX*pImage->sSizeY*pImage->bChannels;

    _CHECK_RESULT_((res = awpGetImagePixelType(pImage, &itype)))

    switch(itype)
    {
    case AWP_BYTE: isize *= sizeof(AWPBYTE); break;
    case AWP_SHORT:isize *= sizeof(AWPSHORT); break;
    case AWP_FLOAT:isize *= sizeof(AWPFLOAT); break;
    case AWP_DOUBLE:isize *= sizeof(AWPDOUBLE); break;
    default:
        {
            res = AWP_BADARG;
            _ERR_EXIT_
        }
    }

    *pSize = isize;
    res = AWP_OK;
CLEANUP:
    return res;
}

AWPRESULT awpGetImageHeaderSize(const awpImage* pImage,AWPINT *iSize)
{
    AWPRESULT res;
    AWPINT isize;
    
    res=AWP_OK;
    isize=0;
    
    _CHECK_RESULT_((res = awpCheckImage(pImage)))
    
    isize+=sizeof(pImage->nMagic);
    isize+=sizeof(pImage->bChannels);
    isize+=sizeof(pImage->sSizeX);
    isize+=sizeof(pImage->sSizeY);
    isize+=sizeof(pImage->dwType);    
    
    *iSize=isize;

CLEANUP:
    return res;
}

AWPRESULT awpRectInImage(const awpImage* pImage,const awpRect* pRect)
{
    AWPRESULT res;
    res=AWP_OK;

    _CHECK_RESULT_(( res = awpCheckImage(pImage)))

    _CHECK_RESULT_(( res = awpCheckRect(pRect)))

    /* check rect coordinates in image */

    /* width */
	//было >=  не делался весь экран
    if(pRect->right>pImage->sSizeX){
        res=AWP_BADARG;
        _ERR_EXIT_
    }

    if(pRect->left < 0){
        res=AWP_BADARG;
        _ERR_EXIT_
    }
    /* height */
  //было >=  не делался весь экран
    if(pRect->bottom>pImage->sSizeY){
        res=AWP_BADARG;
        _ERR_EXIT_
    }

    if(pRect->top <0 )
    {
         res=AWP_BADARG;
        _ERR_EXIT_
    }

CLEANUP:
    return res;
}

AWPRESULT awpMinMax(const awpImage* pImage, AWPDOUBLE** min, AWPDOUBLE** max)
{
    AWPWORD i,j;
    AWPBYTE c;
    AWPINT k;
    AWPRESULT res;
    res=AWP_OK;

    _CHECK_RESULT_(( res = awpCheckImage(pImage)))

    if (min == NULL || max == NULL)
    {
        res = AWP_BADARG;
        _ERR_EXIT_
    }

    /*alloc memory for arrays*/
    *min = NULL;
    *max = NULL;

    *min = (AWPDOUBLE*)malloc(pImage->bChannels*sizeof(AWPDOUBLE));
    if (*min == NULL)
    {
        res = AWP_BADMEMORY;
        _ERR_EXIT_
    }
    *max = (AWPDOUBLE*)malloc(pImage->bChannels*sizeof(AWPDOUBLE));
    if (*max == NULL)
    {
        res = AWP_BADMEMORY;
        _SAFE_FREE_(*min)
        _ERR_EXIT_
    }
    /*setup the minmax*/
    for (c = 0; c < pImage->bChannels; c++)
    {
        switch(pImage->dwType)
        {
           case AWP_BYTE:
                (*min)[c] = ((AWPBYTE*)pImage->pPixels)[c];
                (*max)[c] = ((AWPBYTE*)pImage->pPixels)[c];
           break;
           case AWP_SHORT:
                (*min)[c] = ((AWPSHORT*)pImage->pPixels)[c];
                (*max)[c] = ((AWPSHORT*)pImage->pPixels)[c];
           break;
           case AWP_FLOAT:
                (*min)[c] = ((AWPFLOAT*)pImage->pPixels)[c];
                (*max)[c] = ((AWPFLOAT*)pImage->pPixels)[c];
           break;
           case AWP_DOUBLE:
                (*min)[c] = ((AWPDOUBLE*)pImage->pPixels)[c];
                (*max)[c] = ((AWPDOUBLE*)pImage->pPixels)[c];
           break;
        }
    }
    /*find the minmax*/
    k = 0;
    for(i = 0; i < pImage->sSizeY; i++)
    {
        for (j = 0; j < pImage->sSizeX; j++)
          {
              for (c = 0; c < pImage->bChannels; c++)
              {
                switch(pImage->dwType)
                {
                   case AWP_BYTE:
                        if ((*min)[c] > ((AWPBYTE*)pImage->pPixels)[k])
                            (*min)[c] = ((AWPBYTE*)pImage->pPixels)[k];
                        if ((*max)[c] < ((AWPBYTE*)pImage->pPixels)[k])
                           (*max)[c] = ((AWPBYTE*)pImage->pPixels)[k];
                   break;
                   case AWP_SHORT:
                        if ((*min)[c] > ((AWPSHORT*)pImage->pPixels)[k])
                            (*min)[c] = ((AWPSHORT*)pImage->pPixels)[k];
                        if ((*max)[c] < ((AWPSHORT*)pImage->pPixels)[k])
                            (*max)[c] = ((AWPSHORT*)pImage->pPixels)[k];
                   break;
                   case AWP_FLOAT:
                        if ((*min)[c] > ((AWPFLOAT*)pImage->pPixels)[k])
                            (*min)[c] = ((AWPFLOAT*)pImage->pPixels)[k];
                        if ((*max)[c] < ((AWPFLOAT*)pImage->pPixels)[k])
                            (*max)[c] = ((AWPFLOAT*)pImage->pPixels)[k];
                   break;
                   case AWP_DOUBLE:
                        if ((*min)[c] > ((AWPDOUBLE*)pImage->pPixels)[k])
                            (*min)[c] = ((AWPDOUBLE*)pImage->pPixels)[k];
                        if ((*max)[c] < ((AWPDOUBLE*)pImage->pPixels)[k])
                            (*max)[c] = ((AWPDOUBLE*)pImage->pPixels)[k];
                   break;
                }
                k++;
              }
          }
    }

CLEANUP:
    return res;
}

