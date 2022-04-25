#include "fvlcore.h"
#include "awpeigenlib.h"
//#ifdef __cplusplus
//extern "C" {
//#endif
//#include <awpipl.h>
////#include <_awpcommon.h>
//#ifdef __cplusplus
//}
//#endif
//#include <fstream>
#include <stdlib.h>
#include <math.h>
//#include "fvlcore.h"
//#include "SeparationMatrix.h"

__inline bool operator == (AwpSize size1, AwpSize size2 )
{
    return size1.width == size2.width && size1.height == size2.height;
}

__inline bool operator != (AwpSize size1, AwpSize size2 )
{
    return size1.width != size2.width || size1.height != size2.height;
}


__inline static int _awpRound( double val )
{
    static const int round_delta = 0x59C00000;
    double temp = (val + 1e-7) + *((float*)&round_delta);
    return *((int*)&temp);
}


static int _awpEigenProjection(int    nEigObjs,				//
                               void*  eigInput,
                               int    eigStep,
                               float* coeffs,
                               float* avg,
                               int    avgStep,
                               AWPBYTE* rest,
                               int    restStep,
                               AwpSize size)
{
    int i, j, k;
    float* buf;
    //    float* buffer = NULL;
    float* b;

    if ( size.width > avgStep || 4*size.width > eigStep || size.height < 1)
        return -1;
    if ( rest == NULL || eigInput == NULL || avg == NULL || coeffs == NULL )
        return -1;
    for( i=0; i<nEigObjs; i++ ) if( ((AWPBYTE**)eigInput)[i] == NULL )
            return -1;
    eigStep /= 4;
    avgStep /= 4;

    if( size.width == restStep && size.width == eigStep && size.width == avgStep )
    {
        size.width *= size.height;
        size.height = 1;
        restStep = eigStep = avgStep = size.width;
    }

    buf = (float*)malloc( sizeof(float)*size.width*size.height );
    if( buf == NULL ) return -2;
    b = buf;
    for( i=0; i<size.height; i++, avg+=avgStep, b+=size.width )
        for( j=0; j<size.width; j++ )
            b[j] = avg[j];

    for(k=0; k<nEigObjs; k++)
    {
        float* e = ((float**)eigInput)[k];
        float  c = coeffs[k];

        b = buf;
        for( i = 0; i < size.height; i++, e+=eigStep, b+=size.width )
        {
            for(j = 0; j < size.width-3; j+=4)
            {
                float b0 = c*e[j  ];
                float b1 = c*e[j+1];
                float b2 = c*e[j+2];
                float b3 = c*e[j+3];
                b[j  ] += b0;
                b[j+1] += b1;
                b[j+2] += b2;
                b[j+3] += b3;
            }
            for(; j < size.width; j++)
                b[j] += c*e[j];
        }
    }

    float fl_min, fl_max;
    fl_min = buf[0]; fl_max = buf[0];
    for (i = 0; i < size.width*size.height; i++)
    {
        if (fl_max < buf[i]) fl_max = buf[i];
        if (fl_min > buf[i]) fl_min = buf[i];
    }
    if (fl_max == fl_min)
    {
        free(buf);
        return 0;
    }

    b = buf;
    for( i = 0; i < size.height; i++, avg+=avgStep, b+=size.width, rest+=restStep )
        for(j = 0; j < size.width; j++)
        {
            int w = _awpRound( b[j] );
            w = !(w&~255) ? w : w<0 ? 0 : 255;
            rest[j] = w;//255 - (AWPBYTE)(255*(b[j] - fl_min )/ (fl_max - fl_min));
        }

    free(buf );
    return 0;

}

static int _awpEigenProjectionFloat(int    nEigObjs,				//
                               void*  eigInput,
                               int    eigStep,
                               float* coeffs,
                               float* avg,
                               int    avgStep,
                               float* rest,
                               int    restStep,
                               AwpSize size)
{
    int i, j, k;
    float* buf;
    //    float* buffer = NULL;
    float* b;

    if ( size.width > avgStep || size.width > eigStep || size.height < 1)
        return -1;
    if ( rest == NULL || eigInput == NULL || avg == NULL || coeffs == NULL )
        return -1;
    for( i=0; i<nEigObjs; i++ ) if( ((AWPBYTE**)eigInput)[i] == NULL )
            return -1;
//    eigStep /= 4;
//    avgStep /= 4;

    if( size.width == restStep && size.width == eigStep && size.width == avgStep )
    {
        size.width *= size.height;
		size.height = 1;
        restStep = eigStep = avgStep = size.width;
    }

    buf = (float*)malloc( sizeof(float)*size.width*size.height );
    if( buf == NULL ) return -2;
    b = buf;
    for( i=0; i<size.height; i++, avg+=avgStep, b+=size.width )
        for( j=0; j<size.width; j++ )
			b[j] = avg[j];

    for(k=0; k<nEigObjs; k++)
    {
        float* e = ((float**)eigInput)[k];
        float  c = coeffs[k];

        b = buf;
        for( i = 0; i < size.height; i++, e+=eigStep, b+=size.width )
        {
            for(j = 0; j < size.width-3; j+=4)
            {
                float b0 = c*e[j  ];
                float b1 = c*e[j+1];
                float b2 = c*e[j+2];
                float b3 = c*e[j+3];
                b[j  ] += b0;
                b[j+1] += b1;
                b[j+2] += b2;
                b[j+3] += b3;
            }
            for(; j < size.width; j++)
                b[j] += c*e[j];
        }
    }

    float fl_min, fl_max;
    fl_min = buf[0]; fl_max = buf[0];
    for (i = 0; i < size.width*size.height; i++)
    {
        if (fl_max < buf[i]) fl_max = buf[i];
        if (fl_min > buf[i]) fl_min = buf[i];
    }
    if (fl_max == fl_min)
    {
        free(buf);
        return 0;
    }

    b = buf;
    for( i = 0; i < size.height; i++, avg+=avgStep, b+=size.width, rest+=restStep )
        for(j = 0; j < size.width; j++)
        {
            rest[j] = b[j];//255 - (BYTE)(255*(b[j] - fl_min )/ (fl_max - fl_min));
        }

    free(buf );
    return 0;

}

