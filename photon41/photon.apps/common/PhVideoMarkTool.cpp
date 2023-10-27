//---------------------------------------------------------------------------
#pragma hdrstop
#include "Dialogs.hpp"
#include "PhVideoMarkTool.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "tinyxml.lib"
//---------------------------------------------------------------------------
TMarkItem::TMarkItem()
{
    this->m_color = clRed;
}
//---------------------------------------------------------------------------
TMarkItem::~TMarkItem()
{}
//---------------------------------------------------------------------------
TFrameItem::TFrameItem()
{}
//---------------------------------------------------------------------------
TFrameItem::~TFrameItem()
{
}
//---------------------------------------------------------------------------
TLFSemanticImageDescriptor* TFrameItem::GetDescriptor(int w, int h)
{
	TLFSemanticImageDescriptor* sdscr = new TLFSemanticImageDescriptor(w,h);
	for (int j = 0; j < GetCount(); j++)
	{
		TMarkItem* mi = (TMarkItem*)Get(j);
		awpRect _arect;
		_arect.left = mi->rect.Left;
		_arect.top  = mi->rect.Top;
		_arect.right = mi->rect.Right;
		_arect.bottom = mi->rect.Bottom;
		AnsiString _ansi = mi->label;
		UUID _id;
		LF_UUID_CREATE(_id)
		TLFDetectedItem* di = new TLFDetectedItem(&_arect, /*rect*/
		0, /*raiting*/
		_ansi.c_str(), /*class label*/
		0, /*angle*/
		0, /*rcurs*/
		24, /*base width*/
		24, /*base height*/
		"Human marked", /*detector*/
		_id /*uuid*/);
		sdscr->AddDetectedItem(di);
	}
	return sdscr;
}
//---------------------------------------------------------------------------
TiXmlElement* TFrameItem::SaveXML(int w, int h)
{
		TiXmlElement* element= new TiXmlElement("TFrameItem");
		element->SetDoubleAttribute("frame", (double)m_frame);

		TLFSemanticImageDescriptor* sdscr = new TLFSemanticImageDescriptor(w,h);
		for (int j = 0; j < GetCount(); j++)
		{
			TMarkItem* mi = (TMarkItem*)Get(j);
			awpRect _arect;
			_arect.left = mi->rect.Left;
			_arect.top  = mi->rect.Top;
			_arect.right = mi->rect.Right;
			_arect.bottom = mi->rect.Bottom;
			AnsiString _ansi = mi->label;
			UUID _id;
			LF_UUID_CREATE(_id)
			TLFDetectedItem* di = new TLFDetectedItem(&_arect, /*rect*/
			0, /*raiting*/
			_ansi.c_str(), /*class label*/
			0, /*angle*/
			0, /*rcurs*/
			24, /*base width*/
			24, /*base height*/
			"Human marked", /*detector*/
			_id /*uuid*/);
			sdscr->AddDetectedItem(di);
		}
		TiXmlElement* ee = sdscr->SaveXML();
		element->LinkEndChild(ee);
		delete sdscr;
		return element;
}
//---------------------------------------------------------------------------
bool TFrameItem::LoadXML(TiXmlElement* parent)
{
	this->Clear();
	if (parent == NULL)
		return false;
	if (strcmp(parent->Value(), "TFrameItem") != 0)
		return false;

	double value;
	parent->QueryDoubleAttribute("frame", &value);
	this->m_frame = (int)value;
	TiXmlElement* e = parent->FirstChildElement();
	if (e != NULL)
	{
		TLFSemanticImageDescriptor d;
		if (d.LoadXML(e))
		{
		   for (int i = 0; i < d.GetCount(); i++)
		   {
				TLFDetectedItem* di = d.GetDetectedItem(i);
				TMarkItem* mi  = new TMarkItem();
				mi->frame = this->m_frame;
				mi->label = di->GetType().c_str();
				TLFRect* rect = di->GetBounds();
				mi->rect.Left = rect->GetRect().left;
				mi->rect.Top = rect->GetRect().top;
				mi->rect.Right = rect->GetRect().right;
				mi->rect.Bottom = rect->GetRect().bottom;
				this->Add(mi);
		   }
		}
	}

	return true;
}

