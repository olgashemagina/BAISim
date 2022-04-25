#ifndef _FVLCORE_H_
#define _FVLCORE_H_
/*============================================================
					Face Verification Core
  Version:
	1.6
  Description:
	Face Recognition Using Eigenvectors
	Based on part of code Face Verification Library 1.5
  Copyright and autors:
	Copyright (c) AWP TEAM 2002-2005

	Written by Awp Team 2002 - 2005
	Nizhny Novgorod. Russia
	Autors:
	Alex A.Telnykh, E.Eremin

 Exports:
	-fvcBuildTemplate
	-fvcCompare
	-fvcCreateTemplate
	-fvcFreeTemplate

 Dependences:
	AWP's image processing library: (awpipl.lib)
 History:
	- created 02.2003 1.0
	- Remove dependence from awpeigenlib.lib    -Version 1.1
	- add fvcSaveTemplate and fvcLoadTemplate   -Version 1.2
	- add fvcSaveTemplateAsImage                -Version 1.3
	- add fvcPack and fvcUnpak by E.Eremin      -Version 1.4
	- fix some bugs in the library              -Version 1.41
	- adjust library for awpipl-2               -Version 1.5
	- add fvcConvertTemplateToImage and
		  fvcResampleTemplate                   -Verion  1.6
======================================================================*/
#include "awpipl.h"


/** @defgroup errgroup Error codes
*   @{
*/

/**
*	\def FVC_OK
*	\brief all went well
*/
#define FVC_OK					 0
/**
*	\def FVC_INVALIDARG
*	\brief one of arguments was invalid
*/
#define FVC_INVALIDARG			-1
/**
*	\def FVC_OUTOFMEMORY
*	\brief problems with memory
*/
#define FVC_OUTOFMEMORY			-2
/**
*	\def FVC_INVALIDTEMPL
*	\brief template is invalid
*/
#define FVC_INVALIDTEMPL		-3
/**
*	\def FVC_CANNOTBUILD
*	\brief unable to build template
*/
#define FVC_CANNOTBUILD			-4
/**
*	\def FVC_INCOMPATIBLE
*	\brief incompatible types and(or) sizes of images and template
*/
#define FVC_INCOMPATIBLE		-5
/**
*	\def FVC_CANNOTSAVE
*	\brief cannot save to file
*/
#define FVC_CANNOTSAVE			-6
/**
*	\def FVC_CANNOTLOAD
*	\brief cannot load from file
*/
#define FVC_CANNOTLOAD			-7
/** @} */ /* end of errgroup */


// compare type
#define FVC_COMPARE_EUCLID		0
#define FVC_COMPARE_SEGMENTED	1
#define FVC_COMPARE_EUCLID_NORM 2
#define FVC_COMPARE_CORRELATION 3


/** 
*	\struct tagFvcTemplate
*	\brief Eigen vectors template
*/
typedef struct tagFvcTemplate
{
	/** size of this structure */
	int		nSize;
	/** size of all vectors and average object */
	int		nDataSize;
	/** number of created vectors */
	int		nNumVectors;
	/** width of one vector */
	int		nVectorWidth;
	/** height of one vector */
	int		nVectorHeight;
	/** pointer to vector's data */
	float*  pVectors;
}FvcTemplate;

typedef struct tagSClassInfo
{
    awpImage**	m_pVectors;		// class vectors
	long	 	m_nCount;		// vectors count
    awpImage*	m_pAverage;		// average imge for class
    double      m_dblDistance;	//
    float*      m_eigVals;
}SClassInfo;

typedef struct tagLDAImages
{
    awpImage** ppImages;    //предъ€влени€ в классе
    int classID;            //номер класса, к которому принадлежат ppImage
    int totalInClass;       //число предъ€влений в классе classID
}LDAImages;

typedef struct tagFvcImages
{
    LDAImages* pData;
    int numClasses;            //число классов
}FvcImages;

/** @defgroup coregroup Core fuctions
*   @{
*		Core fuctions for training and recognition process
*/

/**
*	\brief Build FVC template
*	\param ppImages pointer to array of source images
*	\param count size of the source array
*	\param pTemplate pointer to resulting template
*	\return FVC_OK if success, error code in other case
*/
int fvcBuildTemplate(awpImage** ppImages, int count, FvcTemplate* pTemplate);
/**
*	\brief Build FVC template
*	\param ppImages pointer to array of source images with attributes
*	\param count size of the source array
*	\param pTemplate pointer to resulting template
*	\return FVC_OK if success, error code in other case
*/
int fvcBuildTemplateLDA(FvcImages* Images, FvcTemplate** pTemplate);

