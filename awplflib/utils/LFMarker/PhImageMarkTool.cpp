//---------------------------------------------------------------------------
#pragma hdrstop
#include "PhImageMarkTool.h"
#include "ImageUtils.h"
#include "MainForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "tinyxml.lib"
#define _IM_MIN_WIDTH_ 12
#define _IM_MIN_HEIGHT_ 12
//---------------------------------------------------------------------------
__fastcall TPhImageMarkTool::TPhImageMarkTool(TComponent* Owner): TPhImageTool(Owner)
{
	m_down = false;
	m_newRect = NULL;
	m_OnChange = NULL;
	m_OnAddData = NULL;
	m_OnDelFrame = NULL;
	m_OnLoad = NULL;
	m_OnProgress = NULL;
	m_edited = false;
	PopupMenu = NULL;
	m_selected = -1;
	m_dictinary = NULL;
	m_mode = MTRect;
	m_newZone = NULL;
	m_strToolName = "RECT ITEM";
}
//---------------------------------------------------------------------------
__fastcall TPhImageMarkTool::~TPhImageMarkTool()
{
	if (m_newRect != NULL)
		delete m_newRect;
}
//---------------------------------------------------------------------------
double TPhImageMarkTool::_2D_Dist(double x1,double y1,double x2,double y2)
{
	return sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
}
//---------------------------------------------------------------------------
bool   TPhImageMarkTool::_is_near_vertex(int X, int Y, int& idx1, int& idx2)
{
   bool res = false;

   if (m_pImage == NULL)
		return res;

   TPoint p = m_pImage->GetImagePoint(X, Y);

	for (int i = 0; i < m_descriptor.GetCount(); i++)
	{
		TLFDetectedItem* item = (TLFDetectedItem*)m_descriptor.Get(i);
		if (item != NULL)
		{
			TLFZone* z = item->GetZone();
			if (z != NULL)
			{
				 if (z->GetZoneType() == ZTRect && m_mode == MTRect)
				 {
					TRect rect = awpRect2TRect(item->GetBounds()->GetRect());
					for (int j = 0; j < 4; j++)
					{
						TPoint vertex = GetRectPoint(j, rect);
						if  (_2D_Dist(vertex.x, vertex.y, p.x, p.y) < 2)//
						{
						   res = true;
						   idx1 = i;
						   idx2 = j;
						   break;
						}
					}
				 }
				 else if (z->GetZoneType() == ZTLineSegment && m_mode == MTVector)
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

						if  (_2D_Dist(_p1.x, _p1.y, p.x, p.y) < 2)//
						{
						   res = true;
						   idx1 = i;
						   idx2 = 0;
						   break;
						}

						if  (_2D_Dist(_p2.x, _p2.y, p.x, p.y) < 2)//
						{
						   res = true;
						   idx1 = i;
						   idx2 = 1;
						   break;
						}

					 res = false;
				 }
				 delete z;
			}
			else
			{
				TRect rect = awpRect2TRect(item->GetBounds()->GetRect());
				for (int j = 0; j < 4; j++)
				{
					TPoint vertex = GetRectPoint(j, rect);
					if  (_2D_Dist(vertex.x, vertex.y, p.x, p.y) < 2)//
					{
					   res = true;
					   idx1 = i;
					   idx2 = j;
					   break;
					}
				}
			}
		}
	}
	return res;
}
//---------------------------------------------------------------------------
TPoint TPhImageMarkTool::GetRectPoint(int index, TRect& rect)
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
//---------------------------------------------------------------------------
void __fastcall TPhImageMarkTool::SetVertex(int x, int y)
{
   TLFDetectedItem* item = (TLFDetectedItem*)m_descriptor.Get(m_si);
   TPoint p = m_pImage->GetImagePoint(x, y);
   if (item)
   {
	 TLFZone* z = item->GetZone();
	 if (z == NULL || (z->GetZoneType() == ZTRect && m_mode == MTRect))
	 {
		 TRect rect = awpRect2TRect(item->GetBounds()->GetRect());
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
		 item->SetBounds(_rect, m_pImage->Bitmap->Width, m_pImage->Bitmap->Height);
	 }
	 else if (z->GetZoneType() == ZTLineSegment && m_mode == MTVector)
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
			item->SetZone(z, m_pImage->Bitmap->Width, m_pImage->Bitmap->Height);
	 }
	 delete z;
   }
}