//---------------------------------------------------------------------------
__fastcall TPhVideoMarkTool::TPhVideoMarkTool(TComponent* Owner): TPhImageTool(Owner)
{
	m_classes = new TList();
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
    m_dictinary = new TLFSemanticDictinary();
}
//---------------------------------------------------------------------------
__fastcall TPhVideoMarkTool::~TPhVideoMarkTool()
{
    SaveData();
	delete m_classes;
    delete m_dictinary;

	if (m_newRect != NULL)
		delete m_newRect;
}
//---------------------------------------------------------------------------
double TPhVideoMarkTool::_2D_Dist(double x1,double y1,double x2,double y2)
{
	return sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
}
//---------------------------------------------------------------------------
bool   TPhVideoMarkTool::_is_near_vertex(int X, int Y, int& idx1, int& idx2)
{
   bool res = false;

   if (m_pImage == NULL)
		return res;

   TPoint p = m_pImage->GetImagePoint(X, Y);

	for (int i = 0; i < m_data.GetCount(); i++)
	{
		TMarkItem* item = (TMarkItem*)m_data.Get(i);
		if (item != NULL)
		{
			TRect rect = item->rect;
			for (int j = 0; j < 4; j++)
			{
				TPoint vertex = GetRectPoint(j, rect);
				if  (_2D_Dist(vertex.x, vertex.y, p.x, p.y) < 4)//
				{
				   res = true;
				   idx1 = i;
				   idx2 = j;
				   break;
				}
			}

		}
	}
	return res;
}
//---------------------------------------------------------------------------
TPoint TPhVideoMarkTool::GetRectPoint(int index, TRect& rect)
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
void __fastcall TPhVideoMarkTool::SetVertex(int x, int y)
{
   TMarkItem* item = (TMarkItem*)m_data.Get(m_si);
   TPoint p = m_pImage->GetImagePoint(x, y);
   if (item)
   {
	 if (m_sv == 0)
	 {
		item->rect.Left = p.x;
		item->rect.Top  = p.y;
	 }
	 else if (m_sv == 1)
	 {
		item->rect.right = p.x;
		item->rect.Top  = p.y;
	 }
	 else if (m_sv == 2)
	 {
		item->rect.right = p.x;
		item->rect.bottom  = p.y;
	 }
	 else if (m_sv == 3)
	 {
		item->rect.left = p.x;
		item->rect.bottom  = p.y;
	 }
   }
}

TColor TPhVideoMarkTool::GetItemColor(TMarkItem* itm)
{
    if (itm == NULL)
        return clBlack;
    for (int i = 0; i < this->m_classes->Count; i++)
    {
	    TMarkItem* ci = (TMarkItem*)this->m_classes->Items[i];
        if (ci->label == itm->label)
            return ci->color;
    }
    return clBlack;
}

