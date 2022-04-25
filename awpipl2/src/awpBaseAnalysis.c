/*M
//
//      awpipl2 image processing and image analysis library
//		File: awpBaseAnalysis.c
//		Purpose: histogramm image analysis implementation
//		Autors: Alex A.Telnykh (tlnykha@yahoo.com)
//      CopyRight 2004-2019 (c) NN-Videolab.net
//M*/
#include "_awpipl.h"
#pragma warning(disable: 4996)

/*
we assume that AWP_BYTE images has a min_value = 0 and max_value = 255. 
step between bins defined as 256 / hst->sSizeX
hst->sSizeX cannot be more than 256
*/
static AWPRESULT  _awpGetHstByte(awpImage* pImage, awpImage** ppHst)
{
	AWPRESULT  res = AWP_OK;
	awpImage*  pHst = *ppHst;
	AWPWORD    i = 0;
	AWPBYTE    c = 0;
	AWPWORD    idx = 0;
	AWPWORD    x = 0;
	AWPWORD    y = 0;

	AWPDOUBLE* pix  = NULL;
	AWPBYTE*   spix = NULL;

	AWPDOUBLE  hst_step = 256. / (AWPDOUBLE)pHst->sSizeX;

	pix  = _AWP_BPIX_(pHst, AWPDOUBLE);
	spix = _AWP_BPIX_(pImage, AWPBYTE);

	/*setup histogramm value axis*/
	for (i = 0; i < pHst->sSizeX; i++)
	{
		for (c = 0; c < pHst->bChannels; c++)
		{
			pix[i*pHst->bChannels + c] = floor(i*hst_step + 0.5);
		}
	}
	/*calc histogramm*/
	for (y = 0; y < pImage->sSizeY; y++)
	{
		for (x = 0; x < pImage->sSizeX; x++)
		{
			for (c = 0; c < pImage->bChannels; c++)
			{
				idx = (AWPBYTE)floor(spix[y*pImage->bChannels*pImage->sSizeX + x*pImage->bChannels + c] + 0.5);
				pix[pHst->sSizeX*pHst->bChannels + idx*pHst->bChannels + c]++;
			}
		}
	}

	return res;
}
#define _CALC_IDX_(v)\
	idx = (AWPBYTE)(255*(v[y*pImage->bChannels*pImage->sSizeX + x*pImage->bChannels + c] - image_min[c]) /(image_max[c] - image_min[c]) );


