/*M
//
//      awpipl2 image processing and image analysis library
//		File: awpcontrast.c
//		Purpose: contrast functions 
//		Autors: Alex A.Telnykh (tlnykha@yahoo.com)
//      CopyRight 2004-2018 (c) NN-Videolab.net
//M*/
#include "_awpipl.h"
/*
realizes a simple algorithm of autocontrast
source image should contain AWP_BYTE  channels 
*/
AWPRESULT awpAutoLevels(awpImage* pImage)
{
    AWPRESULT res = AWP_OK;
    AWPINT total = pImage->sSizeX*pImage->sSizeY;
	AWPDOUBLE* min_values = NULL;
	AWPDOUBLE* max_values = NULL;
	AWPBYTE*   pix = NULL;
	AWPWORD x = 0;
	AWPWORD y = 0;
	AWPBYTE c = 0;
	AWPBYTE* v = 0;
	if (res = awpCheckImage(pImage) != AWP_OK)
		_ERR_EXIT_

	if (pImage->dwType != AWP_BYTE)
		_ERROR_EXIT_RES_(AWP_NOTSUPPORT)

	if (res = awpMinMax(pImage, &min_values, &max_values) != AWP_OK)
		_ERR_EXIT_

	pix = _AWP_BPIX_(pImage, AWPBYTE);
	for (y = 0; y < pImage->sSizeY; y++)
	{
		for (x = 0; x < pImage->sSizeX; x++)
		{
			for (c = 0; c < pImage->bChannels; c++)
			{
				v = &pix[y*pImage->sSizeX*pImage->bChannels + x*pImage->bChannels + c];
				if (max_values[c] - min_values[c] == 0)
					*v = (AWPBYTE)max_values[c];
				else
					*v = (AWPBYTE)(255 * (*v - min_values[c]) / (max_values[c] - min_values[c]));
			}
		}
	}
CLEANUP:
	_SAFE_FREE_(min_values);
	_SAFE_FREE_(max_values);
    return res;
}

AWPRESULT awpBilinearBlur( awpImage* pImage, AWPDOUBLE dfStrength )
{
    AWPRESULT res = AWP_OK;

    AWPDOUBLE	dfStrengthA = 1.0 / ( dfStrength + 1.0 );
    AWPDOUBLE  dfStrengthA1 = dfStrength * dfStrengthA;
    AWPINT	iX , iY;
    //AWPINT	iOffset00 , iOffset01 , iOffset10 , iOffset11;

    AWPDOUBLE* pdfPixels = (AWPDOUBLE*)pImage->pPixels;

    AWPDOUBLE* pdfPix00 = NULL;
    AWPDOUBLE* pdfPix01 = NULL;
    AWPDOUBLE* pdfPix10 = NULL;
    AWPDOUBLE* pdfPix11 = NULL;

    
    _CHECK_RESULT_( res = awpCheckImage(pImage) );
    _CHECK_RESULT_( res = (pImage->dwType == AWP_DOUBLE) ? AWP_OK : AWP_BADARG );
      
    pdfPix10 = pdfPixels;
    pdfPix11 = pdfPixels + 1;
    for ( iX = 1 ; iX<pImage->sSizeX ; ++iX, ++pdfPix11, ++pdfPix10 )//, --pdfPix00, --pdfPix01)
    {
        pdfPix11[0] =  ( dfStrengthA * pdfPix11[0] + dfStrengthA1 * pdfPix10[0] );
    }
    
    --pdfPix11; --pdfPix10;
    for ( iX = 1 ; iX<pImage->sSizeX ; ++iX, --pdfPix11, --pdfPix10 )//, --pdfPix00, --pdfPix01)
    {
        pdfPix10[0] = dfStrengthA * pdfPix10[0] + dfStrengthA1 * pdfPix11[0];
    }

    pdfPix01 = pdfPixels;
    pdfPix11 =  pdfPixels + pImage->sSizeX;
    for ( iY = 1 ; iY<pImage->sSizeY ; ++iY,  pdfPix01 += pImage->sSizeX, pdfPix11 += pImage->sSizeX )
    {
        pdfPix11[0] = ( dfStrengthA * pdfPix11[0] + dfStrengthA1 * pdfPix01[0] );
    }

    pdfPix01 -= pImage->sSizeX; pdfPix11 -= pImage->sSizeX;

    for ( iY = 1 ; iY<pImage->sSizeY ; ++iY,  pdfPix01 -= pImage->sSizeX, pdfPix11 -= pImage->sSizeX )
    {
        pdfPix01[0] = ( dfStrengthA * pdfPix01[0] + dfStrengthA1 * pdfPix11[0] );
    }



    pdfPix00	= pdfPixels;
    pdfPix01	= pdfPixels + 1;
    pdfPix10	= pdfPixels + pImage->sSizeX;
    pdfPix11	= pdfPixels + pImage->sSizeX + 1;
   
    for ( iY = 1 ; iY<pImage->sSizeY ; ++iY, ++pdfPix00, ++pdfPix01, ++pdfPix10, ++pdfPix11 )
        for ( iX = 1 ; iX<pImage->sSizeX ; ++iX, ++pdfPix00, ++pdfPix01, ++pdfPix10, ++pdfPix11 )
        {
            pdfPix11[0] = dfStrengthA * ( dfStrengthA * pdfPix11[0] + dfStrength * ( pdfPix01[0] + pdfPix10[0] - dfStrengthA1 * pdfPix00[0] ) );
        }


 

    pdfPix00	= pdfPixels + pImage->sSizeX * pImage->sSizeY - 1;
    pdfPix01	= pdfPix00 - 1;
    pdfPix10	= pdfPix00 - pImage->sSizeX;
    pdfPix11	= pdfPix10 - 1;
   
    for ( iY = pImage->sSizeY-1; iY > 0; --iY, --pdfPix00, --pdfPix01, --pdfPix10, --pdfPix11 )
        for ( iX = pImage->sSizeX-1 ; iX > 0; --iX, --pdfPix00, --pdfPix01, --pdfPix10, --pdfPix11 )
        {
            pdfPix11[0] = dfStrengthA * ( dfStrengthA * pdfPix11[0] + dfStrength * ( pdfPix01[0] + pdfPix10[0] - dfStrengthA1 * pdfPix00[0] ) );
        }


CLEANUP:
    return res;
}

