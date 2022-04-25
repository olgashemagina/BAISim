/*
    Locate Face 4.0
    File: LFStrongImpl.cpp
    Purpose: ILFStrong implementation
    Copyright (c) AWPSoft.
*/
#include "_LF.h"

ILFStrong::ILFStrong()
{
    m_threshold = 0.f;
}
ILFStrong::~ILFStrong()
{
    // empty
}

void ILFStrong::Setup(double scale_x, double scale_y,  AWPWORD dx, AWPWORD dy)
{
    for (int i = 0; i < GetCount(); i++)
    {
        ILFWeak* w = (ILFWeak*)Get(i);
        if (w != NULL)
            w->Setup(scale_x, scale_y, dx, dy);
    }
}

ILFWeak* ILFStrong::GetWeak(int index)
{
    return (ILFWeak*)Get(index);
}


void ILFStrong::AddWeak(ILFWeak* pWeak)
{
	Add(pWeak);
	double t = 0;
	for (int i = 0; i < GetCount(); i++)
		t += GetWeak(i)->Weight();
	t /= 2;
	this->m_threshold = t;
}

/*
    threshold access
*/
double ILFStrong::GetThreshold()
{
    return m_threshold;
}
void ILFStrong::SetThreshold(double Value)
{
  m_threshold = Value;
}


TIEFSStrong::TIEFSStrong()
{
	this->m_threshold = 0;
}
double TIEFSStrong::Raiting(TIEFSSample* sample)
{
	double result = 0;
	for (int i = 0; i < this->GetCount(); i++)
	{
		TIEFSWeak* e = (TIEFSWeak*)Get(i);
		result += e->GetWeight()*e->Classify(sample->GetValue(e->GetIndex()));
	}
	return result;
}
int TIEFSStrong::Classify(TIEFSSample* sample)
{
	double result = Raiting(sample);
	return result >= this->m_threshold ? 1 : 0;
}
double TIEFSStrong::GetSumWeakWeight()
{
	double result = 0;
	for (int i = 0; i < this->GetCount(); i++)
	{
		TIEFSWeak* e = (TIEFSWeak*)Get(i);
		result += e->GetWeight();
	}
	return result;
}
double TIEFSStrong::GetThreshold()
{
	return this->m_threshold;
}
void TIEFSStrong::SetThreshold(double value)
{
	this->m_threshold = value;
}
TiXmlElement* TIEFSStrong::SaveXML()
{
	TiXmlElement* parent = new TiXmlElement(GetName());
	if (parent == NULL)
		return NULL;
	parent->SetDoubleAttribute("thr", this->m_threshold);
	for (int i = 0; i < this->GetCount(); i++)
	{
		TIEFSWeak* e = (TIEFSWeak*)Get(i);
		parent->LinkEndChild(e->SaveXML());
	}
	return parent;
}
bool TIEFSStrong::SaveXML(const char* lpFileName)
{
	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
	doc.LinkEndChild(decl);
	TiXmlElement* element = SaveXML();
	
	if (element != NULL)
		doc.LinkEndChild(element);

	return doc.SaveFile(lpFileName);
}
bool TIEFSStrong::LoadXML(const char* lpFileName)
{
	this->Clear();
	TiXmlDocument doc(lpFileName);
	if (!doc.LoadFile())
		return false;
	TiXmlElement* e = doc.FirstChildElement(this->GetName());
	if (e == NULL)
		return false;

	for (TiXmlNode* child = e->FirstChild(); child; child = child->NextSibling())
	{
		TIEFSWeak* pWc = new TIEFSWeak();
		if (pWc == NULL)
			return false;
		if (!pWc->LoadXML(child->ToElement()))
		{
			delete pWc;
			Clear();
			return false;
		}
		this->Add(pWc);
	}
	return true;
}
