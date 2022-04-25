//---------------------------------------------------------------------------
#pragma hdrstop
#include "_awpipl.h"
//#include "awpGauss.h"

/*
  Модуль "линейная алгебра"
  1. Решение систем линейных уравнений методом Гаусса.
  2. Перемножение матриц.
  3. Транспонирование матрицы
  4. Нахождение обратной матрицы.
*/

//---------------------------------------------------------------------------
#pragma package(smart_init)
//https://prog-cpp.ru/gauss/
AWPRESULT awpGaussLinear(awpImage* matrix, awpImage* y, awpImage* result)
{
	AWPRESULT  res = AWP_OK;
    AWPDOUBLE  c;
    AWPINT     k, h, i, j, w, index;
    AWPDOUBLE* mtx, *a,*yy;
    AWPDOUBLE* xx, max, eps;

    if (matrix == NULL || y == NULL || result == NULL)
		_ERROR_EXIT_RES_(AWP_BADARG)
    if (matrix->sSizeX != matrix->sSizeY)
		_ERROR_EXIT_RES_(AWP_BADARG)
    if (matrix->dwType != AWP_DOUBLE || y->dwType != AWP_DOUBLE ||
	    result->dwType != AWP_DOUBLE)
	    _ERROR_EXIT_RES_(AWP_BADARG)
    if (y->sSizeY != result->sSizeY || y->sSizeY != matrix->sSizeX)
	    _ERROR_EXIT_RES_(AWP_BADARG)
    //Метод Гаусса

    xx  = (AWPDOUBLE*)result->pPixels;
    yy  = (AWPDOUBLE*)y->pPixels;
    mtx = (AWPDOUBLE*)matrix->pPixels;

    a = mtx;
    h  = matrix->sSizeY;
    w  = matrix->sSizeX;
    eps = 0.00001;
    k = 0;
    while (k < h)
    {
    // Поиск строки с максимальным a[i][k]
    max = abs(a[k*w+k]);
    index = k;
    for (i = k + 1; i < h; i++)
    {
      if (abs(a[i*w+k]) > max)
      {
        max = abs(a[i*w+k]);
        index = i;
      }
    }
    // Перестановка строк
    if (max < eps)
        _ERROR_EXIT_RES_(AWP_BADARG)
    for (j = 0; j < h; j++)
    {
      c = a[k*w+j];
      a[k*w+j] = a[index*w+j];
      a[index*w+j] = c;
    }
    c = yy[k];
    yy[k] = yy[index];
    yy[index] = c;
    // Нормализация уравнений
    for (i = k; i < h; i++)
    {
      c = a[i*w+k];
      if (abs(c) < eps)
        continue; // для нулевого коэффициента пропустить
      for ( j = 0; j < h; j++)
        a[i*w+j] = a[i*w+j] / c;
      yy[i] = yy[i] / c;
      if (i == k)  continue; // уравнение не вычитать само из себя
      for ( j = 0; j < h; j++)
        a[i*w+j] = a[i*w+j] - a[k*w+j];
      yy[i] = yy[i] - yy[k];
    }
    k++;
    }

    // обратная подстановка
    for (k = h - 1; k >= 0; k--)
    {
        xx[k] = yy[k];
        for ( i = 0; i < k; i++)
          yy[i] = yy[i] - a[i*w+k] * xx[k];
    }
CLEANUP:
	return res;
}
// транспонирование матрицы
AWPRESULT awpMatrixTranspose(awpImage* matrix, awpImage* result)
{
 	AWPRESULT  res = AWP_OK;
    AWPDOUBLE *m,*r;
    AWPINT x,y;

    if (matrix == NULL || result == NULL)
		_ERROR_EXIT_RES_(AWP_BADARG)
    if (matrix->dwType != AWP_DOUBLE || result->dwType != AWP_DOUBLE)
		_ERROR_EXIT_RES_(AWP_BADARG)
    if (matrix->bChannels != 1 || result->bChannels != 1)
		_ERROR_EXIT_RES_(AWP_BADARG)
    if (matrix->sSizeX != result->sSizeY || matrix->sSizeY != result->sSizeX)
		_ERROR_EXIT_RES_(AWP_BADARG)

    m = (AWPDOUBLE*)matrix->pPixels;
    r = (AWPDOUBLE*)result->pPixels;

    for (y = 0; y < matrix->sSizeY; y++)
    {
        for (x = 0; x < matrix->sSizeX; x++)
        {
            r[x*result->sSizeX + y] = m[y*matrix->sSizeX +x];
        }
    }


CLEANUP:
	return res;
}

