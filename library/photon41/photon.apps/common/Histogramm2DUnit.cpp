//---------------------------------------------------------------------------

#pragma hdrstop
#include "Histogramm2DUnit.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

awpImage* Get2DHistogramm(awpImage* source)
{
	if (source == NULL || source->dwType != AWP_BYTE || source->bChannels != 3)
    	return NULL;

    awpImage* result = NULL;
    awpImage* hsv    = NULL;
    if (awpCreateImage(&result, 256, 256, 1, AWP_DOUBLE) != AWP_OK)
    {
		return NULL;
    }
    if (awpCreateImage(&hsv, source->sSizeX, source->sSizeY, 3, AWP_BYTE) != AWP_OK)
    {
		_AWP_SAFE_RELEASE_(result)
        return NULL;
    }
     awpRGBtoHSVImage(source, &hsv);
	 awpGet2DHistogramm(hsv, result, 0, 255, false);
	_AWP_SAFE_RELEASE_(hsv)
	return result;
}
awpImage* Get2DHistogrammMask(awpImage* source, awpImage* mask)
{
	if (source == NULL || source->dwType != AWP_BYTE || source->bChannels != 3)
    	return NULL;
	if (mask == NULL || mask->dwType != AWP_BYTE || mask->bChannels != 1)
    	return NULL;
    if (mask->sSizeX != source->sSizeX || mask->sSizeY != source->sSizeY)
    	return NULL;

    awpImage* img0 = NULL;
    if (awpCreateImage(&img0, mask->sSizeX, mask->sSizeY, 3, AWP_BYTE) != AWP_OK)
		return NULL;

    AWPBYTE* img_pix = _AWP_BPIX_(source, AWPBYTE)
    AWPBYTE* mask_pix = _AWP_BPIX_(mask, AWPBYTE)
    AWPBYTE* img0_pix = _AWP_BPIX_(img0, AWPBYTE)
   for (int i = 0; i < mask->sSizeX*mask->sSizeY; i++)
   {
        if (mask_pix[i] == 0)
        {
        	img0_pix[3*i] = 0;
        	img0_pix[3*i+1] = 0;
        	img0_pix[3*i +2] = 0;
        }
        else
        {
        	img0_pix[3*i] = img_pix[3*i];
        	img0_pix[3*i+1] = img_pix[3*i+1];
        	img0_pix[3*i +2] = img_pix[3*i+2];
        }
   }

	awpImage* result = Get2DHistogramm(img0);
	_AWP_SAFE_RELEASE_(img0)
    return result;
}

awpImage* Get2DHistogrammRect(awpImage* source, awpRect& rect)
{
	if (source == NULL || source->dwType != AWP_BYTE || source->bChannels != 3)
    	return NULL;
     awpImage* fragment = NULL;
     awpCopyRect(source, &fragment, &rect);
     if (fragment == NULL)
    	return NULL;
	awpImage* result = Get2DHistogramm(fragment);
	_AWP_SAFE_RELEASE_(fragment)
    return result;
}


awpImage* MakeMask(awpImage* source, awpImage* hst, double t, bool binary )
{
	if (source == NULL || source->dwType != AWP_BYTE || source->bChannels != 3)
    	return NULL;
    if (hst == NULL || hst->dwType != AWP_DOUBLE || hst->sSizeX != 256 || hst->sSizeX != hst->sSizeY)
    	return NULL;
    if (t < 0)
    	t = 0;

    awpImage* result = NULL;
    awpImage* hsv0 = NULL;
    awpImage* gauss = NULL;

    awpRGBtoHSVImage(source, &hsv0);

    awpBackProjection2D(hsv0, &result, hst, 32, 240);
    awpCreateImage(&gauss, result->sSizeX, result->sSizeY, 1, AWP_BYTE);
    awpGaussianBlur(result, gauss, 3.f);

    AWPBYTE* gauss_pix = _AWP_BPIX_(gauss, AWPBYTE)
    AWPBYTE* result_pix = _AWP_BPIX_(result, AWPBYTE)

    for (int i = 0; i < result->sSizeX*result->sSizeY; i++)
    {
    	if (gauss_pix[i] < t)
        	result_pix[i] = 0;
        else
            result_pix[i] = gauss_pix[i];
    }
	_AWP_SAFE_RELEASE_(gauss)
	_AWP_SAFE_RELEASE_(hsv0)

	return result;
}

