﻿/*M///////////////////////////////////////////////////////////////////////////////////////
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

#include "LFDetector.h"
#include "LFScanners.h"
#include "LFStrong.h"
#include "LFWeak.h"
#include "LFFileUtils.h"
#include "LFParameter.h"

//#include <thread>

#ifdef _OMP_
#include <omp.h>
#endif

#include <mutex>
#include <numeric>


//using namespace std;
#ifndef __CLR_OR_THIS_CALL
	#define __CLR_OR_THIS_CALL __cdecl
#endif
#if defined(WIN32) || defined(_WIN64)
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

TiXmlElement* ILFObjectDetector::TDescription::SaveXML() const
{
	TiXmlElement* sample_desc = new TiXmlElement("LFSampleDescriptor");
	sample_desc->SetAttribute("Result", result);
	sample_desc->SetDoubleAttribute("Score", score);

	for (const auto& strong : descs) {
		TiXmlElement* stage_desc = new TiXmlElement("LFStageDescriptor");
		stage_desc->SetDoubleAttribute("Score", strong.score);
		stage_desc->SetAttribute("Result", strong.result);

		for (const auto& weak : strong.weaks) {

			TiXmlElement* weak_desc = new TiXmlElement("LFWeakDescriptor");
			weak_desc->SetAttribute("Result", weak.result);

			TiXmlElement* features_desc = new TiXmlElement("LFFeatureDescriptor");
			const auto& feature = weak.feature;
			features_desc->SetAttribute("Size", feature.features.size());
			features_desc->SetDoubleAttribute("Value", feature.value);

			if (feature.parent) {
				auto rect = feature.parent->GetRect();
				features_desc->SetAttribute("RectLeft", rect.Left());
				features_desc->SetAttribute("RectTop", rect.Top());
				features_desc->SetAttribute("RectRight", rect.Right());
				features_desc->SetAttribute("RectBottom", rect.Bottom());
			}
						

			if (!weak.feature.features.empty()) {
				auto data = std::accumulate(
					std::next(weak.feature.features.begin()),
					weak.feature.features.end(),
					std::to_string(weak.feature.features.front()),
					[](const auto& a, const  auto& b) {
						return a + " " + std::to_string(b);
					}
				);

				TiXmlText* features_data = new TiXmlText(data);
				features_desc->LinkEndChild(features_data);
			}

			weak_desc->LinkEndChild(features_desc);
			stage_desc->LinkEndChild(weak_desc);
		}
		sample_desc->LinkEndChild(stage_desc);

	}
	return sample_desc;
}


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

const std::string& ILFObjectDetector::GetObjectType() const
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
#if defined(WIN32) || defined(_WIN64)
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
bool TSCObjectDetector::Init(TLFImage* pImage, awpRect* rect)
{
	if (pImage == NULL || pImage->GetImage()->dwType != AWP_BYTE || m_scanner == NULL)
		return false;
	bool changed = m_Image.GetImage() == NULL || m_Image.width() != pImage->width() || m_Image.height() != pImage->height() || m_scanner->GetFragmentsCount() == 0;
    m_Image.FreeImages();
	m_Image = *pImage;
	if (changed)
		if (rect)
			m_scanner->ScanRect(*rect);
		else
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
int  TSCObjectDetector::ClassifyRect(awpRect fragment)
{
	double score = FLT_MAX;
	double scale_x = (fragment.right - fragment.left) / double(m_baseWidth);
	double scale_y = (fragment.bottom - fragment.top) / double(m_baseHeight);
	double scale = std::min<double>(scale_x, scale_y);
	bool has_object = true;

	TDescription		description;

	if (desc_callback_) {
		description.descs.reserve(m_Strongs.GetCount());
	}

	TLFAlignedTransform transform = TLFAlignedTransform(scale, scale, fragment.left, fragment.top);
	for (int j = 0; j < m_Strongs.GetCount(); j++)
    {
				
		TCSStrong* strong = (TCSStrong*)m_Strongs.Get(j);
		        
		auto desc = strong->Classify(&m_Image, transform);
		
		if (desc.result != 0) {
			score = std::min<double>(desc.score, score);
		} else {
			has_object = false;
			score = desc.score;
		}

		if (desc_callback_) {
			//Gather
			description.descs.emplace_back(std::move(desc));
		}
		else if (!has_object) {
			//Stop searching object;
			break;
		}

    }

	description.score = score;
	description.result = has_object;

	if (desc_callback_) {
		desc_callback_(0, TLFBounds{0, 0, fragment}, description);
	}

    return has_object;

}
// возвращает число найденных объектов. 
int  TSCObjectDetector::Detect()
{
	
	if (this->m_scanner == NULL)
		return -1;
	if (this->m_Image.GetImage() == NULL)
		return -1;

	m_objects.Clear();
			
#ifdef _OMP_
#pragma omp parallel for num_threads(omp_get_max_threads())
#endif 
	for (int i = 0; i < m_scanner->GetFragmentsCount(); i++)
	{		
		TDescription		description;

		if (desc_callback_) {
			description.descs.reserve(m_Strongs.GetCount());
		}

		bool has_object = true;
		awpRect rect = m_scanner->GetFragmentRect(i);

		double scale_x = (rect.right - rect.left) / double(this->m_baseWidth);
		double scale_y = (rect.bottom - rect.top) / double(this->m_baseHeight);
		double scale = std::min<double>(scale_x, scale_y);

		TLFAlignedTransform transform(scale, scale, rect.left, rect.top);
		double score = FLT_MAX;

		for (int j = 0; j < m_Strongs.GetCount(); j++)
		{
			ILFStrong* strong = static_cast<ILFStrong*>(m_Strongs.Get(j));
			
			auto desc = strong->Classify(&m_Image, transform);

			//Check If object detected;
			if (desc.result != 0) {
				score = std::min<double>(desc.score, score);
			}
			else {
				has_object = false;
				score = desc.score;
			}

			if (desc_callback_) {
				//Gather
				description.descs.emplace_back(std::move(desc));
			}
			else if (!has_object) {
				//Stop searching object;
				break;
			}
		}

		description.score = score;
		description.result = has_object;


		if (desc_callback_) {
			desc_callback_(i, *m_scanner->GetFragment(i), description);
		}

		if (has_object)
		{
			//object detected
			UUID id;
			LF_NULL_UUID_CREATE(id);
			// записываем результат в лист.
			TLFDetectedItem* de = new TLFDetectedItem(rect, score, this->m_Type, this->m_Angle,
				this->m_racurs, this->m_baseWidth, this->m_baseHeight, this->m_strDetName.c_str(), id);
			de->SetHasObject(true);

#ifdef _OMP_
#pragma omp critical 
#endif
			{
				m_objects.Add(de);
			}
		}

	}

	return m_objects.GetCount();
}

int TSCObjectDetector::DetectInRect(awpRect roi)
{

	
	if (this->m_scanner == NULL)
		return -1;
	if (this->m_Image.GetImage() == NULL)
		return -1;

	m_objects.Clear();
	
#ifdef _OMP_
#pragma omp parallel for num_threads(omp_get_max_threads())
#endif
	for (int i = 0; i < m_scanner->GetFragmentsCount(); i++)
	{
		awpRect fr = m_scanner->GetFragmentRect(i);
		awpPoint c;
		c.X = (fr.left + fr.right) / 2;
		c.Y = (fr.top + fr.bottom) / 2;
		if (c.X >= roi.left && c.X <= roi.right &&
		c.Y >= roi.top && c.Y <= roi.bottom)
		{
			TDescription		description;

			if (desc_callback_) {
				description.descs.reserve(m_Strongs.GetCount());
			}

			bool has_object = true;
			awpRect rect = m_scanner->GetFragmentRect(i);

			double scale_x = (rect.right - rect.left) / double(this->m_baseWidth);
			double scale_y = (rect.bottom - rect.top) / double(this->m_baseHeight);

			double scale = std::min<double>(scale_x, scale_y);

			TLFAlignedTransform transform(scale, scale, rect.left, rect.top);

			double score = FLT_MAX;
			for (int j = 0; j < m_Strongs.GetCount(); j++)
			{
				ILFStrong* strong = static_cast<ILFStrong*>(m_Strongs.Get(j));

				auto desc = strong->Classify(&m_Image, transform);

				//Check If object detected;
				if (desc.result != 0) {
					score = std::min<double>(desc.score, score);
				}
				else {
					has_object = false;
					score = desc.score;
				}

				if (desc_callback_) {
					//Gather
					description.descs.emplace_back(std::move(desc));
				}
				else if (!has_object) {
					//Stop searching object;
					break;
				}
			}

			description.score = score;
			description.result = has_object;

			if (desc_callback_) {
				desc_callback_(i, *m_scanner->GetFragment(i), description);
			}

			if (has_object)
			{
				//object detected
				UUID id;
				LF_NULL_UUID_CREATE(id);
				TLFDetectedItem* de = new TLFDetectedItem(rect, score, this->m_Type, this->m_Angle,
					this->m_racurs, this->m_baseWidth, this->m_baseHeight, this->m_strDetName.c_str(), id);
				de->SetHasObject(true);

#ifdef _OMP_
				#pragma omp critical 
#endif
				{
					m_objects.Add(de);
				}
				
			}
		}
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

bool TLFFGBGDetector::Init(TLFImage* pImage, awpRect* pRect)
{
	if (pImage == NULL || pImage->GetImage()->dwType != AWP_BYTE || m_scanner == NULL)
		return false;
	bool changed = m_Image.GetImage() == NULL || m_Image.width() != pImage->width() || m_Image.height() != pImage->height() || m_scanner->GetFragmentsCount() == 0;
	m_Image = *pImage;
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

			TLFDetectedItem* de = new TLFDetectedItem(rect, 0, "Foreground", 0, 0,
			this->m_baseWidth, this->m_baseHeight, this->m_strDetName.c_str(), id);
			de->SetHasObject(false);
			m_objects.Add(de);
		}
	}
	m_counter++;
	return m_Image.GetImage() != NULL;
}

// classification
int  TLFFGBGDetector::ClassifyRect(awpRect Fragmnet)
{
	if (m_Image.GetImage() == NULL || m_Image.GetIntegralImage() == NULL)
		return 0;

	int w = Fragmnet.right - Fragmnet.left;
	int h = Fragmnet.bottom - Fragmnet.top;   
	double s = w*h;
	double value = m_Image.CalcLnSum(Fragmnet.left, Fragmnet.top, w, h);
	value /= s;
	return -1;
	//TODO: watch this;
	/*
	double v1 = err[0] / (double)this->m_counter;
	*err += value;

	int result = //m_counter > 100 &&
					fabs(value - v1) > 15 ? 1 : 0;
	return result;*/
}

