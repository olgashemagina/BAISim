//---------------------------------------------------------------------------

#pragma hdrstop
#include "awpipl.h"
#include "PhZoomToRectTool.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

//-------------------------------
__fastcall TPhZoomToRectTool::TPhZoomToRectTool(TComponent* Owner)
	: TPhImageTool(Owner)
{
   Pressed = false;
    m_strToolName = L"ZOOM TO RECT";
}
void TPhZoomToRectTool::Draw(TCanvas* Canvas)
{
    // инструмент перемещения ничего не рисует на Canvase
}

void TPhZoomToRectTool::DrawSelRect()
{
   if (m_pImage == NULL)
        return;

   TPenStyle style = m_pImage->Canvas->Pen->Style;
   TColor color = m_pImage->Canvas->Pen->Color;
   TPenMode mode = m_pImage->Canvas->Pen->Mode;

   m_pImage->Canvas->Pen->Style = psDot;
   m_pImage->Canvas->Pen->Color = clBlack;
   m_pImage->Canvas->Pen->Mode = pmNotXor;

   m_pImage->Canvas->MoveTo(SelRect.Left, SelRect.Top);
   m_pImage->Canvas->LineTo(SelRect.Right, SelRect.Top);
   m_pImage->Canvas->LineTo(SelRect.Right, SelRect.Bottom);
   m_pImage->Canvas->LineTo(SelRect.Left, SelRect.Bottom);
   m_pImage->Canvas->LineTo(SelRect.Left, SelRect.Top);

   m_pImage->Canvas->Pen->Style = style;
   m_pImage->Canvas->Pen->Color = color;
   m_pImage->Canvas->Pen->Mode = mode;
}

void TPhZoomToRectTool::MouseDown(int X, int Y, TMouseButton Button)
{
   if (m_pImage == NULL)
        return;

   if (Button == mbLeft)
   {
      SelRect.Top    = Y;
	  SelRect.Bottom = Y;
      SelRect.Left   = X;
      SelRect.Right  = X;
      m_x = X;
      m_y = Y;
	  Pressed        = true;
   }
}
void TPhZoomToRectTool::MouseUp(int X, int Y, TMouseButton Button)
{
   if (m_pImage == NULL)
        return;

   if (Button == mbLeft)
   {
      this->SetVertexes(m_x, X, m_y, Y);
	  m_pImage->ZoomToRect(SelRect);
      Pressed = false;
   }
}

void TPhZoomToRectTool::MouseMove(int X, int Y, TShiftState Shift)
{
   if (m_pImage == NULL)
        return;

   if (Pressed)
   {
      DrawSelRect();

      this->SetVertexes(m_x, X, m_y, Y);

      DrawSelRect();
   }
}

void TPhZoomToRectTool::Reset()
{
}

void __fastcall TPhZoomToRectTool::SetVertexes(int x1, int x2, int y1, int y2)
{
    SelRect.Left  = AWP_MIN(x1,x2);
    SelRect.Right = AWP_MAX(x1,x2);
    SelRect.Top   = AWP_MIN(y1,y2);
    SelRect.Bottom = AWP_MAX(y1,y2);
}

namespace Phzoomtorecttool
{
	void __fastcall PACKAGE Register()
	{
		TComponentClass classes[1] = {__classid(TPhZoomToRectTool)};
		RegisterComponents(L"Photon", classes, 0);
	}
}
//---------------------------------------------------------------------------