/**
*	\brief Compare image and FVC template
*	\param pImage pointer to image
*	\param pTemplate pointer to template
*	\param Error comparison result value
*	\param iCompareType compare type option, can take one of the following values:\n
*			FVC_COMPARE_EUCLID\n
*			FVC_COMPARE_SEGMENTED
*	\return FVC_OK if success, error code in other case
*/
int fvcCompare(awpImage* pImage, FvcTemplate* pTemplate, double* Error, int iCompareType);
int fvcFastCompare(awpImage* pImage, SClassInfo* pInfo);
/** @} */ /* end of coregroup */


/** @defgroup tplgroup Templates management
*   @{
*/

/**
*	\brief Create new template structure
*	\param ppTemplate destination template pointer
*	\param Width template width
*	\param Height template height
*	\param NumVectors number of template vectors to create
*	\return FVC_OK if success, error code in other case
*/
int fvcCreateTemplate(FvcTemplate** ppTemplate, int Width, int Height, int NumVectors, bool with_eigvals = false);

/**
*	\brief Delete template structure
*	\param ppTemplate pointer to pointer to the template
*	\return FVC_OK if success, error code in other case
*/
int fvcFreeTemplate(FvcTemplate** ppTemplate);

/**
*	\brief Save template as image file
*	\param pFileName destination file name
*	\param pTemplate source template
*	\return FVC_OK if success, error code in other case
*/
int fvcSaveTemplateAsImage(const char* pFileName, FvcTemplate* pTemplate);

/**
*	\brief Save template data
*	\param pFileName destination file name
*	\param pTemplate pointer to the source template
*	\return FVC_OK if success, error code in other case
*/
int fvcSaveTemplate(const char* pFileName, FvcTemplate* pTemplate);

/**
*	\brief Load template from file
*	\param pFileName source file name
*	\param ppTemplate pointer to pointer destination template
*	\return FVC_OK if success, error code in other case
*/
int fvcLoadTemplate(const char* pFileName, FvcTemplate** ppTemplate);
             /**
*	\brief Resample template
*	\param pTemplate pointer to the source template
*	\param NumVectors new number of template vectors
*	\return FVC_OK if success, error code in other case
*/
int fvcResampleTemplate(FvcTemplate* pTemplate, int NumVectors);

/**
*	\brief Convert template to image
*	\param pTemplate source template
*	\param ppImage destination image
*	\return FVC_OK if success, error code in other case
*/
int fvcConvertTemplateToImage(FvcTemplate* pTemplate, awpImage** ppImage);
/** @} */ /* end of tplgroup */


/** @defgroup packgroup Templates packing
*   @{
*/

/**
*	\brief Pack template to BLOB
*	\param pTemplate source template
*	\param ppData destination data array
*	\param piLen destination array length
*	\return FVC_OK if success, error code in other case
*/
int fvcPack(FvcTemplate* pTemplate,char** ppData,int* piLen);

/**
*	\brief Unpack template from BLOB
*	\param pData pointer to the source array
*	\param iLen source array length
*	\param pTemplate destination template
*	\return FVC_OK if success, error code in other case
*/
int fvcUnPack(char* pData,int iLen,FvcTemplate** pTemplate);

/**
*	\brief Convert template to SClassInfo
*	\param pTemplate pointer to template
*	\param pInfo pointer to SClassInfo
*	\return FVC_OK if success, error code in other case
*/
int fvcTemplateToClassInfo(FvcTemplate* pTemplate, SClassInfo* pInfo);
int fvcTemplateToClassInfoFloat(FvcTemplate* pTemplate, SClassInfo* pInfo);
/**
*	\brief Convert SClassInfo to Template
*	\param ppTemplate pointer to template
*	\param pInfo pointer to SClassInfo
*	\return FVC_OK if success, error code in other case
*/
int fvcClassInfoToTemplate(FvcTemplate** ppTemplate, SClassInfo* pInfo);

/**
*	\brief Frees awpImages in the SClassInfo and sets them to NULL
*	\param pInfo pointer to SClassInfo
*	\return FVC_OK if success, error code in other case
*/
int fvcFreeClassInfo(SClassInfo* pInfo);

/** @} */ /* end of packgroup */

int fvcGetDecompositionCoeff(FvcTemplate* pTemplate, awpImage* pImage,
float* pCoeff);

int fvcGetReconstruction(FvcTemplate* pTemplate, awpImage* pImage, awpImage** ppReconstrunction);

int fvcBuildReconstruction(FvcTemplate* pTemplate, float* pCoeff, awpImage** ppReconstrunction);
int fvcBuildReconstructionFloat(FvcTemplate* pTemplate, float* pCoeff, awpImage** ppReconstrunction);


#endif //_FVLCORE_H_
