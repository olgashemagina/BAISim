/*
    Locate Face 4.0
    File: LFCSFeature.h
    Purpose: LF Census feature declaration
    Copyright (c) AWPSoft.
*/
#ifndef _LF_CS_FEATURE_H_
#define _LF_CS_FEATURE_H_

#include "LF.h"

class TLFCSFeature : public ILFFeature
{
protected:
    /*
        Feature rectangle
    */
    awpRect m_Rect;
    /*
        coordiants
    */
    AWPWORD m_xcoords[4];
    AWPWORD m_ycoords[4];
public:
    TLFCSFeature();
    TLFCSFeature(ILFFeature* pFeature);
    /*
    calc features value
    */
    virtual unsigned int     uCalcValue(TLFImage* pImage);
    virtual double 			fCalcValue(TLFImage* pImage);
    /*
        XML io operations
    */
    virtual bool LoadXML(TiXmlElement* parent);
    virtual bool SaveXML(TiXmlElement* parent);
    /*
        feature rectangle
    */
    virtual awpRect GetRect();

    virtual ILFFeature* Clone();
};

#endif //_LF_CS_FEATURE_H_
 
