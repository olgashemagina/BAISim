//---------------------------------------------------------------------------
#include <vcl.h>
#include <Clipbrd.hpp>
#include <math.hpp>
#include "FImage41.h"
#include "DIBImage41.h"
#include "ExportRaster.h"
#include "PhImageTool.h"
#include "PhUtils.h"
#pragma hdrstop
#pragma package(smart_init)

#define MAX_ZOOM 10.0f
#define MIN_ZOOM 0.01f
#define ZOOM_STEP 0.1f

extern "C"
{
    #pragma link "awpipl2b.lib"
    #pragma link "JPEGLIB.lib"
    #pragma link "libpng.lib"
    #pragma link "tifflib.lib"
    #pragma link "zlib.lib"
}

//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//
//static inline void ValidCtrCheck(TPhCustomImage *)
//{
 //   new TPhCustomImage(NULL);
//}
//---------------------------------------------------------------------------
/*
    Function: TFCustomImage construction
    Purpose:  Создает растровое изображение и объект Selection,
              устанавливает для внутренних переменныех значения по умолчанию
    Comments:
*/
__fastcall TPhImage::TPhImage(TComponent* Owner)
    : TCustomControl(Owner)
{
	m_Bitmap     = new TDIBImage();
    m_SelectedBitmap = NULL;
	m_Scale      = 1;

	// установка наследуемых свойств по умолчанию
	Color = clWindow;
	ParentColor = false;
	Width   = 100;
	Height  = 100;
	m_FileName  = "";
    m_Paint = NULL;
	m_BeforeOpen = NULL;
	m_AfterOpen  = NULL;
	m_PosChange  = NULL;
	m_ScaleChange = NULL;
	m_Change      = NULL;
	m_OnCancel   = NULL;
	m_OnFinish   = NULL;
    m_OnStart    = NULL;
    m_OnFrame    = NULL;
    m_OnFrameData = NULL;
	m_StartPoint.x = 0;
	m_StartPoint.y = 0;
	m_SelRect.Left = 0;
	m_SelRect.Top = 0;
	m_SelRect.Right = 0;
	m_SelRect.Bottom = 0;

	m_modified = false;
	m_ph_tools = new TList();
    m_selected_ph_tool = 0;

    this->m_Frames = new TPhFrames(this);
    this->m_Timer = new TTimer(this);
    this->m_Timer->Enabled = false;
    this->m_Timer->OnTimer = TimerEventHandler;
    this->m_Timer->Interval = 500;
    m_tWidth = 128;
    m_tHeight = 128;
	m_mosaic = false;
    m_autoMosaic = true;
}
__fastcall TPhImage::TPhImage(HWND Parent):TCustomControl(Parent)
{

}
/* ---------------------------------------------------------------------------
	Function:  destructor of the  TFCustomImage
	Purpose:   if we have raster data abs other objects delete them
	Comments:
---------------------------------------------------------------------------*/
__fastcall TPhImage::~TPhImage()
{
	Close();
    delete this->m_Frames;
    delete this->m_Timer;
}
/* ---------------------------------------------------------------------------
	Function: TFCustomImage::Init(AnsiString& FileName)
	Purpose:  Init Photon with list of media files
	Comments:
	При замене существующего изображения новым, делается копия существующего
	изображения. В случае аварии при загрузке нового изображения старое
	восстанавливается из копии.
---------------------------------------------------------------------------*/
bool  __fastcall TPhImage::Init(TStrings* Names)
{

    if (Names == NULL || Names->Count == 0)
        return false;

    // todo: check number of files in the source list
    // open list with limit 4096 files
    Close();
    return m_Frames->Init(Names);
}

bool __fastcall         TPhImage::InitFile(UnicodeString strFileName)
{
    TStringList* list = new TStringList();
    list->Add(strFileName);
    bool res = Init(list);
    delete list;
    return res;
}

/*---------------------------------------------------------------------------
    Function: TFCustomImage::Close()
    Purpose:  Освобождает растровые данные и данные selection
    Comments:
---------------------------------------------------------------------------*/
void __fastcall TPhImage::Close()
{
   m_Scale          = 1;
   m_StartPoint.x   = 0;
   m_StartPoint.y   = 0;
   m_SelRect.Left = 0;
   m_SelRect.Top = 0;
   m_SelRect.Right = 0;
   m_SelRect.Bottom = 0;
   delete m_Bitmap;
   m_Bitmap     = new TDIBImage();

   if (m_SelectedBitmap != NULL)
   {
    delete m_SelectedBitmap;
    m_SelectedBitmap = NULL;
   }
   m_Timer->Enabled = false;
   m_Frames->Close();
   m_mosaic = false;
}

bool __fastcall TPhImage::GetSlideShow()
{
    return this->m_Timer->Enabled;
}

void __fastcall TPhImage::SetSlideShow(bool Value)
{
    if (m_Timer->Enabled == Value)
        return;

    if (!Value)
    {
        this->m_Timer->Enabled = false;
        return;
    }

    if (Value && this->m_Frames->Count > 1)
        this->m_Timer->Enabled = true;
}

unsigned int __fastcall TPhImage::GetSlideShowInterval()
{
    return this->m_Timer->Interval;
}
void __fastcall TPhImage::SetSlideShowInterval(unsigned int Value)
{
    this->m_Timer->Interval = Value;
}

void __fastcall   TPhImage::SetMosaicSelected(int value)
{
    if (!this->Mosaic)
        return;
    if (value < 0 || value > this->Frames->Count-1)
        return;
    m_idx = value;
    //
    int w = floor(sqrt((double)Frames->Count) + 0.5);
    int y = m_idx / w;
    int x = m_idx % w;

    TPoint p = this->GetScreenPoint(x*this->m_tWidth + this->m_tWidth/2, y*this->m_tHeight + m_tHeight / 2);
    m_xx = p.x + 1;
    m_yy = p.y + 1;
    Paint();
}


bool __fastcall TPhImage::GetMosaic()
{
   return this->m_mosaic;
}
void __fastcall TPhImage::SetMosaic(bool Value)
{
    if (this->m_Bitmap->Empty)
        return;

    if (Value == m_mosaic)
        return;
    if (Value && this->m_Timer->Enabled)
    {
        this->m_Timer->Enabled = false;
    }

    if (Value)
    {
       this->m_Bitmap->Assign(this->m_Frames->Mosaic);
       this->BestFit();
       m_mosaic = true;
    }
    else
    {
        m_Frames->GoFrame(m_Frames->CurrentFrame);
        m_mosaic = false;
    }

    if (m_Mosaic)
        m_Mosaic(this);
    if (m_Change)
        m_Change(this);
}


void __fastcall   TPhImage::TimerEventHandler(TObject *Sender)
{
    this->m_Frames->Next();
}


void __fastcall     TPhImage::Resize(void)
{
        Paint();
}
/*---------------------------------------------------------------------------
    Function: TFCustomImage::GetScale()
    Purpose:  Возвращает масштаб отображаемого изображения
    Comments:
---------------------------------------------------------------------------*/
float __fastcall TPhImage::GetScale() const
{
    return RoundTo( m_Scale*100.0f, -2 );
}

