//---------------------------------------------------------------------------

#pragma hdrstop

#include "PhZonesTool.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
 extern "C"
{
    #pragma link "awpipl2b.lib"
	#pragma link "JPEGLIB.lib"
}
#pragma link "awplflibb.lib"
#pragma link "TinyXML.lib"

static awpRect TRect2awpRect(TRect& rect)
{
	awpRect result;
	result.left = (AWPWORD)rect.Left;
	result.top  = (AWPWORD)rect.top;
	result.right = (AWPWORD)rect.Right;
	result.bottom = (AWPWORD)rect.bottom;
	return result;
}

static TRect awpRect2TRect(awpRect r)
{
	TRect result;
	//result.init(r.left, r.top, r.right, r.bottom);
	result.Left = r.left;
	result.top = r.top;
	result.right = r.right;
	result.bottom = r.bottom;
	return result;
}


__fastcall TPhZonesTool::TPhZonesTool(TComponent* Owner) : TPhImageTool(Owner)
{
     m_mode = ZTRect;
	 m_strToolName = L"ZONES EDITOR";

	m_down = false;
	m_si = 0; // selected index
	m_sv = 0; // selectes vertex
	m_selected = -1;
	m_newZone = NULL;
	m_OnAddRoi = NULL;
	m_CanAddZone = false;
}
__fastcall TPhZonesTool::~TPhZonesTool()
{

}

