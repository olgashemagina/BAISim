//---------------------------------------------------------------------------
#include "_LF.h"

ILFScanner::ILFScanner()
{
	this->m_BaseHeight = 24;
	this->m_BaseWidth = 24;
	m_FragmentsCount = 0;
	m_Fragments = NULL;

	m_minX = 0;
	m_minY = 0;
	m_maxX = 0;
	m_maxY = 0;
}

ILFScanner::~ILFScanner()
{
	Clear();
}
int ILFScanner::GetFragmentsCount()
{
	return m_FragmentsCount;
}
TLFBounds* ILFScanner::GetFragment(int index)
{
	if (m_Fragments == NULL) {
        return NULL;
	}
	return &m_Fragments[index];
}

awpRect	 ILFScanner::GetFragmentRect(int index)
{
	return m_Fragments[index].Rect;
}
void ILFScanner::Clear()
{
	if (m_Fragments != NULL)
		free(m_Fragments);
	m_FragmentsCount = 0;
	m_Fragments = NULL;
}


int ILFScanner::GetParamsCount()
{
	return this->m_Params.GetCount();
}
TLFParameter* ILFScanner::GetParameter(int index)
{
	if (index < 0 || index >= this->m_Params.GetCount())
		return NULL;
	return (TLFParameter*)this->m_Params.Get(index);
}

int ILFScanner::GetBaseWidth()
{
	return this->m_BaseWidth;
}
void ILFScanner::SetBaseWidth(int BaseWidth)
{
	this->m_BaseWidth = BaseWidth;
}

int  ILFScanner::GetBaseHeight()
{
    return this->m_BaseHeight;
}
void ILFScanner::SetBaseHeight(int BaseHeight)
{
    this->m_BaseHeight = BaseHeight;
}



bool ILFScanner::ScanImage(TLFImage* pImage)
{
	if (pImage == NULL)
		return false;
	if (pImage->GetImage() == NULL)
		return false;
	int w = pImage->GetImage()->sSizeX;
	int h = pImage->GetImage()->sSizeY;
	return Scan(w,h);
}

bool ILFScanner::ScanRect(awpRect& rect)
{
	int w = rect.right - rect.left;
	int h = rect.bottom - rect.top;
	if (!Scan(w,h))
		return false;
	if (m_FragmentsCount == 0)
		return false;
	for (int i = 0; i < m_FragmentsCount; i++)
	{
		m_Fragments[i].Rect.left += rect.left;
		m_Fragments[i].Rect.right += rect.left;
		m_Fragments[i].Rect.top += rect.top;
		m_Fragments[i].Rect.bottom += rect.top;
	}
	return true;
}

bool ILFScanner::Filter(awpRect& rect)
{
	int count = 0;
	// the first pass calculates the number of elements
	for (int i = 0; i < m_FragmentsCount; i++)
	{
		awpRect r = m_Fragments[i].Rect;
		if (r.left > rect.left && r.right < rect.right &&
			r.top > rect.top && r.bottom < rect.bottom)
			count++;
	}
	if (count == 0)
		return false;
	TLFBounds* b = (TLFBounds*)malloc(count*sizeof(TLFBounds));
	if (b == NULL)
		return false;
	// the second pass copies the elements into a new array b
	int c = 0;
	for (int i = 0; i < m_FragmentsCount; i++)
	{
		awpRect r = m_Fragments[i].Rect;
		if (r.left > rect.left && r.right < rect.right &&
			r.top > rect.top && r.bottom < rect.bottom)
		{
			b[c] = m_Fragments[i];
			c++;
		}
	}
	free(m_Fragments);
	m_Fragments = b;
	m_FragmentsCount = count;
	return true;
}


bool ILFScanner::LoadXML(TiXmlElement* parent)
{
	if (parent == NULL)
		return false;
    const char* type = parent->Attribute("type");
    const char* name = this->GetName();
	if (strcmp(type, name) != 0)
	{
		printf("error: type = %s name = %s\n", type, name);
		return false;
	}
	int value;
	parent->QueryIntAttribute("ApertureWidth", &value);
	m_BaseWidth = (unsigned int)value;
	parent->QueryIntAttribute("ApertureHeight", &value);
	m_BaseHeight = (unsigned int)value;

	m_Params.Clear();
    TLFParameter* p = new  TLFParameter();
    TiXmlNode* e = NULL;

    while((e = parent->IterateChildren(e)) != NULL)
	{
       if (strcmp(e->Value(), p->GetName()) != 0)
        continue;
        
       TLFParameter* pp = new  TLFParameter();
       if (!pp->LoadXML(e->ToElement()))
       {
          this->m_Params.Clear();
		  delete p;
          return false;
       }
       else
        this->m_Params.Add(pp);
    }
    delete p;
	return true;
}

