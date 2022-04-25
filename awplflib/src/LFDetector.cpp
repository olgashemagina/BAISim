/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                           License Agreement
//                  Locate Framework  Computer Vision Library
//
// Copyright (C) 2004-2018, NN-Videolab.net, all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the NN-Videolab.net or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//      Locate Framework  (LF) Computer Vision Library.
//		File: LFDetector.cpp
//		Purpose: implementation ILFObjectDetector, TLFFGBGDetector, TSCObjectDetector
//
//      CopyRight 2004-2018 (c) NN-Videolab.net
//M*/

#include "_LF.h"


using namespace std;
#ifndef __CLR_OR_THIS_CALL
	#define __CLR_OR_THIS_CALL __cdecl
#endif
#ifdef WIN32
/* 128 bit GUID to human-readable string */
static char * guid_to_str(GUID* id, char * out) {
	int i;
	char * ret = out;
	out += sprintf(out, "%.8lX-%.4hX-%.4hX-", id->Data1, id->Data2, id->Data3);
	for (i = 0; i < sizeof(id->Data4); ++i) {
		out += sprintf(out, "%.2hhX", id->Data4[i]);
		if (i == 1) *(out++) = '-';
	}
	return ret;
}
#endif


#define ANGLE0 15
//ILFObjectDetector
ILFObjectDetector::ILFObjectDetector()
{
	m_baseWidth = 24;
	m_baseHeight = 24;
	m_Type = "unknown";
	m_Angle = 0;
	m_racurs = 0;
	m_scanner = new TLFScanner();
	m_strDetName = "ILFObjectDetector";
};
ILFObjectDetector::~ILFObjectDetector()
{
	if (m_scanner != NULL)
		delete m_scanner;
}

int ILFObjectDetector::GetBaseWidth()
{
	return this->m_baseWidth;
}
void ILFObjectDetector::SetBaseWidht(int w)
{
	this->Clear();
	m_baseWidth = w;
//	if (m_scanner != NULL)
//		m_scanner->SetBaseWidth(m_baseWidth);
}
int ILFObjectDetector::GetBaseHeight()
{
	return this->m_baseHeight;
}
void ILFObjectDetector::SetBaseHeight(int h)
{
	this->Clear();
	m_baseHeight = h;
//	if (m_scanner != NULL)
//		m_scanner->SetBaseHeight(m_baseHeight);
}

int  ILFObjectDetector::GetAngle()
{
	return m_Angle;
}
void ILFObjectDetector::SetAngle(int value)
{
	m_Angle = value;
}
int  ILFObjectDetector::GetRacurs()
{
	return m_racurs;
}
void ILFObjectDetector::SetRacurs(int value)
{
	m_racurs = value;
}


TLFImage* ILFObjectDetector::GetImage()
{
	return &this->m_Image;
}

ILFScanner*   ILFObjectDetector::GetScanner()
{
	return this->m_scanner;
}

void ILFObjectDetector::SetScanner(ILFScanner* scanner)
{
	if (this->m_scanner != NULL)
		delete this->m_scanner;
	
	//todo: this seems as bug
	m_scanner = scanner;
}

int  ILFObjectDetector::GetNumItems()
{
	return this->m_objects.GetCount();
}
TLFDetectedItem* ILFObjectDetector::GetItem(int index)
{
	if (index < 0 || index >= m_objects.GetCount())
		return NULL;

	return (TLFDetectedItem*)m_objects.Get(index);
}

std::string ILFObjectDetector::GetObjectType()
{
	return this->m_Type;
}

void ILFObjectDetector::Clear()
{
	this->m_objects.Clear();
	if (this->m_scanner != NULL)
		this->m_scanner->Clear();

}


void ILFObjectDetector::SaveImages(std::string path)
{
	char uuid_buf[130];
	std::string strOut = path;
	
	UUID id;

    LF_UUID_CREATE(id)
#ifdef WIN32
	guid_to_str(&id, uuid_buf);
#else
    uuid_unparse(id, uuid_buf);
#endif
	std::string strFileName = path;
	strFileName += "\\";
	strFileName += LFGUIDToString(&id);
	strFileName += ".awp";
	m_Image.SaveToFile(strFileName.c_str());

}

