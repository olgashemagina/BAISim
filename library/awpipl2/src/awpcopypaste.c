/*
    AWP Image library v2
    
    $Source: /home/cvs/awp/imageproc/awpipl-v2/src/awpcopypaste.c,v $
    $Revision: 1.13 $
    $Date: 2005/04/11 14:27:59 $
		
    Authors: Eugene Eremin <medlab@appl.sci-nnov.ru>
	Alexander Telnykh <alt@awp.nnov.ru>
*/


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include "_awpipl.h"

/*copying one image to another
*/
AWPRESULT awpCopyImage(const awpImage* pSrcImage, awpImage** ppDstImage)
{
    AWPRESULT res;
    AWPINT  isize;
    if (ppDstImage == NULL)
        return AWP_BADARG;
	
	if (*ppDstImage != NULL)
		return awpFastCopyImage(pSrcImage, *ppDstImage);

    _CHECK_RESULT_((res = awpCheckImage(pSrcImage)))

    _CHECK_RESULT_((res = awpCreateImage(ppDstImage, pSrcImage->sSizeX, pSrcImage->sSizeY,
                                         pSrcImage->bChannels, pSrcImage->dwType)))

    _CHECK_RESULT_((res = awpGetImageSize(pSrcImage, &isize)))

    /*copy pixels*/
    memcpy((*ppDstImage)->pPixels,pSrcImage->pPixels, isize);
CLEANUP:
    return res;
}
/*Quick copying an image into prepared memory pDstImage
*/
AWPRESULT awpFastCopyImage(const awpImage* pSrcImage, awpImage* pDstImage)
{
    AWPRESULT res;

    AWPINT isize;
    AWPINT isize_dst;

    _CHECK_RESULT_((res = awpCheckImage(pSrcImage)))
    _CHECK_RESULT_((res = awpCheckImage(pDstImage)))

    _CHECK_RESULT_((res = awpGetImageSize(pSrcImage, &isize)))
    _CHECK_RESULT_((res = awpGetImageSize(pDstImage, &isize_dst)))

    if (isize != isize_dst)
        _ERROR_EXIT_RES_(AWP_BADARG);

    /*copy pixels*/
    memcpy(pDstImage->pPixels,pSrcImage->pPixels, isize);
CLEANUP:
    return res;
}
/*Copying a rectangular image of one image to another
*/
AWPRESULT awpCopyRect (const awpImage* pSrcImage, awpImage** ppDstImage, const awpRect* pRect)
{
    AWPRESULT res;
    AWPINT i;
	AWPWORD w,h;
    AWPBYTE *tmp/*,*tmp_ch*/,*src;
    AWPINT iImageType;
    AWPINT PixelSize;

    res = AWP_OK;

    if (ppDstImage == NULL || pRect == NULL)
        return AWP_BADARG;

    _CHECK_RESULT_((res = awpCheckImage(pSrcImage)))
    _CHECK_RESULT_((res = awpCheckRect(pRect)))
    _CHECK_RESULT_((res = awpRectInImage(pSrcImage,pRect)))

    w = pRect->right - pRect->left;
    h = pRect->bottom - pRect->top;

    _CHECK_RESULT_((res = awpCreateImage(ppDstImage, w, h, pSrcImage->bChannels, pSrcImage->dwType)))


    _CHECK_RESULT_((res = awpGetImagePixelType( pSrcImage, &iImageType)))

    switch(iImageType)
    {
    case AWP_BYTE:
		PixelSize = sizeof(AWPBYTE)*pSrcImage->bChannels;
        break;
    case AWP_SHORT:
        PixelSize = sizeof(AWPSHORT)*pSrcImage->bChannels;
        break;
    case AWP_FLOAT:
        PixelSize = sizeof(AWPFLOAT)*pSrcImage->bChannels;
        break;
    case AWP_DOUBLE:
        PixelSize = sizeof(AWPDOUBLE)*pSrcImage->bChannels;

        break;
    default:
        {
            res = AWP_BADARG;
            _ERR_EXIT_
        }
    }

    tmp=(AWPBYTE*)(*ppDstImage)->pPixels;
    src=(AWPBYTE*)pSrcImage->pPixels + pRect->top*pSrcImage->sSizeX*PixelSize + pRect->left*PixelSize;

    for (i = 0; i < h; i++)
    {
	    memcpy(tmp, src, w*PixelSize);
        tmp += w*PixelSize;
        src += pSrcImage->sSizeX*PixelSize;
    }

CLEANUP:
    return res;
}

