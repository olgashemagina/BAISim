//---------------------------------------------------------------------------
#include "_LF.h"
#pragma hdrstop
const int c_nEyeWidth  = 12;
const int c_nEyeHeight = 12;
const int c_nEyeSize   = c_nEyeWidth*c_nEyeHeight;
const int c_nEyeSize2  = c_nEyeSize*c_nEyeSize;
const int c_nEyeHalfW  = c_nEyeWidth / 2;
const int c_nEyeHalfH  = c_nEyeHeight / 2;
const double c_fCorrThr = 0.5;

#ifdef _SDEBUG
    static int  gCount = 0;
    static char* gExtJPG  = ".jpg";
    static char* gExtXYZ  = ".dat";
    static char* gExtDAT  = ".txt";
#endif
/*
мозаичное изображение окрестности глаза.

|img11, img12, ... , img1M|
|img21, img22, ... , img2M|
|.........................|
|imgN1, imgN2, ... , imgNM|

Размер мозаики MxN

M = 2*W / step; - число столбцов мозики
N = 2* / step; - число строк мозаики

Ширина изображения: Width  = M*W0,
W0 - ширина элемента мозаики. W0 = c_nEyeWidth
Высота изображения: Htight = N*H0,
H0 - высота элемента мозаики. H0 = c_nEyeHeight
*/
class TLFMosaic : public TLFObject
{
protected:
    int   m_NumCol;         // число столбцов
    int   m_NumRow;         // число строк
    int   m_ElWidth;        // высота элемента
    int   m_ElHeight;       // ширина элемента
    int   m_dx;             // расстояние между элементами по горизонтали
    int   m_dy;             // расстояние между элементами по вертикали

    awpImage* m_pImage;     // изображение мозаики

    void CreateMosaic();
public:
    TLFMosaic();
    TLFMosaic(int NumCol, int NumRow, int W0 = 16, int H0 = 16, int dx = 0, int dy = 0);
    virtual ~TLFMosaic();
    bool AddElement(int col, int row, awpImage* pImage);

    bool SaveMosaic(const char* lpFileName);

};


static void _bSaveAWPAsXYZ(const char* lpFileName, awpImage* pImage)
{
   FILE* F = fopen(lpFileName, "w+t");
   if (F != NULL)
   {
        //
        AWPBYTE* pix = (AWPBYTE*)pImage->pPixels;
        for (int y = 0; y < pImage->sSizeY; y++)
        {
            for (int x = 0; x < pImage->sSizeX; x++)
            {
               fprintf(F, "%i\t%i\t%i\n", x,y,pix[y*pImage->sSizeX + x]);
            }

        }
        fclose(F);
   }
}

static void _SaveAWPAsXYZ(const char* lpFileName, awpImage* pImage)
{
   if (pImage == NULL || lpFileName == NULL)
        return;
   switch (pImage->dwType)
   {
      case AWP_BYTE:
        return _bSaveAWPAsXYZ(lpFileName, pImage);
   }

}

static void _bSaveAWPAsDAT(const char* lpFileName, awpImage* pImage)
{
   FILE* F = fopen(lpFileName, "w+t");
   if (F != NULL)
   {
        //
        AWPBYTE* pix = (AWPBYTE*)pImage->pPixels;
        for (int y = 0; y < pImage->sSizeY; y++)
        {
            for (int x = 0; x < pImage->sSizeX; x++)
            {
               fprintf(F, "%i\n", pix[y*pImage->sSizeX + x]);
            }

        }
        fclose(F);
   }
}