//------------------------- Fitting commands --------------------------------
//эта группа методов позволяет пользователю наилучшим образом размещать
//растровые данные во внутренней области компонента
/*
    Function: TFCustomImage::BestFit()
    Purpose:  Размещает изображение таким образом, чтоб оно полностью
              поместилось во внутренней области компонента
    Comments: При этом центр изображения совпадает с центром компонента
*/
void __fastcall TPhImage::BestFit()
{
   if (Empty)
      return;

   m_StartPoint.x = 0;
   m_StartPoint.y = 0;

   float alfa_c = (float)Width / (float)Height;
   float alfa_i = (float)m_Bitmap->Width / (float)m_Bitmap->Height;

   if (alfa_c >= alfa_i)
      m_Scale = (float)Height / (float)m_Bitmap->Height;
   else
      m_Scale = (float)Width / (float)m_Bitmap->Width;

   m_Scale = RoundTo( m_Scale, -2 );

   Paint();

   if (m_ScaleChange)
      m_ScaleChange(this);
}
/*
    Function: TFCustomImage::FitWidth()
    Purpose:  Размещает изображение таким образом, что его ширина
              становится равной ширине клиентской области окна.
    Comments: При этом центр изображения совпадает с центром компонента
*/
void __fastcall TPhImage::FitWidth()
{
    if (Empty)
       return;

    float alfa_c = (float)Width / (float)Height;
    float alfa_i = (float)m_Bitmap->Width / (float)m_Bitmap->Height;

    if (alfa_c >= alfa_i)
    {
       m_StartPoint.x = 0;
       m_StartPoint.y = m_Bitmap->Height / 2 - m_Bitmap->Width / (2*alfa_c);
       m_Scale = (float)Height * alfa_c / (float)m_Bitmap->Width;
    }
    else
    {
       m_StartPoint.x = 0;
       m_StartPoint.y = 0;
       m_Scale = (float)Width / (float)m_Bitmap->Width;
    }
    m_Scale = RoundTo( m_Scale, -2 );
    Paint();

   if (m_ScaleChange)
      m_ScaleChange(this);
}

/*
    Function: TFCustomImage::FitHeight()
    Purpose:  Размещает изображение таким образом, что его высота
               становится равной высоте клиентской области компонента
    Comments: При этом центр изображения совпадает с центром компонента
*/
void __fastcall TPhImage::FitHeight()
{
   if (Empty)
      return;

   float alfa_c = (float)Width / (float)Height;
   float alfa_i = (float)m_Bitmap->Width / (float)m_Bitmap->Height;
   m_Scale       = (float)Height / (float)m_Bitmap->Height;

   if (alfa_c >= alfa_i) // картинка влезет целиком на Canvas
   {
      m_StartPoint.x = 0;
      m_StartPoint.y = 0;
   }
   else
   {
      m_StartPoint.x = m_Bitmap->Width / 2 - Width/(2*m_Scale);
      m_StartPoint.y = 0;
   }

   m_Scale = RoundTo( m_Scale, -2 );

   Paint();

   if (m_ScaleChange)
      m_ScaleChange(this);
}

//------------------------- Zoom commands ------------------------------------

// макрос, сохраняющий центр видимой области.

#define _VISIBLE_CENTER_      int Cx, Cy;      \
   if ( m_Bitmap->Height * m_Scale >= Height )   \
       Cy = int((float)m_StartPoint.y + (float)Height / (2.0f*m_Scale) +0.5f);\
   else                                         \
       Cy = int ((float)m_StartPoint.y + (float)m_Bitmap->Height / 2.0f +0.5f); \
                                            \
   if ( m_Bitmap->Width * m_Scale >= Width )    \
       Cx = int((float)m_StartPoint.x + (float)Width / (2.0f*m_Scale) +0.5f); \
   else                                       \
       Cx = int((float)m_StartPoint.x + (float)m_Bitmap->Width / 2.0f +0.5);

/*
    Function:  TFCustomImage::ActualSize()
    Purpose:   Устанавливает коэффициент масштабирования в 100%
    Comments:
*/
void __fastcall TPhImage::ActualSize()
{
   if (Empty)
      return;

   ZoomTo(100);

   Paint();

   if (m_ScaleChange)
      m_ScaleChange(this);
}

/*
    Function: TFCustomImage::ZoomIn()
    Purpose:   Увеличивает коэфициент масштабирования изображения на 10%
               от текущего значения todo:
    Comments:
*/
void __fastcall TPhImage::ZoomIn(int x, int y)
{
   if ( Empty || m_Scale > MAX_ZOOM - ZOOM_STEP )
      return;

   // Сохраним центр видимой области
   _VISIBLE_CENTER_

 /*  if (x > 0 && y > 0)
   {
	  int _x =  GetImageX( x );
	  int _y =  GetImageY( y );
	  Cx = _x > 0 ? _x: 0;
	  Cy = _y > 0 ? m_Bitmap->Height - _y : 0;
   }*/

   m_Scale = RoundTo( m_Scale + ZOOM_STEP, -2 );

   // Сместим угол относительно центра
   if ( m_Bitmap->Width * m_Scale > Width )
	  m_StartPoint.x = Cx - Width / (2*m_Scale);
   else
      m_StartPoint.x = 0;

   if ( m_Bitmap->Height * m_Scale > Height )
      m_StartPoint.y = Cy - Height / (2*m_Scale);
   else
      m_StartPoint.y = 0;

   //
   int Cx1, Cy1;
   if ( m_Bitmap->Height * m_Scale >= Height )
       Cy1 = m_StartPoint.y + Height / (2*m_Scale);
   else
       Cy1 = m_StartPoint.y + m_Bitmap->Height / 2;

   if ( m_Bitmap->Width * m_Scale >= Width )
       Cx1 = m_StartPoint.x + Width / (2*m_Scale);
   else
       Cx1 = m_StartPoint.x + m_Bitmap->Width / 2;

   m_StartPoint.x += Cx - Cx1;
   m_StartPoint.y += Cy - Cy1;

   // Скорректируем выходы за границы изображения
   m_StartPoint.y = ( m_StartPoint.y < 0 ) ? 0 : m_StartPoint.y;
   m_StartPoint.y = ( m_StartPoint.y + GetHeightToDisplay()/m_Scale > m_Bitmap->Height ) ?
                     m_Bitmap->Height - GetHeightToDisplay()/m_Scale : m_StartPoint.y;
   m_StartPoint.x = ( m_StartPoint.x < 0 ) ? 0 : m_StartPoint.x;
   m_StartPoint.x = ( m_StartPoint.x + GetWidthToDisplay()/m_Scale > m_Bitmap->Width ) ?
                     m_Bitmap->Width - GetWidthToDisplay()/m_Scale : m_StartPoint.x;

   Paint();

   if (m_ScaleChange)
      m_ScaleChange(this);
}