AWPRESULT awpMulMatrix(awpImage* matrix1, awpImage* matrix2, awpImage** ppDst)
{
    AWPDOUBLE *_m1,*_m2, *_d, sum;
    AWPINT i,j,r;
 	AWPRESULT  res = AWP_OK;
    if (matrix1 == NULL || matrix2 == NULL || ppDst == NULL)
		_ERROR_EXIT_RES_(AWP_BADARG)
    if (matrix1->dwType != AWP_DOUBLE || matrix2->dwType != AWP_DOUBLE)
		_ERROR_EXIT_RES_(AWP_BADARG)
    if (matrix1->bChannels != 1 || matrix2->bChannels != 1)
		_ERROR_EXIT_RES_(AWP_BADARG)
    if (matrix1->sSizeX != matrix2->sSizeY)
 		_ERROR_EXIT_RES_(AWP_BADARG)

    if (awpCreateImage(ppDst, matrix2->sSizeX, matrix1->sSizeY, 1, AWP_DOUBLE) != AWP_OK)
		_ERROR_EXIT_RES_(AWP_BADARG)

    _m1 = (AWPDOUBLE*)matrix1->pPixels;
    _m2 = (AWPDOUBLE*)matrix2->pPixels;
    _d  = (AWPDOUBLE*)(*ppDst)->pPixels;


    for (i = 0; i < matrix1->sSizeY; i++)
    {
        for (j = 0; j < matrix2->sSizeX; j++)
        {
            sum = 0;
            for (r = 0; r < matrix1->sSizeX; r++)
            {
                sum += _m1[i*matrix1->sSizeX + r]*_m2[r*matrix2->sSizeX + j];
            }
            _d[i*matrix2->sSizeX + j] = sum;
        }
    }


CLEANUP:
	return res;
}

//
#define _NaN() (double&)*"Implementation dependent";
static  void  _invers(AWPDOUBLE** m, AWPINT N)
{
    //  (c) Drobotenko    http://drobotenko.com

    AWPINT *rn, *cn;
    AWPINT j,k, jt,kt,jved, kved;
    AWPDOUBLE gaus_minved=1e99;
    AWPDOUBLE gaus_deter=1,vved,t, mul;
    AWPINT gaus_ostatok;

    rn = (AWPINT*)malloc(N*sizeof(int));
    cn = (AWPINT*)malloc(N*sizeof(int));

    for(j=N;j--;)
     rn[j]=cn[j]=j;
    for(gaus_ostatok=N;gaus_ostatok;gaus_ostatok--)
    {  //int jved,kved;
       vved=-1;

      // поиск ведущего
      for(j=N;j--;)
      {  if(~rn[j])
         for(k=N;k--;)
           if(~cn[k])
           if(vved<fabs(m[j][k]))
              vved=fabs(m[j][k]),jved=j,kved=k;
      }

	  if(gaus_minved>vved)
         gaus_minved=vved;
      gaus_deter*=m[jved][kved];
      if(vved<1e-99)
      {  for(j=N;j--;)
         {  if(~rn[j]) for(k=N;k--;)
			  m[j][k]=0;//_NaN();
			if(~cn[j]) for(k=N;k--;)
			  m[k][j]=0;//_NaN();
         }
         return;
      }

      jt=rn[jved],kt=cn[kved];

      if(jt!=kved)
        gaus_deter=-gaus_deter;
      // меняем знак детерминанта если перестановка

      // перестановки
      for(j=N;j--;)
          t=m[kt][j],m[kt][j]=m[jved][j],m[jved][j]=t;
      for(j=N;j--;)
          t=m[j][jt],m[j][jt]=m[j][kved],m[j][kved]=t;

      rn[jved]=rn[kt];
      cn[kved]=cn[jt];
      rn[kt]=cn[jt]=-1;

      vved=m[kt][jt];   m[kt][jt]=1;
      for(j=N;j--;)
      {  if(j==kt)
           continue;
         mul=m[j][jt]/vved;
         m[j][jt]=0;
         for(k=N;k--;)
           m[j][k]-=m[kt][k]*mul;
           // самый внутренний цикл ровно N^3 операций
      }
      for(k=N;k--;)
          m[kt][k]/=vved;
    }

    free(rn);
    free(cn);
}

