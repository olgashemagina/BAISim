/*	$Id: awpEdge.c,v 1.2 2005/04/02 15:48:52 alt Exp $	*/

#include "_awpipl.h"

AWPRESULT awpLocalMax(awpImage* pImage, awpImage** ppImage, AWPBYTE thr, AWPINT options)
{
	AWPRESULT res = AWP_OK;
	awpImage* pTempImage = NULL;
	AWPBYTE *pSrcPix, *pDstPix;
	AWPWORD i, j;
	AWPBYTE curcolor;
		
	_CHECK_RESULT_((res = awpCheckImage(pImage)))
	if ((pImage->bChannels != 1) || (pImage->dwType != AWP_BYTE))
		_ERROR_EXIT_RES_(AWP_NOTSUPPORT)

	_CHECK_RESULT_((res = awpCreateImage(&pTempImage, pImage->sSizeX, pImage->sSizeY, pImage->bChannels, AWP_BYTE)))
	
	pSrcPix = (AWPBYTE *) pImage->pPixels;
	pDstPix = (AWPBYTE *) pTempImage->pPixels;

	switch (options)
	{
	case _4_NEIGHBOURS:
		for(j = 0; j < pImage->sSizeY; j++)
			for(i = 0; i < pImage->sSizeX; i++)
			{
				curcolor = pSrcPix[(j * pImage->sSizeX) + i];

				if ((curcolor < thr) ||
					((i > 0) && (pSrcPix[(j * pImage->sSizeX) + i - 1] > curcolor)) ||
					((i < pImage->sSizeX - 1) && (pSrcPix[(j * pImage->sSizeX) + i + 1] > curcolor)) ||
					((j > 0) && (pSrcPix[((j - 1) * pImage->sSizeX) + i] > curcolor)) ||
					((j < pImage->sSizeY - 1) && (pSrcPix[((j + 1) * pImage->sSizeX) + i] > curcolor)))
					continue;

				pDstPix[(j * pTempImage->sSizeX) + i] = curcolor;
			}
		break;
	case _8_NEIGHBOURS:
		for(j = 0; j < pImage->sSizeY; j++)
			for(i = 0; i < pImage->sSizeX; i++)
			{
				curcolor = pSrcPix[(j * pImage->sSizeX) + i];

				if ((curcolor < thr) ||
					((i > 0) && (j > 0) && (pSrcPix[((j - 1) * pImage->sSizeX) + i - 1] > curcolor)) ||
					((i > 0) && (pSrcPix[(j * pImage->sSizeX) + i - 1] > curcolor)) ||
					((i > 0) && (j < pImage->sSizeY - 1) && (pSrcPix[((j + 1) * pImage->sSizeX) + i - 1] > curcolor)) ||
					((j < pImage->sSizeY - 1) && (pSrcPix[((j + 1) * pImage->sSizeX) + i] > curcolor)) ||
					((i < pImage->sSizeX - 1) && (j < pImage->sSizeY - 1) && (pSrcPix[((j + 1) * pImage->sSizeX) + i + 1] > curcolor)) ||
					((i < pImage->sSizeX - 1) && (pSrcPix[(j * pImage->sSizeX) + i + 1] > curcolor)) ||
					((i < pImage->sSizeX - 1) && (j > 0) && (pSrcPix[((j - 1) * pImage->sSizeX) + i + 1] > curcolor)) ||
					((j > 0) && (pSrcPix[((j - 1) * pImage->sSizeX) + i] > curcolor)))
					continue;

				pDstPix[(j * pTempImage->sSizeX) + i] = curcolor;
			}
		break;
	default:
		res = AWP_BADARG;
		_CHECK_RESULT_(awpReleaseImage(&pTempImage))
		_ERR_EXIT_
	}

	*ppImage = pTempImage;

CLEANUP:
	return res;
}

