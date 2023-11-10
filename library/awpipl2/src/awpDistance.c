/*
//
//  RCS:
//      $Source: awpDistance.c
//      $Revision: 1.0 $
//      $Date:
//
//    Purpose:  AWPIPL source file
//    Contents: source code for awpImage distance measurement
//    Authors : Alexander Telnykh
*/

#include "_awpipl.h"

#define __AWP_L1__(u) \
{\
    u* b1 = (u*)src1->pPixels;\
    u* b2 = (u*)src2->pPixels;\
    AWPINT size = src1->sSizeX*src1->sSizeY;\
    AWPINT i = 0;\
    for (i = 0; i < size; i++)\
        *res += fabs(b1[i] - b2[i]);\
}
//---------------------------------------------------------------------------
AWPRESULT awpL1Distance(awpImage* src1, awpImage* src2, AWPDOUBLE* res)
{
    switch(src1->dwType)
    {
        case AWP_BYTE:
        __AWP_L1__(AWPBYTE)
        break;
        case AWP_SHORT:
        __AWP_L1__(AWPSHORT)
        break;
        case AWP_FLOAT:
        __AWP_L1__(AWPFLOAT)
        break;
        case AWP_DOUBLE:
        __AWP_L1__(AWPDOUBLE)
        break;
        default:
            return AWP_BADARG;
    }

    return AWP_OK;
}

#define __AWP_L2__(u) \
{\
    u* b1 = (u*)src1->pPixels;\
    u* b2 = (u*)src2->pPixels;\
    AWPINT size = src1->sSizeX*src1->sSizeY;\
    AWPINT i = 0;\
    for (i = 0; i < size; i++)\
        *res += (b1[i] - b2[i])*(b1[i] - b2[i]);\
}

AWPRESULT awpL2Distance(awpImage* src1, awpImage* src2, AWPDOUBLE* res)
{
    switch(src1->dwType)
    {
        case AWP_BYTE:
        __AWP_L2__(AWPBYTE)
        break;
        case AWP_SHORT:
        __AWP_L2__(AWPSHORT)
        break;
        case AWP_FLOAT:
        __AWP_L2__(AWPFLOAT)
        break;
        case AWP_DOUBLE:
        __AWP_L2__(AWPDOUBLE)
        break;
        default:
            return AWP_BADARG;
    }

    return AWP_OK;
}

#define __AWP_NCC__(u) \
{\
    AWPDOUBLE corr = 0;\
    AWPDOUBLE norm1 = 0;\
    AWPDOUBLE norm2 = 0;\
    AWPDOUBLE avg1 = 0;\
    AWPDOUBLE avg2 = 0; \
    u* f3 = (u*)src1->pPixels;\
    u* f4 = (u*)src2->pPixels;\
    AWPINT i, size = src1->sSizeX*src1->sSizeY;\
    for( i = 0; i < size; i++) \
    {                         \
        avg1 += f3[i];        \
        avg2 += f4[i];        \
    }                        \
    avg1 /= size;            \
    avg2 /= size;           \
    for( i = 0; i < size; i++)\
    {                             \
        corr += (f3[i] - avg1) * (f4[i]-avg2);\
        norm1 += (f3[i]-avg1) * (f3[i] - avg1);\
        norm2 += (f4[i]-avg2) * (f4[i] - avg2);\
    }                          \
    *res= corr / sqrt(norm1*norm2);\
}

AWPRESULT awpNCCDistance(awpImage* src1, awpImage* src2, AWPDOUBLE* res)
{
    switch(src1->dwType)
    {
        case AWP_BYTE:
        __AWP_NCC__(AWPBYTE)
        break;
        case AWP_SHORT:
        __AWP_NCC__(AWPSHORT)
        break;
        case AWP_FLOAT:
        __AWP_NCC__(AWPFLOAT)
        break;
        case AWP_DOUBLE:
        __AWP_NCC__(AWPDOUBLE)
        break;
        default:
            return AWP_BADARG;
    }

    return AWP_OK;

}
#define __AWP_X2__(u) \
{\
    u* b1 = (u*)src1->pPixels;\
    u* b2 = (u*)src2->pPixels;\
    AWPINT size = src1->sSizeX*src1->sSizeY;\
    AWPINT i = 0;\
    for (i = 0; i < size; i++)\
    {\
        if (b1[i] != 0 && b2[i] != 0)\
         *res += ((b1[i] - b2[i])*(b1[i] - b2[i]))/(b1[i] + b2[i]);\
    }  \
}
AWPRESULT awpX2Distance(awpImage* src1, awpImage* src2, AWPDOUBLE* res)
{
    switch(src1->dwType)
    {
        case AWP_BYTE:
        __AWP_X2__(AWPBYTE)
        break;
        case AWP_SHORT:
        __AWP_X2__(AWPSHORT)
        break;
        case AWP_FLOAT:
        __AWP_X2__(AWPFLOAT)
        break;
        case AWP_DOUBLE:
        __AWP_X2__(AWPDOUBLE)
        break;
        default:
            return AWP_BADARG;
    }

    return AWP_OK;
}