int ILFObjectDetector::GetStagesCount()
{
	return this->m_Strongs.GetCount();
}
int ILFObjectDetector::GetSensorsCount(int Stage)
{
	if (Stage < 0 || Stage >= m_Strongs.GetCount())
		return -1;
	ILFStrong** pHc = (ILFStrong**)m_Strongs.GetList();
	return pHc[Stage]->GetCount();
}
double ILFObjectDetector::GetStageWeight(int Stage)
{
	if (Stage < 0 || Stage >= m_Strongs.GetCount())
		return -1;
	ILFStrong** pHc = (ILFStrong**)m_Strongs.GetList();
	if (pHc[Stage] == NULL)
		return -1;
	double result = 0;
	for (int i = 0; i < pHc[Stage]->GetCount(); i++)
	{
		ILFWeak* w = pHc[Stage]->GetWeak(i);
		result += w->Weight();
	}
	return result;
}
double ILFObjectDetector::GetStageThreshold(int Stage)
{
	if (Stage < 0 || Stage >= m_Strongs.GetCount())
		return -1;
	ILFStrong** pHc = (ILFStrong**)m_Strongs.GetList();
	return pHc[Stage]->GetThreshold();
}

TLFObjectList* ILFObjectDetector::GetStrongs()
{
	return (TLFObjectList*)&m_Strongs;
}



#define DCOUNT 15
TSCObjectDetector::TSCObjectDetector() : ILFObjectDetector()
{ 
	this->m_baseHeight = 24; 
	this->m_baseWidth = 24; 
};
TSCObjectDetector::~TSCObjectDetector()
{
};

// init detector with image
bool TSCObjectDetector::Init(awpImage* pImage, bool scan)
{
	if (pImage == NULL || pImage->dwType != AWP_BYTE || m_scanner == NULL)
		return false;
	bool changed = m_Image.GetImage() == NULL || m_Image.GetImage()->sSizeX != pImage->sSizeX || m_Image.GetImage()->sSizeY != pImage->sSizeY || m_scanner->GetFragmentsCount() == 0;
    m_Image.FreeImages();
	m_Image.SetImage(pImage);
	if (scan && changed)
		m_scanner->ScanImage(&m_Image);
	return m_Image.GetImage() != NULL;
}
bool TSCObjectDetector::AddStrong(ILFStrong* strong)
{
	if (strong == NULL)
		return false;
	TCSStrong* strong0 = dynamic_cast<TCSStrong*>(strong);
	if (strong0 == NULL)
		false;
	int n = 1;
	for (int k = 0; k < n; k++)
	{
		TCSStrong* st = new TCSStrong();
		for (int i = 0; i < strong0->GetCount(); i++)
		{
			ILFWeak* iweak = strong0->GetWeak(i);
			TCSWeak* weak = dynamic_cast<TCSWeak*>(iweak);
			TCSWeak* w = new TCSWeak(*weak);
			st->AddWeak(w);
		}
		st->SetThreshold(strong->GetThreshold());
		this->m_Strongs.Add(st);
	}
	return true;
}

// classification
int  TSCObjectDetector::ClassifyRect(awpRect Fragmnet, double* err, int* vect)
{
	bool   result = 1;

	for (int j = 0; j < m_Strongs.GetCount(); j++)
    {
		double err1 = 0;
		int    solution = 0;
		TCSStrong* strong = (TCSStrong*)m_Strongs.Get(j);
        strong->Setup(Fragmnet, m_baseWidth);
		solution = strong->Classify(&m_Image, err1);
		if(err != NULL) 
			err[j] = err1;
		if (vect != NULL)
			vect[j] = 1;
		if (solution == 0)
        {
			if (vect != NULL)
				vect[j] = 0;
			result = 0;
			return result;
        }
    }
    return result;

}
// возвращает число найденных объектов. 
int  TSCObjectDetector::Detect()
{
	int res = 0;
	if (this->m_scanner == NULL)
		return res;
	if (this->m_Image.GetImage() == NULL)
		return res;
	for (int i = 0; i < m_scanner->GetFragmentsCount(); i++)
	{

		if (!m_scanner->GetFragment(i)->HasObject)
		{
			bool has_object = true;
			double scale = (double)(m_scanner->GetFragmentRect(i).right - m_scanner->GetFragmentRect(i).left)/(double)this->m_baseWidth;
			for (int j = 0; j < m_Strongs.GetCount(); j++)
			{
				TCSStrong* strong = (TCSStrong*)m_Strongs.Get(j);
				strong->Setup(m_scanner->GetFragment(i)->Rect, m_baseWidth);
				double err = 0;
				if (strong->Classify(&m_Image, err) == 0)
				{
					has_object = false;
					break;
				}
			}

			if (has_object)
			{
				//object detected
				m_scanner->GetFragment(i)->HasObject = true;
			}
		}
		else
		{
			m_scanner->GetFragment(i)->HasObject = false;
		}

	}



	// записываем результат в лист.
	m_objects.Clear();
	for (int i = 0; i < m_scanner->GetFragmentsCount(); i++)
	{
		if (m_scanner->GetFragment(i)->HasObject)
		{
			awpRect rect = m_scanner->GetFragmentRect(i);
			UUID id;
			LF_NULL_UUID_CREATE(id);
			TLFDetectedItem* de = new TLFDetectedItem(&rect, 0, this->m_Type, this->m_Angle,
			 this->m_racurs, this->m_baseWidth, this->m_baseHeight, this->m_strDetName.c_str(), id);
			de->SetHasObject(true);
			m_objects.Add(de);
			res++;
		}
		m_scanner->GetFragment(i)->HasObject = false;
	}

	return m_objects.GetCount();
}