AWPRESULT awpLocalMin(awpImage* pImage, awpImage** ppImage, AWPBYTE thr, AWPINT options)
{
	AWPRESULT res = AWP_OK;
	awpImage* pTempImage = NULL;
	AWPBYTE *pSrcPix, *pDstPix;
	AWPWORD i, j;
	AWPBYTE curcolor;

	_CHECK_RESULT_((res = awpCheckImage(pImage)))
	if ((pImage->bChannels != 1) || (pImage->dwType != AWP_BYTE))
		_ERROR_EXIT_RES_(AWP_NOTSUPPORT)

	_CHECK_RESULT_((res = awpCreateImage(&pTempImage, pImage->sSizeX, pImage->sSizeY, pImage->bChannels, AWP_BYTE)))

	memset((AWPBYTE*)pTempImage->pPixels, 255, pImage->sSizeX * pImage->sSizeY * sizeof(AWPBYTE));

	pSrcPix = (AWPBYTE *) pImage->pPixels;
	pDstPix = (AWPBYTE *) pTempImage->pPixels;

	switch (options)
	{
	case _4_NEIGHBOURS:
		for(j = 0; j < pImage->sSizeY; j++)
			for(i = 0; i < pImage->sSizeX; i++)
			{
				curcolor = pSrcPix[(j * pImage->sSizeX) + i];

				if ((curcolor > thr) ||
					((i > 0) && (pSrcPix[(j * pImage->sSizeX) + i - 1] < curcolor)) ||
					((i < pImage->sSizeX - 1) && (pSrcPix[(j * pImage->sSizeX) + i + 1] < curcolor)) ||
					((j > 0) && (pSrcPix[((j - 1) * pImage->sSizeX) + i] < curcolor)) ||
					((j < pImage->sSizeY - 1) && (pSrcPix[((j + 1) * pImage->sSizeX) + i] < curcolor)))
					continue;

				pDstPix[(j * pTempImage->sSizeX) + i] = curcolor;
			}
		break;
	case _8_NEIGHBOURS:
		for(j = 0; j < pImage->sSizeY; j++)
			for(i = 0; i < pImage->sSizeX; i++)
			{
				curcolor = pSrcPix[(j * pImage->sSizeX) + i];

				if ((curcolor > thr) ||
					((i > 0) && (j > 0) && (pSrcPix[((j - 1) * pImage->sSizeX) + i - 1] < curcolor)) ||
					((i > 0) && (pSrcPix[(j * pImage->sSizeX) + i - 1] < curcolor)) ||
					((i > 0) && (j < pImage->sSizeY - 1) && (pSrcPix[((j + 1) * pImage->sSizeX) + i - 1] < curcolor)) ||
					((j < pImage->sSizeY - 1) && (pSrcPix[((j + 1) * pImage->sSizeX) + i] < curcolor)) ||
					((i < pImage->sSizeX - 1) && (j < pImage->sSizeY - 1) && (pSrcPix[((j + 1) * pImage->sSizeX) + i + 1] < curcolor)) ||
					((i < pImage->sSizeX - 1) && (pSrcPix[(j * pImage->sSizeX) + i + 1] < curcolor)) ||
					((i < pImage->sSizeX - 1) && (j > 0) && (pSrcPix[((j - 1) * pImage->sSizeX) + i + 1] < curcolor)) ||
					((j > 0) && (pSrcPix[((j - 1) * pImage->sSizeX) + i] < curcolor)))
					continue;

				pDstPix[(j * pTempImage->sSizeX) + i] = curcolor;
			}
		break;
	default:
		res = AWP_BADARG;
		_CHECK_RESULT_(awpReleaseImage(&pTempImage))
		_ERR_EXIT_
	}

	*ppImage = pTempImage;

CLEANUP:
	return res;
}


