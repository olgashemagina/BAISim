#include "_LF.h"


TLFPoint::TLFPoint()
{
	memset(&m_Point, 0, sizeof(m_Point));
}

TLFPoint::TLFPoint(awpPoint p)
{
	this->m_Point = p;
}
TLFPoint::TLFPoint(TLFPoint& p)
{
	this->m_Point = p.GetPoint();
}

TLFPoint::~TLFPoint()
{

}
awpPoint TLFPoint::GetPoint()
{
	return m_Point;
}

short TLFPoint::GetX()
{
	return m_Point.X;
}

short TLFPoint::GetY()
{
	return m_Point.Y;
}

void TLFPoint::SetPoint(awpPoint Point)
{
	m_Point = Point;
}

double TLFPoint::Distance(TLFPoint& p)
{
	return sqrt(((double)p.m_Point.X - (double)m_Point.X)*((double)p.m_Point.X - (double)m_Point.X) + ((double)p.m_Point.Y - (double)m_Point.Y)*((double)p.m_Point.Y - (double)m_Point.Y));
}

bool TLFPoint::LoadXML(TiXmlElement* parent)
{
	if (parent == NULL)
		return false;
	const char* name = parent->Value();
	if (strcmp(name, this->GetName()) != 0)
		return false;
	int x, y;
	
	parent->QueryIntAttribute("x", &x);
	parent->QueryIntAttribute("y", &y);

	if (x < -32767 || x > 32767 || y < -32767 || y > 32767)
		return false;

	this->m_Point.X = (AWPSHORT)x;
	this->m_Point.Y = (AWPSHORT)y;
	return true;
}

bool TLFPoint::SaveXML(TiXmlElement* parent)
{
	if (parent == NULL)
		return false;
	TiXmlElement* f = new TiXmlElement(this->GetName());
	f->SetAttribute("x", this->m_Point.X);
	f->SetAttribute("y", this->m_Point.Y);
	parent->LinkEndChild(f);
	return true;
}

//--------------------------
TLF2DPoint::TLF2DPoint()
{
	memset(&m_point, 0, sizeof(m_point));
}

TLF2DPoint::TLF2DPoint(awpPoint p)
{
	m_point.X = (double)p.X;
	m_point.Y = (double)p.Y;
}
TLF2DPoint::TLF2DPoint(awp2DPoint p)
{
	m_point = p;
}
TLF2DPoint::TLF2DPoint(TLFPoint& p)
{
	awpPoint pp = p.GetPoint();
	m_point.X = (double)pp.X;
	m_point.Y = (double)pp.Y;
}
TLF2DPoint::TLF2DPoint(TLF2DPoint& p)
{
	m_point = p.GetPoint();
}
TLF2DPoint::TLF2DPoint(double x, double y)

{
	m_point.X = x;
    m_point.Y = y;
}


TLF2DPoint::~TLF2DPoint()
{}
awp2DPoint TLF2DPoint::GetPoint()
{
	return m_point;
}

double TLF2DPoint::Distance(TLF2DPoint& p)
{
	return sqrt(((double)p.m_point.X - (double)m_point.X)*((double)p.m_point.X - (double)m_point.X) + ((double)p.m_point.Y - (double)m_point.Y)*((double)p.m_point.Y - (double)m_point.Y));
}

void TLF2DPoint::SetPoint(awp2DPoint Point)
{
	m_point.X = Point.X;
	m_point.Y = Point.Y;
}
double TLF2DPoint::GetX()
{
	return m_point.X;
}
double TLF2DPoint::GetY()
{
	return m_point.Y;
}

bool TLF2DPoint::LoadXML(TiXmlElement* parent)
{
	if (parent == NULL)
		return false;
	const char* name = parent->Value();
	if (strcmp(name, this->GetName()) != 0)
		return false;

	parent->QueryDoubleAttribute("x", &this->m_point.X);
	parent->QueryDoubleAttribute("y", &this->m_point.Y);

	return true;
}
bool TLF2DPoint::SaveXML(TiXmlElement* parent)
{
	if (parent == NULL)
		return false;
	TiXmlElement* f = new TiXmlElement(this->GetName());
	f->SetDoubleAttribute("x", this->m_point.X);
	f->SetDoubleAttribute("y", this->m_point.Y);
	parent->LinkEndChild(f);
	return true;
}

//--------------------------
TLFRect::TLFRect()
{
	memset(&m_Rect, 0, sizeof(m_Rect));
}
/*TLFRect::TLFRect(awpRect& r)
{
	m_Rect.left = r.left;
	m_Rect.top  = r.top;
	m_Rect.right = r.right;
	m_Rect.bottom = r.bottom;
}*/

TLFRect::TLFRect(awpRect r)
{
    m_Rect = r;
}


TLFRect::TLFRect(int left, int top, int w, int h)
{
	m_Rect.left = left;
	m_Rect.top  = top;
	m_Rect.right = left + w;
	m_Rect.bottom = top + h;
}

TLFRect::TLFRect(TLFRect& r)
{
	m_Rect = r.GetRect();
}
TLFRect::~TLFRect()
{}

awpRect TLFRect::GetRect()
{
 	return m_Rect;
}
void TLFRect::SetRect(awpRect Rect)
{
	m_Rect = Rect;
}

void TLFRect::SetRect(int left, int top, int w, int h)
{
    m_Rect.left = left;
    m_Rect.top = top;
    m_Rect.right = left + w;
    m_Rect.bottom = top + h;
}


bool TLFRect::IsEmpty()
{
	return m_Rect.left == m_Rect.right || m_Rect.top == m_Rect.bottom;
}

void TLFRect::Clear()
{
	memset(&m_Rect, 0, sizeof(m_Rect));
}

double TLFRect::RectOverlap(TLFRect& rect)
{
  awpRect r = rect.GetRect();
  return RectOverlap(r);
}