/*
for other datatypes we use histogramm between min value on the channel data 
and max value. num bins defined as histogramm width
*/
static AWPRESULT _awpGetHst(awpImage* pImage, awpImage** ppHst)
{
	AWPRESULT  res = AWP_OK;
	awpImage*  pHst = *ppHst;
	AWPDOUBLE*       image_min = NULL;
	AWPDOUBLE*       image_max = NULL;
	AWPDOUBLE*		 hst_step = NULL;
	AWPWORD    i = 0;
	AWPBYTE    c = 0;
	AWPWORD    idx = 0;
	AWPWORD    x = 0;
	AWPWORD    y = 0;
	AWPSHORT*  spix = NULL;
	AWPFLOAT*  fpix = NULL;
	AWPDOUBLE* dpix = NULL;

	AWPDOUBLE* pix = _AWP_BPIX_(pHst, AWPDOUBLE);

	/*range of signal variation*/
	if (res = awpMinMax(pImage, &image_min, &image_max))
	{
		if (res != AWP_OK)
			_ERR_EXIT_
	}

	/*setup width step for each channel*/
	hst_step = (AWPDOUBLE*)malloc(pImage->bChannels*sizeof(AWPDOUBLE));
	if (hst_step == NULL)
		_ERROR_EXIT_RES_(AWP_NOTENOUGH_MEM)

		for (i = 0; i < pImage->bChannels; i++)
			hst_step[i] = (image_max[i] - image_min[i]) / (AWPDOUBLE)pHst->sSizeX;

	/*setup histogramm value axis*/
	for (i = 0; i < pHst->sSizeX; i++)
	{
		for (c = 0; c < pHst->bChannels; c++)
		{
			pix[i*pHst->bChannels + c] = image_min[c] + i*hst_step[c];// + 0.5);
		}
	}
	/*calc histogramm*/
	switch (pImage->dwType)
	{
	case AWP_SHORT:
		spix = _AWP_BPIX_(pImage, AWPSHORT)
			break;
	case AWP_FLOAT:
		fpix = _AWP_BPIX_(pImage, AWPFLOAT)
			break;
	case AWP_DOUBLE:
		dpix = _AWP_BPIX_(pImage, AWPDOUBLE)
			break;
	}
	for (y = 0; y < pImage->sSizeY; y++)
	{
		for (x = 0; x < pImage->sSizeX; x++)
		{
			for (c = 0; c < pImage->bChannels; c++)
			{
				switch (pImage->dwType)
				{
				case AWP_SHORT:
					_CALC_IDX_(spix)
						break;
				case AWP_FLOAT:
					_CALC_IDX_(fpix)
						break;
				case AWP_DOUBLE:
					_CALC_IDX_(dpix)
						break;
				}
				pix[pHst->sSizeX*pHst->bChannels + idx*pHst->bChannels + c]++;
			}
		}
	}

CLEANUP:
	if (image_min != NULL)
		free(image_min);
	if (image_max != NULL)
		free(image_max);
	if (hst_step != NULL)
		free(hst_step);
	return res;
}
/*
obtain image histogramm function
image histogramm is stored in the awpImage with height = 2
and has num channels as in the source image pImage
first row of the histogramm image represents value of brightess
second row - number of pixels of this brightess
for AWP_BYTE images num bins in the histogramm should be less than 
256 and more then 1. 
for other image types num bins should be more 1. 
*/
AWPRESULT awpGetHst(awpImage* pImage, awpImage** ppHst, AWPINT options)
{
	AWPRESULT res  = AWP_OK;
	awpImage* pHst = NULL;
	if (awpCheckImage(pImage) != AWP_OK || ppHst == NULL)
		_ERROR_EXIT_RES_(AWP_BADARG)

	if (*ppHst == NULL)
	{
		/*create image for histogramm. default image has 256 pin histogramm*/
		if (awpCreateImage(ppHst, 256, 2, pImage->bChannels, AWP_DOUBLE) != AWP_OK)
			_ERROR_EXIT_RES_(AWP_BADMEMORY)
	}
	else
	{
		/*check histogramm image*/
		pHst = *ppHst; 
		if (awpCheckImage(pHst) != AWP_OK)
			_ERROR_EXIT_RES_(AWP_BADARG)

		if (pHst->bChannels != pImage->bChannels || pHst->sSizeY != 2 || 
			pHst->dwType != AWP_DOUBLE || pHst->sSizeX < 2)
			_ERROR_EXIT_RES_(AWP_BADARG)
	}

	if (pImage->dwType == AWP_BYTE)
	{
		/*make histogramm for the 8-bit image*/
		res = _awpGetHstByte(pImage, ppHst);
	}
	else
	{
		/*make histogramm for the  images of other datatypes*/
		res = _awpGetHst(pImage, ppHst);
	}

CLEANUP:
	return res;
}

#define _AWP_BASE_ANALYSIS_CHECK_HST_(v)\
if (res = awpCheckImage(v) != AWP_OK)\
	_ERR_EXIT_;\
if (v->sSizeY != 2 || v->dwType != AWP_DOUBLE)\
	_ERROR_EXIT_RES_(AWP_BADARG)\

#define _AWP_BASE_ANALYSIS_CHECK_RESULT_(v)\
if (res = awpCheckImage(v) != AWP_OK)\
	_ERR_EXIT_;\
if (v->sSizeX != 1 || v->sSizeY != 1)\
	_ERROR_EXIT_RES_(AWP_BADARG)\
if (v->dwType != AWP_DOUBLE)\
	_ERROR_EXIT_RES_(AWP_BADARG)\

