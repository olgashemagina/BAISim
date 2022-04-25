/*M
//
//      awpipl2 image processing and image analysis library
//		File: awpCamera.c
//		Purpose: videocamera geometry
//		Autors: Alex A.Telnykh (tlnykha@yahoo.com)
//      CopyRight 2004-2020 (c) NN-Videolab.net
//M*/
#include "_awpipl.h"
//---------------------------------------------------------------------------
AWPRESULT awpCameraHorizontalView(awpCamera* pCamera, double* Value)
{
 
	AWPDOUBLE a;
	if (pCamera == NULL)
        return AWP_BADARG;
    if (Value == NULL)
         return AWP_BADARG;

    a =(double) (2.0F * atan((double)(pCamera->dChipWSize / (2.0*pCamera->dFocalLength))));
    *Value =  (double)(180 * a / AWP_PI);

    return AWP_OK;
}
AWPRESULT awpCameraVirticalView(awpCamera* pCamera, double* Value)
{
    double a;
	if (pCamera == NULL)
        return AWP_BADARG;
    if (Value == NULL)
         return AWP_BADARG;

    a = 2.0*atan(pCamera->dChipHSize / (2.0*pCamera->dFocalLength));
    *Value = 180 * a / AWP_PI;

    return AWP_OK;
}
AWPRESULT awpLengthToFirstVisiblePoint(awpCamera* pCamera, double* Value)
{
  
	AWPDOUBLE vv, alfa;
	if (pCamera == NULL)
        return AWP_BADARG;
    if (Value == NULL)
         return AWP_BADARG;
    vv = 0;
    if (awpCameraVirticalView(pCamera, &vv) != AWP_OK)
         return AWP_BADARG;
  	alfa = AWP_PI*(pCamera->dAngle - 0.5*vv) / 180;
	*Value = pCamera->dHeight*tan(alfa);

    return AWP_OK;
}

AWPRESULT awpImageYToLenght(awpCamera* pCamera, awpImage* pImage,  AWPWORD Y, double* Value)
{
    AWPDOUBLE delta, alfa;
	
	if (pCamera == NULL)
        return AWP_BADARG;
    if (pImage == NULL)
        return AWP_BADARG;
    if (Value == NULL)
         return AWP_BADARG;


    if (Y <= 0 || Y > pImage->sSizeY)
        return AWP_BADARG;


 	delta = pCamera->dChipHSize*((pImage->sSizeY -Y)/pImage->sSizeY - 0.5);
	alfa = AWP_PI*pCamera->dAngle/180.;
	*Value =  pCamera->dHeight * tan(atan(delta/pCamera->dFocalLength) + alfa);

    return AWP_OK;
}

AWPRESULT awpImageYHToLength(awpCamera* pCamera, awpImage* pImage,  AWPWORD Y, AWPWORD H,  double* Value)
{
  
	AWPDOUBLE delta, alfa;

	if (pCamera == NULL)
        return AWP_BADARG;
    if (pImage == NULL)
        return AWP_BADARG;
    if (Value == NULL)
         return AWP_BADARG;

    if (Y <= 0 || Y > pImage->sSizeY)
        return AWP_BADARG;


 	delta = pCamera->dChipHSize*((double)(pImage->sSizeY -Y)/(double)pImage->sSizeY - 0.5);
	alfa = AWP_PI*pCamera->dAngle/180.;
	*Value =  (pCamera->dHeight - H) * tan(atan(delta/pCamera->dFocalLength) + alfa);

    return AWP_OK;
}