double TLFRect::RectOverlap(awpRect& rect)
{

 if (IsEmpty())
    return 0;
 int width = rect.right - rect.left;
 int height = rect.bottom - rect.top;

 if (m_Rect.left > rect.left + width)
    return 0;
 if (m_Rect.top > rect.top + height)
    return 0;
 if (m_Rect.left + Width() < rect.left)
    return 0;
 if (m_Rect.top + Height() < rect.top)
    return 0;

 double colInt = double(AWP_MIN(m_Rect.left + Width(), rect.left + width) - AWP_MAX(m_Rect.left, rect.left));
 double rowInt = double(AWP_MIN(m_Rect.top + Height(), rect.top + height) - AWP_MAX(m_Rect.top, rect.top));

 double intersection = colInt*rowInt;
 double area1 = 1.*Width()*Height();
 double area2 = 1.*width*height;

 return intersection / (area1+area2-intersection);
}

void TLFRect::Unioun(awpRect rect)
{
	if (m_Rect.left > rect.left)
		m_Rect.left = rect.left;
	if (m_Rect.top > rect.top)
		m_Rect.top = rect.top;
	if (m_Rect.right < rect.right)
		m_Rect.right = rect.right;
	if (m_Rect.bottom < rect.bottom)
		m_Rect.bottom = rect.bottom;
}

void TLFRect::Inflate(int x_value, int y_value)
{
	int l,r,t,b;
	l = (int)m_Rect.left - x_value;
	if (l < 0)
		l = 0;
	if (l > AWP_MAX_SHORT)
		return;
	r = (int)m_Rect.right + x_value;
	if (r < 0 || r > AWP_MAX_SHORT)
		return;
	t = (int)m_Rect.top - y_value;
	if (t < 0)
		t = 0;
	if(t > AWP_MAX_SHORT)
		return;
	b = (int)m_Rect.bottom + y_value;
	if (b < 0 || b > AWP_MAX_SHORT)
		return;
	m_Rect.left = l;
	m_Rect.top  = t;
	m_Rect.bottom = b;
	m_Rect.right = r;
}


AWPSHORT TLFRect::Width()
{
    return m_Rect.right - m_Rect.left;
}

AWPSHORT TLFRect::Height()
{
    return m_Rect.bottom - m_Rect.top;
}

AWPSHORT TLFRect::Right()
{
	return m_Rect.right;
}
AWPSHORT TLFRect::Bottom()
{
	return m_Rect.bottom;
}

awpPoint TLFRect::Center()
{
	awpPoint p;
	p.X = (m_Rect.left + m_Rect.right) / 2;
	p.Y = (m_Rect.top + m_Rect.bottom) / 2;
	return p;
}

bool  TLFRect::IsCover(TLFRect& rect)
{
    awpRect r = rect.GetRect();
    return IsCover(r);
}
bool  TLFRect::IsCover(awpRect& rect)
{
   if (m_Rect.left <= rect.left && m_Rect.right >= rect.right &&
   m_Rect.top <= rect.top && m_Rect.bottom >= rect.bottom)
    return true;
   return false;
}
bool  TLFRect::Touch(TLFRect& rect)
{
	awpRect r = rect.GetRect();
	if ((fabs((double)m_Rect.left - (double)r.right) <= 1 || fabs((double)m_Rect.left - (double)r.left) <= 1) && (r.bottom <= m_Rect.top && r.bottom >= m_Rect.bottom))
		return true;

	if ((fabs((double)m_Rect.left - r.right) <= 1 || fabs((double)m_Rect.left - r.left) <= 1) && (r.top <= m_Rect.top && r.top >= m_Rect.bottom))
		return true;

	if ((fabs((double)m_Rect.right - r.left) <= 1 || fabs((double)m_Rect.right - r.right) <= 1) && (r.bottom >= m_Rect.top && r.bottom <= m_Rect.bottom))
		return true;

	if ((fabs((double)m_Rect.right - r.left) <= 1 || fabs((double)m_Rect.right - r.right) <= 1) && (r.top >= m_Rect.top && r.top <= m_Rect.bottom))
		return true;

	if (((m_Rect.top - r.bottom) <= 1 || fabs((double)m_Rect.top - r.top) <= 1) && (r.right >= m_Rect.left && r.right <= m_Rect.right))
		return true;

	if ((fabs((double)m_Rect.top - r.bottom) <= 1 || fabs((double)m_Rect.top - r.top) <= 1) && (r.left >= m_Rect.left && r.left <= m_Rect.right))
		return true;

	if ((fabs((double)m_Rect.bottom - r.top) <= 1 || fabs((double)m_Rect.bottom - r.bottom) <= 1) && (r.right >= m_Rect.left && r.right <= m_Rect.right))
		return true;

	if ((fabs((double)m_Rect.bottom - r.top) <= 1 || fabs((double)m_Rect.bottom - r.bottom) <= 1) && (r.left >= m_Rect.left && r.left <= m_Rect.right))
		return true;

	return false;

}
static double L2Dist(awpPoint p1, awpPoint p2)
{
	return sqrt(((double)p1.X - (double)p2.X)*((double)p1.X - (double)p2.X)+((double)p1.Y - (double)p2.Y)*((double)p1.Y - (double)p2.Y));
}
double TLFRect::Distance(TLFRect& rect)
{
	awpRect r = rect.GetRect();
	if ((r.left >= m_Rect.left && r.left <= m_Rect.right) || (r.right >= m_Rect.left && r.right <= m_Rect.right))
	{
		double d1 = fabs((double)r.bottom - (double)m_Rect.top);
		double d2 = fabs((double)r.top - (double)m_Rect.bottom);
		return AWP_MIN(d1,d2);
	}

	if ((r.top >= m_Rect.top && r.top <= m_Rect.bottom) || (r.bottom >= m_Rect.top && r.bottom <= m_Rect.bottom))
	{
		double d1 = fabs((double)r.right - (double)m_Rect.left);
		double d2 = fabs((double)r.left - (double)m_Rect.right);
		return AWP_MIN(d1, d2);
	}
	awpPoint p[4];
	awpPoint m_p[4];

	p[0].X = r.left;
	p[0].Y = r.top;
	p[1].X = r.right;
	p[1].Y = r.top;
	p[2].X = r.right;
	p[2].Y = r.bottom;
	p[3].X = r.left;
	p[3].Y = r.bottom;

	m_p[0].X = m_Rect.left;
	m_p[0].Y = m_Rect.top;
	m_p[1].X = m_Rect.right;
	m_p[1].Y = m_Rect.top;
	m_p[2].X = m_Rect.right;
	m_p[2].Y = m_Rect.bottom;
	m_p[3].X = m_Rect.left;
	m_p[3].Y = m_Rect.bottom;

	double min = FLT_MAX;
	for (int i = 0; i < 4; i++)
	{
		awpPoint _p0 = p[i];
		for (int j = 0; j < 4; j++)
		{
			awpPoint _p1 = m_p[j];
			double d = L2Dist(_p0, _p1);
			if (min > d)
				min = d;
		}
	}
	return min;
}


