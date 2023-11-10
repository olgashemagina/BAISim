//---------------------------------------------------------------------------
#include <math.hpp>
#pragma hdrstop
#include "DIBImage41.h"
#include "PhLenzTool.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

//-------------------------------
static bool in_Rect ( const TRect* r, const TPoint* p )
{
    if ( p->x <= r->Right && p->x >= r->Left &&
         p->y <= r->Bottom && p->y >= r->Top )
		return true;

    return false;
}

static bool in_Rect ( const TRect* r, int X, int Y )
{
    if ( X <= r->Right && X >= r->Left &&
         Y <= r->Bottom && Y >= r->Top )
        return true;

    return false;
}

__fastcall TPhLenzTool::TPhLenzTool(TComponent* Owner): TPhImageTool(Owner)
{
   m_strToolName = L"LENZ";
   Zoom        = 2;
   LenzSize    = 200;
   Applied     = false;
   FDrawHandle = DrawDibOpen();
}

__fastcall TPhLenzTool::~TPhLenzTool()
{
   DrawDibClose(FDrawHandle);
}

void TPhLenzTool::Draw(TCanvas* Canvas)
{
    // инструмент перемещения ничего не рисует на Canvase
}

void TPhLenzTool::MouseDown(int X, int Y, TMouseButton Button)
{
   if ( Button == mbLeft )
      ApplyLenz(X, Y);
}

void TPhLenzTool::MouseUp(int X, int Y, TMouseButton Button)
{
   if (m_pImage == NULL)
		return;

   if (Button != mbLeft)
      return;

   Applied = false;
   m_pImage->Paint();
}

void TPhLenzTool::MouseMove(int X, int Y, TShiftState Shift)
{
   if (m_pImage == NULL)
		return;

   if ( !Shift.Contains( ssLeft ) || Applied == false )
      return;

   m_pImage->Paint();
   ApplyLenz(X, Y);
}

void TPhLenzTool::ApplyLenz(int X, int Y)
{
   int src_width = (LenzSize/Zoom) * (100/m_pImage->Scale);

   // Calculate lenz position
   TPoint dst_corner;
   dst_corner.x = X - LenzSize/2;
   dst_corner.y = Y - LenzSize/2;

   TRect visible_area  = m_pImage->VisibleArea;

   int realY = RoundTo( m_pImage->Corner.y + Y*100/m_pImage->Scale, 0 );
   int realX = RoundTo( m_pImage->Corner.x + X*100/m_pImage->Scale, 0 );

   if ( m_pImage->Corner.x == 0 && !in_Rect( &visible_area, m_pImage->GetImageX(X), m_pImage->GetImageY(Y) ) )
      return;
   if ( m_pImage->Corner.y == 0 && !in_Rect( &visible_area, m_pImage->GetImageX(X), m_pImage->GetImageY(Y) ) )
      return;

   // Calculate area to zoom on the source image
   TPoint src_corner;
   src_corner.x = realX - src_width/2;
   src_corner.y = realY - src_width/2;

   // All  visible, correct empty areas
   if ( m_pImage->Bitmap->Width*m_pImage->Scale/100 < m_pImage->Width )
   {
      src_corner.x -= (100*m_pImage->Width - m_pImage->Bitmap->Width*m_pImage->Scale)/(2*m_pImage->Scale);
	  if ( src_corner.x < 0 )
         src_corner.x = 0;
      if ( src_corner.x >= m_pImage->Bitmap->Width - src_width )
         src_corner.x = m_pImage->Bitmap->Width - src_width - 1;
   }

   if ( m_pImage->Bitmap->Height*m_pImage->Scale/100 < m_pImage->Height )
   {
      src_corner.y -= (100*m_pImage->Height - m_pImage->Bitmap->Height*m_pImage->Scale)/(2*m_pImage->Scale);
      if ( src_corner.y < 0 )
         src_corner.y = 0;
      if ( src_corner.y >= m_pImage->Bitmap->Height - src_width )
         src_corner.y = m_pImage->Bitmap->Height - src_width - 1;
   }
   TDIBImage* dib = dynamic_cast<TDIBImage*>(m_pImage->Bitmap);
   DrawDibDraw(
        FDrawHandle,
        m_pImage->Canvas->Handle,
		dst_corner.x,                   // x-coord of destination upper-left corner
        dst_corner.y,                   // y-coord of destination upper-left corner
        LenzSize,                       // width of destination rectangle
        LenzSize,                       // height of destination rectangle
        &(dib->DIBInfo.bmiHeader),
        dib->OpenPixels(),
		src_corner.x,                       // x-coord of source upper-left corner
        src_corner.y,                     // y-coord of source upper-left corner
        src_width,           // width of source rectangle
        src_width,           // height of source rectangle
        DDF_HALFTONE
   );
   dib->ClosePixels();

   Applied = true;
}

void TPhLenzTool::Reset()
{
}

namespace Phlenztool
{
	void __fastcall PACKAGE Register()
	{
		TComponentClass classes[1] = {__classid(TPhLenzTool)};
		RegisterComponents(L"Photon", classes, 0);
	}
}