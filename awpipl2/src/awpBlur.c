/*M
//
//      awpipl2 image processing and image analysis library
//		File: awpBlur.c
//		Purpose: Gaussian Blur Filter
//		Autors: Alex A.Telnykh (tlnykha@yahoo.com)
//      CopyRight 2004-2019 (c) NN-Videolab.net
//M*/

#include "_awpipl.h"
/*
  awpCreateGaussKernel - creates Gauss kernel as with sigma. sigma  < 50
  high_precision = true - creates AWPDOUBLE kernel
  else creates AWPFLOAT kernel.
*/
AWPRESULT awpCreateGaussKernel(awpImage** ppKernel, AWPDOUBLE sigma)
{
    AWPRESULT res = AWP_OK;
    AWPSHORT x,y,hw,xx,yy;
    AWPWORD w = 0;
    void* p = NULL;
    AWPDOUBLE* dp = NULL;
    AWPDOUBLE  falfa = 1;
    AWPDOUBLE  fbeta = 1;
    if (sigma <= 0)
        _ERROR_EXIT_RES_(AWP_BADARG)
    w = (AWPWORD)(4*sigma);
    if (w < 3) w = 3;
	w = w % 2 == 0?w+1:w;
    hw = w / 2;

    _CHECK_RESULT_(res = awpCreateImage(ppKernel, w,w,1,AWP_DOUBLE))
    p = (*ppKernel)->pPixels;
    dp = (AWPDOUBLE*)p;
    falfa = 1. /(2.*3.14159*sigma*sigma);
    fbeta = -1./(2.*sigma*sigma);
    for (y=0; y < w; y++)
    {
        yy = y - hw;
        for (x = 0; x < w; x++)
        {
            xx = x - hw;
            dp[y*w + x] = falfa*exp(fbeta*(xx*xx +yy*yy));
        }
    }

CLEANUP:
    return res;
}

/*
    awpGaussianBlur - performs Gaussian Blur operator over pSrc
    result of operator stores into pDst image. The size of Gaussian
    kernel is defined by sigma.
*/
AWPRESULT awpGaussianBlur(awpImage* pSrc, awpImage* pDst, AWPFLOAT sigma)
{
    AWPRESULT res = AWP_OK;
	awpImage* pKernel = NULL;
	if (sigma <= 0)
		_ERROR_EXIT_RES_(AWP_BADARG);

	_CHECK_RESULT_(res = awpCheckImage(pSrc))
    _CHECK_RESULT_(res = awpCheckImage(pDst))

    _CHECK_SAME_SIZES(pSrc, pDst)
    _CHECK_SAME_TYPE(pSrc,pDst)
    _CHECK_NUM_CHANNELS(pSrc, pDst)

    _CHECK_RESULT_(res = awpCreateGaussKernel(&pKernel, sigma))
    _CHECK_RESULT_(res = awpMaskConvolution(pSrc, pDst, pKernel))

CLEANUP:
	_SAFE_RELEASE_(pKernel);
    return res;
}
