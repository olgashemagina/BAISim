//---------------------------------------------------------------------------
#include "_LF.h"



using namespace std;

// #if defined(_CRT_SECURE_NO_DEPRECATE) && !defined(_CRT_SECURE_NO_WARNINGS)
//	 #define _CRT_SECURE_NO_WARNINGS
// #endif

static    string ExtractFilePath( string path )
    {
	string output("");
	unsigned int pos = path.rfind( "/", path.size() );
	if( pos != path.size() )
	{
	    output = path.substr( 0, pos );
	}
	return output;
    }

static ILFAttrClassifier* LoadFromNode(TiXmlElement* parent)
{
	
	if (parent == NULL)
		return NULL;
	const char* name = parent->Value();
	if (strcmp(name, "AttrNode") != 0)
		return NULL;
	const char* str =  parent->Attribute("sensor");
	if (str == NULL)
		return NULL;
	ILFAttrClassifier* result = NULL;
	if (strcmp(str, "CSSeparate") == 0)
	{
		result = new TCSSeparate();
	}
	else if (strcmp(str, "AttrCSStrongSign") == 0)
	{
		result = new TAttrCSStrongSign();
	}


	if (result != NULL)
		result->LoadFromNode(parent);
	return result;

}

/* ILFAttrDetector implementation
*/
ILFAttrDetector::ILFAttrDetector()
{
	m_Name = "interfase of attribute detector. abstract.";

	m_className1 = NULL;
	m_className2= NULL;
	m_baseSize = 24;
	m_objectScale =1; 
	m_needResize =1; 
	m_fastComputing = true;
}
ILFAttrDetector::~ILFAttrDetector()
{
    //if (m_className1)
    //    free((void*)m_className1);
    //if (m_className2)
    //    free((void*)m_className2);
    m_className1 = NULL;
    m_className2 = NULL;
}
/* xml io support 
*/
bool ILFAttrDetector::Create(const char* pClassName1, const char* pClassName2, 
	int baseSize, double objectScale, bool needResize, const char* pXmlFileName)
{
	this->SetClassName1(pClassName1);
	this->SetClassName2(pClassName2);
	this->m_baseSize = baseSize;
	this->m_objectScale = objectScale;
	this->m_needResize = needResize;
	if (!this->Save(pXmlFileName))
		return false;
	return this->Load(pXmlFileName);
}
bool ILFAttrDetector::Load(const char* pXmlFileName)
{
   TiXmlDocument doc(pXmlFileName);
   if (!doc.LoadFile())
    return false;

   TiXmlHandle hDoc(&doc);
   TiXmlElement* pElem = NULL;
   TiXmlHandle hRoot(0);

   pElem = hDoc.FirstChildElement().Element();
    if (!pElem)
        return false;
    const char* name = pElem->Value();
    if (strcmp(name, "AttrFilter") != 0)
        return false;
    hRoot=TiXmlHandle(pElem);

    pElem = hRoot.FirstChild( "Classes" ).Element();
    if (pElem == NULL)
        return false;
 
    const char* class_name = pElem->Attribute("class_name1");
	this->SetClassName1(class_name);
	class_name = pElem->Attribute("class_name2");
	this->SetClassName2(class_name);

	pElem = pElem->NextSiblingElement("BaseSize");
	if (!pElem)
		return false;
	pElem->Attribute("size", &this->m_baseSize);

	pElem = pElem->NextSiblingElement("ObjectScale");
	if (!pElem)
		return false;
	int Value = 1;
	pElem->Attribute("scale", &Value);
	this->m_objectScale = Value / 10.f;
	
	pElem = pElem->NextSiblingElement("FragmentResize");
	if (!pElem)
		return false;
	pElem->Attribute("resize", &Value);
	if (Value != 0)
		this->m_needResize = true;
	else
		this->m_needResize = false;

    m_Experts.Clear();
	
	for(TiXmlNode* child = pElem->NextSibling(); child; child = child->NextSibling() )
	 {
		 ILFAttrClassifier* cl = LoadFromNode(child->ToElement());
		if (cl != NULL)
			m_Experts.Add(cl);
	 }

    return true;
}
bool ILFAttrDetector::Save(const char* pXmlFileName)
{
   TiXmlDocument doc;

   TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );
   doc.LinkEndChild( decl );

   	TiXmlElement * root = new TiXmlElement( "AttrFilter" );
	doc.LinkEndChild( root );

    TiXmlElement* classes = new TiXmlElement("Classes");

    classes->SetAttribute("class_name1", m_className1);
    classes->SetAttribute("class_name2", m_className2);
    root->LinkEndChild(classes);

	TiXmlElement* baseSize = new TiXmlElement("BaseSize");
	baseSize->SetAttribute("size", this->m_baseSize);
    root->LinkEndChild(baseSize);

	TiXmlElement* objectScale = new TiXmlElement("ObjectScale");
	objectScale->SetAttribute("scale", (int)(this->m_objectScale*10));
    root->LinkEndChild(objectScale);

	TiXmlElement* fragmentResize = new TiXmlElement("FragmentResize");
	fragmentResize->SetAttribute("resize", this->m_needResize);
    root->LinkEndChild(fragmentResize);

	for (int i = 0; i < m_Experts.GetCount(); i++)
	{
		ILFAttrClassifier* cl = (ILFAttrClassifier*)m_Experts.Get(i);
		TiXmlElement* expertNode = new TiXmlElement("AttrNode");
		root->LinkEndChild(expertNode);
		cl->SaveToNode(expertNode);
	}
	return doc.SaveFile(pXmlFileName);
}
/* classify image
*/
bool ILFAttrDetector::Classify(TLFImage* pImage, TLFRect* pRoi, SLFAttrResult& result)
{
	// todo: this code is subject to change to support fast computing 
	return DoClassify(pImage, pRoi, result);
}
/* detector structure 
*/
int ILFAttrDetector::GetNumExperts()
{
	return m_Experts.GetCount();
}
ILFAttrClassifier* ILFAttrDetector::GetExpert(int nIndex)
{
	if (nIndex < 0 || nIndex > m_Experts.GetCount())
		return NULL;
	return (ILFAttrClassifier*)m_Experts.Get(nIndex);
}
/* edit structure 
*/
int ILFAttrDetector::AddExpert(ILFAttrClassifier* pExpert)
{
	m_Experts.Add(pExpert);
	return m_Experts.GetCount();
}
bool ILFAttrDetector::RemoveExpert(int nIndex)
{
	if (nIndex >= 0 && nIndex < m_Experts.GetCount())
	{
		m_Experts.Delete(nIndex);
		return true;
	}
	else
		return false;
}
/* properties 
*/
char* ILFAttrDetector::GetClassName1()
{
	return this->m_className1;
}
void ILFAttrDetector::SetClassName1(const char* lpClassName1)
{
	if (this->m_className1)
	{
		free(this->m_className1);
		this->m_className1 = NULL;
	}

	m_className1 = (char*) malloc (strlen(lpClassName1)+1);
	strcpy(this->m_className1, lpClassName1);
}
char* ILFAttrDetector::GetClassName2()
{
	return this->m_className2;
}
void ILFAttrDetector::SetClassName2(const char* lpClassName2)
{
	if (this->m_className2)
	{
		free(this->m_className2);
		this->m_className2 = NULL;
	}
	m_className2 = (char*) malloc (strlen(lpClassName2)+1);
	strcpy(this->m_className2, lpClassName2);
}
/* read only properties 
*/
int  ILFAttrDetector::GetBaseSize()
{
	return this->m_baseSize;
}
double ILFAttrDetector::GetObjectSize()
{
	return this->m_objectScale;
}
bool ILFAttrDetector::NeedScaleImage()
{
	return this->m_needResize;
}
/* computing control
*/
bool ILFAttrDetector::NeedFastComputing()
{
	return this->m_fastComputing;
}
void ILFAttrDetector::SetNeedFastComputing(bool Value)
{
	this->m_fastComputing = Value;
}
const char* ILFAttrDetector::GetName()
{
	return this->m_Name;
}
/*TLFAttrCascadeDetector implementation
*/
TLFAttrCascadeDetector::TLFAttrCascadeDetector() 
{
	m_Name = "Cascade detector of image attributes.";
}
bool TLFAttrCascadeDetector::DoClassify(TLFImage* pImage, TLFRect* pRoi, SLFAttrResult& result)
{
	
	bool res = false;
	
	result.m_Raiting1 = 0;
	result.m_Raiting2 = 0;
	result.m_Result = 0;
	
	SLFAttrResult tmp_res;
	memset(&tmp_res, 0, sizeof(SLFAttrResult));

	if (pImage == NULL)
		return res;
	if (pRoi == NULL)
		return res;
    awpRect rect = pRoi->GetRect();
    if (awpRectInImage(pImage->GetImage(), &rect) != AWP_OK )
		return false;

	if (m_Experts.GetCount() == 0)
		return false;
	// если установлен флаг m_needResize, то выполним первичное 
	// преобразование изображения
	if (this->m_needResize)
	{
		TLFImage fragment;
		awpImage* img = pImage->GetImage(); 
		//todo: check this code
		pRoi->Inflate(1,1);
		awpRect r = pRoi->GetRect();
		awpImage* img1 = NULL;
		awpImage* img2 = NULL;
		awpCopyRect(img, &img1, &r);
		if (img1 == NULL)
			return false;
		awpConvert(img1, AWP_CONVERT_3TO1_BYTE);
		awpResize(img1, this->m_baseSize, this->m_baseSize);
		awpIntegral( img1, &img2, 0 );
		fragment.SetImage(img2);
		awpReleaseImage(&img1);
		awpReleaseImage(&img2);
		for (int i = 0; i < this->m_Experts.GetCount(); i++)
		{
			ILFAttrClassifier* cl = (ILFAttrClassifier*)m_Experts.Get(i);
			cl->Classify(&fragment, NULL, tmp_res);
			result.m_Raiting1 += tmp_res.m_Raiting1;
			result.m_Raiting2 += tmp_res.m_Raiting2;
			result.m_Result = tmp_res.m_Result;
			if (result.m_Result != 0)
				break;
		}
		return true;
	}
	for (int i = 0; i < m_Experts.GetCount(); i++)
	{
		ILFAttrClassifier* cl = (ILFAttrClassifier*)m_Experts.Get(i);
		cl->Classify(pImage, pRoi, tmp_res);
		result.m_Raiting1 += tmp_res.m_Raiting1;
		result.m_Raiting2 += tmp_res.m_Raiting2;
		result.m_Result = tmp_res.m_Result;
		if (result.m_Result != 0)
			break;
	}
	return true;
}