TiXmlElement* ILFScanner::SaveXML()
{
	TiXmlElement* elem = new TiXmlElement("ILFScanner");
	elem->SetAttribute("type", GetName());
	elem->SetAttribute("ApertureWidth", this->m_BaseWidth);
	elem->SetAttribute("ApertureHeight", this->m_BaseHeight);
	for (int i = 0; i < this->m_Params.GetCount(); i++)
	{
		TLFParameter* p = this->GetParameter(i);
		if (p)
		{
			TiXmlElement* e = p->SaveXML();
			elem->LinkEndChild(e);
		}
	}
	return elem;
}


unsigned int ILFScanner::GetMinX()
{
	return m_minX;
}
unsigned int ILFScanner::GetMaxX()
{
	return m_maxX;
}
unsigned int ILFScanner::GetMinY()
{
	return m_minY;
}

unsigned int ILFScanner::GetMaxY()
{
	return m_maxY;
}



//-----------------------------------

TLFScanner::TLFScanner() : ILFScanner()
{
    this->m_BaseHeight = 24;
	this->m_BaseWidth = 24;

	TLFParameter* p1 = new  TLFParameter();
	p1->SetPName("step");
	p1->SetDescription("step space as a percentage of baseline resolution");

	p1->SetMaxValue(100);
	p1->SetMinValue(1);

	p1->SetValue(10);
	this->m_Params.Add(p1);


    TLFParameter* p2 = new  TLFParameter();
    p2->SetPName("grow");
    p2->SetDescription("step on the scale");

    p2->SetMaxValue(2);
    p2->SetMinValue(1.1);

    p2->SetValue(1.1);
    this->m_Params.Add(p2);

    TLFParameter* p3 = new  TLFParameter();
    p3->SetPName("MinSize");
    p3->SetDescription("min size of scanned object in the BaseWidth");

    p3->SetMinValue(1);
	p3->SetMaxValue(4);
    p3->SetValue(1);

    this->m_Params.Add(p3);

    TLFParameter* p4 = new  TLFParameter();
    p4->SetPName("MaxSize");
    p4->SetDescription("max size of scanned object in the BaseWidth");
    p4->SetMinValue(1);
    p4->SetMaxValue(100);
	p4->SetValue(50);

	this->m_Params.Add(p4);
}
TLFScanner::~TLFScanner()
{
	if (m_Fragments != NULL)
	{
		free(m_Fragments);
		m_Fragments = NULL;
	}
}
bool TLFScanner::Scan(int w0, int h0)
{

	if (m_Fragments != NULL)
	{
		free(m_Fragments);
		m_Fragments = NULL;
	}

	if (w0 <= 0 || h0 <= 0)
		return false;

	m_FragmentsCount = 0;
	double st = (double)((TLFParameter*)(this->m_Params.Get(0)))->GetValue();
	double grow = (double)((TLFParameter*)(this->m_Params.Get(1)))->GetValue();
	double mins = (double)((TLFParameter*)(this->m_Params.Get(2)))->GetValue();
	double maxs = (double)((TLFParameter*)(this->m_Params.Get(3)))->GetValue();

	int width  = w0;
	int height = h0;
	int w =  this->m_BaseWidth;
	double m_ar = (double)this->m_BaseHeight / (double)this->m_BaseWidth;
	int h = (AWPSHORT)floor(w*m_ar + 0.5);
	double _height;// = 0;
	double _width;// = 0;
	if (w > h)
	{
		_width  =  width-2.f;
		_height = _width*m_ar;
	}
	else
	{
	   _height = height-2.f;
	   _width = _height / m_ar;
	}

	double stepx = floor(st*_width / 100.f +0.5f);
	double stepy = floor(st*_height / 100.f +0.5f);

	stepx = stepx < 1 ? 4.:stepx;
	stepy = stepy < 1 ? 4.:stepy;

	while (_width >= mins*m_BaseWidth)
	{
		if (_width >= maxs*m_BaseWidth)
		{
			_width /= grow;
			_height /= grow;
			continue;
		}
		double y = 0;
		double ey = y + _height;
		while (ey < height-1)
		{
			double x = 0;
			double ex = x + _width;
			while(ex < width-1)
			{

				m_FragmentsCount++;

				ex +=stepx;
				x += stepx;
			}
			y  += stepy;
			ey += stepy;
		}

		_width /=  grow;
		_height /= grow;

		stepx = st*_width / 100;
		stepx = stepx < 4 ? 4:stepx;
		stepy = st*_height / 100;
		if (stepy < 4 )
			stepy = 4;
	}
	if (m_FragmentsCount == 0)
		return false;
	m_Fragments =  (TLFBounds*)malloc(m_FragmentsCount* sizeof(TLFBounds));
	if (m_Fragments == NULL)
	{
        m_FragmentsCount = 0;
		return false;
	}

	if (w > h)
	{
		_width  =  width - 2.f;
		_height = _width*m_ar;
	}
	else
	{
	   _height = height-2.f;
	   _width = _height / m_ar;
	}

	 stepx = floor(st*_width / 100.f +0.5f);
	 stepy = floor(st*_height / 100.f +0.5f);

	stepx = stepx < 1 ? 4:stepx;
	stepy = stepy < 1 ? 4:stepy;
	int c = 0;

	while (_width >= mins*m_BaseWidth)
	{
	   if (_width >= maxs*m_BaseWidth)
		{
			_width /= grow;
			_height /= grow;
			continue;
		}

		double y = 0;
		double ey = y + _height;
		while (ey < height-1)
		{
			double x = 0;
			double ex = x + _width;
			while(ex < width-1)
			{

			 if (x + _width < w0 || y + _height < h0 )
			 {
				  m_Fragments[c].Rect.left   = (AWPSHORT)x;
				  m_Fragments[c].Rect.top = (AWPSHORT)y;
				  m_Fragments[c].Rect.bottom = m_Fragments[c].Rect.top + (AWPSHORT)_height;
				  m_Fragments[c].Rect.right = m_Fragments[c].Rect.left + (AWPSHORT)_width;
				  m_Fragments[c].HasObject = false;
				  m_Fragments[c].Angle = 0;
				  m_Fragments[c].ItemIndex = -1;

				  if (c == 0)
				  {
					m_minX = (unsigned int)x;
					m_maxX = (unsigned int)(x + _width);
					m_minY = (unsigned int)y;
					m_maxY = (unsigned int)(y + _height);
				  }
				  else
				  {
					if (m_minX > x)
						m_minX = (unsigned int)x;
					if (m_maxX < x+_width)
						m_maxX = (unsigned int)(x + _width);
					if (m_minY > y)
						m_minY = (unsigned int)y;
					if (m_maxY < y + _height)
						m_maxY = (unsigned int)(y + _height);
				  }

				  c++;
				  if (c>=this->m_FragmentsCount)
					  break;
			 }
				ex +=stepx;
				x += stepx;
			}
			y += stepy;
			ey += stepy;
		}

		_width /= grow;
		_height /= grow;

		stepx = st*_width / 100;
		stepx = stepx < 4 ? 4:stepx;
		stepy = st*_height / 100;
		if (stepy < 4 )
			stepy = 4;
	}
	m_FragmentsCount = c;
	return true;
}
//------------------------------------------------------------------------------
TLFTileScanner::TLFTileScanner() : ILFScanner()
{
	TLFParameter* p1 = new  TLFParameter();
	p1->SetPName("overlaps");
	p1->SetDescription("overlap between tiles in the %. min value - 0% max value - 99% default value 25%");

	p1->SetMaxValue(99);
	p1->SetMinValue(0);

	p1->SetValue(50);
	this->m_Params.Add(p1);

	m_numX = 0;
	m_numY = 0;
}

