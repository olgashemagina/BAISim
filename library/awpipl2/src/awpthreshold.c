/*proc/awpipl-v2/src/awpthreshold.c,v $
    $Revision: 1.18 $
    $Date: 2005/02/17 07:42:55 $
	
    Authors: Eugene Eremin <medlab@appl.sci-nnov.ru>
	Michael Sorokin <sma@awp.nnov.ru>
	Alexander Telnykh <telnykha@yahoo.com>
*/

#include "_awpipl.h"



#define _MAKE_BINARY_(type)  \
   AWPSHORT x,y;\
   AWPDOUBLE v1 = lOptions == AWP_BINARY?MaxValue:MinValue;\
   AWPDOUBLE v2 = lOptions == AWP_BINARY?MinValue:MaxValue;\
   type* rr = (type*)dst->pPixels;\
   type* ss = (type*)src->pPixels;\
	for (y = r.top; y < r.bottom; y++)\
	{\
	  for (x = r.left; x < r.right; x++)\
	  {\
		rr[x+y*src->sSizeX] = ss[x+y*src->sSizeX] > sThreshold? (type)v1 : (type)v2;\
	  }\
	}\

static AWPRESULT _awpMakeBinaryBYTE(const awpImage* src, awpImage* dst, const AWPDOUBLE sThreshold, 
									const AWPLONG lOptions, awpRect r, AWPDOUBLE MaxValue, AWPDOUBLE MinValue)
{
	_MAKE_BINARY_(AWPBYTE)

	return AWP_OK;
}

static AWPRESULT _awpMakeBinarySHORT(const awpImage* src, awpImage* dst, const AWPDOUBLE sThreshold, 
									 const AWPLONG lOptions, awpRect r, AWPDOUBLE MaxValue, AWPDOUBLE MinValue)
{
	_MAKE_BINARY_(AWPSHORT)

	return AWP_OK;
}

static AWPRESULT _awpMakeBinaryFLOAT(const awpImage* src, awpImage* dst, const AWPDOUBLE sThreshold, 
									 const AWPLONG lOptions, awpRect r, AWPDOUBLE MaxValue, AWPDOUBLE MinValue)
{
	_MAKE_BINARY_(AWPFLOAT)

	return AWP_OK;
}

static AWPRESULT _awpMakeBinaryDOUBLE(const awpImage* src, awpImage* dst, const AWPDOUBLE sThreshold, 
									  const AWPLONG lOptions, awpRect r, AWPDOUBLE MaxValue, AWPDOUBLE MinValue)
{
	_MAKE_BINARY_(AWPDOUBLE)

	return AWP_OK;
}

AWPRESULT awpMakeBinary(const awpImage* pImage, awpImage** ppBinImage, const AWPDOUBLE sThreshold, const AWPLONG lOptions,
						AWPDOUBLE MinValue, AWPDOUBLE MaxValue, awpRect* pRect)
{
	 AWPRESULT res;
	 awpImage* dst;
	 awpRect   r;
	 /* check the input parameters for validity
	 */
	 if (pImage == NULL || ppBinImage == NULL)
		 _ERROR_EXIT_RES_(AWP_BADARG)

     if(lOptions != AWP_BINARY && lOptions != AWP_INV_BINARY)
		 _ERROR_EXIT_RES_(AWP_BADARG)

	 if (pImage->bChannels != 1)
		 _ERROR_EXIT_RES_(AWP_BADARG)
	 if (pRect != NULL)
	 {
		_CHECK_RESULT_(res = awpRectInImage(pImage, pRect))
	 }

     /* Preparation of the resulting image 
	 */
	 dst = *ppBinImage;
	 if (dst == NULL)
	 {
	    /* there is not resulting image. create it 
		*/
		 _CHECK_RESULT_(res = awpCreateImage(&dst, pImage->sSizeX, pImage->sSizeY, pImage->bChannels, pImage->dwType) )
	 }
	 else
	 {
		/* check on the validity of the resulting image
		*/		
		 _CHECK_SAME_SIZES(pImage, dst)
		 _CHECK_SAME_TYPE(pImage, dst)
		 _CHECK_NUM_CHANNELS(pImage, dst)
	 }
	 /* establish the spatial range of image processing
	 */
	 if (pRect == NULL)
	 {
		 r.left = 0;
		 r.top = 0;
		 r.right = pImage->sSizeX;
		 r.bottom = pImage->sSizeY;
	 }
	 else
	 {
		 r = *pRect;
	 }
	 /* carry out the transformation
	 */
	 switch (pImage->dwType)
	 {
	 case AWP_BYTE:
		 res = _awpMakeBinaryBYTE(pImage, dst, sThreshold, lOptions, r, MaxValue, MinValue);
		 break;
	 case AWP_SHORT:
		 res = _awpMakeBinarySHORT(pImage, dst, sThreshold, lOptions, r, MaxValue, MinValue);
		 break;
	 case AWP_FLOAT:
		 res = _awpMakeBinaryFLOAT(pImage, dst, sThreshold, lOptions, r, MaxValue, MinValue);
		 break;
	 case AWP_DOUBLE:
		 res = _awpMakeBinaryDOUBLE(pImage, dst, sThreshold, lOptions, r, MaxValue, MinValue);
		 break;
	 }


     *ppBinImage = dst;
CLEANUP:
	return res;
}