//---------------------------------------------------------------------------
void TPhVideoMarkTool::Draw(TCanvas* Canvas)
{
	if (Canvas == NULL)
		return;

	TColor oldColor =  Canvas->Pen->Color;
	TBrushStyle oldStyle = Canvas->Brush->Style;

	int delta = 5;



	for (int i = 0; i < this->m_data.GetCount(); i++)
	{
		Canvas->Brush->Style = bsSolid;
		TMarkItem* item = (TMarkItem*)m_data.Get(i);
        item->color = GetItemColor(item);
		TRect rect = item->rect;
		TRect rect1 = this->m_pImage->GetScreenRect(rect);
    	Canvas->Brush->Color = item->color;
        if (i == m_selected)
            Canvas->Pen->Width = 4;
        else
            Canvas->Pen->Width = 1;
		Canvas->Pen->Color = item->color;

		TPoint p1 = rect1.TopLeft();
		TPoint p2 = rect1.BottomRight();

        Canvas->TextOutW(p1.x + delta/2, p1.y + delta/2, item->label);


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
}
//---------------------------------------------------------------------------
void TPhVideoMarkTool::MouseDown(int X, int Y, TMouseButton Button)
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
		// new rect
		m_newRect = new TRect();
		TPoint p = m_pImage->GetImagePoint(X, Y);
		m_newRect->Left = p.x;
		m_newRect->top  = p.y;
	  }
	  m_edited = true;
	  m_down = true;
   }
}
//---------------------------------------------------------------------------
void TPhVideoMarkTool::MouseUp(int X, int Y, TMouseButton Button)
{
   if (m_pImage == NULL)
		return;
   if (Button == mbLeft)
   {
	  if (m_newRect != NULL)
	  {
		// process new rect
		  TPoint p = m_pImage->GetImagePoint(X, Y);
		  m_newRect->right = p.x;
		  m_newRect->bottom = p.y;

          if (m_newRect->Width() > 24 && m_newRect->Height() > 24)
	          DoPopup(X, Y);
          else
          {
             delete m_newRect;
             m_newRect = NULL;
             m_pImage->Paint();
          }
/*
		  TMarkItem* item = new TMarkItem();
		  item->rect = *m_newRect;
		  item->label = this->m_classes->Strings[this->m_selectedClass];
		  item->frame = this->MediaSource->CurrentFrame;

		  m_data.Add(item);

		  delete m_newRect;
		  m_newRect = NULL;
		  if (this->m_OnChange != NULL)
			m_OnChange(this);
*/
	  }
	  else
	  {
		// process vertex
	  }
	  m_pImage->Paint();
	  m_down = false;
   }
}
//---------------------------------------------------------------------------
void TPhVideoMarkTool::MouseMove(int X, int Y, TShiftState Shift)
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
	   if (m_newRect != NULL)
	   {
			TPoint p = m_pImage->GetImagePoint(X, Y);
			m_newRect->right = p.x;
			m_newRect->bottom = p.y;
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
void TPhVideoMarkTool::Reset()
{
   if (m_pImage == NULL)
		return;
	m_data.Clear();
	m_pImage->Paint();

    TFrameItem* item = this->GetFrameItem(m_mediaSource->CurrentFrame);
    if (item != NULL)
       this->DeleteEntry(m_mediaSource->CurrentFrame);

	if (this->m_OnChange != NULL)
		m_OnChange(this);
}
//---------------------------------------------------------------------------
__fastcall TPhVideoMarkTool::SetMediaSource(TPhMediaSource* source)
{
	SaveData();
	this->m_mediaSource = source;
	LoadData();
}
//---------------------------------------------------------------------------
void __fastcall TPhVideoMarkTool::SaveData()
{
	if (this->m_mediaSource == NULL)
		return;

	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
	doc.LinkEndChild(decl);
	TiXmlElement* dscr = new TiXmlElement("TPhVideoMarkTool");

	TLFSemanticDictinary dict;
	for (int i = 0; i < m_classes->Count; i++)
	{
        TMarkItem* mi = (TMarkItem*)m_classes->Items[i];
		AnsiString str = mi->label;
		TLFSemanticDictinaryItem* item = new TLFSemanticDictinaryItem(str.c_str());
        int c = ColorToRGB(mi->color);
        item->SetColor(c);
		dict.AddWordToDictinary(item);
	}
	TiXmlElement* e = dict.SaveXML();
	dscr->LinkEndChild(e);
	int iw = this->m_pImage->Bitmap->Width;
	int ih = this->m_pImage->Bitmap->Height;
	for (int i = 0; i < m_frames.GetCount(); i++)
	{
		TFrameItem* fitem = (TFrameItem*)m_frames.Get(i);
		TiXmlElement* ee = fitem->SaveXML(iw, ih);
		dscr->LinkEndChild(ee);
	}

	doc.LinkEndChild(dscr);

	String strName = ChangeFileExt(this->m_mediaSource->Source, L".mark");
	AnsiString _ansi = strName;
	doc.SaveFile(_ansi.c_str());
}
//---------------------------------------------------------------------------
void __fastcall TPhVideoMarkTool::LoadData()
{
	if (this->m_mediaSource == NULL)
		return;
	String strName = ChangeFileExt(this->m_mediaSource->Source, L".mark");
	AnsiString _ansi = strName;

   TiXmlDocument doc;
   if (!doc.LoadFile(_ansi.c_str()))
	return;
   TiXmlHandle hDoc(&doc);
   TiXmlElement* pElem = NULL;

   pElem = hDoc.FirstChildElement().Element();
   if (pElem == NULL)
	return;

   if (strcmp(pElem->Value(), "TPhVideoMarkTool") != 0)
	return;

	pElem = pElem->FirstChildElement();
	if (pElem != NULL)
	{
		TLFSemanticDictinary dict;
		if (!dict.LoadXML(pElem))
			return;
		this->m_classes->Clear();
		this->m_frames.Clear();
		for (int i = 0; i < dict.GetCount(); i++)
		{
			TLFSemanticDictinaryItem* item = dict.GetWordFromDictinary(i);
            TMarkItem* mi = new TMarkItem();
            int c = item->GetColor();
            unsigned char r,g,b;
            awpWebtoRGB(c, &b,&g,&r);
            mi->color = RGB(r,g,b);
            mi->label = item->GetItemLabel();
			m_classes->Add(mi);
		}
		while (pElem = pElem->NextSiblingElement())
		{
			TFrameItem* fi = new TFrameItem();
			if (fi->LoadXML(pElem))
				m_frames.Add(fi);
		}
	}
	if (m_OnLoad != NULL)
		m_OnLoad(this);
}
//---------------------------------------------------------------------------
void __fastcall  TPhVideoMarkTool::NewFrame(bool keepData)
{

	if (m_mediaSource == NULL || m_mediaSource->CurrentFrame == 0)
		return;
    this->m_selected = -1;
	if (m_edited || keepData)
	{
		// save data to prev. frame
		int idx = -1;
		for(int i = 0; i < m_frames.GetCount(); i++)
		{
			TFrameItem* frame = (TFrameItem*)m_frames.Get(i);
			if (frame->FrameNum == m_mediaSource->CurrentFrame-1)
			{
				idx = i;
			}
		}

		if (idx < 0)
		{
			// add data
			if (m_data.GetCount() > 0)
				{
					TFrameItem* frame = new TFrameItem();
					frame->FrameNum =  m_mediaSource->CurrentFrame - 1;
					for (int i = 0; i < m_data.GetCount(); i++)
					{
						TMarkItem* item = (TMarkItem*)m_data.Get(i);
						if (item)
						{
							TMarkItem* out = new TMarkItem();
							out->label = item->label;
							out->rect  = item->rect;
							out->frame = item->frame;
							frame->Add(out);
						}
					}
					m_frames.Add(frame);
					if (m_OnAddData != NULL)
						m_OnAddData(this, frame);
				}
		}
		else
		{
			// change data
			TFrameItem* frame = (TFrameItem*)m_frames.Get(idx);
			frame->Clear();
			frame->FrameNum =  m_mediaSource->CurrentFrame - 1;

			for (int i = 0; i < m_data.GetCount(); i++)
			{
				TMarkItem* item = (TMarkItem*)m_data.Get(i);
				if (item)
				{
					TMarkItem* out = new TMarkItem();
					out->label = item->label;
					out->rect  = item->rect;
					out->frame = item->frame;
					frame->Add(out);
				}
			}

		}
		this->m_edited = false;
	}


	// find data for this frame
	int idx = -1;
	for(int i = 0; i < m_frames.GetCount(); i++)
	{
		TFrameItem* frame = (TFrameItem*)m_frames.Get(i);
		if (frame->FrameNum == m_mediaSource->CurrentFrame)
		{
			// copy items
			idx = i;
			m_data.Clear();
			for (int j = 0; j < frame->GetCount(); j++)
			{
			   TMarkItem* item = (TMarkItem*)frame->Get(j);
			   TMarkItem* out = new TMarkItem();
			   out->rect  = item->rect;
			   out->label = item->label;
			   out->frame = item->frame;
			   m_data.Add(out);
			}
			break;
		}
	}

	if (idx < 0 && !keepData)
		Reset();

}
//---------------------------------------------------------------------------
TLFObjectList* __fastcall TPhVideoMarkTool::GetData()
{
	return &this->m_data;
}
//---------------------------------------------------------------------------
TLFObjectList* __fastcall TPhVideoMarkTool::GetFrames()
{
	return &this->m_frames;

}
//---------------------------------------------------------------------------
void __fastcall  TPhVideoMarkTool::SetFrame(int index)
{

	TFrameItem* itm = (TFrameItem*)m_frames.Get(index);
	if (itm != NULL)
		m_mediaSource->CurrentFrame = itm->FrameNum;
}
//---------------------------------------------------------------------------
void __fastcall  TPhVideoMarkTool::DeleteEntry(int index)
{
	for (int i = m_frames.GetCount() - 1; i >= 0; i--)
	{
		TFrameItem* itm = (TFrameItem*)m_frames.Get(i);
		if (itm && itm->FrameNum == index)
        {
            if (this->OnDelFrame)
                this->OnDelFrame(this, itm);
            m_frames.Delete(i);
        }
	}
    this->SaveData();
}
//---------------------------------------------------------------------------
TLFSemanticDictinary* TPhVideoMarkTool::MakeDictinary()
{
	TLFSemanticDictinary*  dict = new TLFSemanticDictinary();
	for (int i = 0; i < this->m_classes->Count; i++)
	{
        TMarkItem* mi = (TMarkItem*)m_classes->Items[i];
		AnsiString str = mi->label;
		TLFSemanticDictinaryItem* item = new TLFSemanticDictinaryItem(str.c_str());
		dict->AddWordToDictinary(item);
	}
	return dict;
}
//---------------------------------------------------------------------------
void __fastcall  TPhVideoMarkTool::ExportToLFDatabase(const char* path)
{
	 TPhAddDataEvent ee = this->m_OnAddData;
     randomize();
	 m_OnAddData = NULL;
	 if (!LFDirExist(path))
		LFFileExists(path);

	 // Save dictionary
	 TLFSemanticDictinary*  dict = this->MakeDictinary();
	 if (dict != NULL)
	 {
		string fname = path;
		fname += "\\dictinary.xml";
		dict->SaveXML(fname.c_str());
		delete dict;
	 }

     int rnd = rand();

	 for (int i= 0; i < m_frames.GetCount(); i++)
	 {
		TFrameItem* fi = (TFrameItem*)m_frames.Get(i);
		TLFSemanticImageDescriptor* sd = fi->GetDescriptor(m_pImage->Bitmap->Width,
        m_pImage->Bitmap->Height);
		if (sd != NULL)
		{
			// save data
			this->m_mediaSource->CurrentFrame = fi->FrameNum;
			UnicodeString str = path;
            str += "\\";
            str += IntToStr(rnd);
			str += L"_frame_";
			str += IntToStr(fi->FrameNum);
			str += ".png";
			m_pImage->SaveToFile(str.c_str());
			str = ChangeFileExt(str, L".xml");
			AnsiString _ansi = str;
            int progress = 100*i / m_frames.GetCount();
			sd->SaveXML(_ansi.c_str());
            if (m_OnProgress != NULL)
                m_OnProgress(this, progress, str);
			delete sd;
		}
		Application->ProcessMessages();
	 }
	 m_OnAddData = ee;
}

void __fastcall TPhVideoMarkTool::DoPopup(int X, int Y)
{
    PopupMenu = new TPopupMenu(NULL);
    PopupMenu->AutoPopup = false;
    for (int i = 0; i < m_classes->Count; i++)
    {
        TMenuItem* item = new TMenuItem(PopupMenu);
        TMarkItem* mi = (TMarkItem*)m_classes->Items[i];

        item->Caption = mi->label;
        item->OnClick = PopupClick;
        item->Tag = i+1;
        PopupMenu->Items->Add(item);
    }

    TMenuItem* item = new TMenuItem(PopupMenu);
    item->Caption = L"Отменить";
    item->OnClick = PopupClick;
    item->Tag = 0;
    PopupMenu->Items->Add(item);


    TPoint p = Application->MainForm->ClientOrigin;

    PopupMenu->Popup(p.X + X, p.Y + Y);
}

void __fastcall TPhVideoMarkTool::PopupClick(TObject* sender)
{
    TMenuItem* item = (TMenuItem*)sender;
    if (item->Tag - 1 < 0)
    {
        delete m_newRect;
        m_newRect = NULL;
        m_pImage->Paint();
        delete  PopupMenu;
        return;
    }

    TMarkItem* mitem = new TMarkItem();
    TMarkItem* mi = (TMarkItem*)m_classes->Items[item->Tag - 1];

    mitem->rect = *m_newRect;
    mitem->label = mi->label;
    mitem->frame = this->MediaSource->CurrentFrame;
    mitem->color = mi->color;
    m_data.Add(mitem);

    delete m_newRect;
    m_newRect = NULL;
    if (this->m_OnChange != NULL)
    	m_OnChange(this);

    m_pImage->Paint();
    delete  PopupMenu;
}


void __fastcall  TPhVideoMarkTool::SetSelected(int index)
{
   m_selected = index;
   m_pImage->Paint();
}

void __fastcall  TPhVideoMarkTool::DeleteObject(int index)
{
    if (index >= 0 && index < m_data.GetCount())
    {
        m_data.Delete(index);
        TFrameItem* itm = GetFrameItem(m_mediaSource->CurrentFrame);
        if (itm != NULL)
        {
            itm->Delete(index);
            if (itm->GetCount() == 0)
            {
                DeleteEntry(m_mediaSource->CurrentFrame);
            }
        }
        m_pImage->Paint();
        SaveData();
    }
}

int frames_compare(void* item1, void* item2)
{
    TFrameItem* a1 = (TFrameItem*)item1;
    TFrameItem* a2 = (TFrameItem*)item2;

    if (a1->FrameNum == a2->FrameNum)
        return 0;
    if (a1->FrameNum > a2->FrameNum)
        return 1;
    else
        return -1;
}
void __fastcall TPhVideoMarkTool::SortData()
{
    m_frames.Sort(frames_compare);
}

TFrameItem* TPhVideoMarkTool::GetFrameItem(int index)
{
    for (int i = m_frames.GetCount()-1; i>=0; i--)
    {
		TFrameItem* itm = (TFrameItem*)m_frames.Get(i);
        if (itm->FrameNum == index)
        {
            return itm;
        }
    }
   return NULL;
}
/*
    создает словарь по умолчанию. В случае, если

*/
void __fastcall TPhVideoMarkTool::CreateDefaultDictinary()
{
    if (this->m_dictinary != NULL)
    {
        delete m_dictinary;
        m_dictinary = NULL;
    }

}

void __fastcall TPhVideoMarkTool::ChangeLabel(String& src_label, String& dst_label)
{
    UnicodeString src = src_label;
    UnicodeString dst = dst_label;


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
    SaveData();
}

void __fastcall TPhVideoMarkTool::DeleteLabel(String label)
{

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

    SaveData();
	if (m_OnLoad != NULL)
		m_OnLoad(this);
}

void __fastcall TPhVideoMarkTool::LabelStatistics(String label, int& numFrames, int& numEntries)
{
    numFrames = 0;
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
    }
}

