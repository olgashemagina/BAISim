#include "_LF.h"

TLFColorDetector::TLFColorDetector()
{
   m_Threshold = 100;
   m_scale     = 4; // уменьшаем исходное изображение в 4 раза
   m_pIntegralLin = new TLFImage();
}	

TLFColorDetector::~TLFColorDetector()
{
    delete m_pIntegralLin;
}
// процесс преобразования изображения
bool TLFColorDetector::Init(awpImage* pImage)
{
  bool res = false;
  int MinV = 5;
  int MaxV = 255;
  int MinS = 10;
  int MaxS = 230;
  int Cl1 = 255;
  int Cl2 = 30;
   //анализ входного изображения
   if (awpCheckImage(pImage) != AWP_OK)
        return res;
   if (pImage->dwType != AWP_BYTE || pImage->bChannels != 3)
        return res;

   // работаем с копией исходного изображения
   awpImage* pCopy = NULL;
   if (awpCopyImage(pImage, &pCopy) == AWP_OK)
   {
      //уменьшим исходное изображение
      int nSizeX = pCopy->sSizeX / m_scale;
      int nSizeY = pCopy->sSizeY / m_scale;
      if (awpResize(pCopy, nSizeX, nSizeY) == AWP_OK)
      {
         //преобразуем исходное изображение в формат HSV
         awpImage* pHSV = NULL;
         awpImage* pBin = NULL;
         if (awpRGBtoHSVImage(pCopy, &pHSV) == AWP_OK &&
         awpCreateImage(&pBin, nSizeX, nSizeY, 1, AWP_BYTE) == AWP_OK)
         {
            //выполним цветовую фильтрацию
            awpColor* c = (awpColor*)pHSV->pPixels;
            AWPBYTE* b = (AWPBYTE*)pBin->pPixels;
            for (int i = 0; i < pCopy->sSizeX*pCopy->sSizeY; i++)
            {
                if (((c[i].bRed > Cl1 && c[i].bRed < 255) || (c[i].bRed > 0 && c[i].bRed < Cl2)) &&
                       c[i].bGreen >  MinS && c[i].bGreen <  MaxS &&
                       c[i].bBlue > MinV && c[i].bBlue < MaxV)
                    b[i] = 255;
            }
           m_pIntegralLin->SetImage(pBin);
           res = true;
         }
         awpReleaseImage(&pBin);
         awpReleaseImage(&pHSV);
      }
      awpReleaseImage(&pCopy);
   }
   return res;
}

int  TLFColorDetector::ClassifyRect(awpRect Fragment, double* err, int* vect )
{
   bool res = false;
   awpImage* pImage = m_pIntegralLin->GetImage();
   if (pImage == NULL)
      return res;
   double scale = 1.0/ m_scale;
   awpRect r = awpResizeRect(Fragment, scale);
   if (awpRectInImage(pImage, &r) != AWP_OK)
     return res;

    double v1,v2,v3,v4;
    int w = (r.right - r.left);
    int h = (r.bottom - r.top);
    double s = 1 + w*h;
    int x = r.left;
    int y = r.top;
    int ww = pImage->sSizeX;

    double* pix = (double*)pImage->pPixels;
    v1 = pix[x + y*ww];
    v2 = pix[x + w + (y+h)*ww];
    v3 = pix[x + w + y*ww];
    v4 = pix[x + (y+h)*ww];
    res = (v1+v2-v3-v4)/s > m_Threshold;

   return res;
}

// порог распознавания
double TLFColorDetector::GetThreshold()
{
   return m_Threshold;
}

void TLFColorDetector::SetThreshold(double Value)
{
   m_Threshold = Value;
}
