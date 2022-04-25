#include "stdlib.h"
#include "fvlcore.h"
#include "awpeigenlib.h"
//#include "SeparationMatrix.h"
#include <math.h>

const unsigned int c_nMagic = 0x00265646; /* 00 F V L*/
typedef struct tagFileHeader
{                      
	unsigned int	unMagic;
	unsigned char	ucMajor;
	unsigned char   ucMinor;
}FileHeader;

/*Проверяет шаблон*/
static int _fvcCheckTemplate(FvcTemplate* pTemplate)
{

    int size, size_with_eigval;
    if (pTemplate == NULL)
        return FVC_INVALIDARG;
    if (pTemplate->pVectors == NULL && pTemplate->nDataSize > 0 && pTemplate->nNumVectors > 0)
        return FVC_INVALIDARG;
    if (pTemplate->nSize !=     sizeof(FvcTemplate))
        return FVC_INVALIDTEMPL;
    if (pTemplate->nVectorHeight < 0 || pTemplate->nVectorWidth < 0)
		return FVC_INVALIDTEMPL;
    // check the size
    size = pTemplate->nNumVectors*pTemplate->nVectorHeight*pTemplate->nVectorWidth*sizeof(float);
    size_with_eigval = (pTemplate->nNumVectors*(pTemplate->nVectorHeight*pTemplate->nVectorWidth+1)-1)*sizeof(float);
    if (pTemplate->nDataSize != size && pTemplate->nDataSize != size_with_eigval )
        return FVC_INVALIDTEMPL;

    return FVC_OK;
}

static double _fvcEuclidDistanceFLOAT(awpImage* img1, awpImage* img2)
{
    float* b1;
    float* b2;
    int i;
    double result = -1;
    int size = img1->sSizeX*img1->sSizeY;
	if (img1 == NULL || img2 == NULL)
        return result;
    if (img1->sSizeX != img2->sSizeX && img1->sSizeY != img2->sSizeY)
        return result;
    if (img1->dwType != img2->dwType)
        return result;
    if (img1->dwType != AWP_FLOAT)
        return result;

	if (size == 0)
        return result;
    result = 0;
    b1 = (float*)img1->pPixels;
    b2 = (float*)img2->pPixels;
    double bb1 = 0;
    double bb2 = 0;
    for (i = 0; i < size; i++)
    {
        bb1 += b1[i]*b1[i];
        bb2 += b2[i]*b2[i];
        result += ((double)b1[i]-(double)b2[i])*((double)b1[i]-(double)b2[i]);
    }
    if (bb1 == 0 || bb2 == 0)
		return 1;
	bb1 = sqrt(bb1);
	bb2 = sqrt(bb2);
	result = sqrt (result);// / (bb1*bb2);
	return result;
	//НОРМИРОВКА
	//result /= size;
	//return result;
}
static double _fvcEuclidDistanceBYTE(awpImage* img1, awpImage* img2)
{
    AWPBYTE* b1;
	AWPBYTE* b2;
    int i;
    double result = -1;
    int size = img1->sSizeX*img1->sSizeY;
    if (img1 == NULL || img2 == NULL)
        return result;
    if (img1->sSizeX != img2->sSizeX && img1->sSizeY != img2->sSizeY)
        return result;
    if (img1->dwType != img2->dwType)
        return result;
    if (img1->dwType != AWP_BYTE)
        return result;

    if (size == 0)
        return result;
    result = 0;
    b1 = (AWPBYTE*)img1->pPixels;
    b2 = (AWPBYTE*)img2->pPixels;
    double bb1 = 0;
    double bb2 = 0;
    for (i = 0; i < size; i++)
    {
        bb1 += b1[i]*b1[i];
        bb2 += b2[i]*b2[i];
        result += ((double)b1[i]-(double)b2[i])*((double)b1[i]-(double)b2[i]);
    }
	if (bb1 == 0 || bb2 == 0)
        return 1;
   // bb1 = sqrt(bb1);
   // bb2 = sqrt(bb2);
   // result = sqrt (result) / (bb1*bb2);
   // return result;
    //НОРМИРОВКА
    result /= size;
    result /= 255*255;
    return result;
}


/*вычисляет евклидово расстояние между изображениями*/
static double _fvcEuclidDistance(awpImage* img1, awpImage* img2)
{
	if (img1 == NULL || img2 == NULL)
        return -1;
    if (img1->sSizeX != img2->sSizeX && img1->sSizeY != img2->sSizeY)
        return -1;
    if (img1->dwType != img2->dwType)
        return -1;
    if (img1->dwType == AWP_BYTE)
        return _fvcEuclidDistanceBYTE(img1, img2);
    else if (img1->dwType == AWP_FLOAT)
        return _fvcEuclidDistanceFLOAT(img1, img2);
	else
        return -1;
}

/*вычисляет нормированное евклидово расстояние между изображениями,из
которых вычтено среднее значение интенсивности*/
static double _fvcEuclidDistanceNorm(awpImage* img1, awpImage* img2)
{
    AWPBYTE* b1;
    AWPBYTE* b2;
    int i;
    double result = -1;
    int size = img1->sSizeX*img1->sSizeY;
    if (img1 == NULL || img2 == NULL)
        return result;
    if (img1->sSizeX != img2->sSizeX && img1->sSizeY != img2->sSizeY)
        return result;
    if (img1->dwType != img2->dwType)
        return result;
    if (img1->dwType != AWP_BYTE)
        return result;

    if (size == 0)
        return result;
    result = 0;
    b1 = (AWPBYTE*)img1->pPixels;
	b2 = (AWPBYTE*)img2->pPixels;
    double a1 = 0;
    double a2 = 0;
    for (i = 0; i < size; i++)
    {
        a1 += b1[i];
        a2 += b2[i];
    }
    a1 /= size;
    a2 /= size;
    for (i = 0; i < size; i++)
        result += ((double)b1[i] -(double)b2[i])*((double)b1[i] -(double)b2[i]);
    result /= size;
    result /= 255*255;
    return result;
}
/*вычисляет коэф. корреляции между двумя изображениями, с учетом средней интенсивности*/
static double _fvcCorrelationCoeff(awpImage* img1, awpImage* img2)
{
    AWPBYTE* b1;
    AWPBYTE* b2;
    int i;
    double result = -1;
    int size = img1->sSizeX*img1->sSizeY;
    if (img1 == NULL || img2 == NULL)
        return result;
	if (img1->sSizeX != img2->sSizeX && img1->sSizeY != img2->sSizeY)
        return result;
    if (img1->dwType != img2->dwType)
        return result;
    if (img1->dwType != AWP_BYTE)
		return result;

    if (size == 0)
        return result;
    result = 0;
    b1 = (AWPBYTE*)img1->pPixels;
    b2 = (AWPBYTE*)img2->pPixels;
    // вычисление средней интенсивности сигнала
    // и квадрата интенсивности
    double a1 = 0;
    double a2 = 0;
    for (i = 0; i < size; i++)
    {
        a1 += b1[i];
        a2 += b2[i];
    }
    a1 /= size;  // <- средняя интенсивность img1
    a2 /= size;  // <- средняя интенсивность img2
    // вычисление коэф. корреляции
    double c = 0;
    double f = 0;
	double g = 0;
    for( i = 0; i < size; i++)
    {
      c += (b1[i] - a1)*(b2[i] - a2);
      f += (b1[i]- a1)*(b1[i] - a1);
      g += (b2[i]- a2)*(b2[i] - a2);
    }
    f = sqrt(f);
    if (f == 0 || g == 0)
        return 1; // сигналы не коррелированы
    g = sqrt(g);
    double d = f*g;
    result = c / d;
    return 1 - result;
}
static double _fvcSegmentedDistance(awpImage* img1, awpImage* img2, AWPBYTE bThreshold)
{
    AWPBYTE* b1;
    AWPBYTE* b2;
    AWPBYTE* d;
    int i;
    double result = -1;
    int size = img1->sSizeX*img1->sSizeY;
    if (img1 == NULL || img2 == NULL)
        return result;
    if (img1->sSizeX != img2->sSizeX && img1->sSizeY != img2->sSizeY)
		return result;
    if (img1->dwType != img2->dwType)
        return result;
    if (img1->dwType != AWP_BYTE)
        return result;

	if (size == 0)
        return result;
    result = 0;
	b1 = (AWPBYTE*)img1->pPixels;
	b2 = (AWPBYTE*)img2->pPixels;

	awpImage* distance = NULL;

    awpCreateImage(&distance,img1->sSizeX, img1->sSizeY, 1, AWP_BYTE);
	d = (AWPBYTE*)distance->pPixels;
	for (i = 0; i < size; i++)
		d[i] = abs(b1[i]-b2[i]);// < bThreshold) ? 1 : 0;

	for (i = 0; i < size; i++)
        result += (d[i] * ((double)b1[i]-(double)b2[i])*((double)b1[i]-(double)b2[i]));
    result /= size;
    result /= 255*255;
	awpReleaseImage(&distance);
    return result;
}

