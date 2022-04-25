#ifndef __AWPIPL_H_
#define __AWPIPL_H_

#if defined _AWPIPL_H_
#error Only one of "awpipl.h" or "_awpipl.h" may be used
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <memory.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef WIN32
	#include <float.h>
#else
	#include <float.h>
	#include <limits.h>
#endif

#include "awpipl.h"
#include "awperror.h"

/*library's internal*/
#include "_awpio.h"
#include "_awpdraw.h"
#include "_awpcolor.h"
#include "_awpdetect.h"


#define AWP_SHRT_MIN      (-32767-1)        /* minimum signed  AWPSHORT value */
#define AWP_SHRT_MAX        32767           /* maximum signed  AWPSHORT value */
#define AWP_FLOAT_MAX FLT_MAX
#define AWP_FLOAT_MIN -FLT_MAX

#ifdef WIN32
	#define AWP_CDECL __cdecl
#else
	#define AWP_CDECL
#endif


/**
\struct tagawpColor
\brief RGB color structure
*/
typedef struct tagawpColor
{
	/** red channel */
	AWPBYTE bRed;
	/** green channel */
	AWPBYTE bGreen;
	/** blue channel */
	AWPBYTE bBlue;
} awpColor;


#define _CHECK_RESULT_(r) if ((r) != AWP_OK) goto CLEANUP;
#define _CHECK_SAME_SIZES(v,u) if (u->sSizeX != v->sSizeX || u->sSizeY != v->sSizeY) _ERROR_EXIT_RES_(AWP_BADARG);
#define _CHECK_SAME_TYPE(v,u) if (u->dwType != v->dwType) _ERROR_EXIT_RES_(AWP_BADARG);
#define _CHECK_NUM_CHANNELS(u,v) if (u->bChannels != v->bChannels)_ERROR_EXIT_RES_(AWP_BADARG);

#define _ERR_EXIT_	goto CLEANUP;
#define _ERROR_EXIT_RES_(v) {res = v; goto CLEANUP;}

#define _SAFE_RELEASE_(img) if (img != NULL) awpReleaseImage(&img);
#define _SAFE_FREE_(v) if (v != NULL) {free(v); v = NULL;}

/*access to a pixel*/
#define _PIXEL(img, x, y, ch, type)\
	((type*)img->pPixels)[y*img->sSizeX*img->bChannels + x*img->bChannels + ch]


/*working with Spots*/
	/**
	\struct awpSpotProperty
	\brief List of properties for awpStrokeObj object
	*/
    typedef struct
    {
		/** Spot's square */
        AWPFLOAT           flSquare;
		/** Spot's perimeter */
        AWPFLOAT           flPerim;
		/** Spot's X-size */
        AWPFLOAT           flLx;
        AWPFLOAT           flLy;
		/** Spot's Y-size */
        awpPoint        Center;
		/** Spot's senter */
        AWPFLOAT           flSapeCoef;
		/** Spot's coef. of shape */
        AWPFLOAT           flMinor;
		/** Spot's ellops minor axis length */
        AWPFLOAT           flMajor;
		/** Spot's ellips major axis length*/
        AWPFLOAT           flTeta;
		/** Spot's ellips orientation */
        AWPDOUBLE		    dM10;
        AWPDOUBLE          dM01;
        AWPDOUBLE          dM11;
        AWPDOUBLE          dM02;
        AWPDOUBLE          dM20;
        AWPDOUBLE          dM30;
        AWPDOUBLE          dM21;
        AWPDOUBLE          dM12;
        AWPDOUBLE          dM03;
		/** Spots bounding rectengle */
        awpRect         Rect;
    }awpSpotProperty;

/*AWPRESULT awpObtainSpotsTableFromImage(awpImage* pImage, AWPINT* pNumSpots, awpSpotProperty** ppProperties, awpStrokeObj** ppStrokes,AWPDOUBLE thr);
AWPRESULT awpFilterSpotsList(awpSpotProperty* pPropListIn, awpStrokeObj* pStrokesListIn,AWPINT iNumSpotsIn, awpSpotProperty** ppPropListOut,awpStrokeObj** ppStrokesListOut,AWPINT* iNumSpotsOut,AWPBOOL (*rule)(awpSpotProperty));*/