AWPRESULT awpFeatures(awpImage* pImage, awpImage** ppImage, AWPBYTE thr_max, AWPBYTE thr_min, AWPBYTE relative_thr, AWPINT radius, AWPINT MinMax_options, AWPINT options)
{
	AWPRESULT res = AWP_OK;
	awpImage *pImageMax = NULL, *pImageMin = NULL, *pTempImage = NULL;
	AWPBYTE *pSrcMaxPix, *pSrcMinPix, *pDstPix;
	AWPWORD i, j;
	AWPSHORT local_i, local_j;
	AWPINT cur_x, cur_y;
	AWPBYTE cur_max_color, cur_min_color, min_min_color;

	if (radius == 0)
		_ERROR_EXIT_RES_(AWP_BADARG)
	_CHECK_RESULT_((res = awpCheckImage(pImage)))

	_CHECK_RESULT_((res = awpLocalMax(pImage, &pImageMax, thr_max, MinMax_options)))
	_CHECK_RESULT_((res = awpLocalMin(pImage, &pImageMin, thr_min, MinMax_options)))

	_CHECK_RESULT_((res = awpCreateImage(&pTempImage, pImage->sSizeX, pImage->sSizeY, pImage->bChannels, AWP_BYTE)))

	pSrcMaxPix = (AWPBYTE *) pImageMax->pPixels;
	pSrcMinPix = (AWPBYTE *) pImageMin->pPixels;
	pDstPix = (AWPBYTE *) pTempImage->pPixels;

	for(j = 0; j < pImage->sSizeY; j++)
		for(i = 0; i < pImage->sSizeX; i++)
		{
			cur_max_color = pSrcMaxPix[(j * pImage->sSizeX) + i];
			if (cur_max_color == 0)
				continue;

			min_min_color = 255;

			for(local_j = - radius; local_j <= radius; local_j++)
				for(local_i = - radius; local_i <= radius; local_i++)
				{
					if ((local_i == 0) && (local_j == 0)) continue;

					cur_x = i + local_i;
					cur_y = j + local_j;

					if ((cur_x < 0) || (cur_y < 0) || (cur_x >= pImage->sSizeX) ||
						(cur_y >= pImage->sSizeY)) continue;

					cur_min_color = pSrcMinPix[(cur_y * pImage->sSizeX) + cur_x];

					if (cur_min_color < min_min_color)
						min_min_color = cur_min_color;
				}

			if ((cur_max_color - min_min_color >= relative_thr) || ((min_min_color == 255) && (options == LEAVE_UNKNOWN_MAX)))
				pDstPix[(j * pImage->sSizeX) + i] = 255;
		}

	_CHECK_RESULT_((res = awpReleaseImage(&pImageMax)))
	_CHECK_RESULT_((res = awpReleaseImage(&pImageMin)))

	*ppImage = pTempImage;

CLEANUP:
	if (pImageMax != NULL)
		if (res == AWP_OK)
			res = awpReleaseImage(&pImageMax);
		else
			awpReleaseImage(&pImageMax);

	if (pImageMin != NULL)
		if (res == AWP_OK)
			res = awpReleaseImage(&pImageMin);
		else
			awpReleaseImage(&pImageMin);

	return res;
}