AWPRESULT awpPasteRect(const awpImage* pSrcImage, awpImage* pDstImage,  const awpPoint Pos)
{
    AWPRESULT res;
    AWPINT iImageType;
    AWPINT PixelSize;
    AWPBYTE *tmp,*src;
    AWPINT i;

    _CHECK_RESULT_((res = awpCheckImage(pSrcImage)))
    _CHECK_RESULT_((res = awpCheckImage(pDstImage)))
    if (pSrcImage->dwType != pDstImage->dwType)
        _ERROR_EXIT_RES_(AWP_BADARG)
    if (pSrcImage->bChannels != pDstImage->bChannels)
        _ERROR_EXIT_RES_(AWP_BADARG)
    if (Pos.X + pSrcImage->sSizeX > pDstImage->sSizeX)
        _ERROR_EXIT_RES_(AWP_BADARG)
    if (Pos.Y + pSrcImage->sSizeY > pDstImage->sSizeY)
        _ERROR_EXIT_RES_(AWP_BADARG)


    _CHECK_RESULT_((res = awpGetImagePixelType( pSrcImage, &iImageType)))

    switch(iImageType)
    {
    case AWP_BYTE:
		PixelSize = sizeof(AWPBYTE)*pSrcImage->bChannels;
        break;
    case AWP_SHORT:
        PixelSize = sizeof(AWPSHORT)*pSrcImage->bChannels;
        break;
    case AWP_FLOAT:
        PixelSize = sizeof(AWPFLOAT)*pSrcImage->bChannels;
        break;
    case AWP_DOUBLE:
        PixelSize = sizeof(AWPDOUBLE)*pSrcImage->bChannels;
        break;
    default:
        {
            res = AWP_BADARG;
            _ERR_EXIT_
        }
    }

    src = (AWPBYTE*)pSrcImage->pPixels;
    tmp = (AWPBYTE*)pDstImage->pPixels + Pos.Y*pDstImage->sSizeX*PixelSize + Pos.X*PixelSize;
    for (i = 0; i < pSrcImage->sSizeY; i++)
    {
	    memcpy(tmp, src, pSrcImage->sSizeX*PixelSize);
        src += pSrcImage->sSizeX*PixelSize;
        tmp += pDstImage->sSizeX*PixelSize;
    }

CLEANUP:
    return res;
}

AWPRESULT awpCopyImageStr(const awpImage* pSrcImage, awpImage** ppDstImage, const awpStrokeObj* pStr)
{
    AWPRESULT res;
    AWPDWORD i;
    AWPINT PixelSize;
    AWPINT iImageType;
    AWPBYTE *dst,*src;

     _CHECK_RESULT_((res = awpCheckImage(pSrcImage)))
     if (*ppDstImage == NULL)
        _CHECK_RESULT_((res = awpCreateImage(ppDstImage, pSrcImage->sSizeX, pSrcImage->sSizeY, pSrcImage->bChannels, pSrcImage->dwType)))
    if (pStr == NULL)
        _ERROR_EXIT_RES_(AWP_BADARG)

    _CHECK_RESULT_((res = awpGetImagePixelType( pSrcImage, &iImageType)))

    switch(iImageType)
    {
    case AWP_BYTE:
		PixelSize = sizeof(AWPBYTE)*pSrcImage->bChannels;
        break;
    case AWP_SHORT:
        PixelSize = sizeof(AWPSHORT)*pSrcImage->bChannels;
        break;
    case AWP_FLOAT:
        PixelSize = sizeof(AWPFLOAT)*pSrcImage->bChannels;
        break;
    case AWP_DOUBLE:
        PixelSize = sizeof(AWPDOUBLE)*pSrcImage->bChannels;

        break;
    default:
        {
            res = AWP_BADARG;
            _ERR_EXIT_
        }
    }

    for (i = 0; i < pStr->Num; i++)
    {
        src = (AWPBYTE*)pSrcImage->pPixels + pStr->strokes[i].y*pSrcImage->sSizeX*PixelSize +
        pStr->strokes[i].xl*PixelSize;
        dst = (AWPBYTE*)(*ppDstImage)->pPixels + pStr->strokes[i].y*(*ppDstImage)->sSizeX*PixelSize +
        pStr->strokes[i].xl*PixelSize;
        memcpy(dst, src, PixelSize*(pStr->strokes[i].xr - pStr->strokes[i].xl));
    }
        
CLEANUP:
    return res;
}