/*сравнивает вектор с шаблоном*/
static void _fvcCompareVectors(awpImage* img, SClassInfo* class_info, int iCompareType, FvcTemplate* tmpl)
{
    float* DecompCoeff = NULL;
	awpImage* reconstruction = NULL;
	class_info->m_dblDistance = 1;
/*
    awpCreateImage(&reconstruction,img->sSizeX, img->sSizeY, 1, AWP_BYTE);
*/
    DecompCoeff = (float*)malloc(class_info->m_nCount*sizeof(float));
    // we shall find coef. projection of the initial image on
    // space of own vectors
	// differently we shall spread out the initial image
    awpEigenDecomposite(img, class_info->m_nCount,class_info->m_pVectors,class_info->m_pAverage,DecompCoeff);
    if (img->dwType == AWP_BYTE)
    {
        fvcBuildReconstruction(tmpl, DecompCoeff, &reconstruction);
    }
    else if (img->dwType == AWP_FLOAT)
    {
		fvcBuildReconstructionFloat(tmpl, DecompCoeff, &reconstruction);
    }
//    awpEigenProjection(class_info->m_nCount,class_info->m_pVectors,DecompCoeff,class_info->m_pAverage,reconstruction);

    AWPBYTE bThreshold = 100;
	switch( iCompareType ) {
    case FVC_COMPARE_EUCLID:
		class_info->m_dblDistance = _fvcEuclidDistance(img, reconstruction);
		break;
    case FVC_COMPARE_SEGMENTED:
    	class_info->m_dblDistance = _fvcSegmentedDistance(img, reconstruction, bThreshold);
		break;
    case FVC_COMPARE_EUCLID_NORM:
        class_info->m_dblDistance = _fvcEuclidDistanceNorm(img, reconstruction);
    break;
    case FVC_COMPARE_CORRELATION:
        class_info->m_dblDistance = _fvcCorrelationCoeff(img, reconstruction);
    break;
	default:
		class_info->m_dblDistance = _fvcEuclidDistance(img, reconstruction);
    }

	free(DecompCoeff);

#ifdef _DEBUG
	awpConvert(img, AWP_CONVERT_TO_BYTE_WITH_NORM);
	awpSaveImage("src.jpg", img);
	awpConvert(reconstruction, AWP_CONVERT_TO_BYTE_WITH_NORM);
	awpSaveImage("reconstruct.jpg", reconstruction);
#endif
	awpReleaseImage(&reconstruction);
}

int fvcFastCompare(awpImage* pImage, SClassInfo* pInfo)
{
     _fvcCompareVectors(pImage, pInfo, FVC_COMPARE_EUCLID, NULL);
     return FVC_OK;
}


/*Преобразует изображение awp из float формата в byte*/
static void _fvcConvertFloatAwp2GrayscaleAwp(awpImage* pflt, awpImage* pgray)
{
    int i;
    int s = pflt->sSizeX*pflt->sSizeY;
    float* src = (float*)pflt->pPixels;
    AWPBYTE*  dst = (AWPBYTE*)pgray->pPixels;
    float fl_max = src[0];
    float fl_min = src[0];
    for (i = 1; i < s; i++)
    {
        if (fl_max < src[i]) fl_max = src[i];
        if (fl_min > src[i]) fl_min = src[i];
    }
    if (fl_max == fl_min) return;
    for(i = 0 ; i < s; i++)
    {
		dst[i] = (AWPBYTE)(255*(src[i] - fl_min)/(fl_max - fl_min));
    }

}
/*преобразует шаблон в массив изображений*/
static int _fvcTemplate2Images(FvcTemplate* pTemplate, awpImage* pAverage, awpImage** ppObjects, int* num )
{
    int i,w,h;
    float* src;
    float* dst;
    int result = FVC_OK;
    // arguments
    if (pTemplate == NULL || pAverage == NULL || ppObjects == NULL || num == NULL)
        return FVC_INVALIDARG;
    // template
    result = _fvcCheckTemplate(pTemplate);
    if (result != FVC_OK)
        return result;
    // check compatibility of template and images
    if (*num > pTemplate->nNumVectors)
        return FVC_INCOMPATIBLE;
    if (pTemplate->nVectorHeight != pAverage->sSizeY || pTemplate->nVectorWidth != pAverage->sSizeX ||
            pAverage->dwType != AWP_FLOAT)
        return FVC_INCOMPATIBLE;
    // check all images in the ppObjects
    for (i = 0; i < *num-1; i++)
	{
        if (pTemplate->nVectorHeight != ppObjects[i]->sSizeY || pTemplate->nVectorWidth != ppObjects[i]->sSizeX ||
                ppObjects[i]->dwType != AWP_FLOAT)
            return FVC_INCOMPATIBLE;
    }
    // copy data
    w = pTemplate->nVectorWidth;
    h = pTemplate->nVectorHeight;
    // copy average image
    src = pTemplate->pVectors + (pTemplate->nNumVectors-1)*w*h; // pointer to average image
    dst = (float*)pAverage->pPixels;
    memcpy(dst,src, w*h*sizeof(float));
    // copy eigen objects
    for (i = 0; i < pTemplate->nNumVectors-1; i++)
    {
        src = pTemplate->pVectors + i*w*h; // pointer to average image
        dst = (float*)ppObjects[i]->pPixels;
        memcpy(dst,src, w*h*sizeof(float));
    }
    return result;
}