AWPRESULT awpFeaturesV2(awpImage* pImage, awpImage** ppImage, AWPBYTE thr_max, AWPBYTE relative_thr, AWPINT radius, AWPINT MinMax_options)
{
	AWPRESULT res = AWP_OK;
	awpImage *pImageMax = NULL, *pTempImage = NULL;
	AWPBYTE *pSrcMaxPix, *pSrcPix, *pDstPix;
	AWPWORD i, j;
	AWPSHORT local_i, local_j;
	AWPINT cur_x, cur_y;
	AWPBYTE cur_max_color, cur_min_color, min_min_color;

	if (radius == 0)
		_ERROR_EXIT_RES_(AWP_BADARG)
	_CHECK_RESULT_((res = awpCheckImage(pImage)))

	_CHECK_RESULT_((res = awpLocalMax(pImage, &pImageMax, thr_max, MinMax_options)))

	_CHECK_RESULT_((res = awpCreateImage(&pTempImage, pImage->sSizeX, pImage->sSizeY, pImage->bChannels, AWP_BYTE)))

	pSrcMaxPix = (AWPBYTE *) pImageMax->pPixels;
	pSrcPix = (AWPBYTE *) pImage->pPixels;
	pDstPix = (AWPBYTE *) pTempImage->pPixels;

	for(j = 0; j < pImage->sSizeY; j++)
		for(i = 0; i < pImage->sSizeX; i++)
		{
			cur_max_color = pSrcMaxPix[(j * pImage->sSizeX) + i];
			if (cur_max_color == 0)
				continue;

			min_min_color = 255;

			for(local_j = - radius; local_j <= radius; local_j++)
				for(local_i = - radius; local_i <= radius; local_i++)
				{
					if ((local_i == 0) && (local_j == 0)) continue;

					cur_x = i + local_i;
					cur_y = j + local_j;

					if ((cur_x < 0) || (cur_y < 0) || (cur_x >= pImage->sSizeX) ||
						(cur_y >= pImage->sSizeY)) continue;

					cur_min_color = pSrcPix[(cur_y * pImage->sSizeX) + cur_x];

					if (cur_min_color < min_min_color)
						min_min_color = cur_min_color;
				}

			if (cur_max_color - min_min_color >= relative_thr)
				pDstPix[(j * pImage->sSizeX) + i] = 255;
		}

	_CHECK_RESULT_((res = awpReleaseImage(&pImageMax)))

	*ppImage = pTempImage;

CLEANUP:
	if (pImageMax != NULL)
		if (res == AWP_OK)
			res = awpReleaseImage(&pImageMax);
		else
			awpReleaseImage(&pImageMax);

	return res;
}

// awpCanny implementation
// supported data types: AWP_BYTE, AWP_SHORT, AWP_FLOAT, AWP_DOUBLE
// the data types of pSrc and pDst should be same.
// awpCanny works on only single-channel images
// the value of thr should be between 0...1
// sigma is used for smoothig by gauss filter
// size of filter calculates automatically
// if sigma <=0 no noise reduction performed
AWPRESULT awpCanny(awpImage* pSrc, awpImage* pDst, AWPFLOAT thr, AWPFLOAT sigma )
{
    AWPRESULT res = AWP_OK;

    _CHECK_RESULT_(res = awpCheckImage(pSrc))
    _CHECK_RESULT_(res = awpCheckImage(pDst))

    _CHECK_SAME_SIZES(pSrc, pDst)
    _CHECK_SAME_TYPE(pSrc,pDst)
    _CHECK_NUM_CHANNELS(pSrc, pDst)

    if (thr <= 0 || thr >1)
        _ERROR_EXIT_RES_(AWP_BADARG)
/*
    switch(pSrc->dwType)
    {
        case AWP_BYTE:
            res = _awpCannyAWP_BYTE(pSrc,pDst,thr,sigma);
        break;
        case AWP_SHORT:
            res = _awpCannyAWP_SHORT(pSrc,pDst,thr,sigma);
        break;
        case AWP_FLOAT:
            res = _awpCannyAWP_FLOAT(pSrc,pDst,thr,sigma);
        break;
        case AWP_DOUBLE:
            res = _awpCannyAWP_AWPAWPDOUBLE(pSrc,pDst,thr,sigma);
        break;
    }
*/
CLEANUP:
    return res;
}

static const AWPDOUBLE sobel_v_mask[9] =
	{-1,0,1,
	 -2,0,2,
	 -1,0,1};
static const AWPDOUBLE sobel_h_mask[9] =
	{1,2,1,
	 0,0,0,
    -1,-2,-1};