/*
    Function: Уменьшает коэфициент масштабирования на 10%
    Purpose:
    Comments:
*/
void __fastcall TPhImage::ZoomOut(int x, int y)
{
   if (Empty || m_Scale < MIN_ZOOM + ZOOM_STEP)
      return;

	_VISIBLE_CENTER_

/*   if (x > 0 && y > 0)
   {
	  int _x =  GetImageX( x );
	  int _y =  GetImageY( y );
	  Cx = _x > 0 ? _x: 0;
	  Cy = _y > 0 ? m_Bitmap->Height - _y : 0;
   }*/

   m_Scale = RoundTo( m_Scale - ZOOM_STEP, -2 );
   // Сместим угол относительно центра
   if ( m_Bitmap->Height * m_Scale > Height )
      m_StartPoint.y = Cy - Height / (2*m_Scale);
   else
      m_StartPoint.y = 0;

   if ( m_Bitmap->Width * m_Scale > Width )
      m_StartPoint.x = Cx - Width / (2*m_Scale);
   else
      m_StartPoint.x = 0;

   int Cx1, Cy1;
   if ( m_Bitmap->Height * m_Scale >= Height )
       Cy1 = m_StartPoint.y + Height / (2*m_Scale);
   else
       Cy1 = m_StartPoint.y + m_Bitmap->Height / 2;

   if ( m_Bitmap->Width * m_Scale >= Width )
       Cx1 = m_StartPoint.x + Width / (2*m_Scale);
   else
       Cx1 = m_StartPoint.x + m_Bitmap->Width / 2;
   m_StartPoint.x += Cx - Cx1;
   m_StartPoint.y += Cy - Cy1;

   m_StartPoint.y = ( m_StartPoint.y < 0 ) ? 0 : m_StartPoint.y;
   m_StartPoint.y = ( m_StartPoint.y + GetHeightToDisplay()/m_Scale > m_Bitmap->Height ) ?
                     m_Bitmap->Height - GetHeightToDisplay()/m_Scale : m_StartPoint.y;
   m_StartPoint.x = ( m_StartPoint.x < 0 ) ? 0 : m_StartPoint.x;
   m_StartPoint.x = ( m_StartPoint.x + GetWidthToDisplay()/m_Scale > m_Bitmap->Width ) ?
                     m_Bitmap->Width - GetWidthToDisplay()/m_Scale : m_StartPoint.x;

   Paint();

   if (m_ScaleChange)
      m_ScaleChange(this);
}
/*
    Function:  TFCustomImage::ZoomTo(int ZoomFactor)
    Purpose:   Изменяет коэфициент масштабирования с соответствии с аргументом
               ZoomFactor
    Comments:  todo: определить допустимые пределы изменения переменной
               ZoomFactor
*/
void __fastcall TPhImage::ZoomTo(int in_ZoomFactor)
{
   if (Empty)
      return;

   float ZoomFactor = (float)in_ZoomFactor/100.0f;

   if (ZoomFactor == m_Scale ||
       ZoomFactor > MAX_ZOOM ||
       ZoomFactor < MIN_ZOOM)
      return;

   // Сохраним центр видимой области

    _VISIBLE_CENTER_
   // Изменим масштаб
   m_Scale = ZoomFactor;

   if ( m_Bitmap->Height * m_Scale > Height )
      m_StartPoint.y = Cy - Height / (2*m_Scale);
   else
      m_StartPoint.y = 0;

   if ( m_Bitmap->Width * m_Scale > Width )
      m_StartPoint.x = Cx - Width / (2*m_Scale);
   else
      m_StartPoint.x = 0;

   int Cx1, Cy1;
   if ( m_Bitmap->Height * m_Scale >= Height )
       Cy1 = m_StartPoint.y + Height / (2*m_Scale);
   else
       Cy1 = m_StartPoint.y + m_Bitmap->Height / 2;

   if ( m_Bitmap->Width * m_Scale >= Width )
       Cx1 = m_StartPoint.x + Width / (2*m_Scale);
   else
       Cx1 = m_StartPoint.x + m_Bitmap->Width / 2;
   m_StartPoint.x += Cx - Cx1;
   m_StartPoint.y += Cy - Cy1;

   m_StartPoint.y = ( m_StartPoint.y < 0 ) ? 0 : m_StartPoint.y;
   m_StartPoint.y = ( m_StartPoint.y + GetHeightToDisplay()/m_Scale > m_Bitmap->Height ) ?
                     m_Bitmap->Height - GetHeightToDisplay()/m_Scale : m_StartPoint.y;
   m_StartPoint.x = ( m_StartPoint.x < 0 ) ? 0 : m_StartPoint.x;
   m_StartPoint.x = ( m_StartPoint.x + GetWidthToDisplay()/m_Scale > m_Bitmap->Width ) ?
                     m_Bitmap->Width - GetWidthToDisplay()/m_Scale : m_StartPoint.x;
   Paint();


   if (m_ScaleChange)
      m_ScaleChange(this);
}

/*
    Function: TFCustomImage::ZoomToRect(const TRect Rect)
    Purpose:  Размещает выбранный внутри изображения прямоугольный фрагмент
              таким образом, чтобы он полностью поместился внутри
              клиетской области компонента.
    Comments:
*/
void __fastcall TPhImage::ZoomToRect(const TRect Rect)
{
   if ( Empty )
      return;

   if ( Rect.Width() <= 0 || Rect.Height() <= 0 )
      return;

   /* Переместим центр */
   int Cx = GetImageX( Rect.Left ) + Rect.Width()/(2*m_Scale);
   int Cy = m_Bitmap->Height - GetImageY( Rect.Top ) - Rect.Height()/(2*m_Scale);

   /* Новый масштаб */
   float alfa_c = (float)Width / (float)Height;
   float alfa_r = (float)Rect.Width() / (float)Rect.Height();
   
   if (alfa_c >= alfa_r)
      m_Scale = (float)Height * m_Scale / Rect.Height();
   else
      m_Scale = (float)Width * m_Scale / Rect.Width();
   m_Scale = RoundTo( m_Scale, -2 );

   /* Сместим угол относительно центра */
   if ( m_Bitmap->Height * m_Scale > Height )
      m_StartPoint.y = Cy - Height / (2*m_Scale);
   else
      m_StartPoint.y = 0;

   if ( m_Bitmap->Width * m_Scale > Width )
      m_StartPoint.x = Cx - Width / (2*m_Scale);
   else
      m_StartPoint.x = 0;

   /* Скорректируем выходы за границы */
   m_StartPoint.y = ( m_StartPoint.y < 0 ) ? 0 : m_StartPoint.y;
   m_StartPoint.y = ( m_StartPoint.y + GetHeightToDisplay()/m_Scale > m_Bitmap->Height ) ?
                     m_Bitmap->Height - GetHeightToDisplay()/m_Scale : m_StartPoint.y;
   m_StartPoint.x = ( m_StartPoint.x < 0 ) ? 0 : m_StartPoint.x;
   m_StartPoint.x = ( m_StartPoint.x + GetWidthToDisplay()/m_Scale > m_Bitmap->Width ) ?
                     m_Bitmap->Width - GetWidthToDisplay()/m_Scale : m_StartPoint.x;

   Paint();

   if (m_ScaleChange)
      m_ScaleChange(this);
}