AWPRESULT awpIntegral( awpImage* pSrc, awpImage** ppDst, AWPINT type )
{
    AWPRESULT res;
    AWPBYTE* sPix;
    AWPDOUBLE* dPix;
    AWPINT *s;
    AWPINT isize, w, h, i, j;
    if (ppDst == NULL)
        return AWP_BADARG;

    _CHECK_RESULT_((res = awpCheckImage(pSrc)))

    _CHECK_RESULT_((res = awpCreateImage(ppDst, pSrc->sSizeX, pSrc->sSizeY,
                                         pSrc->bChannels, AWP_DOUBLE)))

    _CHECK_RESULT_((res = awpGetImageSize(pSrc, &isize)))

    if( pSrc->dwType != AWP_BYTE )
        return AWP_BADARG;
    w = pSrc->sSizeX;
    h = pSrc->sSizeY;

    s = (AWPINT*)malloc(w*sizeof(AWPINT));

    sPix = (AWPBYTE*)pSrc->pPixels;
    dPix = (AWPDOUBLE*)(*ppDst)->pPixels;

    switch(type)
    {
        case AWP_LINEAR:

            for( i = 0; i < h; i++ )
            {
                for( j = 0; j < w; j++ )
                {
                    if( i == 0 )
                    {
                        s[j] = sPix[j];
                        if( j == 0 )
                            dPix[0] = s[0];
                        else
                            dPix[j] = dPix[j-1] + s[j];

                    }
                    else
                    {
                        s[j] += sPix[j + i*w];
                        if( j == 0 )
                        {
                            dPix[i*w] = s[0];
                        }
                        else
                        {
                            dPix[j + i*w] = dPix[(j-1) + i*w] + s[j];
                        }
                    }
                }

            }
            free(s);
        break;
        case AWP_SQUARE:

            for( i = 0; i < h; i++ )
            {
                for( j = 0; j < w; j++ )
                {
                    if( i == 0 )
                    {
                        s[j] = sPix[j]*sPix[j];
                        if( j == 0 )
                            dPix[0] = s[0];
                        else
                            dPix[j] = dPix[j-1] + s[j];

                    }
                    else
                    {
                        s[j] += (sPix[j + i*w]*sPix[j + i*w]);
                        if( j == 0 )
                        {
                            dPix[i*w] = s[0];
                        }
                        else
                        {
                            dPix[j + i*w] = dPix[(j-1) + i*w] + s[j];
                        }
                    }
                }

            }
            free(s);
        break;

		case AWP_RLINEAR:
			//See intel object detection for additional info;
			//[ANDX]
			//Copy first line x;
			for ( j = 0; j < w; ++j )
			{
				dPix[j] = sPix[j];
			}

			//Left'n'right;
			dPix[w] = dPix[1]
			+ sPix[w] + sPix[0];

			dPix[w+w-1] = dPix[w-2]
			+ sPix[w+w-1] + sPix[w-1];

			//The second line;
			for ( j = 1; j < w-1; ++j )
			{
				dPix[j+w] = dPix[j-1] + dPix[j+1]
				+ sPix[j + w] + sPix[j];
			}

			for( i = 2; i < h; ++i )
			{
				AWPINT ioffs = i*w;
				//left and right pixes;
				dPix[ioffs] = dPix[1 + ioffs-w]-dPix[ioffs-w-w]
				+ sPix[ ioffs] + sPix[ ioffs-w];

				dPix[w-1 + ioffs] = dPix[-2 + ioffs]-dPix[ioffs-w-w]
				+ sPix[ w-1 + ioffs] + sPix[ -1 + ioffs];

				for( j = 1; j < w-1; ++j )
				{
					dPix[j + ioffs] = 
						dPix[j-1 + ioffs-w] + dPix[j+1 + ioffs-w]-dPix[j + ioffs-w-w]
					+ sPix[j + ioffs] + sPix[j + ioffs-w];
				}

			}
			free(s);
			break;

		case AWP_RSQUARE:
			//Copy first line x;
			for ( j = 0; j < w; ++j )
			{
				dPix[j] = sPix[j]*sPix[j];
			}

			//Left'n'right;
			dPix[w] = dPix[1]
			+ sPix[w]*sPix[w] + sPix[0]*sPix[0];

			dPix[w+w-1] = dPix[w-2]
			+ sPix[w+w-1]*sPix[w+w-1] + sPix[w-1]*sPix[w-1];

			//The second line;
			for ( j = 1; j < w-1; ++j )
			{
				dPix[j+w] = dPix[j-1] + dPix[j+1]
				+ sPix[j + w]*sPix[j + w] + sPix[j]*sPix[j];
			}

			for( i = 2; i < h; ++i )
			{
				AWPINT ioffs = i*w;
				//left and right pixes;
				dPix[ioffs] = dPix[1 + ioffs-w]-dPix[ioffs-w-w]
				+ sPix[ ioffs]*sPix[ ioffs] + sPix[ ioffs-w]*sPix[ ioffs-w];

				dPix[w-1 + ioffs] = dPix[-2 + ioffs]-dPix[ioffs-w-w]
				+ sPix[ w-1 + ioffs]*sPix[ w-1 + ioffs] + sPix[ -1 + ioffs]*sPix[ -1 + ioffs];

				for( j = 1; j < w-1; ++j )
				{
					dPix[j + ioffs] = 
						dPix[j-1 + ioffs-w] + dPix[j+1 + ioffs-w]-dPix[j + ioffs-w-w]
					+ sPix[j + ioffs]*sPix[j + ioffs] + sPix[j + ioffs-w]*sPix[j + ioffs-w];
				}

			}
			free(s);
			break;

    }
    CLEANUP:
        return res;
}