// Eigen projection WARP
int awpEigenProjection(int nNumObjects, void* pEigenVectors, float* pfCoeff, awpImage* pAverage, awpImage* Reconstruction)
{
    // преобразование типов и провека параметров
    if (nNumObjects <= 0 || pEigenVectors == NULL || pfCoeff == NULL || pAverage == NULL || Reconstruction == NULL)
        return -1;
    //  преобразуем среднее в тип awpImage
    // проверим среднее
    AWPRESULT res = AWP_OK;
    res = awpCheckImage(pAverage);
    if (res != AWP_OK)
        return -1;
    // проверим тип дынных для прикселей
    if (pAverage->dwType != AWP_FLOAT)
        return -1;
    // проверка векторов
    AwpSize avg_size, proj_size;
    int avg_step = 0, proj_step = 0;
    float*  avg_data;
    AWPBYTE*  proj_data;
    avg_size.height = pAverage->sSizeY; avg_size.width = pAverage->sSizeX;
    avg_step = pAverage->sSizeX*sizeof(float);
    avg_data = (float*) pAverage->pPixels;


    // reconstruction
    res = awpCheckImage(Reconstruction);
    if (res != AWP_OK)
        return -1;

    if (Reconstruction->dwType != AWP_BYTE)
        return -1;

    proj_size.height = Reconstruction->sSizeY; proj_size.width = Reconstruction->sSizeX;
    proj_step = proj_size.width = Reconstruction->sSizeX*sizeof(AWPBYTE);
    proj_data = (AWPBYTE*) Reconstruction->pPixels;

    if (proj_size != avg_size)
        return -1;

    awpImage** eigens = (awpImage**)pEigenVectors;
    float** eigs = (float**)malloc( sizeof( float*) * nNumObjects );
    int eig_step = 0, old_step = 0;
    AwpSize eig_size = avg_size, old_size = avg_size;

    for (int i = 0; i < nNumObjects; i++)
    {
        awpImage* eig = eigens[i];
        float*  eig_data;

        awpCheckImage(eig);
        eig_data = (float*)eig->pPixels;
        eig_step = eig->sSizeX*sizeof(float);
        eig_size.height = eig->sSizeY; eig_size.width = eig->sSizeX;

        if( eig->dwType != AWP_FLOAT)
            return -1;
        if( eig_size != avg_size || eig_size != old_size )
            return -1;
        if( i > 0 && eig_step != old_step )
            return -1;
        old_step = eig_step;
        old_size = eig_size;
        eigs[i]  = eig_data;
    }

    int r = _awpEigenProjection(nNumObjects, eigs, eig_step,pfCoeff,  avg_data, avg_step,proj_data, proj_step, proj_size);
    free(eigs);
    return r;
}

int awpEigenProjectionFloat(int nNumObjects, void* pEigenVectors, float* pfCoeff, awpImage* pAverage, awpImage* Reconstruction)
{
    // преобразование типов и провека параметров
    if (nNumObjects <= 0 || pEigenVectors == NULL || pfCoeff == NULL || pAverage == NULL || Reconstruction == NULL)
        return -1;
    //  преобразуем среднее в тип awpImage
    // проверим среднее
    AWPRESULT res = AWP_OK;
    res = awpCheckImage(pAverage);
    if (res != AWP_OK)
        return -1;
    // проверим тип дынных для прикселей
    if (pAverage->dwType != AWP_FLOAT)
        return -1;
    // проверка векторов
    AwpSize avg_size, proj_size;
    int avg_step = 0, proj_step = 0;
    float*  avg_data;
    float*  proj_data;
    avg_size.height = pAverage->sSizeY; avg_size.width = pAverage->sSizeX;
    avg_step = pAverage->sSizeX;
    avg_data = (float*) pAverage->pPixels;


    // reconstruction
    res = awpCheckImage(Reconstruction);
    if (res != AWP_OK)
        return -1;

    if (Reconstruction->dwType != AWP_BYTE && Reconstruction->dwType != AWP_FLOAT)
        return -1;

    proj_size.height = Reconstruction->sSizeY; proj_size.width = Reconstruction->sSizeX;
    proj_step = Reconstruction->sSizeX;
    proj_data = (float*) Reconstruction->pPixels;

    if (proj_size != avg_size)
        return -1;

    awpImage** eigens = (awpImage**)pEigenVectors;
    float** eigs = (float**)malloc( sizeof( float*) * nNumObjects );
    int eig_step = 0, old_step = 0;
    AwpSize eig_size = avg_size, old_size = avg_size;

    for (int i = 0; i < nNumObjects; i++)
    {
        awpImage* eig = eigens[i];
        float*  eig_data;

        awpCheckImage(eig);
        eig_data = (float*)eig->pPixels;
        eig_step = eig->sSizeX;
        eig_size.height = eig->sSizeY; eig_size.width = eig->sSizeX;

        if( eig->dwType != AWP_FLOAT)
            return -1;
        if( eig_size != avg_size || eig_size != old_size )
            return -1;
        if( i > 0 && eig_step != old_step )
            return -1;
        old_step = eig_step;
        old_size = eig_size;
        eigs[i]  = eig_data;
    }

    int r = _awpEigenProjectionFloat(nNumObjects, eigs, eig_step,pfCoeff,  avg_data, avg_step,proj_data, proj_step, proj_size);
    free(eigs);
    return r;
}

float _awpCalcDecompCoeffBYTE(AWPBYTE*  obj,
                           int     objStep,
                           float*  eigObj,
                           int     eigStep,
                           float*  avg,
                           int     avgStep,
                           AwpSize size )

{
    int i, k;
    float w = 0.0f;

    if ( size.width > objStep || 4*size.width > eigStep
            || 4*size.width > avgStep || size.height < 1)  return -1.0e30f;
    if ( obj == NULL || eigObj == NULL || avg == NULL ) return -1.0e30f;

    eigStep /= 4;
    avgStep /= 4;

    if( size.width == objStep && size.width == eigStep && size.width == avgStep )
    {
        size.width *= size.height;
        size.height = 1;
        objStep = eigStep = avgStep = size.width;
    }

    for(i = 0; i < size.height; i++, obj += objStep, eigObj += eigStep, avg += avgStep)
    {
        for(k = 0; k < size.width-4; k+=4)
        {
            float o = (float)obj[k];
            float e = eigObj[k];
            float a = avg[k];
            w += e*( o - a );
            o = (float)obj[k+1];
            e = eigObj[k+1];
            a = avg[k+1];
            w += e*( o - a );
            o = (float)obj[k+2];
            e = eigObj[k+2];
            a = avg[k+2];
            w += e*( o - a );
            o = (float)obj[k+3];
            e = eigObj[k+3];
            a = avg[k+3];
            w += e*( o - a );
        }
        for(; k < size.width; k++) w += eigObj[k]*( (float)obj[k] - avg[k] );
    }

    return w;
}

