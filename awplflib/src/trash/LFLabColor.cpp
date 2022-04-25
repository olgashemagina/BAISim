/*
Locate From 4.0
File: LFLabColor.cpp
Purpose: Color convertion and color samples database support 
Copyright (c) AWPSoft.
*/

#include "_LF.h"
TLFLabColor::TLFLabColor()
{
	this->m_valueA = 0;
	this->m_valueL = 0;
	this->m_valueB = 0;
}

TLFLabColor::TLFLabColor(const AWPDOUBLE l, const  AWPDOUBLE a, const  AWPDOUBLE b)
{
	this->m_valueA = a;
	this->m_valueL = l;
	this->m_valueB = b;
}
TLFLabColor::TLFLabColor(TLFLabColor& color)
{
	this->m_valueA = color.m_valueA;
	this->m_valueL = color.m_valueL;
	this->m_valueB = color.m_valueB;
}

void TLFLabColor::GetLab(AWPDOUBLE& l, AWPDOUBLE& a, AWPDOUBLE& b)
{
	l = m_valueL;
	a = m_valueA;
	b = m_valueB;
}
void TLFLabColor::GetXyz(AWPDOUBLE& x, AWPDOUBLE& y, AWPDOUBLE& z)
{
	AWPDOUBLE xx, yy, zz;
	AWPBYTE   r, g, b;
	awpLABtoRGB(m_valueL, m_valueA, m_valueB, &r, &g, &b);
	awpRGBtoXYZ(r,g,b, &xx, &yy, &zz);
	x = xx;
	y = yy;
	z = zz;
}
void TLFLabColor::GetRGB(AWPDOUBLE& r, AWPDOUBLE& g, AWPDOUBLE& b)
{
	AWPBYTE   rr, gg, bb;
	awpLABtoRGB(m_valueL, m_valueA, m_valueB, &rr, &gg, &bb);
	r = rr;
	g = gg;
	b = bb;
}
void TLFLabColor::GetHSV(AWPDOUBLE& h, AWPDOUBLE& s, AWPDOUBLE& v)
{
	AWPDOUBLE hh, ss, vv;
	AWPBYTE   r, g, b;
	awpLABtoRGB(m_valueL, m_valueA, m_valueB, &r, &g, &b);
	awpRGBtoXYZ(r, g, b, &hh, &ss, &vv);
	h = hh;
	s = ss;
	v = vv;
}
void TLFLabColor::GetHSL(AWPDOUBLE& h, AWPDOUBLE& s, AWPDOUBLE& l)
{
	AWPDOUBLE hh, ss, ll;
	AWPBYTE   r, g, b;
	awpLABtoRGB(m_valueL, m_valueA, m_valueB, &r, &g, &b);
	awpRGBtoXYZ(r, g, b, &hh, &ss, &ll);
	h = hh;
	s = ss;
	l = ll;
}
void TLFLabColor::GetWeb(AWPINT& web)
{
	AWPBYTE   rr, gg, bb;
	AWPINT    _web;
	awpLABtoRGB(m_valueL, m_valueA, m_valueB, &rr, &gg, &bb);
	awpRGBtoWeb(rr, gg, bb, &_web);
	web = _web;
}

void TLFLabColor::SetLab(const AWPDOUBLE l, const AWPDOUBLE a, const AWPDOUBLE b)
{
	this->m_valueA = a;
	this->m_valueL = l;
	this->m_valueB = b;
}
void TLFLabColor::SetXyz(const AWPDOUBLE x, const AWPDOUBLE y, const AWPDOUBLE z)
{
	AWPBYTE   rr, gg, bb;
	awpXYZtoRGB(x, y, z, &rr, &gg, &bb);
	awpRGBtoLAB(rr,gg,bb, &m_valueL, &m_valueA, &m_valueB);
}
void TLFLabColor::SetRGB(const AWPDOUBLE r, const AWPDOUBLE g, const AWPDOUBLE b)
{
	//todo: check if r,g,b in the 0..255 range 
	awpRGBtoLAB((AWPBYTE)r, (AWPBYTE)g, (AWPBYTE)b, &m_valueL, &m_valueA, &m_valueB);
}
void TLFLabColor::SetHSV(const AWPDOUBLE h, const AWPDOUBLE s, const AWPDOUBLE v)
{
	AWPBYTE   rr, gg, bb;
	awpHSVtoRGB(h, s, v, &rr, &gg, &bb);
	awpRGBtoLAB(rr, gg, bb, &m_valueL, &m_valueA, &m_valueB);
}
void TLFLabColor::SetHSL(const AWPDOUBLE h, const AWPDOUBLE s, const AWPDOUBLE l)
{
	AWPBYTE   rr, gg, bb;
	awpHSVtoRGB(h, s, l, &rr, &gg, &bb);
	awpRGBtoLAB(rr, gg, bb, &m_valueL, &m_valueA, &m_valueB);
}
void TLFLabColor::SetWeb(const AWPINT web)
{
	AWPBYTE   rr, gg, bb;
	awpWebtoRGB(web, &rr, &gg, &bb);
	awpRGBtoLAB(rr, gg, bb, &m_valueL, &m_valueA, &m_valueB);
}

AWPDOUBLE TLFLabColor::CIE76(TLFLabColor& color)
{
	return sqrt((m_valueL - color.m_valueL)*(m_valueL - color.m_valueL) + (m_valueA - color.m_valueA)*(m_valueA - color.m_valueA) + (m_valueB - color.m_valueB)*(m_valueB - color.m_valueB));
}