/* end of TLFAttrCascadeDetector implementation
*/
//---------------------------------------------------------------------------
ILFAttrClassifier::ILFAttrClassifier()
{
    m_threshold1 = 0;
    m_threshold2 = 0;
}
//---------------------------------------------------------------------------
ILFAttrClassifier::~ILFAttrClassifier()
{
}
//---------------------------------------------------------------------------
double ILFAttrClassifier::GetThreshold1()
{
    return m_threshold1;
}
//---------------------------------------------------------------------------
void  ILFAttrClassifier::SetThreshold1(double value)
{
    m_threshold1 = value;
}
//---------------------------------------------------------------------------
double ILFAttrClassifier::GetThreshold2()
{
    return m_threshold2;
}
//---------------------------------------------------------------------------
void  ILFAttrClassifier::SetThreshold2(double value)
{
    m_threshold2 = value;
}
//---------------------------------------------------------------------------
// todo: completely redo 
bool ILFAttrClassifier::Classify(TLFImage* pImage, TLFRect* pRoi, SLFAttrResult& result)
{
    if (true)
    {
        bool res = false;
        res = DoClassify(pImage, pRoi, m_LastResult);
        result = m_LastResult;
        return res;
    }
}
SLFAttrResult ILFAttrClassifier::GetlastResult()
{
        return m_LastResult;
}