float _awpCalcDecompCoeffFLOAT( float*  obj,
                           int     objStep,
                           float*  eigObj,
                           int     eigStep,
                           float*  avg,
                           int     avgStep,
                           AwpSize size )

{
    int i, k;
    float w = 0.0f;

    if ( size.width > objStep || 4*size.width > eigStep
            || 4*size.width > avgStep || size.height < 1)  return -1.0e30f;
    if ( obj == NULL || eigObj == NULL || avg == NULL ) return -1.0e30f;

    eigStep /= 4;
    avgStep /= 4;

    if( size.width == objStep && size.width == eigStep && size.width == avgStep )
    {
        size.width *= size.height;
        size.height = 1;
        objStep = eigStep = avgStep = size.width;
    }

    for(i = 0; i < size.height; i++, obj += objStep, eigObj += eigStep, avg += avgStep)
    {
        for(k = 0; k < size.width-4; k+=4)
        {
            float o = (float)obj[k];
            float e = eigObj[k];
            float a = avg[k];
            w += e*( o - a );
            o = (float)obj[k+1];
            e = eigObj[k+1];
            a = avg[k+1];
            w += e*( o - a );
            o = (float)obj[k+2];
            e = eigObj[k+2];
            a = avg[k+2];
            w += e*( o - a );
            o = (float)obj[k+3];
            e = eigObj[k+3];
            a = avg[k+3];
            w += e*( o - a );
        }
        for(; k < size.width; k++) w += eigObj[k]*( (float)obj[k] - avg[k] );
    }

    return w;
}

static int _awpEigenDecompositeBYTE(AWPBYTE* obj,
                                int    objStep,
                                int    nEigObjs,
                                void*  eigInput,
                                int    eigStep,
                                float* avg,
                                int    avgStep,
                                AwpSize size,
                                float* coeffs )
{
    int i;

    if ( nEigObjs < 2 )                                    return -1;
    if ( size.width > objStep || 4*size.width > eigStep ||
            4*size.width > avgStep || size.height < 1)        return -1;
    if ( obj == NULL || eigInput == NULL || coeffs == NULL || avg == NULL)
        return -1;
    for( i=0; i<nEigObjs; i++ )
        if( ((AWPBYTE**)eigInput)[i] == NULL )
            return -1;

    for(i=0; i<nEigObjs; i++)
    {
        float w = _awpCalcDecompCoeffBYTE( obj, objStep, ((float**)eigInput)[i],
                                       eigStep, avg, avgStep, size );
        if( w < -1.0e29f ) return -1;
        coeffs[i] = w;
    }

    return 0;
}

static int _awpEigenDecompositeFLOAT(float* obj,
                                int    objStep,
                                int    nEigObjs,
                                void*  eigInput,
                                int    eigStep,
                                float* avg,
                                int    avgStep,
                                AwpSize size,
                                float* coeffs )
{
    int i;

    if ( nEigObjs < 1 )                                    return -1;
    if ( size.width > objStep || 4*size.width > eigStep ||
            4*size.width > avgStep || size.height < 1)        return -1;
    if ( obj == NULL || eigInput == NULL || coeffs == NULL || avg == NULL)
        return -1;
    for( i=0; i<nEigObjs; i++ )
        if( ((AWPBYTE**)eigInput)[i] == NULL )
            return -1;

    for(i=0; i<nEigObjs; i++)
    {
        float w = _awpCalcDecompCoeffFLOAT( obj, objStep, ((float**)eigInput)[i],
                                       eigStep, avg, avgStep, size );
        if( w < -1.0e29f ) return -1;
        coeffs[i] = w;
    }

    return 0;
}

int awpEigenDecompositeFLOAT(awpImage* pImage, int nNumObjects, void* pEigenVectors, awpImage* pAverage, float* pfCoeff)
{
    float*  avg_data;
    float*  obj_data;
    int avg_step = 0, obj_step = 0;
    AwpSize avg_size, obj_size;
    int i;

    AWPRESULT res = AWP_OK;

    res = awpCheckImage(pAverage);
    if (res != AWP_OK)
        return -1;
    if (pAverage->dwType != AWP_FLOAT)
        return -1;

    avg_data = (float*)pAverage->pPixels;
    avg_step = pAverage->sSizeX*sizeof(float);
    avg_size.height = pAverage->sSizeY; avg_size.width = pAverage->sSizeX;

    res = awpCheckImage(pImage);
    if (res != AWP_OK)
        return -1;
    if (pImage->dwType != AWP_FLOAT)
        return -1;

    obj_data = (float*)pImage->pPixels;
    obj_step = pImage->sSizeX*sizeof(AWPBYTE);
    obj_size.width = pImage->sSizeX; obj_size.height= pImage->sSizeY;

    if( obj_size != avg_size )
        return -1;

    awpImage** eigens = (awpImage**)pEigenVectors;
    float** eigs = (float**)malloc( sizeof( float*) * nNumObjects );
    int eig_step = 0, old_step = 0;
    AwpSize eig_size = avg_size, old_size = avg_size;

    if( eigs == NULL )
        return -2;

    for( i=0; i < nNumObjects; i++ )
    {
        awpImage* eig = eigens[i];
        float*  eig_data;
        try
        {
            res = awpCheckImage(eig);
            if (res != AWP_OK) throw 0;
            eig_data = (float*)eig->pPixels;
            eig_step = eig->sSizeX*sizeof(float);
            eig_size.height = eig->sSizeY; eig_size.width = eig->sSizeX;
            if (eig->dwType != AWP_FLOAT) throw 0;
            if( eig_size != avg_size || eig_size != old_size )
                throw 0;
            if( i > 0 && eig_step != old_step )
                throw 0;

            old_step = eig_step;
            old_size = eig_size;
            eigs[i]  = eig_data;
        }
        catch(...)
        {
            free (eigs);
            return -1;
        }

    }
    int r = _awpEigenDecompositeFLOAT(obj_data,obj_step,nNumObjects,(void*)eigs,eig_step,avg_data,avg_step,obj_size,pfCoeff);
    free(eigs);
    return r;
}