AWPRESULT awpFastIntegral( awpImage* pSrc, awpImage** ppDst, AWPINT type )
{
    AWPRESULT res;
    AWPBYTE* sPix;
    AWPDOUBLE* dPix;
    AWPINT *s;
    AWPINT isize, /*isize_dst,*/ w, h, i, j;
    if (ppDst == NULL)
        return AWP_BADARG;

    _CHECK_RESULT_((res = awpCheckImage(pSrc)))

    _CHECK_RESULT_((res = awpGetImageSize(pSrc, &isize)))


    if( pSrc->dwType != AWP_BYTE )
        return AWP_BADARG;


    w = pSrc->sSizeX;
    h = pSrc->sSizeY;

    s = (AWPINT*)malloc(w*sizeof(AWPINT));

    sPix = (AWPBYTE*)pSrc->pPixels;
    dPix = (AWPDOUBLE*)(*ppDst)->pPixels;

    switch(type)
    {
        case AWP_LINEAR:

            for( i = 0; i < h; i++ )
            {
                for( j = 0; j < w; j++ )
                {
					//[ANDX]
                    if( i == 0 )
                    {
						s[j] = sPix[j];
                        if( j == 0 )
                            dPix[0] = s[0];
                        else
                            dPix[j] = dPix[j-1] + s[j];

                    }
                    else
                    {
                        s[j] += sPix[j + i*w];
                        if( j == 0 )
                        {
                            dPix[i*w] = s[0];
                        }
                        else
                        {
                            dPix[j + i*w] = dPix[(j-1) + i*w] + s[j];
                        }
                    }
                }

            }
            free(s);
        break;
        case AWP_SQUARE:

            for( i = 0; i < h; i++ )
            {
                for( j = 0; j < w; j++ )
                {
                    if( i == 0 )
                    {
                        s[j] = sPix[j + i*w]*sPix[j + i*w];
                        if( j == 0 )
                            dPix[j + i*w] = s[j];
                        else
                            dPix[j + i*w] = dPix[(j-1) + i*w] + s[j];

                    }
                    else
                    {
                        s[j] += sPix[j + i*w]*sPix[j + i*w];
                        if( j == 0 )
                        {
                            dPix[j + i*w] = s[j];
                        }
                        else
                        {
                            dPix[j + i*w] = dPix[(j-1) + i*w] + s[j];
                        }
                    }
                }

            }
            free(s);
        break;

		case AWP_RLINEAR:
			//Copy first line x;
			for ( j = 0; j < w; ++j )
			{
				dPix[j] = sPix[j];
			}
			
			//Left'n'right;
			dPix[w] = dPix[1]
			+ sPix[w] + sPix[0];

			dPix[w+w-1] = dPix[w-2]
			+ sPix[w+w-1] + sPix[w-1];
			
			//The second line;
			for ( j = 1; j < w-1; ++j )
			{
				dPix[j+w] = dPix[j-1] + dPix[j+1]
				+ sPix[j + w] + sPix[j];
			}

			for( i = 2; i < h; ++i )
			{
				AWPINT ioffs = i*w;
				//left and right pixes;
				dPix[ioffs] = dPix[1 + ioffs-w]-dPix[ioffs-w-w]
				+ sPix[ ioffs] + sPix[ ioffs-w];

				dPix[w-1 + ioffs] = dPix[-2 + ioffs]-dPix[ioffs-w-w]
				+ sPix[ w-1 + ioffs] + sPix[ -1 + ioffs];

				for( j = 1; j < w-1; ++j )
				{
					dPix[j + ioffs] = 
						dPix[j-1 + ioffs-w] + dPix[j+1 + ioffs-w]-dPix[j + ioffs-w-w]
					+ sPix[j + ioffs] + sPix[j + ioffs-w];
				}

			}
			free(s);
			break;

		case AWP_RSQUARE:
			//Copy first line x;
			for ( j = 0; j < w; ++j )
			{
				dPix[j] = sPix[j]*sPix[j];
			}

			//Left'n'right;
			dPix[w] = dPix[1]
			+ sPix[w]*sPix[w] + sPix[0]*sPix[0];

			dPix[w+w-1] = dPix[w-2]
			+ sPix[w+w-1]*sPix[w+w-1] + sPix[w-1]*sPix[w-1];

			//The second line;
			for ( j = 1; j < w-1; ++j )
			{
				dPix[j+w] = dPix[j-1] + dPix[j+1]
				+ sPix[j + w]*sPix[j + w] + sPix[j]*sPix[j];
			}

			for( i = 2; i < h; ++i )
			{
				AWPINT ioffs = i*w;
				//left and right pixes;
				dPix[ioffs] = dPix[1 + ioffs-w]-dPix[ioffs-w-w]
				+ sPix[ ioffs]*sPix[ ioffs] + sPix[ ioffs-w]*sPix[ ioffs-w];

				dPix[w-1 + ioffs] = dPix[-2 + ioffs]-dPix[ioffs-w-w]
				+ sPix[ w-1 + ioffs]*sPix[ w-1 + ioffs] + sPix[ -1 + ioffs]*sPix[ -1 + ioffs];

				for( j = 1; j < w-1; ++j )
				{
					dPix[j + ioffs] = 
						dPix[j-1 + ioffs-w] + dPix[j+1 + ioffs-w]-dPix[j + ioffs-w-w]
					+ sPix[j + ioffs]*sPix[j + ioffs] + sPix[j + ioffs-w]*sPix[j + ioffs-w];
				}

			}
			free(s);
			break;
    }
    CLEANUP:
        return res;
}