TColor TPhImageMarkTool::GetItemColor(TLFDetectedItem* itm)
{
	if (itm == NULL)
		return clBlack;
	for (int i = 0; i < this->m_dictinary->GetCount(); i++)
	{
		TLFSemanticDictinaryItem* ci = (TLFSemanticDictinaryItem*)this->m_dictinary->Get(i);

		if (strcmp(itm->GetType().c_str(), ci->GetId().c_str()) == 0)
		{
			int color = ci->GetColor();
			return color;
		}
	}
	return clBlack;
}

TEZoneTypes TPhImageMarkTool::GetItemZoneType(TLFDetectedItem* itm)
{
	if (itm == NULL)
		return ZTRect;
	for (int i = 0; i < this->m_dictinary->GetCount(); i++)
	{
		TLFSemanticDictinaryItem* ci = (TLFSemanticDictinaryItem*)this->m_dictinary->Get(i);

		if (strcmp(itm->GetType().c_str(), ci->GetId().c_str()) == 0)
		{
			TLFZone* z = itm->GetZone();
			if (z != NULL)
			{
				TEZoneTypes zt = z->GetZoneType();
				delete z;
				return zt;
			}
		}
	}
	return ZTRect;

}

//---------------------------------------------------------------------------
void TPhImageMarkTool::Draw(TCanvas* Canvas)
{
	if (Canvas == NULL)
		return;

	TColor oldColor =  Canvas->Pen->Color;
	TBrushStyle oldStyle = Canvas->Brush->Style;

	int delta = 4;

	for (int i = 0; i < this->m_descriptor.GetCount(); i++)
	{
		Canvas->Brush->Style = bsSolid;
		TLFDetectedItem* item = (TLFDetectedItem*)m_descriptor.Get(i);
		TColor color = GetItemColor(item);
		if (i == m_selected)
			Canvas->Pen->Width = 2;
		else
			Canvas->Pen->Width = 1;
		Canvas->Pen->Color = color;

		TEZoneTypes zt =  GetItemZoneType(item);
		if (zt == ZTRect)
		{
			TRect rect = awpRect2TRect(item->GetBounds()->GetRect());
			TRect rect1 = this->m_pImage->GetScreenRect(rect);
			Canvas->Brush->Color = color;

			TPoint p1 = rect1.TopLeft();
			TPoint p2 = rect1.BottomRight();
			AnsiString _tmp;
			std::string str = m_dictinary->GetWordByUUID(item->GetType().c_str());
			str += " :";
			_tmp = IntToStr(rect.Width());
			str += _tmp.c_str();
			str += ":";
			_tmp = IntToStr(rect.Height());
			str += _tmp.c_str();
			Canvas->TextOutW(p1.x + delta/2, p1.y - 4*delta, str.c_str());


			Canvas->Ellipse(p1.x-delta, p1.y - delta, p1.x + delta, p1.y + delta);
			Canvas->Ellipse(p2.x-delta, p2.y - delta, p2.x + delta, p2.y + delta);


			p1.x = p2.x;
			Canvas->Ellipse(p1.x-delta, p1.y - delta, p1.x + delta, p1.y + delta);
			p1 = rect1.TopLeft();
			p1.y = p2.y;
			Canvas->Ellipse(p1.x-delta, p1.y - delta, p1.x + delta, p1.y + delta);


			Canvas->Brush->Style = bsClear;
			Canvas->Rectangle(rect1);
		}
		else if (zt == ZTLineSegment)
		{
			awp2DPoint s = item->GetZone()->GetLineSegmnet()->GetStart();
			awp2DPoint f = item->GetZone()->GetLineSegmnet()->GetFinish();

			TPoint p1;
			TPoint p2;

			p1.x = s.X * m_pImage->Bitmap->Width / 100;
			p1.y = s.Y * m_pImage->Bitmap->Height / 100;

			p2.x = f.X * m_pImage->Bitmap->Width / 100;
			p2.y = f.Y * m_pImage->Bitmap->Height / 100;

			p1 = m_pImage->GetScreenPoint(p1.x, p1.y);
			p2 = m_pImage->GetScreenPoint(p2.x, p2.y);


			Canvas->MoveTo(p1.x, p1.y);
			Canvas->LineTo(p2.x, p2.y);
			Canvas->Brush->Color = clRed;
			Canvas->Ellipse(p1.x-delta, p1.y - delta, p1.x + delta, p1.y + delta);
			Canvas->Brush->Color = clBlue;
			Canvas->Ellipse(p2.x-delta, p2.y - delta, p2.x + delta, p2.y + delta);


			AnsiString _tmp;
			std::string str = m_dictinary->GetWordByUUID(item->GetType().c_str());
			//str += " :";
			//_tmp = IntToStr(rect.Width());
			//str += _tmp.c_str();
			//str += ":";
			//_tmp = IntToStr(rect.Height());
			//str += _tmp.c_str();
			Canvas->TextOutW(p2.x + delta/2, p2.y - 4*delta, str.c_str());

		}
	}

	if (m_newRect != NULL)
	{
		Canvas->Pen->Color = clLime;
		Canvas->Brush->Color = clLime;

		TRect rect2 = *m_newRect;
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

	if (m_newZone != NULL)
	{
		if (m_newZone->IsLineSegment())
		{
			Canvas->Pen->Color = clLime;
			Canvas->Brush->Color = clLime;

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


			Canvas->MoveTo(p1.x, p1.y);
			Canvas->LineTo(p2.x, p2.y);
			Canvas->Brush->Color = clRed;
			Canvas->Ellipse(p1.x-delta, p1.y - delta, p1.x + delta, p1.y + delta);
			Canvas->Brush->Color = clBlue;
			Canvas->Ellipse(p2.x-delta, p2.y - delta, p2.x + delta, p2.y + delta);

		}
	}
}
//---------------------------------------------------------------------------
void TPhImageMarkTool::MouseDown(int X, int Y, TMouseButton Button)
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
	  }
	  else
	  {
		if (m_mode == MTRect)
		{
			// new rect
			m_newRect = new TRect();
			TPoint p = m_pImage->GetImagePoint(X, Y);
			m_newRect->Left = p.x;
			m_newRect->top  = p.y;
		}
		else if (m_mode == MTVector)
		{
		  m_newZone = new TLFZone(ZTLineSegment);
		  TPoint p = m_pImage->GetImagePoint(X, Y);
		  awp2DPoint p2d;
		  p2d.X = 100.0*(double)p.x/(double)m_pImage->Bitmap->Width;
		  p2d.Y = 100.0*(double)p.y/(double)m_pImage->Bitmap->Height;

		  m_newZone->GetLineSegmnet()->SetStart(p2d);
		}

	  }
	  m_edited = true;
	  m_down = true;
   }
}
//---------------------------------------------------------------------------
void TPhImageMarkTool::MouseUp(int X, int Y, TMouseButton Button)
{
   if (m_pImage == NULL)
		return;
   if (Button == mbLeft)
   {
	  if (m_newRect != NULL || m_newZone != NULL)
	  {
		 if (m_mode == MTRect)
		 {
			 // process new rect
			  TPoint p = m_pImage->GetImagePoint(X, Y);
			  m_newRect->right = p.x;
			  m_newRect->bottom = p.y;

			  if (m_newRect->Width() > _IM_MIN_WIDTH_ && m_newRect->Height() > _IM_MIN_HEIGHT_)
				  DoPopup(X, Y);
			  else
			  {
				 delete m_newRect;
				 m_newRect = NULL;
				 m_pImage->Paint();
			  }
		 }
		 else if (m_mode == MTVector)
		 {

				TPoint p = m_pImage->GetImagePoint(X, Y);
				awp2DPoint p2d;
				p2d.X = 100.0*(double)p.x/m_pImage->Bitmap->Width;
				p2d.Y = 100.0*(double)p.y/m_pImage->Bitmap->Height;

				m_newZone->GetLineSegmnet()->SetFihish(p2d);
				DoPopup(X, Y);
		 }

	  }
	  else
	  {
		// process vertex
        m_descriptor.SaveXML(m_strName.c_str());
        if (this->m_OnChange != NULL)
            m_OnChange(this);
	  }
	  m_pImage->Paint();
	  m_down = false;
   }
}
//---------------------------------------------------------------------------
void TPhImageMarkTool::MouseMove(int X, int Y, TShiftState Shift)
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
	   if (m_newRect != NULL || m_newZone != NULL)
	   {
			TPoint p = m_pImage->GetImagePoint(X, Y);
			if (m_mode == MTRect)
			{
				m_newRect->right = p.x;
				m_newRect->bottom = p.y;
			}
			else if (m_mode == MTVector)
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
			this->SetVertex(X,Y);
	   }
	   m_pImage->Paint();
   }
}
//---------------------------------------------------------------------------
void TPhImageMarkTool::Reset()
{
   if (m_pImage == NULL)
		return;
	m_pImage->Paint();
	if (this->m_OnChange != NULL)
		m_OnChange(this);
}
//---------------------------------------------------------------------------
void __fastcall  TPhImageMarkTool::DeleteEntry(int index)
{
/*	for (int i = m_frames.GetCount() - 1; i >= 0; i--)
	{
		TFrameItem* itm = (TFrameItem*)m_frames.Get(i);
		if (itm && itm->FrameNum == index)
        {
            if (this->OnDelFrame)
                this->OnDelFrame(this, itm);
            m_frames.Delete(i);
        }
	}
    this->SaveData();*/
}
//---------------------------------------------------------------------------

