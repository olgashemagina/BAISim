#include "_LF.h"

TLFZone::TLFZone()
{
  this->m_ZoneType = ZTUnknown;
  m_testRadius = 0.75;
  m_classID = 0;
}

TLFZone::TLFZone(TEZoneTypes type)
{
	this->m_ZoneType = type;
    m_testRadius = 0.75;
  m_classID = 0;
}
TLFZone::TLFZone(TLF2DRect& rect)
{
	this->m_ZoneType = ZTRect;
	this->m_Rect = rect;
    m_testRadius = 0.75;
  m_classID = 0;
}
//
TLFZone::TLFZone(TLF2DContour& contour)
{
    this->m_ZoneType = ZTContour;
    this->m_contour = contour;
    m_testRadius = 0.75;
	  m_classID = 0;
}

TLFZone::TLFZone(TLF2DLineSegment& segment)
{
    this->m_ZoneType = ZTLineSegment;
    m_segment = segment;
    m_testRadius = 0.75;
   m_classID = 0;
}


TLFZone::TLFZone(TLFZone& zone)
{
	this->m_ZoneType = zone.m_ZoneType;
    this->m_Rect = zone.m_Rect;
    this->m_contour =zone.m_contour;
    this->m_segment = zone.m_segment;
    this->m_openPolygon = zone.m_openPolygon;
    m_testRadius = 0.75;
    this->m_classID = zone.m_classID;
}

TLFZone& TLFZone::operator =(TLFZone& zone)
{
	if (this != &zone)
    {
        this->m_ZoneType = zone.m_ZoneType;
        this->m_Rect = zone.m_Rect;
        this->m_contour =zone.m_contour;
        this->m_segment = zone.m_segment;
        this->m_openPolygon = zone.m_openPolygon;
        this->m_testRadius = zone.m_testRadius;
        this->m_classID = zone.m_classID;
    }
	return *this;
}


TEZoneTypes  TLFZone::GetZoneType()
{
	return this->m_ZoneType;
}

const char* TLFZone::ZoneType()
{
	switch(this->m_ZoneType)
    {
        case ZTRect:
				return "Rectangle ";
        case ZTContour:
				return "Polygon  ";
        case ZTLineSegment:
				return "Line Segment  ";
        case ZTOpenPolygon:
				return "Polyline  ";
		case ZTCircle:
				return "Circle  ";
    }
	return "unknown";
}


bool TLFZone::IsRect()
{
	return this->m_ZoneType == ZTRect;
}

bool TLFZone::IsContour()
{
	return this->m_ZoneType == ZTContour;
}

bool TLFZone::IsCircle()
{
	return this->m_ZoneType == ZTCircle;
}


TLF2DRect* TLFZone::GetRect()
{
    if (IsRect())
    {
    	return &m_Rect;
    }
    else
		return NULL;
}

bool TLFZone::IsLineSegment()
{
	return this->m_ZoneType == ZTLineSegment;
}

bool TLFZone::IsOpenPolygon()
{
	return this->m_ZoneType == ZTOpenPolygon;
}

TLF2DContour* TLFZone::GetContour()
{
    if (IsContour())
    {
    	return &this->m_contour;
    }
    else
    	return NULL;
}

TLF2DLineSegment* TLFZone::GetLineSegmnet()
{
	if (IsLineSegment() || this->IsCircle())
    {
      	return &this->m_segment;
    }
    else
    	return NULL;
}

TLF2DOpenPolygon* TLFZone::GetOpenPolygon()
{
  	if (IsOpenPolygon())
    {
    	return &this->m_openPolygon;
    }
    else
    	return NULL;
}