//Shifted intregral field for fast computing;
AWPRESULT awpShlIntegral( awpImage* pSrc, awpImage** ppDst, AWPINT type )
{
    AWPRESULT res;
    AWPBYTE* sPix;
    AWPDOUBLE* dPix;
    AWPINT *s;
    AWPINT isize, w, h, i, j;
    if (ppDst == NULL)
        return AWP_BADARG;

    _CHECK_RESULT_((res = awpCheckImage(pSrc)))

    switch(type)
    {
    case AWP_LINEAR:
    case AWP_SQUARE:
        _CHECK_RESULT_((res = awpCreateImage(ppDst, (AWPWORD)(pSrc->sSizeX + 1), (AWPWORD)(pSrc->sSizeY + 1),
            pSrc->bChannels, AWP_DOUBLE)));
    
        break;
    case AWP_RLINEAR:
    case AWP_RSQUARE:
        _CHECK_RESULT_((res = awpCreateImage(ppDst, pSrc->sSizeX, (AWPWORD)(pSrc->sSizeY + 1),
            pSrc->bChannels, AWP_DOUBLE)));

        break;
    }
 

   _CHECK_RESULT_((res = awpGetImageSize(pSrc, &isize)))

    if( pSrc->dwType != AWP_BYTE )
        return AWP_BADARG;

    w = pSrc->sSizeX;
    h = pSrc->sSizeY;

    s = (AWPINT*)malloc(w*sizeof(AWPINT));

    sPix = (AWPBYTE*)pSrc->pPixels;
    dPix = (AWPDOUBLE*)(*ppDst)->pPixels;

    switch(type)
    {
    case AWP_LINEAR:
        
        for( j = 0; j < w+1; j++, ++dPix )
        {
            dPix[0] = 0;
        }
        
        for( i = 0; i < h; i++ )
        {
            dPix[0] = 0;
            ++dPix;

            
            for( j = 0; j < w; j++, ++dPix, ++sPix )
            {
                if( i == 0 )
                {
                    s[j] = sPix[j];
                    if( j == 0 )
                        dPix[0] = s[0];
                    else
                        dPix[0] = dPix[-1] + s[j];

                }
                else
                {
                    s[j] += sPix[0];
                    if( j == 0 )
                    {
                        dPix[0] = s[j];
                    }
                    else
                    {
                        dPix[0] = dPix[-1] + s[j];
                    }
                }
            }

        }
        free(s);
        break;
    case AWP_SQUARE:

        for( j = 0; j < w+1; j++, ++dPix )
        {
            dPix[0] = 0;
        }

        for( i = 0; i < h; i++ )
        {
            dPix[0] = 0;
            ++dPix;


            for( j = 0; j < w; j++, ++dPix, ++sPix )
            {
                if( i == 0 )
                {
                    s[j] = sPix[0]*sPix[0];
                    if( j == 0 )
                        dPix[0] = s[0];
                    else
                        dPix[0] = dPix[-1] + s[j];

                }
                else
                {
                    s[j] += sPix[0]*sPix[0];
                    if( j == 0 )
                    {
                        dPix[0] = s[j];
                    }
                    else
                    {
                        dPix[0] = dPix[-1] + s[j];
                    }
                }
            }

        }
     
        free(s);
        break;

    case AWP_RLINEAR:
        //See intel object detection for additional info;
        //[ANDX]
        //Copy first line x;
     
        for ( j = 0; j < w; ++j, ++dPix )
        {
            dPix[0] = 0;
            dPix[w] = sPix[j];
        }
        
        //Left'n'right;
        dPix[w] = dPix[1]
        + sPix[w] + dPix[0];

        dPix += w;
        sPix += w;

        dPix[w-1] = dPix[-2]
        + sPix[w-1] + dPix[-1];

        //The second line;
        for ( j = 1; j < w-1; ++j )
        {
            dPix[j] = dPix[j-w-1] + dPix[j-w+1]
            + sPix[j ] + sPix[j-w];
        }

        dPix += w;

        for( i = 2; i < h; ++i )
        {
//            AWPINT ioffs = i*w;

            dPix[0] = dPix[1-w] - dPix[-w-w] + sPix[ 0 ] + sPix[ -w];

            ++dPix;
            ++sPix;
           
            for( j = 1; j < w-1; ++j, ++dPix, ++sPix )
            {
                dPix[0] = dPix[-1 -w] + dPix[1-w] - dPix[-w-w]
                + sPix[0] + sPix[-w];
            }

            dPix[0] = dPix[-1 -w ] - dPix[-w-w] + sPix[ 0] + sPix[ -w];

            ++dPix;
            ++sPix;


        }
        free(s);
        break;

    case AWP_RSQUARE:
        for ( j = 0; j < w; ++j, ++dPix )
        {
            dPix[0] = 0;
            dPix[w] = sPix[j]*sPix[j];
        }

        //Left'n'right;
        dPix[w] = dPix[1]
        + sPix[w]*sPix[w] + dPix[0];

        dPix += w;
        sPix += w;

        dPix[w-1] = dPix[-2]
        + sPix[w-1]*sPix[w-1] + dPix[-1];

        //The second line;
        for ( j = 1; j < w-1; ++j )
        {
            dPix[j] = dPix[j-w-1] + dPix[j-w+1]
            + sPix[j ]*sPix[j ] + sPix[j-w]*sPix[j-w];
        }

        dPix += w;

        for( i = 2; i < h; ++i )
        {
            
            dPix[0] = dPix[1-w] - dPix[-w-w] + sPix[ 0 ]*sPix[ 0 ] + sPix[ -w]*sPix[ -w];

            ++dPix;
            ++sPix;

            for( j = 1; j < w-1; ++j, ++dPix, ++sPix )
            {
                dPix[0] = dPix[-1 -w] + dPix[1-w] - dPix[-w-w]
                + sPix[0]*sPix[0] + sPix[-w]*sPix[-w];
            }

            dPix[0] = dPix[-1 -w ] - dPix[-w-w] + sPix[ 0]*sPix[ 0] + sPix[ -w]*sPix[ -w];

            ++dPix;
            ++sPix;


        }
      
        free(s);
        break;

    }
CLEANUP:
    return res;
}