typedef union 
{
    AWPDOUBLE value;
    struct 
    {
        AWPDWORD     :32;
        AWPDWORD     :31;
        AWPDWORD sign : 1;
    } data;
} __ieee_AWPAWPDOUBLE_shape_type;


AWPRESULT awpNormalizeAdaptive( awpImage* pImage,  awpImage const* pAverage, awpImage const* pDispercy )
{
    AWPRESULT                   res = AWP_OK;
      
    AWPINT					        iOffset = pImage->sSizeX* pImage->sSizeY;

    AWPDOUBLE                      delta, scale;
    __ieee_AWPAWPDOUBLE_shape_type*   pDelta = (__ieee_AWPAWPDOUBLE_shape_type*)&delta;
    __ieee_AWPAWPDOUBLE_shape_type*   pScale = (__ieee_AWPAWPDOUBLE_shape_type*)&scale;

    AWPDOUBLE*                     pdfImage = (AWPDOUBLE*)pImage->pPixels;
    AWPDOUBLE const*               pdfAverage = (AWPDOUBLE const*)pAverage->pPixels;
    AWPDOUBLE const*               pdfDispercy = (AWPDOUBLE const*)pDispercy->pPixels;

    _CHECK_RESULT_( res = awpCheckImage(pImage) );
    _CHECK_RESULT_( res = awpCheckImage(pAverage) );
    _CHECK_RESULT_( res = awpCheckImage(pDispercy) );

    //TODO: check sizes;

    for (; iOffset > 0; --iOffset, ++pdfImage, ++pdfAverage, ++pdfDispercy )
    {
      /*  dfValue	= pdfImage[0] - pdfAverage[0];
        dfValue	*= dfValue;

        if ( dfValue > pdfDispercy[0] || 0.0 == pdfDispercy[0] )
        {
            if ( pdfImage[0] < pdfAverage[0] )
            {
                pdfImage[0]	= 0.0;
            }
            else
            {
                pdfImage[0]	= range;
            }
        }
        else
        {
            if ( pdfImage[0] < pdfAverage[0] )
            {
                pdfImage[0] = mean - dispLo * dfValue / pdfDispercy[0];
            }
            else
            {
                pdfImage[0] = mean + dispHi * dfValue / pdfDispercy[0];
            }
        }*/

      
        delta	= pdfImage[0] - pdfAverage[0];
        //Calc scale of disp;        
        scale = delta * delta/( pdfDispercy[0] + DBL_MIN );
        //Clamp scale to [0, 1];
        scale = AWP_MIN( scale , 1.0 );
        //Copy sign;
        pScale->data.sign = pDelta->data.sign;
        //scale = _copysign( scale, delta );
        pdfImage[0] = 0.5 + 0.5*scale;
    }
CLEANUP:
    return res;
}

AWPRESULT awpDispercy( awpImage const* pImage, awpImage const* pAverage, awpImage* pDispercy )
{
    AWPRESULT res = AWP_OK;
       
    AWPINT					iOffset = pImage->sSizeX*pImage->sSizeY;

    AWPDOUBLE const* pdfImage = (AWPDOUBLE const*)pImage->pPixels;
    AWPDOUBLE const* pdfAverage = (AWPDOUBLE const*)pAverage->pPixels;
    AWPDOUBLE* pdfDispercy = (AWPDOUBLE*)pDispercy->pPixels;

    _CHECK_RESULT_( res = awpCheckImage(pImage) );
    _CHECK_RESULT_( res = awpCheckImage(pAverage) );
    _CHECK_RESULT_( res = awpCheckImage(pDispercy) );

    for ( ; iOffset > 0 ; --iOffset, ++pdfImage, ++pdfAverage, ++pdfDispercy )
    {
        pdfDispercy[0]	= pdfImage[0] - pdfAverage[0];
        pdfDispercy[0] *= pdfDispercy[0];
    }


CLEANUP:
    return res;
}
 
