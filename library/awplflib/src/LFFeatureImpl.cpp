/*
    Locate From 4.0
    File: LFFeatureImpl.cpp
    Purpose: ILFFeature implementation
    Copyright (c) AWPSoft.
*/
#include "LF.h"



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
	int sx, sy, w, h;
	parent->QueryIntAttribute("sxBase", &sx);
	parent->QueryIntAttribute("syBase", &sy);
	parent->QueryIntAttribute("wBase", &w);
	parent->QueryIntAttribute("hBase", &h);

	m_base.SetRect(sx, sy, w, h);
	
	return true;
}
bool ILFFeature::SaveXML(TiXmlElement* parent)
{
	if (parent == NULL)
		return false;
	TiXmlElement* f = new TiXmlElement(this->GetName());
	f->SetAttribute("sxBase", m_base.Left());
	f->SetAttribute("syBase", m_base.Top());
	f->SetAttribute("wBase", m_base.Width());
	f->SetAttribute("hBase", m_base.Height());
	parent->LinkEndChild(f);
	return true;
}