/*==========================================================================
	Function:
	Purpose:
	Arguments:
	Return values:
============================================================================*/
int fvcBuildTemplate(awpImage** ppImages, int count, FvcTemplate* pTemplate)
{
    int result,num,i,w,h,j,r;
    float* pData;
    awpImage** eigenObjects = NULL;			// eigen vectors
    awpImage*  avg = NULL;					// average object
    float*     eigVals = NULL;				// eigen values
    AWPRESULT res = AWP_OK;
    // check arguments
    if (ppImages == NULL)
        return FVC_INVALIDARG;
    if (count <= 0)
        return FVC_INVALIDARG;
    result = _fvcCheckTemplate(pTemplate);
    if (result != FVC_OK)
        return result;

    w   = pTemplate->nVectorWidth;
    h	= pTemplate->nVectorHeight;
    num = pTemplate->nNumVectors - 1;

    // alloc memory for eigen array
    eigenObjects		= (awpImage**)malloc(num*sizeof(awpImage*));
    if (eigenObjects == NULL)
		return FVC_OUTOFMEMORY;

    // alloc memory for objects
    for(i = 0; i < num; i++)
    {
        res = awpCreateImage(&eigenObjects[i],(AWPWORD)w, (AWPWORD)h, 1, AWP_FLOAT);
        if (res != AWP_OK)
        {
            // cleanup
            for (j = 0; j < i; j++)
                awpReleaseImage(&eigenObjects[j]);
            free(eigenObjects);
            return FVC_OUTOFMEMORY;
        }
    }
    // alloc memory for avrage image
    res = awpCreateImage(&avg, (AWPWORD)w, (AWPWORD)h, 1, AWP_FLOAT);
    if (res != AWP_OK)
    {
        // cleanup
        for (i = 0; i < num; i++)
            awpReleaseImage(&eigenObjects[i]);
        free(eigenObjects);
        return FVC_OUTOFMEMORY;
    }
    // alloc memory for eigen values
	eigVals = (float*)malloc(num*sizeof(float));
    if (eigVals == NULL)
    {
        // cleanup
        for (i = 0; i < num; i++)
            awpReleaseImage(&eigenObjects[i]);
        free(eigenObjects);
        return FVC_OUTOFMEMORY;
    }

    r = awpCalcEigenObjects(num, ppImages, eigenObjects, 0.0001f, avg, eigVals);
    if (r != 0)
    {
        // cleanup
        for (i = 0; i < num; i++)
            awpReleaseImage(&eigenObjects[i]);
        free(eigenObjects);
        free(eigVals);
        return FVC_CANNOTBUILD;
    }

    // copy data to vectors and to average object
    for (i = 0; i < num; i++)
    {
        pData = pTemplate->pVectors + i*w*h;
        memcpy(pData, eigenObjects[i]->pPixels, h*w*sizeof(float));
	}
    // copy average
    memcpy(pTemplate->pVectors + num*h*w, avg->pPixels, h*w*sizeof(float));

    // copy eigen values

    if( pTemplate->nDataSize == (pTemplate->nNumVectors*(w*h+1)-1)*sizeof(float) )
    {
        memcpy(pTemplate->pVectors + (num+1)*h*w, eigVals, num*sizeof(float));
    }

    // cleanup

    for (i = 0; i < num; i++)
        awpReleaseImage(&eigenObjects[i]);
    free(eigenObjects);
    free(eigVals);
    awpReleaseImage(&avg);

    return FVC_OK;
}
/*
int fvcBuildTemplateLDA(FvcImages* Images, FvcTemplate** pTemplate)
{
    int result,num,i,w,h,j,r;
    float* pData;

    awpImage*  avg = NULL;

    AWPRESULT res = AWP_OK;
    // check arguments
    if (Images == NULL)
        return FVC_INVALIDARG;
    if (Images->numClasses <= 0)
        return FVC_INVALIDARG;

    LDAImages* Data = Images->pData;
    int numClasses = Images->numClasses;

    if( Data == NULL )
        return FVC_INVALIDARG;

    // alloc memory for avrage image
    res = awpCreateImage(&avg, (AWPWORD)w, (AWPWORD)h, 1, AWP_FLOAT);
    if (res != AWP_OK)
        return FVC_OUTOFMEMORY;

    float* avg_data = (float*)avg->pPixels;
    for( i = 0; i < avg->sSizeY*avg->sSizeX; i++ )
        avg_data[i] = 0;

    for( i=0; i<numClasses; i++ )
	{
        for( j = 0; j < Data[i].totalInClass; j++ )
        {
            awpImage* img = Data[i].ppImages[j];
            try
            {
                res = awpCheckImage(img);
                if (res != AWP_OK)
                    throw 0;
                if (img->dwType != AWP_FLOAT)
                    throw 0;
            }
            catch(...)
            {
                return FVC_INVALIDARG;
            }
        }
    }

	r = BuildSeparationMatrix( (void*)Images, pTemplate );
    if (r != 0)
    {
        awpReleaseImage(&avg);
        return FVC_CANNOTBUILD;
    }
    result = _fvcCheckTemplate(*pTemplate);
	if (result != FVC_OK)
        return result;
    w   = (*pTemplate)->nVectorWidth;
    h	= (*pTemplate)->nVectorHeight;
    num = (*pTemplate)->nNumVectors - 1;
    // copy average
    memcpy( (*pTemplate)->pVectors + num*h*w, avg->pPixels, h*w*sizeof(float));

    // cleanup

    awpReleaseImage(&avg);

    return FVC_OK;
}
*/
/*==========================================================================
	Function:       fvcCompare
	Purpose:        сравнивает изображение и шаблон
	Arguments:      pImage      - изображение для сравнения
                    pTemplate   - шаблон для сравнения
                    Error       - расстояние между изображением и шаблоном
                    iCompareType - свособ вычисления расстояния
                    FVC_COMPARE_EUCLID - вычисляется евклидово расстояние
                                         между изображением и шаблоном
                    FVC_COMPARE_SEGMENTED - вычисляется расстояние
                                            посегментно
	Return values:
============================================================================*/
int fvcCompare(awpImage* pImage, FvcTemplate* pTemplate, double* Error, int iCompareType)
{
    int result,res,i,w,h,j;
    float* fdata;
    SClassInfo class_info;
    if (Error == NULL)
        return FVC_INVALIDARG;
    if (pImage == NULL)
        return FVC_INVALIDARG;
    result = _fvcCheckTemplate(pTemplate);
    if (result != FVC_OK)
        return result;
    if (pImage->sSizeX != pTemplate->nVectorWidth || pImage->sSizeY != pTemplate->nVectorHeight)
        return FVC_INVALIDARG;
    // prepare eigen vectors and average object
    class_info.m_nCount		= pTemplate->nNumVectors-1;
    class_info.m_pAverage	= NULL;
    class_info.m_pVectors	= NULL;

    w = pTemplate->nVectorWidth;
    h = pTemplate->nVectorHeight;

    res = awpCreateImage(&class_info.m_pAverage, (AWPWORD)w, (AWPWORD)h, 1, AWP_FLOAT);
    if (res != AWP_OK)
		return FVC_OUTOFMEMORY;

    class_info.m_pVectors = (awpImage**)malloc(class_info.m_nCount*sizeof(awpImage*));
    if (class_info.m_pVectors == NULL)
    {
        awpReleaseImage(&class_info.m_pAverage);
        return FVC_OUTOFMEMORY;
    }


    for (i = 0; i < class_info.m_nCount; i++)
    {
        // create eigen vector
        res = awpCreateImage(&class_info.m_pVectors[i], (AWPWORD)w, (AWPWORD)h, 1, AWP_FLOAT);
		if (res != AWP_OK)
        {
            // cleanup
            for (j = 0; j < i; j++)
                awpReleaseImage(&class_info.m_pVectors[j]);

            awpReleaseImage(&class_info.m_pAverage);
            free(class_info.m_pVectors);
            return FVC_OUTOFMEMORY;
        }
        // copy eigen vector
        fdata = pTemplate->pVectors + i*w*h;
		memcpy(class_info.m_pVectors[i]->pPixels, (AWPBYTE*)fdata, w*h*sizeof(float));
    }
    fdata = pTemplate->pVectors + (pTemplate->nNumVectors-1)*w*h;
	memcpy(class_info.m_pAverage->pPixels, (AWPBYTE*)fdata, w*h*sizeof(float));

	_fvcCompareVectors(pImage, &class_info, iCompareType, pTemplate);
	*Error = class_info.m_dblDistance;

    // cleanup
    awpReleaseImage(&class_info.m_pAverage);
    for (j = 0; j < class_info.m_nCount; j++)
        awpReleaseImage(&class_info.m_pVectors[j]);
    free(class_info.m_pVectors);

    return FVC_OK;
}
/*==========================================================================
	Function:
		- fvcCreateTemplate
	Purpose:
		- alloc memory for template 
	Arguments:
	Return values:
============================================================================*/
int fvcCreateTemplate(FvcTemplate** ppTemplate, int Width, int Height, int NumVectors, bool with_eigvals)
{
	// check arguments
    int size;
    if (ppTemplate == NULL)
        return FVC_INVALIDARG;
    if (Width < 0 || Height < 0 || NumVectors < 0)
        return FVC_INVALIDARG;
    if (*ppTemplate != NULL)
        return FVC_INVALIDARG;
    // alloc memory for template
    *ppTemplate = (FvcTemplate*)malloc(sizeof(FvcTemplate));
    if (*ppTemplate == NULL)
        return FVC_OUTOFMEMORY;
    // fill out template's fields
    (*ppTemplate)->nSize		= sizeof(FvcTemplate);

    (*ppTemplate)->nNumVectors	= NumVectors+1; // число векторов в шаблоне больше на единицу поскольку освобождается место для усредненного изображения

    (*ppTemplate)->nVectorHeight= Height;
    (*ppTemplate)->nVectorWidth = Width;
    if( !with_eigvals )
        size = (NumVectors+1)*Height*Width*sizeof(float);
    else
        size = ((NumVectors+1)*Height*Width+NumVectors)*sizeof(float); //резервируется место для собственных значений
    (*ppTemplate)->nDataSize	 = size;
    (*ppTemplate)->pVectors = NULL;
    // alloc memory for vectors
	(*ppTemplate)->pVectors = (float*)malloc(size);
    if ((*ppTemplate)->pVectors == NULL)
    {
        free(*ppTemplate);
        *ppTemplate = NULL;
        return FVC_OUTOFMEMORY;
    }
    // all went well
    //return FVC_OK;
    return _fvcCheckTemplate(*ppTemplate);
}
/*==========================================================================
	Function: fvcFreeTemplate
	Purpose: free template
	Arguments:
	Return values:
============================================================================*/
int fvcFreeTemplate(FvcTemplate** ppTemplate)
{
    int result = FVC_OK;
    if (ppTemplate == NULL)
        return FVC_INVALIDARG;

    // check template
    result = _fvcCheckTemplate(*ppTemplate);
    if (result != FVC_OK)
		return result;
    if ((*ppTemplate)->pVectors != NULL)
        free((*ppTemplate)->pVectors);
    free(*ppTemplate);
    *ppTemplate = NULL;
    return result;
}
/*==========================================================================
	Function:
		- fvcSaveTemplate
	Purpose:
		- Save tempalate as tiled awp image
	Arguments:
		- pFileName : file name
		- pTemplate : fvc template
	Return values:
============================================================================*/
int fvcSaveTemplateAsImage(const char* pFileName, FvcTemplate*pTemplate)
{
    // variables
    AWPBYTE* src;				// pointers to pixels
    AWPBYTE* dst;
    int n;
    AWPWORD i,w,h;				// helpers
    AWPWORD ht,wt,j,k;			// helpers too
    int res = AWP_OK;		// result of awpipl's calls
	int result = FVC_OK;	// result of this
    awpImage* pTile = NULL;	// tiled image
    awpImage* pGray = NULL;	// temprary grayscale
    awpImage** pObjects = NULL;	// array of eigen objects
    awpImage* pAverage = NULL;	// average object

    //  arguments
    if (pFileName == NULL || pTemplate == NULL)
        return FVC_INVALIDARG;

    result = _fvcCheckTemplate(pTemplate);
    if (result != FVC_OK)
        return result;

    w = pTemplate->nVectorWidth;
    h = pTemplate->nVectorHeight;
    n = pTemplate->nNumVectors;

    // alloc memory for all objects

    res = awpCreateImage(&pAverage, w, h, 1, AWP_FLOAT);
    if (res != AWP_OK)
        return FVC_OUTOFMEMORY;

    pObjects = (awpImage**)malloc((pTemplate->nNumVectors - 1)*sizeof(awpImage*));
    memset(pObjects, 0, (pTemplate->nNumVectors - 1)*sizeof(awpImage*));
	if (pObjects == NULL)
    {
        result = FVC_OUTOFMEMORY;
        goto cleanup;
    }

    for (i = 0; i < n-1; i++)
    {
        res = awpCreateImage(&pObjects[i], w,h, 1, AWP_FLOAT);
        if (res != AWP_OK)
        {
            result = FVC_OUTOFMEMORY;
            goto cleanup;
        }
    }

    // convert template to awp images
    result = _fvcTemplate2Images(pTemplate, pAverage, pObjects, &n);
    if (result != FVC_OK)
        goto cleanup;

    // create tiled image
    ht = h;
    wt = w*n;
    res = awpCreateImage(&pTile, wt, ht, 1, AWP_BYTE);
    if (res != AWP_OK)
	{
        result = FVC_OUTOFMEMORY;
        goto cleanup;
    }
    // create temporary grayscale image
    res = awpCreateImage(&pGray, w,h, 1, AWP_BYTE);
    if (res != AWP_OK)
    {
        result = FVC_OUTOFMEMORY;
        goto cleanup;
    }


    _fvcConvertFloatAwp2GrayscaleAwp(pAverage, pGray);
    // save to tiled image average
    src = (AWPBYTE*)pGray->pPixels;
    dst = (AWPBYTE*)pTile->pPixels;
    for (i = 0; i < h; i++)
    {
        for (j = 0; j < w; j++)
        {
            dst[i*wt + j] = src[i*pGray->sSizeX + j];
        }
    }
    // save all eigen objects to tiled image
    for (k = 0; k < n-1; k++)
	{
        _fvcConvertFloatAwp2GrayscaleAwp(pObjects[k], pGray);
        src = (AWPBYTE*)pGray->pPixels;
        dst = (AWPBYTE*)pTile->pPixels;
        for (i = 0; i < h; i++)
        {
            for (j = 0; j < w; j++)
            {
                dst[i*wt + (k+1)*pGray->sSizeX + j] = src[i*pGray->sSizeX + j];
            }
        }
    }

    // uff!! tiled image was created save it!
    res = awpSaveImage(pFileName, pTile);
    if (res != AWP_OK)
    {
        result = FVC_CANNOTSAVE;
        goto cleanup;
    }

cleanup:
    if (pTile != NULL)
        awpReleaseImage(&pTile);
    if (pGray != NULL)
        awpReleaseImage(&pGray);
	if (pAverage != NULL)
        awpReleaseImage(&pAverage);
    if (pObjects != NULL)
    {
        for (i = 0; i < n-1; i++)
        {
            if (pObjects[i] != NULL)
                awpReleaseImage(&pObjects[i]);
        }
        free (pObjects);
    }
    return result;
}
/*==========================================================================
	Function:
		- fvlPack
	Purpose:
		- Packs template to byte array
	Arguments:
	Return values:
============================================================================*/
int fvcPack(FvcTemplate* pTemplate,char** ppData,int *piLen)
{
    int result = FVC_OK;	// result of this
    char* pD = NULL;
    int j = 0;//,w,h;
	/*check arguments*/
    if (pTemplate == NULL || ppData == NULL || piLen == NULL)
        return FVC_INVALIDARG;
    result = _fvcCheckTemplate(pTemplate);
    if (result != FVC_OK)
        return result;

    int nSize=0;

    //calculate sizeof structure
    nSize+=sizeof(pTemplate->nSize);
    nSize+=sizeof(pTemplate->nDataSize);
    nSize+=sizeof(pTemplate->nNumVectors);
    nSize+=sizeof(pTemplate->nVectorWidth);
    nSize+=sizeof(pTemplate->nVectorHeight);
    nSize+=pTemplate->nDataSize;

    //allocate memory
    pD = (char*)malloc(nSize*sizeof(char));
    if (pD == NULL)
        return FVC_OUTOFMEMORY;
    memset(pD,0,nSize*sizeof(unsigned char));

    //copy data
    memcpy(&pD[j],&pTemplate->nSize,sizeof(pTemplate->nSize));
    j+=sizeof(pTemplate->nSize);
	memcpy(&pD[j],&pTemplate->nDataSize,sizeof(pTemplate->nDataSize));
    j+=sizeof(pTemplate->nDataSize);
    memcpy(&pD[j],&pTemplate->nNumVectors,sizeof(pTemplate->nNumVectors));
    j+=sizeof(pTemplate->nNumVectors);
    memcpy(&pD[j],&pTemplate->nVectorWidth,sizeof(pTemplate->nVectorWidth));
    j+=sizeof(pTemplate->nVectorWidth);
    memcpy(&pD[j],&pTemplate->nVectorHeight,sizeof(pTemplate->nVectorHeight));
    j+=sizeof(pTemplate->nVectorHeight);
    memcpy(&pD[j],pTemplate->pVectors,pTemplate->nDataSize);

    *ppData=pD;
    *piLen=nSize;

    return result;
}
/*==========================================================================
	Function:
		- fvcUnPack
	Purpose:
		- Unpack template from  byte array
	Arguments:
	Return values:
============================================================================*/
int fvcUnPack(char* pData,int iLen, FvcTemplate** pTemplate)
{
    int result = FVC_OK;	// result of this
	int iWidth,iHeight;
    int iNumVectors = 0;
    int iSize = 0;
    int iDataSize =0;
    int j=0;

    if (pData == NULL || pTemplate == NULL || iLen < sizeof (FvcTemplate))
        return FVC_INVALIDARG;


    //copy data in val
    memcpy(&iSize,&pData[j],sizeof(iSize));
    if (iSize != sizeof(FvcTemplate))
        return FVC_INVALIDARG;
    j+=sizeof(iSize);
    memcpy(&iDataSize,&pData[j],sizeof(iDataSize));
    j+=sizeof(iDataSize);
    memcpy(&iNumVectors,&pData[j],sizeof(iNumVectors));
    j+=sizeof(iNumVectors);
    memcpy(&iWidth,&pData[j],sizeof(iWidth));
    j+=sizeof(iWidth);
    memcpy(&iHeight,&pData[j],sizeof(iHeight));
    j+=sizeof(iHeight);

    /*провека размеров шаблона*/
   // if (iDataSize + iSize != iLen)
   //     return FVC_INVALIDARG;

    FvcTemplate* pTempl=NULL;

    //create template
    if( iDataSize == (iNumVectors*(iWidth*iHeight+1)-1)*sizeof(float) )
        result=fvcCreateTemplate(&pTempl,iWidth,iHeight,iNumVectors-1, true);
    else
        result=fvcCreateTemplate(&pTempl,iWidth,iHeight,iNumVectors-1);

    if(result!=FVC_OK)
    {
        return result;
    }

    //copy data to template
    memcpy(pTempl->pVectors,&pData[j],iDataSize);

    *pTemplate=pTempl;

    return result;
}
/*==========================================================================
	Function:
		- fvcSaveTemplate
	Purpose:
		- Save template to file
	Arguments:
	Return values:
============================================================================*/
int fvcSaveTemplate(const char* pFileName, FvcTemplate* pTemplate)
{
   int result = FVC_OK;	// result of this
   FILE* f = NULL;
   FileHeader header;
   if (pFileName == NULL || pTemplate == NULL)
        return FVC_INVALIDARG;

   result = _fvcCheckTemplate(pTemplate);
   if (result != FVC_OK)
        return result;

	f = fopen(pFileName, "w+b");
	if (f == NULL)
		return FVC_CANNOTSAVE;
	// prepare header
	header.unMagic =  c_nMagic;
	header.ucMajor = 1;
	header.ucMinor = 0;
	// write header
	if (fwrite(&header.unMagic, sizeof(unsigned int), 1, f) != 1)
	{
		fclose(f);
		return FVC_CANNOTSAVE;
	}

	if (fwrite(&header.ucMajor, sizeof(unsigned char), 1, f) != 1)
	{
		fclose(f);
		return FVC_CANNOTSAVE;
	}

	if (fwrite(&header.ucMinor, sizeof(unsigned char), 1, f) != 1)
	{
		fclose(f);
		return FVC_CANNOTSAVE;
	}
	// save template heaeder
	if (fwrite(&pTemplate->nSize, sizeof(int), 1, f) != 1)
	{
		fclose(f);
		return FVC_CANNOTSAVE;
	}

	if (fwrite(&pTemplate->nDataSize, sizeof(int), 1, f) != 1)
	{
		fclose(f);
		return FVC_CANNOTSAVE;
	}

	if (fwrite(&pTemplate->nNumVectors, sizeof(int), 1, f) != 1)
	{
		fclose(f);
		return FVC_CANNOTSAVE;
	}

	if (fwrite(&pTemplate->nVectorWidth, sizeof(int), 1, f) != 1)
	{
		fclose(f);
		return FVC_CANNOTSAVE;
	}

	if (fwrite(&pTemplate->nVectorHeight, sizeof(int), 1, f) != 1)
	{
		fclose(f);
		return FVC_CANNOTSAVE;
	}
	// save data
	if (fwrite((unsigned char*)pTemplate->pVectors, pTemplate->nDataSize, 1, f) != 1)
	{
		fclose(f);
		return FVC_CANNOTSAVE;
	}

	fclose(f);
	return FVC_OK;
}
/*==========================================================================
	Function:
		- fvcLoadTemplate
	Purpose:
		- Loads template from file
	Arguments:
	Return values:
============================================================================*/
int fvcLoadTemplate(const char* pFileName, FvcTemplate** ppTemplate)
{
	//
   FILE* f = NULL;
   *ppTemplate = NULL;
   if (pFileName == NULL || ppTemplate == NULL)
        return FVC_INVALIDARG;
	f = fopen(pFileName, "r+b");
	if(f == NULL)
		return FVC_CANNOTLOAD;
	// read header
	FileHeader header;
	if (fread(&header.unMagic, sizeof(unsigned int), 1, f) != 1)
	{
		fclose(f);
		return FVC_CANNOTLOAD;
	}
	if (header.unMagic != c_nMagic)
	{
		fclose(f);
		return FVC_CANNOTLOAD;
	}
	if (fread(&header.ucMajor, sizeof(unsigned char), 1, f) != 1)
	{
		fclose(f);
		return FVC_CANNOTLOAD;
	}
	if (header.ucMajor != 1)
	{
		fclose(f);
		return FVC_CANNOTLOAD;
	}
	if (fread(&header.ucMinor, sizeof(unsigned char), 1, f) != 1)
	{
		fclose(f);
		return FVC_CANNOTLOAD;
	}

	if (header.ucMinor != 0)
	{
		fclose(f);
		return FVC_CANNOTLOAD;
	}
	// read template header
	int size = 0;
	if (fread(&size, sizeof(int), 1, f) != 1)
	{
		fclose(f);
		return FVC_CANNOTLOAD;
	}
	if (size != sizeof(FvcTemplate))
	{
		fclose(f);
		return FVC_CANNOTLOAD;
	}
	int DataSize = 0;
	if (fread(&DataSize, sizeof(int), 1, f) != 1)
	{
		fclose(f);
		return FVC_CANNOTLOAD;
	}
	int NumVectors = 0;
	if (fread(&NumVectors, sizeof(int), 1, f) != 1)
	{
		fclose(f);
		return FVC_CANNOTLOAD;
	}
	int VectorWidth = 0;
	if (fread(&VectorWidth, sizeof(int), 1, f) != 1)
	{
		fclose(f);
		return FVC_CANNOTLOAD;
	}
	int VectorHeight = 0;
	if (fread(&VectorHeight, sizeof(int), 1, f) != 1)
	{
		fclose(f);
		return FVC_CANNOTLOAD;
	}
	// Create template
    if( DataSize == (NumVectors*(VectorWidth*VectorHeight+1) - 1)*sizeof(float) )
    {
	    if (fvcCreateTemplate(ppTemplate, VectorWidth, VectorHeight, NumVectors-1, true) != FVC_OK)
	    {
		    fclose(f);
		    return FVC_CANNOTLOAD;
	    }
    }
    else
    {
        if (fvcCreateTemplate(ppTemplate, VectorWidth, VectorHeight, NumVectors-1) != FVC_OK)
	    {
		    fclose(f);
			return FVC_CANNOTLOAD;
	    }
    }
	if (DataSize != (*ppTemplate)->nDataSize)
	{
		fclose(f);
		fvcFreeTemplate(ppTemplate);
		*ppTemplate = NULL;
		return FVC_CANNOTLOAD;
	}
	// Fill template with data
	if (fread((unsigned char*)((*ppTemplate)->pVectors), (*ppTemplate)->nDataSize, 1, f) != 1)
	{
		fclose(f);
		fvcFreeTemplate(ppTemplate);
		*ppTemplate = NULL;
		return FVC_CANNOTLOAD;
	}
    fclose(f);
	return FVC_OK;
}