double TLFZone::Square()
{
	if (IsRect())
		return this->m_Rect.Width()*this->m_Rect.Height()/100.;
	else if (this->IsContour())
	{
		double s1, s2, s3, s4;
		awp2DPoint p1 = this->m_contour.GetPoint(0);
		awp2DPoint p2 = this->m_contour.GetPoint(this->m_contour.GetNumPoints() - 1);
		s3 = p1.X*p2.Y;
		s4 = p1.Y*p2.X;
		s1 = 0;
        s2 = 0;
		for (int i = 0; i < this->m_contour.GetNumPoints() - 1; i++)
		{
			p1 = this->m_contour.GetPoint(i);
			p2 = this->m_contour.GetPoint(i+1);
			s1 += p1.X*p2.Y;
			s2 += p1.Y*p2.X;
		}
		return fabs(0.5*(s1 - s2 - s3 + s4)/ 100.);
	}
	else if (IsCircle())
	{
		double r;
		// todo: return square
	}
	else
		return 0;

    return 0;
}
// return bounding box
TLF2DRect* TLFZone::GetBounds()
{
	if (IsRect())
	{
		return new TLF2DRect(m_Rect);
	}
	else if(this->IsContour())
	{
		awp2DPoint p1 = this->m_contour.GetPoint(0);
		double min_x = p1.X;
		double min_y = p1.Y;
		double max_x = p1.X;
		double max_y = p1.Y;
		for (int i = 0; i < this->m_contour.GetNumPoints(); i++)
		{
		   p1 = this->m_contour.GetPoint(i);
		   if (min_x > p1.X)
			min_x = p1.X;
		   if (max_x < p1.X)
			max_x = p1.X;
		   if (min_y > p1.Y)
			min_y = p1.Y;
		   if (max_y < p1.Y)
			max_y = p1.Y;
		}
		TLF2DPoint lt(min_x, min_y);
		TLF2DPoint rb(max_x, max_y);
		return new TLF2DRect(lt.GetPoint(), rb.GetPoint());
	}
	else if (this->IsLineSegment())
	{
		awp2DPoint p1 = m_segment.GetStart();
		awp2DPoint p2 = m_segment.GetFinish();

		double min_x = AWP_MIN(p1.X, p2.X);
		double min_y = AWP_MIN(p1.Y, p2.Y);
		double max_x = AWP_MAX(p1.X, p2.X);
		double max_y = AWP_MAX(p1.Y, p2.Y);
		TLF2DPoint lt(min_x, min_y);
		TLF2DPoint rb(max_x, max_y);
		return new TLF2DRect(lt.GetPoint(), rb.GetPoint());
	}
	else if (this->IsOpenPolygon())
	{
		awp2DPoint p1 = this->m_openPolygon.GetSegment(0)->GetStart();
		double min_x = p1.X;
		double min_y = p1.Y;
		double max_x = p1.X;
		double max_y = p1.Y;
		for (int i = 0; i < this->m_openPolygon.GetCount(); i++)
		{
		   p1 = this->m_openPolygon.GetSegment(i)->GetStart();
		   if (min_x > p1.X)
			min_x = p1.X;
		   if (max_x < p1.X)
			max_x = p1.X;
		   if (min_y > p1.Y)
			min_y = p1.Y;
		   if (max_y < p1.Y)
			max_y = p1.Y;
		}

	   p1 = this->m_openPolygon.GetSegment(m_openPolygon.GetCount()-1)->GetStart();
	   if (min_x > p1.X)
		min_x = p1.X;
	   if (max_x < p1.X)
		max_x = p1.X;
	   if (min_y > p1.Y)
		min_y = p1.Y;
	   if (max_y < p1.Y)
		max_y = p1.Y;


		TLF2DPoint lt(min_x, min_y);
		TLF2DPoint rb(max_x, max_y);
		return new TLF2DRect(lt.GetPoint(), rb.GetPoint());
	}
	else
		return NULL;
}