awp2DPoint __fastcall TPhZonesTool::Get2DPoint(int X, int Y)
{
	  TPoint p = m_pImage->GetImagePoint(X, Y);
	  awp2DPoint p2d;
	  p2d.X = 100.0*(double)p.x/(double)m_pImage->Bitmap->Width;
	  p2d.Y = 100.0*(double)p.y/(double)m_pImage->Bitmap->Height;
	  return p2d;
}
double TPhZonesTool::_2D_Dist(double x1,double y1,double x2,double y2)
{
  return sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
}
bool   TPhZonesTool::_is_near_vertex(int X, int Y, int& idx1, int& idx2)
{
   bool res = false;

   if (m_pImage == NULL)
		return res;


   TPoint p = m_pImage->GetImagePoint(X, Y);

	for (int i = 0; i < m_rois.GetCount(); i++)
	{
		TLFZone* item = m_rois.GetZone(i);
		if (item != NULL)
		{
			TLFZone* z = item;
			if (z != NULL)
			{
				 if (z->GetZoneType() == ZTRect/* && m_mode == TMRect*/)
				 {
					TRect rect = GetBoundsRect(z);
					for (int j = 0; j < 4; j++)
					{
						TPoint vertex = GetRectPoint(j, rect);
						if  (_2D_Dist(vertex.x, vertex.y, p.x, p.y) < 10)//
						{
						   res = true;
						   idx1 = i;
						   idx2 = j;
						   break;
						}
					}
				 }
				 else if (z->GetZoneType() == ZTCircle/* && m_mode == TMCircle*/)
				 {
					 awp2DPoint p1;
					 awp2DPoint p2;
					 p1 = z->GetLineSegmnet()->GetStart();
					 p2 = z->GetLineSegmnet()->GetFinish();

					 TPoint _p1;
					 TPoint _p2;

					 _p1.x = p1.X*m_pImage->Bitmap->Width / 100.;
					 _p1.y = p1.Y*m_pImage->Bitmap->Height / 100.;

					 _p2.x = p2.X*m_pImage->Bitmap->Width / 100.;
					 _p2.y = p2.Y*m_pImage->Bitmap->Height / 100.;

						if  (_2D_Dist(_p1.x, _p1.y, p.x, p.y) < 4)//
						{
						   res = true;
						   idx1 = i;
						   idx2 = 0;
						   break;
						}

						if  (_2D_Dist(_p2.x, _p2.y, p.x, p.y) < 4)//
						{
						   res = true;
						   idx1 = i;
						   idx2 = 1;
							break;
						}

					res = false;
				 }
				 else if (z->GetZoneType() == ZTContour)
				 {

					for (int j = 0; j < z->GetContour()->GetNumPoints(); j++)
					{
						awp2DPoint s = z->GetContour()->GetPoint(j);
						TPoint _p;
						_p.x = s.X * m_pImage->Bitmap->Width / 100;
						_p.y = s.Y * m_pImage->Bitmap->Height / 100;
						if  (_2D_Dist(_p.x, _p.y, p.x, p.y) < 4)//
						{
						   res = true;
						   idx1 = i;
						   idx2 = j;
						    break;
						}

					}
				 }
				// delete z;
			}
		}
	}
	return res;
}
TPoint TPhZonesTool::GetRectPoint(int index, TRect& rect)
{
	TPoint result;
	if (index == 0)
	{
		result = rect.TopLeft();
	}
	else if (index == 1)
	{
		result.x = rect.BottomRight().X;
		result.y = rect.TopLeft().Y;
	}
	else if (index == 2)
	{
		result = rect.BottomRight();
	}
	else
	{
		result.x = rect.TopLeft().X;
		result.y = rect.BottomRight().Y;
	}
	return result;
}
void __fastcall TPhZonesTool::SetVertex(int x, int y, bool update)
{
   TLFZone* item = m_rois.GetZone(m_si);
   TPoint p = m_pImage->GetImagePoint(x, y);
   if (item)
   {
	 TLFZone* z = item;
	 if (z == NULL || (z->GetZoneType() == ZTRect))
	 {
		 TRect rect = this->GetBoundsRect(z);
		 if (m_sv == 0)
		 {
			rect.Left = p.x;
			rect.Top  = p.y;
		 }
		 else if (m_sv == 1)
		 {
			rect.right = p.x;
			rect.Top  = p.y;
		 }
		 else if (m_sv == 2)
		 {
			rect.right = p.x;
			rect.bottom  = p.y;
		 }
		 else if (m_sv == 3)
		 {
			rect.left = p.x;
			rect.bottom  = p.y;
		 }
		 awpRect _rect = TRect2awpRect(rect);
		 TLF2DRect* _r = z->GetRect();
		 double x1,x2,y1, y2;
		 x1 = 100.*_rect.left / (double)m_pImage->Bitmap->Width;
		 x2 = 100.*_rect.right / (double)m_pImage->Bitmap->Width;
		 y1 = 100.*_rect.top / (double)m_pImage->Bitmap->Height;
		 y2 = 100.*_rect.bottom / (double)m_pImage->Bitmap->Height;
		 _r->SetVertexes(x1,x2,y1,y2);
	 }
	 else if (z->GetZoneType() == ZTCircle /*&& m_mode == TMCircle*/)
	 {
		   awp2DPoint p1;
		   p1.X = 100.0*p.x /(double) m_pImage->Bitmap->Width;
		   p1.Y = 100.0*p.y /(double) m_pImage->Bitmap->Height;
			if (m_sv == 0)
			{
				z->GetLineSegmnet()->SetStart(p1);
			}
			else if (m_sv == 1)
			{
				z->GetLineSegmnet()->SetFihish(p1);
			}

	 }
	 else if (z->GetZoneType() == ZTContour) {
		   awp2DPoint p1;
		   p1.X = 100.0*p.x /(double) m_pImage->Bitmap->Width;
		   p1.Y = 100.0*p.y /(double) m_pImage->Bitmap->Height;
           z->GetContour()->SetPoint(m_sv, p1);
		  }
 //	 if (update && this->m_OnChangeRoi) {
 //		 this->OnChangeRoi(this, m_si, update);
 //	 }
   }
}
TRect __fastcall TPhZonesTool::GetBoundsRect(TLFZone* zone)
{
	TLF2DRect* r = zone->GetBounds();
	awpRect _rect;
	_rect.left =   r->GetLeftTop().X*m_pImage->Bitmap->Width / 100.;
	_rect.top  =   r->GetLeftTop().Y*m_pImage->Bitmap->Height / 100.;
	_rect.right =  r->GetRightBottom().X*m_pImage->Bitmap->Width / 100.;
	_rect.bottom = r->GetRightBottom().Y*m_pImage->Bitmap->Height / 100.;
	TRect rect = awpRect2TRect(_rect);
	return rect;
}
int __fastcall TPhZonesTool::GetRoiCount()
{
	 return m_rois.GetCount();
}
void __fastcall TPhZonesTool::SetSelected(int value)
{
 	m_selected = value;
	m_pImage->Paint();
}

