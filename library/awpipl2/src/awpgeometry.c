
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "_awpipl.h"

static AWPRESULT _awpFlipVert(awpImage** ppImageSrc)
{
    AWPRESULT res ;

    AWPBYTE* pTempBuf = NULL;
    AWPBYTE* pTop     = NULL;
    AWPBYTE* pBottom  = NULL;

    awpImage* pImage = NULL;

    AWPINT iBytesCount = 0;
    AWPINT iRowsCount  = 0;
    AWPINT iImageType  = 0;
    res = AWP_OK;
    pImage = *ppImageSrc;

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

    if((pTempBuf = (AWPBYTE*)malloc(pImage->sSizeX*pImage->bChannels*iBytesCount*sizeof(AWPBYTE))) == NULL)
    {
        res = AWP_NOTENOUGH_MEM;
        _ERR_EXIT_
    }

    pBottom = (AWPBYTE*)pImage->pPixels;
    pTop    = pBottom + (pImage->sSizeY - 1)*pImage->sSizeX*pImage->bChannels*iBytesCount;

    for( iRowsCount = 0; iRowsCount < pImage->sSizeY/2; ++iRowsCount)
    {
        memcpy(pTempBuf, pBottom, pImage->sSizeX*pImage->bChannels*iBytesCount*sizeof(AWPBYTE));
        memcpy(pBottom, pTop, pImage->sSizeX*pImage->bChannels*iBytesCount*sizeof(AWPBYTE));
        memcpy(pTop, pTempBuf, pImage->sSizeX*pImage->bChannels*iBytesCount*sizeof(AWPBYTE));

        pBottom += pImage->sSizeX*pImage->bChannels*iBytesCount*sizeof(AWPBYTE);
        pTop    -= pImage->sSizeX*pImage->bChannels*iBytesCount*sizeof(AWPBYTE);

    }

CLEANUP:

    if(pTempBuf != NULL)
        free(pTempBuf);
    
    return res;
}

static AWPRESULT _awpFlipHrzt(awpImage** ppImageSrc)
{
    AWPRESULT res ;

    AWPBYTE* pData_Left  = NULL;
    AWPBYTE* pData_Right = NULL;
    AWPBYTE* pTempBuf    = NULL;

    awpImage *pImage = NULL;

    AWPINT iRowsCount   = 0;
    AWPINT iColsCount   = 0;
    AWPINT iBytesCount = 0;
    AWPINT iImageType  = 0;

    res = AWP_OK;
    pImage = *ppImageSrc;

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

    if((pTempBuf = (AWPBYTE*)malloc(pImage->bChannels*iBytesCount*sizeof(AWPBYTE))) == NULL)
    {
        res = AWP_NOTENOUGH_MEM;
        _ERR_EXIT_
    }

    pData_Left  = (AWPBYTE*)pImage->pPixels;
    pData_Right = (AWPBYTE*)pImage->pPixels + (pImage->sSizeX - 1)*iBytesCount*pImage->bChannels;

    for(iRowsCount = 0; iRowsCount < pImage->sSizeY; iRowsCount++)
    {
        for(iColsCount = 0; iColsCount < pImage->sSizeX/2  ; iColsCount++)
        {
            memcpy(pTempBuf, pData_Left, pImage->bChannels*iBytesCount);
            memcpy(pData_Left, pData_Right, pImage->bChannels*iBytesCount);
            memcpy(pData_Right, pTempBuf, pImage->bChannels*iBytesCount);

            pData_Left  += pImage->bChannels*iBytesCount;
            pData_Right -= pImage->bChannels*iBytesCount;
        }

        pData_Left  = &((AWPBYTE*)pImage->pPixels)[ pImage->sSizeX*(iRowsCount + 1)*pImage->bChannels*iBytesCount];
        pData_Right = &((AWPBYTE*)pImage->pPixels)[ pImage->sSizeX*(iRowsCount + 1)*pImage->bChannels*iBytesCount + (pImage->sSizeX - 1)*iBytesCount*pImage->bChannels];
    }

CLEANUP:

    if(pTempBuf != NULL)
        free(pTempBuf);
   
    return res;
}

static AWPRESULT _awpFlipRght(awpImage** ppImageSrc)
{
    AWPRESULT res ;

    awpImage *pImage     = NULL;
    awpImage *pTempImage = NULL;

    AWPBYTE* pData      = NULL;
    AWPBYTE* pTempBuf   = NULL;

    AWPINT iIndex     = 0;
    AWPINT iRowsCount = 0;
    AWPINT iColsCount = 0;
    AWPINT iBytesCount = 0;
    AWPINT iImageType  = 0;
    res = AWP_OK;
    pImage = *ppImageSrc;

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

    _CHECK_RESULT_((res = awpCreateImage(&pTempImage, pImage->sSizeY, pImage->sSizeX, pImage->bChannels, pImage->dwType)))

    pData    = (AWPBYTE*)pImage->pPixels;
    pTempBuf = (AWPBYTE*)pTempImage->pPixels;

    for(iRowsCount = 0; iRowsCount<pTempImage->sSizeY; ++iRowsCount)
        for(iColsCount =0; iColsCount<pTempImage->sSizeX; ++iColsCount)
        {
            iIndex = (pTempImage->sSizeX - 1 - iColsCount)*pTempImage->sSizeY + iRowsCount;
            memcpy(pTempBuf, &pData[iIndex*pTempImage->bChannels*iBytesCount], pTempImage->bChannels*iBytesCount);
            pTempBuf += pTempImage->bChannels*iBytesCount;
        }

    _SAFE_RELEASE_((*ppImageSrc))
    _CHECK_RESULT_((res = awpCopyImage(pTempImage, ppImageSrc)))

CLEANUP:

    _SAFE_RELEASE_(pTempImage)
           
    return res;
}

static AWPRESULT _awpFlipLeft(awpImage** ppImageSrc)
{

    AWPRESULT res;

    awpImage *pImage     = NULL;
    awpImage *pTempImage = NULL;

    AWPBYTE* pData      = NULL;
    AWPBYTE* pTempBuf   = NULL;

    AWPINT iIndex     = 0;
    AWPINT iRowsCount = 0;
    AWPINT iColsCount = 0;

    AWPINT iBytesCount = 0;
    AWPINT iImageType  = 0;
    res = AWP_OK;
    pImage = *ppImageSrc;

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

    _CHECK_RESULT_((res = awpCreateImage(&pTempImage, pImage->sSizeY, pImage->sSizeX, pImage->bChannels, pImage->dwType)))

    pData    = (AWPBYTE*)pImage->pPixels;
    pTempBuf = (AWPBYTE*)pTempImage->pPixels;

    for(iRowsCount = 0; iRowsCount<pTempImage->sSizeY; ++iRowsCount)
        for(iColsCount =0; iColsCount<pTempImage->sSizeX; ++iColsCount)
        {
            iIndex = iColsCount*pImage->sSizeX + pImage->sSizeX - iRowsCount - 1;
            memcpy(pTempBuf, &pData[iIndex*pTempImage->bChannels*iBytesCount], pTempImage->bChannels*iBytesCount);
            pTempBuf += pTempImage->bChannels*iBytesCount;
        }

    _SAFE_RELEASE_((*ppImageSrc))
    _CHECK_RESULT_((res = awpCopyImage(pTempImage, ppImageSrc)))

CLEANUP:

    _SAFE_RELEASE_(pTempImage)

    return res;
}

AWPRESULT awpFlip(awpImage** ppImageSrc, const long lOptions)
{
    AWPRESULT res = AWP_OK;

    _CHECK_RESULT_((res = awpCheckImage(*ppImageSrc)))

    switch(lOptions)
    {
    case AWP_FLIP_VERT: _CHECK_RESULT_((res = _awpFlipVert(ppImageSrc))); break;
    case AWP_FLIP_HRZT: _CHECK_RESULT_((res = _awpFlipHrzt(ppImageSrc))); break;
    case AWP_FLIP_LEFT: _CHECK_RESULT_((res = _awpFlipLeft(ppImageSrc))); break;
    case AWP_FLIP_RGHT: _CHECK_RESULT_((res = _awpFlipRght(ppImageSrc))); break;

    default: {
            res = AWP_BADARG;
            _ERR_EXIT_
        }
    }

CLEANUP:
    return res;
}

AWPRESULT awpResize(awpImage* pImageSrc, const AWPWORD wNewWidth, const AWPWORD wNewHeight)
{

    AWPWORD wNewRowsCount;
    AWPWORD wNewColsCount;
    AWPWORD wOldRow;
    AWPWORD wOldCol;
    AWPINT iIndex;
    AWPINT iBytesCount;
    AWPINT iImageType;

    AWPDOUBLE dRowAdress;
    AWPDOUBLE dColAdress;

    AWPBYTE *pDataSrc = NULL;
    AWPBYTE *pDataDst = NULL;

    AWPRESULT res;
    res   = AWP_OK;

    _CHECK_RESULT_((res = awpCheckImage(pImageSrc)))

    if((wNewWidth==pImageSrc->sSizeX)&&(wNewHeight==pImageSrc->sSizeY))
        return AWP_OK;

    _CHECK_RESULT_((res = awpGetImagePixelType(pImageSrc, &iImageType)))

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

    pDataSrc = (AWPBYTE*)pImageSrc->pPixels;

    if((pDataDst = (AWPBYTE*)malloc(wNewWidth*wNewHeight*pImageSrc->bChannels*iBytesCount*sizeof(AWPBYTE))) == NULL){
        res = AWP_NOTENOUGH_MEM;
        _ERR_EXIT_
    }

    iIndex = 0;
    for( wNewRowsCount = 0; wNewRowsCount < wNewHeight; ++wNewRowsCount)
    {
        dRowAdress = wNewRowsCount*pImageSrc->sSizeY/wNewHeight;

        for( wNewColsCount = 0; wNewColsCount < wNewWidth; ++wNewColsCount)
        {

            dColAdress = wNewColsCount*pImageSrc->sSizeX/wNewWidth;
            wOldRow = (AWPINT) dRowAdress;
            wOldCol = (AWPINT) dColAdress;

            memcpy(&pDataDst[iIndex],
                   &pDataSrc[(wOldRow*pImageSrc->sSizeX + wOldCol)*pImageSrc->bChannels*iBytesCount*sizeof(AWPBYTE)],
                   pImageSrc->bChannels*iBytesCount*sizeof(AWPBYTE));

            iIndex += pImageSrc->bChannels*iBytesCount*sizeof(AWPBYTE);
        }
    }

    free(pDataSrc);
    pImageSrc->pPixels = pDataDst;

    pImageSrc->sSizeX = wNewWidth;
    pImageSrc->sSizeY = wNewHeight;


CLEANUP:
    return res;
}