int fvcConvertTemplateToImage(FvcTemplate* pTemplate, awpImage** ppImage)
{
    // variables
    AWPBYTE* src;				// pointers to pixels
	AWPBYTE* dst;
    int n;
    AWPWORD i,w,h;				// helpers
    AWPWORD ht,wt,j,k;			// helpers too
    int res = AWP_OK;		// result of awpipl's calls
    int result = FVC_OK;	// result of this
    awpImage* pTile = NULL;	// tiled image
    awpImage* pGray = NULL;	// temprary grayscale
    awpImage** pObjects = NULL;	// array of eigen objects
    awpImage* pAverage = NULL;	// average object

    //  arguments
    if (pTemplate == NULL)
        return FVC_INVALIDARG;

    result = _fvcCheckTemplate(pTemplate);
    if (result != FVC_OK)
        return result;

    w = pTemplate->nVectorWidth;
    h = pTemplate->nVectorHeight;
    n = pTemplate->nNumVectors - 1;

    // alloc memory for all objects

    res = awpCreateImage(&pAverage, w, h, 1, AWP_FLOAT);
	if (res != AWP_OK)
        return FVC_OUTOFMEMORY;

    pObjects = (awpImage**)malloc((pTemplate->nNumVectors - 1)*sizeof(awpImage*));
    memset(pObjects, 0, (pTemplate->nNumVectors - 1)*sizeof(awpImage*));
    if (pObjects == NULL)
    {
        result = FVC_OUTOFMEMORY;
        goto cleanup;
    }

    for (i = 0; i < n; i++)
    {
        res = awpCreateImage(&pObjects[i], w,h, 1, AWP_FLOAT);
        if (res != AWP_OK)
        {
            result = FVC_OUTOFMEMORY;
            goto cleanup;
        }
    }

    // convert template to awp images
    result = _fvcTemplate2Images(pTemplate, pAverage, pObjects, &n);
    if (result != FVC_OK)
        goto cleanup;

	// create tiled image
    ht = h;
    wt = w*(n+1);
    res = awpCreateImage(&pTile, wt, ht, 1, AWP_BYTE);
    if (res != AWP_OK)
    {
        result = FVC_OUTOFMEMORY;
        goto cleanup;
    }
    // create temporary grayscale image
    res = awpCreateImage(&pGray, w,h, 1, AWP_BYTE);
    if (res != AWP_OK)
    {
        result = FVC_OUTOFMEMORY;
        goto cleanup;
    }


    _fvcConvertFloatAwp2GrayscaleAwp(pAverage, pGray);
    // save to tiled image average
    src = (AWPBYTE*)pGray->pPixels;
    dst = (AWPBYTE*)pTile->pPixels;
    for (i = 0; i < h; i++)
    {
        for (j = 0; j < w; j++)
        {
			dst[i*wt + j] = src[i*pGray->sSizeX + j];
        }
    }
    // save all eigen objects to tiled image
    for (k = 0; k < n; k++)
    {
        _fvcConvertFloatAwp2GrayscaleAwp(pObjects[k], pGray);
        src = (AWPBYTE*)pGray->pPixels;
        dst = (AWPBYTE*)pTile->pPixels;
        for (i = 0; i < h; i++)
        {
            for (j = 0; j < w; j++)
            {
                dst[i*wt + (k+1)*pGray->sSizeX + j] = src[i*pGray->sSizeX + j];
            }
        }
    }

    *ppImage = pTile;
cleanup:
    if (pGray != NULL)
        awpReleaseImage(&pGray);
    if (pAverage != NULL)
        awpReleaseImage(&pAverage);
    if (pObjects != NULL)
    {
		for (i = 0; i < n; i++)
        {
            if (pObjects[i] != NULL)
                awpReleaseImage(&pObjects[i]);
        }
        free (pObjects);
    }
    return result;
}