/*
	convolutions with Sobel masks on the double image pImage
*/
AWPRESULT awpEdgeSobel1(awpImage* pImage, awpImage* pVImage, awpImage* pHImage)
{
    AWPRESULT res = AWP_OK;
    AWPDOUBLE*  b = NULL;
    AWPDOUBLE* v = NULL;
    AWPDOUBLE* h = NULL;
    AWPSHORT    y = 0;
    AWPSHORT    x = 0;
    AWPBYTE		k = 0;
    AWPSHORT    xx = 0;
    AWPSHORT    yy = 0;
    AWPDOUBLE   sumv = 0;
    AWPDOUBLE   sumh = 0;
    AWPWORD     c = 0;
    AWPBYTE     _nc = 0;
    AWPWORD		_w = 0;
    AWPWORD     _h = 0;
    _CHECK_RESULT_(res = awpCheckImage(pImage))
    _CHECK_RESULT_(res = awpCheckImage(pVImage))
    _CHECK_RESULT_(res = awpCheckImage(pHImage))

    _CHECK_SAME_SIZES(pHImage, pVImage)
    _CHECK_SAME_SIZES(pImage, pVImage)
    _CHECK_NUM_CHANNELS(pHImage, pVImage)
    _CHECK_SAME_TYPE(pHImage, pVImage)

    //-------------------------
    b = (AWPDOUBLE*)pImage->pPixels;
    h = (AWPDOUBLE*)pHImage->pPixels;
    v = (AWPDOUBLE*)pVImage->pPixels;

    _nc = pImage->bChannels;
    _w  = pImage->sSizeX;
    _h  = pImage->sSizeY;

	for (k = 0; k < _nc; k++)
    {
        for ( y = 1; y < _h - 1; y++)
        {
            for ( x = 1; x < _w - 1; x++)
            {
                sumv = 0;
                sumh = 0;
                c = 0;
                for (yy = -1; yy <=1; yy++)
                {
                   for (xx = -1; xx <= 1; xx++)
                   {
                        sumv += sobel_v_mask[c]*b[k+_nc*(x+xx) +(y+yy)*pImage->sSizeX*_nc];
                        sumh += sobel_h_mask[c]*b[k+_nc*(x+xx) +(y+yy)*pImage->sSizeX*_nc];
                        c++;
                   }
                }
                v[k+_nc*x+y*_nc*pImage->sSizeX] = sumv;
                h[k+_nc*x+y*_nc*pImage->sSizeX] = sumh;
            }
        }
    }

CLEANUP:
    return res;
}
/*
	calculate arctan [0..2*pi]
*/

__inline static double _awpAtan(double dx, double dy)
{
    AWPDOUBLE t = 0;
	if (dx == 0)
    	return 0;
    t = fabs(dy / dx);
    if (dx > 0 && dy >= 0)
    	return atan(t);
    else if (dx >=0 && dy < 0)
    	return 1.5*AWP_PI + atan(t);
    else if (dx < 0 && dy < 0)
    	return AWP_PI + atan(t);
    else
	    return 0.5*AWP_PI + atan(t);
}

#define _CONVERT_SOBEL_ANGLE_(b) \
if (pGradDir != NULL)\
	_CHECK_RESULT_(res = awpConvert(img_dir, b))

#define _COPY_SOBEL_RESULT_ANGLE_ \
if (pGradDir != NULL)\
 _CHECK_RESULT_(res = awpCopyImage(img_dir, &pGradDir));
/*
	perfoms SOBEL filter over awpImage
*/