int awpEigenDecompositeBYTE(awpImage* pImage, int nNumObjects, void* pEigenVectors, awpImage* pAverage, float* pfCoeff)
{

    float*  avg_data;
    AWPBYTE*  obj_data;
    int avg_step = 0, obj_step = 0;
    AwpSize avg_size, obj_size;
    int i;

    AWPRESULT res = AWP_OK;

    res = awpCheckImage(pAverage);
    if (res != AWP_OK)
        return -1;
    if (pAverage->dwType != AWP_FLOAT)
        return -1;

    avg_data = (float*)pAverage->pPixels;
    avg_step = pAverage->sSizeX*sizeof(float);
    avg_size.height = pAverage->sSizeY; avg_size.width = pAverage->sSizeX;

    res = awpCheckImage(pImage);
    if (res != AWP_OK)
        return -1;
    if (pImage->dwType != AWP_BYTE)
        return -1;

    obj_data = (AWPBYTE*)pImage->pPixels;
    obj_step = pImage->sSizeX*sizeof(AWPBYTE);
    obj_size.width = pImage->sSizeX; obj_size.height= pImage->sSizeY;

    if( obj_size != avg_size )
        return -1;

    awpImage** eigens = (awpImage**)pEigenVectors;
    float** eigs = (float**)malloc( sizeof( float*) * nNumObjects );
    int eig_step = 0, old_step = 0;
    AwpSize eig_size = avg_size, old_size = avg_size;

    if( eigs == NULL )
        return -2;

    for( i=0; i < nNumObjects; i++ )
    {
        awpImage* eig = eigens[i];
        float*  eig_data;
        try
        {
            res = awpCheckImage(eig);
            if (res != AWP_OK) throw 0;
            eig_data = (float*)eig->pPixels;
            eig_step = eig->sSizeX*sizeof(float);
            eig_size.height = eig->sSizeY; eig_size.width = eig->sSizeX;
            if (eig->dwType != AWP_FLOAT) throw 0;
            if( eig_size != avg_size || eig_size != old_size )
                throw 0;
            if( i > 0 && eig_step != old_step )
                throw 0;

            old_step = eig_step;
            old_size = eig_size;
            eigs[i]  = eig_data;
        }
        catch(...)
        {
            free (eigs);
            return -1;
        }

    }
    int r = _awpEigenDecompositeBYTE(obj_data,obj_step,nNumObjects,(void*)eigs,eig_step,avg_data,avg_step,obj_size,pfCoeff);
    free(eigs);
    return r;
}



int awpEigenDecomposite(awpImage* pImage, int nNumObjects, void* pEigenVectors, awpImage* pAverage, float* pfCoeff)
{

//    float*  avg_data;
    //AWPBYTE*  obj_data;
    //int avg_step = 0, obj_step = 0;
//    AwpSize /*avg_size,*/ obj_size;
    AWPRESULT res = AWP_OK;

    res = awpCheckImage(pAverage);
    if (res != AWP_OK)
        return -1;
    if (pImage->dwType == AWP_FLOAT)
        return awpEigenDecompositeFLOAT(pImage, nNumObjects, pEigenVectors, pAverage, pfCoeff);
    else if (pImage->dwType == AWP_BYTE)
        return awpEigenDecompositeBYTE(pImage, nNumObjects, pEigenVectors, pAverage, pfCoeff);
    else
        return -1;
}

static int _awpCalcCovarMatrixEx(int    nObjects,
                                 void*  input,
                                 int    objStep1,
                                 float* avg,
                                 int    avgStep,
                                 AwpSize size,
                                 float* covarMatrix)
{
    int objStep = objStep1;


    if ( nObjects < 2 )                                        return -1;
    if ( input == NULL || avg == NULL || covarMatrix == NULL ) return -1;
    if ( size.width > objStep || 4*size.width > avgStep || size.height < 1)
        return -1;

    avgStep /= 4;

    int i, j;
    AWPBYTE** objects = (AWPBYTE**)input;    // !!!!!!!!!

    for(i=0; i<nObjects; i++)
    {
        AWPBYTE* bu = objects[i];   // !!!!!!!
        for(j=i; j<nObjects; j++)
        {
            int      k, l;
            float    w = 0.f;
            float*   a = avg;
            AWPBYTE* bu1 = bu;
            AWPBYTE* bu2 = objects[j]; // !!!!!!

            for(k=0; k<size.height; k++, bu1 += objStep, bu2 += objStep, a += avgStep)
            {
                for(l = 0; l < size.width-3; l+=4)
                {
                    float  f = a  [l];
                    AWPBYTE u1 = bu1[l];
                    AWPBYTE u2 = bu2[l];
                    w += ( u1 - f ) * ( u2 - f );
                    f  = a  [l+1];
                    u1 = bu1[l+1];
                    u2 = bu2[l+1];
                    w += ( u1 - f ) * ( u2 - f );
                    f  = a  [l+2];
                    u1 = bu1[l+2];
                    u2 = bu2[l+2];
                    w += ( u1 - f ) * ( u2 - f );
                    f  = a  [l+3];
                    u1 = bu1[l+3];
                    u2 = bu2[l+3];
                    w += ( u1 - f ) * ( u2 - f );
                }
                for(; l < size.width; l++)
                {
                    float  f = a  [l];
                    AWPBYTE u1 = bu1[l];
                    AWPBYTE u2 = bu2[l];
                    w += ( u1 - f ) * ( u2 - f );
                }
            }

            covarMatrix[i*nObjects + j] = covarMatrix[j*nObjects + i] = w;
        }
    }

    return 0;
}

static int _awpCalcCovarMatrixExFloat(int    nObjects,
                                 void*  input,
                                 int    objStep1,
                                 float* avg,
                                 int    avgStep,
                                 AwpSize size,
                                 float* covarMatrix)
{
    int objStep = objStep1;


    if ( nObjects < 2 )                                        return -1;
    if ( input == NULL || avg == NULL || covarMatrix == NULL ) return -1;
    if ( size.width > objStep || 4*size.width > avgStep || size.height < 1)
        return -1;

    avgStep /= 4;

    int i, j;
    float** objects = (float**)input;
  //  std::ofstream matrix( "matrix.txt", std::ios_base::app );
	for(i=0; i<nObjects; i++)
	{
		float* bu = objects[i];
		for(j=i; j<nObjects; j++)
		{
			int      k, l;
			float    w = 0.f;
			float*   a = avg;
			float* bu1 = bu;
			float* bu2 = objects[j];

			for(k=0; k<size.height; k++, bu1 += objStep, bu2 += objStep, a += avgStep)
			{
				for(l = 0; l < size.width-3; l+=4)
				{
					float  f = a  [l];
					float u1 = bu1[l];
					float u2 = bu2[l];
					w += ( u1 - f ) * ( u2 - f );
					f  = a  [l+1];
					u1 = bu1[l+1];
					u2 = bu2[l+1];
					w += ( u1 - f ) * ( u2 - f );
					f  = a  [l+2];
					u1 = bu1[l+2];
					u2 = bu2[l+2];
					w += ( u1 - f ) * ( u2 - f );
					f  = a  [l+3];
					u1 = bu1[l+3];
					u2 = bu2[l+3];
					w += ( u1 - f ) * ( u2 - f );
				}
				for(; l < size.width; l++)
				{
					float  f = a  [l];
					float u1 = bu1[l];
					float u2 = bu2[l];
					w += ( u1 - f ) * ( u2 - f );
				}
			}

			covarMatrix[i*nObjects + j] = covarMatrix[j*nObjects + i] = w;
		}
	}
  //	for(int n = 0; n < nObjects; n++)
  //	{
  //		for(int l = 0; l < nObjects; l++)
  //			matrix << covarMatrix[n*nObjects + l<< "  ";
  //		matrix << std::endl;
  //	}
  //	matrix.close();
	return 0;
}