double TLFRect::CoverSquare(TLFRect& rect)
{
    awpRect r = rect.GetRect();
    return  CoverSquare(r);
}
double TLFRect::CoverSquare(awpRect& rect)
{

    int w = rect.right - rect.left;
    int h = rect.bottom - rect.top;

    double colInt = double(AWP_MIN(m_Rect.left + Width(), rect.left + w) - AWP_MAX(m_Rect.left, rect.left));
    double rowInt = double(AWP_MIN(m_Rect.top + Height(), rect.top + h) - AWP_MAX(m_Rect.top, rect.top));
	double s1 = colInt*rowInt;
	double res = s1 / Square();
    return res;
}

double TLFRect::Square()
{
	return (double)Width()*Height();
}

void TLFRect::Shift(int dx, int dy)
{
//todo: check bounds
	this->m_Rect.left += dx;
	this->m_Rect.right += dx;
	this->m_Rect.top += dy;
	this->m_Rect.bottom += dy;
}

void TLFRect::Scale(double coef)
{
//todo: check bounds
	if (coef <= 0)
		return;
	m_Rect.left = (int)(m_Rect.left*coef+0.5);
	m_Rect.top = (int)(m_Rect.top*coef+0.5);
	m_Rect.right = (int)(m_Rect.right*coef+0.5);
	m_Rect.bottom = (int)(m_Rect.bottom*coef+0.5);
}

AWPSHORT  TLFRect::Left()
{
	return m_Rect.left;
}
AWPSHORT  TLFRect::Top()
{
	return m_Rect.top;
}

/** Load form XML*/
bool TLFRect::LoadXML(TiXmlElement* parent)
{
	if (parent == NULL)
		return false;
	const char* name = parent->Value();
	if (strcmp(name, this->GetName()) != 0)
		return false;
	int left, top, right, bottom;

	parent->QueryIntAttribute("left", &left);
	parent->QueryIntAttribute("top", &top);
	parent->QueryIntAttribute("right", &right);
	parent->QueryIntAttribute("bottom", &bottom);

	if (left < -32767 || left > 32767 || right < -32767 || right > 32767)
		return false;

	if (top < -32767 || top > 32767 || top < -32767 || top > 32767)
		return false;

	m_Rect.left = left;
	m_Rect.top = top;
	m_Rect.right = right;
	m_Rect.bottom = bottom;

	return true;
}
/** Save to XML*/
bool TLFRect::SaveXML(TiXmlElement* parent)
{
	if (parent == NULL)
		return false;
	TiXmlElement* f = new TiXmlElement(this->GetName());
	f->SetAttribute("left", m_Rect.left);
	f->SetAttribute("top", m_Rect.top);
	f->SetAttribute("right", m_Rect.right);
	f->SetAttribute("bottom", m_Rect.bottom);
	parent->LinkEndChild(f);
	return true;
}



TLF2DRect::TLF2DRect()
{
	memset(&this->m_LeftTop, 0, sizeof(m_LeftTop));
	memset(&this->m_RightBottom, 0, sizeof(m_RightBottom));
}
TLF2DRect::TLF2DRect(awp2DPoint left_top, awp2DPoint right_bottom)
{
	this->m_LeftTop = left_top;
	this->m_RightBottom = right_bottom;
}
TLF2DRect::TLF2DRect(TLF2DRect& rect)
{
	this->m_LeftTop = rect.m_LeftTop;
	this->m_RightBottom = rect.m_RightBottom;
}

TLF2DRect::TLF2DRect(awpRect r)
{
	this->m_LeftTop.X = (double)r.left;
	this->m_LeftTop.Y = (double)r.top;
	this->m_RightBottom.X = (double)r.right;
    this->m_RightBottom.Y = (double)r.bottom;
}

void TLF2DRect::SetRect(TLF2DRect& rect)
{
	this->m_LeftTop = rect.m_LeftTop;
	this->m_RightBottom = rect.m_RightBottom;
}


awp2DPoint TLF2DRect::GetLeftTop()
{
	return this->m_LeftTop;
}
awp2DPoint TLF2DRect::GetRightBottom()
{
	return this->m_RightBottom;
}

void TLF2DRect::SetVertexes(double x1, double x2, double y1, double y2)
{
	this->m_LeftTop.X = AWP_MIN(x1,x2);
    this->m_RightBottom.X = AWP_MAX(x1, x2);
    this->m_LeftTop.Y = AWP_MIN(y1, y2);
    this->m_RightBottom.Y = AWP_MAX(y1, y2);
}



double TLF2DRect::Width()
{
	return m_RightBottom.X - m_LeftTop.X;
}
double TLF2DRect::Height()
{
	return this->m_RightBottom.Y - this->m_LeftTop.Y;
}
awp2DPoint TLF2DRect::Center()
{
	awp2DPoint result;
	result.X =0.5*(m_LeftTop.X + m_RightBottom.X);
	result.Y = 0.5*(m_LeftTop.Y + m_RightBottom.Y);
	return result;
}

double TLF2DRect::left()
{
	return m_LeftTop.X;
}
double TLF2DRect::right()
{
	return m_RightBottom.X;
}
double TLF2DRect::top()
{
	return m_LeftTop.Y;
}
double TLF2DRect::bottom()
{
	return m_RightBottom.Y;
}


