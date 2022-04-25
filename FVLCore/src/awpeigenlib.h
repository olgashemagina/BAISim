#ifndef _AWP_EIGEN_LIB_H_
#define _AWP_EIGEN_LIB_H_

#ifdef __cplusplus
extern "C" {
#endif


#include <awpipl.h>
typedef struct {
    int width;
    int height;
} AwpSize;


/*
   Функция: 
	-- int awpCalcCovarianceMatrix()
	Находит ковариционную матрицу

   Параметры:

	Возвращаемое значение:
	 0		- Функция успешно выполнила операцию
	-1		- неправильные аргументы
	-2		- недостаточно памяти

   Пояснения: 
*/
int awpCalcCovarianceMatrix(int nNumObjects, void* input,awpImage* avg,float*    covarMatrix);
/*
	Находит собственные вектора и собственные значения
*/
int awpCalcEigenObjects(int nObjects,void* input,void* output,float calcLimit,awpImage* avg,float* eigVals );
/*
   Функция: 
	-- int awpEigenDecomposite()
   Находит коэфициенты разложения по собственным векторам 

   Параметры:
   - pImage			- [in] Исходное изображение 
   - nNumObjects	- [in] Число собственных векторов (размер базиса)	
   - pEigenVectors	- [in] Указатель на собственные вектора 
   - pAverage		- [in] Усредненное изображение 	
   - pfCoeff		- [out] Коэффициенты разложения 

	Возвращаемое значение:
	 0		- Функция успешно выполнила операцию
	-1		- неправильные аргументы
	-2		- недостаточно памяти

   Пояснения: 
	
*/
int awpEigenDecomposite(awpImage* pImage, int nNumObjects, void* pEigenVectors, awpImage* pAverage, float* pfCoeff);
/*
	Функция:
	-- int awpEigenProjection()
  
	Находит проекцию объекта на пространство собственных веторов 
	(реконструирует объект) используя предварительно найденный базис
	(пространство собственных векторов) средний обект и коэф. разложения 
	реконструируемого объекта

	Парамерты:
	- nNumObjects	 - [in]  число собственных векторов, сожержащихся в переменной  pEigenVectors
	- pEigenVectors  - [in]  указатель на массив собственных векторов 
	- pfCoeff		 - [in]  коэфициенты разложения, полученные с помощью функции awpEigenDecomposite
	- pAverage		 - [in]  усредненный вектор
	- Reconstruction - [out] Рекоструированное изображение 

	Возвращаемое значение:
	 0		- Функция успешно выполнила операцию
	-1		- неправильные аргументы
	-2		- недостаточно памяти
	
    Пояснения:
		
		Перменная pEigenVectors должна содержать массив из не менее чем nNumObjects элементов типа awpImage*,
		все изображения, должны быть одинакового размера и одинакового типа, только AWP_FLOAT. Если эти условия не 
		выполняются - функция возвращает -1. 

		Изображение pAverage должно иметь тип AWP_FLOAT и быть тогож самого размера, что и изображения в pEigenVectors

		
		

*/
int awpEigenProjection(int nNumObjects, void* pEigenVectors, float* pfCoeff, awpImage* pAverage, awpImage* Reconstruction);
int awpEigenProjectionFloat(int nNumObjects, void* pEigenVectors, float* pfCoeff, awpImage* pAverage, awpImage* Reconstruction);
#ifdef __cplusplus
}
#endif


#endif //_AWP_EIGEN_LIB_H_