int TLFTileScanner::GetNumX()
{
	return this->m_numX;
}
int TLFTileScanner::GetNumY()
{
	return this->m_numY;
}


bool TLFTileScanner::Scan(int w, int h)
{
	double overlap = (double)((TLFParameter*)(this->m_Params.Get(0)))->GetValue();
	overlap = 1 - overlap / 100.;

	if (m_Fragments != NULL)
		free(m_Fragments);

	int num_x = (int)floor((double)w / (overlap*this->m_BaseWidth));
	int num_y = (int)floor((double)h / (overlap*this->m_BaseHeight));

	this->m_FragmentsCount = num_x*num_y;
	if (m_FragmentsCount == 0)
		return false;
	m_Fragments = (TLFBounds*)malloc(m_FragmentsCount* sizeof(TLFBounds));
	int c = 0;
	for (int i = 0; i < num_y; i++)
	{
		for (int j = 0; j < num_x; j++)
		{
			if (overlap*j*this->m_BaseWidth + m_BaseWidth > w || overlap*i*this->m_BaseHeight + m_BaseHeight > h)
				continue;

			m_Fragments[c].Angle = 0;
			m_Fragments[c].HasObject = false;
			m_Fragments[c].Rect.left = AWPSHORT(overlap*j*this->m_BaseWidth + 0.5);
			m_Fragments[c].Rect.top  = AWPSHORT(overlap*i*this->m_BaseHeight + 0.5);

            if (overlap*j*this->m_BaseWidth + m_BaseWidth == w)
				m_Fragments[c].Rect.right = m_Fragments[c].Rect.left + this->m_BaseWidth - 1;
            else
				m_Fragments[c].Rect.right = m_Fragments[c].Rect.left + this->m_BaseWidth;

           if (overlap*i*this->m_BaseHeight + m_BaseHeight == h)
				m_Fragments[c].Rect.bottom = m_Fragments[c].Rect.top + this->m_BaseHeight - 1;
            else
				m_Fragments[c].Rect.bottom = m_Fragments[c].Rect.top + this->m_BaseHeight;

				  if (c == 0)
				  {
					m_minX = m_Fragments[c].Rect.left;
					m_maxX = m_Fragments[c].Rect.left + m_BaseWidth;
					m_minY = m_Fragments[c].Rect.top;
					m_maxY = m_Fragments[c].Rect.top +m_BaseHeight;
				  }
				  else
				  {
					if ((int)m_minX > m_Fragments[c].Rect.left)
						m_minX = m_Fragments[c].Rect.left;
					if (m_maxX < m_Fragments[c].Rect.left+m_BaseWidth)
						m_maxX = m_Fragments[c].Rect.left + m_BaseWidth;
					if ((int)m_minY > m_Fragments[c].Rect.top)
						m_minY = m_Fragments[c].Rect.top;
					if (m_maxY < m_Fragments[c].Rect.top + m_BaseHeight)
						m_maxY = m_Fragments[c].Rect.top + m_BaseHeight;
				  }

			c++;
		}
	}
	m_FragmentsCount = c;
	m_numX = num_x;
	m_numY = num_y;
	return true;
}