AWPRESULT awpResizeNip(awpImage* pImageSrc, awpImage** ppDst, const AWPWORD wNewWidth, const AWPWORD wNewHeight)
{
    AWPWORD wNewRowsCount;
    AWPWORD wNewColsCount;
    AWPWORD wOldRow;
    AWPWORD wOldCol;
    AWPINT iIndex;
    AWPINT iBytesCount;
    AWPINT iImageType;

    AWPDOUBLE dRowAdress;
    AWPDOUBLE dColAdress;

    AWPBYTE *pDataSrc = NULL;
    AWPBYTE *pDataDst = NULL;

    AWPRESULT res;
    res   = AWP_OK;

    _CHECK_RESULT_((res = awpCheckImage(pImageSrc)))

   // if((wNewWidth==pImageSrc->sSizeX)&&(wNewHeight==pImageSrc->sSizeY))
   //     return AWP_OK;

    _CHECK_RESULT_((res = awpGetImagePixelType(pImageSrc, &iImageType)))

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

    pDataSrc = (AWPBYTE*)pImageSrc->pPixels;
    awpCreateImage(ppDst, wNewWidth, wNewHeight, pImageSrc->bChannels, pImageSrc->dwType);
    pDataDst = (AWPBYTE*)(*ppDst)->pPixels;
/*
    if((pDataDst = (AWPBYTE*)malloc(wNewWidth*wNewHeight*pImageSrc->bChannels*iBytesCount*sizeof(AWPBYTE))) == NULL){
        res = AWP_NOTENOUGH_MEM;
        _ERR_EXIT_
    }
*/
    iIndex = 0;
    for( wNewRowsCount = 0; wNewRowsCount < wNewHeight; ++wNewRowsCount)
    {
        dRowAdress = wNewRowsCount*pImageSrc->sSizeY/wNewHeight;

        for( wNewColsCount = 0; wNewColsCount < wNewWidth; ++wNewColsCount)
        {

            dColAdress = wNewColsCount*pImageSrc->sSizeX/wNewWidth;
            wOldRow = (AWPINT) dRowAdress;
            wOldCol = (AWPINT) dColAdress;

            memcpy(&pDataDst[iIndex],
                   &pDataSrc[(wOldRow*pImageSrc->sSizeX + wOldCol)*pImageSrc->bChannels*iBytesCount*sizeof(AWPBYTE)],
                   pImageSrc->bChannels*iBytesCount*sizeof(AWPBYTE));

            iIndex += pImageSrc->bChannels*iBytesCount*sizeof(AWPBYTE);
        }
    }

//    free(pDataSrc);
//    pImageSrc->pPixels = pDataDst;

//    pImageSrc->sSizeX = wNewWidth;
//    pImageSrc->sSizeY = wNewHeight;

CLEANUP:
    return res;
}


AWPRESULT awpRescale(awpImage *pImageSrc, const AWPFLOAT fWidthScaleFactor, const AWPFLOAT fHeightScaleFactor)
{
    AWPWORD wNewWidth  = 0;
    AWPWORD wNewHeight = 0;

    AWPRESULT res;

    res = AWP_OK;

    _CHECK_RESULT_((res = awpCheckImage(pImageSrc)))

    wNewWidth  = (AWPWORD)(pImageSrc->sSizeX * fWidthScaleFactor  + 0.5);
    wNewHeight = (AWPWORD)(pImageSrc->sSizeY * fHeightScaleFactor + 0.5);

    _CHECK_RESULT_((res = awpResize(pImageSrc, wNewWidth, wNewHeight)))

CLEANUP:
    return res;
}

static AWPBOOL _awpGetInvAffin(const AWPDOUBLE M[7], AWPDOUBLE W[7])
{
    AWPDOUBLE eps = 1.0e-8;
    AWPDOUBLE a = M[1], b = M[2], c = M[3], d = M[4], e = M[5], f = M[6];
    AWPDOUBLE det = a*e - b*d;

    W[0] = 0.0;

    if (det > -eps && det < eps) return FALSE;

    W[1] = e/det; W[2] = -b/det; W[3] = (b*f - e*c)/det;
    W[4] = -d/det;W[5] = a/det;  W[6] = (c*d - a*f)/det;

    return TRUE;
}

AWPRESULT awpRotate( awpImage *pImageSrc, const AWPSHORT sAngle )
{
    AWPWORD wNewRowsCount;
    AWPWORD wNewColsCount;
    AWPWORD wOldRow;
    AWPWORD wOldCol;
    AWPWORD wNewWidth;
    AWPWORD wNewHeight;

    AWPINT iIndex;
    AWPINT iBytesCount;
    AWPINT iImageType;

    AWPDOUBLE dRowAdress;
    AWPDOUBLE dColAdress;
    AWPDOUBLE b,e,ui,vj;
    AWPDOUBLE dDir[7];
    AWPDOUBLE dInv[7];

    AWPDOUBLE dCosAngle;
    AWPDOUBLE dSinAngle;

    AWPBYTE *pDataSrc = NULL;
    AWPBYTE *pDataDst = NULL;

    AWPRESULT res;
    res   = AWP_OK;

    _CHECK_RESULT_((res = awpCheckImage(pImageSrc)))

    dCosAngle = cos(sAngle*AWP_PI/180);
    dSinAngle = sin(sAngle*AWP_PI/180);

    wNewHeight = (AWPWORD)(fabs(pImageSrc->sSizeX*dSinAngle) + fabs(pImageSrc->sSizeY*dCosAngle));
    wNewWidth  = (AWPWORD)(fabs(pImageSrc->sSizeX*dCosAngle) + fabs(pImageSrc->sSizeY*dSinAngle));

    memset((void*)dDir, 0, 7*sizeof(AWPDOUBLE));
    dDir[0] = 0.0;
    dDir[1] = dCosAngle;
    dDir[2] = dSinAngle;
    dDir[3] = (sAngle < 0)?-pImageSrc->sSizeX*dSinAngle:0;
    dDir[4] = -dSinAngle;
    dDir[5] = dCosAngle;
    dDir[6] = (sAngle > 0)?pImageSrc->sSizeY*dSinAngle:0;

    _awpGetInvAffin(dDir, dInv);

    b  =  dInv[2];
    e  =  dInv[5];
    ui = dInv[3];
    vj = dInv[6];


    _CHECK_RESULT_((res = awpGetImagePixelType(pImageSrc, &iImageType)))

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

    pDataSrc = (AWPBYTE*)pImageSrc->pPixels;

    if((pDataDst = (AWPBYTE*)malloc(wNewWidth*wNewHeight*pImageSrc->bChannels*iBytesCount*sizeof(AWPBYTE))) == NULL )
    {
        res = AWP_NOTENOUGH_MEM;
        _ERR_EXIT_
    }


    iIndex = 0;
    for(wNewRowsCount = 0; wNewRowsCount<wNewHeight; ++wNewRowsCount)
    {
        dRowAdress = ui += dInv[1];
        dColAdress = vj += dInv[4];

        for( wNewColsCount = 0; wNewColsCount < wNewWidth; ++wNewColsCount)
        {
            dRowAdress += b;
            dColAdress += e;

            wOldRow = (AWPINT)(dRowAdress);
            wOldCol = (AWPINT)(dColAdress);

            if ( wOldRow >= pImageSrc->sSizeY || wOldCol >= pImageSrc->sSizeX)
                memset(&pDataDst[iIndex], 0, pImageSrc->bChannels*iBytesCount*sizeof(AWPBYTE));
            else
                memcpy(&pDataDst[iIndex],
                       &pDataSrc[(wOldRow*pImageSrc->sSizeX + wOldCol)*pImageSrc->bChannels*iBytesCount*sizeof(AWPBYTE)],
                       pImageSrc->bChannels*iBytesCount*sizeof(AWPBYTE));

            iIndex += pImageSrc->bChannels*iBytesCount*sizeof(AWPBYTE);
        }
    }

    free(pDataSrc);
    pImageSrc->pPixels = pDataDst;

    pImageSrc->sSizeX = wNewWidth;
    pImageSrc->sSizeY = wNewHeight;

CLEANUP:
    return res;
}

AWPRESULT awpRotate2(awpImage *pImageSrc, const AWPDOUBLE dCosAngle, const AWPDOUBLE dSinAngle )
{
    AWPWORD wNewRowsCount;
    AWPWORD wNewColsCount;
    AWPWORD wOldRow;
    AWPWORD wOldCol;
    AWPWORD wNewWidth;
    AWPWORD wNewHeight;

    AWPINT iIndex;
    AWPINT iBytesCount;
    AWPINT iImageType;

    AWPDOUBLE dRowAdress;
    AWPDOUBLE dColAdress;
    AWPDOUBLE b,e,ui,vj;
    AWPDOUBLE dDir[7];
    AWPDOUBLE dInv[7];

    AWPBYTE *pDataSrc = NULL;
    AWPBYTE *pDataDst = NULL;

    AWPRESULT res;
    res   = AWP_OK;

    _CHECK_RESULT_((res = awpCheckImage(pImageSrc)))

    wNewHeight = (AWPWORD)(fabs(pImageSrc->sSizeX*dSinAngle) + fabs(pImageSrc->sSizeY*dCosAngle));
    wNewWidth  = (AWPWORD)(fabs(pImageSrc->sSizeX*dCosAngle) + fabs(pImageSrc->sSizeY*dSinAngle));

    memset((void*)dDir, 0, 7*sizeof(AWPDOUBLE));
    dDir[0] = 0.0;
    dDir[1] = dCosAngle;
    dDir[2] = dSinAngle;
    dDir[3] = (dSinAngle < 0)?-pImageSrc->sSizeX*dSinAngle:0;
    dDir[4] = -dSinAngle;
    dDir[5] = dCosAngle;
    dDir[6] = (dSinAngle > 0)?pImageSrc->sSizeY*dSinAngle:0;

    _awpGetInvAffin(dDir, dInv);

    b  =  dInv[2];
    e  =  dInv[5];
    ui = dInv[3];
    vj = dInv[6];


    _CHECK_RESULT_((res = awpGetImagePixelType(pImageSrc, &iImageType)))

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

    pDataSrc = (AWPBYTE*)pImageSrc->pPixels;

    if((pDataDst = (AWPBYTE*)malloc(wNewWidth*wNewHeight*pImageSrc->bChannels*iBytesCount*sizeof(AWPBYTE))) == NULL )
    {
        res = AWP_NOTENOUGH_MEM;
        _ERR_EXIT_
    }


    iIndex = 0;
    for(wNewRowsCount = 0; wNewRowsCount<wNewHeight; ++wNewRowsCount)
    {
        dRowAdress = ui += dInv[1];
        dColAdress = vj += dInv[4];

        for( wNewColsCount = 0; wNewColsCount < wNewWidth; ++wNewColsCount)
        {
            dRowAdress += b;
            dColAdress += e;

            wOldRow = (AWPINT)(dRowAdress);
            wOldCol = (AWPINT)(dColAdress);

            if ( wOldRow >= pImageSrc->sSizeY || wOldCol >= pImageSrc->sSizeX)
                memset(&pDataDst[iIndex], 0, pImageSrc->bChannels*iBytesCount*sizeof(AWPBYTE));
            else
                memcpy(&pDataDst[iIndex],
                &pDataSrc[(wOldRow*pImageSrc->sSizeX + wOldCol)*pImageSrc->bChannels*iBytesCount*sizeof(AWPBYTE)],
                pImageSrc->bChannels*iBytesCount*sizeof(AWPBYTE));

            iIndex += pImageSrc->bChannels*iBytesCount*sizeof(AWPBYTE);
        }
    }

    free(pDataSrc);
    pImageSrc->pPixels = pDataDst;

    pImageSrc->sSizeX = wNewWidth;
    pImageSrc->sSizeY = wNewHeight;

CLEANUP:
    return res;

}

