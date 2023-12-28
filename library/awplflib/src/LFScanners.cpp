//---------------------------------------------------------------------------
#include "LFScanners.h"
#include "LFParameter.h"

ILFScanner::ILFScanner()
{
	this->m_BaseHeight = 24;
	this->m_BaseWidth = 24;

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
	return fragments_.size();
}
TLFBounds* ILFScanner::GetFragment(int index)
{
	return &fragments_.at(index);
}

awpRect	 ILFScanner::GetFragmentRect(int index)
{
	return GetFragment(index)->Rect;
}
void ILFScanner::Clear()
{
	fragments_.clear();
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
	if (fragments_.empty())
		return false;

	for (auto& fragment : fragments_) {
		fragment.Rect.left += rect.left;
		fragment.Rect.right += rect.left;
		fragment.Rect.top += rect.top;
		fragment.Rect.bottom += rect.top;
	}
	
	return true;
}

bool ILFScanner::Filter(awpRect& rect)
{
	int count = 0;
	std::vector<TLFBounds> filtered;
	filtered.reserve(fragments_.size());

	// the first pass calculates the number of elements
	for (auto& fragment : fragments_) {
		const awpRect& r = fragment.Rect;
		if (r.left > rect.left && r.right < rect.right &&
			r.top > rect.top && r.bottom < rect.bottom) {
			fragment.ItemIndex = filtered.size();
			filtered.emplace_back(std::move(fragment));
		}
	}

	if (filtered.empty()) 
		return false;

	fragments_ = filtered;

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

}
bool TLFScanner::Scan(int w0, int h0)
{
		
	fragments_.clear();

	if (w0 <= 0 || h0 <= 0)
		return false;
		
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
		_width  =  width - 2.f;
		_height = _width*m_ar;
	}
	else
	{
	   _height = height-2.f;
	   _width = _height / m_ar;
	}

	double stepx = floor(st*_width / 100.f +0.5f);
	double stepy = floor(st*_height / 100.f +0.5f);

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
					awpRect rect = { (AWPSHORT)x, (AWPSHORT)y, (AWPSHORT)x + (AWPSHORT)_width, (AWPSHORT)y + (AWPSHORT)_height };
					
					
						/*m_Fragments[c].Rect.left = (AWPSHORT)x;
					m_Fragments[c].Rect.top = (AWPSHORT)y;
					m_Fragments[c].Rect.bottom = m_Fragments[c].Rect.top + (AWPSHORT)_height;
					m_Fragments[c].Rect.right = m_Fragments[c].Rect.left + (AWPSHORT)_width;
					m_Fragments[c].HasObject = false;
					m_Fragments[c].Angle = 0;
					m_Fragments[c].ItemIndex = -1;*/

					if (fragments_.empty())
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
						if (m_maxX < x + _width)
							m_maxX = (unsigned int)(x + _width);
						if (m_minY > y)
							m_minY = (unsigned int)y;
						if (m_maxY < y + _height)
							m_maxY = (unsigned int)(y + _height);
					}
					fragments_.push_back({ 0, fragments_.size(), rect});
										
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
	fragments_.clear();
	double overlap = (double)((TLFParameter*)(this->m_Params.Get(0)))->GetValue();
	overlap = 1 - overlap / 100.;

	int num_x = (int)floor((double)w / (overlap*this->m_BaseWidth));
	int num_y = (int)floor((double)h / (overlap*this->m_BaseHeight));

	size_t fragments = num_x * num_y;

	fragments_.reserve(fragments);

	for (int i = 0; i < num_y; i++)
	{
		for (int j = 0; j < num_x; j++)
		{
			if (overlap*j*this->m_BaseWidth + m_BaseWidth > w || overlap*i*this->m_BaseHeight + m_BaseHeight > h)
				continue;

			awpRect rect;
		
			rect.left = AWPSHORT(overlap*j*this->m_BaseWidth + 0.5);
			rect.top  = AWPSHORT(overlap*i*this->m_BaseHeight + 0.5);

            if (overlap*j*this->m_BaseWidth + m_BaseWidth == w)
				rect.right = rect.left + this->m_BaseWidth - 1;
            else
				rect.right = rect.left + this->m_BaseWidth;

			if (overlap * i * this->m_BaseHeight + m_BaseHeight == h)
				rect.bottom = rect.top + this->m_BaseHeight - 1;
			else
				rect.bottom = rect.top + this->m_BaseHeight;

			if (fragments_.empty())
			{
				m_minX = rect.left;
				m_maxX = rect.left + m_BaseWidth;
				m_minY = rect.top;
				m_maxY = rect.top + m_BaseHeight;
			}
			else
			{
				if ((int)m_minX > rect.left)
					m_minX = rect.left;
				if (m_maxX < rect.left + m_BaseWidth)
					m_maxX = rect.left + m_BaseWidth;
				if ((int)m_minY > rect.top)
					m_minY = rect.top;
				if (m_maxY < rect.top + m_BaseHeight)
					m_maxY = rect.top + m_BaseHeight;
			}

			fragments_.push_back({ 0, fragments_.size(), rect});

		}
	}
	
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
	fragments_.clear();

 	double o = (double)m_overlap;
	o = 1 - o / 100.;
		
	int size = 0;
	double bh = (double)m_BaseHeight*m_min_scale;
	double bw = (double)m_BaseWidth*m_min_scale;
	do
	{
		int num_x = (int)floor((double)w / (o*bw));
		int num_y = (int)floor((double)h / (o*bh));
		int num = num_x*num_y;
		size += num;
		if (num > 0)
		{		
			fragments_.reserve(size);

			for (int i = 0; i < num_y; i++)
			{
				for (int j = 0; j < num_x; j++)
				{
					if (o*j*bw + bw >= w || o*i*bh + bh >= h)
						continue;

					awpRect rect;
										
					rect.left = AWPSHORT(o*j*bw);
					rect.top = AWPSHORT(o*i*bh);
					rect.right = AWPSHORT(rect.left + bw);
					rect.bottom = AWPSHORT(rect.top + bh);

					fragments_.push_back({ 0, fragments_.size(), rect});
					
				}
			}

		}
		bh *= m_grow;
		bw *= m_grow;
	}while(bh < m_BaseHeight*m_max_scale && bw < m_BaseWidth*m_max_scale);
	return true;
}

TLFAllScanner::TLFAllScanner() : ILFScanner()
{

}

bool TLFAllScanner::Scan(int w, int h)
{
	fragments_.clear();
	fragments_.reserve((w - m_BaseWidth / 2) * (h - m_BaseHeight / 2));

	for (unsigned int y = m_BaseHeight / 2; y < h - m_BaseHeight / 2; y++)
	{
		for (unsigned int x = m_BaseWidth / 2; x < w - m_BaseWidth / 2; x++)
		{
			awpRect rect;
			
			rect.left  = x - m_BaseWidth / 2;
			rect.right = x + m_BaseWidth / 2;
			rect.top = y - m_BaseHeight / 2;
			rect.bottom = y + m_BaseHeight / 2;
			
			fragments_.push_back({ 0, fragments_.size(), rect});
		}
	}
	return true;
}


TLFWholeImageScanner::TLFWholeImageScanner() : ILFScanner()
{
	
}
bool TLFWholeImageScanner::Scan(int w, int h)
{
	fragments_.clear();
	
	awpRect rect;

	rect.left = 0;
	rect.right = w;
	rect.top = 0;
	rect.bottom = h;

	fragments_.push_back({ 0, 0, rect });

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