//--------------------------------Pane support--------------------------------
/*
    Function: TFCustomImage::MoveToCenter()
    Purpose:   Совмещает центр изображения с центром компонента
    Comments:
*/
void __fastcall TPhImage::MoveToCenter()
{
   if (Empty)
      return;
   if (GetHeightToDisplay() < Height && GetWidthToDisplay() < Width)
      return;

   m_StartPoint.x = m_Bitmap->Width/2 - Width/(2*m_Scale);
   m_StartPoint.y = m_Bitmap->Height/2 - Height/(2*m_Scale);

   Paint();
   if (m_PosChange)
    m_PosChange(this);
}
/*
    Function:   TFCustomImage::MoveToLeftTop()
    Purpose:    Совмещает верхний левый угол изображения и верхний левый угол
                компонента
    Comments:   Данная операция применима, если размеры изображения больше
                размеров клинетской области компонента
*/
void __fastcall TPhImage::MoveToLeftTop()
{
   if (Empty)
      return;
   if (GetHeightToDisplay() < Height && GetWidthToDisplay() < Width)
      return;

   m_StartPoint.x = 0;
   m_StartPoint.y = m_Bitmap->Height - Height/m_Scale;
   Paint();
   if (m_PosChange)
    m_PosChange(this);
}

/*
    Function: MoveToRightBottom()
    Purpose:  Совмещает нижний правый угол изображения и нижний правый угол
              компонента
    Comments: Данная операция применима, если размеры изображения больше
              размеров клинетской области компонента
*/
void __fastcall TPhImage::MoveToRightBottom()
{
   if (Empty)
      return;
   if (GetHeightToDisplay() < Height && GetWidthToDisplay() < Width)
      return;

   m_StartPoint.x = m_Bitmap->Width - Width/m_Scale;
   m_StartPoint.y = 0;

   Paint();
   if (m_PosChange)
    m_PosChange(this);
}

/*  ----------------------------------------------------------
    Function:   TFCustomImage::MoveBy(int in_dX, int in_dY)
    Purpose:    Смещает центр изображения на dX,dY
    Comments:
*/
void __fastcall TPhImage::MoveBy(int in_dX, int in_dY)
{
   if (Empty)
      return;

   double dX = RoundTo( (double)in_dX / RoundTo(m_Scale, -2), 0 );
   double dY = RoundTo( (double)in_dY / RoundTo(m_Scale, -2), 0 );
   
   if ( m_Bitmap->Width * m_Scale <= Width &&
        m_Bitmap->Height * m_Scale <= Height )
      return;

   if ( m_Bitmap->Width * m_Scale > Width )
      if ( m_StartPoint.x + dX > m_Bitmap->Width - Width/m_Scale )
         m_StartPoint.x = m_Bitmap->Width - Width/m_Scale;
      else if (m_StartPoint.x + dX < 0)
         m_StartPoint.x = 0;
      else
         m_StartPoint.x += dX;
         
   if ( m_Bitmap->Height * m_Scale > Height )
      if ( m_StartPoint.y - dY > m_Bitmap->Height - Height/m_Scale )
         m_StartPoint.y = m_Bitmap->Height - Height/m_Scale;
      else if (m_StartPoint.y - dY < 0)
         m_StartPoint.y = 0;
      else
         m_StartPoint.y -= dY;

   Paint();
   if (m_PosChange)
    m_PosChange(this);
}

/*  ----------------------------------------------------------
    Function:   TFCustomImage::MoveTo(int X, int Y)
    Purpose:    Смещает центр изображения в точку (X,Y)
    Comments:
*/
void __fastcall TPhImage::MoveTo(int X, int Y)
{
   if (Empty)
      return;

   // Изображение полностью влазит на компонент
   if (GetHeightToDisplay() < Height && GetWidthToDisplay() < Width)
      return;

   unsigned int x,y; // внутренние координаты
   x = X;
   y = m_Bitmap->Height - Y;

   m_StartPoint.x = x - Width/(2*m_Scale);
   m_StartPoint.y = y - Height/(2*m_Scale);

   if ( m_StartPoint.x + Width/m_Scale > m_Bitmap->Width )
      m_StartPoint.x = m_Bitmap->Width - Width/m_Scale;
   else if ( m_StartPoint.x < 0 )
      m_StartPoint.x = 0;

   if ( m_StartPoint.y + Height/m_Scale > m_Bitmap->Height )
      m_StartPoint.y = m_Bitmap->Height - Height/m_Scale;
   if ( m_StartPoint.y < 0 )
      m_StartPoint.y = 0;
      
   Paint();
   if (m_PosChange)
    m_PosChange(this);
}

/*  ----------------------------------------------------------
    Function:   TFCustomImage::GetWidthToDisplay()
    Purpose:    Вычисляет ширину отображаемой области с учетом коэффициента
                масштабирования.
    Comments:
*/
int TPhImage::GetWidthToDisplay() const
{
   if ( m_Bitmap->Width * m_Scale > Width )
      return Width;
   else
      return (int)(m_Bitmap->Width * m_Scale);
}

/*  ----------------------------------------------------------
    Function:   TFCustomImage::GetHeightToDisplay()
    Purpose:    Вычисляет высоту отображаемой области с учетом коэффициента
                масштабирования.
    Comments:
*/
int TPhImage::GetHeightToDisplay() const
{
   if ( m_Bitmap->Height * m_Scale > Height )
      return Height;
   else
      return m_Bitmap->Height * m_Scale;
}

/*  ----------------------------------------------------------
    Function:   TFCustomImage::GetInternalVisibleArea()
    Purpose:    Вычисляет видимую область изображения во внутреннем
                представлении, т.е. перевернутую по Y
    Comments:
*/
TRect __fastcall TPhImage::GetInternalVisibleArea()
{
    TRect area;

    area.Top    = m_StartPoint.y;
    area.Bottom = RoundTo( area.Top + GetHeightToDisplay() / m_Scale, 0 );

    area.Left   = m_StartPoint.x;
    area.Right  = RoundTo( area.Left + GetWidthToDisplay() / m_Scale, 0 );

    return area;
}

void   TPhImage::DrawSelRect(Graphics::TBitmap *bm)
{
   if (m_SelRect.Width() == 0 || m_SelRect.Height() == 0)
    return;

   if (bm == NULL)
        return;
   TRect FSelRect1 = GetScreenRect(m_SelRect);
   TPenStyle style = bm->Canvas->Pen->Style;
   TColor color = bm->Canvas->Pen->Color;
   TPenMode mode = bm->Canvas->Pen->Mode;
   int pwidth = bm->Canvas->Pen->Width;
   bm->Canvas->Pen->Style = psDot;
   bm->Canvas->Pen->Width = 4;
   bm->Canvas->Pen->Color = clRed;
   bm->Canvas->Pen->Mode = pmCopy;

   bm->Canvas->MoveTo(FSelRect1.Left, FSelRect1.Top);
   bm->Canvas->LineTo(FSelRect1.Right, FSelRect1.Top);
   bm->Canvas->LineTo(FSelRect1.Right, FSelRect1.Bottom);
   bm->Canvas->LineTo(FSelRect1.Left, FSelRect1.Bottom);
   bm->Canvas->LineTo(FSelRect1.Left, FSelRect1.Top);

   bm->Canvas->Pen->Style = style;
   bm->Canvas->Pen->Color = color;
   bm->Canvas->Pen->Mode = mode;
   bm->Canvas->Pen->Width = pwidth;
}