// сохраняет изображение AWP в виде текстового файла
// который содержит один столбец данных -
// значаения пикселей
static void _SaveAWPAsDAT(const char* lpFileName, awpImage* pImage)
{
   if (pImage == NULL || lpFileName == NULL)
        return;
   switch (pImage->dwType)
   {
      case AWP_BYTE:
        return _bSaveAWPAsDAT(lpFileName, pImage);
   }
}
// отрисовка области поиска глаз на копии входного изображения
// и сохраниение копии под именем eye_area.jpg
#define DRAW_EYE_CLIP_RECTS                                                   \
        dLRect.left = LBase.X - s[0]*L/2; dLRect.top = LBase.Y - s[0]*L/2;      \
        dLRect.right= LBase.X + s[0]*L/2; dLRect.bottom = LBase.Y + s[0]*L/2;   \
        dRRect.left = RBase.X - s[0]*L/2; dRRect.top = RBase.Y - s[0]*L/2;      \
        dRRect.right= RBase.X + s[0]*L/2; dRRect.bottom = RBase.Y + s[0]*L/2;   \
                                                                  \
        awpDrawRect(pCopy, &dLRect, 0, 200);                      \
        awpDrawRect(pCopy, &dRRect, 0, 200);                      \
                                                                  \
        dLRect.left = LBase.X - s[1]*L/2; dLRect.top = LBase.Y - s[1]*L/2;      \
        dLRect.right= LBase.X + s[1]*L/2; dLRect.bottom = LBase.Y + s[1]*L/2;   \
        dRRect.left = RBase.X - s[1]*L/2; dRRect.top = RBase.Y - s[1]*L/2;      \
        dRRect.right= RBase.X + s[1]*L/2; dRRect.bottom = RBase.Y + s[1]*L/2;   \
                                                                  \
        awpDrawRect(pCopy, &dLRect, 0, 200);                      \
        awpDrawRect(pCopy, &dRRect, 0, 200);                      \
                                                                  \
        dLRect.left = LBase.X - s[2]*L/2; dLRect.top = LBase.Y - s[2]*L/2;      \
        dLRect.right= LBase.X + s[2]*L/2; dLRect.bottom = LBase.Y + s[2]*L/2;   \
        dRRect.left = RBase.X - s[2]*L/2; dRRect.top = RBase.Y - s[2]*L/2;      \
        dRRect.right= RBase.X + s[2]*L/2; dRRect.bottom = RBase.Y + s[2]*L/2;   \
                                                                  \
        awpDrawRect(pCopy, &dLRect, 0, 200);                      \
        awpDrawRect(pCopy, &dRRect, 0, 200);                      \
                                                                  \
        dLRect.left = LBase.X - s[3]*L/2; dLRect.top = LBase.Y - s[3]*L/2;      \
        dLRect.right= LBase.X + s[3]*L/2; dLRect.bottom = LBase.Y + s[3]*L/2;   \
        dRRect.left = RBase.X - s[3]*L/2; dRRect.top = RBase.Y - s[3]*L/2;      \
        dRRect.right= RBase.X + s[3]*L/2; dRRect.bottom = RBase.Y + s[3]*L/2;   \
                                                                  \
        awpDrawRect(pCopy, &dLRect, 0, 200);                      \
        awpDrawRect(pCopy, &dRRect, 0, 200);                      \
                                                                  \
        dLRect.left = LBase.X - s[4]*L/2; dLRect.top = LBase.Y - s[4]*L/2;      \
        dLRect.right= LBase.X + s[4]*L/2; dLRect.bottom = LBase.Y + s[4]*L/2;   \
        dRRect.left = RBase.X - s[4]*L/2; dRRect.top = RBase.Y - s[4]*L/2;      \
        dRRect.right= RBase.X + s[4]*L/2; dRRect.bottom = RBase.Y + s[4]*L/2;   \
                                                                  \
        awpDrawRect(pCopy, &dLRect, 0, 200);                      \
        awpDrawRect(pCopy, &dRRect, 0, 200);                      \


#define DRAW_EYE_AREA(v)  \
    awpImage* pCopy = NULL;                                       \
    if (awpCopyImage(image.GetImage(), &pCopy) == AWP_OK)         \
    {                                                             \
        awpRect dLRect;                                           \
        awpRect dRRect;                                           \
        dLRect.left = LBase.X - W; dLRect.top = LBase.Y - W;      \
        dLRect.right= LBase.X + W; dLRect.bottom = LBase.Y + W;   \
        dRRect.left = RBase.X - W; dRRect.top = RBase.Y - W;      \
        dRRect.right= RBase.X + W; dRRect.bottom = RBase.Y + W;   \
                                                                  \
        awpDrawRect(pCopy, &dLRect, 0, 128);                      \
        awpDrawRect(pCopy, &dRRect, 0, 128);                      \
        if (v)                                                    \
          DRAW_EYE_CLIP_RECTS                                     \
        awpSaveImage("eye_area.jpg" , pCopy);                     \
        awpReleaseImage(&pCopy);                                  \
    }                                                             \