int TSCObjectDetector::DetectInRect(awpRect roi)
{

	int res = 0;
	if (this->m_scanner == NULL)
		return res;
	if (this->m_Image.GetImage() == NULL)
		return res;
	for (int i = 0; i < m_scanner->GetFragmentsCount(); i++)
	{
		awpRect fr = m_scanner->GetFragmentRect(i);
		awpPoint c;
		c.X = (fr.left + fr.right) / 2;
		c.Y = (fr.top + fr.bottom) / 2;
		if (c.X >= roi.left && c.X <= roi.right &&
		c.Y >= roi.top && c.Y <= roi.bottom)
		{
			if (!m_scanner->GetFragment(i)->HasObject)
			{

				bool has_object = true;
				double scale = (double)(m_scanner->GetFragmentRect(i).right - m_scanner->GetFragmentRect(i).left)/(double)this->m_baseWidth;
				m_scanner->GetFragment(i)->raiting = 0;
				for (int j = 0; j < m_Strongs.GetCount(); j++)
				{
					TCSStrong* strong = (TCSStrong*)m_Strongs.Get(j);
					strong->Setup(m_scanner->GetFragment(i)->Rect, m_baseWidth);
					double err = 0;
					if (strong->Classify(&m_Image, err) == 0)
					{
						has_object = false;
						break;
					}
					m_scanner->GetFragment(i)->raiting += err;
				}

				if (has_object)
				{
					//object detected
					m_scanner->GetFragment(i)->HasObject = true;
				}
			}
			else
			{
				m_scanner->GetFragment(i)->HasObject = false;
			}
		}
	}



	// записываем результат в лист.
	m_objects.Clear();
	for (int i = 0; i < m_scanner->GetFragmentsCount(); i++)
	{
		if (m_scanner->GetFragment(i)->HasObject)
		{
			awpRect rect = m_scanner->GetFragmentRect(i);
			UUID id;
			LF_NULL_UUID_CREATE(id);
			TLFDetectedItem* de = new TLFDetectedItem(&rect, 0, this->m_Type, this->m_Angle,
			 this->m_racurs, this->m_baseWidth, this->m_baseHeight, this->m_strDetName.c_str(), id);
			de->SetHasObject(true);
			de->SetRaiting(m_scanner->GetFragment(i)->raiting);
			m_objects.Add(de);
			res++;
		}
		m_scanner->GetFragment(i)->HasObject = false;
	}

	return m_objects.GetCount();
}


// properties
double TSCObjectDetector::GetThreshold()
{
	return 0;
}
void TSCObjectDetector::SetThreshold(double Value)
{

}

