//---------------------------------------------------------------------------
#pragma hdrstop
#include "awpipl.h"
#include "PhSelectRectTool.h"
//---------------------------------------------------------------------------
__fastcall TPhSelRectTool::TPhSelRectTool(TComponent* Owner):TPhImageTool(Owner)
{

   Pressed = false;
   vertex = 0;
   m_strToolName = L"SELECT RECT";
}
void TPhSelRectTool::Draw(TCanvas* Canvas)
{
}
void TPhSelRectTool::MouseDown(int X, int Y, TMouseButton Button)
{
   if (m_pImage == NULL)
        return;

   if (Button == mbLeft)
   {
      m_pImage->m_SelRect.Top    = 0;
      m_pImage->m_SelRect.Bottom = 0;
      m_pImage->m_SelRect.Left   = 0;
      m_pImage->m_SelRect.Right  = 0;
      m_pImage->Paint();
      m_SelRect.Top    = Y;
      m_SelRect.Bottom = Y;
      m_SelRect.Left   = X;
      m_SelRect.Right  = X;
      m_x = X;
      m_y = Y;
      m_pImage->m_SelRect = m_pImage->GetImageRect(m_SelRect);
      Pressed        = true;
   }
}

void TPhSelRectTool::MouseUp(int X, int Y, TMouseButton Button)
{
   if (m_pImage == NULL)
        return;

   if (Button == mbLeft)
   {
      m_SelRect.Left = AWP_MIN(X, m_SelRect.Left);
      m_SelRect.Top  = AWP_MIN(Y, m_SelRect.Top);
      m_SelRect.Right = AWP_MAX(X, m_SelRect.Right);
      m_SelRect.Bottom = AWP_MAX(Y, m_SelRect.Bottom);

      Pressed = false;

      m_pImage->SetSelRect(m_pImage->GetImageRect(m_SelRect));
      m_pImage->Paint();
   }
}
void TPhSelRectTool::MouseMove(int X, int Y, TShiftState Shift)
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

void TPhSelRectTool::DrawSelRect()
{
   if (m_pImage == NULL)
        return;

   TPenStyle style = m_pImage->Canvas->Pen->Style;
   TColor color = m_pImage->Canvas->Pen->Color;
   TPenMode mode = m_pImage->Canvas->Pen->Mode;

   m_pImage->Canvas->Pen->Style = psDot;
   m_pImage->Canvas->Pen->Color = clBlack;
   m_pImage->Canvas->Pen->Mode = pmNotXor;

   m_pImage->Canvas->MoveTo(m_SelRect.Left, m_SelRect.Top);
   m_pImage->Canvas->LineTo(m_SelRect.Right, m_SelRect.Top);
   m_pImage->Canvas->LineTo(m_SelRect.Right, m_SelRect.Bottom);
   m_pImage->Canvas->LineTo(m_SelRect.Left, m_SelRect.Bottom);
   m_pImage->Canvas->LineTo(m_SelRect.Left, m_SelRect.Top);

   m_pImage->Canvas->Pen->Style = style;
   m_pImage->Canvas->Pen->Color = color;
   m_pImage->Canvas->Pen->Mode = mode;
}
void TPhSelRectTool::Reset()
{
}

void __fastcall TPhSelRectTool::SetVertexes(int x1, int x2, int y1, int y2)
{
    m_SelRect.Left  = AWP_MIN(x1,x2);
    m_SelRect.Right = AWP_MAX(x1,x2);
    m_SelRect.Top   = AWP_MIN(y1,y2);
    m_SelRect.Bottom = AWP_MAX(y1,y2);
}

//---------------------------------------------------------------------------
namespace Phselectrecttool
{
	void __fastcall PACKAGE Register()
	{
		TComponentClass classes[1] = {__classid(TPhSelRectTool)};
		RegisterComponents(L"Photon", classes, 0);
	}
}

#pragma package(smart_init)