// реализация мозаики
TLFMosaic::TLFMosaic()
{
    m_pImage = NULL;

    m_NumCol = 4;
    m_NumRow = 4;

    m_ElWidth = 16;
    m_ElHeight = 16;

    m_dx = 0;
    m_dy = 0;

    CreateMosaic();
}

TLFMosaic::TLFMosaic(int NumCol, int NumRow, int W0, int H0, int dx, int dy)
{
    m_pImage = NULL;

    m_NumCol = NumCol;
    m_NumRow = NumRow;

    m_ElWidth = W0;
    m_ElHeight = H0;

    m_dx = dx;
    m_dy = dy;

    CreateMosaic();
}
TLFMosaic::~TLFMosaic()
{
    if (m_pImage)
        awpReleaseImage (&m_pImage);
}
bool TLFMosaic::AddElement(int col, int row, awpImage* pImage)
{
    // преобразование изображения
    if (pImage == NULL)
        return false;
    if (pImage->bChannels != 1)
        return false;
    if (pImage->dwType != AWP_BYTE)
        return false;

    awpResize(pImage, m_ElWidth, m_ElHeight);

    awpPoint p;
    p.X = m_dx + col*(m_ElWidth + m_dx);
    p.Y = m_dy + row*(m_ElHeight + m_dy);

    return awpPasteRect(pImage, m_pImage, p) == AWP_OK;
}
// сохранение мозаики на диск
bool TLFMosaic::SaveMosaic(const char* lpFileName)
{
    if (m_pImage)
        return awpSaveImage(lpFileName, m_pImage) == AWP_OK;
    else
        return false;
}

void TLFMosaic::CreateMosaic()
{
   int ImageWidth  = 0;
   int ImageHeight = 0;

   ImageWidth  = m_dx + m_NumCol*(m_ElWidth + m_dx);
   ImageHeight = m_dy + m_NumRow*(m_ElHeight + m_dy);

   awpCreateImage(&m_pImage, ImageWidth, ImageHeight, 1, AWP_BYTE);
}
// добавление изображения глаза в мозаику
#define _ADD_TO_MOSAIC_                                                 \
    awpImage* pLImage = NULL;                                           \
    if (awpCopyRect(image.GetImage(), &pLImage, &LRect) == AWP_OK)      \
    {                                                                   \
        LMosaic.AddElement(col, row, pLImage);                          \
        awpReleaseImage(&pLImage);                                      \
    }                                                                   \
                                                                        \
    awpImage* pRImage = NULL;                                           \
    if (awpCopyRect(image.GetImage(), &pRImage, &RRect) == AWP_OK)      \
    {                                                                   \
        RMosaic.AddElement(col, row, pRImage);                          \
        awpReleaseImage(&pRImage);                                      \
    }                                                                   \
                                                                        \
    gCount++;                                                           \

//сохраняет мозаику на диск
#define _SAVE_MOSAIC_                                                   \
    char name[32];                                                      \
    memset(name, 0, sizeof(name));                                      \
    char* lmos = "lm.jpg";                                              \
    char* rmos = "rm.jpg";                                              \
    sprintf(name, "%i%s", m, lmos);                                     \
    LMosaic.SaveMosaic(name);                                           \
    memset(name, 0, sizeof(name));                                      \
    sprintf(name, "%i%s", m, rmos);                                     \
    RMosaic.SaveMosaic(name);


