/*M
//
//      awpipl2 image processing and image analysis library
//		File: awpNorm.c
//		Purpose: image normalizatoin implementation 
//
//      CopyRight 2004-2018 (c) NN-Videolab.net
//M*/

#include "_awpipl.h"

#define _AWP_NORM_L1(u)\
	{u* src; \
	src = (u*)pSrc->pPixels; \
for (i = 0; i < pSrc->sSizeX*pSrc->sSizeY; i++)\
	{\
for (k = 0; k < pSrc->bChannels; k++)\
		{\
		norm[k] += (src[i*pSrc->bChannels + k] < 0 ? -src[i*pSrc->bChannels + k] : src[i*pSrc->bChannels + k]); \
		}\
	}\
	}


static void _awpNormL1(awpImage* pSrc, awpImage* pNorm)
{
	AWPDOUBLE* norm;
	AWPINT i, k;

	norm = (AWPDOUBLE*)pNorm->pPixels;

	switch (pSrc->dwType)
	{
	case AWP_BYTE:
		_AWP_NORM_L1(AWPBYTE)
			break;
	case AWP_SHORT:
		_AWP_NORM_L1(AWPSHORT)
			break;
	case AWP_FLOAT:
		_AWP_NORM_L1(AWPFLOAT)
			break;
	case AWP_DOUBLE:
		_AWP_NORM_L1(AWPDOUBLE)
			break;
	}
}

#define _AWP_NORM_L2(u)\
{u* src; \
	src = (u*)pSrc->pPixels; \
for (i = 0; i < pSrc->sSizeX*pSrc->sSizeY; i++)\
{\
for (k = 0; k < pSrc->bChannels; k++)\
{\
	norm[k] += src[i*pSrc->bChannels + k] * src[i*pSrc->bChannels + k]; \
}\
}\
for (k = 0; k < pSrc->bChannels; k++)\
	norm[k] = sqrt(norm[k]); \
}

static void _AWP_NORM_L2_BYTE(awpImage* pSrc, AWPDOUBLE* norm)
{
	int i, k;
	AWPBYTE* src = (AWPBYTE*)pSrc->pPixels;
	for (i = 0; i < pSrc->sSizeX*pSrc->sSizeY; i++)
	{
		for (k = 0; k < pSrc->bChannels; k++)
		{
			norm[k] += src[i*pSrc->bChannels + k] * src[i*pSrc->bChannels + k];
		}
	}
	for (k = 0; k < pSrc->bChannels; k++)
		norm[k] = sqrt(norm[k]);

}

static void _awpNormL2(awpImage* pSrc, awpImage* pNorm)
{
	AWPDOUBLE* norm;
	AWPINT i, k;

	norm = (AWPDOUBLE*)pNorm->pPixels;
	switch (pSrc->dwType)
	{
	case AWP_BYTE:
		_AWP_NORM_L2(AWPBYTE)
			//_AWP_NORM_L2_BYTE(pSrc, norm);
			break;
	case AWP_SHORT:
		_AWP_NORM_L2(AWPSHORT)
			break;
	case AWP_FLOAT:
		_AWP_NORM_L2(AWPFLOAT)
			break;
	case AWP_DOUBLE:
		_AWP_NORM_L2(AWPDOUBLE)
			break;
	}
}