AWPRESULT awpRotateCenter( awpImage *pImageSrc, const AWPSHORT sAngle, AWPSHORT xCenter, AWPSHORT yCenter )
{
    AWPWORD wNewRowsCount;
    AWPWORD wNewColsCount;
    AWPINT wOldRow;
    AWPINT wOldCol;
    AWPWORD wWidth;
    AWPWORD wHeight;

    AWPINT iIndex;
    AWPINT iBytesCount;
    AWPINT iImageType;

    AWPDOUBLE dRowAdress;
    AWPDOUBLE dColAdress;
    AWPDOUBLE ui,vj;
    AWPDOUBLE dDir[7];
    AWPDOUBLE dInv[7];

    AWPDOUBLE dCosAngle;
    AWPDOUBLE dSinAngle;

    AWPBYTE *pDataSrc = NULL;
    AWPBYTE *pDataDst = NULL;

    AWPRESULT res;
    res   = AWP_OK;

    _CHECK_RESULT_((res = awpCheckImage(pImageSrc)))

    dCosAngle = cos(sAngle*AWP_PI/180);
    dSinAngle = sin(sAngle*AWP_PI/180);

    wWidth = pImageSrc->sSizeX;
    wHeight = pImageSrc->sSizeY;

    memset((void*)dDir, 0, 7*sizeof(AWPDOUBLE));
    dDir[0] = 0.0;
    dDir[1] = dCosAngle;
    dDir[2] = -dSinAngle;
    dDir[3] = 0;
    dDir[4] = dSinAngle;
    dDir[5] = dCosAngle;
    dDir[6] = 0;

    _awpGetInvAffin(dDir, dInv);

    vj = -xCenter*dInv[1] - yCenter*dInv[2] + xCenter; //x coord
    ui = -xCenter*dInv[4] - yCenter*dInv[5] + yCenter; //y coord

    _CHECK_RESULT_((res = awpGetImagePixelType(pImageSrc, &iImageType)))

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

    pDataSrc = (AWPBYTE*)pImageSrc->pPixels;

    if((pDataDst = (AWPBYTE*)malloc(wWidth*wHeight*pImageSrc->bChannels*iBytesCount*sizeof(AWPBYTE))) == NULL )
    {
        res = AWP_NOTENOUGH_MEM;
        _ERR_EXIT_
    }

    iIndex = 0;
    for(wNewRowsCount = 0; wNewRowsCount<wHeight; ++wNewRowsCount)
    {
        dColAdress = vj += dInv[2]; //x coord
        dRowAdress = ui += dInv[5]; //y coord

        for( wNewColsCount = 0; wNewColsCount < wWidth; ++wNewColsCount)
        {
            dColAdress += dInv[1];
            dRowAdress += dInv[4];

            wOldRow = (AWPINT)(dRowAdress);
            wOldCol = (AWPINT)(dColAdress);

            if ( wOldRow >= wHeight || wOldCol >= wWidth || wOldRow < 0 || wOldCol < 0 )
                memset(&pDataDst[iIndex], 0, pImageSrc->bChannels*iBytesCount*sizeof(AWPBYTE));
            else
                memcpy(&pDataDst[iIndex],
                       &pDataSrc[(wOldRow*wWidth + wOldCol)*pImageSrc->bChannels*iBytesCount*sizeof(AWPBYTE)],
                       pImageSrc->bChannels*iBytesCount*sizeof(AWPBYTE));

            iIndex += pImageSrc->bChannels*iBytesCount*sizeof(AWPBYTE);
        }
    }

    free(pDataSrc);
    pImageSrc->pPixels = pDataDst;

CLEANUP:
    return res;
}

AWPRESULT awpRotateCenter2( awpImage *pImageSrc, const AWPDOUBLE dCosAngle, const AWPDOUBLE dSinAngle, AWPSHORT xCenter, AWPSHORT yCenter )
{
    AWPWORD wNewRowsCount;
    AWPWORD wNewColsCount;
    AWPINT wOldRow;
    AWPINT wOldCol;
    AWPWORD wWidth;
    AWPWORD wHeight;

    AWPINT iIndex;
    AWPINT iBytesCount;
    AWPINT iImageType;

    AWPDOUBLE dRowAdress;
    AWPDOUBLE dColAdress;
    AWPDOUBLE ui,vj;
    AWPDOUBLE dDir[7];
    AWPDOUBLE dInv[7];

    //AWPDOUBLE dCosAngle;
    //AWPDOUBLE dSinAngle;

    AWPBYTE *pDataSrc = NULL;
    AWPBYTE *pDataDst = NULL;

    AWPRESULT res;
    res   = AWP_OK;

    _CHECK_RESULT_((res = awpCheckImage(pImageSrc)))

        //dCosAngle = cos(sAngle*M_PI/180);
    //dSinAngle = sin(sAngle*M_PI/180);

    wWidth = pImageSrc->sSizeX;
    wHeight = pImageSrc->sSizeY;

    memset((void*)dDir, 0, 7*sizeof(AWPDOUBLE));
    dDir[0] = 0.0;
    dDir[1] = dCosAngle;
    dDir[2] = -dSinAngle;
    dDir[3] = 0;
    dDir[4] = dSinAngle;
    dDir[5] = dCosAngle;
    dDir[6] = 0;

    _awpGetInvAffin(dDir, dInv);

    vj = -xCenter*dInv[1] - yCenter*dInv[2] + xCenter; //x coord
    ui = -xCenter*dInv[4] - yCenter*dInv[5] + yCenter; //y coord

    _CHECK_RESULT_((res = awpGetImagePixelType(pImageSrc, &iImageType)))

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

    pDataSrc = (AWPBYTE*)pImageSrc->pPixels;

    if((pDataDst = (AWPBYTE*)malloc(wWidth*wHeight*pImageSrc->bChannels*iBytesCount*sizeof(AWPBYTE))) == NULL )
    {
        res = AWP_NOTENOUGH_MEM;
        _ERR_EXIT_
    }

    iIndex = 0;
    for(wNewRowsCount = 0; wNewRowsCount<wHeight; ++wNewRowsCount)
    {
        dColAdress = vj += dInv[2]; //x coord
        dRowAdress = ui += dInv[5]; //y coord

        for( wNewColsCount = 0; wNewColsCount < wWidth; ++wNewColsCount)
        {
            dColAdress += dInv[1];
            dRowAdress += dInv[4];

            wOldRow = (AWPINT)(dRowAdress);
            wOldCol = (AWPINT)(dColAdress);

            if ( wOldRow >= wHeight || wOldCol >= wWidth || wOldRow < 0 || wOldCol < 0 )
                memset(&pDataDst[iIndex], 0, pImageSrc->bChannels*iBytesCount*sizeof(AWPBYTE));
            else
                memcpy(&pDataDst[iIndex],
                &pDataSrc[(wOldRow*wWidth + wOldCol)*pImageSrc->bChannels*iBytesCount*sizeof(AWPBYTE)],
                pImageSrc->bChannels*iBytesCount*sizeof(AWPBYTE));

            iIndex += pImageSrc->bChannels*iBytesCount*sizeof(AWPBYTE);
        }
    }

    free(pDataSrc);
    pImageSrc->pPixels = pDataDst;

CLEANUP:
    return res;
}