// xml support
TiXmlElement* TSCObjectDetector::SaveXML()
{
    TiXmlElement* node1 = new TiXmlElement(this->GetName());
	if (node1 == NULL)
	 return NULL;

  node1->SetAttribute("base_width",  this->m_baseWidth);
  node1->SetAttribute("base_height", this->m_baseHeight);
  node1->SetAttribute("object_type", this->m_Type.c_str());
  node1->SetAttribute("detector", this->m_strDetName.c_str());
  
  // получаем элемент с параметрами сканера.
  // элемент уже готов к записи, так как содержит в себе все что необходимо.
  if (m_scanner != NULL)
  {
	  TiXmlElement* e = m_scanner->SaveXML();
	  node1->LinkEndChild(e);
  }
  else
  {
	  delete node1;
	  return NULL;
  }

  for (int i = 0; i < m_Strongs.GetCount(); i++)
  {
    TCSStrong* c = (TCSStrong*)m_Strongs.Get(i);
    c->SaveXML(node1);
  }
    return node1;
}
bool          TSCObjectDetector::LoadXML(TiXmlElement* parent)
{
    if (parent == NULL)
        return false;
    const char* name  = parent->Value();
	if (strcmp(name, this->GetName() ) != 0)
        return false;
    
	m_Strongs.Clear();

	parent->QueryIntAttribute("base_width", &this->m_baseWidth);
	parent->QueryIntAttribute("base_height", &this->m_baseHeight);
	int value = -1;
	parent->QueryIntAttribute("object_type", &value);
	this->m_Type = parent->Attribute("object_type");
	this->m_strDetName = parent->Attribute("detector");

    TiXmlHandle hRoot = TiXmlHandle(parent);
    TiXmlElement* elem = hRoot.Child(0).ToElement();
	/*
	if (m_scanner == NULL || !this->m_scanner->LoadXML(parent->FirstChildElement()))
		return false;
	m_scanner->SetBaseHeight(m_baseHeight);
	m_scanner->SetBaseWidth(m_baseWidth);
	*/
	if (m_scanner != NULL)
		delete m_scanner;
	m_scanner = CreateScanner(parent->FirstChildElement());
	if (m_scanner == NULL)
		return false;
	TLFString scannerName = m_scanner->GetName();
	printf("scanner name = %s\n", scannerName.c_str());
	if (scannerName == "TLFScanner")
	{
		m_scanner->SetBaseHeight(m_baseHeight);
		m_scanner->SetBaseWidth(m_baseWidth);
	}

    int count = 0;
    for(TiXmlNode* child = parent->FirstChild("CSClassificator"); child; child = child->NextSibling() )
    {
        name = child->Value();
        if (strcmp(name, "CSClassificator") != 0)
            return false;

		TCSStrong* pHc = new TCSStrong();
		if (pHc->LoadXML(child->ToElement()))
		{
			m_Strongs.Add(pHc);
		}
		else
		{
			delete pHc;
			return false;
		}
        count++;
    }

    return true;
}