TiXmlElement* TLFLabColor::SaveXml()
{
	TiXmlElement* f = new TiXmlElement(this->GetName());
	f->SetDoubleAttribute("L", m_valueL);
	f->SetDoubleAttribute("A", m_valueA);
	f->SetDoubleAttribute("B", m_valueB);
	return f;
}

bool TLFLabColor::LoadXml(TiXmlElement* element)
{
	if (element == NULL)
		return false;
	const char* name = element->Value();
	if (strcmp(name, this->GetName()) != 0)
		return false;
	element->QueryDoubleAttribute("L", &m_valueL);
	element->QueryDoubleAttribute("A", &m_valueA);
	element->QueryDoubleAttribute("B", &m_valueB);
	return true;
}



TLFLabColorDatabase::TLFLabColorDatabase()
{

}

TLFLabColorDatabase::TLFLabColorDatabase(TLFLabColorDatabase& _db)
{
    for (int i = 0; i < _db.GetCount(); i++)
        this->AddColor(*_db.GetColor(i));
}

TLFLabColorDatabase& TLFLabColorDatabase::operator=(TLFLabColorDatabase& _db)
{
 	if (this != &_db)
    {
	    Clear();
        for (int i = 0; i < _db.GetCount(); i++)
            AddColor(*_db.GetColor(i));
    }
	return *this;
}


void TLFLabColorDatabase::AddColor(TLFLabColor& color)
{
	TLFLabColor* c = new TLFLabColor(color);
	Add(c);
}
TLFLabColor* TLFLabColorDatabase::GetColor(int index)
{
	if (Get(index) == NULL)
		return NULL;
	TLFLabColor* c = dynamic_cast<TLFLabColor*>(Get(index));
    return c;
}

bool TLFLabColorDatabase::SaveToXml(const char* lpFileName)
{
	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
	doc.LinkEndChild(decl);
	TiXmlElement* element = new TiXmlElement(GetName());
	if (element == NULL)
		return false;
	for (int i = 0; i < GetCount(); i++)
	{
		TLFLabColor* c = dynamic_cast<TLFLabColor*>(Get(i));
		if (c != NULL)
		{
			TiXmlElement* e = c->SaveXml();
			element->LinkEndChild(e);
		}
	}
	doc.LinkEndChild(element);
	return doc.SaveFile(lpFileName);
}
bool TLFLabColorDatabase::LoadFromXml(const char* lpFileName)
{
	Clear();
	TiXmlDocument doc(lpFileName);
	if (!doc.LoadFile())
		return false;
	TiXmlHandle hDoc(&doc);
	TiXmlElement* pElem = NULL;
	TiXmlHandle hRoot(0);
	pElem = hDoc.FirstChildElement().Element();

	if (strcmp(pElem->Value(), GetName()) != 0)
		return false;

	for (TiXmlNode* child = pElem->FirstChild(); child; child = child->NextSibling())
	{
		TLFLabColor color(0,0,0);
		if (color.LoadXml(child->ToElement()))
			AddColor(color);
	}
	return true;
}

TLFLabColor* TLFLabColorDatabase::Center()
{
	if (this->GetCount() == 0)
		return NULL;

	AWPDOUBLE l, a, b;
	l = 0; 
	b = 0;
	a = 0;
	int cc = 0;
	for (int i = 0; i < GetCount(); i++)
	{
		TLFLabColor* c = dynamic_cast<TLFLabColor*>(Get(i));
		if (c != NULL)
		{
			AWPDOUBLE ll, aa, bb;
			c->GetLab(ll,aa,bb);
			l += ll;
			a += aa;
			b += bb;
			cc++;
		}
	}
	if (cc == 0)
		return NULL;

	return new TLFLabColor(l/cc, a /cc, b / cc);
}
TLFLabColor* TLFLabColorDatabase::Nearlest(TLFLabColor& color, double& dist, int& ind)
{
	dist = 100;
	ind = -1;
	if (this->GetCount() == 0)
		return NULL;
	int index = 0;
	for (int i = 0; i < GetCount(); i++)
	{
		TLFLabColor* c = dynamic_cast<TLFLabColor*>(Get(i));
		if (c != NULL)
		{
			double v = color.CIE76(*c);
			if (v < dist)
			{
				dist = v;
				index = i;
			}
		}
	}
	ind = index;
	return GetColor(index);
}
// exchange sorting
bool TLFLabColorDatabase::SortFromColor(TLFLabColor& color, TLFLabColorDatabase& db)
{
    for (int i = 0; i < this->GetCount(); i++)
    {
        double min = 1e10;
        int index = -1;
        for (int j = i; j < this->GetCount();j++)
        {
            double d = this->Distance(color, j);
            if (d < min)
            {
                min = d;
                index = j;
            }
        }
        if (index >= 0)
        {
            Exchange(i,index);
            db.Exchange(i, index);
        }
    }
    return true;
}
// distance to color in the database by index
double TLFLabColorDatabase::Distance(TLFLabColor& color, int idx)
{
   if (idx < 0 || idx >= this->GetCount())
     return -1;
   TLFLabColor* c = this->GetColor(idx);
   return c->CIE76(color);
}

bool  TLFLabColorDatabase::Copy(TLFLabColorDatabase& _db, int start, int end)
{
    if (start < 0 || start > end || end >= GetCount())
        return false;
    _db.Clear();
    for (int i = start; i < end; i++)
    	_db.AddColor(*GetColor(i));
    return true;
}