AWPRESULT awpSceneZToImageY(awpCamera* pCamera, awpImage* pImage, double Z, double* Value)
{
    AWPDOUBLE Y,b;
	
	if (pCamera == NULL)
        return AWP_BADARG;
    if (pImage == NULL)
        return AWP_BADARG;
    if (Value == NULL)
         return AWP_BADARG;

    if (Z <= 0 || Z > pCamera->dMaxLenght)
    {
         *Value = -1;
         return AWP_BADARG;
    }


	Y = 0;
	b = atan(Z / pCamera->dHeight);
	Y = (pImage->sSizeY / pCamera->dChipHSize)*(0.5*pCamera->dChipHSize +
		pCamera->dFocalLength* tan(b + 3.14159*pCamera->dAngle/180));
    *Value = -Y;

    return AWP_OK;
}
AWPRESULT awpImagePointToShift(awpCamera* pCamera, awpImage* pImage, awpPoint* pPoint, double* Value)
{

    AWPDOUBLE L;
	
	if (pCamera == NULL)
        return AWP_BADARG;
    if (pImage == NULL)
        return AWP_BADARG;
    if (Value == NULL)
         return AWP_BADARG;

    if (pPoint->X < 0 || pPoint->X > pImage->sSizeX)
        return AWP_BADARG;
    if (pPoint->Y < 0 || pPoint->Y > pImage->sSizeY)
        return AWP_BADARG;

    L = 0;
    if (awpImageYToLenght(pCamera, pImage, pPoint->Y, &L) != AWP_OK)
        return AWP_BADARG;

    *Value = (pPoint->X / pImage->sSizeX - 0.5)*pCamera->dChipWSize*L/pCamera->dFocalLength;

    return AWP_OK;
}

AWPRESULT awpImagePointToShiftHX(awpCamera* pCamera, awpImage* pImage, awpPoint* pPoint,AWPWORD H, double* Value)
{
    AWPDOUBLE L;
	
	if (pCamera == NULL)
        return AWP_BADARG;
    if (pImage == NULL)
        return AWP_BADARG;
    if (Value == NULL)
         return AWP_BADARG;

    if (pPoint->X < 0 || pPoint->X > pImage->sSizeX)
        return AWP_BADARG;
    if (pPoint->Y < 0 || pPoint->Y > pImage->sSizeY)
        return AWP_BADARG;

    L = 0;
    if (awpImageYHToLength(pCamera, pImage, pPoint->Y, H, &L) != AWP_OK)
        return AWP_BADARG;

    *Value = ((double)pPoint->X / (double)pImage->sSizeX - 0.5)*pCamera->dChipWSize*L/pCamera->dFocalLength;

    return AWP_OK;
}

AWPRESULT awpImagePointToDistance(awpCamera* pCamera, awpImage* pImage, awpPoint* pPoint, double* Value)
{

    AWPDOUBLE L,S;
	
	if (pCamera == NULL)
        return AWP_BADARG;
    if (pImage == NULL)
        return AWP_BADARG;
    if (Value == NULL)
         return AWP_BADARG;

    if (pPoint->X < 0 || pPoint->X > pImage->sSizeX)
        return AWP_BADARG;
    if (pPoint->Y < 0 || pPoint->Y > pImage->sSizeY)
        return AWP_BADARG;

    L = 0;
    if (awpImageYToLenght(pCamera, pImage, pPoint->Y, &L) != AWP_OK)
        return AWP_BADARG;
     S = 0;
    if (awpImagePointToShift(pCamera, pImage, pPoint, &S) != AWP_OK)
        return AWP_BADARG;

    *Value = sqrt(L*L + S*S);
    return AWP_OK;
}

AWPRESULT awpImageObjectWidth(awpCamera* pCamera, awpImage* pImage, awpRect* pObject, double* Value)
{
    AWPDOUBLE L;
	
	if (pCamera == NULL)
        return AWP_BADARG;
    if (pImage == NULL)
        return AWP_BADARG;
    if (Value == NULL)
         return AWP_BADARG;
    if (pObject == NULL)
        return AWP_BADARG;

    if (awpRectInImage(pImage, pObject) != AWP_OK)
        return AWP_BADARG;

    L = 0;
    if (awpImageYToLenght(pCamera, pImage, pObject->bottom, &L) != AWP_OK)
        return AWP_BADARG;

    *Value = fabs(L*pCamera->dChipWSize*(pObject->right - pObject->left)/(pCamera->dFocalLength*pImage->sSizeX));
    return AWP_OK;
}