#define _8_NEIGHBOURS	0
#define _4_NEIGHBOURS	1

AWPRESULT awpLocalMax(awpImage* pImage, awpImage** ppImage, AWPBYTE thr, AWPINT options);
AWPRESULT awpLocalMin(awpImage* pImage, awpImage** ppImage, AWPBYTE thr, AWPINT options);

#define DELETE_UNKNOWN_MAX	0
#define LEAVE_UNKNOWN_MAX	1

AWPRESULT awpFeatures(awpImage* pImage, awpImage** ppImage, AWPBYTE thr_max, AWPBYTE thr_min, AWPBYTE relative_thr, AWPINT radius, AWPINT MinMax_options, AWPINT options);
AWPRESULT awpFeaturesV2(awpImage* pImage, awpImage** ppImage, AWPBYTE thr_max, AWPBYTE relative_thr, AWPINT radius, AWPINT MinMax_options);

/*Point functions*/
AWPRESULT awpDistancePoints(awpPoint p1, awpPoint p2, AWPDOUBLE* distance);

AWPRESULT awpFastIntegral( awpImage* pSrc, awpImage** ppDst, AWPINT type );


//[ANDX]
//Creating shifted image for fast computing;
//COMMENT: At 0 rotated image added 1 pixel both to width and height;
//At 45 rotated added 1 pixel to height;
AWPRESULT awpShlIntegral( awpImage* pSrc, awpImage** ppDst, AWPINT type );
AWPRESULT awpBilinearBlur( awpImage* pImage, AWPDOUBLE dfStrength );
//The result dispercy 0.5, mean 0.5 -> range = 1;
AWPRESULT awpNormalizeAdaptive( awpImage* pImage,  awpImage const* pAverage, awpImage const* pDispercy );
AWPRESULT awpDispercy( awpImage const* pImage, awpImage const* pAverage, awpImage* pDispercy );
AWPRESULT awpFastCopyImage(const awpImage* pSrcImage, awpImage* pDstImage);
AWPRESULT awpGetChannel2(awpImage* pImage, awpImage* pChannel, AWPBYTE bChannel);
AWPRESULT awpResizePiecewiseCubic(awpImage* pImageSrc, const AWPWORD wNewWidth, const AWPWORD wNewHeight);
AWPRESULT awpResizeBicubic2x2(awpImage* pImageSrc, const AWPWORD wNewWidth, const AWPWORD wNewHeight);
AWPRESULT awpResizeBicubic4x4(awpImage* pImageSrc, const AWPWORD wNewWidth, const AWPWORD wNewHeight);
AWPRESULT awpConvertV2(awpImage* Image, long lOptions);
/**
*	\brief Perform mask convolution on image
*	\param pSrcImage pointer source to the source image
*	\param pMask pointer to the mask
*	\return AWP_OK if success or else AWP_ERROR
*/
AWPRESULT __awpMaskConvolution(awpImage* pSrcImage,awpImage* pMask);
AWPRESULT awpMaskConvolutionDbl(awpImage* pSrcImage,awpImage* pMask, awpImage** pDstImage);
void _awpRGBtoHSV(AWPBYTE rr,AWPBYTE gg,AWPBYTE bb,AWPFLOAT *h, AWPFLOAT *s, AWPFLOAT *l);
/*
the awpIntegralGrid function takes an integral image pImage
output - result image pDst of type AWP_DOUBLE
which contains the values of the intensities in the grid cells,
which are specified by the size of the resulting image and input
rectangle pRect
*/
AWPRESULT awpIntegralGrid(const awpImage* pImage, const awpRect* pRect, awpImage* pDst);
#ifdef __cplusplus
}
#endif

#endif /*__AWPIPL_H_ */