/** Load form XML*/
bool TLF2DRect::LoadXML(TiXmlElement* parent)
{
	if (parent == NULL)
		return false;
	const char* name = parent->Value();
	if (strcmp(name, this->GetName()) != 0)
		return false;

	parent->QueryDoubleAttribute("left", &this->m_LeftTop.X);
	parent->QueryDoubleAttribute("top", &this->m_LeftTop.Y);
	parent->QueryDoubleAttribute("right", &this->m_RightBottom.X);
	parent->QueryDoubleAttribute("bottom", &this->m_RightBottom.Y);

	return true;
}
/** Save to XML*/
bool TLF2DRect::SaveXML(TiXmlElement* parent)
{
	if (parent == NULL)
		return false;
	TiXmlElement* f = new TiXmlElement(this->GetName());
	f->SetDoubleAttribute("left", this->m_LeftTop.X);
	f->SetDoubleAttribute("top", this->m_LeftTop.Y);
	f->SetDoubleAttribute("right", this->m_RightBottom.X);
	f->SetDoubleAttribute("bottom", this->m_RightBottom.Y);
	parent->LinkEndChild(f);
	return true;
}

TLFTriangle::TLFTriangle(awp2DPoint v1, awp2DPoint v2, awp2DPoint v3, TLFTriangle* parent, int depth, int index)
{
	this->m_v1 = v1;
	this->m_v2 = v2;
	this->m_v3 = v3;
	this->m_parent = parent;
	this->m_index = index;
	this->m_hasPoint = false;
    this->m_depth = depth;
    m_v = 0;
    m_h = 0;
	memset(m_child, 0, 4 * sizeof(TLFTriangle*));
    //todo: _square threshold
//	if (this->_square() > 10 && depth != 0)
	{
		for (int i = 0; i < 4; i++)
		{
			m_child[i] = CreateTriangle(i, depth -1, index);
		}
	}
}
TLFTriangle::TLFTriangle(TLFTriangle* t)
{
    this->m_v1 = t->m_v1;
    this->m_v2 = t->m_v2;
    this->m_v3 = t->m_v3;
    this->m_parent = NULL;
    this->m_index = t->m_index;
    this->m_hasPoint = t->m_hasPoint;
    this->m_depth = 0;
    this->m_v = t->m_v;
    this->m_h = t->m_h;
    memset(m_child, 0, 4 * sizeof(TLFTriangle*));
}



TLFTriangle::~TLFTriangle()
{
	for (int i = 0; i < 4; i++)
	{
		if (m_child[i] != NULL)
		{
			delete m_child[i];
			m_child[i] = NULL;
		}
	}
}

TLFTriangle* TLFTriangle::CreateTriangle(int index, int depth, int gi)
{

	if (depth < 0)
		return NULL;

	awp2DPoint v1;
	awp2DPoint v2;
	awp2DPoint v3;

	switch (index)
	{
	case 0:
		v1.X = m_v1.X;
		v1.Y = m_v1.Y;
		v2.X = (m_v1.X + m_v2.X)/2;
		v2.Y = (m_v1.Y + m_v2.Y) / 2;
		v3.X = (m_v1.X + m_v3.X) / 2;
		v3.Y = (m_v1.Y + m_v3.Y) / 2;
		break;
	case 1:
		v1.X = (m_v1.X + m_v2.X) / 2;
		v1.Y = (m_v1.Y + m_v2.Y) / 2;
		v2.X = m_v2.X;
		v2.Y = m_v2.Y;
		v3.X = (m_v2.X + m_v3.X) / 2;
		v3.Y = (m_v2.Y + m_v3.Y) / 2;
		break;
	case 2:
		v1.X = (m_v1.X + m_v3.X) / 2;
		v1.Y = (m_v1.Y + m_v3.Y) / 2;
		v2.X = (m_v2.X + m_v3.X) / 2;
		v2.Y = (m_v2.Y + m_v3.Y) / 2;
		v3.X = m_v3.X;
		v3.Y = m_v3.Y;
		break;
	case 3:
		v1.X = (m_v1.X + m_v2.X) / 2;
		v1.Y = (m_v1.Y + m_v2.Y) / 2;
		v2.X = (m_v2.X + m_v3.X) / 2;
		v2.Y = (m_v2.Y + m_v3.Y) / 2;
		v3.X = (m_v1.X + m_v3.X) / 2;
		v3.Y = (m_v1.Y + m_v3.Y) / 2;
		break;
	default:
		return NULL;
	}

	return new TLFTriangle(v1,v2,v3, this, depth, gi);
}


TLFTriangle* TLFTriangle::GetParent()
{
	return this->m_parent;
}
TLFTriangle* TLFTriangle::GetChild(int index)
{
	return m_child[index];
}

awp2DPoint TLFTriangle::_cm()
{
    if (this->m_hasPoint)
        return this->m_cm;

	awp2DPoint p[3];
	p[0] = m_v1;
	p[1] = m_v2;
	p[2] = m_v3;
	return awpGetPointsCentroid(p,3);
}
awp2DPoint TLFTriangle::_v1()
{
	return this->m_v1;
}
awp2DPoint TLFTriangle::_v2()
{
	return this->m_v2;
}
awp2DPoint TLFTriangle::_v3()
{
	return this->m_v3;
}
bool	   TLFTriangle::_hasPoint()
{
	return this->m_hasPoint;
}
int  TLFTriangle::_count()
{
	return (int)pow((double)4, m_depth);
}

double     TLFTriangle::_v()
{
    return this->m_v;
}
double     TLFTriangle::_h()
{
    return this->m_h;
}


bool IsPointInContour(int x, int y, awpContour* contour)
{
	int count = 0;
	for (AWPDWORD i = 0; i < contour->NumPoints; i++)
	{
		int j = (i+1)%contour->NumPoints;
		//?????????????? ???????.
		if ((int)contour->Points[i].Y == (int)contour->Points[j].Y)
			continue;
		//??????? ???? ?? ????
		else if (contour->Points[i].Y > y && contour->Points[j].Y > y)
			continue;
		//??????? ???? ?? ????
		else if (contour->Points[i].Y < y && contour->Points[j].Y < y)
			continue;
		// ??????? ?????? ?? ????
		else if ((int)AWP_MIN(contour->Points[i].X,contour->Points[j].X) > x)
			continue;
		//?????? ??????? ?? ????
		else if ((int)AWP_MIN(contour->Points[i].Y,contour->Points[j].Y) == (int)y)
			continue;
		//??????? ??????? ??????? ?? ????
		else if ((int)AWP_MAX(contour->Points[i].Y,contour->Points[j].Y) == (int)y)
		{
			double x1 = contour->Points[i].Y > contour->Points[j].Y ? contour->Points[i].X : contour->Points[j].X;
			if (x > x1)
				count++;
		}
		else
		{
			double k,b;
			k = (double)(contour->Points[j].Y - contour->Points[i].Y) / (double)(contour->Points[j].X - contour->Points[i].X +.01);
			b = contour->Points[j].Y - k*contour->Points[j].X;
			// ????? ???????????.
			double t;
			t = (y - b) / k;
			if (t < x )
				count++;
		}
	}
	return count & 1;
}

