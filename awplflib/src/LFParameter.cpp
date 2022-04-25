#include "_LF.h"


TLFParameter::TLFParameter()
{
	memset(m_Name, 0, sizeof(m_Name));
	memset(m_Descr, 0, sizeof(m_Descr));
	m_Value = 0;
	m_MinValue = -FLT_MAX;
	m_MaxValue = FLT_MAX;
}
TLFParameter::~TLFParameter(){}

char* TLFParameter::GetPName()
{
	return m_Name;
}

char* TLFParameter::GetDescription()
{
	return m_Descr;
}
double TLFParameter::GetValue()
{
	return m_Value;
}
double TLFParameter::GetMaxValue()
{
	return m_MaxValue;
}
double TLFParameter::GetMinValue()
{
	return m_MinValue;
}
void  TLFParameter::SetPName(const char* Value)
{
	if (strlen(Value) < LF_MAX_NAME_LEN)
		strcpy(m_Name, Value);
}
void  TLFParameter::SetDescription(const char* Value)
{
	if (strlen(Value) < LF_MAX_DESCR_LEN)
		strcpy(m_Descr, Value);
}
bool  TLFParameter::SetValue(double Value)
{
	if (Value >= m_MinValue && Value <= m_MaxValue)
		m_Value = Value;
	else 
		return false;
	return true;
}
bool  TLFParameter::SetMaxValue(double Value)
{
	if (m_Value <= Value)
		m_MaxValue = Value;
	else
		return false;
	return true;
}
bool TLFParameter::SetMinValue(double Value)
{
	if (Value >= m_Value)
		m_MinValue = Value;
	else
		return false;
	return true;
}
bool TLFParameter::LoadXML(TiXmlElement* parent)
{

    if (parent == NULL)
        return false;
    const char* type = parent->Attribute("type");

    if (type == NULL)
        return false;
    if (strcmp(type, GetName()) != 0)
        return false;

    const char* attrName = parent->Attribute("m_Name");
    if (attrName == NULL)
       return false;
    strcpy(m_Name, attrName);

    const char* attrDescr = parent->Attribute("m_Descr");
    if (attrDescr == NULL)
       return false;
    strcpy(m_Descr, attrDescr);

    if (parent->Attribute("m_Value", &m_Value) == NULL)
        return false;

    if (parent->Attribute("m_MaxValue", &m_MaxValue) == NULL)
        return false;

    if (parent->Attribute("m_MinValue", &m_MinValue) == NULL)
        return false;

	return true;
}
TiXmlElement* TLFParameter::SaveXML()
{
    TiXmlElement* e = new  TiXmlElement(GetName());
    e->SetAttribute("type", GetName());
    e->SetDoubleAttribute("m_Value", m_Value);
    e->SetDoubleAttribute("m_MaxValue", m_MaxValue);
    e->SetDoubleAttribute("m_MinValue",m_MinValue);
    e->SetAttribute("m_Name", m_Name);
    e->SetAttribute("m_Descr",m_Descr);
	return e;
}