int fvcResampleTemplate(FvcTemplate* pTemplate, int NumVectors)
{
    int Result = FVC_OK;
    int Size = 0; // новый размер собственных векторов
    float* pBuffer = NULL; //новый буфер для собственных векторов
    float* ptr = NULL;
    float* ptrSrc = NULL;
    /*проверим шаблон*/
    Result = _fvcCheckTemplate(pTemplate);
    if (Result != FVC_OK)
        return Result;
    /*проверим действительно ли новое число собственных векторов меньше
    их числа в шаблоне*/
    if (pTemplate->nNumVectors < NumVectors+1)
        return FVC_INVALIDARG;
    /*скопируем первые NumVectors векторов в буфер*/
	if( pTemplate->nDataSize == (pTemplate->nNumVectors*(pTemplate->nVectorWidth*pTemplate->nVectorHeight+1)-1)*sizeof(float) )
        Size = ((NumVectors+1)*(pTemplate->nVectorWidth*pTemplate->nVectorHeight + 1)-1)*sizeof(float);
    else
        Size = (NumVectors+1)*pTemplate->nVectorWidth*pTemplate->nVectorHeight*sizeof(float);
    pBuffer = (float*)malloc(Size);
    if (pBuffer == NULL)
        return FVC_OUTOFMEMORY;
    /*скопируем вектора*/
    if( pTemplate->nDataSize == (pTemplate->nNumVectors*(pTemplate->nVectorWidth*pTemplate->nVectorHeight+1)-1)*sizeof(float) )
        memcpy((void*)pBuffer, (void*)pTemplate->pVectors, Size-(pTemplate->nVectorWidth*pTemplate->nVectorHeight + NumVectors )*sizeof(float));
    else
        memcpy((void*)pBuffer, (void*)pTemplate->pVectors, Size-pTemplate->nVectorWidth*pTemplate->nVectorHeight*sizeof(float));
    /*скопируем среднее изображение*/
    ptr = pBuffer + NumVectors*pTemplate->nVectorWidth*pTemplate->nVectorHeight;

    ptrSrc = pTemplate->pVectors + pTemplate->nVectorWidth*pTemplate->nVectorHeight*(pTemplate->nNumVectors - 1);

    memcpy((void*)ptr,(void*)ptrSrc, pTemplate->nVectorWidth*pTemplate->nVectorHeight*sizeof(float));
    /*скопируем собственные значения, если они есть в шаблоне*/
    if( pTemplate->nDataSize == (pTemplate->nNumVectors*(pTemplate->nVectorWidth*pTemplate->nVectorHeight+1)-1)*sizeof(float) )
    {
        ptr += pTemplate->nVectorWidth*pTemplate->nVectorHeight;
        ptrSrc += pTemplate->nVectorWidth*pTemplate->nVectorHeight;
        memcpy((void*)ptr,(void*)ptrSrc, NumVectors*sizeof(float));
    }

	/*преобразуем шаблон*/
    pTemplate->nDataSize = Size;

    pTemplate->nNumVectors = NumVectors+1;

    free(pTemplate->pVectors);
    pTemplate->pVectors = pBuffer;
    return Result;
}