int  TLFFGBGDetector::Detect()
{
	for (int i = 0; i < m_objects.GetCount(); i++)
	{
		TLFDetectedItem* di = (TLFDetectedItem*)m_objects.Get(i);

		if (di != NULL)
		{
		   	di->SetHasObject(false);
			auto square = di->GetBounds().Square();
			if (square > 0)
			{ 
				double e = di->GetRaiting();
				
			   	ILFWeak* weak_a = (ILFWeak*)this->m_weaks_a.Get(i);
				ILFWeak* weak_s = (ILFWeak*)this->m_weaks_s.Get(i);
				ILFWeak* weak_h = (ILFWeak*)this->m_weaks_h.Get(i);
				ILFWeak* weak_v = (ILFWeak*)this->m_weaks_v.Get(i);
				ILFWeak* weak_d = (ILFWeak*)this->m_weaks_d.Get(i);

				//TODO: check it? May be rect?
				TLFAlignedTransform transform(1.0);

			 	auto a = weak_a->Classify(&this->m_Image, transform);
		   		auto s = weak_s->Classify(&this->m_Image, transform);
		   		auto h = weak_h->Classify(&this->m_Image, transform);
		   		auto v = weak_v->Classify(&this->m_Image, transform);
		   		auto d = weak_d->Classify(&this->m_Image, transform);

				di->SetHasObject(v.result + h.result + d.result > 0 && a.result + s.result > 0);
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
