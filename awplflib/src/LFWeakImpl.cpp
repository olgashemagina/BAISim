/*
    Locate Face 4.0
    File: LFWeakImpl.cpp
    Purpose: ILFWeak implementation
    Copyright (c) AWPSoft.
*/
#include "_LF.h"

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

/*
  positioning feature on image
*/
void  ILFWeak::Setup(double scale_x, double scale_y, AWPWORD dx, AWPWORD dy)
{
    if (m_pFeature != NULL)
        m_pFeature->Setup(scale_x,scale_y, dx, dy);
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



 
