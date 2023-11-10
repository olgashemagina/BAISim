 /*M
//
//    awpipl2 image processing and image analysis library
//    Purpose: AWPIPL header file
//    Author : Alex A.Telnykh
//    CoAutors: Eugeny Eremin, Andrey Kovalchuck, Youry Yakhno, Olga Shemagina
//    Copyright (c) 2004-2020 NN-Videolab.net
//
//M*/


	/** @defgroup datastructures Graphic data structures 
	*   @{
	*	@}
    */

    /** @defgroup commongroup Common functions
    *   @{
	*	@}
    */

    /** @defgroup filesgroup Input/Output functions
    *   @{
	*	@}
    */

    /** @defgroup geometricgroup Geometry functions
    *   @{
	*	@}
    */

	/** @defgroup editgroup Copy-Paste functions
	*	@{
	*	@}
    */


#ifndef _AWPIPL_H_
#define _AWPIPL_H_


#ifdef __cplusplus
extern "C" {
#endif


//#define HAVE_JPEG

#ifdef HAVE_CONFIG_H
        #include <config.h>
#endif
#include <stdlib.h>
#include "awperror.h"

#ifdef WIN32
    #include <windows.h>
#endif

#ifdef HAVE_FFTW3
	#include <fftw3.h>
#endif

#define AWP_MAGIC2     0x000A0D5D /*magic = AWP*/

#define AWP_VERSION   2

#ifndef NULL
	#define NULL 0 
#endif

#ifndef TRUE
	#define TRUE     -1
#endif

#ifndef FALSE
	#define FALSE     0
#endif

#define AWP_MAX(a,b) (((a) > (b)) ? (a) : (b))
#define AWP_MIN(a,b) (((a) < (b)) ? (a) : (b))

#ifndef AWP_PI
	#define AWP_PI 3.14159265358979323846
#endif

/*pixel data types*/
#define AWP_BYTE				0x01
#define AWP_SHORT				0x02
#define AWP_FLOAT				0x03
#define	AWP_DOUBLE				0x04

#define AWP_MAX_SHORT  			65535


/* convert options */
#define AWP_CONVERT_BASE          	0
#define AWP_CONVERT_TO_BYTE         	(AWP_CONVERT_BASE+0)
#define AWP_CONVERT_TO_BYTE_WITH_NORM   (AWP_CONVERT_BASE+1)
#define AWP_CONVERT_TO_SHORT            (AWP_CONVERT_BASE+2)
#define AWP_CONVERT_TO_FLOAT            (AWP_CONVERT_BASE+3)
#define AWP_CONVERT_TO_DOUBLE           (AWP_CONVERT_BASE+4)
#define AWP_CONVERT_3TO1_BYTE			(AWP_CONVERT_BASE+5)

/*norm options*/

#define AWP_NORM_L1 1
#define AWP_NORM_L2 2
	
/*geometric transformations*/
#define AWP_FLIP_VERT  0x1
#define AWP_FLIP_HRZT  0x2 
#define AWP_FLIP_LEFT  0x3
#define AWP_FLIP_RGHT  0x4

/** @defgroup arifgroup Arithmetic and logical operations
*   @{
*/
/*calculate images*/
#define AWP_CALC_ANDIMAGES  0x00000001
#define AWP_CALC_ORIMAGES   0x00000002
#define AWP_CALC_XORIMAGES  0x00000003
#define AWP_CALC_ADDIMAGES  0x00000004
#define AWP_CALC_SUBIMAGES  0x00000005
#define AWP_CALC_MLTIMAGES  0x00000006
#define AWP_CALC_DIVIMAGES  0x00000007
#define AWP_CALC_MINIMAGES  0x00000008
#define AWP_CALC_MAXIMAGES  0x00000009
#define AWP_CALC_AVGIMAGES  0x0000000A

/*flags of destinations for calculate images functions */
#define AWP_CALC_INPLACE    0x00000001
#define AWP_CALC_OUTPLACE   0x00000002

/** @} */ /* end of arifgroup  */

#define AWP_BINARY	            0x00000000
#define AWP_INV_BINARY          0x00000001


#ifdef HAVE_FFTW3
        #define AWP_FFT_FORWARD FFTW_FORWARD
        #define AWP_FFT_INVERSE FFTW_BACKWARD
#endif


#define AWP_BLUR					0
#define AWP_BLUR_MORE				1
#define AWP_SHARPEN					2
#define AWP_SHARPEN_MORE			3
#define AWP_SHARPEN_EDGES			4
#define AWP_FIND_EDGES				5
#define AWP_FIND_EDGES1				6
#define AWP_EMBOSS_NORTH			7
#define AWP_EMBOSS_NORTH_OST		8
#define AWP_EMBOSS_OST				9
#define AWP_EMBOSS_SOUTH_OST		10
#define AWP_EMBOSS_SOUTH			11
#define AWP_EMBOSS_SOUTH_WEST		12
#define AWP_EMBOSS_WEST				13
#define AWP_EMBOSS_NORTH_WEST		14

#define AWP_PREWITT_V				    15
#define AWP_PREWITT_H					16
#define AWP_SOBEL_V						17
#define AWP_SOBEL_H						18


#define AWP_FILTER_METHOD_ORDINARY      0
#define AWP_FILTER_METHOD_FFT			1
#define AWP_FILTER_METHOD_DOUBLE	    2

#define AWP_FAREST_POINT  0
#define AWP_NEAREST_POINT 1

#define AWP_NULL_DETECTOR 0
#define AWP_GMCT_DETECTOR 456
#define AWP_LMCT_DETECTOR 896
#define AWP_GLBP_DETECTOR 765
#define AWP_LLBP_DETECTOR 987

#define AWP_GMST_WIDTH 517
#define AWP_LMST_WIDTH 514

#define AWP_MCT_NUMBINS 512
#define AWP_LBP_NUMBINS 256

#define AWP_DETECT_GROW 1.41
#define AWP_DETECT_MIN_STEP 10

#define AWP_LINEAR  0
#define AWP_SQUARE  1
#define AWP_RLINEAR	2
#define AWP_RSQUARE	3


#define AWP_DIST_L1     1
#define AWP_DIST_L2     2
#define AWP_DIST_NCC    3
#define AWP_DIST_X2     4
#define AWP_DIST_MSE	5
#define AWP_DIST_RMSE	6


#define AWP_LBP_U1R1 1
#define AWP_LBP_U2R1 2
#define AWP_LBP_U1R2 3
#define AWP_LBP_U2R2 4


/*awpipl data types*/
typedef int               AWPBOOL;
typedef unsigned char     AWPBYTE;
typedef unsigned short    AWPWORD;
typedef short             AWPSHORT;
typedef int				  AWPINT;
typedef long              AWPLONG;
typedef unsigned int      AWPDWORD;
typedef float			  AWPFLOAT;
typedef double            AWPDOUBLE;
typedef void*			  AWPHANDLE;


	/** \addtogroup datastructures
	*   @{
	*/

    /**
    \struct tagawpImage
    \brief Awp image struct
    */
    typedef struct tagawpImage
    {
        /** image idetificator*/
        AWPDWORD		nMagic;
        /** number of channels*/
        AWPBYTE		bChannels;
        /** width image */
        AWPWORD		sSizeX;
        /** height image */
        AWPWORD		sSizeY;
        /** type of pixels */
        AWPDWORD		dwType;
        /** pointer to pixels */
        void*		pPixels;
    } awpImage;

    /**
    \struct tagawpRGBColor
    \brief RGB color structure
    */
    typedef struct tagawpRGBColor
    {
        /** red channel */
        AWPBYTE bRed;
        /** green channel */
        AWPBYTE bGreen;
        /** blue channel */
        AWPBYTE bBlue;
    } awpRGBColor;

    /**
    \struct awpPoint awpipl.h
    \brief Point abstraction
    */
    typedef struct
    {
        /** X coordinate */
		AWPSHORT X;
        /** Y coordinate */
		AWPSHORT Y;

    }awpPoint;

    /**
    \struct awpPoint awpipl.h
    \brief 3DPoint abstraction
    */
    typedef struct
    {
       /** X coordinate */
       AWPDOUBLE X;
       /** Y coordinate */
       AWPDOUBLE Y;
       /** Z coordinate */
       AWPDOUBLE Z;
    }awp3DPoint;
    /**
    \struct awpPoint awpipl.h
    \brief 2DPoint abstraction
    */
    typedef struct
    {
       /** X coordinate */
       AWPDOUBLE X;
       /** Y coordinate */
       AWPDOUBLE Y;
    }awp2DPoint;

    /**
    \struct awpRect awpipl.h
    \brief Rectangle abstraction
    */
    typedef struct
    {
        /** left  coordinare rectangle */
        AWPSHORT left;
        /** top  coordinare rectangle */
        AWPSHORT top;
        /** right coordinare rectangle */
        AWPSHORT right;
        /** bottom coordinare rectangle */
        AWPSHORT bottom;
    }awpRect;

    /**
    \struct awpStroke awpipl.h
    \brief Stroke structure
    */
    typedef struct
    {
        /** begin stroke */
        AWPINT xl;
        /** end stroke */
        AWPINT xr;
        /** Y coordinate stroke */
        AWPINT y;
    }awpStroke;

    /**
    \struct awpStrokeObj awpipl.h
    \brief Stroke object
    */
    typedef struct
    {
        /** count strokes */
        AWPDWORD           Num;
        /** array strokes */
        awpStroke*      strokes;
    }awpStrokeObj;

    /**
    \struct awpContour awpipl.h
    \brief Contain array points of countors
    */
    typedef struct
    {
        /** Count point in countor */
        AWPDWORD	NumPoints;
        /** Direction: TRUE - external,FALSE - internal */
        AWPBOOL	Direction;
        /** Array points */
        awpPoint* Points;
    } awpContour;

	/**
	\struct awp2DContour awpipl.h
	\brief Contain array 2D points of countors
	*/
	typedef struct
	{
		/** Count point in countor */
		AWPDWORD	NumPoints;
		/** Direction: TRUE - external,FALSE - internal */
		AWPBOOL	Direction;
		/** Array points */
		awp2DPoint* Points;
	} awp2DContour;


    /**
    \struct awpQuadrangle awpipl.h
    \brief Describes quadrangle
    */
    typedef struct
    {
		/**size of this structure*/
	    AWPINT		size;
		/**left_top of the quadrangle*/
		awp2DPoint        left_top;
		/**right_top of the quadrangle*/
		awp2DPoint        right_top;
		/**right_bottom of the quadrangle*/
		awp2DPoint        right_bottom;
		/**left_bottom of the quadrangle*/
		awp2DPoint        left_bottom;
    } awpQuadrangle;

	/**
	\struct awpLineSegment awpipl.h
	\brief Describes LineSegment
	*/
	typedef struct
	{
		/*line segment start point*/
		awpPoint strat;
		/*line segment end point*/
		awpPoint end;
	}awpLineSegment;

	/**
	\struct awp2DLineSegment awpipl.h
	\brief Describes LineSegment
	*/
	typedef struct
	{
		/*line segment start point*/
		awp2DPoint strat;
		/*line segment end point*/
		awp2DPoint end;
	}awp2DLineSegment;

    /*camera*/
	/**
	\struct awpCamera
	\describes single camera settings and hardware properties
	*/
	typedef struct
    {
        /*Focal length*/
		AWPDOUBLE  dFocalLength;
		/*Chip width*/
		AWPDOUBLE  dChipWSize;
		/*Chip height*/
		AWPDOUBLE  dChipHSize;
		/*Angle of optical axis over horizontal plane*/
		AWPDOUBLE  dAngle;
		/*Height of camera setup*/
		AWPDOUBLE  dHeight;
		/*Length of visible area. Obsolete*/
		AWPDOUBLE  dMaxLenght;
    }awpCamera;


	/**
//struct awpDetectItem
//brief description of the detected object
*/
	typedef struct
	{
		/** location of in the image*/
		awpRect rect;
		/** flag the presence of the object*/
		AWPBOOL    hasObject;
		/** distance from the camera*/
		AWPINT  Distance;
		/** object's height above the plane*/
		AWPINT  Height;
		/** width of the object*/
		AWPINT  Width;
		/** shift of the object*/
		AWPINT  Shift;
		/**todo: what is overlap means*/
		AWPFLOAT overlap;
	}awpDetectItem;
	/**
	\enum awpScannerType
	\brief Possible types of scanners
	*/
	typedef enum
	{
		NullScanner = 0,
		/** scans the whole image with a fixed step*/
		AllScanner,
		/** scans the whole image with a pitch
		that depends on the scale*/
		ScaleScanner,
		/** scans the image using the parameters of the camera*/
		CameraScanner
	}awpScannerType;
	
	/**
	\struct awpScanner
	\brief description of the scanner
	*/
	typedef struct
	{
		/** type of scanner. see awpScannerType */	
		awpScannerType scannerType;
		/** pointer to awpCamera structure*/
		awpCamera*     camera;
		/**base width. width of minimal rectangular object*/
		AWPSHORT       baseWidth;
		/**base height. height of minimal rectangular object*/
		AWPSHORT       baseHeight;
		/**Depends on type of scanner. see remarks*/
		AWPSHORT       xStep;
		/**Depends on type of scanner. see remarks*/
		AWPSHORT       yStep;
		/**minimal step in pixels. should be more than 1*/
		AWPSHORT       stepGranularity;
		/** step by scale. should be more 1.1*/
		AWPFLOAT       scaleStep;
		/** Defines minimum width and minimum height of deteced object. MinimumWidth =  baseWidth*minScale 
		MinimumHeight= baseHeight*minScale*/
		AWPFLOAT       minScale;
		/** Defines maximum width and maximum height of deteced object. MaximumWidth =  baseWidth*maxScale
		MaximumHeight= baseHeight*maxScale*/
		AWPFLOAT       maxScale;
		/**minimum distance from the cmera in mm.  for CameraScanner only*/
		AWPFLOAT         minDistance;
		/**maximum distance from the cmera in mm.  for CameraScanner only*/
 		AWPFLOAT         maxDistance;
		/**step  in mm between minDistance and maxDistance.  for CameraScanner only*/
		AWPFLOAT         stepDistance;
		/** min X shift from optical axis in mm. for CameraScanner only*/
		AWPFLOAT         minShift;
		/** max X shift from optical axis in mm. for CameraScanner only*/
		AWPFLOAT         maxShift;
		/**step  in mm between minShift and maxShift.  for CameraScanner only*/
		AWPFLOAT         stepShift;
		/**minimum height of scannic are in mm. for CameraScanner only*/
		AWPFLOAT         minHeight;
		/**maximum height of scannic are in mm. for CameraScanner only*/
		AWPFLOAT         maxHeight;
		/**step  in mm between minimum  and maximum height.  for CameraScanner only*/
		AWPFLOAT         stepHeight;
	}awpScanner;
	/** @} */ /*  end of datastructures group */


#define _AWP_BPIX_(v,type) \
           (type*)v->pPixels;

#define _AWP_SAFE_RELEASE_(img) if (img != NULL){ awpReleaseImage(&img); img = NULL;}
#define _AWP_SAFE_FREE_(v) if(v != NULL){free(v); v = NULL;}

    /** \addtogroup commongroup
    *   @{
    */

    /**
    \brief get version awp image library
    \param pVersion version library
    \result AWP_OK if success else AWP_ERROR
    */
    AWPRESULT awpGetVersion(AWPINT* pVersion);

    /**
    \brief allocs memory for image \a ppImage type \a dwType size \a (width,height) with \a channels
    \param ppImage pointer on create image
    \param width width image
    \param height height image
    \param channels channels in image
    \param dwType type create image
    \result AWP_OK if success else AWP_ERROR
    */
    AWPRESULT awpCreateImage(awpImage** ppImage, AWPWORD width, AWPWORD height, AWPBYTE channels, AWPDWORD dwType);
	/**
	\brief allocs memory for sigle channel image \a ppImage type \a dwType size \a (width,height) with \a channels
	\param ppImage pointer on create image
	\param width width image
	\param height height image
	\param channels channels in image
	\param data external buffer with image data
	\result AWP_OK if success else AWP_ERROR
	*/
	AWPRESULT awpCreateGrayImage(awpImage** ppImage, AWPWORD width, AWPWORD height, AWPBYTE channels, AWPBYTE* data);
	/**
	\brief allocs memory for multi-channel image
	\a ppImage type \a dwType size \a (width,height) with \a channels
	\param ppImage pointer on create image
	\param width width image
	\param height height image
	\param channels channels in image
	\param data external buffer with image data
	\result AWP_OK if success else AWP_ERROR
	*/
	AWPRESULT awpCreateMultiImage(awpImage** ppImage, AWPWORD width, AWPWORD height, AWPBYTE channels, AWPBYTE bpp, AWPBYTE* data);


    /**
    \brief Free memory for awpImage
    \param ppImage pointer on image
    \result AWP_OK if success else AWP_ERROR
    */
    AWPRESULT awpReleaseImage(awpImage** ppImage);

    /**
    \brief Check image structure
    \param pImage pointer on image
    \result AWP_OK if success else AWP_ERROR
    */
    AWPRESULT awpCheckImage(const awpImage* pImage);

    /**
    \brief Check rectangle structure
    \param pRect pointer on image
    \result AWP_OK if success else AWP_ERROR
    */
    AWPRESULT awpCheckRect(const awpRect* pRect);

	/**
	*	\brief Calculate size of image pixels array
	*	\param pImage pointer to the source image
	*	\param pSize pointer to returning image size value
	*	\return AWP_OK if success else AWP_ERROR
	*/
    AWPRESULT awpGetImageSize(const awpImage* pImage, AWPINT* pSize);

	/**
	*	\brief Calculate size of image header
	*	\param pImage - pointer to the source image
	*	\param iSize pointer to returning image size value
	*	\return AWP_OK if success else AWP_ERROR
	*/
    AWPRESULT awpGetImageHeaderSize(const awpImage* pImage,AWPINT *iSize);

	/**
	*	\brief Get image pixel type
	*	\param pImage pointer to the source image
	*	\param pType pointer to returning type value
	*	\return AWP_OK if success else AWP_ERROR
	*/
    AWPRESULT awpGetImagePixelType(const awpImage* pImage, AWPINT* pType);

    /**
    \brief Check whether rect lies in image or not
    \param pImage pointer on image
    \param pRect pointer on image
    \result AWP_OK if success else AWP_ERROR
    */
    AWPRESULT awpRectInImage(const awpImage* pImage,const awpRect* pRect);

    /** @} */ /*  end of commongroup */


    /** \addtogroup filesgroup
    *   @{
    */
    /**
	*	\brief Load image from file
	*	\param lpFileName image file path
	*	\param ppImage pointer to awpImage
	*	\return AWP_OK if success else AWP_ERROR
	*/
    AWPRESULT awpLoadImage(const char* lpFileName, awpImage** ppImage);

    /**
	*	\brief Save image to file
	*	\param lpFileName image file path
	*	\param ppImage pointer to awpImage
	*	\return AWP_OK if success else AWP_ERROR
	*/
    AWPRESULT awpSaveImage(const char* lpFileName, awpImage* pImage);

    /**
    *	\brief Pack awpImage in memory buffer
    *	\param pImage image
    *	\param ppData memory buffer
    *	\param length lenght memory buffer in bytes
    *	\return AWP_OK if success or else AWP_ERROR
    */
    AWPRESULT awpImagePack( const awpImage* pImage, AWPBYTE** ppData, AWPINT* length );

    /**
    *	\brief Unpack memory buffer to awpImage
    *	\param pData memory buffer
    *	\param length lenght memory buffer in bytes
    *	\param ppImage image
    *	\return AWP_OK if success or else AWP_ERROR
    */

    AWPRESULT awpImageUnpack( const AWPBYTE* pData, AWPINT length, awpImage** ppImage );

    /** @} */ /* end of filesgroup    */


	/** \addtogroup editgroup
	*	@{
	*/

	/**
	*	\brief Make a copy of awpImage
	*	\param pSrcImage pointer to source image
	*	\param ppDstImage pointer to pointer to destination image
	*	\return AWP_OK if success else AWP_ERROR
	*	\remarks Destination image is creating by function. So *ppDstImage should be NULL
	*/
    AWPRESULT awpCopyImage(const awpImage* pSrcImage, awpImage** ppDstImage);

	/**
	*	\brief Copy rectangle area to a new image
	*	\param pSrcImage pointer to the source image
	*	\param ppDstImage pointer to pointer to resulting image
	*	\param pRect pointer to the rectangle of area to copy
	*	\return AWP_OK if success else AWP_ERROR
	*	\remarks Destination image is creating by function. So *ppDstImage should be NULL
	*/
    AWPRESULT awpCopyRect (const awpImage* pSrcImage, awpImage** ppDstImage, const awpRect* pRect);
		/**
	*	\brief Copy rectangle area to an image
	*	\param pSrcImage pointer to the source image
	*	\param pDstImage pointer to resulting image
	*	\param pRect pointer to the rectangle of area to copy
	*	\return AWP_OK if success else AWP_ERROR
	*/
	AWPRESULT awpCopyRectSize(const awpImage* pSrcImage, awpImage* pDstImage, const awpRect* pRect);
	/**
	*	\brief Paste one image to another
	*	\param pSrcImage source image
	*	\param pDstImage destination image to paste to
	*	\param Pos point of the left top corner of the source image
	*	\return AWP_OK if success else AWP_ERROR
	*	\remarks Destination image is creating by function. So *ppDstImage should be NULL
	*/
    AWPRESULT awpPasteRect(const awpImage* pSrcImage, awpImage* pDstImage,   const awpPoint Pos);

	/**
	*	\brief Copy stroke object to a new image
	*	\param pSrcImage pointer to source image
	*	\param ppDstImage pointer to pointer to result image
	*	\param pStr pointer to a stroke object of the source image
	*	\return AWP_OK if success else AWP_ERROR
	*	\remarks Destination image is creating by function. So *ppDstImage should be NULL
	*/
    AWPRESULT awpCopyImageStr(const awpImage* pSrcImage, awpImage** ppDstImage, const awpStrokeObj* pStr);

    /**
    *	\brief Get image channel from image
    *	\param pImage pointer to the source image
    *	\param ppImageDst pointer to the resulting image
    *	\param bOption number of the channel
    *	\return AWP_OK if success or else AWP_ERROR
	*/
    AWPRESULT awpGetChannel(const awpImage *pImage, awpImage **ppChannel, const AWPBYTE bOptions);
    /**
    *	\brief Replace image channel
	*
	*	Function replaces image channel indicated by bOption to channel from pChannel. pChannel
	*	image should have only one channel.
	*
    *	\param pImage pointer to the image
    *	\param pChannel pointer to the image channel information
    *	\param bOption number of the channel statrs with 0
    *	\return AWP_OK if success or else AWP_ERROR
	*/
    AWPRESULT awpPutChannel(awpImage* Image, const awpImage* pChannel, const AWPBYTE bOptions);

	/** @} */ /* end of editgroup    */


    /** @addtogroup filesgroup
    *   @{
    */

#ifdef	HAVE_JPEG
	/**
	*	\brief Convert awpImage to JPEG format
	*	\param pImage source image
	*	\param ppJpegData output array with JPEG data
	*	\param length length of the output array
	*	\param quality desired JPEG quality
	*	\return AWP_OK if success else AWP_ERROR
	*/
    AWPRESULT awpImageToJpeg( const awpImage* pImage, AWPBYTE** ppJpegData, AWPDWORD* length, AWPBYTE quality );

	/**
	*	\brief Convert JPEG image data to awpImage format
	*	\param pJpegData array containing JPEG data
	*	\param length length of JPEG data array
	*	\param ppImage destination awpImage pointer
	*	\return AWP_OK if success else AWP_ERROR
	*/
    AWPRESULT awpJpegToImage( const AWPBYTE* pJpegData, AWPDWORD length,   awpImage** ppImage);
#endif


#ifdef WIN32
	/**
	*	\brief Convert awpImage to Windows DIB format
	*	\param pImage source image
	*	\param pBMI pointer to BITMAPINFO structure
	*	\param pBMData pointer to DIB pixels
	*	\return AWP_OK if success or else AWP_ERROR
	*/
	AWPRESULT awpImageToDIB(const awpImage* pImage, BITMAPINFO* pBMI, unsigned char** pBMData, AWPBOOL replace);
	/**
	*	\brief Convert Windows DIB to awpImage format
	*	\param pBMI pointer to BITMAPINFO structure
	*	\param pBMData pointer to DIB pixels
	*	\param pImage result image
	*	\return AWP_OK if success or else AWP_ERROR
	*/
	AWPRESULT awpDIBToImage( const BITMAPINFO* pBMI, const void* pBMData, awpImage** pImage );
#endif

#ifdef _OPENCV_
	AWPRESULT awpImageToOpenCv(awpImage* pImage, IplImage** ppImage);
	AWPRESULT awpOpenCvToAwpImage(IplImage* pImage, awpImage** ppImage);
#endif //_OPENCV_
    /** @} */ /* end of filegroup    */


    /** \addtogroup geometricgroup
    *   @{
    */

    /** Basic AWPIPL functions for geometric transformation.
     *  It defines functions for flipping, resizing, rescaling and 
     *  rotation of AWPImage.
    */

	/** \brief find value of bilinear interpolation in the souce image  with subpixel accuracy 
	  * \param awpImage* src - source image 
	  *	\param awp2DPoint* p - point where the value sould be calculated
	  * \param awpImage* dst - interpolated value, should be awpImage type AWP_DOUBLE and size 1x1  
	  *  @return AWP_OK if success or else AWP_ERROR.
	*/
	AWPRESULT awpBilinearInterpolation(awpImage* src, awp2DPoint* p, awpImage* dst);

    /** \brief 180(90)-degree turn function
	 *
     *  @param  awpImage* pImageSrc - AWP Image.
     *  @param  long      lOption   - turn option, should be on of the following:\n\n
     *  AWP_FLIP_VERT   a 180-degree rotation around horizontal axis \n
     *  AWP_FLIP_HRZT   a 180-degree rotation around vertical axis \n
     *  AWP_FLIP_LEFT   90-degree turn left \n
     *  AWP_FLIP_RGHT   90-degree turn right
     *  @return AWP_OK if success or else AWP_ERROR.
	 */
    AWPRESULT awpFlip(awpImage** ppImageSrc, const long lOptions);

    /** \brief Resize image function
     *  @param awpImage* pImageSrc - AWP Image.
     *  @param WORD wNewWidth  - new width of image.
     *  @param WORD wNewHeight - new height of image.
     *  @return AWP_OK if success or else AWP_ERROR.*/
    AWPRESULT awpResize(awpImage* pImageSrc, const AWPWORD wNewWidth, const AWPWORD sNewHeight);
	/** \brief Resize image with bilinear interpolation 
	 * @param awpImage* pImageSrc - source image to be resized 
	 * @param AWPWORD wNewWidth - width of the new image
	 * @param AWPWORD wNewHaight - heaght of the new image 
	 * @return AWP_OK if success of else AWP_ERROR
	*/
	AWPRESULT awpResizeBilinear(awpImage* pImageSrc, const AWPWORD wNewWidth, const AWPWORD wNewHeight);
	/**
	*	\brief Resize image function
	*	\param pImageSrc pointer to the source image and put resized image in the new image 
	*	\param ppDst pointer to pointer to the resulting resized image
	*	\param wNewWidth new width value
	*	\param wNewHeight new height value
	*	\return AWP_OK if success or else AWP_ERROR
	*	\remarks Creates new image, so *ppDst should be NULL
	*/
    AWPRESULT awpResizeNip(awpImage* pImageSrc, awpImage** ppDst, const AWPWORD wNewWidth, const AWPWORD wNewHeight);

    /** \brief Rescale image function
	 *
     *  New image heigh = old image heigh * wHeightScaleFactor;\n
     *  New image width = old image width * wWidthScaleFactor ;
     *  @param awpImage* pImageSrc - AWP Image.
     *  @param WORD wWidthScaleFactor  - width scale factor.
     *  @param WORD wHeightScaleFactor - heigh scale factor.
     *  @return AWP_OK if success or else AWP_ERROR.*/
    AWPRESULT awpRescale(awpImage* pImageSrc, const AWPFLOAT fWidthScaleFactor, const AWPFLOAT fHeightScaleFactor);

    /** \brief Rotate image function
	 * 
     *  @param awpImage* pImageSrc - AWP Image.
     *  @param SHORT sAngle - angle for rotation.
     *  @return AWP_OK if success or else AWP_ERROR.*/
    AWPRESULT awpRotate(awpImage *Image, const AWPSHORT sAngle );

	/** \brief rotate image with bilinear interpolation 
	*
	*  @param awpImage* pImageSrc - AWP Image.
	*  @param SHORT sAngle - angle for rotation.
	*  @return AWP_OK if success or else AWP_ERROR.
	*/
    AWPRESULT awpRotateBilinear(awpImage *pImageSrc, const short sAngle);
   
	/** \brief Rotate image function with sin and cos angle 
    *  @param awpImage* pImageSrc - AWP Image.
    *  @param SHORT sAngle - angle for rotation.
    *  @return AWP_OK if success or else AWP_ERROR.*/
    AWPRESULT awpRotate2(awpImage *Image, const AWPDOUBLE dCos, const AWPDOUBLE dSin );


    /** \brief Rotate image around given point function
     *  @param awpImage* pImageSrc - AWP Image.
     *  @param SHORT sAngle - angle for rotation.
     *  @param SHORT xCenter, SHORT yCenter - point for rotation.
     *  @return AWP_OK if success or else AWP_ERROR.*/
    //TODO: fix bug: then center near one of the side the image is scaling;
    AWPRESULT awpRotateCenter( awpImage *pImageSrc, const AWPSHORT sAngle, AWPSHORT xCenter, AWPSHORT yCenter );

    /** \brief Rotate image around given point function
    *  @param awpImage* pImageSrc - AWP Image.
    *  @param SHORT sAngle - angle for rotation.
    *  @param SHORT xCenter, SHORT yCenter - point for rotation.
    *  @return AWP_OK if success or else AWP_ERROR.*/
    AWPRESULT awpRotateCenter2( awpImage *pImageSrc, const AWPDOUBLE dcos, const AWPDOUBLE dsin, AWPSHORT xCenter, AWPSHORT yCenter );

    /** @} */ /* end of geometricgroup */


    /** @addtogroup arifgroup
    *   @{
    */

    /**
     \brief Arithmetic and logical operations function
	 *
	 *  It sets various image manipulations like adding, substraction, multiplication,
	 *  division, logical and, logical or, logical xor, finding min, max, and average
	 *  image. Image operation sets depending on function parameter lOption, which
	 *  should be set to one of the following options.\n\n
     *
     *  AWP_CALC_ADDIMAGES - adding images function\n
     *  AWP_CALC_SUBIMAGES - substraction images function\n
     *  AWP_CALC_MLTIMAGES - multiplication images function\n
     *  AWP_CALC_DIVIMAGES - division images function\n
     *  AWP_CALC_ANDIMAGES - logical AND images function\n
     *  AWP_CALC_ORIMAGES  - logical OR images function\n
     *  AWP_CALC_XORIMAGES - logical XOR images function\n
     *  AWP_CALC_MINIMAGES - finding minimal image function\n
     *  AWP_CALC_MAXIMAGES - finding maximum image function\n
     *  AWP_CALC_AVGIMAGES - calculate averaged image function\n\n
     *
     *  All functions with the exception of logical AND, logical OR and logical XOR have AWP_DOUBLE as
     *  output awpImage picture type, and any of awpImage type as input one. For logical AND, logical OR and
     *  logical XOR images functions you should use only AWP_BYTE awpImage type as input and output parameters.\n\n
     *  
     *  You can get output image as new image or you can replace second initial image. It dependes on function
     *  parameter lWhere, which should be set to one of the following options.\n\n
     *
     *  AWP_CALC_INPLACE  - replace second initial image\n
     *  AWP_CALC_OUTPLACE - get new image
     *
     *  \param awpImage*  pImageSrc1 - first input AWP Image.
     *  \param awpImage*  pImageSrc1 - first input AWP Image.
     *  \param awpImage** ppImageDst - output AWP Image, if lWhere = AWP_CALC_INPLACE, ppImageDst should be set NULL.
     *  \param const long lOption    - image operation parameter.
     *  \param const long lWhere     - image parameter, replace old image or create new one?  
     *  \return AWP_OK if success or else AWP_ERROR
	 */
    AWPRESULT awpCalcImage(const awpImage* pImageSrc1,  awpImage* pImageSrc2, awpImage** ppImageDst, const long lOptions, const long lWhere);

	/**
	*	\brief  Calculates weighted sum of input image pSrcImage and the 
	*    accumulator acc so that acc becomes a running average of frame sequence 
	*    pAcc(x,y)=(1-alpha) * pAcc(x,y) + alpha * pSrcImage(x,y) 
	*     where alpha regulates update speed (how fast accumulator forgets about previous frames) 
	*	\param const awpImage* pImage pointer to the source image source image should be AWP_DOUBLE 
	*	\param pAcc -accamulator image with same size as source image and should be AWP_DOUBLE 
	*	\param alfa - regulates update factor 
	*	\return AWP_OK if success or else AWP_ERROR
	*/
    AWPRESULT  awpRunningAvg(const awpImage* pSrcImage, awpImage* pAcc, AWPDOUBLE alfa);
    
	/**
	*	\brief  Calculates absolute difference between two images  
	* 	ppDiff(x,y)= abs(pSrcImage1(x,y) - pSrcImage2(x,y)). 
	* 	All the images must have the same data type and the same size
	*	\param const awpImage1* pImage pointer to the first source image 
	*	\param const awpImage2* pImage pointer to the second source image 
	*	\param ppDiff - double pointer to the destination image  
	*	\return AWP_OK if success or else AWP_ERROR
	*/

    AWPRESULT  awpAbsDiff(const awpImage* pSrcImage1, const awpImage* pSrcImage2, awpImage** ppDiff);

	/**
	*	\brief Calculate minimum and maximum light levels
	*	\param const awpImage* pImage pointer to the source image
	*	\param AWPDOUBLE** min pointer to pointer to found minimal value
	*	\param AWPDOUBLE** max pointer to pointer to found maximal value
	*	\return AWP_OK if success or else AWP_ERROR
	*/
    AWPRESULT awpMinMax(const awpImage* pImage, AWPDOUBLE** min, AWPDOUBLE** max);

#ifdef HAVE_FFTW3
    /* fft function */
    /* Dangerous function */
    /*AWPRESULT awpConvFFTCircular(awpImage* pSrcImage,awpImage* pMask,awpImage** pDstImage);*/

	/**
	*	\breif Calculate mask convolution using FFTW3 library
	*	\param awpImage* pSrcImage pointer to the source image
	*	\param awpImage* pMask pointer to the mask
	*	\param awpImage** pDstImage pointer to pointer to the convolution result
	*	\return AWP_OK if success or else AWP_ERROR
	*	\remarks Destination image is creating by function. So *ppDstImage should be NULL
	*/
    AWPRESULT awpConvFFTLinear(awpImage* pSrcImage,awpImage* pMask,awpImage** pDstImage);
    AWPRESULT awpFFT(awpImage** pImage,awpImage** pImageRe,awpImage** pImageIm,AWPINT iFftDir);
#endif

	/**
	*	\brief Perform mask convolution on image
	*	\param pSrcImage pointer to the source image
	*	\param pMask pointer to the mask
	*	\param pDstImage pointer to pointer to the resulting image
	*	\return AWP_OK if success or else AWP_ERROR
	*	\remarks Resulting image is creating by the function. So *ppDstImage should be NULL
	*/
    AWPRESULT awpMaskConvolution(awpImage* pSrcImage, awpImage* pDst, awpImage* pMask);
	/**
	*	\brief Perfom image filtering with predifined masks 
	*	\param awpImage pImage image to be filtered 
	*   \param AWPIMT options to set what type of mask will be used 
	*   possible values 
	*	AWP_BLUR				average mask 3x3					
	*	AWP_BLUR_MORE			average mask 5x5	
	*	AWP_SHARPEN				sharpen mask 				
	*	AWP_SHARPEN_MORE		sharpen more mask 	
	*	AWP_SHARPEN_EDGES		Laplas mask  	
	*	AWP_FIND_EDGES			Laplas differential mask 	
	*	AWP_FIND_EDGES1			Laplas differential mask 	
	*	AWP_EMBOSS_NORTH		North gradient mask 	
	*	AWP_EMBOSS_NORTH_OST	Nothost gradient mask 	
	*	AWP_EMBOSS_OST			Ost gradient mask 	
	*	AWP_EMBOSS_SOUTH_OST	Southost gradietn mask 	
	*	AWP_EMBOSS_SOUTH		South gradient mask 	
	*	AWP_EMBOSS_SOUTH_WEST	Soutthwest gradietn mask	
	*	AWP_EMBOSS_WEST			West gradient mask	
	*	AWP_EMBOSS_NORTH_WEST	Nothwest gradient mask 	
	*	AWP_PREWITT_V			Verical prewitt mask 	
	*	AWP_PREWITT_H			Horizontal prewitt mask 	
	*	AWP_SOBEL_V				Vartical sobel mask	
	*	AWP_SOBEL_H				Horizonral sobel mask 	
	*	\return AWP_OK if success or else AWP_ERROR
	*/
	AWPRESULT awpFilter(awpImage* src, awpImage* dst,  AWPINT options);


	/** \brief Calculates sum of all pixels in the source image and store sum in the pDst image 
	*	pDst should be AWP_DOUBLE image with sizes 1x1 and have same number of channels as pSrc image 
	*
	*  \param awpImaeg pSrc - source image 
	*  \paran awpImage pDct - result image with sum of pSrc 
	*
	*	\return AWP_OK if success or else AWP_ERROR
	*/
	AWPRESULT awpGetSumPix(awpImage* pSrc, awpImage* pDst);
	/**
	*	\brief perform gaussian blur filter over pSrc image and store result in the pDst image
	*	\param awpImage pSrc - source imaege 
	*	\param awpImage pDst - destination image 
	*	\param AWPFLOAT sigma - the sigma of the gaussian
	*	\return AWP_OK if success or else AWP_ERROR
	*/
	AWPRESULT awpGaussianBlur(awpImage* pSrc, awpImage* pDst, AWPFLOAT sigma);

	/**
	*	\brief perform median filter over pSrc image and store result in the pDst image
	*	\param awpImage src - source imaege
	*	\param awpImage dst - destination image
	*	\param AWPBYTE radius - the radius of the filter area
	*	\return AWP_OK if success or else AWP_ERROR
	*/
	AWPRESULT awpMedian(awpImage* src, awpImage* dst, AWPBYTE radius);

	/**
	*	\brief Light level normalization
	*
	*  Finds maximal and minimal illumination levels for supplied image, sets them to 0 and 255
	*  respectively and recalculates all other levels according to the new values.
	*
	*	\param awpImage* pImageSrc pointer to the image
	*	\return AWP_OK if success or else AWP_ERROR
	*/
    AWPRESULT awpAutoLevels(awpImage* pImage);

    /** @} */ /* end of arifgroup  */


    /** @defgroup strokegroup Strokes function
    *   @{
    */

    /**
    *	\brief Find object in image
    *	\param src source image
    *	\param Num number found objects
    *   \param strokes pointer on massiv objects
    *	\param thr threshold binarization
    *	\result AWP_OK if success else AWP_ERROR
    */
    AWPRESULT awpGetStrokes(const awpImage* src, AWPINT* Num, awpStrokeObj** strokes, AWPBYTE thr, awpRect* pRoi);

    /**
    *	\brief Free memory for stroke object 
    *	\param Num number objects
    *	\param strokes pointer on massiv objects
    *	\result AWP_OK if success else AWP_ERROR
    */
    AWPRESULT awpFreeStrokes(AWPINT Num, awpStrokeObj** s);

	/**
	*	\brief Copy one stroke object to another 
	*	\param awpStrokeObj* pSrc - source strolke object 
	*	\param awpStrokeObj* pDst - destination stroke object 
	*	\result AWP_OK if success else AWP_ERROR
	*/
    AWPRESULT awpCopyStrokeObj1(awpStrokeObj* pSrc, awpStrokeObj* pDst);


    /**
    *	\brief Calculate rectangle around objects
    *	\param s stroke objects
    *	\param r rectangle
    *	\result AWP_OK if success else AWP_ERROR
    */
    AWPRESULT awpCalcObjRect(const awpStrokeObj* s, awpRect* r);
 	/**
	*	\brief Returns point within Stroke object with L2 distance
    *   from point src according options
        AWP_FAREST_POINT - returns farest point
        AWP_NEAREST_POINT - returns nearst point
	*	\param s pointer to the source stroke object
	*	\param src  pointer to the source point
	*	\param dst  pointer to the destination point
        \param options options to which point return
	*	\return AWP_OK if success else AWP_ERROR
	*/
    AWPRESULT awpGetObjPoint(const awpStrokeObj* s, awpPoint* src, awpPoint* dst, AWPBYTE options);

	/**
	*	\brief Calculate mean object intensity
	*	\param pImg pointer to the source image
	*	\param pObj pointer to the stroke object
	*	\param I pointer to the resulting intensity value
	*	\return AWP_OK if success else AWP_ERROR
	*/
    AWPRESULT awpGetObjIntensity(const awpImage* pImg, const awpStrokeObj* pObj, AWPDOUBLE* I);

	/**
	*	\brief Calculate Total object intensity
	*	\param pImg pointer to the source image
	*	\param pObj pointer to the stroke object
	*	\param I pointer to the resulting intensity value
	*	\return AWP_OK if success else AWP_ERROR
	*/
    AWPRESULT awpGetObjTotalIntensity(const awpImage* pImg, const awpStrokeObj* pObj, AWPDOUBLE* I);

    /**
    *	\brief Calculate object square
    *	\param s pointer to the strokes object
    *	\param Sq square of the object
    *	\result AWP_OK if success else AWP_ERROR
    */
    AWPRESULT awpStrObjSquare(const awpStrokeObj* s, AWPINT* Sq);

	/**
	*	\brief Build object contour
	*	\param s pointer to the strokes object
	*	\param c countour of the object
    *	\result AWP_OK if success else AWP_ERROR
	*/
    AWPRESULT awpGetObjCountour(const awpStrokeObj* s, awpContour* c);
 	/**
	*	\brief Calculate object centroid
	*	\param pImg pointer to the source image
	*	\param pObj pointer to the strokes object
	*	\param p pointer to the centroid point
	*	\return AWP_OK if success else AWP_ERROR
	*/
    AWPRESULT awpGetObjCentroid(const awpImage* pImg, const awpStrokeObj* pObj, awpPoint* p);

	/**
	*	\brief Calculate p-th and q-th moments for the strokes object
	*	\param pImg pointer to the source image
	*	\param pObj pointer to the strokes object
	*	\param p moment order value
	*	\param q moment order value
	*	\param m moment value
	*	\return AWP_OK if success else AWP_ERROR
	*	\remark Works only with AWP_BYTE type and one channel image
	*/
    AWPRESULT awpGetObjMoment(const awpImage* pImg, const awpStrokeObj* pObj, AWPINT p, AWPINT q, AWPDOUBLE* m);

	/**
	*	\brief Calculate orientation of the object
	*	\param pImg pointer to the source image
	*	\param pObj pointer to the strokes object
	*	\param teta orientation angle
	*	\param mi minor axis lenght
	*	\param ma major axis lenght
	*	\return AWP_OK if success else AWP_ERROR
	*/
    AWPRESULT awpGetObjOrientation(const awpImage* pImg, const awpStrokeObj* pObj, AWPDOUBLE* teta, AWPDOUBLE* mi, AWPDOUBLE* ma);
     /**
    *	\brief Draw stroke objects on image
    *	\param ppimg pointer on image
    *	\param w width image
    *	\param h height image
    *	\param num count strokes objects
    *	\param pobjs pointer on objects
    *	\result AWP_OK if success else AWP_ERROR
    */
    AWPRESULT awpDrawStrokes(awpImage** ppimg, AWPWORD w, AWPWORD h, AWPINT num, awpStrokeObj* pobjs);

	/**
	*	\brief Draw single stroke object
	*	\param pImage pointer to the destination image
	*	\param pObj pointer to the strokes object
	*	\return AWP_OK if success else AWP_ERROR
	*/
    AWPRESULT awpDrawStrokeObj(awpImage* pImage, const awpStrokeObj* pObj, AWPBYTE v);
	/**
	*	\brief Draw single stroke object with specific color valaues 
	*	\param pImage pointer to the destination image
	*	\param pObj pointer to the strokes object
	*   \param AWPBYTE r - r color value 
	*   \param AWPBYTE g - g color value 
	*   \param AWPBYTE b - b color value 
	*	\return AWP_OK if success else AWP_ERROR
	*/
	AWPRESULT awpDrawColorStrokeObj(awpImage* pImage, const awpStrokeObj* pObj, AWPBYTE r, AWPBYTE g, AWPBYTE b);
     /** @} */ /* end of strokegroup  */

	/** @defgroup convertgroup Convertion function
	*	@{
	*/

	/**
	*	\brief Image norm
	*	\param pImage pointer to the source image
	*   \param ppNorm a pointer to the image, which has a norm
	*	\param lOptions calculation option. Can take one of the following values:\n
	*		AWP_NORM_L1\n
	*		AWP_NORM_L2\n
	*	\return AWP_OK if success else AWP_ERROR
	*/

	AWPRESULT awpNorm(awpImage* pSrc, awpImage** ppNorm, long lOptions);

	/**
	*	\brief Image normalize
	*	\param pImage pointer to the source image
	*   \param ppNorm a pointer to the image, which has a normalized data
	*	\param lOptions calculation option. Can take one of the following values:\n
	*		AWP_NORM_L1\n
	*		AWP_NORM_L2\n
	*	\return AWP_OK if success else AWP_ERROR
	*/

    AWPRESULT awpNormalize(awpImage* pSrc, awpImage** ppNorm, long lOptions);

	/**
	*	\brief Image convertion
	*	\param pImage pointer to the source image
	*	\param lOptions convertion option. Can take one of the following values:\n
	*		AWP_CONVERT_TO_AWPAWPDOUBLE\n
	*		AWP_CONVERT_3TO1_BYTE\n
	*		AWP_CONVERT_TO_BYTE\n
	*		AWP_CONVERT_TO_BYTE_WITH_NORM
	*	\return AWP_OK if success else AWP_ERROR
	*/

    AWPRESULT awpConvert(awpImage* pImage,  long lOptions);
 	/**
	*	\brief Image convertion to integral form
	*	\param pSrc pointer to the source image
    *	\param ppDst double pointer to the result image
	*	\param type convertion option. Can take one of the following values:\n
	*		AWP_LINEAR\n
	*		AWP_SQUARE\n
	*		AWP_RLINEAR\n
	*		AWP_RSQUARE\n
	*	\return AWP_OK if success else AWP_ERROR
    *	\comment source image type should be one channel AWP_BYTE
	*/
	AWPRESULT awpIntegral( awpImage* pSrc, awpImage** ppDst, AWPINT type );
 	/**
	*	\brief Image convertion to integral form
	*	\param pSrc pointer to the source image AWP_DOUBLE
    *	\param pDst  pointer to the result image  AWP_DOUBLE
	*	\param type convertion option. Can take one of the following values:\n
	*		AWP_LINEAR\n
	*		AWP_SQUARE\n
	*		AWP_RLINEAR\n
	*		AWP_RSQUARE\n
	*	\return AWP_OK if success else AWP_ERROR
    *	\comment source and destination images should be one channel AWP_DOUBLE
	*/
	AWPRESULT awpIntegral2( awpImage const* pSrc, awpImage* pDst, AWPINT type );

    /**
    *	\brief Bilevels image segmentation with threshold separation
	*	\param pImage pointer to the source image
	*	\param ppBinImage pointer to the pointer to resulting binary image
	*	\param sThreshold binarization threshold value. Should be between 0 and 255.
	*	\param lOptions binarization option. Can take following values:\n
	*		AWP_BINARY\n
    *		AWP_INV_BINARY
	*	\return AWP_OK if success or else AWP_ERROR
	*/
    AWPRESULT awpMakeBinary(const awpImage* pImage, awpImage** ppBinImage, const AWPDOUBLE sThreshold, 
		const long lOptions, AWPDOUBLE MinValue , AWPDOUBLE MaxValue , awpRect* pRect );

    /*color space convertions */
	
	/**
	*	\brief Convert RGB triple to HSV color space 
	*	\remarks 
	*	\return AWP_OK if success or else AWP_ERROR
	*/
	AWPRESULT awpRGBtoHSV(AWPBYTE r, AWPBYTE g, AWPBYTE b, AWPDOUBLE* h, AWPDOUBLE* s, AWPDOUBLE* v);
	/**
	*	\brief Convert RGB triple to HSL color space
	*	\remarks
	*	\return AWP_OK if success or else AWP_ERROR
	*/
	
	AWPRESULT awpRGBtoHSL(AWPBYTE r, AWPBYTE g, AWPBYTE b, AWPDOUBLE* h, AWPDOUBLE* s, AWPDOUBLE* l);
	/**
	*	\brief Convert RGB triple to XYZ color space
	*	\remarks
	*	\return AWP_OK if success or else AWP_ERROR
	*/
	AWPRESULT awpRGBtoXYZ(AWPBYTE r, AWPBYTE g, AWPBYTE b, AWPDOUBLE* x, AWPDOUBLE* y, AWPDOUBLE* z);
	/**
	*	\brief Convert RGB triple to LAB color space
	*	\remarks
	*	\return AWP_OK if success or else AWP_ERROR
	*/
	AWPRESULT awpRGBtoLAB(AWPBYTE r, AWPBYTE g, AWPBYTE b, AWPDOUBLE* l, AWPDOUBLE* a, AWPDOUBLE* bb);
	/**
	*	\brief Convert RGB triple to #webcolors 
	*	\remarks
	*	\return AWP_OK if success or else AWP_ERROR
	*/
	AWPRESULT awpRGBtoWeb(AWPBYTE r, AWPBYTE g, AWPBYTE b, AWPINT* web);
	/**
	*	\brief Convert HSV triple to RGB color space
	*	\remarks
	*	\return AWP_OK if success or else AWP_ERROR
	*/
	AWPRESULT awpHSVtoRGB(AWPDOUBLE h, AWPDOUBLE s, AWPDOUBLE v, AWPBYTE* r, AWPBYTE* g, AWPBYTE* b);
	/**
	*	\brief Convert HSL triple to RGB color space
	*	\remarks
	*	\return AWP_OK if success or else AWP_ERROR
	*/
	AWPRESULT awpHSLtoRGB(AWPDOUBLE h, AWPDOUBLE s, AWPDOUBLE l, AWPBYTE* r, AWPBYTE* g, AWPBYTE* b);
	/**
	*	\brief Convert XYZ triple to RGB color space
	*	\remarks
	*	\return AWP_OK if success or else AWP_ERROR
	*/
	AWPRESULT awpXYZtoRGB(AWPDOUBLE x, AWPDOUBLE y, AWPDOUBLE z, AWPBYTE* r, AWPBYTE* g, AWPBYTE* b);
	/**
	*	\brief Convert LAB triple to RGB color space
	*	\remarks
	*	\return AWP_OK if success or else AWP_ERROR
	*/
	AWPRESULT awpLABtoRGB(AWPDOUBLE l, AWPDOUBLE a, AWPDOUBLE b0, AWPBYTE* r, AWPBYTE* g, AWPBYTE* b);
	/**
	*	\brief Convert  #webcolors to RGB triple
	*	\remarks
	*	\return AWP_OK if success or else AWP_ERROR
	*/
	AWPRESULT awpWebtoRGB(AWPINT web, AWPBYTE* r, AWPBYTE* g, AWPBYTE* b);


	/**
	*	\brief Convert RGB image to HSL format
	*	\param pImage pointer to the source image
	*	\param ppHSL pointer to pointer to the resulting HSL image
	*	\remarks Resulting image is creating by the function, so *ppHSL should be set to NULL
	*	\return AWP_OK if success or else AWP_ERROR
	*/
    AWPRESULT awpRGBtoHSLImage(const awpImage* pImage,awpImage** ppHSL);

	/**
	*	\brief Convert HSL image to RGB format
	*	\param pImage pointer to the source image
	*	\param ppRGB pointer to pointer to the resulting RGB image
	*	\remarks Resulting image is creating by the function, so *ppRGB should be set to NULL
	*	\return AWP_OK if success or else AWP_ERROR
	*/
    AWPRESULT awpHSLtoRGBImage(const awpImage* pImage,awpImage** ppRGB);

	/**
	*	\brief Convert RGB image to HSV format
	*	\param pImage pointer to the source image
	*	\param ppHSV pointer to pointer to the resulting HSV image
	*	\remarks Resulting image is creating by the function, so *ppHSV should be set to NULL
	*	\return AWP_OK if success or else AWP_ERROR
	*/
    AWPRESULT awpRGBtoHSVImage(const awpImage* pImage, awpImage** ppHSV);

	/**
	*	\brief Convert HSV image to RGB format
	*	\param pImage pointer to the source image
	*	\param ppRGB pointer to pointer to the resulting RGB image
	*	\remarks Resulting image is creating by the function, so *ppRGB should be set to NULL
	*	\return AWP_OK if success or else AWP_ERROR
	*/
    AWPRESULT awpHSVtoRGBImage(const awpImage* pImage, awpImage** ppRGB);
	/** @} */ /* end of convertgroup */


	/** @defgroup histogroup Histogram function
	*   @{
	*/

    /*Histogram functions */
	
	AWPRESULT awpGetHst(awpImage* pImage, awpImage** pHst, AWPINT options);


	/**
	*	\brief Calculate mean value of the histogram
	*	\param pHst pointer to the image histogram
	*	\param pMean average value 
	*	\return AWP_OK if success or else AWP_ERROR
	*/
	AWPRESULT awpGetHstMean(awpImage* pHst, awpImage* pMean);

	/**
	*	\brief Calculate standard devation value of the histogram
	*	\param pHst pointer to the image histogram
	*	\param pStdDev standard devation value
	*	\return AWP_OK if success or else AWP_ERROR
	*/
	AWPRESULT awpGetHstStdDev(awpImage* pHst, awpImage* pStdDev);

	/**
	*	\brief Calculate median value of the image
	*	\param pImage pointer to the image
	*	\param pMedian median value
	*	\return AWP_OK if success or else AWP_ERROR
	*/
	AWPRESULT awpGetMedian(awpImage* pImage, awpImage* pMedian);

	/**
	*	\brief Calculate entropy value of the histogram
	*	\param pHst pointer to the histogram
	*	\param pEntropy entropy value
	*	\return AWP_OK if success or else AWP_ERROR
	*/
	AWPRESULT awpGetHstEntropy(awpImage* pHst, awpImage* pEntropy);

    /*2D Histogramm functions */
	/**
	*	\brief Calculate 2D HS histogramm
	*	\param pImage pointer to the source image
	*	\param p2DHist pointer to the resulting histogramm
    *   \param low min threshold for V channel in HSV
    *   \param up max threshold for V channel in HSV
    *   \param needToConvert if true resulting histogramm is in AWPBYTE, else in AWPDOUBLE
	*	\return AWP_OK if success or else AWP_ERROR
	*/

    AWPRESULT awpGet2DHistogramm(awpImage* pImage, awpImage* p2DHist, AWPBYTE low, AWPBYTE up, AWPINT needToConvert);

	/**
	*	\brief Normalization of the light level with histogram equilization
	*	\param PImage pointer to the image
	*	\return AWP_OK if success or else AWP_ERROR
	*/
    AWPRESULT awpHistogrammEqualize(awpImage* PImage);
	/**
	*	\brief Finds probability image by using Back Projection algorythm 
	*	\param Image pointer to the image
	*	\param ppProb result image 
	*	\param pPreset 2DHistogramm 
	*	\param min_v - minimum value in the preset to use for projection 
	*	\param max_v - maximum value in the preset to use for projection 
	*	\return AWP_OK if success or else AWP_ERROR
	*/	
	AWPRESULT awpBackProjection2D(awpImage* Image, awpImage** ppProb, awpImage* pPreset, AWPBYTE min_v, AWPBYTE max_v);
    /** @}	end  histogroup */

	/** @defgroup momemtsgroup Moments related functions
	*   @{
	*/
	
	/**
	*	\brief finds centroid of awpImage
	*	\param Image pointer to the image
	*	\param p    result centroid location 
	*	\return AWP_OK if success or else AWP_ERROR
	*/
	AWPRESULT awpGetCentroid(const awpImage* Image, awpPoint* p);
	/**
	*	\brief finds ellipse orientation 
	*	\param Image pointer to the image
	*	\param teta    angle of ellipse axis 
	*	\param mi      length of minor axis 
	*	\param ma      length of major axis 
	*	\return AWP_OK if success or else AWP_ERROR
	*/
	AWPRESULT awpGetOrientation(const awpImage* Image, AWPDOUBLE* teta, AWPDOUBLE* mi, AWPDOUBLE* ma);

	/** @}	end  momemtsgroup */

    /** @defgroup contourgroup Contour functions
    *   @{
    */

	/**
	*	\brief Create contour object
	*	\param ppContour pointer to pointer to destination object
	*	\param NumPoints number of contour points
	*	\param dir contour direction
	*	\return AWP_OK if success or else AWP_ERROR
	*/
    AWPRESULT awpCreateContour(awpContour** ppContour, AWPINT NumPoints, AWPBOOL dir);

	/**
	*	\brief Delete contour object
	*	\param ppContour pointer to pointer to the contour object
	*	\return AWP_OK if success or else AWP_ERROR
	*/
    AWPRESULT awpFreeContour(awpContour** ppContour);

	/**
	*	\brief Calculate circumscribed rectangle for contour
	*	\param pContour pointer to the contour object
	*	\param rect pointer to the circumscribed rectangle
	*	\return AWP_OK if success or else AWP_ERROR
	*/
    AWPRESULT awpGetContourRect(const awpContour* pContour, awpRect* rect);

	/**
	*	\brief Check whether point belongs to the contour area
	*	\param pContour pointer to the contour object
	*	\param p pointer to the point
	*	\param result pointer to the check result
	*	\return AWP_OK if success or else AWP_ERROR
	*/
    AWPRESULT awpIsPointInContour(const awpContour* pContour, const awpPoint* p, AWPBOOL* result);
	/**
	*	\brief Calculate perimeter of the contour
	*	\param c pointer to the contour
	*	\param Perim pointer to the perimeter value
	*	\result AWP_OK if success else AWP_ERROR
	*/
	AWPRESULT awpGetContPerim(const awpContour* c, AWPDOUBLE* Perim);

	/**
	*	\brief Add new point to the end of contour
	*	\param c pointer to the contour
	*	\param point pointer to the new point to be added
	*	\result AWP_OK if success else AWP_ERROR
	*/
	AWPRESULT awpContAddPoint(awpContour* c, awpPoint* point);

	/**
	*	\brief Remove point from the contour
	*	\param c pointer to the contour
	*	\param idx  the index of point withing contour to be removed
	*	\result AWP_OK if success else AWP_ERROR
	*/
	AWPRESULT awpContRemovePoint(awpContour* c, AWPDWORD idx);

	/**
	*	\brief Create contour object
	*	\param ppContour pointer to pointer to destination object
	*	\param NumPoints number of contour points
	*	\param dir contour direction
	*	\return AWP_OK if success or else AWP_ERROR
	*/
	AWPRESULT awpCreate2DContour(awp2DContour** ppContour, AWPINT NumPoints, AWPBOOL dir);

	/**
	*	\brief Delete contour object
	*	\param ppContour pointer to pointer to the contour object
	*	\return AWP_OK if success or else AWP_ERROR
	*/
	AWPRESULT awpFree2DContour(awp2DContour** ppContour);

	/**
	*	\brief Calculate circumscribed rectangle for contour
	*	\param pContour pointer to the contour object
	*	\param rect pointer to the circumscribed rectangle
	*	\return AWP_OK if success or else AWP_ERROR
	*/
	AWPRESULT awpGet2DContourRect(const awp2DContour* pContour, awpQuadrangle* rect);

	/**
	*	\brief Check whether point belongs to the contour area
	*	\param pContour pointer to the contour object
	*	\param p pointer to the point
	*	\param result pointer to the check result
	*	\return AWP_OK if success or else AWP_ERROR
	*/
	AWPRESULT awpIsPointIn2DContour(const awp2DContour* pContour, const awp2DPoint* p, AWPBOOL* result);


	/**
	*	\brief Calculate perimeter of the contour
	*	\param c pointer to the contour
	*	\param Perim pointer to the perimeter value
	*	\result AWP_OK if success else AWP_ERROR
	*/
	AWPRESULT awpGet2DContPerim(const awp2DContour* c, AWPDOUBLE* Perim);

	/**
	*	\brief Add new point to the end of contour
	*	\param c pointer to the contour
	*	\param point pointer to the new point to be added
	*	\result AWP_OK if success else AWP_ERROR
	*/
	AWPRESULT awp2DContAddPoint(awp2DContour* c, awp2DPoint* point);

	/**
	*	\brief Remove point from the contour
	*	\param c pointer to the contour
	*	\param idx  the index of point withing contour to be removed
	*	\result AWP_OK if success else AWP_ERROR
	*/
	AWPRESULT awp2DContRemovePoint(awp2DContour* c, AWPDWORD idx);
     /** @} */ /* end of contourgroup */


	/** @defgroup edgegroup Edge functions
	*   @{
	*/
    /**
	*	\brief Perform Sobel operator on image
	*	\param pImage pointer to the source image
	*	\param pGradAmpl pointer to the resulting gradient image
	*   \param pGradDir pointer to gradient direction image. To supress gardient dir calculation
	*	\      set pGradDir to NULL;
	*	\return AWP_OK if success or else AWP_ERROR
	*	\remarks Resulting image is creating by the function. So *ppPhase should be NULL
	*/
	AWPRESULT awpEdgeSobel(awpImage* pImage, awpImage* pGradAmpl, awpImage* pGradDir);

	/**
	*	\brief Perform Sobel operator on image
	*	\param pImage pointer to the source image. Source image should be one channel AWP_DOUBLE type
	*	\param pVImage pointer to the resulting vertical gradient image. pVImage should be one channel AWP_DOUBLE image, same size as pImage
	*   \param pHImage pointer to gradient direction image. pHImage should be one channel AWP_DOUBLE image, same size as pImage
	*/
	AWPRESULT awpEdgeSobel1(awpImage* pImage, awpImage* pVImage, awpImage* pHImage);

    /** @} */ /* end of Edge functions */


    /** @defgroup drawinggroup Drawing functions
    *   @{
    */

	/**
	*	\brief Solid fill
	*	\param pImage pointer to the source image
	*	\param pColor pointer to the fill color
	*	\return AWP_OK if success else AWP_ERROR
	*/
	AWPRESULT awpFill(awpImage *pImage, AWPDOUBLE dValue);

	/**
	*	\brief Solid fill rect 
	*	\param pImage pointer to the source image
	*	\param pColor pointer to the fill color
	*	\return AWP_OK if success else AWP_ERROR
	*/
	AWPRESULT awpFillRect(awpImage *pImage, awpRect* rect, AWPBYTE bChan, AWPDOUBLE dValue);


	/**
	*	\brief Draw point
	*	\param pImage pointer to the image
	*	\param p point coordinates
	*	\param bChan image channel
	*	\param dValue pixel value
	*	\return AWP_OK if	 success else AWP_ERROR
	*/
    AWPRESULT awpDrawPoint(awpImage* pImage, awpPoint p, AWPBYTE bChan, AWPDOUBLE dValue, AWPBYTE radius);

	/**
	*	\brief Draw line
	*	\param pImage pointer to the image
	*	\param p1 start point of the line
	*	\param p2 finish point of the line
	*	\param bChan image channel
	*	\param dValue pixels value
	*	\return AWP_OK if success else AWP_ERROR
	*/
    AWPRESULT awpDrawLine(awpImage* pImage, awpPoint p1, awpPoint p2, AWPBYTE bChan, AWPDOUBLE dValue, AWPBYTE radius);

	/**
	*	\brief Draw rectangle
	*	\param pImage pointer to the image
	*	\param pRect rectangle to draw
	*	\param bChan image channel
	*	\param dValue pixels value
	*	\return AWP_OK if success else AWP_ERROR
	*/
    AWPRESULT awpDrawRect(awpImage* pImage, awpRect* pRect, AWPBYTE bChan, AWPDOUBLE dValue, AWPBYTE radius);

	/**
	*	\brief Draw cross
	*	\param pImage pointer to the image
	*	\param pRect cross rectangle
	*	\param bChan image channel
	*	\param dValue pixels value
	*	\return AWP_OK if success else AWP_ERROR
	*/
    AWPRESULT awpDrawCross(awpImage* pImage, awpRect* pRect, AWPBYTE bChan, AWPDOUBLE dValue, AWPBYTE radius);

	/**
	*	\brief Draw ellipse
	*	\param pImage pointer to the image
	*	\param center center of the ellipce
	*	\param width  width of the bounding rectangle
	*	\param height height of the bounding rectangle
	*   \param angle angle of the ellipse
	*	\param bChan image channel
	*	\param dValue pixels value
	*	\return AWP_OK if success else AWP_ERROR
	*/
    AWPRESULT awpDrawEllipse(awpImage* pImage, awpPoint center, AWPWORD width, AWPWORD height, AWPDOUBLE angle, AWPBYTE bChan, AWPDOUBLE dValue, AWPBYTE radius);

	/**
	*	\brief Draw ellipse 2
	*	\param pImage pointer to the image
	*	\param center center of the ellipce
	*	\param major  lenght of major axis
	*	\param minor  lenght of minor axis
	*   \param angle  angle of major axis counterclockwise with respect to the colum axis
	*	\param bChan image channel
	*	\param dValue pixels value
	*	\return AWP_OK if success else AWP_ERROR
	*/
    AWPRESULT awpDrawEllipse2(awpImage* pImage, awpPoint center, AWPWORD major, AWPWORD minor, AWPDOUBLE angle, AWPBYTE bChan, AWPDOUBLE dValue, AWPBYTE radius);

	/**
	*	\brief Draw Ellipse Cross
	*	\param pImage pointer to the image
	*	\param center center of the ellipce
	*	\param major  lenght of major axis
	*	\param minor  lenght of minor axis
	*   \param angle  angle of major axis counterclockwise with respect to the colum axis
	*	\param bChan image channel
	*	\param dValue pixels value
	*	\return AWP_OK if success else AWP_ERROR
	*/
    AWPRESULT awpDrawEllipseCross(awpImage* pImage, awpPoint center, AWPWORD major, AWPWORD minor, AWPDOUBLE angle, AWPBYTE bChan, AWPDOUBLE dValue, AWPBYTE radius);


	/**
	*	\brief Draw polygon 
	*	\param pImage pointer to the image
	*   \param pContour points of the polygon 
	*	\param bChan image channel
	*	\param dValue pixels value
	*	\return AWP_OK if success else AWP_ERROR
	*/
	AWPRESULT awpDrawPolygon(awpImage* pImage, awpContour* pContour, AWPBYTE bChan, AWPDOUBLE dValue, AWPBYTE radius);
	/**
	*	\brief Fill polygon 
	*	\param pImage pointer to the image
	*   \param pContour points of the polygon 
	*	\param bChan image channel
	*	\param dValue pixels value
	*	\return AWP_OK if success else AWP_ERROR
	*/
    AWPRESULT awpFillPolygon(awpImage* pImage, awpContour* pContour, AWPBYTE bChan, AWPDOUBLE dValue, AWPDOUBLE dFillValue);

	/**
	*	\brief Fill polygon with color
	*	\param pImage pointer to the image
	*   \param c points of the polygon 
	*	\param r red channel value
	*	\param g green channel value
	*	\param b blue channel value
	*/
	#define awpFillCPolygon(pImage, c,  r, g, b)\
	{\
	awpFillPolygon(pImage, c, 0, r);\
	awpFillPolygon(pImage, c, 1, g);\
	awpFillPolygon(pImage, c, 2, b);\
	}\


	/**
	*	\brief Draw color point with radius
	*	\param pImage pointer to the image
	*	\param p point coordinates
	*	\param r red channel value
	*	\param g green channel value
	*	\param b blue channel value
	*	\param rd radius
	*/
	#define awpDrawCPoint(pImage, p, r, g, b, rd)\
	{\
	awpDrawPoint(pImage, p, 0, r,rd);\
	awpDrawPoint(pImage, p, 1, g,rd);\
	awpDrawPoint(pImage, p, 2, b,rd);\
	}\

	/**
	*	\brief Draw color line with thickness
	*	\param pImage pointer to the image
	*	\param p1 line start point
	*	\param p2 line end point
	*	\param r red channel value
	*	\param g green channel value
	*	\param b blue channel value
	*	\param rd radius
	*/
	#define awpDrawCLine(pImage, p1, p2, r, g, b, rd)\
	{\
	awpDrawLine(pImage, p1, p2, 0, r,rd);\
	awpDrawLine(pImage, p1, p2, 1, g,rd);\
	awpDrawLine(pImage, p1, p2, 2, b,rd);\
	}\
	/**
	*	\brief Draw color rectangle with thick lines 
	*	\param pImage pointer to the image
	*	\param pRect rectangle to draw
	*	\param r red channel value
	*	\param g green channel value
	*	\param b blue channel value
	*	\param rd radius
	*/
	#define awpDrawCRect(pImage, pRect, r, g, b, rd)\
	{\
	awpDrawRect(pImage, pRect, 0, r, rd);\
	awpDrawRect(pImage, pRect, 1, g, rd);\
	awpDrawRect(pImage, pRect, 2, b, rd);\
	}\


	/**
	*	\brief Draw color cross with thick lines 
	*	\param pImage pointer to the image
	*	\param pRect cross rectangle to draw
	*	\param r red channel value
	*	\param g green channel value
	*	\param b blue channel value
	*	\param rd radius
	*/
	#define awpDrawCCross(pImage, pRect, r, g, b, rd)\
	{\
	awpDrawCross(pImage, pRect, 0, r, rd);\
	awpDrawCross(pImage, pRect, 1, g, rd);\
	awpDrawCross(pImage, pRect, 2, b, rd);\
	}\

	/**
	*	\brief Draw color ellipce with thick border
	*	\param pImage pointer to the image
	*	\param p center of the ellipce
	*	\param w width of the bounding rectangle 
	*	\param h height of the bounding rectangle
	*	\param a angle of the ellipse
	*	\param r red channel value
	*	\param g green channel value
	*	\param b blue channel value
	*	\param rd radius
	*/
	#define awpDrawCEllipse2(pImage, p,ma,mi,a, r, g, b, rd)\
	{\
	awpDrawEllipse2(pImage, p, ma,mi, a, 0, r, rd);\
	awpDrawEllipse2(pImage, p, ma,mi, a, 1, g, rd);\
	awpDrawEllipse2(pImage, p, ma,mi, a, 2, b, rd);\
	}\

	/**
	*	\brief Draw color ellipce with thick border
	*	\param pImage pointer to the image
	*	\param p center of the ellipce
	*	\param w width of the bounding rectangle
	*	\param h height of the bounding rectangle
	*	\param a angle of the ellipse
	*	\param r red channel value
	*	\param g green channel value
	*	\param b blue channel value
	*	\param rd radius
	*/
	#define awpDrawCEllipse(pImage, p,w,h,a, r, g, b, rd)\
	{\
	awpDrawEllipse(pImage, p, w, h, a, 0, r, rd);\
	awpDrawEllipse(pImage, p, w, h, a, 1, g, rd);\
	awpDrawEllipse(pImage, p, w, h, a, 2, b, rd);\
	}\

	/**
	*	\brief Draw color ellipce cross with thick border
	*	\param pImage pointer to the image
	*	\param p center of the ellipce
	*	\param ma lenght of major axis
	*	\param mi lenght of minor axis
	*	\param a angle of the ellipse
	*	\param r red channel value
	*	\param g green channel value
	*	\param b blue channel value
	*	\param rd radius
	*/
	#define awpDrawCEllipseCross(pImage, p,ma,mi,a, r, g, b, rd)\
	{\
	awpDrawEllipseCross(pImage, p, ma, mi, a, 0, r, rd);\
	awpDrawEllipseCross(pImage, p, ma, mi, a, 1, g, rd);\
	awpDrawEllipseCross(pImage, p, ma, mi, a, 2, b, rd);\
	}\

	/**
	*	\brief Draw color polygon with thick border 
	*	\param pImage pointer to the image
	*   \param c points of the polygon 
	*	\param r red channel value
	*	\param g green channel value
	*	\param b blue channel value
	*	\param rd radius
	*/
	#define awpDrawCPolygon(pImage, c,  r, g, b, rd)\
	{\
	awpDrawPolygon(pImage, c, 0, r, rd);\
	awpDrawPolygon(pImage, c, 1, g, rd);\
	awpDrawPolygon(pImage, c, 2, b, rd);\
	}\

	#define awpZeroImage(pImage)\
	{\
		awpFill(pImage, 0);\
	}\

/** @}*/ /* end of drawinggroup  */


/** @defgroup distancegroup Distances between image functions
*   @{
*/
	/**
	*	\brief takes distance between two one channel images
	*	\param src1 pointer to the source image  1
	*	\param src2 pointer to the source image  2
	*	\param options option type of ditance operation. Should be one of the following:\n
	*		AWP_DIST_L1 - L1 distance\n
	*		AWP_DIST_L2 - L2 distance\n
	*		AWP_DIST_NCC - Normal cross-correlation\n
    *   	AWP_DIST_X2 - X2 distance\n
	*		AWP_DIST_MSE - mean square error\n
	*		AWP_DIST_RMSE - root mean square error\n
    *	\param res - distanse between src1 and src2
	*	\return AWP_OK if success or else AWP_ERROR
	*/

AWPRESULT awpDistance(awpImage* src1, awpImage* src2, AWPBYTE options, AWPDOUBLE* res);
	/**
	*	\brief takes grid distance between two one channel images
    *	\calculates the distance between two images divided by
    *	\N fragments. Each fragment of the image is represented by a row of pixels in
    *	\variables src1 and src2
    *	\weight is the weight of each fragment. If weight == NULL, it is not considered
	*	\param src1 pointer to the source image  1
	*	\param src2 pointer to the source image  2
	*	\param options option type of ditance operation. Should be one of the following:\n
	*		AWP_DIST_L1 - L1 distance\n
	*		AWP_DIST_L2 - L2 distance\n
	*		AWP_DIST_NCC - Normal cross-correlation\n
    *   	AWP_DIST_X2 - X2 distance\n
    *	\param res - distanse between src1 and src2
	*	\return AWP_OK if success or else AWP_ERROR
	*/
AWPRESULT awpGridDistance(awpImage* src1, awpImage* src2, awpImage* weight, AWPBYTE options, AWPDOUBLE* res);


AWPRESULT awpDistancePoints(awpPoint p1, awpPoint p2, AWPDOUBLE* distance);
/** @} */ /* end of distancegroup  */

/** @defgroup cameragroup Single camera geometry functions
*   @{
*/
/**
*	\brief Calc horizontal view angle of the awpCamera 
*	\param [in] awpCamera* pCamera pointer to awpCamera structure 
*   \param [out] AWPDOUBLE Value of the horizontal view angle of the camera in radians 
*	\return AWP_OK if success or else AWP_ERROR
*/
AWPRESULT awpCameraHorizontalView(awpCamera* pCamera, AWPDOUBLE* Value);
/**
*	\brief Calc vertical view angle of the awpCamera
*	\param [in] awpCamera* pCamera pointer to awpCamera structure
*   \param [out] AWPDOUBLE Value of the vertical view angle of the camera in radians
*	\return AWP_OK if success or else AWP_ERROR
*/
AWPRESULT awpCameraVirticalView(awpCamera* pCamera, AWPDOUBLE* Value);
/**
*	\brief Calc length of invisible zone for given camera settings
*	\param [in] awpCamera* pCamera pointer to awpCamera structure
*   \param [out] AWPDOUBLE Value of the length of invisible zone in mm
*	\return AWP_OK if success or else AWP_ERROR
*/
AWPRESULT awpLengthToFirstVisiblePoint(awpCamera* pCamera, AWPDOUBLE* Value);
/**
*	\brief Calc distance in mm to some Y point on the image taken with awpCamera 
*	\param [in] awpCamera* pCamera pointer to awpCamera structure
*	\param [in] awpImage*  pImage pointer to awpImage  structure wich taken with awpCamera 
*	\param [in] AWPWORD    Y coordinate on the image to calc distance to
*   \param [out] AWPDOUBLE Value of the distance in mm
*	\return AWP_OK if success or else AWP_ERROR
*/
AWPRESULT awpImageYToLenght(awpCamera* pCamera, awpImage* pImage, AWPWORD Y, AWPDOUBLE* Value);
/**
*	\brief Calc Y coordinate on the image taken with awpCamera some point on Ground Plane 
*	\param [in] awpCamera* pCamera pointer to awpCamera structure
*	\param [in] awpImage*  pImage pointer to awpImage  structure wich taken with awpCamera
*	\param [in] AWPDOUBLE  Z coordinate on the Ground Plane 
*   \param [out] AWPDOUBLE Value of Y coordimate in the pixels
*	\return AWP_OK if success or else AWP_ERROR
*/
AWPRESULT awpSceneZToImageY(awpCamera* pCamera, awpImage* pImage, AWPDOUBLE Z, AWPDOUBLE* Value);
/**
*	\brief Calc X shift in mm from optical axis on the ground plane for given image point
*	\param [in] awpCamera* pCamera pointer to awpCamera structure
*	\param [in] awpImage*  pImage pointer to awpImage  structure wich taken with awpCamera
*	\param [in] awpPoint*  pPoint coordinate on image in pixels
*   \param [out] AWPDOUBLE Value of X shift in mm
*	\return AWP_OK if success or else AWP_ERROR
*/
AWPRESULT awpImagePointToShiftX(awpCamera* pCamera, awpImage* pImage, awpPoint* pPoint, AWPDOUBLE* Value);
/**
*	\brief Calc X shift in mm from optical axis on the plane over ground plane on H mm for given image point
*	\param [in] awpCamera* pCamera pointer to awpCamera structure
*	\param [in] awpImage*  pImage pointer to awpImage  structure wich taken with awpCamera
*	\param [in] awpPoint*  pPoint coordinate on image in pixels
*	\param [in] AWPWORD    H height of the palne over Ground Plane in mm
*   \param [out] AWPDOUBLE Value of X shift in mm
*	\return AWP_OK if success or else AWP_ERROR
*/
AWPRESULT awpImagePointToShiftHX(awpCamera* pCamera, awpImage* pImage, awpPoint* pPoint, AWPWORD H, AWPDOUBLE* Value);
/**
*	\brief Calc distance on ground plane from camera to given image point 
*	\param [in] awpCamera* pCamera pointer to awpCamera structure
*	\param [in] awpImage*  pImage pointer to awpImage  structure wich taken with awpCamera
*	\param [in] awpPoint*  pPoint coordinate on image in pixels
*   \param [out] AWPDOUBLE Value of distance in mm
*	\return AWP_OK if success or else AWP_ERROR
*/
AWPRESULT awpImagePointToDistance(awpCamera* pCamera, awpImage* pImage, awpPoint* pPoint, AWPDOUBLE* Value);
/**
*	\brief Calc distance to point on plane over Ground Plane  
*	\param [in] awpCamera* pCamera pointer to awpCamera structure
*	\param [in] awpImage*  pImage pointer to awpImage  structure wich taken with awpCamera
*	\param [in] AWPWORD Y - y coordinat of the image point in pixels 
*	\param [in] AWPWORD H - height of plane over Ground Plane 
*   \param [out] AWPDOUBLE Value of distance in mm
*	\return AWP_OK if success or else AWP_ERROR
*/
AWPRESULT awpImageYHToLength(awpCamera* pCamera, awpImage* pImage, AWPWORD Y, AWPWORD H, AWPDOUBLE* Value);
/**
*	\brief Calc width of the object on the scene Ground Plane given by rect in the image
*	\param [in] awpCamera* pCamera pointer to awpCamera structure
*	\param [in] awpImage*  pImage pointer to awpImage  structure wich taken with awpCamera
*	\param [in] awpRect* pObject rectangle im the image 
*   \param [out] AWPDOUBLE Value of distance in mm
*	\return AWP_OK if success or else AWP_ERROR
*/
AWPRESULT awpImageObjectWidth(awpCamera* pCamera, awpImage* pImage, awpRect* pObject, AWPDOUBLE* Value);
/**
*	\brief Calc width of the object H height over Ground Plane on the scene given by rect in the image
*	\param [in] awpCamera* pCamera pointer to awpCamera structure
*	\param [in] awpImage*  pImage pointer to awpImage  structure wich taken with awpCamera
*	\param [in] awpRect* pObject rectangle im the image
*	\param [in] AWPWORD H - height of plane over Ground Plane
*   \param [out] AWPDOUBLE Value of distance in mm
*	\return AWP_OK
+-if success or else AWP_ERROR
*/ 
AWPRESULT awpImageObjectHWidth(awpCamera* pCamera, awpImage* pImage, awpRect* pObject, AWPWORD H, AWPDOUBLE* Value);
/**
*	\brief Calc height of  of the object on Ground Plane on the scene given by rect in the image
*	\param [in] awpCamera* pCamera pointer to awpCamera structure
*	\param [in] awpImage*  pImage pointer to awpImage  structure wich taken with awpCamera
*	\param [in] awpRect* pObject rectangle im the image
*   \param [out] AWPDOUBLE Value of height in mm
*	\return AWP_OK if success or else AWP_ERROR
*/
AWPRESULT awpImageObjectHeight(awpCamera* pCamera, awpImage* pImage, awpRect* pObject, AWPDOUBLE* Value);
/**
*	\brief Calc position of 3D point on the scene within image 
*	\param [in] awpCamera* pCamera pointer to awpCamera structure
*	\param [in] awpImage*  pImage pointer to awpImage  structure wich taken with awpCamera
*	\param [in] awp3DPoint* pSrc - 3D point on the scene
*   \param [out] awp2DPoint* pDst - 2D point on the image 
*	\return AWP_OK if success or else AWP_ERROR
*/
AWPRESULT awpSceneToImagePoint(awpCamera* pCamera, awpImage* pImage, awp3DPoint* pSrc, awp2DPoint* pDst);
/**
*	\brief Calc position in the scene on H height  over Ground Plane of 2D image point 
*	\param [in] awpCamera* pCamera pointer to awpCamera structure
*	\param [in] awpImage*  pImage pointer to awpImage  structure wich taken with awpCamera
*	\param [in] awp3DPoint* pSrc - 3D point on the scene
*   \param [out] awp2DPoint* pDst - 2D point on the image
*	\return AWP_OK if success or else AWP_ERROR
*/
AWPRESULT awpImageToScenePoint(awpCamera* pCamera, awpImage* pImage, AWPDOUBLE Height, awp2DPoint* pSrc, awp3DPoint* pDst);
/** @} */ /* end of cameragroup  */


/** @defgroup detectorgroup Object detection related functions
*   @{
*/

AWPRESULT awpScanImage(awpImage* pImage, awpRect* pRoi, awpScanner* pScanner, AWPINT* num, awpDetectItem** ppItems);
AWPRESULT awpExteractFound(AWPINT num, awpDetectItem* pItems, AWPINT* numFound, awpDetectItem** ppFound);
AWPRESULT awpClusterFound(AWPINT numFound, awpDetectItem* pFound, AWPINT* numClusters, awpDetectItem** ppClusters);

/**
* @brief Load object detector from file
* \param [in] lpFileName - name of the file where the detector model is located
* \param [out] detector - pointer to the detector object 
* \return AWP_OK if success or else AWP_ERROR
*/
AWPRESULT awpLoadDetector(const char* lpFileName, AWPHANDLE* detector);
/**
*@brief Free object detector 
*\param detector - pointer to the detector object
*\return AWP_OK if success or else AWP_ERROR
*/
AWPRESULT awpFreeDetector(AWPHANDLE* detector);
/**
*@brief Ggeneral object detection function. This function does not use a second integral image
*		to estimate the variance of the input data
* \param [in] awpImage - source integral image
* \param [in] pRoi		- the area is interesting where the search for objects should be performed. NULL means that the search must be performed on the entire image
* \param [in] detector  - object detector
* \param [out] num      - number of objects found
* \param [out] ppResult - pointer to an array of rectangles found
* \return AWP_OK if success or else AWP_ERROR
*/
AWPRESULT awpObjectDetect(awpImage* pImage, awpRect* pRoi, AWPHANDLE detector, AWPINT* num, awpRect** ppResult);
/**
*@brief checking for an object in a given rectangle
* \awpImage - source integral image
* \pRect	- the area is interesting where the search for objects should be performed. NULL means that the search must be performed on the entire image
* \detector - object detector
* \res      - detection result. TRUE the object is present in the specified rectangle. FALSE the object is not in the given rectangle
* \return AWP_OK if success or else AWP_ERROR
*/

AWPRESULT awpDetectInRect(awpImage* pImage, AWPHANDLE detector, awpRect* pRect, AWPBOOL* res);

/** @} */ /* end of detectorgroup  */

AWPRESULT awp2BitLBP(awpImage* pImage, awpRect* pRect, AWPBYTE* pCode);
AWPRESULT awp4BitLBP(awpImage* pImage, awpRect* pRect, AWPBYTE* pCode);

/** \addtogroup awplbbgroup Loacal Binary Pattens
*   @{
*/
/**
*    \brief receives a set of coordinates of points for measurement.
*    \param [in] -p - a point in the image, which must be calculated code.
*    \param [in] -shiftX - horizontal offset of the point
*    \param [in] -shiftY - point offset vertically.
*    \param [in] -scale - the coefficient. stretching the neighborhood. (> = 1)
*    \param [out] -out - pointer to an array of resulting coordinates
*	\return AWP_OK if success or else AWP_ERROR
*/
AWPRESULT awpLPBSPoints(awpPoint p, AWPWORD shiftX, AWPWORD shiftY, AWPFLOAT scale, AWPBOOL interpolation, awp2DPoint* out);
/**
*    \brief receives a set of coordinates of points for measurement.
*    \param [in] -p - a point in the image, which must be calculated code.
*    \param [in] -options - parameter indicating which variation of the algorithm to use \n
*   possible values:\n
*	gives the coordinates of pixels with a radius of 1 \n
*	AWP_LBP_U1R1\n
*	AWP_LBP_U2R1\n
*	produces coordinates of pixels with radius 2\n
*	AWP_LBP_U1R2\n
*	AWP_LBP_U2R2\n
*    \param [out]-out - pointer to an array of resulting coordinates
*	\return AWP_OK if success or else AWP_ERROR
*/
AWPRESULT awpLPBPoints(awpPoint p, AWPBYTE options, AWPBOOL interpolation, awp2DPoint* out);


/**
*    \brief receives LBP code for the smeared point of the image.
*    \param [in] -src is the original image. The image type is AWP_BYTE. Number of channels 1 or 3.\n
*	  In the case of 3 channels (RGB) channel # 1 (R)
*    \param [in] -p is the point on the image for which the code is to be calculated.
*    \param [in] -shiftX - horizontal offset of the point
*    \param [in] -shiftY - point offset vertically.
*    \param [in] -scale - the coefficient. stretching the neighborhood. (> = 1)
*    \param [in] -options - a parameter indicating which variation of the algorithm to use for\n
*	 get the code. Must be one of the AWP_LBP_XXX constants.\n
*	 AWP_LBP_U1R1 - nonuniform transformation of radius 1.\n
*	 AWP_LBP_U2R1 - uniform transformation of radius 1.\n
*	 ununiform codes - * pcode = 0\n
*	 AWP_LBP_U1R2 - nonuniform transformation of radius 2.\n
*	 AWP_LBP_U2R2 - uniform transformation of radius 2.\n
*	 nonuniform codes * pcode = 0\n
*    \param [out]-pcode is a pointer to a variable that contains the code
*	\return AWP_OK if success or else AWP_ERROR \n
*	if the result of the function! = AWP_OK * pcode is undefined.
*/
AWPRESULT awpLBPCode(awpImage* src, awpPoint p, AWPWORD shiftX, AWPWORD shiftY, AWPFLOAT scale, AWPBYTE options, AWPBOOL interpolation, AWPBYTE* pcode);

/**
*    \brief receives the LBP descriptor of the image section in the form of a histogram
*    \param [in] --src is the original image. The image type is AWP_BYTE. Number of channels - 1.
*    \param [out] --dst - LBP image descriptor of its image
*    \param [in] --roi - the area of ​​the image for which the descriptor should be calculated.\n
*	 NULL means that the handle is evaluated for the entire image.
*    \param [in] --options - a parameter indicating which variation of the algorithm to use for\n
*	 receive the descriptor. Must be one of the AWP_LBP_XXX constants.\n
*	 AWP_LBP_U1R1 - nonuniform transformation of radius 1. Descriptor size - 256\n
*	 AWP_LBP_U2R1 is a uniform transformation of radius 1. The size of the descriptor is 59\n
*	 ununiform codes in the zero element of the descriptor\n
*	 AWP_LBP_U1R2 - nonuniform transformation of radius 2. Descriptor size - 256\n
*	 AWP_LBP_U2R2 is a uniform transformation of radius 2. The size of the descriptor is 59\n
*	 ununiform codes in the zero element of the descriptor\n
*    \param [in]-norm - flag of the normalization of the received descriptor. TRUE means\n
*	 normalization sum (dst) = 1
*	\return AWP_OK if success or else AWP_ERROR \n
*/
AWPRESULT awpLBPDescriptor(awpImage* src, awpImage* dst, awpRect* roi, AWPBYTE options, AWPBOOL interpolation, AWPBOOL norm);
/**
*    \brief receives the LBP descriptor of the image area as a set\n
*	 histograms. The image region is divided into n * m non-overlapping\n
*	 rectangles.
*    \param [in]-src is the original image.
*    \param [out]-dst - set of LBP image descriptors or its section. Image Height\n
*	 m * n, the width is determined by the options parameter
*    \param [in]-roi - the area of ​​the image for which the set is to be calculated\n
*	 descriptors. NULL means that the descriptors are computed for the entire image.
*    \param [in]-n - number of descriptors horizontally
*    \param [in]-m - number of descriptors vertically
*    \param [in] --options - a parameter indicating which variation of the algorithm to use for\n
*	 receive the descriptor. Must be one of the AWP_LBP_XXX constants.\n
*	 AWP_LBP_U1R1 - nonuniform transformation of radius 1. Descriptor size - 256\n
*	 AWP_LBP_U2R1 is a uniform transformation of radius 1. The size of the descriptor is 59\n
*	 ununiform codes in the zero element of the descriptor\n
*	 AWP_LBP_U1R2 - nonuniform transformation of radius 2. Descriptor size - 256\n
*	 AWP_LBP_U2R2 is a uniform transformation of radius 2. The size of the descriptor is 59\n
*	 ununiform codes in the zero element of the descriptor\n
*    \param [in]-norm - flag of the normalization of the received descriptor. TRUE means\n
*	 normalization sum (dst (i)) = 1, where i is the number of the descriptor from n * m
*	\return AWP_OK if success or else AWP_ERROR \n
*/
AWPRESULT awpLBPGridDescriptor(awpImage* src, awpImage* dst, awpRect* roi, AWPBYTE n, AWPBYTE m, AWPBYTE options, AWPBOOL interpolation, AWPBOOL norm);

/** @} */ /*  end of awplbbgroup */


/** @defgroup approximation
*   @{
*/

/**
*	\brief polynom approximation function 
*	\param src pointer to the source image 
*		   image sould be type AWP_DOUBLE and has sSizeY = 2 
*		   first line contains x0,x1...xN, where N  = sSizeX 
*		   second line contaims y0,y1...yN, where N = sSizeX
*	\param dst pointer to coefficients 
*	       dst should be type AWP_DOUBLE and has sSizeY == 1
*		   sSizeX contans power of polynom 	sSizeX <= 6
*	\return AWP_OK if success or else AWP_ERROR
*/
AWPRESULT awpApproxPoly(awpImage* src, awpImage* dst);


/** @} */ /* end of approximation  */


/** @defgroup get
*   @{
*/

/**
*	\brief get pixels data from line 
*	\param src pointer to the source image
*	\param p1 start point of line 
*	\param p2 end point of line 
*	\param dst double pointer to destination image, contains pixels data 
*	\return AWP_OK if success or else AWP_ERROR
*/

AWPRESULT awpGetLineImage(awpImage* src, awpPoint p1, awpPoint p2, awpImage** dst);

/**
*	\brief retruns size of buffer for awpGetLineData
*	\param src pointer to the source image. 
*	\param p1 start point of line
*	\param p2 end point of line
*	\param linelenght pointer to integer contains lene lenght 
*	\return AWP_OK if success or else AWP_ERROR
*/
AWPRESULT awpGetLineDataLenght(awpImage* src, awpPoint p1, awpPoint p2, AWPINT* linelenght);


/**
*	\brief get pixels data from line
*	\param src pointer to the source image. image should be single channel
*	\param p1 start point of line 
*	\param p2 end point of line 
*	\param data double pointer to destination buffer
*	\return AWP_OK if success or else AWP_ERROR
*/
AWPRESULT awpGetLineData(awpImage* src, awpPoint p1, awpPoint p2, AWPDOUBLE* data);


/** @} */ /* end of get  */


//---------------------------------------------------------------------------
AWPRESULT awpGaussLinear(awpImage* matrix, awpImage* y, awpImage* result);
AWPRESULT awpMatrixTranspose (awpImage* matrix, awpImage* result);
AWPRESULT awpMulMatrix(awpImage* matrix1, awpImage* matrix2, awpImage** ppDst);
AWPRESULT awpMatrixInvert(awpImage* matrix, awpImage* invert);
AWPRESULT awpLinearRegress(awpImage* matrix, awpImage* y, awpImage* b);



#ifdef __cplusplus
};
#endif

#endif  /*__AWP_IPL__ */
