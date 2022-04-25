/*
//
//  RCS:
//      $Source: _awpColor.h
//      $Revision: 1.0 $
//      $Date:
//
//    Purpose: AWPIPL header file
//    Contents: internal header file for object detection 
//    Authors : Alexander Telnykh
*/
#ifndef _AWPDETECTOR_H_
#define _AWPDETECTOR_H_

/**
\struct awpWeak
\brief weak classifier
*/
typedef struct
{
	/**x shift of the weak*/
	AWPSHORT x;
	/**y shift of the weak*/
	AWPSHORT y;
	/**width of the weak*/
	AWPSHORT w;
	/**height of the weak*/
	AWPSHORT h;
	/**number of beans in the classifier*/
	AWPSHORT numBins;
	/**weaks weight*/
	AWPDOUBLE alfa;
	/**pointer to classifiers data*/
	AWPDOUBLE* bins;
}awpWeak;

/**
\struct awpStrong
\brief strong classifier
*/
typedef struct
{
	/**threshold of strong classifier*/
	AWPDOUBLE alfa;
	/** number of weaks*/
	AWPSHORT  nWeaks;
	/** pointer to the weaks array*/
	awpWeak*  pWeaks;
}awpStrong;
/**
\struct awpDetector
\brief cascade detector data
*/
typedef struct
{
	AWPSHORT type;
	AWPSHORT width;
	AWPSHORT height;
	AWPSHORT nStages;
	awpStrong* pStrongs;
}awpDetector;
/**
\
*/
typedef AWPBOOL(*awpDetectFunc)(awpImage* pImage, awpImage* pImage1, awpRect* pRect, awpDetector* pDetector);

typedef struct
{
	awpDetector     detectorCascade;
	awpDetectFunc   detectFunc;
}awpCascade;

AWPRESULT awpCreateCascade(awpCascade** ppCascade);
AWPRESULT awpFreeCascade(awpCascade** ppCascade);
AWPRESULT awpInitCascade(awpCascade* pCascade, awpImage* img);
AWPRESULT awpLoadCascade(awpCascade* pCascade, const char* lpFileName);
AWPRESULT awpReleaseCascade(awpCascade* pCascade);

#endif 