void TPhZonesTool::Draw(TCanvas* Canvas)
{
	if (Canvas == NULL)
		return;

	TColor oldColor =  Canvas->Pen->Color;
	TBrushStyle oldStyle = Canvas->Brush->Style;

	int delta = 4;

	for (int i = 0; i < m_rois.GetCount(); i++)
	{
		Canvas->Brush->Style = bsSolid;
		TLFZone* item = m_rois.GetZone(i);
		TColor color = clWhite;
	   //	if (i == m_selected)
		{
			Canvas->Pen->Width = 1;
			Canvas->Pen->Color = clLime;
			Canvas->Brush->Color = clLime;
		}
 //		else
//		{
 //			Canvas->Pen->Width = 1;
 //			Canvas->Pen->Color = color;
  //			Canvas->Brush->Color = color;
 //       }

		TEZoneTypes zt =  item->GetZoneType();
		if (zt == ZTRect)
		{
			TRect rect = this->GetBoundsRect(item);
			TRect rect1 = this->m_pImage->GetScreenRect(rect);
			//Canvas->Brush->Color = color;

			TPoint p1 = rect1.TopLeft();
			TPoint p2 = rect1.BottomRight();
			//if (i == m_selected)
			{
				Canvas->Ellipse(p1.x-delta, p1.y - delta, p1.x + delta, p1.y + delta);
				Canvas->Ellipse(p2.x-delta, p2.y - delta, p2.x + delta, p2.y + delta);


				p1.x = p2.x;
				Canvas->Ellipse(p1.x-delta, p1.y - delta, p1.x + delta, p1.y + delta);
				p1 = rect1.TopLeft();
				p1.y = p2.y;
				Canvas->Ellipse(p1.x-delta, p1.y - delta, p1.x + delta, p1.y + delta);
			}


			Canvas->Brush->Style = bsClear;
			Canvas->Rectangle(rect1);
		}
		else if (zt == ZTCircle)
		{
			awp2DPoint s = item->GetLineSegmnet()->GetStart();
			awp2DPoint f = item->GetLineSegmnet()->GetFinish();

			TPoint p1;
			TPoint p2;

			p1.x = s.X * m_pImage->Bitmap->Width / 100;
			p1.y = s.Y * m_pImage->Bitmap->Height / 100;

			p2.x = f.X * m_pImage->Bitmap->Width / 100;
			p2.y = f.Y * m_pImage->Bitmap->Height / 100;

			p1 = m_pImage->GetScreenPoint(p1.x, p1.y);
			p2 = m_pImage->GetScreenPoint(p2.x, p2.y);

			double r = sqrt(double(p1.x - p2.x)*(p1.x - p2.x) + (double)(p1.y - p2.y)*(p1.y-p2.y));
			Canvas->Brush->Style = bsClear;
			Canvas->Ellipse(p1.x-r, p1.y - r, p1.x + r, p1.y + r);

			Canvas->MoveTo(p1.x, p1.y);
			Canvas->LineTo(p2.x, p2.y);
			//if (i == m_selected)
			{
				Canvas->Brush->Color = clRed;
				Canvas->Ellipse(p1.x-delta, p1.y - delta, p1.x + delta, p1.y + delta);
				Canvas->Brush->Color = clBlue;
				Canvas->Ellipse(p2.x-delta, p2.y - delta, p2.x + delta, p2.y + delta);
			}
		}
		else if (zt == ZTContour)
		{
			//Canvas->Brush->Style = bsClear;
			//Canvas->Brush->Color = color;
			awp2DPoint s = item->GetContour()->GetPoint(0);
			TPoint p,p1;
			p.x = s.X * m_pImage->Bitmap->Width / 100;
			p.y = s.Y * m_pImage->Bitmap->Height / 100;
			p1 = m_pImage->GetScreenPoint(p.x, p.y);
			Canvas->MoveTo(p1.x, p1.y);
			for (int j = 0; j < item->GetContour()->GetNumPoints(); j++)
			{
				s = item->GetContour()->GetPoint(j);
				p.x = s.X * m_pImage->Bitmap->Width / 100;
				p.y = s.Y * m_pImage->Bitmap->Height / 100;
				p1 = m_pImage->GetScreenPoint(p.x, p.y);
				Canvas->LineTo(p1.x, p1.y);
				//if (i == m_selected)
				{
					Canvas->Ellipse(p1.x-delta, p1.y - delta, p1.x + delta, p1.y + delta);
				}
			}
			s = item->GetContour()->GetPoint(0);
			p.x = s.X * m_pImage->Bitmap->Width / 100;
			p.y = s.Y * m_pImage->Bitmap->Height / 100;
			p1 = m_pImage->GetScreenPoint(p.x, p.y);
			Canvas->LineTo(p1.x, p1.y);
			Canvas->Brush->Style = bsClear;
		}
	}

	if (m_newZone != NULL)
	{
		if (m_newZone->IsCircle())
		{
			Canvas->Pen->Color = clLime;
			Canvas->Brush->Style = bsClear;
			awp2DPoint s = m_newZone->GetLineSegmnet()->GetStart();
			awp2DPoint f = m_newZone->GetLineSegmnet()->GetFinish();

			TPoint p1;
			TPoint p2;

			p1.x = s.X * m_pImage->Bitmap->Width / 100;
			p1.y = s.Y * m_pImage->Bitmap->Height / 100;

			p2.x = f.X * m_pImage->Bitmap->Width / 100;
			p2.y = f.Y * m_pImage->Bitmap->Height / 100;

			p1 = m_pImage->GetScreenPoint(p1.x, p1.y);
			p2 = m_pImage->GetScreenPoint(p2.x, p2.y);

			double r = sqrt(double(p1.x - p2.x)*(p1.x - p2.x) + (double)(p1.y - p2.y)*(p1.y-p2.y));
			Canvas->Ellipse(p1.x-r, p1.y - r, p1.x + r, p1.y + r);

			Canvas->Brush->Color = clLime;
			Canvas->MoveTo(p1.x, p1.y);
			Canvas->LineTo(p2.x, p2.y);
			Canvas->Brush->Color = clRed;
			Canvas->Ellipse(p1.x-delta, p1.y - delta, p1.x + delta, p1.y + delta);
			Canvas->Brush->Color = clBlue;
			Canvas->Ellipse(p2.x-delta, p2.y - delta, p2.x + delta, p2.y + delta);

		}
		else if (m_newZone->IsRect())
		{
			Canvas->Pen->Color = clLime;
			Canvas->Brush->Color = clLime;

			TRect rect2 = this->GetBoundsRect(m_newZone);
			TRect rect3 = this->m_pImage->GetScreenRect(rect2);


			TPoint p1 = rect3.TopLeft();
			TPoint p2 = rect3.BottomRight();

			Canvas->Ellipse(p1.x-delta, p1.y - delta, p1.x + delta, p1.y + delta);
			Canvas->Ellipse(p2.x-delta, p2.y - delta, p2.x + delta, p2.y + delta);


			p1.x = p2.x;
			Canvas->Ellipse(p1.x-delta, p1.y - delta, p1.x + delta, p1.y + delta);
			p1 = rect3.TopLeft();
			p1.y = p2.y;
			Canvas->Ellipse(p1.x-delta, p1.y - delta, p1.x + delta, p1.y + delta);

			Canvas->Brush->Style = bsClear;
			Canvas->Rectangle(rect3);

			Canvas->Pen->Color = oldColor;
			Canvas->Brush->Color = oldColor;
			Canvas->Brush->Style = oldStyle;
		 }
		 else if (m_newZone->IsContour())
		 {
			Canvas->Pen->Color = clLime;
			Canvas->Brush->Color = clLime;

			awp2DPoint s = m_newZone->GetContour()->GetPoint(0);
			TPoint p,p1;
			p.x = s.X * m_pImage->Bitmap->Width / 100;
			p.y = s.Y * m_pImage->Bitmap->Height / 100;
			p1 = m_pImage->GetScreenPoint(p.x, p.y);
			Canvas->MoveTo(p1.x, p1.y);
			for (int i = 0; i < m_newZone->GetContour()->GetNumPoints(); i++)
			{
				s = m_newZone->GetContour()->GetPoint(i);
				p.x = s.X * m_pImage->Bitmap->Width / 100;
				p.y = s.Y * m_pImage->Bitmap->Height / 100;
				p1 = m_pImage->GetScreenPoint(p.x, p.y);
				Canvas->LineTo(p1.x, p1.y);
				Canvas->Ellipse(p1.x-delta, p1.y - delta, p1.x + delta, p1.y + delta);
			}

		 }
	}

}
void TPhZonesTool::MouseDown(int X, int Y, TMouseButton Button)
{
   if (m_pImage == NULL)
		return;
   if (Button == mbLeft)
   {
	  int idx1, idx2;
	  if (_is_near_vertex(X, Y, idx1, idx2))
	  {
		  m_si = idx1;
		  m_sv = idx2;
          m_down = true;
	  }
	  else
	  {
		if (m_mode == ZTRect && this->m_CanAddZone)
		{
			// new rect
			m_newZone = new TLFZone(ZTRect);
			TPoint p = m_pImage->GetImagePoint(X, Y);
			double x1, x2, y1, y2;
			  x1 = 100.*p.x / (double)m_pImage->Bitmap->Width;
			  y1 = 100.*p.y /(double)m_pImage->Bitmap->Height;
			  x2 = 100.*p.x / (double)m_pImage->Bitmap->Width;
			  y2 = 100.*p.y /(double)m_pImage->Bitmap->Height;

			  m_newZone->GetRect()->SetVertexes(x1, x2, y1, y2);
               m_down = true;
		}
		else if (m_mode == ZTCircle && this->m_CanAddZone)
		{
		  m_newZone = new TLFZone(ZTCircle);
		  awp2DPoint p2d = this->Get2DPoint(X,Y);
		  m_newZone->GetLineSegmnet()->SetStart(p2d);
		   m_down = true;
		}
		else if (m_mode == ZTContour && m_newZone == NULL && this->m_CanAddZone)
		{
		   m_newZone = new TLFZone(ZTContour);
			m_down = true;
		}

	  }
   }
}
void TPhZonesTool::MouseUp(int X, int Y, TMouseButton Button)
{
  if (m_pImage == NULL)
		return;
   if (Button == mbLeft)
   {
	  if ( m_newZone != NULL)
	  {
		 if (m_mode == ZTRect)
		 {
			 // process new rect
			  TPoint p = m_pImage->GetImagePoint(X, Y);
			  double x1, x2, y1, y2;
			  x1 = m_newZone->GetRect()->GetLeftTop().X;
			  y1 = m_newZone->GetRect()->GetLeftTop().Y;
			  x2 = 100.*p.x / (double)m_pImage->Bitmap->Width;
			  y2 = 100.*p.y /(double)m_pImage->Bitmap->Height;

			  m_newZone->GetRect()->SetVertexes(x1,x2,y1,y2);

		 }
		 else if (m_mode == ZTCircle)
		 {
				awp2DPoint p2d = this->Get2DPoint(X,Y);
				m_newZone->GetLineSegmnet()->SetFihish(p2d);
		 }
		 if (m_mode != ZTContour)
		 {
			 // добавляем новый элемент, ли это не контур
			 this->AddNewRoi();
		 }
		 else
		 {
			// добавляем точку контура
			awp2DPoint p2d = this->Get2DPoint(X,Y);
			m_newZone->GetContour()->AddPoint(p2d);
		 }

		  m_pImage->Paint();
		  m_down = false;

	  }
	  else
	  {
		// process vertex
		 if (this->m_OnChangeRoi) {
			 this->OnChangeRoi(this, m_si, true);
		 }
		 m_down = false;
	  }
   }
  else if (Button == mbRight && m_mode == ZTContour)
  {
	 // Добавляем контур
	 this->AddNewRoi();
	 m_pImage->Paint();
  }
}
void TPhZonesTool::MouseMove(int X, int Y, TShiftState Shift)
{
   if (m_pImage == NULL)
		return;
	m_pImage->Cursor = crArrow;
	int idx1, idx2;
	if (_is_near_vertex(X, Y, idx1, idx2))
		m_pImage->Cursor = crHandPoint;
	else
		m_pImage->Cursor = crArrow;

   if (m_down)
   {
	   if (m_newZone != NULL)
	   {
			TPoint p = m_pImage->GetImagePoint(X, Y);
			if (m_mode == ZTRect)
			{
			  TPoint p = m_pImage->GetImagePoint(X, Y);
			  double x1, x2, y1, y2;
			  x1 = m_newZone->GetRect()->GetLeftTop().X;
			  y1 = m_newZone->GetRect()->GetLeftTop().Y;
			  x2 = 100.*p.x / (double)m_pImage->Bitmap->Width;
			  y2 = 100.*p.y /(double)m_pImage->Bitmap->Height;

			  m_newZone->GetRect()->SetVertexes(x1,x2,y1,y2);
			}
			else if (m_mode == ZTCircle)
			{
				awp2DPoint p2d;
				p2d.X = 100.0*(double)p.x/(double)m_pImage->Bitmap->Width;
				p2d.Y = 100.0*(double)p.y/(double)m_pImage->Bitmap->Height;

				m_newZone->GetLineSegmnet()->SetFihish(p2d);
			}
	   }
	   else
	   {
			// process vertex
			this->SetVertex(X,Y, false);
	   }
	   m_pImage->Paint();
   }
}
void TPhZonesTool::Reset()
{
   m_rois.Clear();
}

void __fastcall TPhZonesTool::AddNewRoi()
{
	 if (m_newZone == NULL) {
		return;
	 }

	 TLFZone* roi = new TLFZone(*m_newZone) ;
	 //roi->SetZone(m_newZone);
	 this->m_rois.Add(roi);
	 if (m_OnAddRoi) {
		m_OnAddRoi(this, roi);
	 }
	 delete  m_newZone;
	 m_newZone = NULL;
}

	// сохрание roi в файл
bool __fastcall TPhZonesTool::SaveZones(const UnicodeString fileName)
{
	AnsiString _ansi = fileName;
	return m_rois.Save(_ansi.c_str());
}
// загрузка roi из файла
bool __fastcall TPhZonesTool::LoadZones(const UnicodeString fileName)
{
	AnsiString _ansi = fileName;
	return m_rois.Load(_ansi.c_str());
}

namespace Phzonestool
{
	void __fastcall PACKAGE Register()
	{
		TComponentClass classes[1] = {__classid(TPhZonesTool)};
		RegisterComponents(L"Photon", classes, 0);
	}
}