// TCSSeparate Class------------------------------------------------------------
#define BANNER_SCSEPARATE "[CSSEPARATE]"
TCSSeparate::TCSSeparate()
{
    m_Detector = new TCSStrong();
    m_Detector1 = new TCSStrong();
}

TCSSeparate::~TCSSeparate()
{
    delete m_Detector;
    delete m_Detector1;
}

void  TCSSeparate::SetThreshold1(double value)
{
    m_threshold1 = value;
    m_Detector->SetThreshold(value);
}
void  TCSSeparate::SetThreshold2(double value)
{
    m_threshold2 = value;
    m_Detector1->SetThreshold(value);
}
ILFAttrClassifier* TCSSeparate::LoadFromNode(TiXmlElement* parent)
{
  if (parent == NULL)
        return NULL;
  TiXmlElement* pElem= parent;
  TiXmlHandle hRoot(0);
  const char* str =  pElem->Attribute("sensor");
  if (strcmp(str, "CSSeparate") != 0 )
    return NULL;
  hRoot = TiXmlHandle(pElem);
  pElem = hRoot.FirstChild("CSClassificator").Element();
  if (pElem == NULL)
      return NULL;
  if (!m_Detector->LoadXML(pElem))
      return NULL;
  pElem = hRoot.Child(1).ToElement();
  if (pElem == NULL)
      return NULL;
  if (!m_Detector1->LoadXML(pElem))
      return NULL;
    m_threshold1 = m_Detector->GetThreshold();
    m_threshold2 = m_Detector1->GetThreshold();
  return this;
}