int fvcGetReconstruction(FvcTemplate* pTemplate, awpImage* pImage, awpImage** ppReconstrunction)
{
    int Result = FVC_OK;
    float* DecompCoeff = NULL;
    int result,res,i,w,h,j;
    float* fdata;
    SClassInfo class_info;
    if (pImage == NULL)
        return FVC_INVALIDARG;
    result = _fvcCheckTemplate(pTemplate);
    if (result != FVC_OK)
        return result;
    if (pImage->sSizeX != pTemplate->nVectorWidth || pImage->sSizeY != pTemplate->nVectorHeight)
        return FVC_INVALIDARG;
    // prepare eigen vectors and average object
    class_info.m_nCount		= pTemplate->nNumVectors-1;
	class_info.m_pAverage	= NULL;
    class_info.m_pVectors	= NULL;

    w = pTemplate->nVectorWidth;
    h = pTemplate->nVectorHeight;

    res = awpCreateImage(&class_info.m_pAverage, (AWPWORD)w, (AWPWORD)h, 1, AWP_FLOAT);
    if (res != AWP_OK)
        return FVC_OUTOFMEMORY;

    class_info.m_pVectors = (awpImage**)malloc(class_info.m_nCount*sizeof(awpImage*));
    if (class_info.m_pVectors == NULL)
    {
        awpReleaseImage(&class_info.m_pAverage);
        return FVC_OUTOFMEMORY;
    }


    for (i = 0; i < class_info.m_nCount; i++)
    {
        // create eigen vector
        res = awpCreateImage(&class_info.m_pVectors[i], (AWPWORD)w, (AWPWORD)h, 1, AWP_FLOAT);
        if (res != AWP_OK)
        {
            // cleanup
            for (j = 0; j < i; j++)
				awpReleaseImage(&class_info.m_pVectors[j]);

            awpReleaseImage(&class_info.m_pAverage);
            free(class_info.m_pVectors);
            return FVC_OUTOFMEMORY;
        }
        // copy eigen vector
        fdata = pTemplate->pVectors + i*w*h;
        memcpy(class_info.m_pVectors[i]->pPixels, (AWPBYTE*)fdata, w*h*sizeof(float));
    }
    fdata = pTemplate->pVectors + (pTemplate->nNumVectors-1)*w*h;
    memcpy(class_info.m_pAverage->pPixels, (AWPBYTE*)fdata, w*h*sizeof(float));

    DecompCoeff = (float*)malloc(class_info.m_nCount*sizeof(float));
    // we shall find coef. projection of the initial image on
    // space of own vectors
    // differently we shall spread out the initial image
    awpEigenDecomposite(pImage, class_info.m_nCount,class_info.m_pVectors,class_info.m_pAverage,DecompCoeff);
    awpCreateImage(ppReconstrunction,pImage->sSizeX, pImage->sSizeY, 1, AWP_FLOAT);


	//awpEigenProjection(class_info.m_nCount,class_info.m_pVectors,DecompCoeff,class_info.m_pAverage,*ppReconstrunction);
	awpEigenProjectionFloat(class_info.m_nCount,class_info.m_pVectors,DecompCoeff,class_info.m_pAverage,*ppReconstrunction);

	free(DecompCoeff);
    // cleanup
	awpReleaseImage(&class_info.m_pAverage);
    for (j = 0; j < class_info.m_nCount; j++)
        awpReleaseImage(&class_info.m_pVectors[j]);
    free(class_info.m_pVectors);
    return Result;
}