void __fastcall 	TPhImage::ClearSelection()
{
    m_SelRect.Left = 0;
    m_SelRect.Top = 0;
    m_SelRect.Right = 0;
    m_SelRect.Bottom = 0;
    if (this->m_SelectedBitmap != NULL)
    {
        delete this->m_SelectedBitmap;
        this->m_SelectedBitmap = NULL;
	}
	if (this->m_Selection) {
		this->m_Selection(this);
	}
    Paint();
}

bool __fastcall		   TPhImage::HasSelection()
{
	if (m_SelRect.Width() == 0 && m_SelRect.Height() == 0)
    	return false;
    else
    	return true;
}

TRect						TPhImage::GetSelRect()
{
	return m_SelRect;
}

void                        TPhImage::SetSelRect(TRect r)
{
     m_SelRect = r;
     TDIBImage* dib = dynamic_cast<TDIBImage*>(m_Bitmap);
     if (dib != NULL)
     {
        // clear selected bitmap
        if (this->m_SelectedBitmap != NULL)
        {
            delete this->m_SelectedBitmap;
            this->m_SelectedBitmap = NULL;
        }

        m_SelectedBitmap = new TDIBImage();

        awpImage* src = NULL;
        awpImage* sel = NULL;
        TRect sel_rect = m_SelRect;
        awpRect sel_awprect;
        sel_awprect.left = sel_rect.left;
        sel_awprect.top = sel_rect.top;
        sel_awprect.right = sel_awprect.left + sel_rect.Width();
        sel_awprect.bottom = sel_awprect.top + sel_rect.Height();

        //
        dib->GetAWPImage(&src);
        if (src != NULL)
        {
            awpCopyRect(src, &sel, &sel_awprect);
            if (sel != NULL)
            {

                TDIBImage* sel_image = dynamic_cast<TDIBImage*>(m_SelectedBitmap);
                sel_image->SetAWPImage(sel);
	            _AWP_SAFE_RELEASE_(sel)
            }
            _AWP_SAFE_RELEASE_(src)
        }
	 }
	if (this->m_Selection) {
		this->m_Selection(this);
	}
     this->Paint();
}

/*  ----------------------------------------------------------
	Function: TFCustomImage::Paint(void)
	Purpose:  Отображает растровые данные и границы выбранного фрагмента
			  в клиентской области компонента
	Comments:
*/
void __fastcall TPhImage::Paint(void)
{
	if (!m_Bitmap->Empty)
	{
	   Graphics::TBitmap *bm = new Graphics::TBitmap();
	   bm->Width  = Width;
	   bm->Height = Height;
	   bm->Canvas->Brush->Color = clBlack;
	   bm->Canvas->FillRect(ClientRect);
	   TRect src_r, dst_r;
	   dst_r.Left   = 0;
	   dst_r.Right  = GetWidthToDisplay();
	   dst_r.Top    = 0;
	   dst_r.Bottom = GetHeightToDisplay();

	   // Center image
	   if (dst_r.Right < Width)
	   {
		  dst_r.Left  = (Width - dst_r.Right) / 2;
		  dst_r.Right = dst_r.Left + dst_r.Right;
	   }
	   if (dst_r.Bottom < Height)
	   {
		  dst_r.Top    = (Height - dst_r.Bottom) / 2;
		  dst_r.Bottom = dst_r.Top + dst_r.Bottom;
	   }

	   src_r = GetInternalVisibleArea();
	   TDIBImage* dib = dynamic_cast<TDIBImage*>(m_Bitmap);
	   unsigned char* pDIB = dib->OpenPixels();

	   ::SetStretchBltMode(bm->Canvas->Handle, HALFTONE);
	   ::StretchDIBits(bm->Canvas->Handle,
		  dst_r.Left,
		  dst_r.Top,
		  dst_r.Width(),
		  dst_r.Height(),
		  src_r.Left,
		  src_r.Top,
		  src_r.Width(),
		  src_r.Height(),
		  pDIB,
		  &(dib->DIBInfo),
		  DIB_RGB_COLORS,
		  SRCCOPY);

		DrawSelRect(bm);
		DrawSelectedItems(bm, m_xx, m_yy);
        // todo: add drawing
        if (PhTool != NULL)
		  PhTool->Draw(bm->Canvas);

	   dib->ClosePixels();
	   Canvas->CopyMode = cmSrcCopy;
	   Canvas->Draw(0,0,bm);
	   delete bm;
	}
	else
	{
		TColor oldColor = this->Canvas->Brush->Color;
		this->Canvas->Brush->Color = clBlack;
		this->Canvas->FillRect(this->ClientRect);
		this->Canvas->Brush->Color = oldColor;
	}


	if (m_Paint != NULL)
	{
		m_Paint(this);
	}
}

/*  ----------------------------------------------------------
*/
int __fastcall TPhImage::GetImageX(int ScreenX)
{
   if (m_Bitmap == NULL)
    return -1;

   int display_w = GetWidthToDisplay();
   int left, right;

   if ( display_w < Width )
   {
      left = (Width - display_w) / 2;
      right = left + display_w;

      if ( ScreenX < left || ScreenX >= right )
         return -1;
   }
   else
   {
      left = 0;
      right = display_w;
   }

   double sx = RoundTo( m_StartPoint.x + (double)(ScreenX - left) / m_Scale, 0 );

   return ( sx >= 0 && sx <= m_Bitmap->Width ) ? sx : -1;
}

/*  ----------------------------------------------------------
*/
int __fastcall TPhImage::GetImageY( int ScreenY )
{
   if (m_Bitmap == NULL)
    return -1;

   int display_h = GetHeightToDisplay();
   int top, bottom;
   int sY = Height - ScreenY;

   if ( display_h < Height )
   {
      top    = ( Height - display_h ) / 2;
      bottom = top + display_h;

      if ( sY <= top || sY > bottom )
         return -1;
   }
   else
   {
      top = 0;
      bottom = display_h;
   }

   double sy  = RoundTo( m_Bitmap->Height - m_StartPoint.y - (double)(sY - top) / m_Scale, 0 );

   return (sy >= 0 && sy <= m_Bitmap->Height) ? sy : -1;
}