TLFTileScaleScanner::TLFTileScaleScanner() : ILFScanner()
{
	m_overlap = 50;
	m_min_scale = 1;
	m_max_scale = 4;
	m_grow = 1.1;
}
TLFTileScaleScanner::TLFTileScaleScanner(unsigned int bw, unsigned int bh) : ILFScanner()
{
	this->m_BaseHeight = bh;
	this->m_BaseWidth  = bw;

	m_overlap = 50;
	m_min_scale = 1;
	m_max_scale = 4;
	m_grow = 1.1;
}

TLFTileScaleScanner::TLFTileScaleScanner(unsigned int bw, unsigned int bh, double min_scale, double max_scale, double grow, unsigned int overlap) : ILFScanner()
{
	this->m_BaseHeight = bh;
	this->m_BaseWidth  = bw;

	m_overlap = overlap;

	m_min_scale = min_scale;
	m_max_scale = max_scale;

	if (bw*m_min_scale < 8 || bh*m_min_scale < 8)
		m_min_scale = 8. / AWP_MIN(bw,bh);

	m_grow = grow;
}

bool TLFTileScaleScanner::Scan(int w, int h)
{
	if (m_Fragments != NULL)
	{
		free(m_Fragments);
		m_Fragments = NULL;
    }
 	double o = (double)m_overlap;
	o = 1 - o / 100.;



	int num;// =  0;
	int c = 0;
	int size = 0;
	double bh = (double)m_BaseHeight*m_min_scale;
	double bw = (double)m_BaseWidth*m_min_scale;
	do
	{
		int num_x = (int)floor((double)w / (o*bw));
		int num_y = (int)floor((double)h / (o*bh));
		num = num_x*num_y;
		size += num;
		if (num > 0)
		{
			m_Fragments = (TLFBounds*)realloc(m_Fragments, size*sizeof(TLFBounds));

			for (int i = 0; i < num_y; i++)
			{
				for (int j = 0; j < num_x; j++)
				{
					if (o*j*bw + bw >= w || o*i*bh + bh >= h)
						continue;

					m_Fragments[c].Angle = 0;
					m_Fragments[c].HasObject = false;
					m_Fragments[c].Rect.left = AWPSHORT(o*j*bw);
					m_Fragments[c].Rect.top = AWPSHORT(o*i*bh);
					m_Fragments[c].Rect.right = AWPSHORT(m_Fragments[c].Rect.left + bw);
					m_Fragments[c].Rect.bottom = AWPSHORT(m_Fragments[c].Rect.top + bh);
					c++;
				}
			}

		}
		bh *= m_grow;
		bw *= m_grow;
	}while(bh < m_BaseHeight*m_max_scale && bw < m_BaseWidth*m_max_scale);

	m_FragmentsCount = c;
	return true;
}