bool TSCObjectDetector::SaveDetector(const char* lpFileName)
{
	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
	doc.LinkEndChild(decl);

	TiXmlElement* e = SaveXML();
	doc.LinkEndChild(e);
	return doc.SaveFile(lpFileName);
}
bool TSCObjectDetector::LoadDetector(const char* lpFileName)
{
	Clear();

	TiXmlDocument doc;
	if (!doc.LoadFile(lpFileName))
		return false;
	TiXmlElement* e = doc.FirstChildElement("TSCObjectDetector");
	if (e == NULL)
		return false;
	return LoadXML(e);
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TLFFGBGDetector::TLFFGBGDetector()
{
	m_counter = 0;
	if (m_scanner != NULL)
		delete m_scanner;
	m_scanner = new TLFTileScanner();
	m_scanner->GetParameter(0)->SetValue(50);
	m_threshold = 5;
	m_buf_size = 5;
    m_bg_stability = 1;
    m_delay = 0;
}
TLFFGBGDetector::~TLFFGBGDetector()
{
	this->m_weaks_a.Clear();
	this->m_weaks_s.Clear();
	this->m_weaks_h.Clear();
	this->m_weaks_v.Clear();
	this->m_weaks_d.Clear();
}

TiXmlElement* TLFFGBGDetector::SaveXML()
{
	return NULL;
}
bool          TLFFGBGDetector::LoadXML(TiXmlElement* parent)
{
	return false;
}

bool TLFFGBGDetector::Init(awpImage* pImage, bool scan)
{
	if (pImage == NULL || pImage->dwType != AWP_BYTE || m_scanner == NULL)
		return false;
	bool changed = m_Image.GetImage() == NULL || m_Image.GetImage()->sSizeX != pImage->sSizeX || m_Image.GetImage()->sSizeY != pImage->sSizeY || m_scanner->GetFragmentsCount() == 0;
	m_Image.SetImage(pImage);
	if (changed)
	{
		m_counter = 0;
		m_objects.Clear();
		this->m_weaks_a.Clear();
		this->m_weaks_s.Clear();
		this->m_weaks_h.Clear();
		this->m_weaks_v.Clear();
		this->m_weaks_d.Clear();
		printf("treshold = %f\n", m_threshold);
		m_scanner->ScanImage(&m_Image);

		for (int i = 0; i < m_scanner->GetFragmentsCount(); i++)
		{
			awpRect rect = m_scanner->GetFragmentRect(i);
            UUID id;

			ILFFeature* feature = new TLFAFeature(rect.left, rect.top, (rect.right - rect.left), (rect.bottom - rect.top));
			TLFAccWeak* weak = new TLFAccWeak(feature, m_threshold);
			weak->SetBufSize(this->m_buf_size);
			weak->SetBgStability(this->m_bg_stability);
            weak->SetDelay(this->m_delay);
			m_weaks_a.Add(weak);
			delete feature;

			feature = new TLFSFeature(rect.left, rect.top, (rect.right - rect.left), (rect.bottom - rect.top));
			weak = new TLFAccWeak(feature,m_threshold);
			weak->SetBufSize(this->m_buf_size);
			weak->SetBgStability(this->m_bg_stability);
            weak->SetDelay(this->m_delay);
			m_weaks_s.Add(weak);
			delete feature;

			feature = new TLFHFeature(rect.left, rect.top, (rect.right - rect.left), (rect.bottom - rect.top)/2);
			 weak = new TLFAccWeak(feature, m_threshold);
			weak->SetBufSize(this->m_buf_size);
			weak->SetBgStability(this->m_bg_stability);
            weak->SetDelay(this->m_delay);
			m_weaks_h.Add(weak);
			delete feature;

			feature = new TLFVFeature(rect.left, rect.top, (rect.right - rect.left)/2, (rect.bottom - rect.top));
			weak = new TLFAccWeak(feature, m_threshold);
			weak->SetBufSize(this->m_buf_size);
			weak->SetBgStability(this->m_bg_stability);
            weak->SetDelay(this->m_delay);
			m_weaks_v.Add(weak);
			delete feature;

			feature = new TLFDFeature(rect.left, rect.top, (rect.right - rect.left)/2, (rect.bottom - rect.top)/2);
			weak = new TLFAccWeak(feature, m_threshold);
			weak->SetBufSize(this->m_buf_size);
			weak->SetBgStability(this->m_bg_stability);
            weak->SetDelay(this->m_delay);
			m_weaks_d.Add(weak);
			delete feature;

			LF_NULL_UUID_CREATE(id);

			TLFDetectedItem* de = new TLFDetectedItem(&rect, 0, "Foreground", 0, 0,
			this->m_baseWidth, this->m_baseHeight, this->m_strDetName.c_str(), id);
			de->SetHasObject(false);
			m_objects.Add(de);
		}
	}
	m_counter++;
	return m_Image.GetImage() != NULL;
}
// classification
int  TLFFGBGDetector::ClassifyRect(awpRect Fragmnet, double* err, int* vect)
{
	if (m_Image.GetImage() == NULL || m_Image.GetIntegralImage() == NULL)
		return 0;


	int w = Fragmnet.right - Fragmnet.left;
	int h = Fragmnet.bottom - Fragmnet.top;   
	double s = w*h;
	double value = m_Image.CalcLnSum(Fragmnet.left, Fragmnet.top, w, h);
	value /= s;
	double v1 = err[0] / (double)this->m_counter;
	*err += value;

	int result = /*m_counter > 100 &&*/ fabs(value - v1) > 15 ? 1 : 0;
	return result;
}
int  TLFFGBGDetector::Detect()
{
	for (int i = 0; i < m_objects.GetCount(); i++)
	{
		TLFDetectedItem* di = (TLFDetectedItem*)m_objects.Get(i);

		if (di != NULL)
		{
		   	di->SetHasObject(false);
			TLFRect* rect = di->GetBounds();
			if (rect != NULL)
			{ 
				double e = di->GetRaiting();
				int va,vs,vh,vv,vd;
			   	ILFWeak* weak_a = (ILFWeak*)this->m_weaks_a.Get(i);
				ILFWeak* weak_s = (ILFWeak*)this->m_weaks_s.Get(i);
				ILFWeak* weak_h = (ILFWeak*)this->m_weaks_h.Get(i);
				ILFWeak* weak_v = (ILFWeak*)this->m_weaks_v.Get(i);
				ILFWeak* weak_d = (ILFWeak*)this->m_weaks_d.Get(i);

			 	va = weak_a->Classify(&this->m_Image);
		   		vs = weak_s->Classify(&this->m_Image);
		   		vh = weak_h->Classify(&this->m_Image);
		   		vv = weak_v->Classify(&this->m_Image);
		   		vd = weak_d->Classify(&this->m_Image);

				di->SetHasObject(vv + vh  + vd > 0 && va + vs > 0);
                //di->SetHasObject(false);
			//	di->SetHasObject(0.2*vv + 0.2*vh + 0.2*vd + 0.2*va + 0.2*vs > 0.5);
				di->SetRaiting(0);
			}
		}
	}
	return 0;
}
// properties
int TLFFGBGDetector::GetStagesCount()
{
	return 1;
}
bool  TLFFGBGDetector::AddStrong(ILFStrong* strong)
{
	return true;
}
double TLFFGBGDetector::GetThreshold()
{
	return 1 - this->m_threshold / 15.;
}
void TLFFGBGDetector::SetThreshold(double Value)
{

	if (Value < 0 || Value > 1)
		return;
	m_threshold = 15*(1 - Value);
	Value = m_threshold;
	TLFAccWeak* weak = NULL;
	for (int i = 0; i < m_weaks_a.GetCount(); i++)
	{
		weak = (TLFAccWeak*)this->m_weaks_a.Get(i);
		if (weak != NULL)
			weak->SetThreshold(Value);
		weak = (TLFAccWeak*)this->m_weaks_s.Get(i);
		if (weak != NULL)
			weak->SetThreshold(Value);
		weak = (TLFAccWeak*)this->m_weaks_h.Get(i);
		if (weak != NULL)
			weak->SetThreshold(Value);
		weak = (TLFAccWeak*)this->m_weaks_v.Get(i);
		if (weak != NULL)
			weak->SetThreshold(Value);
		weak = (TLFAccWeak*)this->m_weaks_d.Get(i);
		if (weak != NULL)
			weak->SetThreshold(Value);
	}
}

int TLFFGBGDetector::GetAverageBufferSize()
{
	return this->m_buf_size;
}
void TLFFGBGDetector::SetAverageBufferSize(int value)
{
	TLFAccWeak* weak = NULL;
	this->m_buf_size = value;
	for (int i = 0; i < m_weaks_a.GetCount(); i++)
	{
		weak = (TLFAccWeak*)this->m_weaks_a.Get(i);
		if (weak != NULL)
			weak->SetBufSize(value);
		weak = (TLFAccWeak*)this->m_weaks_s.Get(i);
		if (weak != NULL)
			weak->SetBufSize(value);
		weak = (TLFAccWeak*)this->m_weaks_h.Get(i);
		if (weak != NULL)
			weak->SetBufSize(value);
		weak = (TLFAccWeak*)this->m_weaks_v.Get(i);
		if (weak != NULL)
			weak->SetBufSize(value);
		weak = (TLFAccWeak*)this->m_weaks_d.Get(i);
		if (weak != NULL)
			weak->SetBufSize(value);
	}

}

int TLFFGBGDetector::GetBgStability()
{
   TLFAccWeak* weak = NULL;
   weak = (TLFAccWeak*)this->m_weaks_a.Get(0);
   if (weak != NULL)
   {
	 return weak->GetBgStability();
   }
   else
   	 return 0;
}

void TLFFGBGDetector::SetBgStability(int value)
{
	TLFAccWeak* weak = NULL;
    this->m_bg_stability = value;
	for (int i = 0; i < m_weaks_a.GetCount(); i++)
	{
		weak = (TLFAccWeak*)this->m_weaks_a.Get(i);
		if (weak != NULL)
			weak->SetBgStability(value);
		weak = (TLFAccWeak*)this->m_weaks_s.Get(i);
		if (weak != NULL)
			weak->SetBgStability(value);
		weak = (TLFAccWeak*)this->m_weaks_h.Get(i);
		if (weak != NULL)
			weak->SetBgStability(value);
		weak = (TLFAccWeak*)this->m_weaks_v.Get(i);
		if (weak != NULL)
			weak->SetBgStability(value);
		weak = (TLFAccWeak*)this->m_weaks_d.Get(i);
		if (weak != NULL)
			weak->SetBgStability(value);
	}
}

int TLFFGBGDetector::GetDelay()
{
   TLFAccWeak* weak = NULL;
   weak = (TLFAccWeak*)this->m_weaks_a.Get(0);
   if (weak != NULL)
   {
	 return weak->GetDelay();
   }
   else
   	 return this->m_delay;
}

void TLFFGBGDetector::SetDelay(int value)
{
	TLFAccWeak* weak = NULL;
    this->m_delay = value;
	for (int i = 0; i < m_weaks_a.GetCount(); i++)
	{
		weak = (TLFAccWeak*)this->m_weaks_a.Get(i);
		if (weak != NULL)
			weak->SetDelay(value);
		weak = (TLFAccWeak*)this->m_weaks_s.Get(i);
		if (weak != NULL)
			weak->SetDelay(value);
		weak = (TLFAccWeak*)this->m_weaks_h.Get(i);
		if (weak != NULL)
			weak->SetDelay(value);
		weak = (TLFAccWeak*)this->m_weaks_v.Get(i);
		if (weak != NULL)
			weak->SetDelay(value);
		weak = (TLFAccWeak*)this->m_weaks_d.Get(i);
		if (weak != NULL)
			weak->SetDelay(value);
	}
}

/////////////////////////////////////////////////////////////////////////////////
TLFSmokeDetector::TLFSmokeDetector()
{
	m_baseWidth = 24;
	m_baseHeight = 24;
	m_Type = "Smoke";
	m_Angle = 0;
	m_racurs = 0;
	m_scanner = new TLFTileScanner();
	m_strDetName = "TLFSmokeDetector";
	m_threshold = 10;
}
TLFSmokeDetector::~TLFSmokeDetector()
{

}

// init detector with image
bool TLFSmokeDetector::Init(awpImage* pImage, bool scan)
{
	if (pImage == NULL || pImage->dwType != AWP_BYTE || m_scanner == NULL)
		return false;
	bool changed = m_Image.GetImage() == NULL || m_Image.GetImage()->sSizeX != pImage->sSizeX || m_Image.GetImage()->sSizeY != pImage->sSizeY || m_scanner->GetFragmentsCount() == 0;
	m_Image.SetImage(pImage);

	if (changed)
	{
		this->m_weaks_s.Clear();
		this->m_weaks_a.Clear();
		this->m_objects.Clear();
		this->m_scanner->Scan(pImage->sSizeX, pImage->sSizeY);
		for (int i = 0; i < m_scanner->GetFragmentsCount(); i++)
		{
			awpRect rect = m_scanner->GetFragmentRect(i);
			ILFFeature* feature = new TLFSFeature(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
			TLFDynamicWeak* weak = new TLFDynamicWeak(feature, 10, 1);
			weak->SetT1(15);
			weak->SetT2(25);
			weak->Setmethod(HYST_METHOD_LESS);
			m_weaks_s.Add(weak);

			feature = new TLFAFeature(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
			weak = new TLFDynamicWeak(feature, 10, 1);
			weak->SetT1(32);
			weak->SetT2(10);
			weak->Setmethod(HYST_METHOD_MORE);
			m_weaks_a.Add(weak);

            UUID id;

			LF_NULL_UUID_CREATE(id);
            TLFDetectedItem* de = new TLFDetectedItem(&rect, 0, "smoke", 0, 0,
            this->m_baseWidth, this->m_baseHeight, this->m_strDetName.c_str(), id);
			de->SetHasObject(false);
			m_objects.Add(de);
		}
	}
	return m_Image.GetImage() != NULL;
}
// classification
int  TLFSmokeDetector::ClassifyRect(awpRect Fragmnet, double* err, int* vect)
{
	return 0;
}
int  TLFSmokeDetector::Detect()
{
	for (int i = 0; i < m_objects.GetCount(); i++)
	{
		TLFDetectedItem* di = (TLFDetectedItem*)m_objects.Get(i);

		if (di != NULL)
		{
			di->SetHasObject(false);
			TLFRect* rect = di->GetBounds();
			if (rect != NULL)
			{
				double e = di->GetRaiting();
				int v = 0;
				ILFWeak* weak_s = (ILFWeak*)this->m_weaks_s.Get(i);
				ILFWeak* weak_a = (ILFWeak*)this->m_weaks_a.Get(i);

				int v1 = weak_s->Classify(&this->m_Image);
				int v2 = weak_a->Classify(&this->m_Image);

				di->SetHasObject(v1 > 0 && v2 > 0);
				di->SetRaiting(e);
			}
		}
	}
	return 0;
}
int	  TLFSmokeDetector::GetStagesCount()
{
	return 1;
}
bool	TLFSmokeDetector::AddStrong(ILFStrong* strong)
{
	return true;
}
double TLFSmokeDetector::GetThreshold()
{
	return this->m_threshold;
}
void TLFSmokeDetector::SetThreshold(double Value)
{
	this->m_threshold = Value;
}

TiXmlElement* TLFSmokeDetector::SaveXML()
{
	return NULL;
}
bool          TLFSmokeDetector::LoadXML(TiXmlElement* parent)
{
	return false;
}

TLFFireDetector::TLFFireDetector()
{
	m_baseWidth = 4;
	m_baseHeight = 4;
	m_Type = "Fire";
	m_Angle = 0;
	m_racurs = 0;
	m_scanner = new TLFTileScanner();
	m_scanner->SetBaseWidth(m_baseWidth);
	m_scanner->SetBaseHeight(m_baseHeight);
	m_strDetName = "TLFFireDetector";
	m_threshold = 10;
#ifdef _DEBUG
	m_log = fopen("fire.log", "a+t");
#endif 
}
TLFFireDetector::~TLFFireDetector()
{
#ifdef _DEBUG
	fclose(m_log);
#endif 

}

// init detector with image
bool TLFFireDetector::Init(awpImage* pImage, bool scan)
{
	if (pImage == NULL || pImage->dwType != AWP_BYTE || m_scanner == NULL)
		return false;
	bool changed = m_Image.GetImage() == NULL || m_Image.GetImage()->sSizeX != pImage->sSizeX || m_Image.GetImage()->sSizeY != pImage->sSizeY || m_scanner->GetFragmentsCount() == 0;

	awpImage* tmp = NULL;
	awpGetChannel(pImage, &tmp, 2);
	m_Image.SetImage(tmp);
	awpReleaseImage(&tmp);

	if (changed)
	{
		this->m_weaks_a.Clear();
		this->m_objects.Clear();
		this->m_scanner->Scan(pImage->sSizeX, pImage->sSizeY);
		for (int i = 0; i < m_scanner->GetFragmentsCount(); i++)
		{
			awpRect rect = m_scanner->GetFragmentRect(i);
			ILFFeature* feature = new TLFAFeature(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
			TLFHysteresisWeak* weak = new TLFHysteresisWeak(feature);
			weak->SetT1(80);
			weak->SetT2(105);
			weak->Setmethod(HYST_METHOD_MORE);
			if (feature->uCalcValue(&m_Image) < 80)
			{
				m_weaks_a.Add(weak);

				UUID id;

				LF_NULL_UUID_CREATE(id);
				TLFDetectedItem* de = new TLFDetectedItem(&rect, 0, "Fire", 0, 0,
					this->m_baseWidth, this->m_baseHeight, this->m_strDetName.c_str(), id);
				de->SetHasObject(false);
				m_objects.Add(de);
			}
		}
	}
	else
	{

	}
	return m_Image.GetImage() != NULL;

}
// classification
int  TLFFireDetector::ClassifyRect(awpRect Fragmnet, double* err, int* vect)
{
	return 0;
}
int  TLFFireDetector::Detect()
{
	for (int i = 0; i < m_objects.GetCount(); i++)
	{
		TLFDetectedItem* di = (TLFDetectedItem*)m_objects.Get(i);

		if (di != NULL)
		{
			bool old_has_object = di->HasObject();
			di->SetHasObject(false);
			TLFRect* rect = di->GetBounds();
			if (rect != NULL)
			{
				double e = di->GetRaiting();
				int v = 0;
				ILFWeak* weak_a = (ILFWeak*)this->m_weaks_a.Get(i);
				int v2 = weak_a->Classify(&this->m_Image);
#ifdef _DEBUG
				if(!old_has_object && v2>0)
				{ 
					//save data to log
					TLFRingBuffer* rb = ((TLFHysteresisWeak*)weak_a)->GetBuffer();
					for (int k = 0; k < rb->GetSize(); k++)
					{
						fprintf(m_log, "%f\t", rb->GetValue(k)); 
					}
					fprintf(m_log, "\n");
				}
#endif
				TLFRingBuffer* rb = ((TLFHysteresisWeak*)weak_a)->GetBuffer();

				di->SetHasObject(v2 >0);
			}
		}
	}
	return 0;
}
// properties
bool	TLFFireDetector::AddStrong(ILFStrong* strong)
{
	return false;
}
double TLFFireDetector::GetThreshold()
{
	return m_threshold;
}
void TLFFireDetector::SetThreshold(double Value)
{
	this->m_threshold = Value;
}
// xml support
TiXmlElement* TLFFireDetector::SaveXML()
{
	return NULL;
}
bool          TLFFireDetector::LoadXML(TiXmlElement* parent)
{
	return false;
}