/*
find the k-th moment of the distribution pHst ans store them to the pRes 
*/
static AWPRESULT __awpHstMomemt(awpImage* pHst, awpImage* pMoment, AWPDOUBLE k)
{
	AWPRESULT res = AWP_OK;
	AWPWORD	x = 0;
	AWPBYTE c = 0;
	AWPDOUBLE* hst		= NULL;
	AWPDOUBLE* Moment	= NULL;
    AWPDOUBLE* sum = NULL;

	if (pHst == NULL || pMoment == NULL || k < 1 || k>3)
		_ERROR_EXIT_RES_(AWP_BADARG)

	_AWP_BASE_ANALYSIS_CHECK_HST_(pHst);
	_AWP_BASE_ANALYSIS_CHECK_RESULT_(pMoment);

    Moment = (AWPDOUBLE*)pMoment->pPixels;
    hst    = (AWPDOUBLE*)pHst->pPixels;
	sum = (AWPDOUBLE*)malloc(pHst->bChannels*sizeof(AWPDOUBLE));
	memset(sum, 0, pHst->bChannels*sizeof(AWPDOUBLE));

	for (x = 0; x < pHst->sSizeX; x++)
	{
		for (c = 0; c < pHst->bChannels; c++)
		{
			Moment[c] += pow(hst[pHst->bChannels*x + c], k)*hst[pHst->bChannels*pHst->sSizeX + x*pHst->bChannels + c];
			sum[c] += hst[pHst->bChannels*pHst->sSizeX + x*pHst->bChannels + c];
		}
	}

    for (c = 0; c < pHst->bChannels; c++)
    {
        Moment[c] /= sum[c];
    }

    free(sum);
CLEANUP:
	return res;
}

/*
find the mean over histogramm 
*/
AWPRESULT awpGetHstMean(awpImage* pHst, awpImage* pMean)
{
	AWPRESULT res = AWP_OK;

	if (pHst == NULL || pMean == NULL)
		_ERROR_EXIT_RES_(AWP_BADARG)

	_AWP_BASE_ANALYSIS_CHECK_HST_(pHst);
	_AWP_BASE_ANALYSIS_CHECK_RESULT_(pMean);

	if (pHst->bChannels != pMean->bChannels)
		_ERROR_EXIT_RES_(AWP_BADARG)

	if (res = __awpHstMomemt(pHst, pMean, 1) != AWP_OK)
		_ERR_EXIT_

CLEANUP:
	return res;
}
/*
find the standard devation over histogramm
*/
AWPRESULT awpGetHstStdDev(awpImage* pHst, awpImage* pStdDev)
{
	AWPRESULT   res  = AWP_OK;
	awpImage*   mean = NULL;
	AWPDOUBLE*  m1   = NULL;
	AWPDOUBLE*  m2   = NULL;
	AWPBYTE		c    = 0;

	if (pHst == NULL || pStdDev == NULL)
		_ERROR_EXIT_RES_(AWP_BADARG)

	_AWP_BASE_ANALYSIS_CHECK_HST_(pHst);
	_AWP_BASE_ANALYSIS_CHECK_RESULT_(pStdDev);

	if (pHst->bChannels != pStdDev->bChannels)
		_ERROR_EXIT_RES_(AWP_BADARG)

	if (res = awpCreateImage(&mean, 1, 1, pStdDev->bChannels, AWP_DOUBLE) != AWP_OK)
		_ERR_EXIT_

	if (res = __awpHstMomemt(pHst, mean, 1) != AWP_OK)
		_ERR_EXIT_
	if (res = __awpHstMomemt(pHst, pStdDev, 2) != AWP_OK)
		_ERR_EXIT_

	m1 = _AWP_BPIX_(mean, AWPDOUBLE)
	m2 = _AWP_BPIX_(pStdDev, AWPDOUBLE)
	
	for (c = 0; c < pStdDev->bChannels; c++)
    {
        if (m2[c] - m1[c]*m1[c] < 1e-30)
        {
            m2[c] = 1e-30;
        }
        else
			m2[c] = sqrt(m2[c] - m1[c]*m1[c]);
    }

CLEANUP:
	_SAFE_RELEASE_(mean)
	return res;
}