int awpCalcCovarianceMatrix(int nNumObjects, void* input, awpImage* avg, float* covarMatrix)
{
	float*  avg_data;
	int avg_step = 0;
	AwpSize avg_size;
	int i;
	AWPRESULT res = AWP_OK;
	res = awpCheckImage(avg);
	if (res != AWP_OK)
		return -1;
	avg_data = (float*)avg->pPixels;
	avg_step = avg->sSizeX*sizeof(float);
	avg_size.height = avg->sSizeY; avg_size.width = avg->sSizeX;
	if (avg->dwType != AWP_FLOAT)
		return -1;

	awpImage** images = (awpImage**)input;
	AWPBYTE** objects = (AWPBYTE**)malloc( sizeof( AWPBYTE*) * nNumObjects);
	int img_step = 0, old_step = 0;
	AwpSize img_size = avg_size, old_size = avg_size;

	if( objects == NULL )
		return -2;

	for( i=0; i< nNumObjects; i++ )
	{
        awpImage* img = images[i];
        AWPBYTE*  img_data;
        try
        {
            res = awpCheckImage(img);
            if (res != AWP_OK) throw 0;
            img_data = (AWPBYTE*)img->pPixels;
            img_step = img->sSizeX*sizeof(AWPBYTE);
            img_size.height = img->sSizeY; img_size.width = img->sSizeX;
            if (img->dwType != AWP_BYTE)
                throw 0;
            if( img_size != avg_size || img_size != old_size )
                throw 0;
            if( i > 0 && img_step != old_step )
                throw 0;

            old_step   = img_step;
            old_size   = img_size;
            objects[i] = img_data;
        }
        catch(...)
        {
            free((void**)&objects);
            return -1;
        }

    }
    int r = _awpCalcCovarMatrixEx(nNumObjects, objects,img_step, avg_data, avg_step, avg_size, covarMatrix);
    free((void**)&objects);
    return r;
}

int _awpJacobiEigens ( float* A,
                       float* V,
                       float* E,
                       int    n,
                       float  eps )
{
    int i, j, k, ind;
    float *AA = A, *VV = V;
    double Amax, anorm=0, ax;

    if ( A == NULL || V == NULL || E == NULL ) return -1;
    if ( n <= 0 )                              return -1;
    if (eps < 1.0e-7f )  eps = 1.0e-7f;

    /*-------- Prepare --------*/
    for(i=0; i<n; i++, VV+=n, AA+=n)
    {
        for(j=0; j<i; j++)
        {
            double Am = AA[j];
            anorm += Am*Am;
        }
        for(j=0; j<n; j++) VV[j] = 0.f;
        VV[i] = 1.f;
    }

    anorm = sqrt( anorm + anorm );
    ax    = anorm*eps/n;
    Amax  = anorm;

    while ( Amax > ax )
    {
        Amax /= n;
        do  /* while (ind) */
        {
            int p, q;
            float *V1  = V, *A1  = A;
            ind = 0;
            for(p=0; p<n-1; p++, A1+=n, V1+=n)
            {
                float *A2 = A + n*(p+1), *V2 = V + n*(p+1);
                for(q=p+1; q<n; q++, A2+=n, V2+=n)
                {
                    double x, y, c, s, c2, s2, a;
                    float *A3, Apq=A1[q], App, Aqq, Aip, Aiq, Vpi, Vqi;
                    if( fabs( Apq ) < Amax ) continue;

                    ind=1;

                    /*---- Calculation of rotation angle's sine & cosine ----*/
                    App = A1[p];
                    Aqq = A2[q];
                    y   = 5.0e-1*(App - Aqq);
                    x = -Apq / sqrt(Apq*Apq + y*y);
                    if(y<0.0) x = -x;
                    s = x / sqrt(2.0*(1.0 + sqrt(1.0 - x*x)));
                    s2 = s*s;
                    c  = sqrt(1.0 - s2);
                    c2 = c*c;
                    a  = 2.0*Apq*c*s;

                    /*---- Apq annulation ----*/
                    A3 = A;
                    for(i=0; i<p; i++, A3+=n)
                    {
                        Aip = A3[p];
                        Aiq = A3[q];
                        Vpi = V1[i];
                        Vqi = V2[i];
                        A3[p] = (float)(Aip*c - Aiq*s);
                        A3[q] = (float)(Aiq*c + Aip*s);
                        V1[i] = (float)(Vpi*c - Vqi*s);
                        V2[i] = (float)(Vqi*c + Vpi*s);
                    }
                    for(; i<q; i++, A3+=n)
                    {
                        Aip = A1[i];
                        Aiq = A3[q];
                        Vpi = V1[i];
                        Vqi = V2[i];
                        A1[i] = (float)(Aip*c - Aiq*s);
                        A3[q] = (float)(Aiq*c + Aip*s);
                        V1[i] = (float)(Vpi*c - Vqi*s);
                        V2[i] = (float)(Vqi*c + Vpi*s);
                    }
                    for(; i<n; i++)
                    {
                        Aip = A1[i];
                        Aiq = A2[i];
                        Vpi = V1[i];
                        Vqi = V2[i];
                        A1[i] = (float)(Aip*c - Aiq*s);
                        A2[i] = (float)(Aiq*c + Aip*s);
                        V1[i] = (float)(Vpi*c - Vqi*s);
                        V2[i] = (float)(Vqi*c + Vpi*s);
                    }
                    A1[p] = (float)(App*c2 + Aqq*s2 - a);
                    A2[q] = (float)(App*s2 + Aqq*c2 + a);
                    A1[q] = A2[p] = 0.0f;
                } /*q*/
            }     /*p*/
        } while (ind);
        Amax /= n;
    }   /* while ( Amax > ax ) */

    for(i=0, k=0; i<n; i++, k+=n+1) E[i] = A[k];
    /*printf(" M = %d\n", M);*/

    /* -------- ordering --------*/
    for(i=0; i<n; i++)
    {
        int m = i;
        float Em = (float)fabs(E[i]);
        for(j=i+1; j<n; j++)
        {
            float Ej = (float)fabs(E[j]);
            m  = ( Em < Ej ) ?  j :  m;
            Em = ( Em < Ej ) ? Ej : Em;
        }
        if( m != i )
        {
            int l;
            float b = E[i];
            E[i] = E[m];
            E[m] = b;
            for(j=0, k=i*n, l=m*n; j<n; j++, k++, l++)
            {
                b    = V[k];
                V[k] = V[l];
                V[l] = b;
            }
        }
    }

    return 0;
}