AWPRESULT awpResizeBilinear(awpImage* pImageSrc, const AWPWORD wNewWidth, const AWPWORD wNewHeight)
{
    AWPINT iIndex;
    AWPINT iBytesCount;
    AWPINT iImageType;
	AWPWORD cols, rows; //for increase speed
	AWPBYTE chan_count; //for increase speed
	AWPDWORD chan_bytes; //for increase speed
	AWPWORD x, y;
	AWPWORD x_min, x_max, y_min, y_max;
	AWPBYTE c;
	AWPDOUBLE coeffs[4];
	AWPDOUBLE val;
	AWPDOUBLE kx, ky;
	AWPDOUBLE sx, sy;
	AWPDWORD addr_1, addr_2, addr_3, addr_4;

    AWPDOUBLE dRowAddress;
    AWPDOUBLE dColAddress;

    AWPBYTE *pDataSrcBYTE = NULL;
//	SHORT *pDataSrcSHORT = NULL;
//	AWPFLOAT *pDataSrcFLOAT = NULL;
//	AWPDOUBLE *pDataSrcAWPAWPDOUBLE = NULL;
    AWPBYTE *pDataDstBYTE = NULL;

    AWPRESULT res;
    res   = AWP_OK;

    _CHECK_RESULT_((res = awpCheckImage(pImageSrc)))

    if((wNewWidth==pImageSrc->sSizeX)&&(wNewHeight==pImageSrc->sSizeY))
        return AWP_OK;

	if ((pImageSrc->sSizeX < 3) || (pImageSrc->sSizeY < 3))
		_ERROR_EXIT_RES_(AWP_BADARG)

    _CHECK_RESULT_((res = awpGetImagePixelType(pImageSrc, &iImageType)))

    switch(iImageType) {
    case AWP_BYTE :  iBytesCount = sizeof(AWPBYTE); break;
    case AWP_SHORT:  iBytesCount = sizeof(AWPSHORT)/sizeof(AWPBYTE);  break;
    case AWP_FLOAT:  iBytesCount = sizeof(AWPFLOAT)/sizeof(AWPBYTE);  break;
    case AWP_DOUBLE: iBytesCount = sizeof(AWPDOUBLE)/sizeof(AWPBYTE); break;

    default:
        {
            res = AWP_BADARG;
            _ERR_EXIT_
        }
    }

	cols = pImageSrc->sSizeX;
	rows = pImageSrc->sSizeY;
	chan_count = pImageSrc->bChannels;

	switch (iImageType)
	{
	case AWP_BYTE:
		pDataSrcBYTE = (AWPBYTE *) pImageSrc->pPixels;

		if((pDataDstBYTE = (AWPBYTE*)malloc(wNewWidth*wNewHeight*pImageSrc->bChannels*iBytesCount*sizeof(AWPBYTE))) == NULL)
			_ERROR_EXIT_RES_(AWP_NOTENOUGH_MEM)

		iIndex = 0;

		kx = (cols - 1) / (AWPDOUBLE) (wNewWidth - 1);
		ky = (rows - 1) / (AWPDOUBLE) (wNewHeight - 1);

		for(y = 0; y < wNewHeight; y++)
		{
			dRowAddress = y * ky;

			y_min = (AWPWORD) (dRowAddress);
			y_max = y_min + 1;

			if (y_max == rows)
			{
				 y_max--;
				 y_min--;
			}

			sy = dRowAddress - y_min;

			for(x = 0; x < wNewWidth; x++)
			{
				dColAddress = x * kx;

				x_min = (AWPWORD) (dColAddress);
				x_max = x_min + 1;

				if (x_max == cols)
				{
					x_max--;
					x_min--;
				}
	
				sx = dColAddress - x_min;

				chan_bytes = chan_count * iBytesCount;
				addr_1 = (y_min * cols + x_min) * chan_bytes;
				addr_2 = (y_min * cols + x_max) * chan_bytes;
				addr_3 = (y_max * cols + x_min) * chan_bytes;
				addr_4 = (y_max * cols + x_max) * chan_bytes;

				for(c = 0; c < chan_count; c++)
				{

					coeffs[3] = pDataSrcBYTE[addr_4] + pDataSrcBYTE[addr_1] - pDataSrcBYTE[addr_2] - pDataSrcBYTE[addr_3];
					coeffs[2] = pDataSrcBYTE[addr_3] - pDataSrcBYTE[addr_1];
					coeffs[1] = pDataSrcBYTE[addr_2] - pDataSrcBYTE[addr_1];
					coeffs[0] = pDataSrcBYTE[addr_1];

					addr_1 += iBytesCount; addr_2 += iBytesCount;
					addr_3 += iBytesCount; addr_4 += iBytesCount;

					val = coeffs[0] +
						+ coeffs[1]*sx + coeffs[2]*sy
						+ coeffs[3]*sx*sy;

					if (val < 0) val = 0;
					if (val > 255) val = 255;

					pDataDstBYTE[iIndex] = (AWPBYTE) (val);
					iIndex += iBytesCount;
				}
			}
		}
	}

	free(pDataSrcBYTE);
    pImageSrc->pPixels = pDataDstBYTE;

    pImageSrc->sSizeX = wNewWidth;
    pImageSrc->sSizeY = wNewHeight;

CLEANUP:
    return res;
}
AWPRESULT awpResizePiecewiseCubic(awpImage* pImageSrc, const AWPWORD wNewWidth, const AWPWORD wNewHeight)
{
	//derivatives in current point
    typedef struct tagPtDerivs
    {
		AWPDOUBLE dx;
		AWPDOUBLE dy;
    }PtDerivs;

    AWPINT iIndex;
    AWPINT iBytesCount;
    AWPINT iImageType;
	AWPBYTE *pSrcMap = NULL;
	AWPWORD cols, rows; //for increase speed
	AWPBYTE chan_count; //for increase speed
	AWPWORD x, y;
	AWPWORD x_min, x_max, y_min, y_max;
	AWPBYTE c;
	AWPDOUBLE coeffs[16];
	AWPDOUBLE val;
	AWPDOUBLE kx, ky;
	AWPDOUBLE sx, sy, sx2, sy2;
	AWPDWORD base_addr, addr_1, addr_2, addr_3, addr_1_d, addr_2_d, addr_3_d;

    AWPDOUBLE dRowAddress;
    AWPDOUBLE dColAddress;

    AWPBYTE *pDataSrcBYTE = NULL;
//	SHORT *pDataSrcSHORT = NULL;
//	AWPFLOAT *pDataSrcFLOAT = NULL;
//	AWPDOUBLE *pDataSrcAWPAWPDOUBLE = NULL;
    AWPBYTE *pDataDstBYTE = NULL;

    AWPRESULT res;
    res   = AWP_OK;

    _CHECK_RESULT_((res = awpCheckImage(pImageSrc)))

    if((wNewWidth==pImageSrc->sSizeX)&&(wNewHeight==pImageSrc->sSizeY))
        return AWP_OK;

	if ((pImageSrc->sSizeX < 3) || (pImageSrc->sSizeY < 3))
		_ERROR_EXIT_RES_(AWP_BADARG)

    _CHECK_RESULT_((res = awpGetImagePixelType(pImageSrc, &iImageType)))

    switch(iImageType) {
    case AWP_BYTE :  iBytesCount = sizeof(AWPBYTE); break;
    case AWP_SHORT:  iBytesCount = sizeof(AWPSHORT)/sizeof(AWPBYTE);  break;
    case AWP_FLOAT:  iBytesCount = sizeof(AWPFLOAT)/sizeof(AWPBYTE);  break;
    case AWP_DOUBLE: iBytesCount = sizeof(AWPDOUBLE)/sizeof(AWPBYTE); break;

    default:
        {
            res = AWP_BADARG;
            _ERR_EXIT_
        }
    }

	if ((pSrcMap = (AWPBYTE *) malloc(pImageSrc->sSizeX * pImageSrc->sSizeY * pImageSrc->bChannels * sizeof(PtDerivs))) == NULL)
		_ERROR_EXIT_RES_(AWP_NOTENOUGH_MEM)

	cols = pImageSrc->sSizeX;
	rows = pImageSrc->sSizeY;
	chan_count = pImageSrc->bChannels;

	//count derivatives
	switch (iImageType)
	{
	case AWP_BYTE:
		pDataSrcBYTE = (AWPBYTE *) pImageSrc->pPixels;
		for(x = 0; x < cols; x++)
			for(y = 0; y < rows; y++)
				for(c = 0; c < chan_count; c++)
				{
					base_addr = (y * cols + x) * chan_count + c;

					if (x == 0)
						((PtDerivs *) (pSrcMap + base_addr * sizeof(PtDerivs)))->dx = (-3 * pDataSrcBYTE[base_addr * iBytesCount] + 4 * pDataSrcBYTE[(base_addr + chan_count) * iBytesCount] - pDataSrcBYTE[(base_addr + 2 * chan_count) * iBytesCount]) / 2.;
					else if (x == cols - 1)
						((PtDerivs *) (pSrcMap + base_addr * sizeof(PtDerivs)))->dx = (pDataSrcBYTE[(base_addr - 2 * chan_count) * iBytesCount] - 4 * pDataSrcBYTE[(base_addr - chan_count) * iBytesCount] + 3 * pDataSrcBYTE[base_addr * iBytesCount]) / 2.;
					else
						((PtDerivs *) (pSrcMap + base_addr * sizeof(PtDerivs)))->dx = (pDataSrcBYTE[(base_addr + chan_count) * iBytesCount] - pDataSrcBYTE[(base_addr - chan_count) * iBytesCount]) / 2.;

					if (y == 0)
						((PtDerivs *) (pSrcMap + base_addr * sizeof(PtDerivs)))->dy = (-3 * pDataSrcBYTE[base_addr * iBytesCount] + 4 * pDataSrcBYTE[(base_addr + cols * chan_count) * iBytesCount] - pDataSrcBYTE[(base_addr + 2 * cols * chan_count) * iBytesCount]) / 2.;
					else if (y == rows - 1)
						((PtDerivs *) (pSrcMap + base_addr * sizeof(PtDerivs)))->dy = (pDataSrcBYTE[(base_addr - 2 * cols * chan_count) * iBytesCount] - 4 * pDataSrcBYTE[(base_addr - cols * chan_count) * iBytesCount] + 3 * pDataSrcBYTE[base_addr * iBytesCount]) / 2.;
					else
						((PtDerivs *) (pSrcMap + base_addr * sizeof(PtDerivs)))->dy = (pDataSrcBYTE[(base_addr + cols * chan_count) * iBytesCount] - pDataSrcBYTE[(base_addr - cols * chan_count) * iBytesCount]) / 2.;
				}
	}

    if((pDataDstBYTE = (AWPBYTE*)malloc(wNewWidth*wNewHeight*pImageSrc->bChannels*iBytesCount*sizeof(AWPBYTE))) == NULL)
		_ERROR_EXIT_RES_(AWP_NOTENOUGH_MEM)

	iIndex = 0;

	kx = (cols - 1) / (AWPDOUBLE) (wNewWidth - 1);
	ky = (rows - 1) / (AWPDOUBLE) (wNewHeight - 1);

	for(y = 0; y < wNewHeight; y++)
	{
		dRowAddress = y * ky;

		y_min = (AWPWORD) (dRowAddress);
		y_max = y_min + 1;

		if (y_max == rows)
		{
			 y_max--;
			 y_min--;
		}

		sy = dRowAddress - y_min;
		sy2 = sy * sy;

		for(x = 0; x < wNewWidth; x++)
		{
			dColAddress = x * kx;

			x_min = (AWPWORD) (dColAddress);
			x_max = x_min + 1;

			if (x_max == cols)
			{
				x_max--;
				x_min--;
			}

			sx = dColAddress - x_min;
			sx2 = sx * sx;

			addr_1 = (y_min * cols + x_min) * chan_count;
			addr_2 = (y_min * cols + x_max) * chan_count;
			addr_3 = (y_max * cols + x_min) * chan_count;
			addr_1_d = addr_1 * sizeof(PtDerivs);
			addr_2_d = addr_2 * sizeof(PtDerivs);
			addr_3_d = addr_3 * sizeof(PtDerivs);
			addr_1 *= iBytesCount;
			addr_2 *= iBytesCount;
			addr_3 *= iBytesCount;

			for(c = 0; c < chan_count; c++)
			{
				coeffs[3] =  (((PtDerivs *) (pSrcMap + addr_1_d))->dx + ((PtDerivs *) (pSrcMap + addr_2_d))->dx - 2 * (pDataSrcBYTE[addr_2] - pDataSrcBYTE[addr_1]));
				coeffs[2] = (((PtDerivs *) (pSrcMap + addr_2_d))->dx - ((PtDerivs *) (pSrcMap + addr_1_d))->dx - 3 * coeffs[3]) / 2.;
				coeffs[1] = ((PtDerivs *) (pSrcMap + addr_2_d))->dx - (2 * coeffs[2] + 3 * coeffs[3]);
				coeffs[0] = (AWPDOUBLE) pDataSrcBYTE[addr_2] - coeffs[1] - coeffs[2] - coeffs[3];

				coeffs[7] =  (((PtDerivs *) (pSrcMap + addr_1_d))->dy + ((PtDerivs *) (pSrcMap + addr_3_d))->dy - 2 * (pDataSrcBYTE[addr_3] - pDataSrcBYTE[addr_1]));
				coeffs[6] = (((PtDerivs *) (pSrcMap + addr_3_d))->dy - ((PtDerivs *) (pSrcMap + addr_1_d))->dy - 3 * coeffs[7]) / 2.;
				coeffs[5] = ((PtDerivs *) (pSrcMap + addr_3_d))->dy - (2 * coeffs[6] + 3 * coeffs[7]);
				coeffs[4] = pDataSrcBYTE[addr_3] - coeffs[5] - coeffs[6] - coeffs[7];

				addr_1 += iBytesCount;
				addr_2 += iBytesCount;
				addr_3 += iBytesCount;
				addr_1_d += sizeof(PtDerivs);
				addr_2_d += sizeof(PtDerivs);
				addr_3_d += sizeof(PtDerivs);

				val = (coeffs[0] +
					+ coeffs[1]*sx + coeffs[2]*sx2
					+ coeffs[3]*sx2*sx + coeffs[4]
					+ coeffs[5]*sy + coeffs[6]*sy2
					+ coeffs[7]*sy2*sy) / 2.;

				if (val < 0) val = 0;
				if (val > 255) val = 255;

				pDataDstBYTE[iIndex] = (AWPBYTE) (val);
				iIndex += iBytesCount;
			}
		}
	}

	free(pSrcMap);
	pSrcMap = NULL;

    free(pDataSrcBYTE);
    pImageSrc->pPixels = pDataDstBYTE;

    pImageSrc->sSizeX = wNewWidth;
    pImageSrc->sSizeY = wNewHeight;

CLEANUP:
	if (pSrcMap != NULL)
		free(pSrcMap);

    return res;
}
AWPRESULT awpResizeBicubic4x4(awpImage* pImageSrc, const AWPWORD wNewWidth, const AWPWORD wNewHeight)
{
    AWPINT iIndex;
    AWPINT iBytesCount;
    AWPINT iImageType;
	AWPWORD cols, rows; //for increase speed
	AWPWORD max_x, max_y; //for increase speed
	AWPBYTE chan_count; //for increase speed
	AWPWORD x, y;
	AWPWORD x_min, y_min;
	AWPBYTE c;
	AWPDWORD base_addr, c_base_addr, addr_11, addr_12, addr_13, addr_14,
		addr_21, addr_22, addr_23, addr_24, addr_31, addr_32, addr_33, addr_34,
		addr_41, addr_42, addr_43, addr_44; //for increase speed
	AWPDOUBLE *coeffs = NULL;
	AWPDOUBLE sx, sy;
	AWPDOUBLE sx2, sx3, sy2, sy3; //for increase speed
	AWPDOUBLE kx, ky; //for increase speed
	AWPDOUBLE val;
	AWPBOOL recalc;

    AWPDOUBLE dRowAddress;
    AWPDOUBLE dColAddress;

    AWPBYTE *pDataSrcBYTE = NULL;
//	SHORT *pDataSrcSHORT = NULL;
//	AWPFLOAT *pDataSrcFLOAT = NULL;
//	AWPDOUBLE *pDataSrcAWPAWPDOUBLE = NULL;
    AWPBYTE *pDataDstBYTE = NULL;

    AWPRESULT res;
    res   = AWP_OK;

    _CHECK_RESULT_((res = awpCheckImage(pImageSrc)))

    if((wNewWidth==pImageSrc->sSizeX)&&(wNewHeight==pImageSrc->sSizeY))
        return AWP_OK;

	if ((pImageSrc->sSizeX < 4) || (pImageSrc->sSizeY < 4))
		_ERROR_EXIT_RES_(AWP_BADARG)

    _CHECK_RESULT_((res = awpGetImagePixelType(pImageSrc, &iImageType)))

    switch(iImageType) {
    case AWP_BYTE :  iBytesCount = sizeof(AWPBYTE); break;
    case AWP_SHORT:  iBytesCount = sizeof(AWPSHORT)/sizeof(AWPBYTE); break;
    case AWP_FLOAT:  iBytesCount = sizeof(AWPFLOAT)/sizeof(AWPBYTE); break;
    case AWP_DOUBLE: iBytesCount = sizeof(AWPDOUBLE)/sizeof(AWPBYTE); break;

    default:
        {
            res = AWP_BADARG;
            _ERR_EXIT_
        }
    }

	cols = pImageSrc->sSizeX;
	rows = pImageSrc->sSizeY;
	chan_count = pImageSrc->bChannels;

    if((pDataDstBYTE = (AWPBYTE*) malloc(wNewWidth*wNewHeight*pImageSrc->bChannels*iBytesCount*sizeof(AWPBYTE))) == NULL)
		_ERROR_EXIT_RES_(AWP_NOTENOUGH_MEM)

	pDataSrcBYTE = (AWPBYTE *) pImageSrc->pPixels;

    if((coeffs = (AWPDOUBLE*) malloc(16*pImageSrc->bChannels*sizeof(AWPDOUBLE))) == NULL)
		_ERROR_EXIT_RES_(AWP_NOTENOUGH_MEM)

	iIndex = 0;

	kx = (cols - 1) / (AWPDOUBLE) (wNewWidth - 1);
	ky = (rows - 1) / (AWPDOUBLE) (wNewHeight - 1);

	max_x = cols - 4;
	max_y = rows - 4;

	y_min = 0;
	for(y = 0; y < wNewHeight; y++)
	{
		dRowAddress = y * ky;

		if (dRowAddress > y_min + 2)
		{
			if (dRowAddress > 1)
			{
				y_min = (AWPWORD) (dRowAddress) - 1;
				if (y_min > max_y) y_min = max_y;
			}
			else
				y_min = 0;
		}

		sy = dRowAddress - y_min;
		sy2	= sy*sy;
		sy3 = sy2*sy;

		x_min = 0; recalc = TRUE;
		for(x = 0; x < wNewWidth; x++)
		{
			dColAddress = x * kx;

			if (dColAddress > x_min + 2)
			{
				recalc = TRUE;
				if (dColAddress > 1)
				{
					x_min = (AWPWORD) (dColAddress) - 1;
					if (x_min > max_x) x_min = max_x;
				}
				else
					x_min = 0;
			}

			sx = dColAddress - x_min;
			sx2 = sx*sx;
			sx3 = sx2*sx;

			if (recalc)
			{
				addr_11 = ((y_min * cols + x_min) * chan_count);
				addr_12 = (addr_11 + chan_count);
				addr_13 = (addr_12 + chan_count);
				addr_14 = (addr_13 + chan_count);
				addr_21 = (addr_11 + cols * chan_count);
				addr_22 = (addr_21 + chan_count);
				addr_23 = (addr_22 + chan_count);
				addr_24 = (addr_23 + chan_count);
				addr_31 = (addr_21 + cols * chan_count);
				addr_32 = (addr_31 + chan_count);
				addr_33 = (addr_32 + chan_count);
				addr_34 = (addr_33 + chan_count);
				addr_41 = (addr_31 + cols * chan_count);
				addr_42 = (addr_41 + chan_count);
				addr_43 = (addr_42 + chan_count);
				addr_44 = (addr_43 + chan_count);

				addr_11 *= iBytesCount; addr_12 *= iBytesCount;
				addr_13 *= iBytesCount; addr_14 *= iBytesCount;
				addr_21 *= iBytesCount; addr_22 *= iBytesCount;
				addr_23 *= iBytesCount; addr_24 *= iBytesCount;
				addr_31 *= iBytesCount; addr_32 *= iBytesCount;
				addr_33 *= iBytesCount; addr_34 *= iBytesCount;
				addr_41 *= iBytesCount; addr_42 *= iBytesCount;
				addr_43 *= iBytesCount; addr_44 *= iBytesCount;

				for(c = 0; c < chan_count; c++)
				{
						c_base_addr = c * 16;

						coeffs[c_base_addr] = pDataSrcBYTE[addr_11];

						coeffs[++c_base_addr] = (
							-66 * pDataSrcBYTE[addr_11] +
							108 * pDataSrcBYTE[addr_12] +
							-54 * pDataSrcBYTE[addr_13] +
							12 * pDataSrcBYTE[addr_14])
							/ 36.;

						coeffs[++c_base_addr] = (
							-66 * pDataSrcBYTE[addr_11] +
							108 * pDataSrcBYTE[addr_21] +
							-54 * pDataSrcBYTE[addr_31] +
							12 * pDataSrcBYTE[addr_41])
							/ 36.;

						coeffs[++c_base_addr] = (
							121 * pDataSrcBYTE[addr_11] +
							-198 * pDataSrcBYTE[addr_12] +
							99 * pDataSrcBYTE[addr_13] +
							-22 * pDataSrcBYTE[addr_14] +
							-198 * pDataSrcBYTE[addr_21] +
							324 * pDataSrcBYTE[addr_22] +
							-162 * pDataSrcBYTE[addr_23] +
							36 * pDataSrcBYTE[addr_24] +
							99 * pDataSrcBYTE[addr_31] +
							-162 * pDataSrcBYTE[addr_32] +
							81 * pDataSrcBYTE[addr_33] +
							-18 * pDataSrcBYTE[addr_34] +
							-22 * pDataSrcBYTE[addr_41] +
							36 * pDataSrcBYTE[addr_42] +
							-18 * pDataSrcBYTE[addr_43] +
							4 * pDataSrcBYTE[addr_44])
							/ 36.;
						
						coeffs[++c_base_addr] = (
							-66 * pDataSrcBYTE[addr_11] +
							165 * pDataSrcBYTE[addr_12] +
							-132 * pDataSrcBYTE[addr_13] +
							33 * pDataSrcBYTE[addr_14] +
							108 * pDataSrcBYTE[addr_21] +
							-270 * pDataSrcBYTE[addr_22] +
							216 * pDataSrcBYTE[addr_23] +
							-54 * pDataSrcBYTE[addr_24] +
							-54 * pDataSrcBYTE[addr_31] +
							135 * pDataSrcBYTE[addr_32] +
							-108 * pDataSrcBYTE[addr_33] +
							27 * pDataSrcBYTE[addr_34] +
							12 * pDataSrcBYTE[addr_41] +
							-30 * pDataSrcBYTE[addr_42] +
							24 * pDataSrcBYTE[addr_43] +
							-6 * pDataSrcBYTE[addr_44])
							/ 36.;

						coeffs[++c_base_addr] = (
							-66 * pDataSrcBYTE[addr_11] +
							108 * pDataSrcBYTE[addr_12] +
							-54 * pDataSrcBYTE[addr_13] +
							12 * pDataSrcBYTE[addr_14] +
							165 * pDataSrcBYTE[addr_21] +
							-270 * pDataSrcBYTE[addr_22] +
							135 * pDataSrcBYTE[addr_23] +
							-30 * pDataSrcBYTE[addr_24] +
							-132 * pDataSrcBYTE[addr_31] +
							216 * pDataSrcBYTE[addr_32] +
							-108 * pDataSrcBYTE[addr_33] +
							24 * pDataSrcBYTE[addr_34] +
							33 * pDataSrcBYTE[addr_41] +
							-54 * pDataSrcBYTE[addr_42] +
							27 * pDataSrcBYTE[addr_43] +
							-6 * pDataSrcBYTE[addr_44])
							/ 36.;

						coeffs[++c_base_addr] = (
							36 * pDataSrcBYTE[addr_11] +
							-90 * pDataSrcBYTE[addr_12] +
							72 * pDataSrcBYTE[addr_13] +
							-18 * pDataSrcBYTE[addr_14] +
							-90 * pDataSrcBYTE[addr_21] +
							225 * pDataSrcBYTE[addr_22] +
							-180 * pDataSrcBYTE[addr_23] +
							45 * pDataSrcBYTE[addr_24] +
							72 * pDataSrcBYTE[addr_31] +
							-180 * pDataSrcBYTE[addr_32] +
							144 * pDataSrcBYTE[addr_33] +
							-36 * pDataSrcBYTE[addr_34] +
							-18 * pDataSrcBYTE[addr_41] +
							45 * pDataSrcBYTE[addr_42] +
							-36 * pDataSrcBYTE[addr_43] +
							9 * pDataSrcBYTE[addr_44])
							/ 36.;

  						coeffs[++c_base_addr] = (
							-6 * pDataSrcBYTE[addr_11] +
							18 * pDataSrcBYTE[addr_12] +
							-18 * pDataSrcBYTE[addr_13] +
							6 * pDataSrcBYTE[addr_14])
							/ 36.;

  						coeffs[++c_base_addr] = (
							-6 * pDataSrcBYTE[addr_11] +
							18 * pDataSrcBYTE[addr_21] +
							-18 * pDataSrcBYTE[addr_31] +
							6 * pDataSrcBYTE[addr_41])
							/ 36.;

						coeffs[++c_base_addr] = (
							36 * pDataSrcBYTE[addr_11] +
							-90 * pDataSrcBYTE[addr_12] +
							72 * pDataSrcBYTE[addr_13] +
							-18 * pDataSrcBYTE[addr_14])
							/ 36.;

						coeffs[++c_base_addr] = (
							36 * pDataSrcBYTE[addr_11] +
							-90 * pDataSrcBYTE[addr_21] +
							72 * pDataSrcBYTE[addr_31] +
							-18 * pDataSrcBYTE[addr_41])
							/ 36.;

						coeffs[++c_base_addr] = (
							11 * pDataSrcBYTE[addr_11] +
							-33 * pDataSrcBYTE[addr_12] +
							33 * pDataSrcBYTE[addr_13] +
							-11 * pDataSrcBYTE[addr_14] +
							-18 * pDataSrcBYTE[addr_21] +
							54 * pDataSrcBYTE[addr_22] +
							-54 * pDataSrcBYTE[addr_23] +
							18 * pDataSrcBYTE[addr_24] +
							9 * pDataSrcBYTE[addr_31] +
							-27 * pDataSrcBYTE[addr_32] +
							27 * pDataSrcBYTE[addr_33] +
							-9 * pDataSrcBYTE[addr_34] +
							-2 * pDataSrcBYTE[addr_41] +
							6 * pDataSrcBYTE[addr_42] +
							-6 * pDataSrcBYTE[addr_43] +
							2 * pDataSrcBYTE[addr_44])
							/ 36.;

						coeffs[++c_base_addr] = (
							-6 * pDataSrcBYTE[addr_11] +
							18 * pDataSrcBYTE[addr_12] +
							-18 * pDataSrcBYTE[addr_13] +
							6 * pDataSrcBYTE[addr_14] +
							15 * pDataSrcBYTE[addr_21] +
							-45 * pDataSrcBYTE[addr_22] +
							45 * pDataSrcBYTE[addr_23] +
							-15 * pDataSrcBYTE[addr_24] +
							-12 * pDataSrcBYTE[addr_31] +
							36 * pDataSrcBYTE[addr_32] +
							-36 * pDataSrcBYTE[addr_33] +
							12 * pDataSrcBYTE[addr_34] +
							3 * pDataSrcBYTE[addr_41] +
							-9 * pDataSrcBYTE[addr_42] +
							9 * pDataSrcBYTE[addr_43] +
							-3 * pDataSrcBYTE[addr_44])
							/ 36.;

						coeffs[++c_base_addr] = (
							-6 * pDataSrcBYTE[addr_11] +
							15 * pDataSrcBYTE[addr_12] +
							-12 * pDataSrcBYTE[addr_13] +
							3 * pDataSrcBYTE[addr_14] +
							18 * pDataSrcBYTE[addr_21] +
							-45 * pDataSrcBYTE[addr_22] +
							36 * pDataSrcBYTE[addr_23] +
							-9 * pDataSrcBYTE[addr_24] +
							-18 * pDataSrcBYTE[addr_31] +
							45 * pDataSrcBYTE[addr_32] +
							-36 * pDataSrcBYTE[addr_33] +
							9 * pDataSrcBYTE[addr_34] +
							6 * pDataSrcBYTE[addr_41] +
							-15 * pDataSrcBYTE[addr_42] +
							12 * pDataSrcBYTE[addr_43] +
							-3 * pDataSrcBYTE[addr_44])
							/ 36.;

						coeffs[++c_base_addr] = (
							11 * pDataSrcBYTE[addr_11] +
							-18 * pDataSrcBYTE[addr_12] +
							9 * pDataSrcBYTE[addr_13] +
							-2 * pDataSrcBYTE[addr_14] +
							-33 * pDataSrcBYTE[addr_21] +
							54 * pDataSrcBYTE[addr_22] +
							-27 * pDataSrcBYTE[addr_23] +
							6 * pDataSrcBYTE[addr_24] +
							33 * pDataSrcBYTE[addr_31] +
							-54 * pDataSrcBYTE[addr_32] +
							27 * pDataSrcBYTE[addr_33] +
							-6 * pDataSrcBYTE[addr_34] +
							-11 * pDataSrcBYTE[addr_41] +
							18 * pDataSrcBYTE[addr_42] +
							-9 * pDataSrcBYTE[addr_43] +
							2 * pDataSrcBYTE[addr_44])
							/ 36.;

						coeffs[++c_base_addr] = (
							pDataSrcBYTE[addr_11] +
							-3 * pDataSrcBYTE[addr_12] +
							3 * pDataSrcBYTE[addr_13] +
							-pDataSrcBYTE[addr_14] +
							-3 * pDataSrcBYTE[addr_21] +
							9 * pDataSrcBYTE[addr_22] +
							-9 * pDataSrcBYTE[addr_23] +
							3 * pDataSrcBYTE[addr_24] +
							3 * pDataSrcBYTE[addr_31] +
							-9 * pDataSrcBYTE[addr_32] +
							9 * pDataSrcBYTE[addr_33] +
							-3 * pDataSrcBYTE[addr_34] +
							-pDataSrcBYTE[addr_41] +
							3 * pDataSrcBYTE[addr_42] +
							-3 * pDataSrcBYTE[addr_43] +
							pDataSrcBYTE[addr_44])
							/ 36.;

					if (c < chan_count)
					{
						addr_11 += iBytesCount; addr_12 += iBytesCount;
						addr_13 += iBytesCount; addr_14 += iBytesCount;
						addr_21 += iBytesCount; addr_22 += iBytesCount;
						addr_23 += iBytesCount; addr_24 += iBytesCount;
						addr_31 += iBytesCount; addr_32 += iBytesCount;
						addr_33 += iBytesCount; addr_34 += iBytesCount;
						addr_41 += iBytesCount; addr_42 += iBytesCount;
						addr_43 += iBytesCount; addr_44 += iBytesCount;
					}
				}
				recalc = FALSE;
			}

			for(c = 0; c < chan_count; c++)
			{
				base_addr = c * 16;

				val = coeffs[base_addr]
					+ coeffs[base_addr + 1]*sx + coeffs[base_addr + 2]*sy
					+ coeffs[base_addr + 3]*sx*sy + coeffs[base_addr + 4]*sx2*sy
					+ coeffs[base_addr + 5]*sx*sy2 + coeffs[base_addr + 6]*sx2*sy2
					+ coeffs[base_addr + 7]*sx3	+ coeffs[base_addr + 8]*sy3
					+ coeffs[base_addr + 9]*sx2 + coeffs[base_addr + 10]*sy2
					+ coeffs[base_addr + 11]*sx3*sy + coeffs[base_addr + 12]*sx3*sy2
					+ coeffs[base_addr + 13]*sx2*sy3 + coeffs[base_addr + 14]*sx*sy3
					+ coeffs[base_addr + 15]*sx3*sy3;

				if (val < 0) val = 0;
				if (val > 255) val = 255;

				pDataDstBYTE[iIndex] = (AWPBYTE) (val);
				iIndex += iBytesCount;
			}
		}
	}

	free(coeffs);
	coeffs = NULL;

    free(pDataSrcBYTE);
    pImageSrc->pPixels = pDataDstBYTE;

    pImageSrc->sSizeX = wNewWidth;
    pImageSrc->sSizeY = wNewHeight;

CLEANUP:
	if (coeffs != NULL)
		free(coeffs);
	
	if ((res != AWP_OK) && (pDataDstBYTE != NULL))
		free(pDataDstBYTE);

    return res;
}