/* compare two numbers with double precision*/
static AWPINT AWP_CDECL _awpDCompare(const void* a1, const void* a2)
{
	AWPDOUBLE* e1 = (AWPDOUBLE*)a1;
	AWPDOUBLE* e2 = (AWPDOUBLE*)a2;

	if (*e1 > *e2)
		return 1;
	else if (*e1 < *e2)
		return -1;
	else
		return 0;
}

static AWPINT AWP_CDECL _awpBCompare(const void* a1, const void* a2)
{
	AWPBYTE* e1 = (AWPBYTE*)a1;
	AWPBYTE* e2 = (AWPBYTE*)a2;

	if (*e1 > *e2)
		return 1;
	else if (*e1 < *e2)
		return -1;
	else
		return 0;
}
static AWPINT AWP_CDECL _awpFCompare(const void* a1, const void* a2)
{
	AWPFLOAT* e1 = (AWPFLOAT*)a1;
	AWPFLOAT* e2 = (AWPFLOAT*)a2;

	if (*e1 > *e2)
		return 1;
	else if (*e1 < *e2)
		return -1;
	else
		return 0;
}
static AWPINT AWP_CDECL _awpSCompare(const void* a1, const void* a2)
{
	AWPSHORT* e1 = (AWPSHORT*)a1;
	AWPSHORT* e2 = (AWPSHORT*)a2;

	if (*e1 > *e2)
		return 1;
	else if (*e1 < *e2)
		return -1;
	else
		return 0;
}
/*
find the median values over histogramm
*/
AWPRESULT awpGetMedian(awpImage* pImage, awpImage* pMedian)
{
	AWPRESULT   res = AWP_OK;
	awpImage*   channel = NULL;
	AWPBYTE		c = 0;
	AWPDOUBLE*  pix = NULL;
	AWPFLOAT *  fpix = NULL;
	AWPBYTE *   bpix = NULL;
	AWPSHORT*   spix = NULL;
	AWPDOUBLE*  median = NULL;
	if (pImage == NULL || pMedian == NULL)
		_ERROR_EXIT_RES_(AWP_BADARG)

	_AWP_BASE_ANALYSIS_CHECK_RESULT_(pMedian);

	median = _AWP_BPIX_(pMedian, AWPDOUBLE);

	for (c = 0; c < pMedian->bChannels; c++)
	{	
		if (res =awpGetChannel(pImage, &channel, c) != AWP_OK)
			_ERR_EXIT_
			if (pImage->dwType == AWP_DOUBLE)
			{
				pix = _AWP_BPIX_(channel, AWPDOUBLE);
			
			
				qsort((void*)pix, pImage->sSizeX*pImage->sSizeY, sizeof(AWPDOUBLE), _awpDCompare);
				median[c] = pix[channel->sSizeX*pImage->sSizeY/2];
			}
			else if (pImage->dwType == AWP_BYTE)
			{
				bpix = _AWP_BPIX_(channel, AWPBYTE);
			

				qsort((void*)bpix, pImage->sSizeX*pImage->sSizeY, sizeof(AWPBYTE), _awpBCompare);
				median[c] = bpix[channel->sSizeX*pImage->sSizeY / 2];
			}
			else if (pImage->dwType ==AWP_FLOAT)
			{
				fpix = _AWP_BPIX_(channel, AWPFLOAT);


				qsort((void*)fpix, pImage->sSizeX*pImage->sSizeY, sizeof(AWPFLOAT), _awpFCompare);
				median[c] = fpix[channel->sSizeX*pImage->sSizeY / 2];
			}
			else if (pImage->dwType == AWP_SHORT)
			{
				spix = _AWP_BPIX_(channel, AWPSHORT);


				qsort((void*)spix, pImage->sSizeX*pImage->sSizeY, sizeof(AWPSHORT), _awpSCompare);
				median[c] = spix[channel->sSizeX*pImage->sSizeY / 2];
			}
		_SAFE_RELEASE_(channel)
	}
CLEANUP:
	_SAFE_RELEASE_(channel)
	return res;
}