bool TCSSeparate::DoClassify(TLFImage* pImage, TLFRect* pRoi, SLFAttrResult& result)
{
   if (pImage == NULL || pRoi == NULL)
    return false;
   awpImage* img = pImage->GetImage();
   if (img == NULL)
    return false;
   awpImage* fragment = NULL;
   awpRect r = pRoi->GetRect();
   awpCopyRect(img, &fragment, &r);
   if (fragment == NULL)
    return false;

#ifdef _DEBUG
    awpSaveImage("C:\\fragment.jpg", fragment);
#endif
	TLFImage test_img;
	test_img.SetImage(fragment);

   awpRect window;
   window.left = 0;
   window.top = 0;
   window.right = fragment->sSizeX;
   window.bottom = fragment->sSizeY;

   m_Detector->Setup(window, 24);
   m_Detector1->Setup(window, 24);

   double err1 = 0;
   double err2 = 0;
   int res1 = -1;
   int res2 = -1;


   res1 = m_Detector->Classify(&test_img, err1) ? 1 : 0;
   res2 = m_Detector1->Classify(&test_img, err2) ? 1 : 0;

   if (res1 + res2 == 0 || res1 + res2 == 2)
      result.m_Result = 0;
   else if (res1 == 1)
     result.m_Result = 1;
   else
     result.m_Result = -1;

   result.m_Raiting1 = double(err1 /  m_Detector->GetSumWeakWeight());
   result.m_Raiting2 = double(err2 /  m_Detector1->GetSumWeakWeight());

   awpReleaseImage(&fragment);
   return true;
}
// сохранение структуры в xml узел
bool TCSSeparate::SaveToNode(TiXmlElement* parent)
{
   TiXmlElement* attr_node = new TiXmlElement("AttrNode");
   attr_node->SetAttribute("sensor", "CSSeparate");
   attr_node->SetAttribute("Type", 1);
   parent->LinkEndChild(attr_node);
   m_Detector->SaveXML(attr_node);
   m_Detector1->SaveXML(attr_node);
   return true;
}



//реализация класса TAttrCSStrongSign

TAttrCSStrongSign::TAttrCSStrongSign()
{
    m_Detector = new TCSStrongSign();
}

TAttrCSStrongSign::~TAttrCSStrongSign()
{
	if(m_Detector!= NULL)
		delete m_Detector;
}


void  TAttrCSStrongSign::SetThreshold1(double value)
{
    m_threshold1 = value;
}

void  TAttrCSStrongSign::SetThreshold2(double value)
{
    m_threshold2 = value;
}

bool TAttrCSStrongSign::DoClassify(TLFImage* pImage, TLFRect* pRoi, SLFAttrResult& result)
{

   if (pImage == NULL)
    return false;
   if (pRoi != NULL)
   {
	   awpImage* img = pImage->GetImage();
	   if (img == NULL)
		return false;

	   awpImage* fragment = NULL;
	   awpImage* pIntegral = NULL;
	   awpRect r = pRoi->GetRect();

	   awpCopyRect(img, &fragment, &r);
	   if (fragment == NULL)
		return false;
	   awpConvert(fragment, AWP_CONVERT_3TO1_BYTE);
	   awpIntegral( fragment, &pIntegral, 0 );

	   awpRect window;
	   window.left = 0;
	   window.top = 0;
	   window.right = fragment->sSizeX;
	   window.bottom = fragment->sSizeY;

	   double err1 = 0;
	   m_Detector->Classify(pIntegral, err1);

	   if (err1 > m_threshold1)
	   {
		  if (err1 > m_threshold2)
			result.m_Result = 1;
		  else
			result.m_Result = 0;
	   }
	   else
		 result.m_Result = -1;

	   result.m_Raiting1 = err1;
	   result.m_Raiting2 = err1;

	   awpReleaseImage(&fragment);
	   awpReleaseImage(&pIntegral);
	   return true;
   
   }
   else
   {
	   awpImage* img = pImage->GetImage();
	   if (img == NULL)
		return false;
   
	   double err1 = 0;
	   m_Detector->Classify(img, err1);

	   if (err1 > m_threshold1)
	   {
		  if (err1 > m_threshold2)
			result.m_Result = 1;
		  else
			result.m_Result = 0;
	   }
	   else
		 result.m_Result = -1;

	   result.m_Raiting1 = err1;
	   result.m_Raiting2 = err1;
	   return true;
   }
}