AWPRESULT __awpMakeBinary(const awpImage* pImage, awpImage** ppBinImage, const AWPDOUBLE sThreshold, const AWPLONG lOptions)
{
    awpImage* pTempImage;
    AWPINT iImageType;
    AWPINT iIndex;
    AWPINT iImageSize;

	void *pTempBuf;
    void *pSrcBuf;

    AWPRESULT res;
	
    pTempImage = NULL;
    iImageType = 0;
	iImageSize = 0;

	/*BUG: need checked theshold depended of type image */

    //if( sThreshold < 0 || sThreshold > 255 ||
    if(lOptions != AWP_BINARY && lOptions != AWP_INV_BINARY)
    {
        res = AWP_BADARG;
        _ERR_EXIT_
    }

    _CHECK_RESULT_(( res = awpCheckImage(pImage)))
    _CHECK_RESULT_(( res = awpGetImagePixelType(pImage, &iImageType)))

    switch(iImageType){
    case AWP_BYTE:
        if( sThreshold < 0 || sThreshold > 255)
        {
            res = AWP_BADARG;
            _ERR_EXIT_
        }
        break;
    case AWP_SHORT:
        if( sThreshold < AWP_SHRT_MIN || sThreshold > AWP_SHRT_MAX )
        {
            res = AWP_BADARG;
            _ERR_EXIT_
        }
        break;
    case AWP_FLOAT:
        if( sThreshold < FLT_MIN || sThreshold > FLT_MAX )
        {
            res = AWP_BADARG;
            _ERR_EXIT_
        }
        break;
    }

    _CHECK_RESULT_(( res = awpGetImageSize(pImage, &iImageSize)))
    if (ppBinImage != NULL){
        _CHECK_RESULT_(( res = awpCreateImage(&pTempImage, pImage->sSizeX, pImage->sSizeY, pImage->bChannels, iImageType)))
    }else{
        pTempImage = (awpImage*)pImage;
    }

    switch(iImageType)
    {
    case AWP_BYTE:   pSrcBuf  = (AWPBYTE*)   pImage->pPixels;
        pTempBuf = (AWPBYTE*)   pTempImage->pPixels;  break;
    case AWP_SHORT:  pSrcBuf  = (AWPSHORT*)  pImage->pPixels;
        pTempBuf = (AWPSHORT*)  pTempImage->pPixels; break;
    case AWP_FLOAT:  pSrcBuf  = (AWPFLOAT*)  pImage->pPixels;
        pTempBuf = (AWPFLOAT*)  pTempImage->pPixels; break;
    case AWP_DOUBLE: pSrcBuf  = (AWPDOUBLE*) pImage->pPixels;
        pTempBuf = (AWPDOUBLE*) pTempImage->pPixels; break;

    default:
        {
            res = AWP_BADARG;
            _ERR_EXIT_
        }
    }

    if(lOptions == AWP_BINARY)
        for(iIndex = 0; iIndex < pImage->sSizeX*pImage->sSizeY*pImage->bChannels; ++iIndex)
        {
            switch(iImageType)
            {
            case AWP_BYTE:   ((AWPBYTE*)  pTempBuf)[iIndex] = (((AWPBYTE*)  pSrcBuf)[iIndex] > sThreshold)?255:0; break;
            case AWP_SHORT:  ((AWPSHORT*) pTempBuf)[iIndex] = (((AWPSHORT*) pSrcBuf)[iIndex] > sThreshold)?255:0; break;
            case AWP_FLOAT:  ((AWPFLOAT*) pTempBuf)[iIndex] = (((AWPFLOAT*) pSrcBuf)[iIndex] > sThreshold)?255.f:0; break;
            case AWP_DOUBLE: ((AWPDOUBLE*)pTempBuf)[iIndex] = (((AWPDOUBLE*)pSrcBuf)[iIndex] > sThreshold)?255.:0; break;

            default:
                {
                    res = AWP_BADARG;
                    _ERR_EXIT_
                }
            }
        }
    else
        for(iIndex = 0; iIndex < pImage->sSizeX*pImage->sSizeY*pImage->bChannels; ++iIndex)
        {
            switch(iImageType)
            {
            case AWP_BYTE:   ((AWPBYTE*)  pTempBuf)[iIndex] = (((AWPBYTE*)  pSrcBuf)[iIndex] > sThreshold)?0:255; break;
            case AWP_SHORT:  ((AWPSHORT*) pTempBuf)[iIndex] = (((AWPSHORT*) pSrcBuf)[iIndex] > sThreshold)?0:255; break;
            case AWP_FLOAT:  ((AWPFLOAT*) pTempBuf)[iIndex] = (((AWPFLOAT*) pSrcBuf)[iIndex] > sThreshold)?0:255.f; break;
            case AWP_DOUBLE: ((AWPDOUBLE*)pTempBuf)[iIndex] = (((AWPDOUBLE*)pSrcBuf)[iIndex] > sThreshold)?0:255.; break;

            default:
                {
                    res = AWP_BADARG;
                    _ERR_EXIT_
                }
            }
        }
    if (ppBinImage!= NULL){
        _CHECK_RESULT_((res = awpCopyImage(pTempImage, ppBinImage)))
    }


CLEANUP:
    if (ppBinImage != NULL){
        _SAFE_RELEASE_(pTempImage)
    }
    return res;
}