/*  ----------------------------------------------------------
*/
TRect __fastcall TPhImage::GetImageRect(TRect ScreenR)
{
    TRect ImageR = TRect(-1, -1, -1, -1);

    if ( !Empty )
    {
        ImageR.Left   = GetImageX( ScreenR.Left );
        ImageR.Right  = GetImageX( ScreenR.Right );
        ImageR.Top    = GetImageY( ScreenR.Top );
        ImageR.Bottom = GetImageY( ScreenR.Bottom );
    }

    return ImageR;
}
TPoint   __fastcall         TPhImage::GetImagePoint(int x, int y)
{
	TPoint result;
     result.x = 0;
	 result.y = 0;
	 if (Empty)
		return result;
	 result.x = this->GetImageX(x);
     result.y = this->GetImageY(y);
	 return result;
}
TPoint   __fastcall         TPhImage::GetScreenPoint(int x, int y)
{
     TPoint result;
	 result.x = 0;
	 result.y = 0;
	 if (Empty)
		return result;
    int dx = 0;
    int dy = 0;

    if ( GetWidthToDisplay() < Width )
        dx = (Width - GetWidthToDisplay()) / 2;

    if ( GetHeightToDisplay() < Height )
        dy = (Height - GetHeightToDisplay()) / 2;

    result.x    =  (x - Corner.x)*m_Scale + dx;
    result.y    =  (y - Corner.y)*m_Scale + dy;//m_StartPoint.y + m_Scale*ImageR.Top;//GetImageY( ScreenR.Top );

    return result;
}
TRect	 __fastcall  		TPhImage::GetScreenRect(TRect ImageR)
{
   TRect ScreenR = TRect(-1, -1, -1, -1);

    int dx = 0;
    int dy = 0;
    if ( GetWidthToDisplay() < Width )
        dx = (Width - GetWidthToDisplay()) / 2;

    if ( GetHeightToDisplay() < Height )
        dy = (Height - GetHeightToDisplay()) / 2;

    if ( !Empty )
    {
        ScreenR.Left   = (ImageR.Left - Corner.x)*m_Scale + dx;
        ScreenR.Right  = (ImageR.Right - Corner.x)*m_Scale + dx;//m_StartPoint.x + m_Scale*ImageR.Right;//GetImageX( ScreenR.Right );
        ScreenR.Top    = (ImageR.Top - Corner.y)*m_Scale + dy;//m_StartPoint.y + m_Scale*ImageR.Top;//GetImageY( ScreenR.Top );
        ScreenR.Bottom = (ImageR.Bottom - Corner.y)*m_Scale + dy;//GetImageY( ScreenR.Bottom );
    }
    return ScreenR;
}

 /*  ----------------------------------------------------------
    Function:   TFCustomImage::GetVisibleArea()
    Purpose:    Вычисляет видимую область изображения
    Comments:
*/
TRect __fastcall TPhImage::GetVisibleArea() const
{
    TRect area;

    if ( m_Bitmap->Height * m_Scale > Height )
        area.Top = m_Bitmap->Height - Height/m_Scale - m_StartPoint.y;
    else
        area.Top = 0;
    area.Bottom = area.Top + GetHeightToDisplay() / m_Scale;
    if (m_Bitmap->Width * m_Scale > Width)
        area.left = m_Bitmap->Width - Width/m_Scale - m_StartPoint.x;
    else
        area.left = 0;
    area.Right  = area.Left + GetWidthToDisplay() / m_Scale;

    return area;
}
/*TFCustomImage-------------------------------------------------------------------
    Function: TFCustomImage::LoadFromFile(const AnsiString& FileName)
    Purpose:  Загружает растровые данные из файла. Создает обект типа
              TImportRaster. Генерирует событие BeforeOpen, переводит
              объект в состояние Loading.
    Comments:
---------------------------------------------------------------------------*/
bool  __fastcall TPhImage::LoadFromFile(const char* lpFileName)
{

      if (m_BeforeOpen)
         m_BeforeOpen(this);
      try
      {
	      m_Bitmap->LoadFromFile(lpFileName);
      }
      catch(Exception& e)
      {
        ShowMessage(e.Message);
        return false;
      }

      // setup filename
      m_FileName = lpFileName;

      if (m_OnFrame)
      {
		  TDIBImage* dib = dynamic_cast<TDIBImage*>(m_Bitmap);
          m_OnFrame(this, dib);
      }

      if (m_OnFrameData)
      {
		  TDIBImage* dib = dynamic_cast<TDIBImage*>(m_Bitmap);
          awpImage* img = NULL;
          dib->GetAWPImage(&img);
          if (img != NULL)
          {
            //
            m_OnFrameData(this, img->sSizeX, img->sSizeY, img->bChannels, (unsigned char*)img->pPixels);
            dib->SetAWPImage(img);
            _AWP_SAFE_RELEASE_(img)
          }
      }

      if (m_AfterOpen)
          m_AfterOpen(this);
      if (m_Change)
          m_Change(this);

      return true;
}

/*TFCustomImage--------------------------------------------------------------------
	Function: TFCustomImage::SaveToFile(const AnsiString& FileName)
    Purpose:  Save image to file.
    Comments: Default save image in JPEG format
---------------------------------------------------------------------------*/
void __fastcall TPhImage::SaveToFile(const LPWSTR lpwFileName)
{
	   UnicodeString FileName = lpwFileName;

	   TDIBImage* dib = dynamic_cast<TDIBImage*>(m_Bitmap);
	   dib->SaveToFile(FileName);

	   this->m_modified = false;
		m_FileName = lpwFileName;
		if (m_Change)
			m_Change(this);

	   return;
}
/*TFCustomImage--------------------------------------------------------------------
	Function: TFCustomImage::LoadFromClipboard()
	Purpose:  Импортирует изображение из буфера обмена.
	Comments:
---------------------------------------------------------------------------*/
void __fastcall TPhImage::LoadFromClipboard()
{
   m_Bitmap->Assign(Clipboard());
   this->m_modified = true;
   m_FileName = "";
    if (m_Change)
        m_Change(this);
   Paint();
}
/*TFCustomImage--------------------------------------------------------------------
    Function: TFCustomImage::SaveToClipBoard()
    Purpose: Записывает изображение в буфер обмена
    Comments:
---------------------------------------------------------------------------*/
void __fastcall TPhImage::SaveToClipBoard()
{
   if (!Empty)
   {
      if (this->HasSelection())
          Clipboard()->Assign(this->SelectedBitmap);
      else
	      Clipboard()->Assign(m_Bitmap);
   }
}
/*TFCustomImage--------------------------------------------------------------------
    Function:
    Purpose:
    Comments:
---------------------------------------------------------------------------*/
bool __fastcall TPhImage::GetModified()
{
        return this->m_modified;
}
/*TFCustomImage-----------------------------------------------------------------
    Function:  TFCustomImage::GetCorner()
    Purpose:    Смещение центра изображения относительно центра компонента.
    Comments:
---------------------------------------------------------------------------*/
TPoint __fastcall TPhImage::GetCorner() const
{
   TPoint C( -1, -1 );

   if (!Empty)
   {
      C.x = m_StartPoint.x;
      if ( m_Bitmap->Height * m_Scale > Height )
          C.y = RoundTo( m_Bitmap->Height - Height/m_Scale - m_StartPoint.y, 0 );
      else
          C.y = 0;
   }

   return C;
}
/*TFCustomImage--------------------------------------------------------------------
    Function:
    Purpose:
    Comments:
---------------------------------------------------------------------------*/
bool __fastcall TPhImage::GetEmpty() const
{
    try
    {
	   if (m_Bitmap != NULL)
		   return m_Bitmap->Empty;
       else
		   return true;
    }
	catch (...)
    {
       return true;
    }
}

