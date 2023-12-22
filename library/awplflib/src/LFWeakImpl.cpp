/*
    Locate Face 4.0
    File: LFWeakImpl.cpp
    Purpose: ILFWeak implementation
    Copyright (c) AWPSoft.
*/
#include "LF.h"
#include "LFWeak.h"

ILFWeak::ILFWeak(const char* lpFeatureName)
{
    m_fWeight = 1.f;
    m_pFeature = LFCreateFeature(lpFeatureName, 0,0,4,4);
}
/*

*/
ILFWeak::ILFWeak(ILFFeature* pFeature)
{
  m_fWeight = 1.f;
  m_pFeature = LFCreateFeature(pFeature);
}
ILFWeak::~ILFWeak()
{
    if (m_pFeature != NULL)
        delete m_pFeature;
}


ILFFeature* ILFWeak::Fetaure()
{
	return this->m_pFeature;
}
double ILFWeak::Weight()
{
	return this->m_fWeight;
}
void   ILFWeak::SetWeight(double value)
{
	m_fWeight = value;
}



 