/*
Calculates the distance between two images. The result is written to the variable res.
Images should have one data channel of the same size and
be of the same type.
*/
AWPRESULT awpDistance(awpImage* src1, awpImage* src2, AWPBYTE options, AWPDOUBLE* res)
{
    AWPRESULT result = AWP_OK;

    if (src1 == NULL || src2 == NULL || res == NULL)
        return AWP_BADARG;
    if (src1->sSizeX != src2->sSizeX)
        return AWP_BADARG;
    if (src1->sSizeY != src2->sSizeY)
        return AWP_BADARG;
    if (src1->dwType != src2->dwType)
        return AWP_BADARG;
    if (src1->bChannels != 1)
        return AWP_BADARG;
    if (src1->bChannels != src2->bChannels)
        return AWP_BADARG;

    *res = 0;

    switch(options)
    {
        case AWP_DIST_L1:
            result =  awpL1Distance(src1, src2, res);
        break;

        case AWP_DIST_L2:
            result =  awpL2Distance(src1, src2, res);
        break;

        case AWP_DIST_NCC:
            result =  awpNCCDistance(src1, src2, res);
        break;
        case AWP_DIST_X2:
            result =  awpX2Distance(src1, src2, res);
        break;
        case AWP_DIST_MSE:
        	result =  awpL2Distance(src1, src2, res);
            _CHECK_RESULT_(result)
            *res = *res / (AWPDOUBLE)(src1->sSizeX*src1->sSizeY);
        break;

        case AWP_DIST_RMSE:
        	result =  awpL2Distance(src1, src2, res);
            _CHECK_RESULT_(result)
            *res = *res / (AWPDOUBLE)(src1->sSizeX*src1->sSizeY);
            *res = sqrt(*res);
        break;
        default:
            result = AWP_BADARG;
    }

CLEANUP:
    return result;
}

/*
calculates the distance between two images divided by
n fragments. Each fragment of the image is represented by a row of pixels in
variables src1 and src2
weight is the weight of each fragment. If weight == NULL, it is not considered
*/
AWPRESULT awpGridDistance(awpImage* src1, awpImage* src2, awpImage* weight, AWPBYTE options, AWPDOUBLE* res)
{
    AWPRESULT result = AWP_OK;

    awpImage* img1 = NULL;
    awpImage* img2 = NULL;

    AWPINT i = 0;
    AWPINT linesize = 0;
    AWPBYTE* b1 = NULL;
    AWPBYTE* b2 = NULL;
    AWPBYTE* b3 = NULL;
    AWPBYTE* b4 = NULL;
    AWPDOUBLE* w = NULL;

    AWPDOUBLE value = 0;

    if (src1 == NULL || src2 == NULL || res == NULL)
        return AWP_BADARG;
    if (src1->sSizeX != src2->sSizeX)
        return AWP_BADARG;
    if (src1->sSizeY != src2->sSizeY)
        return AWP_BADARG;
    if (src1->dwType != src2->dwType)
        return AWP_BADARG;
    if (src1->bChannels != 1)
        return AWP_BADARG;
    if (src1->bChannels != src2->bChannels)
        return AWP_BADARG;
    if (weight != NULL && weight->sSizeX != src1->sSizeY)
        return AWP_BADARG;


    if (awpCreateImage(&img1, src1->sSizeX, 1, 1, src1->dwType) != AWP_OK)
        return AWP_BADMEMORY;
    if (awpCreateImage(&img2, src1->sSizeX, 1, 1, src1->dwType) != AWP_OK)
    {
        awpReleaseImage(&img1);
        return AWP_BADMEMORY;
    }

    switch(src1->dwType)
    {
       case AWP_BYTE:
        linesize = src1->sSizeX*sizeof(AWPBYTE);
       break;
       case AWP_SHORT:
        linesize = src1->sSizeX*sizeof(AWPSHORT);
       break;
       case AWP_FLOAT:
        linesize = src1->sSizeX*sizeof(AWPFLOAT);
       break;
       case AWP_DOUBLE:
        linesize = src1->sSizeX*sizeof(AWPDOUBLE);
       break;
       default:
       {
            awpReleaseImage(&img1);
            awpReleaseImage(&img2);
            return AWP_BADARG;
       }
    }

    b1 = (AWPBYTE*)src1->pPixels;
    b2 = (AWPBYTE*)src2->pPixels;
    b3 = (AWPBYTE*)img1->pPixels;
    b4 = (AWPBYTE*)img2->pPixels;
    if (weight != NULL)
        w = (AWPDOUBLE*)weight->pPixels;
    *res = 0;

    for (i = 0; i < src1->sSizeY; i++)
    {
        memcpy(b3,b1,linesize);
        memcpy(b4,b2,linesize);
        b1+= linesize;
        b2+= linesize;

        if ((result = awpDistance(img1, img2, options, &value)) != AWP_OK)
        {
            awpReleaseImage(&img1);
            awpReleaseImage(&img2);
            return result;
        }

        *res += w !=NULL ? w[i]*value : value;
    }
    awpReleaseImage(&img1);
    awpReleaseImage(&img2);
    return result;
}