AWPRESULT awpResizeBicubic2x2(awpImage* pImageSrc, const AWPWORD wNewWidth, const AWPWORD wNewHeight)
{
	//3 derivatives in current point
    typedef struct tagPtDerivs
    {
		AWPDOUBLE dx;
		AWPDOUBLE dy;
		AWPDOUBLE dxy;
    }PtDerivs;

    AWPINT iIndex;
    AWPINT iBytesCount;
    AWPINT iImageType;
	AWPBYTE *pSrcMap = NULL;
	AWPWORD cols, rows; //for increase speed
	AWPWORD dec_cols, dec_rows; //for increase speed
	AWPBYTE chan_count; //for increase speed
	AWPINT chan_bytes; //for increase speed
	AWPWORD max_x, max_y; //for increase speed
	AWPWORD x, y;
	AWPWORD x_min, y_min;
	AWPBYTE c;
	AWPDWORD base_addr, addr_11, addr_12, addr_21, addr_22,
		daddr_11, daddr_12, daddr_21, daddr_22; //for increase speed
	AWPDOUBLE *coeffs = NULL;
	AWPDOUBLE sx, sy;
	AWPDOUBLE sx2, sx3, sy2, sy3; //for increase speed
	AWPDOUBLE kx, ky; //for increase speed
	AWPDOUBLE val;
	AWPBOOL recalc;

    AWPDOUBLE dRowAddress;
    AWPDOUBLE dColAddress;

    AWPBYTE *pDataSrcBYTE = NULL;
//	SHORT *pDataSrcSHORT = NULL;
//	AWPFLOAT *pDataSrcFLOAT = NULL;
//	AWPDOUBLE *pDataSrcAWPAWPDOUBLE = NULL;
    AWPBYTE *pDataDstBYTE = NULL;

    AWPRESULT res;
    res   = AWP_OK;

    _CHECK_RESULT_((res = awpCheckImage(pImageSrc)))

    if((wNewWidth==pImageSrc->sSizeX)&&(wNewHeight==pImageSrc->sSizeY))
        return AWP_OK;

	if ((pImageSrc->sSizeX < 3) || (pImageSrc->sSizeY < 3))
		_ERROR_EXIT_RES_(AWP_BADARG)

    _CHECK_RESULT_((res = awpGetImagePixelType(pImageSrc, &iImageType)))

    switch(iImageType) {
    case AWP_BYTE :  iBytesCount = sizeof(AWPBYTE); break;
    case AWP_SHORT:  iBytesCount = sizeof(AWPSHORT)/sizeof(AWPBYTE);  break;
    case AWP_FLOAT:  iBytesCount = sizeof(AWPFLOAT)/sizeof(AWPBYTE);  break;
    case AWP_DOUBLE: iBytesCount = sizeof(AWPDOUBLE)/sizeof(AWPBYTE); break;

    default:
        {
            res = AWP_BADARG;
            _ERR_EXIT_
        }
    }

	if ((pSrcMap = (AWPBYTE *) malloc(pImageSrc->sSizeX * pImageSrc->sSizeY * pImageSrc->bChannels * sizeof(PtDerivs))) == NULL)
		_ERROR_EXIT_RES_(AWP_NOTENOUGH_MEM)

	cols = pImageSrc->sSizeX;
	rows = pImageSrc->sSizeY;
	chan_count = pImageSrc->bChannels;
	chan_bytes = chan_count * iBytesCount;

	//count derivatives
	switch (iImageType)
	{
	case AWP_BYTE:
		pDataSrcBYTE = (AWPBYTE *) pImageSrc->pPixels;
		dec_cols = cols - 1;
		dec_rows = rows - 1;
		for(x = 0; x < cols; x++)
			for(y = 0; y < rows; y++)
			{
				addr_11 = (y * cols + x) * chan_count;
				daddr_11 = (AWPDWORD) pSrcMap + addr_11 * sizeof(PtDerivs);
				addr_11 *= iBytesCount;
				for(c = 0; c < chan_count; c++)
				{
					if (x == 0)
					{
						((PtDerivs *) daddr_11)->dx = (-3 * pDataSrcBYTE[addr_11] + 4 * pDataSrcBYTE[addr_11 + chan_bytes] - pDataSrcBYTE[addr_11 + 2 * chan_bytes]) / 2.;
						if (y == 0)
						{
							((PtDerivs *) daddr_11)->dy = (-3 * pDataSrcBYTE[addr_11] + 4 * pDataSrcBYTE[addr_11 + cols * chan_bytes] - pDataSrcBYTE[addr_11 + 2 * cols * chan_bytes]) / 2.;
							((PtDerivs *) daddr_11)->dxy = (-3 * (-3 * pDataSrcBYTE[addr_11] + 4 * pDataSrcBYTE[addr_11 + chan_bytes] - pDataSrcBYTE[addr_11 + 2 * chan_bytes])
								+ 4 * (-3 * pDataSrcBYTE[addr_11 + cols * chan_bytes] + 4 * pDataSrcBYTE[addr_11 + (cols + 1) * chan_bytes] - pDataSrcBYTE[addr_11 + (cols + 2) * chan_bytes])
								- (-3 * pDataSrcBYTE[addr_11 + 2 * cols * chan_bytes] + 4 * pDataSrcBYTE[addr_11 + (2 * cols + 1) * chan_bytes] - pDataSrcBYTE[addr_11 + (2 * cols + 2) * chan_bytes])) / 4.;
						}
						else if (y == dec_rows)
						{
							((PtDerivs *) daddr_11)->dy = (pDataSrcBYTE[addr_11 - 2 * cols * chan_bytes] - 4 * pDataSrcBYTE[addr_11 - cols * chan_bytes] + 3 * pDataSrcBYTE[addr_11]) / 2.;
							((PtDerivs *) daddr_11)->dxy = ((-3 * pDataSrcBYTE[addr_11 - 2 * cols * chan_bytes] + 4 * pDataSrcBYTE[addr_11 + (-2 * cols + 1) * chan_bytes] - pDataSrcBYTE[addr_11 + (-2 * cols + 2) * chan_bytes])
								- 4 * (-3 * pDataSrcBYTE[addr_11 - cols * chan_bytes] + 4 * pDataSrcBYTE[addr_11 + (-cols + 1) * chan_bytes] - pDataSrcBYTE[addr_11 + (-cols + 2) * chan_bytes])
								+ 3 * (-3 * pDataSrcBYTE[addr_11] + 4 * pDataSrcBYTE[addr_11 + chan_bytes] - pDataSrcBYTE[addr_11 + 2 * chan_bytes])) / 4.;
						}
						else
						{
							((PtDerivs *) daddr_11)->dy = (pDataSrcBYTE[addr_11 + cols * chan_bytes] - pDataSrcBYTE[addr_11 - cols * chan_bytes]) / 2.;
							((PtDerivs *) daddr_11)->dxy = ((-3 * pDataSrcBYTE[addr_11 + cols * chan_bytes] + 4 * pDataSrcBYTE[addr_11 + (cols + 1) * chan_bytes] - pDataSrcBYTE[addr_11 + (cols + 2) * chan_bytes])
								- (-3 * pDataSrcBYTE[addr_11 - cols * chan_bytes] + 4 * pDataSrcBYTE[addr_11 + (-cols + 1) * chan_bytes] - pDataSrcBYTE[addr_11 + (-cols + 2) * chan_bytes])) / 4.;
						}
					}
					else if (x == dec_cols)
					{
						((PtDerivs *) daddr_11)->dx = (pDataSrcBYTE[addr_11 - 2 * chan_bytes] - 4 * pDataSrcBYTE[addr_11 - chan_bytes] + 3 * pDataSrcBYTE[addr_11]) / 2.;
						if (y == 0)
						{
							((PtDerivs *) daddr_11)->dy = (-3 * pDataSrcBYTE[addr_11] + 4 * pDataSrcBYTE[addr_11 + cols * chan_bytes] - pDataSrcBYTE[addr_11 + 2 * cols * chan_bytes]) / 2.;
							((PtDerivs *) daddr_11)->dxy = (-3 * (pDataSrcBYTE[addr_11 - 2 * chan_bytes] - 4 * pDataSrcBYTE[addr_11 - chan_bytes] + 3 * pDataSrcBYTE[addr_11])
								+ 4 * (pDataSrcBYTE[addr_11 + (cols - 2) * chan_bytes] - 4 * pDataSrcBYTE[addr_11 + (cols - 1) * chan_bytes] + 3 * pDataSrcBYTE[addr_11 + cols * chan_bytes])
								- (pDataSrcBYTE[addr_11 + (2 * cols - 2) * chan_bytes] - 4 * pDataSrcBYTE[addr_11 + (2 * cols - 1) * chan_bytes] + 3 * pDataSrcBYTE[addr_11 + 2 * cols * chan_bytes])) / 4.;
						}
						else if (y == dec_rows)
						{
							((PtDerivs *) daddr_11)->dy = (pDataSrcBYTE[addr_11 - 2 * cols * chan_bytes] - 4 * pDataSrcBYTE[addr_11 - cols * chan_bytes] + 3 * pDataSrcBYTE[addr_11]) / 2.;
							((PtDerivs *) daddr_11)->dxy = ((pDataSrcBYTE[addr_11 + (- 2 * cols - 2) * chan_bytes] - 4 * pDataSrcBYTE[addr_11 + (- 2 * cols - 1) * chan_bytes] + 3 * pDataSrcBYTE[addr_11 + (- 2 * cols) * chan_bytes])
								- 4 * (pDataSrcBYTE[addr_11 + (-cols - 2) * chan_bytes] - 4 * pDataSrcBYTE[addr_11 + (-cols - 1) * chan_bytes] + 3 * pDataSrcBYTE[addr_11 + (-cols) * chan_bytes])
								+ 3 * (pDataSrcBYTE[addr_11 - 2 * chan_bytes] - 4 * pDataSrcBYTE[addr_11 - chan_bytes] + 3 * pDataSrcBYTE[addr_11])) / 4.;
						}
						else
						{
							((PtDerivs *) daddr_11)->dy = (pDataSrcBYTE[addr_11 + cols * chan_bytes] - pDataSrcBYTE[addr_11 - cols * chan_bytes]) / 2.;
							((PtDerivs *) daddr_11)->dxy = ((pDataSrcBYTE[addr_11 + (cols - 2) * chan_bytes] - 4 * pDataSrcBYTE[addr_11 + (cols - 1) * chan_bytes] + 3 * pDataSrcBYTE[addr_11 + cols * chan_bytes])
								- (pDataSrcBYTE[addr_11 - (cols + 2) * chan_bytes] - 4 * pDataSrcBYTE[addr_11 - (cols + 1) * chan_bytes] + 3 * pDataSrcBYTE[addr_11 - cols * chan_bytes])) / 4.;
						}
					}
					else
					{
						((PtDerivs *) daddr_11)->dx = (pDataSrcBYTE[addr_11 + chan_bytes] - pDataSrcBYTE[addr_11 - chan_bytes]) / 2.;
						if (y == 0)
						{
							((PtDerivs *) daddr_11)->dy = (-3 * pDataSrcBYTE[addr_11] + 4 * pDataSrcBYTE[addr_11 + cols * chan_bytes] - pDataSrcBYTE[addr_11 + 2 * cols * chan_bytes]) / 2.;
							((PtDerivs *) daddr_11)->dxy = (-3 * (pDataSrcBYTE[addr_11 + chan_bytes] - pDataSrcBYTE[addr_11 - chan_bytes])
								+ 4 * (pDataSrcBYTE[addr_11 + (cols + 1) * chan_bytes] - pDataSrcBYTE[addr_11 + (cols - 1) * chan_bytes])
								- (pDataSrcBYTE[addr_11 + (2 * cols + 1) * chan_bytes] - pDataSrcBYTE[addr_11 + (2 * cols - 1) * chan_bytes])) / 4.;
						}
						else if (y == dec_rows)
						{
							((PtDerivs *) daddr_11)->dy = (pDataSrcBYTE[addr_11 - 2 * cols * chan_bytes] - 4 * pDataSrcBYTE[addr_11 - cols * chan_bytes] + 3 * pDataSrcBYTE[addr_11]) / 2.;
							((PtDerivs *) daddr_11)->dxy = ((pDataSrcBYTE[addr_11 + (-2 * cols + 1) * chan_bytes] - pDataSrcBYTE[addr_11 + (-2 * cols - 1) * chan_bytes])
								- 4 * (pDataSrcBYTE[addr_11 + (-cols + 1) * chan_bytes] - pDataSrcBYTE[addr_11 + (-cols - 1) * chan_bytes])
								+ 3 * (pDataSrcBYTE[addr_11 + chan_bytes] - pDataSrcBYTE[addr_11 - chan_bytes])) / 4.;
						}
						else
						{
							((PtDerivs *) daddr_11)->dy = ((AWPSHORT)pDataSrcBYTE[addr_11 + cols * chan_bytes] - (AWPSHORT)pDataSrcBYTE[addr_11 - cols * chan_bytes]) / 2.;
							((PtDerivs *) daddr_11)->dxy = ((pDataSrcBYTE[addr_11 + (cols + 1) * chan_bytes] - pDataSrcBYTE[addr_11 + (cols - 1) * chan_bytes])
								- (pDataSrcBYTE[addr_11 + (-cols + 1) * chan_bytes] - pDataSrcBYTE[addr_11 - (cols + 1) * chan_bytes])) / 4.;
						}
					}
					addr_11 += iBytesCount;
					daddr_11 += sizeof(PtDerivs);
				}
			}
	}

    if((pDataDstBYTE = (AWPBYTE*)malloc(wNewWidth*wNewHeight*pImageSrc->bChannels*iBytesCount*sizeof(AWPBYTE))) == NULL)
		_ERROR_EXIT_RES_(AWP_NOTENOUGH_MEM)

	if((coeffs = (AWPDOUBLE*) malloc(16*pImageSrc->bChannels*sizeof(AWPDOUBLE))) == NULL)
		_ERROR_EXIT_RES_(AWP_NOTENOUGH_MEM)

	iIndex = 0; 

	kx = (cols - 1) / (AWPDOUBLE) (wNewWidth - 1);
	ky = (rows - 1) / (AWPDOUBLE) (wNewHeight - 1);

	max_x = cols - 2;
	max_y = rows - 2;

	y_min = 0;
	for(y = 0; y < wNewHeight; y++)
	{
		dRowAddress = y * ky;

		if (dRowAddress > y_min + 1)
		{
			y_min = (AWPWORD) (dRowAddress);
			if (y_min > max_y) y_min = max_y;
		}

		sy = dRowAddress - y_min;
		sy2	= sy*sy;
		sy3 = sy2*sy;

		x_min = 0; recalc = TRUE;
		for(x = 0; x < wNewWidth; x++)
		{
			dColAddress = x * kx;

			if (dColAddress > x_min + 1)
			{
				recalc = TRUE;
				x_min = (AWPWORD) (dColAddress);
				if (x_min > max_x) x_min = max_x;
			}

			sx = dColAddress - x_min;
			sx2 = sx*sx;
			sx3 = sx2*sx;

			if (recalc)
			{
				addr_11 = ((y_min * cols + x_min) * chan_count);
				addr_12 = (addr_11 + chan_count);
				addr_21 = (addr_11 + cols * chan_count);
				addr_22 = (addr_21 + chan_count);

				daddr_11 = (AWPDWORD) pSrcMap + addr_11 * sizeof(PtDerivs);
				daddr_12 = (AWPDWORD) pSrcMap + addr_12 * sizeof(PtDerivs);
				daddr_21 = (AWPDWORD) pSrcMap + addr_21 * sizeof(PtDerivs);
				daddr_22 = (AWPDWORD) pSrcMap + addr_22 * sizeof(PtDerivs);

				addr_11 *= iBytesCount;
				addr_12 *= iBytesCount;
				addr_21 *= iBytesCount;
				addr_22 *= iBytesCount;

				for(c = 0; c < chan_count; c++)
				{

					base_addr = c * 16;

					coeffs[base_addr] = pDataSrcBYTE[addr_11];

					coeffs[++base_addr] = ((PtDerivs *) daddr_11)->dx;

					coeffs[++base_addr] = ((PtDerivs *) daddr_11)->dy;

					coeffs[++base_addr] = ((PtDerivs *) daddr_11)->dxy;

					coeffs[++base_addr] =
						-3 * ((PtDerivs *) daddr_11)->dy +
						3 * ((PtDerivs *) daddr_12)->dy +
						-2 * ((PtDerivs *) daddr_11)->dxy +
						- ((PtDerivs *) daddr_12)->dxy;

					coeffs[++base_addr] = 
						-3 * ((PtDerivs *) daddr_11)->dx +
						3 * ((PtDerivs *) daddr_21)->dx +
						-2 * ((PtDerivs *) daddr_11)->dxy +
						- ((PtDerivs *) daddr_21)->dxy;

					coeffs[++base_addr] = (
						18 * pDataSrcBYTE[addr_11] +
						-18 * pDataSrcBYTE[addr_12] +
						18 * pDataSrcBYTE[addr_22] +
						-18 * pDataSrcBYTE[addr_21] +
						12 * ((PtDerivs *) daddr_11)->dx +
						6 * ((PtDerivs *) daddr_12)->dx +
						-6 * ((PtDerivs *) daddr_22)->dx +
						-12 * ((PtDerivs *) daddr_21)->dx +
						21 * ((PtDerivs *) daddr_11)->dy +
						-21 * ((PtDerivs *) daddr_12)->dy +
						3 * ((PtDerivs *) daddr_22)->dy +
						-3 * ((PtDerivs *) daddr_21)->dy +
						14 * ((PtDerivs *) daddr_11)->dxy +
						7 * ((PtDerivs *) daddr_12)->dxy +
						- ((PtDerivs *) daddr_22)->dxy +
						-2 * ((PtDerivs *) daddr_21)->dxy)
						/ 8.;

					coeffs[++base_addr] = 
						2 * pDataSrcBYTE[addr_11] +
						-2 * pDataSrcBYTE[addr_12] +
						((PtDerivs *) daddr_11)->dx +
						((PtDerivs *) daddr_12)->dx;

					coeffs[++base_addr] = (
						-2 * pDataSrcBYTE[addr_11] +
						18 * pDataSrcBYTE[addr_12] +
						-18 * pDataSrcBYTE[addr_22] +
						2 * pDataSrcBYTE[addr_21] +
						-12 * ((PtDerivs *) daddr_11)->dx +
						-6 * ((PtDerivs *) daddr_12)->dx +
						6 * ((PtDerivs *) daddr_22)->dx +
						12 * ((PtDerivs *) daddr_21)->dx +
						- ((PtDerivs *) daddr_11)->dy +
						9 * ((PtDerivs *) daddr_12)->dy +
						9 * ((PtDerivs *) daddr_22)->dy +
						- ((PtDerivs *) daddr_21)->dy +
						-6 * ((PtDerivs *) daddr_11)->dxy +
						-3 * ((PtDerivs *) daddr_12)->dxy +
						-3 * ((PtDerivs *) daddr_22)->dxy +
						-6 * ((PtDerivs *) daddr_21)->dxy)
						/ 8.;

					coeffs[++base_addr] = 
						-3 * pDataSrcBYTE[addr_11] +
						3 * pDataSrcBYTE[addr_12] +
						-2 * ((PtDerivs *) daddr_11)->dx +
						- ((PtDerivs *) daddr_12)->dx;

					coeffs[++base_addr] = (
						-6 * pDataSrcBYTE[addr_11] +
						-18 * pDataSrcBYTE[addr_12] +
						18 * pDataSrcBYTE[addr_22] +
						6 * pDataSrcBYTE[addr_21] +
						12 * ((PtDerivs *) daddr_11)->dx +
						6 * ((PtDerivs *) daddr_12)->dx +
						-6 * ((PtDerivs *) daddr_22)->dx +
						-12 * ((PtDerivs *) daddr_21)->dx +
						-7 * ((PtDerivs *) daddr_11)->dy +
						-9 * ((PtDerivs *) daddr_12)->dy +
						-9 * ((PtDerivs *) daddr_22)->dy +
						((PtDerivs *) daddr_21)->dy +
						6 * ((PtDerivs *) daddr_11)->dxy +
						3 * ((PtDerivs *) daddr_12)->dxy +
						3 * ((PtDerivs *) daddr_22)->dxy +
						6 * ((PtDerivs *) daddr_21)->dxy)
						/ 8.;

					coeffs[++base_addr] =
						2 * ((PtDerivs *) daddr_11)->dy +
						-2 * ((PtDerivs *) daddr_12)->dy +
						((PtDerivs *) daddr_11)->dxy +
						((PtDerivs *) daddr_12)->dxy;

					coeffs[++base_addr] = (
						-12 * pDataSrcBYTE[addr_11] +
						12 * pDataSrcBYTE[addr_12] +
						-12 * pDataSrcBYTE[addr_22] +
						12 * pDataSrcBYTE[addr_21] +
						-12 * ((PtDerivs *) daddr_12)->dx +
						12 * ((PtDerivs *) daddr_22)->dx +
						-14 * ((PtDerivs *) daddr_11)->dy +
						14 * ((PtDerivs *) daddr_12)->dy +
						-2 * ((PtDerivs *) daddr_22)->dy +
						2 * ((PtDerivs *) daddr_21)->dy +
						-4 * ((PtDerivs *) daddr_11)->dxy +
						-10 * ((PtDerivs *) daddr_12)->dxy +
						-2 * ((PtDerivs *) daddr_22)->dxy +
						4 * ((PtDerivs *) daddr_21)->dxy)
						/ 8.;

					coeffs[++base_addr] = (
						6 * pDataSrcBYTE[addr_11] +
						-6 * pDataSrcBYTE[addr_12] +
						6 * pDataSrcBYTE[addr_22] +
						-6 * pDataSrcBYTE[addr_21] +
						4 * ((PtDerivs *) daddr_11)->dx +
						2 * ((PtDerivs *) daddr_12)->dx +
						-2 * ((PtDerivs *) daddr_22)->dx +
						-4 * ((PtDerivs *) daddr_21)->dx +
						3 * ((PtDerivs *) daddr_11)->dy +
						-3 * ((PtDerivs *) daddr_12)->dy +
						-3 * ((PtDerivs *) daddr_22)->dy +
						3 * ((PtDerivs *) daddr_21)->dy +
						2 * ((PtDerivs *) daddr_11)->dxy +
						1 * ((PtDerivs *) daddr_12)->dxy +
						1 * ((PtDerivs *) daddr_22)->dxy +
						2 * ((PtDerivs *) daddr_21)->dxy)
						/ 8.;

					coeffs[++base_addr] = 
						2 * ((PtDerivs *) daddr_11)->dx +
						-2 * ((PtDerivs *) daddr_21)->dx +
						((PtDerivs *) daddr_11)->dxy +
						((PtDerivs *) daddr_21)->dxy;

					coeffs[++base_addr] = (
						-4 * pDataSrcBYTE[addr_11] +
						4 * pDataSrcBYTE[addr_12] +
						-4 * pDataSrcBYTE[addr_22] +
						4 * pDataSrcBYTE[addr_21] +
						-8 * ((PtDerivs *) daddr_11)->dx +
						4 * ((PtDerivs *) daddr_12)->dx +
						-4 * ((PtDerivs *) daddr_22)->dx +
						8 * ((PtDerivs *) daddr_21)->dx +
						-2 * ((PtDerivs *) daddr_11)->dy +
						2 * ((PtDerivs *) daddr_12)->dy +
						2 * ((PtDerivs *) daddr_22)->dy +
						-2 * ((PtDerivs *) daddr_21)->dy +
						-4 * ((PtDerivs *) daddr_11)->dxy +
						2 * ((PtDerivs *) daddr_12)->dxy +
						2 * ((PtDerivs *) daddr_22)->dxy +
						-4 * ((PtDerivs *) daddr_21)->dxy)
						/ 8.;

					if (c < chan_count)
					{
						addr_11 += iBytesCount; addr_12 += iBytesCount;
						addr_22 += iBytesCount; addr_21 += iBytesCount;
						daddr_11 += sizeof(PtDerivs); daddr_12 += sizeof(PtDerivs);
						daddr_22 += sizeof(PtDerivs); daddr_21 += sizeof(PtDerivs);
					}

				}
				recalc = FALSE;
			}

			for(c = 0; c < chan_count; c++)
			{
				base_addr = c * 16;
				val = coeffs[base_addr]
					+ coeffs[base_addr + 1]*sx + coeffs[base_addr + 2]*sy
					+ coeffs[base_addr + 3]*sx*sy + coeffs[base_addr + 4]*sx2*sy
					+ coeffs[base_addr + 5]*sx*sy2 + coeffs[base_addr + 6]*sx2*sy2
					+ coeffs[base_addr + 7]*sx3	+ coeffs[base_addr + 8]*sy3
					+ coeffs[base_addr + 9]*sx2 + coeffs[base_addr + 10]*sy2
					+ coeffs[base_addr + 11]*sx3*sy + coeffs[base_addr + 12]*sx3*sy2
					+ coeffs[base_addr + 13]*sx2*sy3 + coeffs[base_addr + 14]*sx*sy3
					+ coeffs[base_addr + 15]*sx3*sy3;

				if (val < 0) val = 0;
				if (val > 255) val = 255;

				pDataDstBYTE[iIndex] = (AWPBYTE) (val);
				iIndex += iBytesCount;
			}
		}
	}

	free(pSrcMap);
	pSrcMap = NULL;

	free(coeffs);
	coeffs = NULL;

    free(pDataSrcBYTE);
    pImageSrc->pPixels = pDataDstBYTE;

    pImageSrc->sSizeX = wNewWidth;
    pImageSrc->sSizeY = wNewHeight;

CLEANUP:
	if (pSrcMap != NULL)
		free(pSrcMap);

	if (coeffs != NULL)
		free(coeffs);

	if ((res != AWP_OK) && (pDataDstBYTE != NULL))
		free(pDataDstBYTE);

    return res;
}