///////////////////////////////////////////////////////////////////////////////
// Уточнение местоположения глаза. Вызывает виртуальную функцию  CheckEye
// для набора прямоугольных фрагментов.
//
TLFRoi* ILFEye::Precise(TLFImage& image, TLFRoi& roi, LFPreciseMethod Method)
{
  TLFRoi* pLFRoi = NULL;
  //изображение на которм уточняются глаза
  awpImage* img =  image.GetImage();
  if (img == NULL)
        return pLFRoi;
  double lerr, rerr;
  // расстояние между глазами
  TROI troi = roi.GetRoi();
  double L = troi.EyeDistance();
  // ширина прямоугольника, в котором будет оцениваться наличие глаза
  int    W   =    int(L/3);
  int    WW  =    int(0.75*L);
  int    step = int(L/16);
  if (step == 0) step = 1;
  double  s[5] = {0.5, 0.65, 0.75, 0.85, 0.95};

  // базовые точки вокруг которых выполняется поиск
  awpPoint LBase;LBase = troi.p;
  awpPoint RBase;RBase = troi.p1;

  awpRect LRect;
  awpRect RRect;

  //цикл, выполняющий детектирование глаз
  int x,y,x1, y1;
  double sx  = 0;
  double sy  = 0;
  double sx1 = 0;
  double sy1 = 0;
  double mx  = 0;
  double my  = 0;
  double mx1 = 0;
  double my1 = 0;
  double sm  = 0;
  double sm1 = 0;

  int count  = 0;
  int count1 = 0;

  bool LFound = false;
  bool RFound = false;

#ifdef _SDEBUG
    gCount = 0;

    // создание файлов отладочной информации
    // файл отладочной информации Xeye_dist.dat содержит три
    // текстовых столбца: [x-координата], [у-координата], [расстояние до эталона]
    FILE* FL = fopen("leye_dist.dat", "w+t");
    FILE* FR = fopen("reye_dist.dat", "w+t");

    DRAW_EYE_AREA(false)
#endif

  bool  CheckLResult = false; // результат проверки наличия глаза в точке.
  bool  CheckRResult = false; // результат проверки наличия глаза в точке.
  double flmax       = -1;
  double frmax       = -1;
  for (int m = 0; m < 4; m++)
  {

#ifdef _SDEBUG
   int M = 2*W / step;
   int N = 2*W / step;
   TLFMosaic LMosaic(M, N, c_nEyeWidth, c_nEyeHeight, 1, 1);
   TLFMosaic RMosaic(M, N, c_nEyeWidth, c_nEyeHeight, 1, 1);
#endif

  WW = int(s[m]*L);
  int row,col;
  for ( y = LBase.Y - W, y1 = RBase.Y - W,  row = 0; y < LBase.Y + W; y += step, y1 += step, row++)
  {
        for ( x = LBase.X - W, x1 = RBase.X - W,  col = 0; x < LBase.X + W; x += step, x1 += step, col++)
        {
           LRect.left  = x - WW / 2; LRect.top = y - WW / 2;
           LRect.right = x + WW / 2; LRect.bottom = y + WW /2;

           RRect.left  = x1 - WW /2; RRect.top = y1 - WW/2;
           RRect.right = x1 + WW/2;  RRect.bottom = y1 + WW/2;

           if (awpRectInImage(img, &LRect) != AWP_OK)
             continue;

           if (awpRectInImage(img, &RRect) != AWP_OK)
             continue;
#ifdef _SDEBUG
          _ADD_TO_MOSAIC_
#endif
           CheckLResult = CheckEye(LRect, img, lerr, LFLeftEye);

#ifdef _SDEBUG
            gCount++;
#endif
           CheckRResult = CheckEye(RRect, img, rerr, LFRightEye);

           if (Method == LFAverage)
           {
              if (CheckLResult)
              {
                sx += x;
                sy += y;
                count++;
                LFound = true;
              }
              if (CheckRResult)
              {
                sx1 += x1;
                sy1 += y1;
                count1++;
                RFound = true;
              }
           }
           else if (Method == LFExtremum)
           {
              LFound = true;
              RFound = true;
              if (flmax < lerr)
              {
                flmax = lerr;
                sx = x;
                sy = y;
                count = 1;
              }

              if (frmax < rerr)
              {
                frmax = rerr;
                sx1 = x1;
                sy1 = y1;
                count1 = 1;
              }
           }
           else if (Method == LFMean)
           {
               if (CheckLResult)
               {
                 mx = mx + (1+lerr)*x;
                 my = my + (1+lerr)*y;
                 sm +=(1+lerr);
                 LFound = true;
               }
               if (CheckRResult)
               {
                   mx1 += (1+rerr)*x1;
                   my1 += (1+rerr)*y1;
                   sm1 +=(1+rerr);
                   RFound = true;
               }
           }

#ifdef _SDEBUG
    fprintf(FL, "%i\t%i\t%f\n", x,y, lerr);
    fprintf(FR, "%i\t%i\t%f\n", x1,y1, rerr);
#endif
        }
  }
#ifdef _SDEBUG
            _SAVE_MOSAIC_
#endif
  }
  // выполним уточнение
  //awpPoint p, p1; // новое местоположение глаз
  switch (Method)
  {
    case LFAverage:
        troi.p.X = count == 0? troi.p.X: AWPSHORT(sx / count);
        troi.p.Y = count == 0? troi.p.Y: AWPSHORT(sy / count);
        troi.p1.X = count1 == 0? troi.p1.X: AWPSHORT(sx1 / count1);
        troi.p1.Y = count1 == 0? troi.p1.Y: AWPSHORT(sy1 / count1);
    break;
    case LFExtremum:
        troi.p.X = AWPSHORT(sx);
        troi.p.Y = AWPSHORT(sy);
        troi.p1.X = AWPSHORT(sx1);
        troi.p1.Y = AWPSHORT(sy1);
    break;
    case LFMean:
        troi.p.X  = sm != 0? AWPSHORT(mx / sm): troi.p.X;
        troi.p.Y  = sm != 0? AWPSHORT(my / sm): troi.p.Y;
        troi.p1.X = sm1 != 0? AWPSHORT(mx1 / sm1): troi.p1.X;
        troi.p1.Y = sm1 != 0? AWPSHORT(my1 / sm1): troi.p1.Y;
    break;
  }
  pLFRoi = new TLFRoi();
  troi.err[2] = LFound?1:0;
  troi.err[3] = RFound?1:0;
  pLFRoi->SetRoi(troi);

#ifdef _SDEBUG
    fclose(FL);
    fclose(FR);
#endif

  return pLFRoi;
}
////////////////////////////////////////////////////////////////////////////////
// Уточнение местоположения глаз методом сравнения исследуемого прямогольника
// со усредненным изображением глаза
TLFEyeCorrelation::TLFEyeCorrelation()
{
    m_pLeftEye  = NULL;
    m_pRightEye = NULL;
}

