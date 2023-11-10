//---------------------------------------------------------------------------

#pragma hdrstop

#include "PhLandmarksTool.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

static double _2D_Dist(double x1,double y1,double x2,double y2)
{
  return sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
}

__fastcall TPhLandmarksTool::TPhLandmarksTool(TComponent* Owner): TPhPaneTool(Owner)
{
	m_file = NULL;
	m_strToolName = L"LANDMARKS";
	m_selected = -1;
	m_OnChange = NULL;

    m_skin = 0;
}
__fastcall TPhLandmarksTool::~TPhLandmarksTool()
{

}

void TPhLandmarksTool::Draw(TCanvas* Canvas)
{
   if(m_file == NULL || m_pImage == NULL || m_pImage->Bitmap == NULL)
	return;
	int w = m_pImage->Bitmap->Width;
	int h = m_pImage->Bitmap->Height;
	for (int i = 0; i < m_file->Count(); i++)
	{
		TLFLandmark* ll = m_file->Landmark(i);
		if (ll) {
		   double x,y;
		   x = ll->x();
		   y = ll->y();
		   int xx = x*w / 100;
		   int yy = y*h / 100;
		   TCanvas* cnv = Canvas;

		   TPoint p = m_pImage->GetScreenPoint(xx, yy);

		   TPoint pt = m_pImage->GetScreenPoint(xx+48, yy-48);

		   TRect r;
		   TRect rr;

		   cnv->Brush->Color = ll->Color();
		   cnv->Pen->Color = ll->Color();
		   cnv->TextOutW(pt.x, pt.y, ll->ClassName());
		   r.init(xx - 8, yy-8,xx+8, yy+8 );
		   rr= m_pImage->GetScreenRect(r);
		   if (m_skin == 0) {
			   cnv->Ellipse(rr);
		   }
		   else if (m_skin == 1)
		   {
			   cnv->Brush->Style = bsClear;
			   cnv->Ellipse(rr);
		   }
		   else
		   {
			   cnv->Brush->Style = bsClear;
			   cnv->Ellipse(rr);
			   TPoint p1,p2;
			   p1.x = (r.Right + r.Left) / 2;
			   p1.y = (r.top + r.Bottom) / 2;

			   p2.x = p1.x - 16;
			   p2.y = p1.y;
			   TPoint p3 = m_pImage->GetScreenPoint(p2.x, p2.y);
			   cnv->MoveTo(p3.x, p3.y);
			   p2.x = p1.x + 16;
			   p2.y = p1.y;
			   p3 = m_pImage->GetScreenPoint(p2.x, p2.y);
			   cnv->LineTo(p3.x, p3.y);

			   p2.x = p1.x;
			   p2.y = p1.y -16;
			   p3 = m_pImage->GetScreenPoint(p2.x, p2.y);
			   cnv->MoveTo(p3.x, p3.y);
			   p2.x = p1.x;
			   p2.y = p1.y+16;
			   p3 = m_pImage->GetScreenPoint(p2.x, p2.y);
			   cnv->LineTo(p3.x, p3.y);

		   }

		   cnv->Brush->Style = bsClear;
		   r.init(xx - 48, yy-48,xx+48, yy+48 );
		   rr= m_pImage->GetScreenRect(r);
		   cnv->Ellipse(rr);
		}
	}
}
void TPhLandmarksTool::MouseDown(int X, int Y, TMouseButton Button)
{
   if (IsNearPoint(X, Y, m_selected))
   {
		m_down = true;
   }
   else
	  TPhPaneTool::MouseDown(X, Y, Button);
}
void TPhLandmarksTool::MouseUp(int X, int Y, TMouseButton Button)
{
	  if (m_down && this->m_OnChange) {
		 this->OnChange(this);
	  }
	  m_down = false;
	  m_selected = -1;
	  TPhPaneTool::MouseUp(X, Y, Button);
}
void TPhLandmarksTool::MouseMove(int X, int Y, TShiftState Shift)
{
	int idx;
	if (m_down)
	{
		TLFLandmark* ll = m_file->Landmark(m_selected);
		awp2DPoint _p;
		TPoint p = m_pImage->GetImagePoint(X, Y);
		_p.X = 100.*p.x / m_pImage->Bitmap->Width;
		_p.Y = 100.*p.y / m_pImage->Bitmap->Height;

		ll->SetPoint(_p);
		ll->SetStatus(0.5);
		m_pImage->Paint();
	}
	else if (true)
	{
		if (IsNearPoint(X, Y, idx))
			m_pImage->Cursor = crHandPoint;
		else
			m_pImage->Cursor = crArrow;
		TPhPaneTool::MouseMove(X, Y, Shift);
	}
}
void TPhLandmarksTool::Reset()
{

}


bool __fastcall TPhLandmarksTool::Connect(const UnicodeString& fileName)
{
	m_db.Close();
	AnsiString _ansi = fileName;
	return m_db.Connect(_ansi.c_str());
}

void __fastcall TPhLandmarksTool::Close()
{
	m_db.Close();
}

bool __fastcall TPhLandmarksTool::SelectFile(const UnicodeString& fileName)
{
	AnsiString _ansi = ExtractFileName(fileName);
	m_file = m_db.Files()->File(_ansi.c_str());
	return m_file != NULL;
}
bool __fastcall TPhLandmarksTool::GetConnected()
{
    return m_db.Files()->Count() > 0;
}

TLFDBLandmarks* __fastcall TPhLandmarksTool::GetDatabase()
{
	return &m_db;
}

bool __fastcall TPhLandmarksTool::IsNearPoint(int x, int y, int& idx)
{
   if (m_pImage == NULL || m_file == NULL)
	   return false;
   TPoint p = m_pImage->GetImagePoint(x, y);
   for (int i = 0; i < m_file->Count(); i++)
   {
		TLFLandmark* ll = m_file->Landmark(i);
		if (ll)
		{
		   double _x,_y;
		   _x = ll->x();
		   _y = ll->y();
		   int xx = _x*m_pImage->Bitmap->Width / 100;
		   int yy = _y*m_pImage->Bitmap->Height / 100;
		   if (_2D_Dist(xx,yy, p.x, p.y) < 10) {
				idx = i;
				return true;
		   }
		}
   }
   return false;
}

void __fastcall TPhLandmarksTool::SetSkin(const int value)
{
	m_skin = value;
	m_pImage->Paint();
}


//-------------------------------------------------------------------------
namespace Phlandmarkstool
{
	void __fastcall PACKAGE Register()
	{
		TComponentClass classes[1] = {__classid(TPhLandmarksTool)};
		RegisterComponents(L"Photon", classes, 0);
	}
}