AWPRESULT awpMatrixInvert(awpImage* matrix, awpImage* invert)
{
 	AWPRESULT  res = AWP_OK;
    AWPDOUBLE **m, *m0;
    AWPINT i,j;
    if (matrix == NULL || invert == NULL)
		_ERROR_EXIT_RES_(AWP_BADARG)
    if (matrix->dwType != AWP_DOUBLE || invert->dwType != AWP_DOUBLE)
		_ERROR_EXIT_RES_(AWP_BADARG)
    if (matrix->bChannels != 1 || invert->bChannels != 1)
		_ERROR_EXIT_RES_(AWP_BADARG)
    if (matrix->sSizeX != invert->sSizeX || matrix->sSizeY != matrix->sSizeY)
		_ERROR_EXIT_RES_(AWP_BADARG)

    m = (double**)malloc(matrix->sSizeX*sizeof(double*));
    for (i = 0; i < matrix->sSizeX; i++)
        m[i] = (double*)malloc(matrix->sSizeX*sizeof(double));

    m0 = (AWPDOUBLE*)matrix->pPixels;
    for (i = 0; i < matrix->sSizeY; i++)
        for (j = 0; j < matrix->sSizeX; j++)
            m[i][j] = m0[i*matrix->sSizeX +j];

    _invers(m, matrix->sSizeX);

    m0 = (AWPDOUBLE*)invert->pPixels;
    for (i = 0; i < matrix->sSizeY; i++)
        for (j = 0; j < matrix->sSizeX; j++)
            m0[i*matrix->sSizeX +j] =m[i][j];
CLEANUP:
	return res;
}

AWPRESULT awpLinearRegress(awpImage* matrix, awpImage* y, awpImage* b)
{
    awpImage* xt    = NULL;
    awpImage* mxxt  = NULL;
    awpImage* imxxt = NULL;
    awpImage* hh    = NULL;
    awpImage* bb    = NULL;

    AWPDOUBLE* _bb = NULL;
    AWPDOUBLE* _b  = NULL;
    AWPINT i;
 	AWPRESULT  res = AWP_OK;
    if (matrix == NULL || matrix->bChannels != 1 || matrix->dwType != AWP_DOUBLE)
		_ERROR_EXIT_RES_(AWP_BADARG)
    if (y == NULL || y->bChannels != 1 || y->dwType != AWP_DOUBLE)
		_ERROR_EXIT_RES_(AWP_BADARG)
    if (b == NULL || b->bChannels != 1 || b->dwType != AWP_DOUBLE)
		_ERROR_EXIT_RES_(AWP_BADARG)
    if (matrix->sSizeY != y->sSizeY || matrix->sSizeX != b->sSizeY)
		_ERROR_EXIT_RES_(AWP_BADARG)

    // alloc images
    _CHECK_RESULT_(res = awpCreateImage(&xt, matrix->sSizeY, matrix->sSizeX, 1, AWP_DOUBLE))
    _CHECK_RESULT_(res = awpCreateImage(&imxxt, xt->sSizeY, matrix->sSizeX, 1, AWP_DOUBLE))

    // do regression
    _CHECK_RESULT_(res = awpMatrixTranspose(matrix, xt))
    _CHECK_RESULT_(res = awpMulMatrix(xt, matrix, &mxxt))
    _CHECK_RESULT_(res = awpMatrixInvert(mxxt, imxxt))
    _CHECK_RESULT_(res = awpMulMatrix(imxxt, xt, &hh))
    _CHECK_RESULT_(res = awpMulMatrix(hh, y, &bb))

    _bb = (AWPDOUBLE*)bb->pPixels;
    _b  = (AWPDOUBLE*)b->pPixels;
    for (i = 0; i < b->sSizeX*b->sSizeY; i++)
        _b[i] = _bb[i];

CLEANUP:
    _AWP_SAFE_RELEASE_(xt)
    _AWP_SAFE_RELEASE_(mxxt)
    _AWP_SAFE_RELEASE_(imxxt)
    _AWP_SAFE_RELEASE_(hh)
    _AWP_SAFE_RELEASE_(bb)

	return res;
}

