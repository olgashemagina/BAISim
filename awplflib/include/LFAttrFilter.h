//---------------------------------------------------------------------------
#ifndef __LF_ATTR_FILTER__
#define __LF_ATTR_FILTER__
/*
#include <string>
#include "LF.h"
#include "LFStrong.h"
#include "LFCSBoost.h"
*/
#define NAME_SIZE 32

class TCSAdaBoostSign;

class TLFAttrCascadeDetector : public ILFAttrDetector
{
protected:
	virtual bool DoClassify(TLFImage* pImage, TLFRect* pRoi, SLFAttrResult& result);
public :
	TLFAttrCascadeDetector();
};

class TLFAttrSeriesDetector : public ILFAttrDetector
{
private:
   TLFAttrCascadeDetector    *m_infant_detector; 
   TLFAttrCascadeDetector    *m_do_6_detector; 
   TLFAttrCascadeDetector	*m_do_11_detector;
   TLFAttrCascadeDetector	*m_do_17_detector;
protected:
	virtual bool DoClassify(TLFImage* pImage, TLFRect* pRoi, SLFAttrResult& result);
public :
	TLFAttrSeriesDetector();
	virtual ~TLFAttrSeriesDetector();
	bool Load(const char* pXmlFileName);
};

using namespace std;

typedef enum names { unknown = 0, adult, child } ClassName;
/*
        Детектор атирбутов, основанный на двух сильных классификаторах
        типа TCSStrong. Одни из них принимает решение о наличии атрибута,
        другой о его отсутствии.
        Решение принимается путем голосования, если оба классификатора
        высказываются за то или иное решение. Если оба классификатора
        принимают одинаковое решение, то общее решение не определено.
*/
class TCSSeparate : public ILFAttrClassifier
{
private:
    TCSStrong* m_Detector;
    TCSStrong* m_Detector1;
protected:
    virtual bool DoClassify(TLFImage* pImage, TLFRect* pRoi, SLFAttrResult& result);
public:
    TCSSeparate();
    virtual ~TCSSeparate();
    //
    virtual void  SetThreshold1(double value);
    virtual void  SetThreshold2(double value);
	virtual const char* GetName() {return "TCSSeparate";}
    virtual ILFAttrClassifier* LoadFromNode(TiXmlElement* parent);
	virtual bool SaveToNode(TiXmlElement* parent);
};
/*
Классификатор (эксперт) атрибутов TAttrCSStrongSign, построенный на одном сильном
классификаторе типа  TCSStrongSign.
*/
class TAttrCSStrongSign : public ILFAttrClassifier
{
private:
        TCSStrongSign* m_Detector;
protected:
    virtual bool DoClassify(TLFImage* pImage, TLFRect* pRoi, SLFAttrResult& result);
public:
        TAttrCSStrongSign();
        virtual ~TAttrCSStrongSign();
        // установка порогов
        virtual void  SetThreshold1(double value);
        virtual void  SetThreshold2(double value);
        virtual const char* GetName () {return "TAttrCSStrongSign";}
        virtual ILFAttrClassifier* LoadFromNode(TiXmlElement* parent);
 		virtual bool SaveToNode(TiXmlElement* parent);

        TCSStrongSign* GetDetector()
        {
                return m_Detector;
        }
};

#endif // __LF_ATTR_FILTER__