AWPRESULT awpImageObjectHWidth(awpCamera* pCamera, awpImage* pImage, awpRect* pObject, AWPWORD H,  double* Value)
{
    AWPDOUBLE L;
	
	if (pCamera == NULL)
        return AWP_BADARG;
    if (pImage == NULL)
        return AWP_BADARG;
    if (Value == NULL)
         return AWP_BADARG;
    if (pObject == NULL)
        return AWP_BADARG;

    if (awpRectInImage(pImage, pObject) != AWP_OK)
        return AWP_BADARG;

    L = 0;
    if (awpImageYHToLength(pCamera, pImage, pObject->top, H, &L) != AWP_OK)
        return AWP_BADARG;

    *Value = 2*fabs(L*pCamera->dChipWSize*(pObject->right - pObject->left)/(pCamera->dFocalLength*pImage->sSizeX));
    return AWP_OK;
}

AWPRESULT awpImageObjectHeight(awpCamera* pCamera, awpImage* pImage, awpRect* pObject, double* Value)
{
    AWPDOUBLE Z, delta, gamma, tan_alfa;
	
	if (pCamera == NULL)
        return AWP_BADARG;
    if (pImage == NULL)
        return AWP_BADARG;
    if (Value == NULL)
         return AWP_BADARG;
    if (pObject == NULL)
        return AWP_BADARG;

    if (awpRectInImage(pImage, pObject) != AWP_OK)
        return AWP_BADARG;

    Z = 0;
    if (awpImageYToLenght(pCamera, pImage, pObject->bottom, &Z) != AWP_OK)
        return AWP_BADARG;

	delta = pCamera->dChipHSize*((pImage->sSizeY - pObject->top )/pImage->sSizeY - 0.5);
	gamma = atan(delta / pCamera->dFocalLength);
	tan_alfa = tan(gamma + AWP_PI*pCamera->dAngle/180.);
	*Value = pCamera->dHeight - Z / tan_alfa;

    return AWP_OK;
}

AWPRESULT awpSceneToImagePoint(awpCamera* pCamera, awpImage* pImage, awp3DPoint* pSrc, awp2DPoint* pDst)
{
    AWPDOUBLE gamma;
	
	if (pCamera == NULL)
        return AWP_BADARG;
    if (pImage == NULL)
        return AWP_BADARG;
    if (pSrc == NULL)
        return AWP_BADARG;
    if (pDst == NULL)
        return AWP_BADARG;

  	pDst->X = pImage->sSizeX*(pSrc->X*pCamera->dFocalLength / (pCamera->dChipWSize*pSrc->Z) + 0.5);
	gamma = atan(pSrc->Z /(pCamera->dHeight - pSrc->Y));
	pDst->Y = pImage->sSizeY*(pCamera->dFocalLength*tan(AWP_PI*pCamera->dAngle /180 - gamma)/pCamera->dChipHSize + 0.5);


    return AWP_OK;
}

AWPRESULT awpImageToScenePoint(awpCamera* pCamera, awpImage* pImage, double Height,  awp2DPoint* pSrc, awp3DPoint* pDst)
{
	AWPDOUBLE X, Y, Z;
	awpPoint p;
    awpRect r;
	
	if (pCamera == NULL)
        return AWP_BADARG;
    if (pImage == NULL)
        return AWP_BADARG;
    if (pSrc == NULL)
        return AWP_BADARG;
    if (pDst == NULL)
        return AWP_BADARG;

    if (awpImageYToLenght(pCamera, pImage,(AWPWORD)pSrc->Y, &Z) != AWP_OK)
        return AWP_BADARG;
    p.X = (AWPSHORT)pSrc->X;
	p.Y = (AWPSHORT)pSrc->Y;
    if (awpImagePointToShift(pCamera, pImage, &p, &X) != AWP_OK)
        return AWP_BADARG;

	r.left = (AWPSHORT)pSrc->X;
    r.right =  r.left + 1;
	r.top = (AWPSHORT)pSrc->Y;
    r.bottom = (AWPSHORT)(r.top + Height);

    if (awpImageObjectHeight(pCamera, pImage, &r, &Y) != AWP_OK)
        return AWP_BADARG;

    pDst->X = X;
    pDst->Y = Y;
    pDst->Z = Z;

    return AWP_OK;
}
