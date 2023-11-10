/*M
//
//      awpipl2 image processing and image analysis library
//		File: awpchannels.c
//		Purpose: work with awpImage channels 
//		Autors: Alex A.Telnykh (tlnykha@yahoo.com)
//      CopyRight 2004-2018 (c) NN-Videolab.net
//M*/
#include "_awpipl.h"

AWPRESULT awpGetChannel(const awpImage *pImage, awpImage **ppChannel, const AWPBYTE bOptions)
{
    AWPINT iImageType  = 0;
    AWPINT iBytesCount = 0;
    AWPINT iIndex      = 0;

    AWPBYTE *pTempBuf = NULL;
    const AWPBYTE *pSrcBuf  = NULL;

    awpImage *pTempImage = NULL;

    AWPRESULT res;
    res = AWP_OK;
                                    
    _CHECK_RESULT_((res = awpCheckImage(pImage)))

    if(bOptions >= pImage->bChannels)
        {
            res = AWP_BADARG;
            _ERR_EXIT_
	}

    _CHECK_RESULT_((res = awpGetImagePixelType(pImage, &iImageType)))

    switch(iImageType) {
    case AWP_BYTE :  iBytesCount = 1; break;
    case AWP_SHORT:  iBytesCount = sizeof(AWPSHORT)/sizeof(AWPBYTE);  break;
    case AWP_FLOAT:  iBytesCount = sizeof(AWPFLOAT)/sizeof(AWPBYTE);  break;
    case AWP_DOUBLE: iBytesCount = sizeof(AWPDOUBLE)/sizeof(AWPBYTE); break;

    default:
        {
            res = AWP_BADARG;
            _ERR_EXIT_
        }
    }

    _CHECK_RESULT_((res = awpCreateImage(&pTempImage, pImage->sSizeX, pImage->sSizeY, 1, iImageType)))
    
    pTempBuf = (AWPBYTE*)pTempImage->pPixels;
    pSrcBuf  = (AWPBYTE*)pImage->pPixels + bOptions*iBytesCount*sizeof(AWPBYTE)   ;

    for(iIndex = 0; iIndex < pImage->sSizeX*pImage->sSizeY-1; ++iIndex )
    {
            memcpy(pTempBuf, pSrcBuf, iBytesCount*sizeof(AWPBYTE));

            pTempBuf += iBytesCount*sizeof(AWPBYTE);
            pSrcBuf  += pImage->bChannels*iBytesCount*sizeof(AWPBYTE);
            
	}

    _CHECK_RESULT_((res = awpCopyImage(pTempImage, ppChannel))) 

CLEANUP:
	_SAFE_RELEASE_(pTempImage)
	return res;
}

#define _AWP_GET_CHANNEL_(v) {AWPINT i;\
	v* bpix = (v*)pImage->pPixels;\
	v* cpix = (v*)pChannel->pPixels;\
	for (i = 0; i < pImage->sSizeX*pImage->sSizeY; i++)\
	{\
		cpix[i] = bpix[pImage->bChannels*i + bChannel];\
}}\


AWPRESULT awpGetChannel2(awpImage* pImage, awpImage* pChannel, AWPBYTE bChannel)
{
	AWPRESULT res = AWP_OK;
	_CHECK_RESULT_(res = awpCheckImage(pImage))
    _CHECK_RESULT_(res = awpCheckImage(pChannel))

    _CHECK_SAME_SIZES(pImage, pChannel)
    _CHECK_SAME_TYPE(pImage,pChannel)	

	if (bChannel >= pImage->bChannels)
		_ERROR_EXIT_RES_(AWP_BADARG);

	if (pChannel->bChannels != 1)
		_ERROR_EXIT_RES_(AWP_BADARG);
	switch(pImage->dwType)
	{
	case AWP_BYTE:
		_AWP_GET_CHANNEL_(AWPBYTE)	
		break;
	case AWP_SHORT:
		_AWP_GET_CHANNEL_(short)	
		break;
	case AWP_FLOAT:
		_AWP_GET_CHANNEL_(AWPFLOAT)	
		break;
	case AWP_DOUBLE:
		_AWP_GET_CHANNEL_(AWPDOUBLE)	
		break;
	}


CLEANUP:
    return res;
}


#define _AWP_PUT_CHANNEL_(v) {AWPINT i;\
	v* bpix = (v*)pImage->pPixels;\
	v* cpix = (v*)pChannel->pPixels;\
	for (i = 0; i < pImage->sSizeX*pImage->sSizeY; i++)\
	{\
		bpix[pImage->bChannels*i + bIndex] = cpix[i];\
}}\
		

/*
	Inserts the channel number bOptions into the image pImage

*/
AWPRESULT awpPutChannel(awpImage* pImage, const awpImage* pChannel, const AWPBYTE bIndex)
{

	AWPRESULT res = AWP_OK;
	_CHECK_RESULT_(res = awpCheckImage(pImage))
    _CHECK_RESULT_(res = awpCheckImage(pChannel))

    _CHECK_SAME_SIZES(pImage, pChannel)
    _CHECK_SAME_TYPE(pImage,pChannel)	
	
	if (bIndex >= pImage->bChannels)
		_ERROR_EXIT_RES_(AWP_BADARG);

	if (pChannel->bChannels != 1)
		_ERROR_EXIT_RES_(AWP_BADARG);

	switch(pImage->dwType)
	{
	case AWP_BYTE:
		_AWP_PUT_CHANNEL_(AWPBYTE)	
		break;
	case AWP_SHORT:
		_AWP_PUT_CHANNEL_(short)	
		break;
	case AWP_FLOAT:
		_AWP_PUT_CHANNEL_(AWPFLOAT)	
		break;
	case AWP_DOUBLE:
		_AWP_PUT_CHANNEL_(AWPDOUBLE)	
		break;
	}

CLEANUP:
    return res;
}