void __fastcall  TPhImage::SetEmpty(bool value)
{
    if (m_Bitmap != NULL)
    {
        m_Bitmap->Assign(NULL);
        Paint();
        FileName = "";
        m_modified = false;
        if (m_Change)
	    	m_Change(this);
    }
}
//---------------------------------------------------------------------------
void __fastcall TPhImage::DlgMessage( TWMGetDlgCode &Message )
{
	Message.Result = DLGC_WANTARROWS;
}
void __fastcall             TPhImage::HookKey(Word &Key, Classes::TShiftState Shift)
{
	KeyDown(Key, Shift);
}
void __fastcall 	TPhImage::WMGetDlgCode(TWMNoParams &Message)
{
		Message.Result |= DLGC_WANTARROWS;
}
//---------------------------------------------------------------------------
void __fastcall TPhImage::KeyDown(Word &Key, Classes::TShiftState Shift)
{
    int step = 3;
    if ( Shift.Contains( ssCtrl ) )
    {
        step *= 10;
        if ( Key == 'V' )
            LoadFromClipboard();
        if ( Key == 'C' || Key == VK_INSERT )
            SaveToClipBoard();
    }
    if ( Shift.Contains( ssShift ) )
    {
        if ( Key == VK_INSERT )
            LoadFromClipboard();
    }
    
    switch ( Key )
    {
    case VK_UP:
        MoveBy(0, -step);
        break;

    case VK_DOWN:
        MoveBy(0, step);
        break;

    case VK_LEFT:
        MoveBy(-step, 0);
        break;

    case VK_RIGHT:
        MoveBy(step, 0);
        break;

    case VK_ADD:
        ZoomIn();
        break;

    case VK_SUBTRACT:
        ZoomOut();
        break;

    case VK_MULTIPLY:
        BestFit();
        break;

    case VK_DIVIDE:
        ActualSize();
        break;
    }

}
void __fastcall 	TPhImage::DblClick(void)
{

   if (GetKeyState(VK_LSHIFT) < 0)
   {
       if (Mosaic && this->m_idx >= 0 && this->m_idx < this->Frames->Count)
	   {
			Frames->GoFrame(m_idx);
			Mosaic = false;
       }
	   else
			Mosaic = true;
   }

  TCustomControl::DblClick();
}
//---------------------------------------------------------------------------
bool __fastcall TPhImage::DoMouseWheel(System::Classes::TShiftState Shift, int WheelDelta, const System::Types::TPoint &MousePos)
{
	if (WheelDelta > 0)
    {
		ZoomIn(MousePos.x, MousePos.y);
	}
	else
	{
		ZoomOut(MousePos.x, MousePos.y);
    }
    return true;
}
bool  __fastcall TPhImage::DoMouseWheelUp(TShiftState Shift, const TPoint &MousePos)
{
   ZoomIn();

   TCustomControl::DoMouseWheelUp(Shift, MousePos);
   return true;
}
//---------------------------------------------------------------------------

bool  __fastcall TPhImage::DoMouseWheelDown(TShiftState Shift, const TPoint &MousePos)
{
   ZoomOut();

   TCustomControl::DoMouseWheelDown(Shift, MousePos);
   return true;
}
//---------------------------------------------------------------------------
void __fastcall TPhImage::MouseDown(TMouseButton Button,  TShiftState Shift, Integer X, Integer Y)
{
   SetFocus();
   TPhImageTool* t =    GetSelectedTool();
   if (t != NULL)
	t->MouseDown(X,Y, Button);
   TCustomControl::MouseDown(Button, Shift,X,Y);
}
//---------------------------------------------------------------------------
void __fastcall TPhImage::MouseMove( TShiftState Shift, Integer X, Integer Y)
{
    if (this->Mosaic && Shift.Contains(ssShift))
    {
        // call mosaic helper
        int x = GetImageX(X);
        int y = GetImageY(Y);
        if (x >= 0 && x < m_Bitmap->Width && y >= 0 && y < m_Bitmap->Height)
        {
            x /= m_tWidth;
            y /= m_tHeight;
            int w = (int)floor(sqrt((float)this->Frames->Count) + 0.5);
            int idx = (x+w*y);
            if (idx != m_idx && idx < Frames->Count)
            {
                m_xx = X;
                m_yy = Y;
                m_idx = idx;
                Paint();
            }
        }
    }

   TPhImageTool* t =    GetSelectedTool();
   if (t != NULL)
    t->MouseMove(X,Y, Shift);

    TCustomControl::MouseMove(Shift,X,Y);
}
//---------------------------------------------------------------------------
void __fastcall TPhImage::MouseUp(TMouseButton Button,  TShiftState Shift, Integer X, Integer Y)
{

    if (this->Mosaic && Shift.Contains(ssShift))
    {
        // call mosaic helper
        int x = GetImageX(X);
        int y = GetImageY(Y);
        if (x >= 0 && x < m_Bitmap->Width && y >= 0 && y < m_Bitmap->Height)
        {
            x /= m_tWidth;
            y /= m_tHeight;
            int w = (int)floor(sqrt((float)this->Frames->Count) + 0.5);
            int idx = (x+w*y);
            if (idx < Frames->Count)
            {
                SFrameItem* item = Frames->GetFrameItem(idx);
                if (item != NULL)
                {
                    item->selected = !item->selected;
                    Paint();
                }
            }
        }
    }


   TPhImageTool* t =    GetSelectedTool();
   if (t != NULL)
	t->MouseUp(X,Y, Button);
    TCustomControl::MouseUp(Button, Shift,X,Y);
}

//---------------------------------------------------------------------------
void __fastcall TPhImage::SetImage(TGraphic* aBitmap)
{
    TDIBImage* dib = dynamic_cast<TDIBImage*>(m_Bitmap);
    dib->Assign(aBitmap);
    if (m_Change)
      m_Change(this);
}

TGraphic* __fastcall   TPhImage::GetSelectedBitmap()
{
    return m_SelectedBitmap;
}
//todo: working with tools
int	 __fastcall TPhImage::GetSelectedIndex()
{
 /*	TThumbSelectTool* tool = dynamic_cast<TThumbSelectTool*>(FTool);
	if (tool != NULL)
		return tool->GetLastSelected();
	else
		return -1; */
}

