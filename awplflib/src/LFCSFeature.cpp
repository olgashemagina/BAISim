#include "_LF.h"
//#pragma hdrstop
TLFCSFeature::TLFCSFeature() : ILFFeature()
{
 // set empty rect
 //memset(&m_Rect, 0, sizeof(m_rect));
 // set empty coordiants
 memset(&m_xcoords, 0, sizeof(m_xcoords));
 memset(&m_ycoords, 0, sizeof(m_ycoords));
}
TLFCSFeature::TLFCSFeature(ILFFeature* pFeature) : ILFFeature(pFeature)
{
    // setup rect and coordinats 
}
/*
    calc features value
*/
unsigned int     TLFCSFeature::uCalcValue(TLFImage* pImage)
{
   return 0;
}
/*
    Действительное значение для данного типа признака не вычисляется
*/
double TLFCSFeature::fCalcValue(TLFImage* pImage)
{
  return 0;
}
/*
    XML io operations
*/
bool TLFCSFeature::LoadXML(TiXmlElement* parent)
{
    if (parent == NULL)
        return false;
    const char* name = parent->Value();
    if (strcmp (name, "CSFeature") != 0)
        return false;
    int Value = 0;
    parent->QueryIntAttribute("sxBase",&Value);
    m_sxBase = (AWPWORD)Value;
    parent->QueryIntAttribute("syBase",&Value);
    m_syBase = (AWPWORD)Value;
    parent->QueryIntAttribute("wBase",&Value);
    m_wBase = (AWPWORD)Value;
    parent->QueryIntAttribute("hBase",&Value);
    m_hBase = (AWPWORD)Value;
    Setup(1,1, 0, 0);
    return true;
}
bool TLFCSFeature::SaveXML(TiXmlElement* parent)
{
    if (parent == NULL)
        return false;
    TiXmlElement* f = new TiXmlElement("CSFeature");
    f->SetAttribute("sxBase", m_sxBase);
    f->SetAttribute("syBase", m_syBase);
    f->SetAttribute("wBase", m_wBase);
    f->SetAttribute("hBase", m_hBase);
    parent->LinkEndChild(f);
    return true;
}
/*
    feature rectangle
*/
awpRect TLFCSFeature::GetRect()
{
    return m_Rect;
}

ILFFeature* TLFCSFeature::Clone()
{
   TLFCSFeature* res = new TLFCSFeature(this);
   return res;
}