void TLFTriangle::Process(awpImage* img, awpImage* vg, awpImage* hg)
{
    //
    if (img == NULL || img->bChannels > 1 || img->dwType != AWP_BYTE)
        return;
    //
    double max_y, min_y, max_x, min_x;
    int    maxy_idx, miny_idx,maxx_idx,minx_idx;
    awp2DPoint p[4];
    awpPoint   pp[4];

    p[0] = this->m_v1;
    p[1] = this->m_v2;
    p[2] = this->m_v3;
    p[3] = this->m_v1;
    max_y = p[0].Y;
    min_y = p[0].Y;
    max_x = p[0].X;
    min_x = p[0].X;
    maxy_idx = 0;
    miny_idx = 0;
    maxx_idx = 0;
    minx_idx = 0;
    for (int i = 1; i < 3; i++)
    {
       if (p[i].Y > max_y)
       {
          max_y = p[i].Y;
          maxy_idx = i;
       }
       if (p[i].Y < min_y)
       {
          min_y = p[i].Y;
          miny_idx = i;
       }
       if (p[i].X > max_x)
       {
          max_x = p[i].X;
          maxx_idx = i;
       }
       if (p[i].X < min_x)
       {
          min_x = p[i].X;
          minx_idx = i;
       }
    }
    //
    if (min_y < 0 || min_y >= img->sSizeY || max_y >= img->sSizeY)
        return;
    if (min_x < 0 || min_x >= img->sSizeX || max_x >= img->sSizeX)
        return;

    for (int i = 0; i < 4; i++)
    {
        pp[i].X = (int)floor(p[i].X + 0.5);
        pp[i].Y = (int)floor(p[i].Y + 0.5);
    }

     awpContour contour;
     contour.NumPoints = 4;
     contour.Points = pp;

     awpRect r;
     r.top = (AWPSHORT)floor(min_y + 0.5);
	 r.bottom = (AWPSHORT)floor(max_y);
	 r.left = (AWPSHORT)floor(min_x);
	 r.right = (AWPSHORT)floor(max_x);
     double sum = 0;
     double sumv = 0;
     double sumh = 0;
     double count = 0;
     double x0 = 0;
     double y0 = 0;

     AWPBYTE* b = (AWPBYTE*)img->pPixels;
     AWPDOUBLE* h = (AWPDOUBLE*)vg->pPixels;
     AWPDOUBLE* v = (AWPDOUBLE*)hg->pPixels;
     for (int y = r.top; y < r.bottom; y++)
     {
        for (int x = r.left; x < r.right; x++)
        {
            awpPoint p;
            p.X = x;
            p.Y = y;
            if (IsPointInContour(x,y, &contour))
            {
               sum += b[x+y*img->sSizeX];
               x0 += x*b[x+y*img->sSizeX];
               y0 += y*b[x+y*img->sSizeX];
               count++;
               sumv += v[x+y*img->sSizeX];
               sumh += h[x+y*img->sSizeX];
            }
        }
     }

     if (sum  > 0)
     {
        this->m_hasPoint = true;
        m_cm.X = x0 / sum;
        m_cm.Y = y0 / sum;
     }
     if (count > 0)
     {
       sumv /= count;
       sumh /= count;
     }
     m_v = sumv;
     m_h = sumh;
}
int        TLFTriangle::_index()
{
    return this->m_index;
}

double     TLFTriangle::_AB()
{
    double AB = sqrt((m_v1.X - m_v2.X)*(m_v1.X - m_v2.X) + (m_v1.Y - m_v2.Y)*(m_v1.Y - m_v2.Y));
    return AB;
}
double     TLFTriangle::_BC()
{
    double BC = sqrt((m_v2.X - m_v3.X)*(m_v2.X - m_v3.X) + (m_v2.Y - m_v3.Y)*(m_v2.Y - m_v3.Y));
    return BC;
}
double     TLFTriangle::_CA()
{
    double CA = sqrt((m_v3.X - m_v1.X)*(m_v3.X - m_v1.X) + (m_v3.Y - m_v1.Y)*(m_v3.Y - m_v1.Y));
    return CA;
}


double     TLFTriangle::_perim()
{
    double AB = _AB();
    double BC = _BC();
    double CA = _CA();
    return AB + BC + CA;
}
double     TLFTriangle::_square()
{
    double AB = _AB();
    double BC = _BC();
    double CA = _CA();

    double p = (AB + BC + CA) / 2.0;
    double S = sqrt(p*(p-AB)*(p-BC)*(p-CA));
    return S;
}

TLFContour::TLFContour()
{
	this->m_contour = NULL;
	awpCreateContour(&this->m_contour, 0, true);
}
TLFContour::TLFContour(TLFContour& contour)
{
	this->m_contour = NULL;
	awpCreateContour(&this->m_contour, contour.m_contour->NumPoints, contour.m_contour->Direction);
	for (AWPDWORD i = 0; i < contour.m_contour->NumPoints; i++)
	{
		this->m_contour->Points[i] = contour.m_contour->Points[i];
	}
}
TLFContour::~TLFContour()
{
	awpFreeContour(&this->m_contour);
}

void TLFContour::AddPoint(awpPoint& point)
{
	awpContAddPoint(this->m_contour, &point);
}

void TLFContour::RemovePoint(AWPDWORD idx)
{
	awpContRemovePoint(this->m_contour, idx);
}

int TLFContour::GetNumPoints()
{
	return m_contour->NumPoints;
}
awpPoint TLFContour::GetPoint(AWPDWORD idx)
{
	awpPoint result;
	memset(&result, 0, sizeof(awpPoint));
	if ( idx >= this->m_contour->NumPoints)
		return result;
	result = m_contour->Points[idx];
	return result;
}