bool TPhImageMarkTool::CheckZoneType(TLFSemanticDictinaryItem* sdi)
{
	if (m_mode == MTRect && sdi->GetZoneType() == ZTRect)
		return true;
	if (m_mode == MTVector && sdi->GetZoneType() == ZTLineSegment)
		return true;
	if (m_mode == MTContour && sdi->GetZoneType() == ZTContour)
		return true;

	return false;
}

void __fastcall TPhImageMarkTool::DoPopup(int X, int Y)
{
    PopupMenu = new TPopupMenu(NULL);
    PopupMenu->AutoPopup = false;
    for (int i = 0; i < m_dictinary->GetCount(); i++)
    {
		TLFSemanticDictinaryItem* mi = (TLFSemanticDictinaryItem*)m_dictinary->Get(i);
		if (!CheckZoneType(mi))
			continue;

		TMenuItem* item = new TMenuItem(PopupMenu);

        item->Caption = mi->GetItemLabel();
        item->OnClick = PopupClick;
        item->Tag = i+1;
		PopupMenu->Items->Add(item);
    }

    TMenuItem* item = new TMenuItem(PopupMenu);
    item->Caption = L"Cancel";
    item->OnClick = PopupClick;
    item->Tag = 0;
    PopupMenu->Items->Add(item);
    TPoint p = Application->MainForm->ClientOrigin;
    p.X += Form1->Panel1->Width;
    PopupMenu->Popup(p.X + X, p.Y + Y);
}