// returns zone's square in pixels
double TLFZone::SquareOnImage(awpImage* img)
{
	if (img == NULL)
		return 0.;

	int w = img->sSizeX;
    int h = img->sSizeY;

	if (IsRect())
    {
    	int W = (int)(this->m_Rect.Width()*w / 100. + 0.5);
        int H = (int)(this->m_Rect.Height()*h/ 100. + 0.5);
		return W*H;
    }
	else if (this->IsContour())
	{
		double s1, s2, s3,s4;
		awp2DPoint p1 = this->m_contour.GetPoint(0);
		awp2DPoint p2 = this->m_contour.GetPoint(this->m_contour.GetNumPoints() - 1);

        TLF2DPoint pp1(p1);
        TLF2DPoint pp2(p2);


		s3 = (0.01*pp1.GetX()*w)*(0.01*pp2.GetY()*h);
        s4 = (0.01*pp1.GetY()*h)*(0.01*pp2.GetX()*w);
        s2 = 0;
		s1 = 0;
		for (int i = 0; i < this->m_contour.GetNumPoints() - 1;  i++)
		{

			p1 = this->m_contour.GetPoint(i);
			p2 = this->m_contour.GetPoint(i + 1);

			s1 += (0.01*w*p1.X)*(0.01*h*p2.Y);
			s2 += (0.01*h*p1.Y)*(0.01*w*p2.X);
		}
		return fabs(0.5*(s1 - s2 - s3 + s4));
	}
    else
    	return 0.;
}

bool TLFZone::IsPointNearRect(TLF2DPoint& point)
{
    if (!this->IsRect())
    	return false;

	TLF2DPoint p;
    p.SetPoint(this->GetRect()->GetLeftTop());
    if (p.Distance(point) < m_testRadius)
        return true;
    p.SetPoint(this->GetRect()->GetRightBottom());
    if (p.Distance(point) < m_testRadius)
        return true;
    awp2DPoint lt = this->GetRect()->GetLeftTop();
    awp2DPoint rb = this->GetRect()->GetRightBottom();
    awp2DPoint _p;
    _p.X = lt.X;
    _p.Y = rb.Y;
	p.SetPoint(_p);
    if (p.Distance(point) < m_testRadius)
        return true;
    _p.X = rb.X;
    _p.Y = lt.Y;
	p.SetPoint(_p);
    if (p.Distance(point) < m_testRadius)
        return true;
    return false;

}
bool TLFZone::IsPointNearContour(TLF2DPoint& point)
{
    if (!this->IsContour())
    	return false;

	TLF2DPoint p;
    for (int i = 0; i < m_contour.GetNumPoints(); i++)
    {
		awp2DPoint _p = m_contour.GetPoint(i);
        p.SetPoint(_p);
        if (p.Distance(point) < m_testRadius)
        	return true;
    }

    return false;
}
bool TLFZone::IsPointNearLineSegment(TLF2DPoint& point)
{
    if (!this->IsLineSegment())
    	return false;
    TLF2DPoint p;
    awp2DPoint _p;
    _p = this->GetLineSegmnet()->GetStart();
    p.SetPoint(_p);
    if (p.Distance(point) < m_testRadius)
    	return true;
    _p = this->GetLineSegmnet()->GetFinish();
    p.SetPoint(_p);
    if (p.Distance(point) < m_testRadius)
    	return true;

    return false;

}
bool TLFZone::IsPointNearOpenPolygon(TLF2DPoint& point)
{
    if (!this->IsOpenPolygon())
    	return false;
	for (int i = 0; i < m_openPolygon.GetCount(); i++)
    {
        TLF2DLineSegment* ls = m_openPolygon.GetSegment(i);

        TLF2DPoint p;
        awp2DPoint _p;
        _p = ls->GetStart();
        p.SetPoint(_p);
        if (p.Distance(point) < m_testRadius)
            return true;
        _p = ls->GetFinish();
        p.SetPoint(_p);
        if (p.Distance(point) < m_testRadius)
            return true;

    }

    return false;
}

bool TLFZone::IsPointNearVertex(TLF2DPoint& point)
{
 	switch(this->m_ZoneType)
    {
		case ZTRect:
				return IsPointNearRect(point);
		case ZTContour:
				return IsPointNearContour(point);
		case ZTLineSegment:
				return IsPointNearLineSegment(point);
		case ZTOpenPolygon:
				return IsPointNearOpenPolygon(point);
		case ZTCircle:
				return IsPointNearLineSegment(point);
	}

	return false;
}

