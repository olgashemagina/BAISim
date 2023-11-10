//---------------------------------------------------------------------------

#pragma hdrstop

#include "Cube3D.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

T3DTransform::T3DTransform()
{
	angleX = 0;
	angleY = 45;
	angleZ = 30;
	startX = 300;
	startY = 300;
	zoom = 100;
}

T3DTransform::~T3DTransform()
{

}

TPoint T3DTransform::GetProjection(T3DPoint p)
{
   float a,b,g, sina, sinb, sing,cosa, cosb, cosg;
   float a11,a12,a13,a21,a22,a23,a31,a32,a33;
   a = 3.14*angleX / 180.;
   b = 3.14*angleY / 180.;
   g = 3.14*angleZ / 180.;
   sina = sin(a);
   sinb = sin(b);
   sing = sin(g);
   cosa = cos(a);
   cosb = cos(b);
   cosg = cos(g);
   a11 = cosa*cosg-sina*cosb*sing;
   a12 = -cosa*sing-sina*cosb*cosg;
   a13 = sina*sinb;
   a21 = sina*cosg + cosa*cosb*sing;
   a22 = -sina*sing+cosa*cosb*cosg;
   a23 = -cosa*sinb;
   a31 = sinb*sing;
   a32 = sinb*cosg;
   a33 = cosb;

   T3DPoint pt;
   pt.x = a11*p.x + a12*p.y + a13*p.z;
   pt.y = a21*p.x + a22*p.y + a23*p.z;
   pt.z = a31*p.x + a32*p.y + a33*p.z;

   TPoint result;
   float zm = Zoom*256./100;
   result.x = startX+ zm*pt.x;
   result.y = startY+ zm*pt.y;

   return result;

}

void T3DTransform::SetZoom(float v)
{
	if (v >= 10 && v <= 200)
	{
		Zoom = v;
	}
}

TCube::TCube(T3DTransform* t)
{
	this->m_t = t;
	T3DPoint v[8] = {{0,0,0},{1,1,1},{0,1,1},{1,0,1},{1,1,0},{1,0,0},{0,1,0},{0,0,1}};
	memcpy(m_v, v, sizeof(v));
	m_DrawText = false;
	m_color = clWhite;
}

void TCube::ConvertTo(void (*convert)(double, double, double, double&, double&, double&))
{
	if (convert != NULL)
	{
	   for (int i = 0; i < 8; i++)
	   {
		   double x,y,z;
		   convert((double)m_v[i].x, (double)m_v[i].y, (double)m_v[i].z, x,y,z);
		   m_v[i].x = x / 100;
		   m_v[i].y = y / 100;
		   m_v[i].z = z / 100;
	   }
	}
}
void TCube::Draw(TCanvas* pCanvas)
{
	TColor oldColor = pCanvas->Pen->Color;
	pCanvas->Pen->Color = m_color;
	pCanvas->Font->Color = m_color;

	TPoint p;
	p = m_t->GetProjection(m_v[0]);
	pCanvas->MoveTo(p.X, p.Y);
	p = m_t->GetProjection(m_v[5]);
	pCanvas->LineTo(p.X, p.Y);
	p = m_t->GetProjection(m_v[3]);
	pCanvas->LineTo(p.X, p.Y);
	p = m_t->GetProjection(m_v[7]);
	pCanvas->LineTo(p.X, p.Y);
	p = m_t->GetProjection(m_v[0]);
	pCanvas->LineTo(p.X, p.Y);
	p = m_t->GetProjection(m_v[6]);
	pCanvas->LineTo(p.X, p.Y);
	p = m_t->GetProjection(m_v[4]);
	pCanvas->LineTo(p.X, p.Y);
	p = m_t->GetProjection(m_v[1]);
	pCanvas->LineTo(p.X, p.Y);
	p = m_t->GetProjection(m_v[2]);
	pCanvas->LineTo(p.X, p.Y);
	p = m_t->GetProjection(m_v[6]);
	pCanvas->LineTo(p.X, p.Y);
	p = m_t->GetProjection(m_v[4]);
	pCanvas->MoveTo(p.X, p.Y);
	p = m_t->GetProjection(m_v[5]);
	pCanvas->LineTo(p.X, p.Y);
	p = m_t->GetProjection(m_v[1]);
	pCanvas->MoveTo(p.X, p.Y);
	p = m_t->GetProjection(m_v[3]);
	pCanvas->LineTo(p.X, p.Y);
	p = m_t->GetProjection(m_v[2]);
	pCanvas->MoveTo(p.X, p.Y);
	p = m_t->GetProjection(m_v[7]);
	pCanvas->LineTo(p.X, p.Y);
	if (m_DrawText)
	{
		p = m_t->GetProjection(m_v[0]);
		pCanvas->TextOut(p.X, p.Y, L"Black 0");
		p = m_t->GetProjection(m_v[5]);
		pCanvas->TextOut(p.X, p.Y, L"Red 5");
		p = m_t->GetProjection(m_v[3]);
		pCanvas->TextOut(p.X, p.Y, L"Magenta 3");
		p = m_t->GetProjection(m_v[7]);
		pCanvas->TextOut(p.X, p.Y, L"Blue 7");
		p = m_t->GetProjection(m_v[6]);
		pCanvas->TextOut(p.X, p.Y, L"Green 6");
		p = m_t->GetProjection(m_v[4]);
		pCanvas->TextOut(p.X, p.Y, L"Yellow 4");
		p = m_t->GetProjection(m_v[1]);
		pCanvas->TextOut(p.X, p.Y, L"White 1");
		p = m_t->GetProjection(m_v[2]);
		pCanvas->TextOut(p.X, p.Y, L"Cyan 2");
	}


	pCanvas->Pen->Color = oldColor;
}