bool TLFEyeCorrelation::Init(const char* lpInitString)
{
#ifdef _SDEBUG
// код для отладки. сохраняет изображение глаза на диск в формате jpg
//awpImage* dbg_img = NULL;
//awpCopyImage(imgLeftEye, &dbg_img);
//awpConvert(dbg_img, AWP_CONVERT_TO_BYTE_WITH_NORM);
//awpSaveImage("left_eye.jpg", dbg_img);
//_SaveAWPAsXYZ("left_eye.xyz", dbg_img);
//_SaveAWPAsDAT("left_eye.dat", dbg_img);
//awpReleaseImage(&dbg_img);
#endif

  char* chName = NULL;
  const char* chLEye  = "left_eye7512.awp";
  const char* chREye  = "right_eye7512.awp";
  chName = (char*)malloc(strlen(lpInitString) + strlen(chREye) + 2);
  memset(chName, 0, strlen(lpInitString) + strlen(chREye));
  memcpy(chName, lpInitString, strlen(lpInitString));
  chName = strcat(chName, chLEye);
  if (awpLoadImage(chName, &m_pLeftEye) == AWP_OK)
  {
        //для того, чтобы выполнить операцию поворота изображение должно иметь
        //тип AWP_BYTE
        if (awpConvert(m_pLeftEye, AWP_CONVERT_TO_BYTE_WITH_NORM) == AWP_OK)
        {
           memset(chName, 0, strlen(lpInitString) + strlen(chREye));
           memcpy(chName, lpInitString, strlen(lpInitString));
           chName = strcat(chName, chREye);

           //скопируем изображение imgLeft в изображение imgRight
           //и повернем последнее.
           if (awpLoadImage(chName, &m_pRightEye) == AWP_OK)
           {
              //выполним операцию поворота
              awpConvert(m_pRightEye, AWP_CONVERT_TO_BYTE_WITH_NORM);

              //вычислим среднюю интенсивность глаза.
			  double avgl = 0;
			  double displ = 0;
			  double avgr = 0;
			  double dispr = 0;
              AWPBYTE*  pixLeft = (AWPBYTE*)m_pLeftEye->pPixels;
              AWPBYTE*  pixRight = (AWPBYTE*)m_pRightEye->pPixels;
              for (int i = 0; i < m_pLeftEye->sSizeX*m_pLeftEye->sSizeY; i++)
              {
                avgl += pixLeft[i];
                displ += pixLeft[i]*pixLeft[i];
                avgr += pixRight[i];
                dispr += pixRight[i]*pixRight[i];
              }

              displ = (c_nEyeSize*displ - avgl*avgl) / c_nEyeSize2;
              avgl /= m_pLeftEye->sSizeX*m_pLeftEye->sSizeY;

              dispr = (c_nEyeSize*dispr - avgr*avgr) / c_nEyeSize2;
              avgr /= m_pRightEye->sSizeX*m_pRightEye->sSizeY;

              m_fMathLeft = avgl;
              m_fDispLeft = sqrt(displ);

              m_fMathRight =  avgr;
              m_fDispRight =  sqrt(dispr);

              m_IsInitialized = true;
           }
        }
  }
#ifdef _SDEBUG
awpSaveImage("wleft_eye.jpg",  m_pLeftEye);
awpSaveImage("wright_eye.jpg", m_pRightEye);
#endif
   free(chName);
   return m_IsInitialized;
}