SZoneVertex TLFZone::SelectRectVertex(TLF2DPoint& point)
{
	SZoneVertex vertex;
    vertex.index = -1;
    vertex.type =  ZTRect;
    memset(&vertex.p, 0, sizeof(vertex.p));

    if (!this->IsRect())
    	return vertex;

	TLF2DPoint p;
    p.SetPoint(this->GetRect()->GetLeftTop());
    if (p.Distance(point) < m_testRadius)
    {
        vertex.index = 0;
        vertex.p =  GetRect()->GetLeftTop();
        return vertex;
    }
    p.SetPoint(this->GetRect()->GetRightBottom());
    if (p.Distance(point) < m_testRadius)
    {
        vertex.index = 2;
        vertex.p =  GetRect()->GetRightBottom();
        return vertex;
    }
    awp2DPoint lt = this->GetRect()->GetLeftTop();
    awp2DPoint rb = this->GetRect()->GetRightBottom();
    awp2DPoint _p;
    _p.X = lt.X;
    _p.Y = rb.Y;
	p.SetPoint(_p);
    if (p.Distance(point) < m_testRadius)
    {
        vertex.index = 3;
        vertex.p =  _p;
        return vertex;
    }
    _p.X = rb.X;
    _p.Y = lt.Y;
	p.SetPoint(_p);
    if (p.Distance(point) < m_testRadius)
    {
        vertex.index = 1;
        vertex.p =  _p;
        return vertex;
    }

    return vertex;
}

SZoneVertex TLFZone::SelectContourVertex(TLF2DPoint& point)
{
	SZoneVertex vertex;
    vertex.index = -1;
    vertex.type =  ZTContour;
    memset(&vertex.p, 0, sizeof(vertex.p));

    if (!this->IsContour())
    	return vertex;

	TLF2DPoint p;
    for (int i = 0; i < m_contour.GetNumPoints(); i++)
    {
		awp2DPoint _p = m_contour.GetPoint(i);
        p.SetPoint(_p);
        if (p.Distance(point) < m_testRadius)
        {
            vertex.index = i;
            vertex.p =  _p;
            return vertex;
        }
    }

    return vertex;
}
SZoneVertex TLFZone::SelectLineSegmentVertex(TLF2DPoint& point)
{
 	SZoneVertex vertex;
    vertex.index = -1;
    vertex.type =  ZTLineSegment;
    memset(&vertex.p, 0, sizeof(vertex.p));

    if (!this->IsLineSegment())
    	return vertex;

    TLF2DPoint p;
    awp2DPoint _p;
    _p = this->GetLineSegmnet()->GetStart();
    p.SetPoint(_p);
    if (p.Distance(point) < m_testRadius)
    {
        vertex.index = 0;
        vertex.p =  _p;
        return vertex;
    }
    _p = this->GetLineSegmnet()->GetFinish();
    p.SetPoint(_p);
    if (p.Distance(point) < m_testRadius)
    {
        vertex.index = 1;
        vertex.p =  _p;
        return vertex;
    }

    return vertex;
}
SZoneVertex TLFZone::SelectOpenPolygonVertex(TLF2DPoint& point)
{

 	SZoneVertex vertex;
    vertex.index = -1;
    vertex.type =  ZTOpenPolygon;
    memset(&vertex.p, 0, sizeof(vertex.p));

    if (!this->IsOpenPolygon())
    	return vertex;

	for (int i = 0; i < m_openPolygon.GetCount(); i++)
    {
        TLF2DLineSegment* ls = m_openPolygon.GetSegment(i);

        TLF2DPoint p;
        awp2DPoint _p;
        _p = ls->GetStart();
        p.SetPoint(_p);
        if (p.Distance(point) < m_testRadius)
        {
            vertex.index = 2*i;
            vertex.p =  _p;
            return vertex;
        }
        _p = ls->GetFinish();
        p.SetPoint(_p);
        if (p.Distance(point) < m_testRadius)
        {
            vertex.index = 2*i + 1;
            vertex.p =  _p;
            return vertex;
        }
    }

    return vertex;
}