int fvcGetDecompositionCoeff(FvcTemplate* pTemplate, awpImage* pImage,
float* pCoeff)
{
    int Result = FVC_OK;
    float* DecompCoeff = NULL;
    int result,res,i,w,h,j;
    float* fdata;
    SClassInfo class_info;
    if (pImage == NULL)
        return FVC_INVALIDARG;
    result = _fvcCheckTemplate(pTemplate);
    if (result != FVC_OK)
        return result;
    if (pImage->sSizeX != pTemplate->nVectorWidth || pImage->sSizeY != pTemplate->nVectorHeight)
        return FVC_INVALIDARG;
    // prepare eigen vectors and average object
    class_info.m_nCount		= pTemplate->nNumVectors-1;
    class_info.m_pAverage	= NULL;
    class_info.m_pVectors	= NULL;

    w = pTemplate->nVectorWidth;
    h = pTemplate->nVectorHeight;

    res = awpCreateImage(&class_info.m_pAverage, (AWPWORD)w, (AWPWORD)h, 1, AWP_FLOAT);
    if (res != AWP_OK)
        return FVC_OUTOFMEMORY;

    class_info.m_pVectors = (awpImage**)malloc(class_info.m_nCount*sizeof(awpImage*));
    if (class_info.m_pVectors == NULL)
    {
        awpReleaseImage(&class_info.m_pAverage);
        return FVC_OUTOFMEMORY;
    }


    for (i = 0; i < class_info.m_nCount; i++)
    {
        // create eigen vector
        res = awpCreateImage(&class_info.m_pVectors[i], (AWPWORD)w, (AWPWORD)h, 1, AWP_FLOAT);
        if (res != AWP_OK)
        {
            // cleanup
            for (j = 0; j < i; j++)
                awpReleaseImage(&class_info.m_pVectors[j]);

			awpReleaseImage(&class_info.m_pAverage);
            free(class_info.m_pVectors);
            return FVC_OUTOFMEMORY;
        }
        // copy eigen vector
        fdata = pTemplate->pVectors + i*w*h;
        memcpy(class_info.m_pVectors[i]->pPixels, (AWPBYTE*)fdata, w*h*sizeof(float));
    }
    fdata = pTemplate->pVectors + (pTemplate->nNumVectors-1)*w*h;
    memcpy(class_info.m_pAverage->pPixels, (AWPBYTE*)fdata, w*h*sizeof(float));

//    class_info.m_eigVals = (float*)malloc(class_info.m_nCount*sizeof(float));
//    fdata += w*h;
//    memcpy(class_info.m_eigVals, (BYTE*)fdata, class_info.m_nCount*sizeof(float));

    DecompCoeff = (float*)malloc(class_info.m_nCount*sizeof(float));
    // we shall find coef. projection of the initial image on
    // space of own vectors
    // differently we shall spread out the initial image
    awpEigenDecomposite(pImage, class_info.m_nCount,class_info.m_pVectors,class_info.m_pAverage,DecompCoeff);
    memcpy(pCoeff, DecompCoeff, class_info.m_nCount*sizeof(float));
//    for( i = 0; i < class_info.m_nCount; i++ )
//        pCoeff[i] *= (class_info.m_eigVals)[i];
    free(DecompCoeff);
    // cleanup
    awpReleaseImage(&class_info.m_pAverage);
	for (j = 0; j < class_info.m_nCount; j++)
        awpReleaseImage(&class_info.m_pVectors[j]);
    free(class_info.m_pVectors);
    return Result;
}