TLFAllScanner::TLFAllScanner() : ILFScanner()
{
	m_Fragments = NULL;
}

bool TLFAllScanner::Scan(int w, int h)
{
	if (m_Fragments != NULL)
		free(m_Fragments);
	m_FragmentsCount = (w - m_BaseWidth / 2)*(h - m_BaseHeight / 2);
	if (m_FragmentsCount <= 0)
		return false;
	m_Fragments = (TLFBounds*)malloc(m_FragmentsCount* sizeof(TLFBounds));
	int c = 0;
	for (unsigned int y = m_BaseHeight / 2; y < h - m_BaseHeight / 2; y++)
	{
		for (unsigned int x = m_BaseWidth / 2; x < w - m_BaseWidth / 2; x++)
		{
			m_Fragments[c].Angle = 0;
			m_Fragments[c].HasObject = false;
			m_Fragments[c].ItemIndex = 0;
			m_Fragments[c].Rect.left  = x - m_BaseWidth / 2;
			m_Fragments[c].Rect.right = x + m_BaseWidth / 2;
			m_Fragments[c].Rect.top = y - m_BaseHeight / 2;
			m_Fragments[c].Rect.bottom = y + m_BaseHeight / 2;
			c++;
		}
	}
	return true;
}


TLFWholeImageScanner::TLFWholeImageScanner() : ILFScanner()
{
	this->m_Fragments = NULL;
	this->m_FragmentsCount = 0;
}
bool TLFWholeImageScanner::Scan(int w, int h)
{
	if (m_Fragments != NULL)
		free(m_Fragments);
	m_FragmentsCount = 1;
	m_Fragments = (TLFBounds*)malloc(m_FragmentsCount* sizeof(TLFBounds));
	m_Fragments[0].Angle = 0;
	m_Fragments[0].HasObject = false;
	m_Fragments[0].ItemIndex = 0;
	m_Fragments[0].Rect.left = 0;
	m_Fragments[0].Rect.right = w;
	m_Fragments[0].Rect.top = 0;
	m_Fragments[0].Rect.bottom = h;
	return true;
}

ILFScanner* CreateScanner(TiXmlElement* parent)
{
	if (parent == NULL)
		return NULL;
	ILFScanner* scanner = NULL;
	const char* type = parent->Attribute("type");
	TLFString strType = type;
	if (strType == "TLFScanner")
		scanner = new TLFScanner();
	else if (strType == "TLFTileScanner")
		scanner = new TLFTileScanner();
	else if (strType == "TLFTileScaleScanner")
		scanner = new TLFTileScaleScanner();
	else if (strType == "TLFAllScanner")
		scanner = new TLFAllScanner();
	else if (strType == "TLFWholeImageScanner")
		scanner = new TLFWholeImageScanner();
	else
		return NULL;
	if (scanner->LoadXML(parent))
		return scanner;

	delete scanner;
	return NULL;
}