/** Select vertex near the point. vertex.index < 0 means
vertex is not selected*/
SZoneVertex TLFZone::SelectVertex(TLF2DPoint& point)
{
   SZoneVertex vertex;
   vertex.index = -1;
   vertex.type  = ZTUnknown;
   vertex.p.X = 0;
   vertex.p.Y = 0;

 	switch(this->m_ZoneType)
    {
        case ZTRect:
				return SelectRectVertex(point);
        case ZTContour:
				return SelectContourVertex(point);
		case ZTLineSegment:
				return SelectLineSegmentVertex(point);
        case ZTOpenPolygon:
				return SelectOpenPolygonVertex(point);
		case ZTCircle:
			return SelectLineSegmentVertex(point);
    }

	return vertex;
}

void TLFZone::SetRectVertex(SZoneVertex& point)
{
	if (point.type != ZTRect || point.index < 0 || point.index > 3)
    	return;
    awp2DPoint p = point.p;
	awp2DPoint p1;
    switch (point.index)
    {
		case 0:
          p1 = m_Rect.GetRightBottom();
          m_Rect.SetVertexes(p.X, p1.X, p.Y, p1.Y);
        break;
		case 1:
          p1.X = m_Rect.GetLeftTop().X;
          p1.Y = m_Rect.GetRightBottom().Y;
          m_Rect.SetVertexes(p.X, p1.X, p.Y, p1.Y);
        break;
		case 2:
          p1 = m_Rect.GetLeftTop();
          m_Rect.SetVertexes(p.X, p1.X, p.Y, p1.Y);
        break;
		case 3:
          p1.X = m_Rect.GetRightBottom().X;
          p1.Y = m_Rect.GetLeftTop().Y;
          m_Rect.SetVertexes(p.X, p1.X, p.Y, p1.Y);
        break;
    }
}
void TLFZone::SetContourVertex(SZoneVertex& point)
{
	if (point.type != ZTContour || point.index < 0 || point.index >= m_contour.GetNumPoints())
    	return;
    m_contour.SetPoint(point.index, point.p);

}
void TLFZone::SetLineSegmentVertex(SZoneVertex& point)
{
 	if (point.type != ZTLineSegment || point.index < 0 || point.index > 1)
    	return;
    if (point.index == 0)
    {
     	m_segment.SetStart(point.p);
    }
    else
    {
       m_segment.SetFihish(point.p);
    }
}
void TLFZone::SetOpenPolygonVertex(SZoneVertex& point)
{
 	if (point.type != ZTOpenPolygon || point.index < 0 || point.index > 2*m_openPolygon.GetCount())
    	return;
    int segmnet_idx = point.index / 2;
    if ((2+point.index) % 2 == 0)
    {
		// even
        if (point.index == 0)
        {
        	// change only start point
            TLF2DLineSegment* ls = m_openPolygon.GetSegment(0);
            ls->SetStart(point.p);
        }
        else
        {
           // change
            TLF2DLineSegment* ls1 = m_openPolygon.GetSegment(segmnet_idx);
            TLF2DLineSegment* ls2 = m_openPolygon.GetSegment(segmnet_idx-1);
            ls1->SetStart(point.p);
            ls2->SetFihish(point.p);
        }
    }
    else
    {
    	// odd
        if (point.index == 2*m_openPolygon.GetCount() -1 )
        {
            TLF2DLineSegment* ls = m_openPolygon.GetSegment(m_openPolygon.GetCount() -1);
            ls->SetFihish(point.p);
        }
        else
        {
            TLF2DLineSegment* ls2 = m_openPolygon.GetSegment(segmnet_idx);
            TLF2DLineSegment* ls1 = m_openPolygon.GetSegment(segmnet_idx+1);
            ls1->SetStart(point.p);
            ls2->SetFihish(point.p);
        }
    }
}

/** sets new coordinates to specific vertex*/
void TLFZone::SetVertex(SZoneVertex& vertex)
{
 	switch(this->m_ZoneType)
    {
        case ZTRect:
                 SetRectVertex(vertex);
        break;
        case ZTContour:
                SetContourVertex(vertex);
        break;
		case ZTLineSegment:
				SetLineSegmentVertex(vertex);
		break;
		case ZTOpenPolygon:
				SetOpenPolygonVertex(vertex);
		break;
		case ZTCircle:
				SetLineSegmentVertex(vertex);
		break;
	}
}



