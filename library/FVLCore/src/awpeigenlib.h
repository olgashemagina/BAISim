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
   �������: 
	-- int awpCalcCovarianceMatrix()
	������� ������������� �������

   ���������:

	������������ ��������:
	 0		- ������� ������� ��������� ��������
	-1		- ������������ ���������
	-2		- ������������ ������

   ���������: 
*/
int awpCalcCovarianceMatrix(int nNumObjects, void* input,awpImage* avg,float*    covarMatrix);
/*
	������� ����������� ������� � ����������� ��������
*/
int awpCalcEigenObjects(int nObjects,void* input,void* output,float calcLimit,awpImage* avg,float* eigVals );
/*
   �������: 
	-- int awpEigenDecomposite()
   ������� ����������� ���������� �� ����������� �������� 

   ���������:
   - pImage			- [in] �������� ����������� 
   - nNumObjects	- [in] ����� ����������� �������� (������ ������)	
   - pEigenVectors	- [in] ��������� �� ����������� ������� 
   - pAverage		- [in] ����������� ����������� 	
   - pfCoeff		- [out] ������������ ���������� 

	������������ ��������:
	 0		- ������� ������� ��������� ��������
	-1		- ������������ ���������
	-2		- ������������ ������

   ���������: 
	
*/
int awpEigenDecomposite(awpImage* pImage, int nNumObjects, void* pEigenVectors, awpImage* pAverage, float* pfCoeff);
/*
	�������:
	-- int awpEigenProjection()
  
	������� �������� ������� �� ������������ ����������� ������� 
	(�������������� ������) ��������� �������������� ��������� �����
	(������������ ����������� ��������) ������� ����� � ����. ���������� 
	����������������� �������

	���������:
	- nNumObjects	 - [in]  ����� ����������� ��������, ������������ � ����������  pEigenVectors
	- pEigenVectors  - [in]  ��������� �� ������ ����������� �������� 
	- pfCoeff		 - [in]  ����������� ����������, ���������� � ������� ������� awpEigenDecomposite
	- pAverage		 - [in]  ����������� ������
	- Reconstruction - [out] ����������������� ����������� 

	������������ ��������:
	 0		- ������� ������� ��������� ��������
	-1		- ������������ ���������
	-2		- ������������ ������
	
    ���������:
		
		��������� pEigenVectors ������ ��������� ������ �� �� ����� ��� nNumObjects ��������� ���� awpImage*,
		��� �����������, ������ ���� ����������� ������� � ����������� ����, ������ AWP_FLOAT. ���� ��� ������� �� 
		����������� - ������� ���������� -1. 

		����������� pAverage ������ ����� ��� AWP_FLOAT � ���� ����� ������ �������, ��� � ����������� � pEigenVectors

		
		

*/
int awpEigenProjection(int nNumObjects, void* pEigenVectors, float* pfCoeff, awpImage* pAverage, awpImage* Reconstruction);
int awpEigenProjectionFloat(int nNumObjects, void* pEigenVectors, float* pfCoeff, awpImage* pAverage, awpImage* Reconstruction);
#ifdef __cplusplus
}
#endif


#endif //_AWP_EIGEN_LIB_H_