void __fastcall  TPhImage::AddPhTool(TPhImageTool* tool)
{
	if (tool != NULL)
	{
		if (m_ph_tools->IndexOf(tool) < 0)
            m_ph_tools->Add(tool);
	}
}
void __fastcall  TPhImage::RemovePhTool(TPhImageTool* tool)
{
   if (m_ph_tools->IndexOf(tool) > 0)
	m_ph_tools->Remove(tool);
}
void __fastcall   TPhImage::SelectPhTool(TPhImageTool* tool)
{
	int old_selected_tool_index = m_selected_ph_tool;

	if (tool == NULL)
		this->m_selected_ph_tool = 0;
	else if (this->m_ph_tools->IndexOf(tool) >= 0)
		this->m_selected_ph_tool = this->m_ph_tools->IndexOf(tool);
	else
		this->m_selected_ph_tool = 0;

	if (old_selected_tool_index != m_selected_ph_tool)
	{
	   TPhImageTool* t = (TPhImageTool*)this->m_ph_tools->Items[old_selected_tool_index];
	   if (t != NULL)
			t->SetActive(false);
	   t = (TPhImageTool*)this->m_ph_tools->Items[m_selected_ph_tool];
	   if (t != NULL)
			t->SetActive(true);
	}

    if (m_ToolChange != NULL)
        m_ToolChange(this);
}

TPhImageTool* __fastcall    TPhImage::GetSelectedTool()
{
	if (m_selected_ph_tool < m_ph_tools->Count)
		return (TPhImageTool*)this->m_ph_tools->Items[m_selected_ph_tool];
	else
		return NULL;
}

void            __fastcall TPhImage::DrawSelectedItems(Graphics::TBitmap* bm, int xx, int yy)
{
    if (!Mosaic)
        return;

	int x = GetImageX(xx);
	int y = GetImageY(yy);

	x /= m_tWidth;
	y /= m_tHeight;

	x*=m_tWidth;
	y*=m_tHeight;

	TRect rect;
	rect.init(x,y,x+m_tWidth,y+m_tHeight);
	TRect rect1 = GetScreenRect(rect);
    TCanvas* cnv = bm->Canvas;
	TColor 			oldColor = cnv->Pen->Color;
	TBrushStyle     oldStyle = cnv->Brush->Style;
	int 			oldWidth = cnv->Pen->Width;
	cnv->Pen->Color = clYellow;
	cnv->Pen->Width = 3;
 	cnv->Brush->Style = bsClear;

	cnv->Rectangle(rect1);

	// draw selected
  	int w = (int)floor(sqrt((float)Frames->Count) + 0.5);
	for (int i = 0; i < Frames->Count; i++)
	{
		SFrameItem* item = Frames->GetFrameItem(i);

		if (item->selected)
		{
			x = i % w;
			y = i / w;
			x *= m_tWidth;
			y *= m_tHeight;

			rect.init(x,y,x+m_tWidth,y+m_tHeight);
			rect1 = GetScreenRect(rect);
			cnv->Pen->Width = 4;
			cnv->Pen->Color = clRed;

			cnv->Rectangle(rect1);
			rect1.Inflate(-16,-16);
			cnv->Rectangle(rect1);
		}
	}

	cnv->Brush->Style = oldStyle;
	cnv->Pen->Color = oldColor;
	cnv->Pen->Width = oldWidth;
}

void __fastcall TPhImage::DoDeleteImage()
{
    if (Frames->Count > 1)
    {
        Frames->DeleteImage(Frames->CurrentFrame);
        Frames->Next();
    }
    else
    {
        DeleteFile(FileName);
        Close();
    }
}

void __fastcall TPhImage::Delete()
{
    SlideShow = false;
    if (Mosaic)
        Frames->DeleteSelected();
    else
        DoDeleteImage();
}

void __fastcall     TPhImage::Copy(const LPWSTR lpwFolderName)
{
	SlideShow = false;
	UnicodeString FolderName = lpwFolderName;
    if (DirectoryExists(FolderName))
    {
        if (Mosaic && Frames->Count > 1)
		{
			if (!Frames->CopySelected(lpwFolderName))
                ShowMessage(L"Cannot copy files to target folder: " + FolderName);
        }
		else
		{
			if (!PhCopyFile(FileName, FolderName,m_copyAction))
			{
			  ShowMessage(L"Cannot copy files to target folder: " + FolderName);
			}
		}
	}
	else
		ShowMessage(L"Target folder does not exists.");
}
void __fastcall     TPhImage::Move(const LPWSTR lpwFolderName)
{
    SlideShow = false;
    UnicodeString FolderName = lpwFolderName;
    if (DirectoryExists(FolderName))
    {
        if (Frames->Count > 1)
        {
			if (!Frames->MoveSelected(lpwFolderName))
                ShowMessage(L"Cannot move files to target folder: " + FolderName);
        }
        else
        {
			UnicodeString strSrcFile = FileName;
			UnicodeString strDstFile = lpwFolderName;
            strDstFile += ExtractFileName(strSrcFile);
			if (CopyFile(strSrcFile.c_str(), strDstFile.c_str(), false) != S_OK)
            {
				ShowMessage(L"Cannot copy files to target folder: " + FolderName);
            }
            DeleteFile(strSrcFile);
            Close();
        }
    }
    else
        ShowMessage(L"Target folder does not exists.");
}

void __fastcall TPhImage::Cancel()
{
    Frames->Cancel();
}

void __fastcall TPhImage::SetImageData(int w, int h, int c, unsigned char* data)
{
    awpImage img;
    img.nMagic = AWP_MAGIC2;
    img.sSizeX = w;
    img.sSizeY = h;
    img.bChannels = c;
    img.dwType = AWP_BYTE;
    img.pPixels = data;
    if (m_OnFrameData != NULL)
    {
        m_OnFrameData(this, w,h,c,data);
    }

    TDIBImage* dib = dynamic_cast<TDIBImage*>(m_Bitmap);
    if (dib != NULL)
    {
		dib->SetAWPImage(&img);
        Paint();
		//BestFit();
    }
}

bool __fastcall TPhImage::GetAwpImage(awpImage** img)
{
    TDIBImage* dib = dynamic_cast<TDIBImage*>(m_Bitmap);
    if (dib == NULL)
        return false;
    dib->GetAWPImage(img);
    return *img != NULL;
}

bool __fastcall   TPhImage::GetSelectedImage(awpImage** img)
{
    TDIBImage* dib = dynamic_cast<TDIBImage*>(m_SelectedBitmap);
    if (dib == NULL)
        return false;
    dib->GetAWPImage(img);
	return *img != NULL;
}

bool __fastcall TPhImage::SetAwpImage(awpImage* img)
{
	TDIBImage* dib = dynamic_cast<TDIBImage*>(m_Bitmap);
	if (dib == NULL)
		return false;
	if (dib != NULL)
	{
		dib->SetAWPImage(img);
		BestFit();
	}
	return true;
}
TGraphic* __fastcall TPhImage::GetBitmap()
{
   if (m_Bitmap != NULL)
	   return m_Bitmap;
   else
       return NULL;
}

//---------------------------------------------------------------------------
namespace Fimage41
{

	void __fastcall PACKAGE Register    ()
	{
		 TComponentClass classes[1] =
		 {
		 __classid(TPhImage)
		 };
		 RegisterComponents("Photon", classes,
		 (sizeof(classes)/sizeof(TComponentClass))-1);
	}
}
//----------------------------------------------------------------------------