bool TLFZone::LoadXML(TiXmlElement* parent)
{
	if (parent == NULL)
		return false;
	const char* name = parent->Value();
	if (strcmp(name, this->GetName()) != 0)
		return false;
	int itype;
    TEZoneTypes type;
	parent->QueryIntAttribute("type", &itype);
	type = (TEZoneTypes)itype;
	if (parent->QueryIntAttribute("id", &m_classID) == TIXML_NO_ATTRIBUTE)
    	m_classID = -1;
    if (type == ZTRect)
    {
         TLF2DRect* rect = new TLF2DRect();
         TiXmlElement* e = parent->FirstChildElement(rect->GetName());
         if (e != NULL)
         {
         	if (!rect->LoadXML(e))
         	{
            	delete rect;
                return false;
            }

            this->m_Rect = *rect;
         }
         delete rect;
    }
    else if (type == ZTContour)
    {
         TLF2DContour* contour = new TLF2DContour();
         TiXmlElement* e = parent->FirstChildElement(contour->GetName());
         if (e != NULL)
         {
         	if (!contour->LoadXML(e))
         	{
            	delete contour;
                return false;
            }

            this->m_contour = *contour;
         }
         delete contour;
    }
    else if (type == ZTLineSegment || type == ZTCircle)
    {
         TLF2DLineSegment* segment = new TLF2DLineSegment();
         TiXmlElement* e = parent->FirstChildElement(segment->GetName());
         if (e != NULL)
         {
         	if (!segment->LoadXML(e))
         	{
            	delete segment;
                return false;
            }

            this->m_segment = *segment;
         }
         delete segment;
    }
    else if (type == ZTOpenPolygon)
    {
         TLF2DOpenPolygon* polygon = new TLF2DOpenPolygon();
         TiXmlElement* e = parent->FirstChildElement(polygon->GetName());
         if (e != NULL)
         {
         	if (!polygon->LoadXML(e))
         	{
            	delete polygon;
                return false;
            }

            this->m_openPolygon = *polygon;
         }
         delete polygon;
    }
    else
    	return false;

    this->m_ZoneType = type;
	return true;
}
bool TLFZone::SaveXML(TiXmlElement* parent)
{
 	if (parent == NULL)
		return false;
	TiXmlElement* f = new TiXmlElement(this->GetName());
	f->SetAttribute("type", (int)this->m_ZoneType);
	f->SetAttribute("id", this->m_classID);
	if (this->IsRect())
    {
		this->m_Rect.SaveXML(f);
    }
	else if (this->IsContour())
    {
    	this->m_contour.SaveXML(f);
    }
	else if (this->IsLineSegment() || this->IsCircle())
    {
		this->m_segment.SaveXML(f);
    }
    else if (this->IsOpenPolygon())
    {
         this->m_openPolygon.SaveXML(f);
    }
	else
    	return false;
    parent->LinkEndChild(f);
    return true;
}



TLFZones::TLFZones()
{

}

TLFZones::TLFZones(TLFZones& zones)
{
	for (int i = 0; i < zones.GetCount(); i++)
	{
		TLFZone* z = zones.GetZone(i);
		if (z != NULL)
		{
			TLFZone* zz = new TLFZone(*z);
			Add(zz);
		}
	}
}

TLFZones& TLFZones::operator=(TLFZones& zones)
{
	if (this != &zones)
    {
		this->Clear();
        for (int i = 0; i < zones.GetCount(); i++)
        {
			TLFZone* z = new TLFZone(*zones.GetZone(i));
            Add(z);
        }
    }
	return *this;
}


void TLFZones::AddZone(TLFZone* zone)
{
	if (zone != NULL)
		Add(zone);
}

TLFZone* TLFZones::GetZone(int index)
{
	if (index < 0 || index >= this->GetCount())
		return NULL;
	return dynamic_cast<TLFZone*>(Get(index));
}