AWPRESULT awpRotateBilinear(awpImage *pImageSrc, const AWPSHORT sAngle)
{
    AWPWORD wNewRowsCount;
    AWPWORD wNewColsCount;
    AWPWORD wNewWidth;
    AWPWORD wNewHeight;

    int iIndex;
    int iBytesCount;
    int iImageType;
	AWPWORD cols, rows; //for increase speed
	AWPBYTE chan_count; //for increase speed
	AWPWORD x_min, x_max, y_min, y_max;
	AWPBYTE c;
	AWPDOUBLE coeffs[4];
	AWPDOUBLE val;
	AWPDOUBLE sx, sy;
	AWPDWORD addr_1, addr_2, addr_3, addr_4;

    AWPDOUBLE dRowAddress;
    AWPDOUBLE dColAddress;
    AWPDOUBLE b,e,ui,vj;
    AWPDOUBLE dDir[7];
    AWPDOUBLE dInv[7];

    AWPDOUBLE dCosAngle;
    AWPDOUBLE dSinAngle;

    AWPBYTE *pDataSrcBYTE = NULL;
    AWPBYTE *pDataDstBYTE = NULL;

    AWPRESULT res;
    res   = AWP_OK;

    _CHECK_RESULT_((res = awpCheckImage(pImageSrc)))

	if ((pImageSrc->sSizeX < 4) || (pImageSrc->sSizeY < 4))
		_ERROR_EXIT_RES_(AWP_BADARG)

    dCosAngle = cos(sAngle*AWP_PI/180);
    dSinAngle = sin(sAngle*AWP_PI/180);

    wNewHeight = (AWPWORD)(fabs(pImageSrc->sSizeX*dSinAngle) + fabs(pImageSrc->sSizeY*dCosAngle));
    wNewWidth  = (AWPWORD)(fabs(pImageSrc->sSizeX*dCosAngle) + fabs(pImageSrc->sSizeY*dSinAngle));

    memset((void*)dDir, 0, 7*sizeof(AWPDOUBLE));
    dDir[0] = 0.0;
    dDir[1] = dCosAngle;
    dDir[2] = dSinAngle;
    dDir[3] = (sAngle < 0)?-pImageSrc->sSizeX*dSinAngle:0;
    dDir[4] = -dSinAngle;
    dDir[5] = dCosAngle;
    dDir[6] = (sAngle > 0)?pImageSrc->sSizeY*dSinAngle:0;

    _awpGetInvAffin(dDir, dInv);

    b  =  dInv[2];
    e  =  dInv[5];
    ui = dInv[3];
    vj = dInv[6];


    _CHECK_RESULT_((res = awpGetImagePixelType(pImageSrc, &iImageType)))

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

	pDataSrcBYTE = (AWPBYTE *) pImageSrc->pPixels;

	if((pDataDstBYTE = (AWPBYTE*)malloc(wNewWidth*wNewHeight*pImageSrc->bChannels*iBytesCount*sizeof(AWPBYTE))) == NULL)
		_ERROR_EXIT_RES_(AWP_NOTENOUGH_MEM)

	cols = pImageSrc->sSizeX;
	rows = pImageSrc->sSizeY;
	chan_count = pImageSrc->bChannels;

    iIndex = 0;
    for(wNewRowsCount = 0; wNewRowsCount<wNewHeight; ++wNewRowsCount)
    {
        dRowAddress = ui += dInv[1];
        dColAddress = vj += dInv[4];

        for( wNewColsCount = 0; wNewColsCount < wNewWidth; ++wNewColsCount)
        {
            dRowAddress += b;
            dColAddress += e;

			if (dColAddress < 0 || dRowAddress < 0 || dColAddress > cols - 1 || dRowAddress > rows - 1)
			{
                memset(&pDataDstBYTE[iIndex], 0, pImageSrc->bChannels*iBytesCount*sizeof(AWPBYTE));
				iIndex += chan_count*iBytesCount*sizeof(AWPBYTE);
			}
			else
			{
				
				x_min = (int) (dColAddress);
				x_max = x_min + 1;

				if (x_max == cols)
				{
					x_max--;
					x_min--;
				}

				y_min = (int) (dRowAddress);
				y_max = y_min + 1;

				if (y_max == rows)
				{
					 y_max--;
					 y_min--;
				}


				sx = dColAddress - x_min;
				sy = dRowAddress - y_min;

				for(c = 0; c < chan_count; c++)
				{
					addr_1 = ((y_min * cols + x_min) * chan_count + c);
					addr_2 = ((y_min * cols + x_max) * chan_count + c);
					addr_3 = ((y_max * cols + x_min) * chan_count + c);
					addr_4 = ((y_max * cols + x_max) * chan_count + c);

					coeffs[3] = pDataSrcBYTE[addr_4 * iBytesCount] + pDataSrcBYTE[addr_1 * iBytesCount] - pDataSrcBYTE[addr_2 * iBytesCount] - pDataSrcBYTE[addr_3 * iBytesCount];
					coeffs[2] = pDataSrcBYTE[addr_3 * iBytesCount] - pDataSrcBYTE[addr_1 * iBytesCount];
					coeffs[1] = pDataSrcBYTE[addr_2 * iBytesCount] - pDataSrcBYTE[addr_1 * iBytesCount];
					coeffs[0] = pDataSrcBYTE[addr_1 * iBytesCount];

					val = coeffs[0] +
						+ coeffs[1]*sx + coeffs[2]*sy
						+ coeffs[3]*sx*sy;

					if (val < 0) val = 0;
					if (val > 255) val = 255;

					pDataDstBYTE[iIndex] = (AWPBYTE) (val);
					iIndex += iBytesCount;
				}
			}
        }
    }

    free(pDataSrcBYTE);
    pImageSrc->pPixels = pDataDstBYTE;

    pImageSrc->sSizeX = wNewWidth;
    pImageSrc->sSizeY = wNewHeight;

CLEANUP:
    return res;
}