AWPRESULT awpNorm(awpImage* pSrc, awpImage** ppNorm, long lOptions)
{
	AWPRESULT res;
	res = AWP_OK;/*initialize result */

	_CHECK_RESULT_((res = awpCheckImage(pSrc)))

	if (ppNorm == NULL)
		_ERROR_EXIT_RES_(AWP_BADARG)

	if (*ppNorm == NULL)
		_CHECK_RESULT_((res = awpCreateImage(ppNorm, pSrc->bChannels, 1, 1, AWP_DOUBLE)))
	else
	{
		_CHECK_RESULT_((res = awpCheckImage(*ppNorm)))
//		if ((*ppNorm)->bChannels != 1)
//			_ERROR_EXIT_RES_(AWP_BADARG)
		if ((*ppNorm)->sSizeY > 1)
			_ERROR_EXIT_RES_(AWP_BADARG)
		if ((*ppNorm)->dwType != AWP_DOUBLE)
			_ERROR_EXIT_RES_(AWP_BADARG)
		if ((*ppNorm)->sSizeX != pSrc->bChannels)
			_ERROR_EXIT_RES_(AWP_BADARG)
	}


	awpZeroImage(*ppNorm);

	if (lOptions == AWP_NORM_L1)
		_awpNormL1(pSrc, *ppNorm);
	else if (lOptions == AWP_NORM_L2)
		_awpNormL2(pSrc, *ppNorm);
	else
		_ERROR_EXIT_RES_(AWP_BADARG)

	CLEANUP:
	return res;
}

#define _AWP_NORMALIZE_(v)\
{\
	int i, k; \
	v* src = (v*)pSrc->pPixels; \
	AWPDOUBLE* norm = (AWPDOUBLE*)pNorm->pPixels; \
	AWPDOUBLE* res = (AWPDOUBLE*)pNormImg->pPixels; \
for (i = 0; i < pSrc->sSizeX*pSrc->sSizeY; i++) \
{               \
for (k = 0; k < pSrc->bChannels; k++)\
{\
	res[i*pSrc->bChannels + k] = src[i*pSrc->bChannels + k] / norm[k]; \
}\
}\
}\

static void _awpNormalize(awpImage* pSrc, awpImage* pNormImg, awpImage* pNorm)
{
	switch (pSrc->dwType)
	{
	case AWP_BYTE:
		_AWP_NORMALIZE_(AWPBYTE)
			break;
	case AWP_SHORT:
		_AWP_NORMALIZE_(AWPSHORT)
			break;
	case AWP_FLOAT:
		_AWP_NORMALIZE_(AWPFLOAT)
			break;
	case AWP_DOUBLE:
		_AWP_NORMALIZE_(AWPDOUBLE)
			break;
	}
}

AWPRESULT awpNormalize(awpImage* pSrc, awpImage** ppNorm, long lOptions)
{
	AWPRESULT res;
	awpImage* pNorm;
	AWPBYTE i;
	AWPDOUBLE* data;
	res = AWP_OK;/*initialize result */
	pNorm = NULL;
	data = NULL;

	_CHECK_RESULT_((res = awpCheckImage(pSrc)))

	if (ppNorm == NULL)
		_ERROR_EXIT_RES_(AWP_BADARG)

	if (*ppNorm == NULL)
		_CHECK_RESULT_((res = awpCreateImage(ppNorm, pSrc->sSizeX, pSrc->sSizeY, pSrc->bChannels, AWP_DOUBLE)))
	else
	{
		_CHECK_RESULT_((res = awpCheckImage(*ppNorm)))
		if ((*ppNorm)->bChannels != pSrc->bChannels)
			_ERROR_EXIT_RES_(AWP_BADARG)
		if ((*ppNorm)->sSizeY != pSrc->sSizeY)
			_ERROR_EXIT_RES_(AWP_BADARG)
		if ((*ppNorm)->sSizeX != pSrc->sSizeX)
			_ERROR_EXIT_RES_(AWP_BADARG)
		if ((*ppNorm)->dwType != AWP_DOUBLE)
			_ERROR_EXIT_RES_(AWP_BADARG)
	}

	_CHECK_RESULT_((res = awpNorm(pSrc, &pNorm, lOptions)))

	// check norm 
	data = _AWP_BPIX_(pNorm, AWPDOUBLE)
	for (i = 0; i < pNorm->bChannels; i++)
	{
		if (data[i] == 0)
			data[i] = 1;

	}
	//call normalize
	_awpNormalize(pSrc, *ppNorm, pNorm);


CLEANUP:
	_SAFE_RELEASE_(pNorm)
	return res;
}
