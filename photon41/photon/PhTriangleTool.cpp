//---------------------------------------------------------------------------

#pragma hdrstop

#include "PhTriangleTool.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

__fastcall TPhTriangleTool::TPhTriangleTool(TComponent* Owner): TPhImageTool(Owner)
{
   this->m_sp.X = 0;
   this->m_sp.y = 0;
   this->m_ep.X = 0;
   this->m_ep.y = 0;
   this->m_mp.X = 0;
   this->m_mp.y = 0;
   m_strToolName = L"TRIANGLE";
   m_state = 0;
}
 __fastcall TPhTriangleTool::~TPhTriangleTool()
{

}

void TPhTriangleTool::Draw(TCanvas* Canvas)
{
    if (this->m_pImage == NULL)
        return;
   TCanvas* c = Canvas;
   int delta = 5;
   int delta2 = 30;
   int delta3 = 50;
   TPenStyle style = c->Pen->Style;
   TColor color = c->Pen->Color;
   TPenMode mode = c->Pen->Mode;
   c->Pen->Color = clLime;
   c->Brush->Color = clGreen;

   TPoint _sp = this->m_pImage->GetScreenPoint(m_sp.x, m_sp.y);
   TPoint _mp = this->m_pImage->GetScreenPoint(m_mp.x, m_mp.y);
   TPoint _ep = this->m_pImage->GetScreenPoint(m_ep.x, m_ep.y);

   c->MoveTo(_sp.x, _sp.y);
   c->LineTo(_mp.x, _mp.y);
   c->LineTo(_ep.x, _ep.y);

   c->Ellipse(_sp.x-delta, _sp.y - delta, _sp.x + delta, _sp.y + delta);
   c->Ellipse(_mp.x-delta, _mp.y - delta, _mp.x + delta, _mp.y + delta);
   c->Ellipse(_ep.x-delta, _ep.y - delta, _ep.x + delta, _ep.y + delta);

   double d  = sqrt(float((_sp.x-_mp.x)*(_sp.x-_mp.x) + (_sp.y-_mp.y)*(_sp.y-_mp.y)));
   double dd  = sqrt(float((_ep.x-_mp.x)*(_ep.x-_mp.x) + (_ep.y-_mp.y)*(_ep.y-_mp.y)));
   if (d > 0 && dd > 0) {
	   double d1 = _mp.x - _sp.x;
	   double d2 = _mp.y - _sp.y;
	   double cosa = d1 / d;
	   double sina = d2 / d;

	   double dd1 = _mp.x - _ep.x;
	   double dd2 = _mp.y - _ep.y;
	   double cosa1 = dd1 / dd;
	   double sina1 = dd2 / dd;
	   c->TextOutW(_mp.x - delta3*cosa1, _mp.y - delta3*sina1, FormatFloat("000.00", Angle));
	   c->Brush->Style = bsClear;
	   c->Arc(_mp.x-delta2, _mp.y - delta2, _mp.x + delta2, _mp.y + delta2, _mp.x - delta2*cosa, _mp.y - delta2*sina, _mp.x - delta2*cosa1, _mp.y - delta2*sina1);
   }





   c->Pen->Style = style;
   c->Pen->Color = color;
   c->Pen->Mode = mode;
}

void TPhTriangleTool::MouseDown(int X, int Y, TMouseButton Button )
{

}
void TPhTriangleTool::MouseUp(int X, int Y, TMouseButton Button)
{
   if (m_pImage == NULL)
		return;
   if (Button != mbLeft)
		return;
   int _x = m_pImage->GetImageX(X);
   int _y = m_pImage->GetImageY(Y);
   if (_x >= 0 && _y >= 0)
   {
	   if (m_state == 0)
	   {
			this->m_sp.x = _x;
			this->m_sp.y = _y;
			this->m_mp = m_sp;
			this->m_ep = m_sp;
			m_state = 1;
	   }
	   else if (m_state == 1)
	   {
		  m_mp.x = _x;
		  m_mp.y = _y;
		  m_ep = m_mp;
		  m_state = 2;
	   }
	   else if (m_state == 2)
	   {
		  m_ep.x = _x;
		  m_ep.y = _y;
		  m_state = 0;
	   }
   }
   this->m_pImage->Paint();
}
void TPhTriangleTool::MouseMove(int X, int Y, TShiftState Shift)
{
   if (m_pImage == NULL)
		return;
   if (m_state == 0)
	   return;

   int _x = m_pImage->GetImageX(X);
   int _y = m_pImage->GetImageY(Y);

   if (m_state == 1) {
	  if (_x > 0 && _y > 0) {
		  m_mp.x = _x;
		  m_mp.y = _y;
		  m_ep = m_mp;
	  }
   }
   else if (m_state == 2) {
	  if (_x > 0 && _y > 0) {
		  m_ep.x = _x;
		  m_ep.y = _y;
	  }
   }
   this->m_pImage->Paint();
}
void TPhTriangleTool::Reset()
{

}
double __fastcall TPhTriangleTool::GetAngle()
{
   TPoint _sp = this->m_pImage->GetScreenPoint(m_sp.x, m_sp.y);
   TPoint _mp = this->m_pImage->GetScreenPoint(m_mp.x, m_mp.y);
   TPoint _ep = this->m_pImage->GetScreenPoint(m_ep.x, m_ep.y);

   double norm1 = sqrt(float((_sp.x-_mp.x)*(_sp.x-_mp.x) + (_sp.y-_mp.y)*(_sp.y-_mp.y)));
   double norm2 = sqrt(float((_ep.x-_mp.x)*(_ep.x-_mp.x) + (_ep.y-_mp.y)*(_ep.y-_mp.y)));
   double norm = norm1*norm2;
   if (norm > 0)
   {
		awp2DPoint p1,p2,p3;
		p1.X = _sp.x - _mp.x;
		p1.Y = _sp.y - _mp.y;
		p3.X = _ep.x - _mp.x;
		p3.Y = _ep.y - _mp.y;

		double scalar = (p1.X*p3.X+ p1.Y*p3.Y)/ norm;
		double angle = 0;
		if (p1.X*p3.Y - p3.X*p1.Y >= 0) {
			angle = 360-180*acos(scalar) / AWP_PI;
		}
		else
			angle = 180*acos(scalar) / AWP_PI;
		return angle;
   }
   else
	   return 0;
}

//----------------
namespace Phtriangletool
{
	void __fastcall PACKAGE Register()
	{
		TComponentClass classes[1] = {__classid(TPhTriangleTool)};
		RegisterComponents(L"Photon", classes, 0);
	}
}