void __fastcall TPhImageMarkTool::PopupClick(TObject* sender)
{
    TMenuItem* item = (TMenuItem*)sender;
	if (item->Tag - 1 < 0)
	{
		if (m_newRect != NULL)
		{
			delete m_newRect;
			m_newRect = NULL;
		}
		if (m_newZone != NULL)
		{
			delete m_newZone;
			m_newZone = NULL;
		}
		m_pImage->Paint();
		delete  PopupMenu;
        return;
    }

	TLFDetectedItem* di = new TLFDetectedItem();
	TLFSemanticDictinaryItem* dict_item = m_dictinary->GetWordFromDictinary(item->Tag - 1);
	if (m_mode == MTRect)
	{
		awpRect bounds = TRect2awpRect(*m_newRect);
		di->SetBounds(bounds, m_pImage->Bitmap->Width, m_pImage->Bitmap->Height);
		delete m_newRect;
		m_newRect = NULL;
	}
	else if (m_mode == MTVector)
	{
		di->SetZone(m_newZone, m_pImage->Bitmap->Width, m_pImage->Bitmap->Height);
		delete m_newZone;
		m_newZone = NULL;
	}

	std::string uuid =  dict_item->GetId();
	di->SetType(uuid.c_str());
	ILFScanner* s = dict_item->GetScanner();
	di->m_bh = s->GetBaseHeight();
	di->m_bw = s->GetBaseWidth();
	di->SetDetectorName("Human marked");
	m_descriptor.Add(di);
	m_descriptor.SaveXML(m_strName.c_str());

    if (this->m_OnChange != NULL)
    	m_OnChange(this);

    m_pImage->Paint();
	delete  PopupMenu;
}