static AWPDOUBLE awpLog2(AWPDOUBLE value)
{
	return log(value) / log(2.);
}

/*
find the entropy  values over histogramm
*/
AWPRESULT awpGetHstEntropy(awpImage* pHst, awpImage* pEntropy)
{
	AWPRESULT   res = AWP_OK;
	awpImage*   channel = NULL;
	AWPBYTE		c = 0;
	AWPWORD     x = 0;
	AWPDOUBLE*  pix = NULL;
	AWPDOUBLE*  entropy = NULL;
	AWPDOUBLE* sum = NULL;
	if (pHst == NULL || pEntropy == NULL)
		_ERROR_EXIT_RES_(AWP_BADARG)

	_AWP_BASE_ANALYSIS_CHECK_HST_(pHst);
	_AWP_BASE_ANALYSIS_CHECK_RESULT_(pEntropy);

	entropy = _AWP_BPIX_(pEntropy, AWPDOUBLE);
	pix =     _AWP_BPIX_(pHst, AWPDOUBLE);
	sum = (AWPDOUBLE*)malloc(pHst->bChannels*sizeof(AWPDOUBLE));
	memset(sum, 0, pHst->bChannels*sizeof(AWPDOUBLE));
	pix += pHst->sSizeX*pHst->bChannels;
	for (x = 0; x < pHst->sSizeX; x++)
	{
		for (c = 0; c < pHst->bChannels; c++)
		{
			sum[c] += pix[x*pHst->bChannels + c];
		}
	}

	for (x = 0; x < pHst->sSizeX; x++)
	{
		for (c = 0; c < pHst->bChannels; c++)
		{
			entropy[c] += pix[x*pHst->bChannels + c] != 0 ? -pix[x*pHst->bChannels + c] / sum[c] * awpLog2(pix[x*pHst->bChannels + c] / sum[c]) : 0;
		}
	}
CLEANUP:
	return res;
}