bool TLFZones::IsPointInContour(int x, int y, TLF2DContour* list, double dx, double dy)
{
	if (list == NULL)
		return false;
	if (list->GetNumPoints() == 0)
		return false;
	awp2DPoint point0;
	awp2DPoint point1;
	int j;
	double x1, k,b,t;
	awp2DContour* c = list->GetContour();
	int count = 0;
	for (AWPDWORD i = 0; i < c->NumPoints; i++)
	{
		j = (i + 1) % c->NumPoints;
		point0 = c->Points[i];
		point1 = c->Points[j];

        point0.X *= dx;
        point0.Y *= dy;
        point1.X *= dx;
        point1.Y *= dy;

		if ((int)point0.Y == (int)point1.Y)
			continue;
		else if (point0.Y > y && point1.Y > y)
			continue;
		else if (point0.Y < y && point1.Y < y)
			continue;
		else if ((int)AWP_MIN(point0.X, point1.X) > x)
			continue;
		else if ((int)AWP_MIN(point0.Y, point1.Y) == (int)y)
			continue;
		else if ((int)AWP_MAX(point0.Y, point1.Y) == (int)y)
		{
			x1 = point0.Y > point1.Y ? point0.X : point1.X;
			if (x > x1)
				count++;
		}
        else if (point0.X == point1.X)
        {
        	count++;
        }
		else
		{
			k, b;
			k = (point1.Y - point0.Y) / (point1.X - point0.X);
			b = point1.Y - k*point1.X;
			t;
			t = (y - b) / k;
			if (t < x)
				count++;
		}
	}

	return count & 1;
}

int TLFZones::IsPointInZone(int x, int y, double dx, double dy)
{
	for (int i = 0; i < this->GetCount(); i++)
	{
		TLFZone* zone = GetZone(i);
		if (zone == NULL)
			continue;

		if (zone->IsRect())
		{
			TLF2DRect* rect = zone->GetRect();
			if (x > dx*rect->GetLeftTop().X && x < dx*rect->GetRightBottom().X &&
				y > dy*rect->GetLeftTop().Y && y < dy*rect->GetRightBottom().Y)
				return zone->GetClassID();
		}
		if (zone->IsCircle())
		{
			TLF2DLineSegment* s = zone->GetLineSegmnet();
			awp2DPoint sp = s->GetStart();
			awp2DPoint ep = s->GetFinish();
			sp.X *= dx;
			sp.Y *= dy;
			ep.X *= dx;
			ep.Y *= dy;
			double r1 = sqrt((sp.X - ep.X)*(sp.X - ep.X) + (sp.Y - ep.Y)*(sp.Y - ep.Y));
			double r2 = sqrt((sp.X - x)*(sp.X - x) + (sp.Y - y)*(sp.Y-y));
			if (r2 < r1) {
                return zone->GetClassID();
			}
		}
		if (IsPointInContour(x, y, zone->GetContour(), dx, dy))
				return zone->GetClassID();
	}
	return -1;
}


awpImage* TLFZones::GetMaskImage(awpImage* source, bool use_class_info)
{
	if (source == NULL)
		return NULL;

	if (awpCheckImage(source) != AWP_OK)
		return NULL;

	awpImage* result = NULL;
	if (awpCreateImage(&result, source->sSizeX, source->sSizeY, 1, AWP_BYTE) != AWP_OK)
		return NULL;
	awpImage* img = result;

    //
    double dx = (double)source->sSizeX / 100.;
    double dy = (double)source->sSizeY / 100.;

	for (int y = 0; y < img->sSizeY; y++)
	{
		for (int x = 0; x < img->sSizeX; x++)
		{
            int v = IsPointInZone(x, y, dx, dy);
			if (v  < 0)
			{
				AWPBYTE* pixels = (AWPBYTE*)img->pPixels;
				pixels += (y*img->sSizeX + x)*img->bChannels;
				for (int i = 0; i < img->bChannels; i++)
				{
					pixels[i] = 0;
				}
			}
			else
			{
				AWPBYTE* pixels = (AWPBYTE*)img->pPixels;
				pixels += (y*img->sSizeX + x)*img->bChannels;
				for (int i = 0; i < img->bChannels; i++)
				{
					pixels[i] = use_class_info? v: 1;
				}
			}

		}
	}


	return result;
}