AWPRESULT awpEdgeSobel(awpImage* pImage, awpImage* pGradAmpl, awpImage* pGradDir)
{
	AWPRESULT res = AWP_OK;
    awpImage* img_dx = NULL;
    awpImage* img_dy = NULL;
    awpImage* img_grad = NULL;
    awpImage* img_dir  = NULL;
    awpImage* img_src = NULL;

    AWPINT i = 0;
    AWPINT j = 0;
    AWPBYTE k = 0;
    AWPDOUBLE* dx = NULL;
    AWPDOUBLE* dy = NULL;
    AWPDOUBLE* grad = NULL;
    AWPDOUBLE* dir = NULL;
     AWPINT     idx = 0;
     AWPINT     idx0 = 0;
    _CHECK_RESULT_(res = awpCheckImage(pGradAmpl))
    _CHECK_RESULT_(res = awpCheckImage(pImage))

    _CHECK_RESULT_(res = awpCreateImage(&img_dx, pImage->sSizeX, pImage->sSizeY, pImage->bChannels, AWP_DOUBLE))
    _CHECK_RESULT_(res = awpCreateImage(&img_dy, pImage->sSizeX, pImage->sSizeY, pImage->bChannels, AWP_DOUBLE))
    _CHECK_RESULT_(res = awpCreateImage(&img_grad, pImage->sSizeX, pImage->sSizeY, pImage->bChannels, AWP_DOUBLE))
    _CHECK_RESULT_(res = awpCopyImage(pImage, &img_src))
    if (img_src->dwType != AWP_DOUBLE)
    	_CHECK_RESULT_(res = awpConvert(img_src, AWP_CONVERT_TO_DOUBLE))

	// call edge sobel dx-dy convolutoin
    _CHECK_RESULT_(res = awpEdgeSobel1(img_src, img_dx, img_dy))

    if (pGradDir != NULL)
    {
	    _CHECK_RESULT_(res = awpCheckImage(pGradDir))
        _CHECK_SAME_SIZES(pGradDir, pGradAmpl)
    	_CHECK_NUM_CHANNELS(pGradDir, pGradAmpl)
    	_CHECK_SAME_TYPE(pGradDir, pGradAmpl)
        _CHECK_RESULT_(res = awpCreateImage(&img_dir, pImage->sSizeX, pImage->sSizeY, pImage->bChannels, AWP_DOUBLE))
    }

    dx = (AWPDOUBLE*)img_dx->pPixels;
    dy = (AWPDOUBLE*)img_dy->pPixels;
    grad  = (AWPDOUBLE*)img_grad->pPixels;
    if (pGradDir == NULL)
    {
        for (k = 0; k < pImage->bChannels; k++)
        {
            for (i = 0; i < pImage->sSizeY; i++)
            {
                idx0 = i*pImage->sSizeX*pImage->bChannels;
                for (j = 0; j < pImage->sSizeX; j++)
                {
                   grad[idx] = sqrt(dx[idx]*dx[idx] + dy[idx]*dy[idx]);
                   idx = idx0 + pImage->bChannels*j + k;
                }
            }

        }
    }
    else
    {
    	dir  = (AWPDOUBLE*)img_dir->pPixels;
        for (k = 0; k < pImage->bChannels; k++)
        {
            for (i = 0; i < pImage->sSizeY; i++)
            {
                idx0 = i*pImage->sSizeX*pImage->bChannels;
                for (j = 0; j < pImage->sSizeX; j++)
                {
                   dir[idx]  = _awpAtan(dx[idx], dy[idx]);
                   grad[idx] = sqrt(dx[idx]*dx[idx] + dy[idx]*dy[idx]);
                   idx = idx0 + pImage->bChannels*j + k;
                }
            }
        }
    }
    // convert result to desired type
    switch(pImage->dwType)
    {
     	case AWP_BYTE:
    		_CHECK_RESULT_(res = awpConvert(img_grad, AWP_CONVERT_TO_BYTE_WITH_NORM));
            _CONVERT_SOBEL_ANGLE_(AWP_CONVERT_TO_BYTE_WITH_NORM)
	    break;
        case AWP_SHORT:
    		_CHECK_RESULT_(res = awpConvert(img_grad, AWP_CONVERT_TO_SHORT));
            _CONVERT_SOBEL_ANGLE_(AWP_CONVERT_TO_SHORT)
        break;
        case AWP_FLOAT:
    		_CHECK_RESULT_(res = awpConvert(img_grad, AWP_CONVERT_TO_FLOAT));
			_CONVERT_SOBEL_ANGLE_(AWP_CONVERT_TO_FLOAT)
        break;
    }
    _CHECK_RESULT_(res = awpCopyImage(img_grad, &pGradAmpl));

    _COPY_SOBEL_RESULT_ANGLE_

CLEANUP:
	_SAFE_RELEASE_(img_dx);
	_SAFE_RELEASE_(img_dy);
	_SAFE_RELEASE_(img_grad);
    _SAFE_RELEASE_(img_src);
    _SAFE_RELEASE_(img_dir);
    return res;
}