void __fastcall  TPhImageMarkTool::SetSelected(int index)
{
   m_selected = index;
   m_pImage->Paint();
}

void __fastcall  TPhImageMarkTool::DeleteObject(int index)
{
    if (index >= 0 && index < m_descriptor.GetCount())
    {
        m_descriptor.Delete(index);
/*        TFrameItem* itm = GetFrameItem(m_mediaSource->CurrentFrame);
        if (itm != NULL)
        {
            itm->Delete(index);
            if (itm->GetCount() == 0)
			{
                DeleteEntry(m_mediaSource->CurrentFrame);
            }
        } */
        m_pImage->Paint();
    }
}


void __fastcall TPhImageMarkTool::ChangeLabel(String& src_label, String& dst_label)
{
    UnicodeString src = src_label;
    UnicodeString dst = dst_label;

/*
    for (int i = 0; i < m_classes->Count; i++)
    {
	    TMarkItem* ci = (TMarkItem*)this->m_classes->Items[i];

        if (ci->label == src)
            ci->label = dst;
	}

    for (int i = 0; i < m_frames.GetCount(); i++)
    {
		TFrameItem* fitem = (TFrameItem*)m_frames.Get(i);
        for (int j = 0; j < fitem->GetCount(); j++)
        {
			TMarkItem* item = (TMarkItem*)fitem->Get(j);
            if (item->label == src)
            	item->label = dst;
        }
    }
    SaveData();*/
}

void __fastcall TPhImageMarkTool::DeleteLabel(String label)
{
/*
    for (int i = m_frames.GetCount() - 1; i >=0 ; i--)
    {
		TFrameItem* fitem = (TFrameItem*)m_frames.Get(i);
        for (int j = fitem->GetCount() -1; j >=0; j--)
        {
			TMarkItem* item = (TMarkItem*)fitem->Get(j);
            if (item->label == label)
                 fitem->Delete(j);
        }
        if(fitem->GetCount() == 0)
            m_frames.Delete(i);
    }

    for (int i = m_classes->Count - 1; i >= 0; i--)
    {
	    TMarkItem* ci = (TMarkItem*)this->m_classes->Items[i];

        if (ci->label == label)
        {
            m_classes->Delete(i);
        }
    }

	SaveData();   */
	if (m_OnLoad != NULL)
		m_OnLoad(this);
}

void __fastcall TPhImageMarkTool::LabelStatistics(String label, int& numFrames, int& numEntries)
{
 /*   numFrames = 0;
    numEntries = 0;
    for (int i = m_frames.GetCount() - 1; i >=0 ; i--)
    {
		TFrameItem* fitem = (TFrameItem*)m_frames.Get(i);
        bool f = false;
        for (int j = fitem->GetCount() -1; j >=0; j--)
        {
			TMarkItem* item = (TMarkItem*)fitem->Get(j);
            if (item->label == label)
            {
                numEntries++;
                f = true;
			}
        }
        if (f)
            numFrames++;
    } */
}
TLFSemanticDictinaryItem* __fastcall TPhImageMarkTool::GetClasses(int index)
{
    return (TLFSemanticDictinaryItem*)m_dictinary->Get(index);
}

void __fastcall TPhImageMarkTool::SetDictionary(TLFSemanticDictinary* value)
{
    this->m_dictinary = value;
}

void __fastcall  TPhImageMarkTool::SetFrame(const char* lpFileName)
{
	  m_descriptor.LoadXML(lpFileName);
	  m_strName = lpFileName;
	  m_pImage->Paint();
}

void __fastcall TPhImageMarkTool::SetMode(const TEMarkToolModes Value)
{
	//
	m_mode = Value;
//typedef enum {MTRect, MTVector, MTContour} TEMarkToolModes;

	switch(m_mode)
	{
		case MTRect:
			m_strToolName = "RECT ITEM";
		break;
		case MTVector:
			m_strToolName = "VECTOR ITEM";
		break;
		case MTContour:
			m_strToolName = "POLYGON ITEM";
		break;
	}
}