static int _awpCalcEigenObjects(int             nObjects,
                                void*           input,
                                int             objStep,
                                void*           output,
                                int             eigStep,
                                AwpSize          size,
                                float			 calcLimit,
                                float*          avg,
                                int             avgStep,
                                float*          eigVals)
{
    int i, j, /*n,*/ iev=0, m1=nObjects - 1, objStep1 = objStep, eigStep1 = eigStep/4;
    AwpSize /*objSize,*/ eigSize, avgSize;
    float* c=0;
    float* ev=0;
    float* bf=0;
    AWPBYTE* buf=0;
    void* buffer=0;
    float m = 1.0f/(float)nObjects;

    int r;
    /*
        if( m1 > calcLimit->maxIter && calcLimit->type != CV_TERMCRIT_EPS )
            m1 = calcLimit->maxIter;
    */
    /* ---- TEST OF PARAMETERS ---- */

    if ( nObjects < 2 )                                        return -1;
    if ( input == NULL || output == NULL || avg == NULL )      return -1;
    if ( size.width > objStep || 4*size.width > eigStep ||
            4*size.width > avgStep || size.height < 1)            return -1;

    avgStep /= 4;
    eigStep /= 4;

    if( objStep==size.width && eigStep==size.width && avgStep==size.width )
    {
        size.width *= size.height;
        size.height = 1;
        objStep = objStep1 = eigStep = eigStep1 = avgStep = size.width;
    }
    eigSize = avgSize = size;


   // n =   objSize.height*objSize.width +
   //       2*eigSize.height*eigSize.width;


    /* Calculation of averaged object */
    bf = avg;
    for(i = 0; i < avgSize.height; i++, bf += avgStep)
        for(j = 0; j < avgSize.width; j++)
            bf[j] = 0.f;

    for( i=0; i<nObjects; i++ )
    {
        int k, l;
        AWPBYTE* bu = ((AWPBYTE**)input)[i]; // !!!!!

        bf = avg;
        for(k = 0; k < avgSize.height; k++, bf += avgStep, bu += objStep1)
            for(l = 0; l < avgSize.width; l++)
                bf[l] += bu[l];
    }

    bf = avg;
    for(i = 0; i < avgSize.height; i++, bf += avgStep)
        for(j = 0; j < avgSize.width; j++)
            bf[j] *= m;

    /* Calculation of covariance matrix */
    c  = (float*)malloc ( sizeof(float)*nObjects*nObjects );
    if( c == NULL )
    {
        if(buffer) free((void**)&buffer );
        if(buf) free((void**)&buf );
        return -2;
    }

    r = _awpCalcCovarMatrixEx( nObjects, input, objStep1, avg, 4*avgStep, size, c );
    if(r != 0)
    {
        free((void**)&c );
        if(buffer) free((void**)&buffer );
        if(buf) free((void**)&buf );
        return r;
    }

    /* Calculation of eigenvalues & eigenvectors */
    ev = (float*)malloc ( sizeof(float)*nObjects*nObjects );
    if( ev == NULL )
    {
        free((void**)&c );
        if(buffer) free((void**)&buffer );
        if(buf) free((void**)&buf );
        return -2;
    }

    if( eigVals == NULL )
    {
        eigVals = (float*)malloc( sizeof(float)*nObjects );
        if( eigVals == NULL )
        {
            free((void**)&c );
            free((void**)&ev );
            if(buffer) free((void**)&buffer );
            if(buf) free((void**)&buf );
            return -2;
        }
        iev = 1;
    }

    r  =  _awpJacobiEigens( c, ev, eigVals, nObjects, 0.0f );
    //    free((void**)&c );
    free(c);
    if(r)
    {
        free((void**)&ev );
        if(buffer) free((void**)&buffer );
        if(buf) free((void**)&buf );
        if(iev) free((void**)&eigVals );
        return r;
    }

    /* Eigen objects number determination */
    for( i=0; i<m1; i++ )
        if( fabs(eigVals[i]/eigVals[0]) < calcLimit) break;
    m1 = i;
    calcLimit = (float)fabs( eigVals[m1 - 1] / eigVals[0] );

    for( i=0; i<m1; i++ ) eigVals[i] = (float)( 1.0/sqrt( eigVals[i] ) );

    /* ----------------- Calculation of eigenobjects ----------------------- */
    int k, p, l;

    for(i=0; i<m1; i++)  /* e.o. annulation */
    {
        float* be = ((float**)output)[i];
        for(p=0; p<eigSize.height; p++, be+=eigStep)
            for(l=0; l<eigSize.width; l++) be[l] = 0.0f;
    }

    for( k=0; k<nObjects; k++ )
    {
        AWPBYTE* bv = ((AWPBYTE**)input)[k]; // !!!!!!


        for(i=0; i<m1; i++)
        {
            float  v = eigVals[i]*ev[i*nObjects + k];
            float* be = ((float**)output)[i];
            AWPBYTE* bu = bv;

            bf = avg;

            for( p=0; p<size.height; p++, bu+=objStep1,
                    bf+= avgStep, be+=eigStep1 )
            {
                for( l=0; l<size.width-3; l+=4 )
                {
                    float f = bf[l];
                    AWPBYTE u = bu[l];
                    be[l]  += v * ( u - f );
                    f = bf[l+1];
                    u = bu[l+1];
                    be[l+1] += v * ( u - f );
                    f = bf[l+2];
                    u = bu[l+2];
                    be[l+2] += v * ( u - f );
                    f = bf[l+3];
                    u = bu[l+3];
                    be[l+3] += v * ( u - f );
                }
                for( ; l<size.width; l++ )
                    be[l] += v * ( bu[l] - bf[l] );
            }
        } /* i */
    } /* k */

    free(ev );
    if(iev) free(eigVals );
    else for( i=0; i<m1; i++ ) eigVals[i] =  1.f/( eigVals[i]*eigVals[i] );
    if(buffer) free(buffer );
    if(buf) free(buf );
    return 0;
}