awpContour* TLFContour::GetContour()
{
	return m_contour;
}

/** Load form XML*/
bool TLFContour::LoadXML(TiXmlElement* parent)
{
	if (parent == NULL)
		return false;
	const char* name = parent->Value();
	if (strcmp(name, this->GetName()) != 0)
		return false;
	int np, dir;
	parent->QueryIntAttribute("np", &np);
	parent->QueryIntAttribute("dir", &dir);

	awpContour* c = NULL;
	if (awpCreateContour(&c, np, dir) != AWP_OK)
		return false;
	int k = 0;
	for (TiXmlNode* child = parent->FirstChild(); child; child = child->NextSibling())
	{
		TLFPoint* point = new TLFPoint();
		if (point == NULL)
			return false;
		if (!point->LoadXML(child->ToElement()))
		{
			delete point;
			awpFreeContour(&c);
			return false;
		}

		c->Points[k] = point->GetPoint();
		delete point;
	}

	awpFreeContour(&this->m_contour);
	this->m_contour = c;

	return true;
}
/** Save to XML*/
bool TLFContour::SaveXML(TiXmlElement* parent)
{
	if (parent == NULL)
		return false;
	TiXmlElement* f = new TiXmlElement(this->GetName());
	
	f->SetAttribute("np", this->m_contour->NumPoints);
	f->SetAttribute("dir", this->m_contour->Direction);

	for (AWPDWORD i = 0; i < this->m_contour->NumPoints; i++)
	{
		TLFPoint pp(m_contour->Points[i]);
		pp.SaveXML(f);
	}
	
	parent->LinkEndChild(f);
	return true;
}

TLF2DContour::TLF2DContour()
{
	this->m_contour = NULL;
	awpCreate2DContour(&this->m_contour, 0, true);
}
TLF2DContour::TLF2DContour(TLF2DContour& contour)
{
	this->m_contour = NULL;
	awpCreate2DContour(&this->m_contour, contour.m_contour->NumPoints, contour.m_contour->Direction);
	for (AWPDWORD i = 0; i < contour.m_contour->NumPoints; i++)
	{
		this->m_contour->Points[i] = contour.m_contour->Points[i];
	}
}
TLF2DContour::~TLF2DContour()
{
	awpFree2DContour(&this->m_contour);
}

TLF2DContour& TLF2DContour::operator = (const TLF2DContour& other)
{
	if (this != &other)
    {
        awpFree2DContour(&this->m_contour);
        awpCreate2DContour(&this->m_contour, other.m_contour->NumPoints, other.m_contour->Direction);
		for (AWPDWORD i = 0; i < other.m_contour->NumPoints; i++)
        {
            this->m_contour->Points[i] = other.m_contour->Points[i];
        }
    }
	return *this;
}


void TLF2DContour::AddPoint(awp2DPoint& point)
{
	awp2DContAddPoint(this->m_contour, &point);
}
void TLF2DContour::RemovePoint(AWPDWORD idx)
{
	awp2DContRemovePoint(this->m_contour, idx);
}

int TLF2DContour::GetNumPoints()
{
	return this->m_contour->NumPoints;
}
awp2DPoint TLF2DContour::GetPoint(AWPDWORD idx)
{
	awp2DPoint result;
	memset(&result, 0, sizeof(awp2DPoint));
	if ( idx >= this->m_contour->NumPoints)
		return result;
	result = m_contour->Points[idx];
	return result;
}

void TLF2DContour::SetPoint(AWPDWORD idx, awp2DPoint& p)
{
	if (idx >= this->m_contour->NumPoints)
		return;
	m_contour->Points[idx] = p;
}

awp2DContour* TLF2DContour::GetContour()
{
	return this->m_contour;
}

/** Load form XML*/
bool TLF2DContour::LoadXML(TiXmlElement* parent)
{
	if (parent == NULL)
		return false;
	const char* name = parent->Value();
	if (strcmp(name, this->GetName()) != 0)
		return false;
	int np, dir;
	parent->QueryIntAttribute("np", &np);
	parent->QueryIntAttribute("dir", &dir);

	awp2DContour* c = NULL;
	if (awpCreate2DContour(&c, np, dir) != AWP_OK)
		return false;
	int k = 0;
	for (TiXmlNode* child = parent->FirstChild(); child; child = child->NextSibling())
	{
		TLF2DPoint* point = new TLF2DPoint();
		if (point == NULL)
			return false;
		if (!point->LoadXML(child->ToElement()))
		{
			delete point;
			awpFree2DContour(&c);
			return false;
		}

		c->Points[k] = point->GetPoint();
        k++;
		delete point;
	}

	awpFree2DContour(&this->m_contour);
	this->m_contour = c;

	return true;
}
/** Save to XML*/
bool TLF2DContour::SaveXML(TiXmlElement* parent)
{
	if (parent == NULL)
		return false;
	TiXmlElement* f = new TiXmlElement(this->GetName());

	f->SetAttribute("np", this->m_contour->NumPoints);
	f->SetAttribute("dir", this->m_contour->Direction);

	for (AWPDWORD i = 0; i < this->m_contour->NumPoints; i++)
	{
		TLF2DPoint pp(m_contour->Points[i]);
		pp.SaveXML(f);
	}

	parent->LinkEndChild(f);
	return true;
}

TLFLineSegment::TLFLineSegment()
{
	memset(&m_segment, 0, sizeof(m_segment));
}

TLFLineSegment::TLFLineSegment(TLFLineSegment& segment)
{
	m_segment = segment.m_segment;
}

void TLFLineSegment::SetStart(awpPoint p)
{
	this->m_segment.strat = p;
}
void TLFLineSegment::SetFihish(awpPoint p)
{
	this->m_segment.end = p;
}

awpPoint TLFLineSegment::GetStart()
{
	return this->m_segment.strat;
}
awpPoint TLFLineSegment::GetFinish()
{
    return this->m_segment.end;
}