AWPRESULT awpGet2DHistogramm(awpImage* pImage, awpImage* p2DHist, AWPBYTE low, AWPBYTE up, AWPINT needToConvert)
{
    AWPINT i, j;
    AWPDOUBLE*     dblPix;
    awpColor*   clPix;
    AWPBYTE        h,s, v;
    AWPRESULT res = AWP_OK;
    awpImage* pDblHst = NULL;
	awpImage* p2DHisto = NULL;
	// checking arguments
    if (pImage == NULL || p2DHist == NULL)
        return AWP_BADARG;
    if (pImage->dwType != AWP_BYTE)
        return AWP_BADARG;
    if (pImage->bChannels != 3)
        return AWP_BADARG;

    if (p2DHist->dwType != AWP_BYTE && p2DHist->dwType != AWP_DOUBLE )
        return AWP_BADARG;
    if (p2DHist->bChannels != 1)
        return AWP_BADARG;

	// to build a two-dimensional histogram we use the 1st and 2nd channels of the image
	// pImage and write the histogram into a temporary image like AWPDOUBLE
    res = awpCreateImage(&pDblHst, 256, 256, 1, AWP_DOUBLE);
    if (res != AWP_OK)
        return res;
    dblPix = (AWPDOUBLE*)pDblHst->pPixels;
    clPix  = (awpColor*)pImage->pPixels;
    for (i = 0; i < pImage->sSizeY; i++)
    {
        for (j = 0; j < pImage->sSizeX; j++)
        {
            v = clPix[i*pImage->sSizeX + j].bBlue;
            if (v>= low && v <= up)
            {
              h = clPix[i*pImage->sSizeX + j].bRed;
              s = clPix[i*pImage->sSizeX + j].bGreen;
              dblPix[s*256 + h]++;
            }
        }
    }
	awpCopyImage(p2DHist, &p2DHisto);
	// convert a temporary image of type AWPDOUBLE to AWPBYTE
    if ( needToConvert == TRUE )
    {
        if ((res = awpConvert(pDblHst, AWP_CONVERT_TO_BYTE_WITH_NORM)) != AWP_OK)
            return res;
    }
    if((res = awpFastCopyImage(pDblHst, p2DHist)) != AWP_OK)
        return res;
    if ((res = awpReleaseImage(&pDblHst)) != AWP_OK)
        return res;
    if ((res = awpFilter(p2DHist, p2DHisto, AWP_BLUR)) != AWP_OK)
        return res;
    return AWP_OK;
}
/*Histogramm equalize function
The function of the histogram equilibration. Brings an image histogram
pImage, to the form as close as possible to the uniform. 
Source image should has AWP_BYTE datatype
*/
AWPRESULT awpHistogrammEqualize(awpImage* pImage)
{
	AWPRESULT               res   = AWP_OK;  
	awpImage*				pHst  = NULL;
	awpImage*				pFunc = NULL;
	AWPDOUBLE*				SumOfEntries = NULL;
	AWPINT					count = 0;
	AWPINT					i = 0;
	AWPINT					j = 0;
	AWPBYTE					c = 0;
	AWPBYTE*				p = NULL;

	AWPDOUBLE*	            Hst; 
	AWPDOUBLE*			    PFunc;

    /*check source image*/
	if (res = awpCheckImage(pImage) != AWP_OK)
		_ERR_EXIT_
	if (pImage->dwType != AWP_BYTE)
		_ERROR_EXIT_RES_(AWP_BADARG)
	/*todo: use Options flag to setup output histogramm*/
	if (res = awpGetHst(pImage, &pHst, 0) != AWP_OK)
		_ERR_EXIT_
	if (res = awpCreateImage(&pFunc, pHst->sSizeX, 1, pHst->bChannels, AWP_DOUBLE) != AWP_OK)
		_ERR_EXIT_
	count = pImage->sSizeX*pImage->sSizeY;
	SumOfEntries = (AWPDOUBLE*)malloc(pImage->bChannels*sizeof(AWPDOUBLE));
	Hst = _AWP_BPIX_(pHst, AWPDOUBLE);
	Hst += pHst->sSizeX*pHst->bChannels;
	PFunc = _AWP_BPIX_(pFunc, AWPDOUBLE);
	for (i = 0; i < pHst->sSizeX; i++)
	{
		for (c = 0; c < pImage->bChannels; c++)
		{
			SumOfEntries[c] = 0;
			for (j = 0; j <= i; j++)
				SumOfEntries[c] += Hst[j*pHst->bChannels + c];
			PFunc[i*pHst->bChannels + c] = SumOfEntries[c] / count;
		}

	}
	p = _AWP_BPIX_(pImage, AWPBYTE);
	for (i = 0; i < count; i++)
	{
		for (c = 0; c < pImage->bChannels; c++)
		{
			p[i*pImage->bChannels + c] = (AWPBYTE)(255 * PFunc[p[i*pImage->bChannels + c] * pImage->bChannels + c]);
		}
	}

CLEANUP:
	_SAFE_RELEASE_(pHst)
	_SAFE_RELEASE_(pFunc)
	_SAFE_FREE_(SumOfEntries)
	return res;
}


AWPRESULT awpGetCentroid(const awpImage* pImg, awpPoint* p)
{
	AWPRESULT res;
	AWPDOUBLE sum, x0, y0;
	AWPBYTE* pix;
	AWPINT  x, y;
	AWPDWORD i;

	res = AWP_OK;
	_CHECK_RESULT_((res = awpCheckImage(pImg)))

		if (pImg->dwType != AWP_BYTE)
		{
			res = AWP_NOTSUPPORT;
			_ERR_EXIT_
		}

	if (pImg->bChannels != 1)
	{
		res = AWP_NOTSUPPORT;
		_ERR_EXIT_
	}
	x0 = 0;
	y0 = 0;
	sum = 0;
	pix = (AWPBYTE*)pImg->pPixels;
	for (i = 0; i < pImg->sSizeY; i++)
	{
		y = i;
		for (x = 0; x <= pImg->sSizeX; x++)
		{
			sum += pix[y*pImg->sSizeX + x];
			x0 += x*pix[y*pImg->sSizeX + x];
			y0 += y*pix[y*pImg->sSizeX + x];
		}
	}

	p->X = (AWPSHORT)floor(x0 / (double)sum + 0.5);
	p->Y = (AWPSHORT)floor(y0 / (double)sum + 0.5);
CLEANUP:
	return res;
}