/**
*	\brief Convert template to SClassInfo
*	\param pTemplate pointer to template
*	\param pInfo pointer to SClassInfo
*	\return FVC_OK if success, error code in other case
*/
int fvcTemplateToClassInfo(FvcTemplate* pTemplate, SClassInfo* pInfo)
{
    int w,h, i, j;
    float* fdata;
    int Result = FVC_OK;
    Result = _fvcCheckTemplate(pTemplate);
    if (Result != FVC_OK)
      return Result;

    w = pTemplate->nVectorWidth;
    h = pTemplate->nVectorHeight;

    pInfo->m_nCount		= pTemplate->nNumVectors-1;

	pInfo->m_pAverage	= NULL;
    pInfo->m_pVectors	= NULL;

    if (awpCreateImage(&pInfo->m_pAverage, (AWPWORD)w, (AWPWORD)h, 1, AWP_FLOAT) != AWP_OK)
        return FVC_OUTOFMEMORY;

    pInfo->m_pVectors = (awpImage**)malloc(pInfo->m_nCount*sizeof(awpImage*));
    if (pInfo->m_pVectors == NULL)
    {
        awpReleaseImage(&pInfo->m_pAverage);
        return FVC_OUTOFMEMORY;
    }

    for (i = 0; i < pInfo->m_nCount; i++)
    {
        if (awpCreateImage(&pInfo->m_pVectors[i], (AWPWORD)w, (AWPWORD)h, 1, AWP_FLOAT)!= AWP_OK)
        {
            // cleanup
            for (j = 0; j < i; j++)
                awpReleaseImage(&pInfo->m_pVectors[j]);

            awpReleaseImage(&pInfo->m_pAverage);
            free(pInfo->m_pVectors);
            return FVC_OUTOFMEMORY;
        }
        // copy eigen vector
		fdata = pTemplate->pVectors + i*w*h;
        memcpy(pInfo->m_pVectors[i]->pPixels, (AWPBYTE*)fdata, w*h*sizeof(float));
    }
    fdata = pTemplate->pVectors + (pTemplate->nNumVectors-1)*w*h;
    memcpy(pInfo->m_pAverage->pPixels, (AWPBYTE*)fdata, w*h*sizeof(float));

    if( pTemplate->nDataSize == (pTemplate->nNumVectors*(w*h+1)-1)*sizeof(float) )
    {
        fdata += w*h;
        pInfo->m_eigVals = (float*)malloc(pInfo->m_nCount*sizeof(float));
        memcpy( pInfo->m_eigVals, (AWPBYTE*)fdata, pInfo->m_nCount*sizeof(float) );
    }
    else
        pInfo->m_eigVals = NULL;

    return Result;
}

int fvcTemplateToClassInfoFloat(FvcTemplate* pTemplate, SClassInfo* pInfo)
{
    int w,h, i, j;
    float* fdata;
    int Result = FVC_OK;
    Result = _fvcCheckTemplate(pTemplate);
    if (Result != FVC_OK)
      return Result;

    w = pTemplate->nVectorWidth;
    h = pTemplate->nVectorHeight;

	pInfo->m_nCount		= pTemplate->nNumVectors-1;

    pInfo->m_pAverage	= NULL;
    pInfo->m_pVectors	= NULL;

    if (awpCreateImage(&pInfo->m_pAverage, (AWPWORD)w, (AWPWORD)h, 1, AWP_FLOAT) != AWP_OK)
        return FVC_OUTOFMEMORY;

    pInfo->m_pVectors = (awpImage**)malloc(pInfo->m_nCount*sizeof(awpImage*));
    if (pInfo->m_pVectors == NULL)
    {
        awpReleaseImage(&pInfo->m_pAverage);
        return FVC_OUTOFMEMORY;
    }

    for (i = 0; i < pInfo->m_nCount; i++)
    {
        if (awpCreateImage(&pInfo->m_pVectors[i], (AWPWORD)w, (AWPWORD)h, 1, AWP_FLOAT)!= AWP_OK)
        {
            // cleanup
            for (j = 0; j < i; j++)
                awpReleaseImage(&pInfo->m_pVectors[j]);

            awpReleaseImage(&pInfo->m_pAverage);
            free(pInfo->m_pVectors);
            return FVC_OUTOFMEMORY;
		}
        // copy eigen vector
        fdata = pTemplate->pVectors + i*w*h;
        memcpy(pInfo->m_pVectors[i]->pPixels, fdata, w*h*sizeof(float));
    }
    fdata = pTemplate->pVectors + (pTemplate->nNumVectors-1)*w*h;
    memcpy(pInfo->m_pAverage->pPixels, fdata, w*h*sizeof(float));

    if( pTemplate->nDataSize == (pTemplate->nNumVectors*(w*h+1)-1)*sizeof(float) )
    {
        fdata += w*h;
        pInfo->m_eigVals = (float*)malloc(pInfo->m_nCount*sizeof(float));
        memcpy( pInfo->m_eigVals, (AWPBYTE*)fdata, pInfo->m_nCount*sizeof(float) );
    }

    return Result;
}

/**
*	\brief Convert SClassInfo to Template
*	\param ppTemplate pointer to template
*	\param pInfo pointer to SClassInfo
*	\return FVC_OK if success, error code in other case
*/
int fvcClassInfoToTemplate(FvcTemplate** ppTemplate, SClassInfo* pInfo)
{
	  int Result = FVC_OK;
      return Result;
}

int fvcFreeClassInfo(SClassInfo* pInfo)
{
  int Result = FVC_OK;
  if (pInfo == NULL)
    FVC_INVALIDARG;

  for (int i = 0; i < pInfo->m_nCount; i++)
  {
      if (pInfo->m_pVectors[i] != NULL)
          awpReleaseImage(&pInfo->m_pVectors[i]);
  }
  free(pInfo->m_pVectors);
  pInfo->m_pVectors = NULL;
  pInfo->m_nCount = 0;
  if (pInfo->m_pAverage != NULL)
      awpReleaseImage(&pInfo->m_pAverage);
  pInfo->m_pAverage = NULL;
  if( pInfo->m_eigVals!=NULL )
	free( pInfo->m_eigVals );
  return Result;
}
/*
	строит рекострукцию изображения по коэф. разложения
    количество коэф. в массиве pCoeff должно быть точно
    равно количеству векторов в pTemplate
*/
int fvcBuildReconstruction(FvcTemplate* pTemplate, float* pCoeff, awpImage** ppReconstruction)
{
    int Result = FVC_OK;
    Result = _fvcCheckTemplate(pTemplate);

    if (Result != FVC_OK)
      return Result;

    if (awpCreateImage(ppReconstruction,pTemplate->nVectorWidth, pTemplate->nVectorHeight, 1, AWP_BYTE) != AWP_OK)
        return FVC_INVALIDARG;

    SClassInfo class_info;

    Result = fvcTemplateToClassInfo(pTemplate, &class_info);
    if(Result!= FVC_OK)
        return Result;

    Result = awpEigenProjection(class_info.m_nCount, class_info.m_pVectors, pCoeff, class_info.m_pAverage, *ppReconstruction);
	if(Result!= AWP_OK)
        return Result;
    fvcFreeClassInfo (&class_info);

	return Result;
}

int fvcBuildReconstructionFloat(FvcTemplate* pTemplate, float* pCoeff, awpImage** ppReconstruction)
{
    int Result = FVC_OK;
    Result = _fvcCheckTemplate(pTemplate);

    if (Result != FVC_OK)
      return Result;

    if (awpCreateImage(ppReconstruction,pTemplate->nVectorWidth, pTemplate->nVectorHeight, 1, AWP_FLOAT) != AWP_OK)
        return FVC_INVALIDARG;

    SClassInfo class_info;

    Result = fvcTemplateToClassInfoFloat(pTemplate, &class_info);
    if(Result!= FVC_OK)
        return Result;

    Result = awpEigenProjectionFloat(class_info.m_nCount, class_info.m_pVectors, pCoeff, class_info.m_pAverage, *ppReconstruction);
    if(Result!= AWP_OK)
		return Result;
    fvcFreeClassInfo (&class_info);

    return Result;
}
