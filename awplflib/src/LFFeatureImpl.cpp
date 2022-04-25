/*
    Locate From 4.0
    File: LFFeatureImpl.cpp
    Purpose: ILFFeature implementation
    Copyright (c) AWPSoft.
*/
#include "_LF.h"

// construction
ILFFeature::ILFFeature()
{
    m_sxBase =0;              // base point  within aperture
    m_syBase =0;

    m_wBase =1;              // size of feature unit  within aperture
    m_hBase =1;

    m_sx =0;                 // base point within image
    m_sy =0;

    m_w = 1;                  // size of feature unit within image
	m_h = 1;

	m_uValue = 0;
	m_fValue = 0.;
}
ILFFeature::ILFFeature(ILFFeature* pFeature)
{
    m_sxBase =0;              // base point  within aperture
    m_syBase =0;

    m_wBase =1;              // size of feature unit  within aperture
    m_hBase =1;

    m_sx =0;                 // base point within image
    m_sy =0;

    m_w =1;                  // size of feature unit within image
    m_h =1;
    if (pFeature == NULL)
        return;
    m_sxBase =pFeature->sxBase();              // base point  within aperture
    m_syBase =pFeature->syBase();

    m_wBase =pFeature->wUnitBase();              // size of feature unit  within aperture
    m_hBase =pFeature->hUnitBase();

    m_sx =pFeature->sx();                 // base point within image
    m_sy =pFeature->sy();

    m_w =pFeature->wUnit();                  // size of feature unit within image
    m_h =pFeature->hUnit();
}

ILFFeature::ILFFeature(int sxbase, int sybase, int wbase, int hbase)
{
	m_sxBase = sxbase;              // base point  within aperture
	m_syBase = sybase;

	m_wBase = wbase;              // size of feature unit  within aperture
	m_hBase = hbase;

	m_sx = m_sxBase;                 // base point within image
	m_sy = m_syBase;

	m_w = m_wBase;                  // size of feature unit within image
	m_h = m_hBase;

}

ILFFeature::~ILFFeature()
{
 // empty
}
/*
        featere setup
*/
void ILFFeature::Setup(double scale_x, double scale_y, AWPWORD dx, AWPWORD dy)
{
   m_sx = int(m_sxBase*scale_x + dx + 0.5);
   m_sy = int(m_syBase*scale_y + dy + 0.5);

   m_w = int(m_wBase*scale_x + 0.5);
   m_h = int(m_hBase*scale_y + 0.5);
}

void ILFFeature::Setup(TLFRect& rect)
{
    m_sx = rect.Left();
    m_sy = rect.Top();
    m_w  = rect.Width();
    m_h  = rect.Height();
}

void ILFFeature::Scale(double factor)
{
	m_sx = (int)(m_sxBase*factor + 0.5);
	m_sy = (int)(m_syBase*factor + 0.5);

	m_w = (int)(m_wBase*factor + 0.5);
	m_h = (int)(m_hBase*factor + 0.5);
}
void ILFFeature::Shift(int dx, int dy)
{
	m_sx += dx;
	m_sy += dy;
}

// aperture position
int ILFFeature::sxBase()
{
   return m_sxBase;
}
int ILFFeature::syBase()
{
   return m_syBase;
}
int ILFFeature::wUnitBase()
{
   return m_wBase;
}
int ILFFeature::hUnitBase()
{
  return m_hBase;
}
// image position
int ILFFeature::sx()
{
    return m_sx;
}
int ILFFeature::sy()
{
   return m_sy;
}
int ILFFeature::wUnit()
{
  return m_w;
}
int ILFFeature::hUnit()
{
  return m_h;
}

unsigned int ILFFeature::uValue()
{
	return this->m_uValue;
}
double       ILFFeature::fValue()
{
	return this->m_fValue;
}

//
awpRect ILFFeature::GetRect()
{
	awpRect result;
	result.left = this->m_sx;
	result.top = this->m_sy;
	result.right = this->m_sx + this->m_w;
	result.bottom = this->m_sy + this->m_h;
	return result;
}


/*
XML io operations
*/
bool ILFFeature::LoadXML(TiXmlElement* parent)
{
	if (parent == NULL)
		return false;
	const char* name = parent->Value();
	if (strcmp(name, this->GetName()) != 0)
		return false;
	parent->QueryIntAttribute("sxBase", &m_sxBase);
	parent->QueryIntAttribute("syBase", &m_syBase);
	parent->QueryIntAttribute("wBase", &m_wBase);
	parent->QueryIntAttribute("hBase", &m_hBase);
	Scale(1);
	return true;
}
bool ILFFeature::SaveXML(TiXmlElement* parent)
{
	if (parent == NULL)
		return false;
	TiXmlElement* f = new TiXmlElement(this->GetName());
	f->SetAttribute("sxBase", m_sxBase);
	f->SetAttribute("syBase", m_syBase);
	f->SetAttribute("wBase", m_wBase);
	f->SetAttribute("hBase", m_hBase);
	parent->LinkEndChild(f);
	return true;
}