awpImage* TLFZones::GetMaskImage(awpImage* source, ILFScanner* scanner)
{
    if (source == NULL || scanner == NULL)
		return NULL;

	if (awpCheckImage(source) != AWP_OK)
		return NULL;

	awpImage* result = NULL;
	if (awpCreateImage(&result, source->sSizeX, source->sSizeY, 1, AWP_BYTE) != AWP_OK)
		return NULL;

    double dx = (double)source->sSizeX / 100.;
    double dy = (double)source->sSizeY / 100.;

   scanner->Scan(source->sSizeX, source->sSizeY);

    for (int i = 0; i < scanner->GetFragmentsCount(); i++)
    {
        awpRect r = scanner->GetFragmentRect(i);
        //todo: add options: center all points, any point
        awpPoint p;
        p.X = (r.left + r.right) / 2;
        p.Y = (r.top + r.bottom) / 2;
        if (this->IsPointInZone(p.X, p.Y, dx, dy)>=0)
        {
           awpFillRect(result, &r, 0, 1);
        }
    }

	return result;
}


bool TLFZones::TestPoint(int x, int y, int idx)
{
	if (idx < 0 || idx >= this->GetCount())
		return false;
	TLFZone* zone = GetZone(idx);
	if (zone == NULL)
		return false;

	if (zone->IsRect())
	{
		TLF2DRect* rect = zone->GetRect();
		if (x > rect->GetLeftTop().X && x < rect->GetRightBottom().X &&
			y > rect->GetLeftTop().Y && y < rect->GetRightBottom().Y)
			return true;
	}
	else
		if (IsPointInContour(x, y, zone->GetContour()))
			return true;
	return false;
}
int TLFZones::TestRect(awpRect r, int w, int h)
{
    double dx = (double)w / 100.;
    double dy = (double)h / 100.;
	int ZoneIdx = IsPointInZone(r.left,r.top,dx,dy);
    if (ZoneIdx >= 0)
    {
        if (IsPointInZone(r.right,r.top,dx,dy) == ZoneIdx &&
        IsPointInZone(r.right,r.bottom,dx,dy) == ZoneIdx && IsPointInZone(r.left,r.bottom,dx,dy) == ZoneIdx)
            return ZoneIdx;
    }

    return -1;
}


bool TLFZones::LoadXML(TiXmlElement* parent)
{
    if (parent == NULL)
        return false;
	const char* name = parent->Value();
	if (strcmp(name, this->GetName()) != 0)
		return false;
    this->Clear();
	for (TiXmlNode* child = parent->FirstChild(); child; child = child->NextSibling())
	{
		TLFZone* zone = new TLFZone(ZTUnknown);
		if (zone == NULL)
			return false;
		if (!zone->LoadXML(child->ToElement()))
		{
            this->Clear();
			delete zone;
			return false;
		}
		Add(zone);
	}
	return true;
}

bool TLFZones::SaveXML(TiXmlElement* parent)
{
	if (parent == NULL)
		return false;

	int c = GetCount();
	TLFZone** zones = (TLFZone**)GetList();
	for (int i = 0; i < c; i++)
		zones[i]->SaveXML(parent);

	return true;
}

bool TLFZones::Load(const char* lpFileName)
{
	TiXmlDocument doc(lpFileName);
	if (!doc.LoadFile())
		return false;
	TiXmlHandle hDoc(&doc);
	TiXmlElement* pElem = NULL;
	TiXmlHandle hRoot(0);
	pElem = hDoc.FirstChildElement().Element();


	return this->LoadXML(pElem);
}

bool TLFZones::Save(const char* lpFileName)
{
	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
	doc.LinkEndChild(decl);
	TiXmlElement* zones_element = new TiXmlElement(this->GetName());
	if (zones_element == NULL)
		return false;
	if (!this->SaveXML(zones_element))
	{
		delete zones_element;
		return false;
	}
	doc.LinkEndChild(zones_element);
	return doc.SaveFile(lpFileName);
}