static int _awpCalcEigenObjectsFloat(int             nObjects,
                                void*           input,
                                int             objStep,
                                void*           output,
                                int             eigStep,
                                AwpSize          size,
                                float			 calcLimit,
                                float*          avg,
                                int             avgStep,
                                float*          eigVals)
{
    int i, j, /*n,*/ iev=0, m1=nObjects - 1, objStep1 = objStep, eigStep1 = eigStep/4;
    AwpSize /*objSize,*/ eigSize, avgSize;
    float* c=0;
    float* ev=0;
    float* bf=0;
    float* buf=0;
    void* buffer=0;
    float m = 1.0f/(float)nObjects;

    int r;
    /*
        if( m1 > calcLimit->maxIter && calcLimit->type != CV_TERMCRIT_EPS )
            m1 = calcLimit->maxIter;
    */
    /* ---- TEST OF PARAMETERS ---- */

    if ( nObjects < 2 )                                        return -1;
    if ( input == NULL || output == NULL || avg == NULL )      return -1;
    if ( size.width > objStep || size.width > eigStep ||
            size.width > avgStep || size.height < 1)            return -1;
//    if ( size.width > objStep || 4*size.width > eigStep ||
//            4*size.width > avgStep || size.height < 1)            return -1;

//    avgStep /= 4;
//    eigStep /= 4;

    if( objStep==size.width && eigStep==size.width && avgStep==size.width )
    {
        size.width *= size.height;
        size.height = 1;
        objStep = objStep1 = eigStep = eigStep1 = avgStep = size.width;
    }
   eigSize = avgSize = size;


//    n =   objSize.height*objSize.width +
//          2*eigSize.height*eigSize.width;


    /* Calculation of averaged object */
    bf = avg;
    for(i = 0; i < avgSize.height; i++, bf += avgStep)
        for(j = 0; j < avgSize.width; j++)
            bf[j] = 0.f;

    for( i=0; i<nObjects; i++ )
    {
        int k, l;
        float* bu = ((float**)input)[i];

        bf = avg;
        for(k = 0; k < avgSize.height; k++, bf += avgStep, bu += objStep1)
            for(l = 0; l < avgSize.width; l++)
                bf[l] += bu[l];
    }

    bf = avg;
    for(i = 0; i < avgSize.height; i++, bf += avgStep)
        for(j = 0; j < avgSize.width; j++)
            bf[j] *= m;

    /* Calculation of covariance matrix */
    c  = (float*)malloc ( sizeof(float)*nObjects*nObjects );
    if( c == NULL )
    {
        if(buffer) free((void**)&buffer );
        if(buf) free((void**)&buf );
        return -2;
    }

    r = _awpCalcCovarMatrixExFloat( nObjects, input, objStep1, avg, 4*avgStep, size, c );
    if(r != 0)
    {
        free((void**)&c );
        if(buffer) free((void**)&buffer );
        if(buf) free((void**)&buf );
        return r;
    }

    /* Calculation of eigenvalues & eigenvectors */
    ev = (float*)malloc ( sizeof(float)*nObjects*nObjects );
    if( ev == NULL )
    {
        free((void**)&c );
        if(buffer) free((void**)&buffer );
        if(buf) free((void**)&buf );
        return -2;
    }

    if( eigVals == NULL )
    {
        eigVals = (float*)malloc( sizeof(float)*nObjects );
        if( eigVals == NULL )
        {
            free((void**)&c );
            free((void**)&ev );
            if(buffer) free((void**)&buffer );
            if(buf) free((void**)&buf );
            return -2;
        }
        iev = 1;
    }

    r  =  _awpJacobiEigens( c, ev, eigVals, nObjects, 0.0f );
    //    free((void**)&c );
    free(c);
    if(r)
    {
        free((void**)&ev );
        if(buffer) free((void**)&buffer );
        if(buf) free((void**)&buf );
        if(iev) free((void**)&eigVals );
        return r;
    }

    if (eigVals[0] == 0)
    {
        return 0;
    }

    /* Eigen objects number determination */
    for( i=0; i<m1; i++ )
        if( fabs(eigVals[i]/eigVals[0]) < calcLimit) break;
    m1 = i;
    calcLimit = (float)fabs( eigVals[m1 - 1] / eigVals[0] );

    for( i=0; i<m1; i++ ) eigVals[i] = (float)( 1.0/sqrt( eigVals[i] ) );

    /* ----------------- Calculation of eigenobjects ----------------------- */
    int k, p, l;

    for(i=0; i<m1; i++)  /* e.o. annulation */
    {
        float* be = ((float**)output)[i];
        for(p=0; p<eigSize.height; p++, be+=eigStep)
            for(l=0; l<eigSize.width; l++) be[l] = 0.0f;
    }

    for( k=0; k<nObjects; k++ )
    {
        float* bv = ((float**)input)[k]; // !!!!!!


        for(i=0; i<m1; i++)
        {
            float  v = eigVals[i]*ev[i*nObjects + k];
            float* be = ((float**)output)[i];
            float* bu = bv;

            bf = avg;

            for( p=0; p<size.height; p++, bu+=objStep1,
                    bf+= avgStep, be+=eigStep1 )
            {
                for( l=0; l<size.width-3; l+=4 )
                {
                    float f = bf[l];
                    float u = bu[l];
                    be[l]  += v * ( u - f );
                    f = bf[l+1];
                    u = bu[l+1];
                    be[l+1] += v * ( u - f );
                    f = bf[l+2];
                    u = bu[l+2];
                    be[l+2] += v * ( u - f );
                    f = bf[l+3];
                    u = bu[l+3];
                    be[l+3] += v * ( u - f );
                }
                for( ; l<size.width; l++ )
                    be[l] += v * ( bu[l] - bf[l] );
            }
        } /* i */
    } /* k */

    free(ev );
    if(iev) free(eigVals );
    else for( i=0; i<m1; i++ ) eigVals[i] =  1.f/( eigVals[i]*eigVals[i] );
    if(buffer) free(buffer );
    if(buf) free(buf );
    return 0;
}

