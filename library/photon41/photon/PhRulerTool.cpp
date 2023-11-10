//---------------------------------------------------------------------------

#pragma hdrstop

#include "PhRulerTool.h"
//---------------------------------------------------------------------------

__fastcall TPhRulerTool::TPhRulerTool(TComponent* Owner): TPhImageTool(Owner)
{
   this->m_sp.X = 0;
   this->m_sp.y = 0;
   this->m_ep.X = 0;
   this->m_ep.y = 0;
   this->m_down = false;
   m_strToolName = L"RULER";
}
__fastcall TPhRulerTool::~TPhRulerTool()
{

}

void TPhRulerTool::Draw(TCanvas* Canvas)
{
    if (this->m_pImage == NULL)
        return;
   TCanvas* c = Canvas;
   int delta = 5;
   TPenStyle style = c->Pen->Style;
   TColor color = c->Pen->Color;
   TPenMode mode = c->Pen->Mode;
   c->Pen->Color = clLime;
   c->Brush->Color = clGreen;

   TPoint _sp = this->m_pImage->GetScreenPoint(m_sp.x, m_sp.y);
   TPoint _ep = this->m_pImage->GetScreenPoint(m_ep.x, m_ep.y);

   c->MoveTo(_sp.x, _sp.y);
   c->LineTo(_ep.x, _ep.y);

   c->Ellipse(_sp.x-delta, _sp.y - delta, _sp.x + delta, _sp.y + delta);
   c->Ellipse(_ep.x-delta, _ep.y - delta, _ep.x + delta, _ep.y + delta);

   c->TextOutW(_ep.x + 10, _ep.y - 10, FormatFloat("####.##", Length));
   c->Pen->Style = style;
   c->Pen->Color = color;
   c->Pen->Mode = mode;
}
void TPhRulerTool::MouseDown(int X, int Y, TMouseButton Button)
{
   if (m_pImage == NULL)
		return;
   if (Button != mbLeft)
		return;
   int _x = m_pImage->GetImageX(X);
   int _y = m_pImage->GetImageY(Y);
   if (_x >= 0 && _y >= 0)
   {
        this->m_sp.x = _x;
        this->m_sp.y = _y;
        this->m_ep = m_sp;
	    m_down = true;
   }
}
void TPhRulerTool::MouseUp(int X, int Y, TMouseButton Button)
{
   if (m_pImage == NULL)
		return;
   if (Button != mbLeft)
		return;
   if (this->m_down == true)
   {
	   int _x = m_pImage->GetImageX(X);
       int _y = m_pImage->GetImageY(Y);
       this->m_ep.x = _x;
       this->m_ep.y = _y;
	   m_down = false;
   }
}
void TPhRulerTool::MouseMove(int X, int Y, TShiftState Shift)
{
   if (m_pImage == NULL)
        return;
   if (this->m_down == true)
   {
	   int _x = m_pImage->GetImageX(X);
       int _y = m_pImage->GetImageY(Y);
       this->m_ep.x = _x;
       this->m_ep.y = _y;
       this->m_pImage->Paint();
   }

}
void TPhRulerTool::Reset()
{

}

void TPhRulerTool::DrawRuler()
{
    if (this->m_pImage == NULL)
        return;
    TCanvas* c = m_pImage->Canvas;

   TPenStyle style = c->Pen->Style;
   TColor color = c->Pen->Color;
   TPenMode mode = c->Pen->Mode;



   c->Pen->Style = style;
   c->Pen->Color = color;
   c->Pen->Mode = mode;
}

double __fastcall TPhRulerTool::GetLenght()
{
    double _d = (m_sp.x - m_ep.x)*(m_sp.x - m_ep.x) + (m_sp.y - m_ep.y)*(m_sp.y - m_ep.y);
    return sqrt(_d);
}


//----------------
namespace Phrulertool
{
	void __fastcall PACKAGE Register()
	{
		TComponentClass classes[1] = {__classid(TPhRulerTool)};
		RegisterComponents(L"Photon", classes, 0);
	}
}

#pragma package(smart_init)