ILFAttrClassifier* TAttrCSStrongSign::LoadFromNode(TiXmlElement* parent)
{
   TiXmlElement* pElem = NULL;

   pElem = parent->ToElement();
        if (!pElem)
            return NULL;
   // AttrNode
    const char* class_name = pElem->Attribute("sensor");
    if (strcmp(class_name, "AttrCSStrongSign") != 0 )
        return NULL;
    double v;
	pElem->Attribute("thr1", &v); 
	this->m_threshold1 = v;
	pElem->Attribute("thr2", &v);
	this->m_threshold2 = v;

    if (!m_Detector->LoadXML(pElem))
        return NULL;
   return this;
}

bool TAttrCSStrongSign::SaveToNode(TiXmlElement* parent)
{
   TiXmlElement* pElem = NULL;

   pElem = parent->ToElement();
        if (!pElem)
            return false;
   pElem->SetAttribute("sensor", "AttrCSStrongSign");
   pElem->SetAttribute("Type", 1);
   pElem->SetDoubleAttribute("thr1", this->m_threshold1);
   pElem->SetDoubleAttribute("thr2", this->m_threshold2);
   m_Detector->SaveXML(pElem);
   return true;
}

// class TLFAttrSeriesDetector

TLFAttrSeriesDetector::TLFAttrSeriesDetector()
{
	m_infant_detector = new TLFAttrCascadeDetector();
	m_do_6_detector = new TLFAttrCascadeDetector();
	m_do_11_detector = new TLFAttrCascadeDetector();
	m_do_17_detector = new TLFAttrCascadeDetector();
}

TLFAttrSeriesDetector::~TLFAttrSeriesDetector()
{
	if( m_infant_detector != NULL )
		delete m_infant_detector;
	if( m_do_6_detector != NULL )
		delete m_do_6_detector;
	if( m_do_11_detector != NULL )
		delete m_do_11_detector;
	if( m_do_17_detector != NULL )
		delete m_do_17_detector;
}

bool TLFAttrSeriesDetector::Load(const char* pXmlFileName)
{
   TiXmlDocument doc(pXmlFileName);
   if (!doc.LoadFile())
    return false;

   TiXmlHandle hDoc(&doc);
   TiXmlElement* pElem = NULL;
   TiXmlHandle hRoot(0);

   pElem = hDoc.FirstChildElement().Element();
    if (!pElem)
        return false;
    const char* name = pElem->Value();
    if (strcmp(name, "Detectors") != 0)
        return false;
    hRoot=TiXmlHandle(pElem);

    pElem = hRoot.FirstChild( "Detector" ).Element();
    if (pElem == NULL)
        return false;
 
    const char* infant_detector_path = pElem->Attribute("path");
	if(!m_infant_detector->Load(infant_detector_path))
		return false;

	pElem = pElem->NextSiblingElement("Detector");
	if (!pElem)
		return false;
	const char* do_6_detector_path = pElem->Attribute("path");
	if(!m_do_6_detector->Load(do_6_detector_path))
		return false;

	pElem = pElem->NextSiblingElement("Detector");
	if (!pElem)
		return false;
	const char* do_11_detector_path = pElem->Attribute("path");
	if(!m_do_11_detector->Load(do_11_detector_path))
		return false;

	pElem = pElem->NextSiblingElement("Detector");
	if (!pElem)
		return false;
	const char* do_17_detector_path = pElem->Attribute("path");
	if(!m_do_17_detector->Load(do_17_detector_path))
		return false;

    return true;
}

bool TLFAttrSeriesDetector::DoClassify(TLFImage* pImage, TLFRect* pRoi, SLFAttrResult& result)
{
	if ( !m_infant_detector->Classify( pImage, pRoi, result ) )
		return false;
	if ( result.m_Result == -1 || result.m_Result == 0 )
	{
		if ( !m_do_6_detector->Classify( pImage, pRoi, result ) )
			return false;
	}	
	else return true;
	if( result.m_Result == -1 || result.m_Result == 0 )
	{		
		if ( !m_do_11_detector->Classify( pImage, pRoi, result ) )
			return false;
	}
	else return true;
	if( result.m_Result == -1 || result.m_Result == 0 )
	{
		if ( !m_do_17_detector->Classify( pImage, pRoi, result ) )
			return false;
	}
	else
		return true;
	return true;
}


//------------------------------------------------------------------------------
#ifdef __BCPLUSPLUS__
	#pragma package(smart_init)
#endif

#undef _CRT_SECURE_NO_WARNINGS