int awpCalcEigenObjectsFloat(int nObjects,void* input,void* output,float calcLimit,awpImage* avg,float* eigVals )
{
    float*  avg_data;
    int avg_step = 0;
    AwpSize avg_size;
    int i;
    int nEigens = nObjects - 1;
    AWPRESULT res = AWP_OK;
    res = awpCheckImage(avg);
    if (res != AWP_OK)
        return -1;
    avg_data = (float*)avg->pPixels;
//    avg_step = avg->sSizeX*sizeof(float);
    avg_step = avg->sSizeX;
    avg_size.height = avg->sSizeY; avg_size.width = avg->sSizeX;
    if (avg->dwType != AWP_FLOAT)
        return -1;
    /*
        if( nEigens > calcLimit->maxIter && calcLimit->type != CV_TERMCRIT_EPS )
            nEigens = calcLimit->maxIter;
    */
    awpImage** objects = (awpImage**)input;
    awpImage** eigens  = (awpImage**)output;
    float** objs = (float**)malloc( sizeof( float*) * nObjects );
    float** eigs = (float**)malloc( sizeof( float*) * nEigens );

    int obj_step = 0, old_step  = 0;
    int eig_step = 0, oldeig_step = 0;
    AwpSize obj_size = avg_size, old_size    = avg_size,
                       eig_size = avg_size, oldeig_size = avg_size;

    if( objects == NULL || eigens == NULL )
    {
        free(objs);
        free(eigs);
        return -2;
    }

    for( i=0; i<nObjects; i++ )
    {
        awpImage* img = objects[i];
        float*    obj_data;

        try
        {
            res = awpCheckImage(img);
            if (res != AWP_OK) throw 0;
            obj_data = (float*)img->pPixels;
//            obj_step = img->sSizeX*sizeof(float);
            obj_step = img->sSizeX;
            obj_size.height = img->sSizeY; obj_size.width = img->sSizeX;
            if (img->dwType != AWP_FLOAT)
                throw 0;
            if( obj_size != avg_size || obj_size != old_size )
                throw 0;
            if( i > 0 && obj_step != old_step )
                throw 0;

            old_step   = obj_step;
            old_size   = obj_size;
            objs[i] = obj_data;
        }
        catch(...)
        {
            free(objs);
            free(eigs);
            return -1;
        }
    }
    for( i=0; i<nEigens; i++ )
    {
        awpImage* eig = eigens[i];
        float*    eig_data;
        try
        {
            res = awpCheckImage(eig);
            if (res != AWP_OK) throw 0;
            eig_data = (float*)eig->pPixels;
//            eig_step = eig->sSizeX*sizeof(float);
            eig_step = eig->sSizeX;
            eig_size.height = eig->sSizeY; eig_size.width = eig->sSizeX;
            if (eig->dwType != AWP_FLOAT)
                throw 0;
            if( eig_size != avg_size || eig_size != oldeig_size )
                throw 0;
            if( i > 0 && eig_step != oldeig_step )
                throw 0;

            oldeig_step   = eig_step;
            oldeig_size   = eig_size;
            eigs[i] = eig_data;
        }
        catch(...)
        {
            free(objs);
            free(eigs);
            return -1;
        }
    }
	int r = _awpCalcEigenObjectsFloat(nObjects,(void*)objs,obj_step,(void*)eigs,eig_step,obj_size,calcLimit,avg_data,avg_step,eigVals);
	awpSaveImage("avg.awp", avg);
    free(objs);
    free(eigs);
    return r;
}

int awpCalcEigenObjects(int nObjects,void* input,void* output,float calcLimit,awpImage* avg,float* eigVals )
{
    if( nObjects > 0 && (( awpImage** )input)[0]->dwType == AWP_FLOAT )
    {
	    return awpCalcEigenObjectsFloat( nObjects, input, output, calcLimit, avg, eigVals );
    }

    float*  avg_data;
    int avg_step = 0;
    AwpSize avg_size;
    int i;
    int nEigens = nObjects;
    AWPRESULT res = AWP_OK;
    res = awpCheckImage(avg);
    if (res != AWP_OK)
        return -1;
    avg_data = (float*)avg->pPixels;
    avg_step = avg->sSizeX*sizeof(float);
    avg_size.height = avg->sSizeY; avg_size.width = avg->sSizeX;
    if (avg->dwType != AWP_FLOAT)
        return -1;
    /*
        if( nEigens > calcLimit->maxIter && calcLimit->type != CV_TERMCRIT_EPS )
            nEigens = calcLimit->maxIter;
    */
    awpImage** objects = (awpImage**)input;
    awpImage** eigens  = (awpImage**)output;
    AWPBYTE** objs = (AWPBYTE**)malloc( sizeof( AWPBYTE*) * nObjects );
    float** eigs = (float**)malloc( sizeof( float*) * nEigens );

    int obj_step = 0, old_step  = 0;
    int eig_step = 0, oldeig_step = 0;
    AwpSize obj_size = avg_size, old_size    = avg_size,
                       eig_size = avg_size, oldeig_size = avg_size;

    if( objects == NULL || eigens == NULL )
    {
        free(objs);
        free(eigs);
        return -2;
    }

    for( i=0; i<nObjects; i++ )
    {
        awpImage* img = objects[i];
        AWPBYTE*    obj_data;

        try
        {
            res = awpCheckImage(img);
            if (res != AWP_OK) throw 0;
            obj_data = (AWPBYTE*)img->pPixels;
            obj_step = img->sSizeX*sizeof(AWPBYTE);
            obj_size.height = img->sSizeY; obj_size.width = img->sSizeX;
            if (img->dwType != AWP_BYTE)
                throw 0;
            if( obj_size != avg_size || obj_size != old_size )
                throw 0;
            if( i > 0 && obj_step != old_step )
                throw 0;

            old_step   = obj_step;
            old_size   = obj_size;
            objs[i] = obj_data;
        }
        catch(...)
        {
            free(objs);
            free(eigs);
            return -1;
        }
    }
    for( i=0; i<nEigens; i++ )
    {
        awpImage* eig = eigens[i];
        float*    eig_data;
        try
        {
            res = awpCheckImage(eig);
            if (res != AWP_OK) throw 0;
            eig_data = (float*)eig->pPixels;
            eig_step = eig->sSizeX*sizeof(float);
            eig_size.height = eig->sSizeY; eig_size.width = eig->sSizeX;
            if (eig->dwType != AWP_FLOAT)
                throw 0;
            if( eig_size != avg_size || eig_size != oldeig_size )
                throw 0;
            if( i > 0 && eig_step != oldeig_step )
                throw 0;

            oldeig_step   = eig_step;
            oldeig_size   = eig_size;
            eigs[i] = eig_data;
        }
        catch(...)
        {
            free(objs);
            free(eigs);
            return -1;
        }
    }
    int r = _awpCalcEigenObjects(nObjects,(void*)objs,obj_step,(void*)eigs,eig_step,obj_size,calcLimit,avg_data,avg_step,eigVals);
    free(objs);
    free(eigs);
    return r;
}

