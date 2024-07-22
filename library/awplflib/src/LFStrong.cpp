#include "LFStrong.h"

//---------------------------------------------------------------------------
// сильный классификатор, основанный на преобразовании Census
TCSStrong::TCSStrong()
{
}

// классификафия

TCSStrong::TResult TCSStrong::Classify(TLFImage* pImage, const TLFAlignedTransform& transform)
{
    double sumWeight = 0;
    double err = 0;
	int c = GetCount();
	if (c == 0)
        return {};
	TCSWeak** pWCL = (TCSWeak**)GetList();
    TResult result;

	for (int i = 0; i < c; i++)
	{
        auto weak = pWCL[i]->Classify(pImage, transform);
		err += pWCL[i]->Weight() * weak.result;
        sumWeight += pWCL[i]->Weight();
        result.weaks.emplace_back(std::move(weak));
	}
          

    result.score = err / sumWeight;
    result.result = ((err - m_threshold) > -0.00001);
    return result;
}

void TCSStrong::SaveXML(TiXmlElement* parent)
{
   if (parent == NULL)
    return;

   //
   TiXmlElement* classificator = new TiXmlElement("CSClassificator");
   classificator->SetDoubleAttribute("threshold", m_threshold);

    int c = GetCount();
    TCSWeak** pWCL = (TCSWeak**)GetList();
    for (int i = 0; i < c; i++)
        pWCL[i]->SaveXML(classificator);

    parent->LinkEndChild(classificator);
    return;
}

bool TCSStrong::LoadXML(TiXmlElement* parent)
{
    if (parent  == NULL)
        return false;
    const char* name = parent->Value();
    if (strcmp(name, "CSClassificator") != 0)
        return false;
    parent->QueryDoubleAttribute("threshold", &m_threshold);
    for(TiXmlNode* child = parent->FirstChild(); child; child = child->NextSibling() )
    {
         TCSWeak* pWc = new TCSWeak();
         if (pWc == NULL)
            return false;
         if (!pWc->LoadXML(child->ToElement()))
         {
            delete pWc;
            return false;
         }
        Add( pWc );
        m_sumWeakWeight += pWc->Weight();
    }
	parent->QueryDoubleAttribute("threshold", &m_threshold);
    return true;
}

double TCSStrong::GetSumWeakWeight()
{
	return this->m_sumWeakWeight;
}

////////////////////////////////////////////////////////////////////////////////
TCSStrongSign::TCSStrongSign()
{
	m_sumWeakWeight = 0;
	this->m_Threshold = 0;
}
/*
// позиционирование
void TCSStrongSign::Setup( awpRect const& window, int det_width )
{
	double factor = (double)(window.right - window.left - 1) / (double)det_width;

    int c = m_WeakList.GetCount();
    TCSWeakSign** pWCL = (TCSWeakSign**)m_WeakList.GetList();
    for (int i = 0; i < c; i++)
    {
        pWCL[i]->Scale( factor );
        pWCL[i]->Shift( window.left+1, window.top+1);
    }
}*/
// классификафия
ILFStrong::TResult TCSStrongSign::Classify(TLFImage* pImage, const TLFAlignedTransform& transform)
{
	
	int c = m_WeakList.GetCount();
    TCSWeakSign** pWCL = (TCSWeakSign**)m_WeakList.GetList();
    double err = 0;
    ILFStrong::TResult result;
    for (int i = 0; i < c; i++)
    {
        auto weak = pWCL[i]->Classify(pImage, transform); 
        err +=  pWCL[i]->Weight() * weak.result;
        result.weaks.emplace_back(std::move(weak));
    }
    //
    int cls = 0;
    if (fabs(err) > m_Threshold)
    {
        cls = err > 0 ? 1 : -1;
    }
    result.result = cls;
    return result;

}
//ввод - вывод
void TCSStrongSign::SaveXML(TiXmlElement* parent)
{
   if (parent == NULL)
    return;

   //
   TiXmlElement* classificator = new TiXmlElement("AttrCSStrongSign");
   classificator->SetDoubleAttribute("threshold", m_Threshold);

    int c = m_WeakList.GetCount();
    TCSWeakSign** pWCL = (TCSWeakSign**)m_WeakList.GetList();
    for (int i = 0; i < c; i++)
        pWCL[i]->SaveXML(classificator);

    parent->LinkEndChild(classificator);
    return;
}
bool TCSStrongSign::LoadXML(TiXmlElement* parent)
{
    if (parent  == NULL)
        return false;

	parent = parent->FirstChildElement();
    const char* name = parent->Value();
    if (strcmp(name, "AttrCSStrongSign") != 0)
        return false;
    double val = 0;
    parent->QueryDoubleAttribute("threshold", &val);
	m_Threshold = val;
	
    for(TiXmlNode* child = parent->FirstChild(); child; child = child->NextSibling() )
    {
         TCSWeakSign* pWc = new TCSWeakSign();
         if (pWc == NULL)
            return false;
         if (!pWc->LoadXML(child->ToElement()))
         {
            delete pWc;
            return false;
         }
        m_WeakList.Add( pWc );
    }
    return true;
}

// порог принятия решения
double TCSStrongSign::GetThreshold()
{
    return m_Threshold;
}
void   TCSStrongSign::SetThreshold(double Value)
{
    m_Threshold = Value;
}

TCSWeakSign* TCSStrongSign::GetWeak(int index)
{
    if (index < 0 || index > m_WeakList.GetCount())
        return NULL;

    return (TCSWeakSign*)m_WeakList.Get(index);

}
void TCSStrongSign::AddWeak(TCSWeakSign* Weak)
{
   TCSWeakSign* NewWeak = new TCSWeakSign(*Weak);
   m_WeakList.Add(NewWeak);
   m_sumWeakWeight = 0;
   TCSWeakSign** pWCL = (TCSWeakSign**)m_WeakList.GetList();
   for (int i = 0; i < m_WeakList.GetCount(); i++)
	   m_sumWeakWeight += pWCL[i]->Weight();
}

void TCSStrongSign::Clear()
{
   m_WeakList.Clear();
}


ILFStrong*    LFCreateStrong(const char* lpName)
{
	ILFStrong* result = NULL;
	if (strcmp(lpName, "CSClassificator") == 0)
		return new TCSStrong();
	return result;
}