AWPRESULT awpGetOrientation(const awpImage* pImg, AWPDOUBLE* teta, AWPDOUBLE* mi, AWPDOUBLE* ma)
{
	/*local variables*/
	AWPRESULT res;
	AWPINT  x, y, j;
	AWPDOUBLE mxx, myy, mxy, intes, s; /*cental moments*/
	AWPBYTE* pixels;
	awpPoint center;
	AWPDWORD i;
	/*init out arguments */
	*teta = 0;
	*mi = 0;
	*ma = 0;
	res = AWP_OK;
	/*check the arguments*/
	_CHECK_RESULT_((res = awpCheckImage(pImg)))
		if (teta == NULL || mi == NULL || ma == NULL)
		{
			res = AWP_BADARG;
			_ERR_EXIT_
		}
	/*check the capabilities*/
	if (pImg->bChannels > 1 || pImg->dwType != AWP_BYTE)
	{
		res = AWP_BADARG;
		_ERR_EXIT_
	}
	/*init local variables*/
	mxx = 0; myy = 0; mxy = 0;
	if (awpGetCentroid(pImg, &center) != AWP_OK)
	{
		res = AWP_BADARG;
		_ERR_EXIT_
	}

	/*find the moment*/
	intes = 0;
	s = 0;
	pixels = (AWPBYTE*)pImg->pPixels;
	for (i = 0; i < pImg->sSizeY; i++)
	{
		for (j = 0; j <pImg->sSizeX; j++)
		{
			x = j;
			y = i;
			intes += pixels[y*pImg->sSizeX + x];
			if (pixels[y*pImg->sSizeX + x] > 0)
			{
				mxx += (x - center.X)*(x - center.X);
				myy += (y - center.Y)*(y - center.Y);
				mxy += (x - center.X)*(y - center.Y);
				s++;
			}
		}
	}
	if (intes == 0)
	{

		res = AWP_BADARG;
		_ERR_EXIT_
	}
	intes = s;
	mxx /= intes;
	myy /= intes;
	mxy /= intes;

	/*find the moment*/
	if (mxy == 0 && myy <= mxx)
	{
		*teta = -AWP_PI / 2;
		*ma = 4 * sqrt(mxx);
		*mi = 4 * sqrt(myy);
	}
	else if (mxy == 0 && myy > mxx)
	{
		*teta = 0;
		*ma = 4 * sqrt(myy);
		*mi = 4 * sqrt(mxx);
	}
	else if (myy <= mxx)
	{
		*teta = atan(-2 * mxy*myy - mxx + sqrt((myy - mxx)*(myy - mxx) + 4 * mxy*mxy));
		*ma = sqrt(8 * (myy + mxx + sqrt((myy - mxx)*(myy - mxx) + 4 * mxy*mxy)));
		*mi = sqrt(8 * (myy + mxx - sqrt((myy - mxx)*(myy - mxx) + 4 * mxy*mxy)));

	}
	else if (myy > mxx)
	{
		*teta = atan(sqrt(mxx + myy + sqrt((mxx - myy)*(mxx - myy) + 4 * mxy*mxy)) / (-2 * mxy + 0.000001));
		*ma = sqrt(8 * (myy + mxx + sqrt((myy - mxx)*(myy - mxx) + 4 * mxy*mxy)));
		*mi = sqrt(8 * (myy + mxx - sqrt((myy - mxx)*(myy - mxx) + 4 * mxy*mxy)));
	}

	*teta = 180.0*(*teta) / AWP_PI;
CLEANUP:
	return res;
}