bool TLFLineSegment::LoadXML(TiXmlElement* parent)
{
	if (parent == NULL)
		return false;
	const char* name = parent->Value();
	if (strcmp(name, this->GetName()) != 0)
		return false;
    int sx,sy,ex,ey;
	parent->QueryIntAttribute("start_x", &sx);
	parent->QueryIntAttribute("start_y", &sy);

	parent->QueryIntAttribute("end_x", &ex);
	parent->QueryIntAttribute("end_y", &ey);

    this->m_segment.strat.X = sx;
    this->m_segment.strat.Y = sy;
    this->m_segment.end.X = ex;
    this->m_segment.end.Y = ey;

	return true;
}
bool TLFLineSegment::SaveXML(TiXmlElement* parent)
{
	if (parent == NULL)
		return false;
	TiXmlElement* f = new TiXmlElement(this->GetName());

	f->SetAttribute("start_x", this->m_segment.strat.X);
	f->SetAttribute("start_y", this->m_segment.strat.Y);

	f->SetAttribute("end_x", this->m_segment.end.X);
	f->SetAttribute("end_y", this->m_segment.end.Y);

	parent->LinkEndChild(f);
	return true;
}

TLF2DLineSegment::TLF2DLineSegment()
{
	memset(&m_segment, 0, sizeof(m_segment));
}
TLF2DLineSegment::TLF2DLineSegment(TLF2DLineSegment& segment)
{
	m_segment = segment.m_segment;
}

void TLF2DLineSegment::SetStart(awp2DPoint p)
{
 	this->m_segment.strat = p;
}
void TLF2DLineSegment::SetFihish(awp2DPoint p)
{
    this->m_segment.end = p;
}

awp2DPoint TLF2DLineSegment::GetStart()
{
   return this->m_segment.strat;
}
awp2DPoint TLF2DLineSegment::GetFinish()
{
    return this->m_segment.end;
}

awp2DLineSegment TLF2DLineSegment::GetSegment()
{
	return this->m_segment;
}


/** Load form XML*/
bool TLF2DLineSegment::LoadXML(TiXmlElement* parent)
{
	if (parent == NULL)
		return false;
	const char* name = parent->Value();
	if (strcmp(name, this->GetName()) != 0)
		return false;

    double sx,sy,ex,ey;
	parent->QueryDoubleAttribute("start_x", &sx);
	parent->QueryDoubleAttribute("start_y", &sy);

	parent->QueryDoubleAttribute("end_x", &ex);
	parent->QueryDoubleAttribute("end_y", &ey);

    this->m_segment.strat.X = sx;
    this->m_segment.strat.Y = sy;
    this->m_segment.end.X = ex;
    this->m_segment.end.Y = ey;

	return true;
}
/** Save to XML*/
bool TLF2DLineSegment::SaveXML(TiXmlElement* parent)
{
	if (parent == NULL)
		return false;
	TiXmlElement* f = new TiXmlElement(this->GetName());

	f->SetDoubleAttribute("start_x", this->m_segment.strat.X);
	f->SetDoubleAttribute("start_y", this->m_segment.strat.Y);

	f->SetDoubleAttribute("end_x", this->m_segment.end.X);
	f->SetDoubleAttribute("end_y", this->m_segment.end.Y);

	parent->LinkEndChild(f);
	return true;
}

TLF2DOpenPolygon::TLF2DOpenPolygon()
{
}

TLF2DOpenPolygon& TLF2DOpenPolygon::operator=(TLF2DOpenPolygon& other)
{
	if (this != &other)
    {
		this->Clear();
        for (int i = 0; i < other.GetCount(); i++)
        {
        	TLF2DLineSegment* s = new TLF2DLineSegment(*other.GetSegment(i));
            Add(s);
        }
    }
	return *this;
}


TLF2DLineSegment* TLF2DOpenPolygon::GetSegment(int idx)
{
	return dynamic_cast<TLF2DLineSegment*>(Get(idx));
}
void TLF2DOpenPolygon::AddSegment(TLF2DLineSegment* segment)
{
	Add(segment);
}

/** Load form XML*/
bool TLF2DOpenPolygon::LoadXML(TiXmlElement* parent)
{
    if (parent  == NULL)
        return false;
	const char* name = parent->Value();
	if (strcmp(name, this->GetName()) != 0)
		return false;

    for(TiXmlNode* child = parent->FirstChild(); child; child = child->NextSibling() )
    {
         TLF2DLineSegment* ls = new TLF2DLineSegment();
         if (ls == NULL)
            return false;
         if (!ls->LoadXML(child->ToElement()))
         {
            delete ls;
            return false;
         }
        Add( ls );
    }
    return true;
}
/** Save to XML*/
bool TLF2DOpenPolygon::SaveXML(TiXmlElement* parent)
{
	if (parent == NULL)
		return false;
	TiXmlElement* f = new TiXmlElement(this->GetName());

    for (int i = 0; i < this->GetCount(); i++)
    {
        TLF2DLineSegment* s =  GetSegment(i);
        if (s != NULL)
        {
        	s->SaveXML(f);
        }
    }

	parent->LinkEndChild(f);

	return true;
}

TLFTriangleList::TLFTriangleList()
{
	m_procImage = NULL;
	m_vg = NULL;
	m_vh = NULL;
}
TLFTriangleList::~TLFTriangleList()
{
	_AWP_SAFE_RELEASE_(m_procImage);
	_AWP_SAFE_RELEASE_(m_vg);
	_AWP_SAFE_RELEASE_(m_vh);
}