//Shifted intregral field for fast computing;
AWPRESULT awpIntegral2( awpImage const* pSrc, awpImage* pDst, AWPINT type )
{
    AWPRESULT res;
    AWPDOUBLE* sPix;
    AWPDOUBLE* dPix;
    AWPDOUBLE *s;
    AWPINT isize, w, h, i, j;
    if (pDst == NULL)
        return AWP_BADARG;

    _CHECK_RESULT_((res = awpCheckImage(pSrc)));
    _CHECK_RESULT_((res = awpCheckImage(pDst)));

    _CHECK_RESULT_((res = awpGetImageSize(pSrc, &isize)));
     

    if( pSrc->dwType != AWP_DOUBLE )
    {
            return AWP_BADARG;
    }


    w = pSrc->sSizeX;
    h = pSrc->sSizeY;

    s = (AWPDOUBLE*)malloc(w*sizeof(AWPDOUBLE));

    sPix = (AWPDOUBLE*)pSrc->pPixels;
    dPix = (AWPDOUBLE*)(pDst)->pPixels;

    switch(type)
    {
    case AWP_LINEAR:

        for( i = 0; i < h; i++ )
        {
            /*dPix[0] = 0;
            ++dPix;*/


            for( j = 0; j < w; j++, ++dPix, ++sPix )
            {
                if( i == 0 )
                {
                    s[j] = sPix[j];
                    if( j == 0 )
                        dPix[0] = s[0];
                    else
                        dPix[0] = dPix[-1] + s[j];

                }
                else
                {
                    s[j] += sPix[0];
                    if( j == 0 )
                    {
                        dPix[0] = s[j];
                    }
                    else
                    {
                        dPix[0] = dPix[-1] + s[j];
                    }
                }
            }

        }
        
        break;
    case AWP_SQUARE:

     
        for( i = 0; i < h; i++ )
        {
           
            for( j = 0; j < w; j++, ++dPix, ++sPix )
            {
                if( i == 0 )
                {
                    s[j] = sPix[0]*sPix[0];
                    if( j == 0 )
                        dPix[0] = s[0];
                    else
                        dPix[0] = dPix[-1] + s[j];

                }
                else
                {
                    s[j] += sPix[0]*sPix[0];
                    if( j == 0 )
                    {
                        dPix[0] = s[j];
                    }
                    else
                    {
                        dPix[0] = dPix[-1] + s[j];
                    }
                }
            }

        }

        
        break;

    case AWP_RLINEAR:
        
        //See intel object detection for additional info;
        //[ANDX]
        //Copy first line x;

        for ( j = 0; j < w; ++j )
        {
            //dPix[0] = 0;
            dPix[j] = sPix[j];
        }

        //Left'n'right;
        dPix[w] = dPix[1]
        + sPix[w] + dPix[0];

        dPix += w;
        sPix += w;

        dPix[w-1] = dPix[-2]
        + sPix[w-1] + dPix[-1];

        //The second line;
        for ( j = 1; j < w-1; ++j )
        {
            dPix[j] = dPix[j-w-1] + dPix[j-w+1]
            + sPix[j ] + sPix[j-w];
        }

        dPix += w;
        sPix += w;

        for( i = 2; i < h; ++i )
        {
         
            //Left point:
            dPix[0] = dPix[1-w] - dPix[-w-w] + sPix[ 0 ] + sPix[ -w];

            ++dPix;
            ++sPix;

            for( j = 1; j < w-1; ++j, ++dPix, ++sPix )
            {
                dPix[0] = dPix[-1 -w] + dPix[1-w] - dPix[-w-w]
                + sPix[0] + sPix[-w];
            }

            //Right point:
            dPix[0] = dPix[-1 -w ] - dPix[-w-w] + sPix[ 0] + sPix[ -w];

            ++dPix;
            ++sPix;


        }
        break;

    case AWP_RSQUARE:

#define SQR( a ) ((a)*(a))

        for ( j = 0; j < w; ++j, ++dPix, ++sPix )
        {
            dPix[0] = SQR(sPix[0]);
        }

        //Left'n'right;
        dPix[0] = dPix[-w+1] + dPix[-w] + SQR(sPix[0]);
        //dPix[w] = dPix[1] + SQR(sPix[w]) + dPix[0];

        //dPix += w;
        //sPix += w;

        dPix[w-1] = dPix[-2] + SQR( sPix[w-1] ) + dPix[-1];

        //The second line;
        for ( j = 1; j < w-1; ++j )
        {
            dPix[j] = dPix[j-w-1] + dPix[j-w+1] + SQR(sPix[j ]) + SQR( sPix[j-w] );
        }

        dPix += w;
        sPix += w;

        for( i = 2; i < h; ++i )
        {

            dPix[0] = dPix[1-w] - dPix[-w-w] + SQR(sPix[ 0 ]) + SQR(sPix[ -w]);

            ++dPix;
            ++sPix;

            for( j = 1; j < w-1; ++j, ++dPix, ++sPix )
            {
                dPix[0] = dPix[-1 -w] + dPix[1-w] - dPix[-w-w]
                + SQR(sPix[0]) + SQR(sPix[-w]);
            }

            dPix[0] = dPix[-1 -w ] - dPix[-w-w] + SQR(sPix[ 0]) + SQR(sPix[ -w]);

            ++dPix;
            ++sPix;


        }
        break;

    }
    free(s);
CLEANUP:
    return res;
}
/*
Copying of the pSrcImage image portion into the pDstImage image.
the size and position of the teacher is specified by the parameter pRect
*/
AWPRESULT awpCopyRectSize(const awpImage* pSrcImage, awpImage* pDstImage, const awpRect* pRect)
{

	// local variables
	float x0,y0; // left top corner from where the image is copied 
	float w1,h1; // the size of the rectangle to copy
	float w,h;   // The dimensions of the image into which the pixels of the original image are copied
	float alfa, beta; // coefficient. scaling
	int   x,y;  // the coordinates of the point in the resulting image
	int   x1,y1; // coordinates of the point on the original image
    AWPINT iImageType;
    AWPINT PixelSize;

	AWPBYTE* dst0;// = (AWPBYTE*)(pDstImage)->pPixels;
    AWPBYTE* src0;// = (AWPBYTE*)pSrcImage->pPixels;
	AWPBYTE* dst; //= NULL;
	AWPBYTE* src; //= NULL;

    AWPRESULT res = AWP_OK;
	if (pSrcImage == NULL || pDstImage == NULL || pRect == NULL)
		_ERROR_EXIT_RES_ (AWP_BADARG)


	_CHECK_RESULT_((res = awpCheckImage(pSrcImage)))
    _CHECK_RESULT_((res = awpCheckImage(pDstImage)))
	_CHECK_RESULT_((res = awpCheckRect(pRect)))
	_CHECK_RESULT_((res = awpRectInImage(pSrcImage,  pRect)))
	_CHECK_SAME_TYPE(pSrcImage, pDstImage)
	_CHECK_NUM_CHANNELS(pSrcImage, pDstImage)



    _CHECK_RESULT_((res = awpGetImagePixelType( pSrcImage, &iImageType)))

    switch(iImageType)
    {
    case AWP_BYTE:
		PixelSize = sizeof(AWPBYTE)*pSrcImage->bChannels;
        break;
    case AWP_SHORT:
        PixelSize = sizeof(AWPSHORT)*pSrcImage->bChannels;
        break;
    case AWP_FLOAT:
        PixelSize = sizeof(AWPFLOAT)*pSrcImage->bChannels;
        break;
    case AWP_DOUBLE:
        PixelSize = sizeof(AWPFLOAT)*pSrcImage->bChannels;

        break;
    default:
        {
            res = AWP_BADARG;
            _ERR_EXIT_
        }
    }


	dst0 = (AWPBYTE*)(pDstImage)->pPixels;
    src0 = (AWPBYTE*)pSrcImage->pPixels;
	dst = NULL;
	src = NULL;

	x0 = (float)pRect->left;
	y0 = (float)pRect->top;
	w1 = (float)(pRect->right - pRect->left);
	h1 = (float)(pRect->bottom - pRect->top);
	w  = (float)pDstImage->sSizeX;
	h  = (float)pDstImage->sSizeY;
	alfa = w1 / w;
	beta = h1 / h;
	

	for (y = 0; y < pDstImage->sSizeY; y++)
	{
		y1 = (int)floor(beta*y + y0 +0.5);
		for (x = 0; x < pDstImage->sSizeX; x++)
		{
			x1 = (int)floor(alfa*x + x0 +0.5);
			dst = dst0 + PixelSize*(x + y*pDstImage->sSizeX);
			src = src0 + PixelSize*(x1 + y1*pSrcImage->sSizeX);
			memcpy(dst, src, PixelSize);
		}
	}


CLEANUP:
    return res;
}
