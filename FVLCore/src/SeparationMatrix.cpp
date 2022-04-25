//---------------------------------------------------------------------------


#pragma hdrstop
#include <stdlib.h>
#include <mem.h>
#include <fstream>
#include <math.h>
#include "SeparationMatrix.h"
#include "fvlcore.h"
#include "cxcore.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)
#pragma link "cxcore.lib"

int __cdecl sort_function( const void *a, const void *b)
{
   double* item1 = (double*)a;
   double* item2 = (double*)b;

   if( *item1 > *item2 )
        return 1;
   else if ( *item1 < *item2 )
        return -1;
   else
        return 0;
}

int BuildSeparationMatrix( void* input, FvcTemplate** pTemplate )
{
    if( ((FvcImages*)input)->numClasses <= 0 )
        return FVC_INVALIDARG;

    LDAImages* Data = ((FvcImages*)input)->pData;
    int numClasses = ((FvcImages*)input)->numClasses;
/*
    for( int mn = 0; mn < numClasses; mn++ )
    {
        for( int v = 0; v < Data[mn].totalInClass; v++ )
        {
             char name[6];
             string str("c:\\temp\\!\\");
             str  = str + itoa( mn, name, 10 );
             str = str + itoa( v, name, 10 );
             str += ".jpg";
             awpConvert( Data[mn].ppImages[v], AWP_CONVERT_TO_BYTE_WITH_NORM );
             awpSaveImage( str.c_str(), Data[mn].ppImages[v] );
        }
    }

*/
// Building Within-Class Scatter Matrix Sw

    /*------ mu - average vectors for all classes, mu_overall - overall average vector -------------------*/
    int size = (Data[0].ppImages[0])->sSizeX*(Data[0].ppImages[0])->sSizeY;
    double** mu = (double**)malloc( numClasses * sizeof(double*) );
    if( mu == NULL )
        return FVC_OUTOFMEMORY;
    double* mu_overall = (double*)malloc( size*sizeof(double) );
    if( mu_overall == NULL )
    {
        free(mu);
        return FVC_OUTOFMEMORY;
    }
    memset( mu_overall, 0., size*sizeof(double) );
    int i, j, k, l, m, s, num = 0;

    for( i = 0; i < numClasses; i++ )
    {
        mu[i] = (double*)malloc( size*sizeof( double ) );
        if( mu[i] == NULL )
        {
            free(mu);
            free(mu_overall);
            return FVC_OUTOFMEMORY;
        }
        memset( mu[i], 0., size*sizeof( double ) );
        for( j = 0; j < Data[i].totalInClass; j++ )
        {
            float* pixels = (float* )(Data[i].ppImages[j])->pPixels;
            for( k = 0; k < size; k++ )
                mu[i][k] += pixels[k];
        }
        for( k = 0; k < size; k++ )
        {
            mu_overall[k] += mu[i][k];
            mu[i][k] /= Data[i].totalInClass;
        }
        num += Data[i].totalInClass;
    }
    for( k = 0; k < size; k++ )
        mu_overall[k] /= num;
    /*---------------------------------------------------------------------*/

    double* SwData = ( double* )malloc( size*size*sizeof(double) );
    if( SwData == NULL )
    {
        free(mu_overall);
        for( i = 0; i < numClasses; i++ )
            free( mu[i] );
        free(mu);
        return FVC_OUTOFMEMORY;
    }
    memset( SwData, 0., size*size*sizeof(double) );

    double* Vect = (double*)malloc( size*sizeof(double) );
    if( Vect == NULL )
    {
        free(mu_overall);
        for( i = 0; i < numClasses; i++ )
            free( mu[i] );
        free(mu);
        free(SwData);
        return FVC_OUTOFMEMORY;
    }

    double* TVect = (double*)malloc( size*sizeof(double) );
    if( TVect == NULL )
    {
        free(mu_overall);
        for( i = 0; i < numClasses; i++ )
            free( mu[i] );
        free(mu);
        free(SwData);
        free(Vect);
        return FVC_OUTOFMEMORY;
    }

    CvMat Vector, TVector, MulVectors, Sw;
    double* MulData = (double*)malloc(size*size*sizeof(double));
    if( MulData == NULL )
    {
        free(mu_overall);
        for( i = 0; i < numClasses; i++ )
            free( mu[i] );
        free(mu);
        free(SwData);
        free(Vect);
        free(TVect);
        return FVC_OUTOFMEMORY;
    }
    for( i = 0; i < numClasses; i++ )
    {
        for( j = 0; j < Data[i].totalInClass; j++ )
        {
            float* pixels = ( float* )(Data[i].ppImages[j])->pPixels;
            for( k = 0; k < size; k++ )
                Vect[k] = pixels[k] - mu[i][k];


            cvInitMatHeader(&Vector, size, 1, CV_64FC1, Vect);

            cvInitMatHeader(&TVector, 1, size, CV_64FC1, TVect);

            cvInitMatHeader(&MulVectors, size, size, CV_64FC1, MulData);
            cvT(&Vector, &TVector);
            cvMatMul( &Vector, &TVector, &MulVectors );

            for( l = 0; l < size; l++ )
                for( m = 0; m < size; m++ )
                    SwData[l*size + m] += MulData[l*size + m]/num;
        }

    }
    cvInitMatHeader(&Sw, size, size, CV_64FC1, SwData);
    free(Vect);
    free(TVect);
    free(MulData);
// Building Between-Class Scatter Matrix Sb

    double* SbData = ( double* )malloc( size*size*sizeof(double) );
    if( SbData == NULL )
    {
        free(mu_overall);
        for( i = 0; i < numClasses; i++ )
            free( mu[i] );

        free(mu);
        free(SwData);
        return FVC_OUTOFMEMORY;
    }
    memset( SbData, 0., size*size*sizeof(double) );
    double* PhiData = (double*)malloc( size*numClasses*sizeof(double) );
    if( PhiData == NULL )
    {
        free(mu_overall);
        for( i = 0; i < numClasses; i++ )
            free( mu[i] );
        free(mu);
        free(SwData);
        free(SbData);
        return FVC_OUTOFMEMORY;
    }
    for( i = 0; i < numClasses; i++ )
    {
        double koeff = sqrt((double)Data[i].totalInClass / num);
        for( k = 0; k < size; k++ )
        {
            PhiData[i + k*numClasses] = koeff * ( mu[i][k] - mu_overall[k] );
        }
    }

    free(mu_overall);
    for( i = 0; i < numClasses; i++ )
        free( mu[i] );
    free(mu);

    double* PhiTData = (double*)malloc( size*numClasses*sizeof(double) );
    if( PhiTData == NULL )
    {
        free(SwData);
        free(SbData);
        free( PhiData );
        return FVC_OUTOFMEMORY;
    }

    double* SbTData = (double*)malloc( numClasses*numClasses*sizeof(double) );
    if( SbTData == NULL )
    {
        free(SwData);
        free(SbData);
        free( PhiData );
        free( PhiTData );
        return FVC_OUTOFMEMORY;
    }

    CvMat Sb, Phi, PhiT, SbT, EigVect, EigVals, EigVectT;
    cvInitMatHeader(&Sb, size, size, CV_64FC1, SbData);
    cvInitMatHeader(&Phi, size, numClasses, CV_64FC1, PhiData);
    cvInitMatHeader(&PhiT, numClasses, size, CV_64FC1, PhiTData);
    cvInitMatHeader(&SbT, numClasses, numClasses, CV_64FC1, SbTData);

    cvT(&Phi, &PhiT);

    cvMatMul( &PhiT, &Phi, &SbT );
    cvMatMul( &Phi, &PhiT, &Sb );

    free( PhiTData );

    int m_b = numClasses - 1;

    double* eigvals = (double*)malloc( numClasses*sizeof(double) );
    if( eigvals == NULL )
    {
        free(SwData);
        free(SbData);
        free(SbTData);
        free( PhiData );
        return FVC_OUTOFMEMORY;
    }
    double* eigvects = (double*)malloc( numClasses*numClasses*sizeof(double) );
    if( eigvects == NULL )
    {
        free(SwData);
        free(SbData);
        free(SbTData);
        free( PhiData );
        free(eigvals);
        return FVC_OUTOFMEMORY;
    }
    double* eigvectsT = (double*)malloc( m_b*numClasses*sizeof(double) );
    if( eigvectsT == NULL )
    {
        free(SwData);
        free(SbData);
        free(SbTData);
        free(PhiData);
        free(eigvals);
        free(eigvects);
        return FVC_OUTOFMEMORY;
    }

    cvInitMatHeader(&EigVect, numClasses, numClasses, CV_64FC1, eigvects);
    cvInitMatHeader(&EigVals, 1, numClasses, CV_64FC1, eigvals);

    cvEigenVV( &SbT, &EigVect, &EigVals );

    free(SbTData);
    free(eigvals);

    eigvects = (double*)realloc( eigvects, numClasses*m_b*sizeof(double) );
    cvInitMatHeader(&EigVect, m_b, numClasses, CV_64FC1, eigvects);
    cvInitMatHeader(&EigVectT, numClasses, m_b, CV_64FC1, eigvectsT);

    cvT( &EigVect, &EigVectT );
    free( eigvects );

    double* eigvectsSb = (double*)malloc( size*m_b*sizeof(double) );
    if( eigvectsSb == NULL )
    {
        free(SwData);
        free(SbData);
        free(PhiData);
        free(eigvectsT);
        return FVC_OUTOFMEMORY;
    }
    double* eigvectsSbT = (double*)malloc( size*m_b*sizeof(double) );
    if( eigvectsSbT == NULL )
    {
        free(SwData);
        free(SbData);
        free(PhiData);
        free(eigvectsT);
        free(eigvectsSb);
        return FVC_OUTOFMEMORY;
    }
    double* eigvalsSb = (double*)malloc( m_b*m_b*sizeof(double) );
    if( eigvalsSb == NULL )
    {
        free(SwData);
        free(SbData);
        free(PhiData);
        free(eigvectsT);
        free(eigvectsSb);
        free(eigvectsSbT);
        return FVC_OUTOFMEMORY;
    }
    double* tmp = (double*)malloc( m_b*size*sizeof(double) );
    if( tmp == NULL )
    {
        free(SwData);
        free(SbData);
        free(PhiData);
        free(eigvectsT);
        free(eigvectsSb);
        free(eigvectsSbT);
        free(eigvalsSb);
        return FVC_OUTOFMEMORY;
    }
    CvMat EigVectsSb, EigVectsSbT, EigValsSb, Tmp, EigValsSbNorm;
    cvInitMatHeader(&EigVectsSb, size, m_b, CV_64FC1, eigvectsSb);
    cvInitMatHeader(&EigVectsSbT, m_b, size,  CV_64FC1, eigvectsSbT);
    cvInitMatHeader(&EigValsSb, m_b, m_b, CV_64FC1, eigvalsSb);
    cvInitMatHeader(&Tmp, m_b, size, CV_64FC1, tmp);

    cvMatMul( &Phi, &EigVectT, &EigVectsSb );

    free(PhiData);
    free(eigvectsT);

    cvT( &EigVectsSb, &EigVectsSbT );
    cvMatMul( &EigVectsSbT, &Sb, &Tmp );
    cvMatMul( &Tmp, &EigVectsSb, &EigValsSb );
    free(tmp);
    free(SbData);
    free(eigvectsSbT);

    double* eigvalsSbnorm = (double*)malloc( m_b*m_b*sizeof(double) );
    if( eigvalsSbnorm == NULL )
    {
        free(SwData);
        free(eigvectsSb);
        free(eigvalsSb);
        return FVC_OUTOFMEMORY;
    }
    memset( eigvalsSbnorm, 0., m_b*m_b*sizeof(double) );
    cvInitMatHeader(&EigValsSbNorm, m_b, m_b, CV_64FC1, eigvalsSbnorm);

    for( i = 0; i < m_b; i++ )
         eigvalsSbnorm[i*numClasses] =  1./sqrt(eigvalsSb[i*numClasses]);

    cvInitMatHeader(&EigValsSb, m_b, m_b, CV_64FC1, eigvalsSbnorm);

    free(eigvalsSb);

    CvMat H, HT, HSw;
    double* h = (double*)malloc( size*m_b*sizeof(double) );
    if( h == NULL )
    {
        free(SwData);
        free(eigvectsSb);
        free(eigvalsSbnorm);
        return FVC_OUTOFMEMORY;
    }
    cvInitMatHeader(&H, size, m_b, CV_64FC1, h);
    double* hT = (double*)malloc( size*m_b*sizeof(double) );
    if( hT == NULL )
    {
        free(SwData);
        free(eigvectsSb);
        free(eigvalsSbnorm);
        free(h);
        return FVC_OUTOFMEMORY;
    }
    cvInitMatHeader(&HT, m_b, size, CV_64FC1, hT);
    double* hSw = (double*)malloc( m_b*m_b*sizeof(double) );
    if( hSw == NULL )
    {
        free(SwData);
        free(eigvectsSb);
        free(eigvalsSbnorm);
        free(h);
        free(hT);
        return FVC_OUTOFMEMORY;
    }
    cvInitMatHeader(&HSw, m_b, m_b, CV_64FC1, hSw);

    cvMatMul( &EigVectsSb, &EigValsSb, &H );

    free(eigvectsSb);
    free(eigvalsSbnorm);

    cvT( &H, &HT );
    tmp = (double*)malloc( m_b*size*sizeof(double) );
    if( tmp == NULL )
    {
        free(SwData);
        free(hSw);
        free(h);
        free(hT);
        return FVC_OUTOFMEMORY;
    }
    cvInitMatHeader(&Tmp, m_b, size, CV_64FC1, tmp);
    cvMatMul( &HT, &Sw, &Tmp );
    cvMatMul( &Tmp, &H, &HSw );

    free(hT);
    free(tmp);
    free(SwData);

    double* eigvectsHSw = (double*)malloc( m_b*m_b*sizeof(double) );
    if( eigvectsHSw == NULL )
    {
        free(hSw);
        free(h);
        return FVC_OUTOFMEMORY;
    }
    double* eigvalsHSw = (double*)malloc( m_b*sizeof(double) );
    if( eigvalsHSw == NULL )
    {
        free(hSw);
        free(h);
        free(eigvectsHSw);
        return FVC_OUTOFMEMORY;
    }

    CvMat EigVectsHSw, EigValsHSw;
    cvInitMatHeader(&EigVectsHSw, m_b, m_b, CV_64FC1, eigvectsHSw);
    cvInitMatHeader(&EigValsHSw, 1, m_b, CV_64FC1, eigvalsHSw);

    cvEigenVV( &HSw, &EigVectsHSw, &EigValsHSw );
    free(hSw);
    qsort( eigvalsHSw, m_b, sizeof(double), sort_function );

    double* eigvectsHSwSort = (double*)malloc( m_b*m_b*sizeof(double) );
    if( eigvectsHSwSort == NULL )
    {
        free(h);
        free(eigvectsHSw);
        return FVC_OUTOFMEMORY;
    }

    for( i = 0; i < m_b; i++ )
         memcpy( eigvectsHSwSort + i*m_b, eigvectsHSw + m_b*(m_b-1) - i*m_b, m_b*sizeof(double));

    free(eigvectsHSw);

    cvInitMatHeader(&EigVectsHSw, m_b, m_b, CV_64FC1, eigvectsHSwSort);

    double eta;
    if( (double)num/numClasses > 2 )
        eta = 0.001;
    else if( (double)num/numClasses == 2 )
        eta = 1;

    double* eigvalsHSwEtaNorm = (double*)malloc( m_b*m_b*sizeof(double) );
    if( eigvalsHSwEtaNorm == NULL )
    {
        free(h);
        free(eigvectsHSwSort);
        return FVC_OUTOFMEMORY;
    }
    memset( eigvalsHSwEtaNorm, 0., m_b*m_b*sizeof(double) );
    CvMat EigValsHSwEtaNorm, Psi;

    for( i = 0; i < m_b; i++ )
        eigvalsHSwEtaNorm[i*numClasses] = 1./sqrt( eta + eigvalsHSw[i] );

    cvInitMatHeader(&EigValsHSwEtaNorm, m_b, m_b, CV_64FC1, eigvalsHSwEtaNorm);

    tmp = (double*)malloc( size*m_b*sizeof(double) );
    if( tmp == NULL )
    {
        free(h);
        free(eigvectsHSwSort);
        free(eigvalsHSwEtaNorm);
        return FVC_OUTOFMEMORY;
    }

    cvInitMatHeader(&Tmp, size, m_b, CV_64FC1, tmp);
    cvMatMul( &H, &EigVectsHSw, &Tmp );

    double* psi = (double*)malloc( size*m_b*sizeof(double) );
    if( psi == NULL )
    {
        free(h);
        free(eigvectsHSwSort);
        free(eigvalsHSwEtaNorm);
        free(tmp);
        return FVC_OUTOFMEMORY;
    }
    cvInitMatHeader(&Psi, size, m_b, CV_64FC1, psi);
    cvMatMul( &Tmp, &EigValsHSwEtaNorm, &Psi );

    free(tmp);

    *pTemplate = NULL;
    fvcCreateTemplate( pTemplate, 64, 64, m_b, false );

    for( i = 0; i < m_b; i++ )
        for( j = 0; j < size; j++ )
            (*pTemplate)->pVectors[i*size + j] = psi[ j*m_b + i ];

    free(h);
    free(eigvectsHSwSort);
    free(eigvalsHSwEtaNorm);
    free(psi);

    return 0;
}