static void _awpGetNormFromGradient(awpImage* img, double h, double v, AWPBYTE* g, int x0, int y0)
{
	AWPBYTE* b = (AWPBYTE*)img->pPixels;
	int grad = 0;
	if (h >= 0)
	{
		if (h > 2 * fabs(v))
			grad = 0;
		else if (h < 0.5*fabs(v))
			grad = 90;
		else
			grad = 45;
	}
	else
	{
		if (fabs(h) > 2 * fabs(v))
			grad = 0;
		else if (fabs(h) < 0.5*fabs(v))
			grad = 90;
		else
			grad = 135;
	}
	if (grad == 0)
	{
		g[0] = b[y0*img->sSizeX + x0 - 2];
		g[1] = b[y0*img->sSizeX + x0 - 1];
		g[2] = b[y0*img->sSizeX + x0];
		g[3] = b[y0*img->sSizeX + x0 + 1];
		g[4] = b[y0*img->sSizeX + x0 + 2];
	}
	else if (grad == 45)
	{
		g[0] = b[(y0 - 2)*img->sSizeX + x0 + 2];
		g[1] = b[(y0 - 1)*img->sSizeX + x0 + 1];
		g[2] = b[y0*img->sSizeX + x0];
		g[3] = b[(y0 + 1)*img->sSizeX + x0 - 1];
		g[4] = b[(y0 + 2)*img->sSizeX + x0 - 2];
	}
	else if (grad == 90)
	{
		g[0] = b[(y0 - 2)*img->sSizeX + x0];
		g[1] = b[(y0 - 1)*img->sSizeX + x0];
		g[2] = b[(y0 - 0)*img->sSizeX + x0];
		g[3] = b[(y0 + 1)*img->sSizeX + x0];
		g[4] = b[(y0 + 2)*img->sSizeX + x0];
	}
	else
	{
		g[0] = b[(y0 + 2)*img->sSizeX + x0 + 2];
		g[1] = b[(y0 + 1)*img->sSizeX + x0 + 1];
		g[2] = b[(y0 + 0)*img->sSizeX + x0 + 0];
		g[3] = b[(y0 - 1)*img->sSizeX + x0 - 1];
		g[4] = b[(y0 - 2)*img->sSizeX + x0 - 2];
	}
}

void TLFTriangleList::ProcessImage(awpImage* img)
{
	_AWP_SAFE_RELEASE_(m_procImage);
	_AWP_SAFE_RELEASE_(m_vg);
	_AWP_SAFE_RELEASE_(m_vh);

	awpImage* img1 = NULL;
	awpImage* img2 = NULL;
	awpImage* img3 = NULL;
	awpImage* img4 = NULL;
	if (img != NULL)
	{
		awpCopyImage(img, &m_procImage);
		awpConvert(m_procImage, AWP_CONVERT_3TO1_BYTE);
		awpCreateImage(&img4, img->sSizeX, img->sSizeY, 1, AWP_BYTE);

		awpGaussianBlur(m_procImage, img4, 2.2f);
		_AWP_SAFE_RELEASE_(m_procImage);
		awpCopyImage(img4, &m_procImage);
		_AWP_SAFE_RELEASE_(img4);

		awpCreateImage(&img1, img->sSizeX, img->sSizeY, 1, AWP_DOUBLE);
		awpCreateImage(&img2, img->sSizeX, img->sSizeY, 1, AWP_DOUBLE);

		//awpEdgeSobel1(m_procImage, img1, img2);

		AWPBYTE* b = (AWPBYTE*)m_procImage->pPixels;
		AWPDOUBLE* v = (AWPDOUBLE*)img1->pPixels;
		AWPDOUBLE* h = (AWPDOUBLE*)img2->pPixels;
		AWPBYTE    GNorm[5];
		//
		for (int i = 0; i < img->sSizeX*img->sSizeY; i++)
			b[i] = sqrt(v[i] * v[i] + h[i] * h[i]) > 255 ? 255 : (AWPBYTE)sqrt(v[i] * v[i] + h[i] * h[i]);

		awpCopyImage(m_procImage, &img3);
		AWPBYTE* bb = (AWPBYTE*)img3->pPixels;
		for (int y = 2; y < img->sSizeY - 2; y++)
		{
			for (int x = 2; x < img->sSizeX - 2; x++)
			{

				if (b[x + y*img->sSizeX] > 16)
				{
					_awpGetNormFromGradient(m_procImage, v[x + y*img->sSizeX], h[x + y*img->sSizeX], GNorm, x, y);
					AWPBYTE max = GNorm[0];

					for (int k = 1; k < 5; k++)
					{
						if (max <= GNorm[k])
							max = GNorm[k];
					}
					if (b[x + y*img->sSizeX] != max)
						bb[x + y*img->sSizeX] = 0;
					else
						bb[x + y*img->sSizeX] = max;
				}
				else
					bb[x + y*img->sSizeX] = 0;
			}
		}

		awpFill(m_procImage, 0);
		awpStrokeObj* obj = NULL;
		int Num = 0;
		awpGetStrokes(img3, &Num, &obj, 16, NULL);
		for (int i = 0; i < Num; i++)
		{
			awpRect r;
			int s;
			awpCalcObjRect(&obj[i], &r);
			awpStrObjSquare(&obj[i], &s);
			AWPDOUBLE v = 255;
			awpGetObjIntensity(img3, &obj[i], &v);
			int w = r.right - r.left;
			int h = r.bottom - r.top;
			if (v > 32)
			{
				awpDrawStrokeObj(m_procImage, &obj[i], (AWPBYTE)v);
			}
		}
		awpFreeStrokes(Num, &obj);

		m_vg = img1;
		m_vh = img2;
		_AWP_SAFE_RELEASE_(img3);
	}
}

void TLFTriangleList::Process(TLFImage& img)
{
	Process(img.GetImage());
}

void TLFTriangleList::Process(awpImage* img)
{
	this->ProcessImage(img);
	if (this->m_procImage == NULL)
		return;
	for (int i = 0; i < this->GetCount(); i++)
	{
		TLFTriangle* t = this->GetTriangle(i);
		t->Process(this->m_procImage, m_vg, m_vh);
	}
}

TLFTriangle* TLFTriangleList::GetTriangle(int index)
{
	if (index >= 0 && index < this->GetCount())
		return (TLFTriangle*)Get(index);
	return NULL;
}

void TLFTriangleList::AddTriangle(TLFTriangle* t)
{
	if (t == NULL)
		return;
	TLFTriangle* tt = new TLFTriangle(t);
	this->Add(tt);
}

double TLFTriangleList::Ro(TLFTriangleList* list)
{
	return 0;
}

void TLFTriangleList::Save(const char* lpFileName)
{
	FILE* f = fopen(lpFileName, "w+t");
	for (int i = 0; i < GetCount(); i++)
	{
		TLFTriangle* t = this->GetTriangle(i);
		fprintf(f, "%d\t%f\t%f\t%f\t%f\n", t->_index(), t->_cm().X, t->_cm().Y, t->_v(), t->_h());
	}
	fclose(f);
}