TLFEyeCorrelation::~TLFEyeCorrelation()
{
    awpReleaseImage(&m_pLeftEye);
    awpReleaseImage(&m_pRightEye);
}

bool TLFEyeCorrelation::CheckEye(awpRect& r, awpImage* pImage, double& err, LFEyes eye)
{
	double feye[c_nEyeSize];
    
	double sx = (r.right - r.left) / (double)c_nEyeWidth;
	double sy = (r.bottom - r.top) / (double)c_nEyeHeight;

    int i, j , c;
    c = 0;
    AWPBYTE* pix = (AWPBYTE*)pImage->pPixels;
    for (int y = 0; y < c_nEyeHeight; y++)
    {
        for (int x = 0; x < c_nEyeWidth; x++)
        {
           i = int(r.left + x*sx);
           j = int(r.top  + y*sy);
           feye[c] = pix[j*pImage->sSizeX + i];
           c++;
        }
    }
#ifdef _SDEBUG
char strName[32];
sprintf(strName, "%i%s", gCount, gExtJPG);
awpImage* dbg_img = NULL;
awpCreateImage(&dbg_img, c_nEyeWidth, c_nEyeHeight, 1, AWP_BYTE);
BYTE* dbg_pix = (BYTE*)dbg_img->pPixels;
for (int dbg_i = 0; dbg_i < c_nEyeWidth*c_nEyeHeight; dbg_i++)
      dbg_pix[dbg_i] = feye[dbg_i];
//awpSaveImage(strName, dbg_img);
sprintf(strName, "%i%s", gCount, gExtDAT);
//_SaveAWPAsDAT(strName, dbg_img);
awpReleaseImage(&dbg_img);
#endif

    err = Correlation(feye, eye);
    return err > c_fCorrThr;
}


double TLFEyeCorrelation::Correlation(double* TestVct, LFEyes eye)
{
  // характеристики исходного вектора
  AWPBYTE* SourceVct  = NULL; // исходный вектор
  double  SourceAvg = 0;    // матожидание для исходного вектора
  double  SourceDisp = 0;    // дисперсия для исходного вектора

  // характеристики тестового вектора
  double TestAvg = 0;
  double TestDisp = 0;

  double cov = 0; // матожидание для результата

  if (eye == LFLeftEye)
  {
     SourceVct  = (AWPBYTE*)m_pLeftEye->pPixels;
     SourceAvg  = m_fMathLeft;
     SourceDisp = m_fDispLeft;
  }
  else if (eye == LFRightEye)
  {
     SourceVct  = (AWPBYTE*)m_pRightEye->pPixels;
     SourceAvg  = m_fMathRight;
     SourceDisp = m_fDispRight;
  }


  for (int i = 0; i < c_nEyeSize; i++)
  {
    TestAvg  += TestVct[i];
    TestDisp += TestVct[i]*TestVct[i];
  }

  TestDisp = sqrt((c_nEyeSize*TestDisp - TestAvg*TestAvg) / c_nEyeSize2);
  if (TestDisp == 0)
    return 0;
  TestAvg  /= c_nEyeSize;
  for (int i = 0; i < c_nEyeSize; i++)
  {
    cov += (TestVct[i] - TestAvg)*(SourceVct[i] - SourceAvg);
  }
  cov /= c_nEyeSize;

  return cov / (TestDisp*SourceDisp);

}
