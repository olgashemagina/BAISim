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
//		File: LFSemantic.cpp
//		Purpose: implements TLFSemanticDictinaryItem, TLFSemanticDictinary
//
//      CopyRight 2004-2019 (c) NN-Videolab.net
//M*/
#include "_LF.h"
//--------------------------element dictionary ----------------------------------
TLFSemanticDictinaryItem::TLFSemanticDictinaryItem()
{
	m_strLabel = "Unknown";
	m_scanner = new TLFScanner();
	awpRGBtoWeb(0,255,0, &m_color);
	UUID id;
	LF_UUID_CREATE(id)
	m_id = LFGUIDToString(&id);
	m_zoneType = ZTRect;
	m_needRepairItem = false;
}

TLFSemanticDictinaryItem::TLFSemanticDictinaryItem(const char* lpWord)
{
	this->m_strLabel = lpWord;
	this->m_scanner = new TLFScanner();
	awpRGBtoWeb(0,255,0, &m_color);
	UUID id;
	LF_UUID_CREATE(id)
	m_id = LFGUIDToString(&id);
	m_zoneType = ZTRect;
	m_needRepairItem = false;
}

TLFSemanticDictinaryItem::TLFSemanticDictinaryItem(TLFSemanticDictinaryItem& item)
{
	m_strLabel = item.m_strLabel;
	m_color = item.m_color;
	m_scanner = new TLFScanner();
	m_scanner->SetBaseHeight(item.m_scanner->GetBaseHeight());
	m_scanner->SetBaseWidth(item.m_scanner->GetBaseWidth());
	m_id = item.m_id;
	m_zoneType = item.m_zoneType;
	m_needRepairItem = item.m_needRepairItem;
}

TLFSemanticDictinaryItem::~TLFSemanticDictinaryItem()
{
	delete m_scanner;
}

TLFSemanticDictinaryItem&  TLFSemanticDictinaryItem::operator =(TLFSemanticDictinaryItem& item)
{
	if (this != &item)
	{
		m_strLabel = item.m_strLabel;
		m_color = item.m_color;
		m_scanner = new TLFScanner();
		m_scanner->SetBaseHeight(item.m_scanner->GetBaseHeight());
		m_scanner->SetBaseWidth(item.m_scanner->GetBaseWidth());
		m_id = item.m_id;
		m_zoneType = item.m_zoneType;
	}
	return *this;
}

const char* TLFSemanticDictinaryItem::GetItemLabel()
{
	return m_strLabel.c_str();
}
ILFScanner* TLFSemanticDictinaryItem::GetScanner()
{
	return m_scanner;
}

bool TLFSemanticDictinaryItem::SaveXML(const char* lpFileName)
{
	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
	doc.LinkEndChild(decl);
	TiXmlElement* dscr = SaveXML();
	if (dscr == NULL)
		return false;
    doc.LinkEndChild(dscr);
	return doc.SaveFile(lpFileName);
}

int TLFSemanticDictinaryItem::GetColor()
{
	 return this->m_color;
}
void TLFSemanticDictinaryItem::SetColor(int color)
{
	this->m_color = color;
}

void TLFSemanticDictinaryItem::SetItemLabel(const char* lpLabel)
{
	this->m_strLabel = lpLabel;
}

TLFString TLFSemanticDictinaryItem::GetId()
{
	return m_id;
}

void TLFSemanticDictinaryItem::SetId(const TLFString& id)
{
	m_id = id;
}

TEZoneTypes TLFSemanticDictinaryItem::GetZoneType()
{
	return this->m_zoneType;
}

void TLFSemanticDictinaryItem::SetZoneType(TEZoneTypes zt)
{
	this->m_zoneType = zt;
}


bool TLFSemanticDictinaryItem::GetNeedRepairItem()
{
	return m_needRepairItem;
}
void TLFSemanticDictinaryItem::SetNeedRepairItem(const bool value)
{
    m_needRepairItem = value;
}


////////////////////////////////////////////////////////////////////////////////
bool TLFSemanticDictinaryItem::LoadXML(const char* lpFileName)
{
	return false;
}
////////////////////////////////////////////////////////////////////////////////
TiXmlElement* TLFSemanticDictinaryItem::SaveXML()
{
	TiXmlElement* dscr = new TiXmlElement(GetName());
	dscr->SetAttribute("noun", m_strLabel.c_str());
	dscr->SetAttribute("color", m_color);
	dscr->SetAttribute("id", m_id.c_str());
	dscr->SetAttribute("zoneType", m_zoneType);
	if (m_scanner != NULL)
	{
		TiXmlElement* scanner_dscr = m_scanner->SaveXML();
		dscr->LinkEndChild(scanner_dscr);
	}
	return dscr;
}
bool TLFSemanticDictinaryItem::LoadXML(TiXmlElement* parent)
{
	if (parent == NULL)
		return false;
	if (strcmp(parent->Value(), GetName()) != 0)
		return false;
	this->m_strLabel = parent->Attribute("noun");
	const char* ch = parent->Attribute("color");
	if (ch != NULL)
	{
		parent->QueryIntAttribute("color", &m_color);
	}
	else
		awpRGBtoWeb(0,255,0, &m_color);
	const char* id = parent->Attribute("id");
	if (id != NULL)
		m_id = id;
	else
	{
		UUID id;
		LF_UUID_CREATE(id)
		m_id = LFGUIDToString(&id);
		m_needRepairItem = true;
	}
	if (!this->m_scanner->LoadXML(parent->FirstChildElement()))
		return false;
	const char* zt = parent->Attribute("zoneType");
	if (zt != NULL)
	{
		int value = 0;
		parent->QueryIntAttribute("zoneType", &value);
		m_zoneType = (TEZoneTypes)value;
	}
	else
	{
		m_zoneType = ZTRect;
        m_needRepairItem = true;
	}
	return true;
}
//---------------------------семантической словарь-----------------------------
TLFSemanticDictinary::TLFSemanticDictinary()
{
	this->m_Description = "default dictinary";
}
TLFSemanticDictinary::~TLFSemanticDictinary()
{
}

bool TLFSemanticDictinary::AddWordToDictinary(TLFSemanticDictinaryItem* item)
{
	const char* lpWord = item->GetItemLabel();
	std::string id     = item->GetId();
	// check dublicates
	for (int i = 0; i < this->GetCount(); i++)
	{
		TLFSemanticDictinaryItem* di = this->GetWordFromDictinary(i);
		if (strcmp(lpWord, di->GetItemLabel()) == 0 ||
			id == di->GetId())
		{
			return false;
		};
	}

	Add(item);
	return true;
}
void TLFSemanticDictinary::DelWordForomDictinary(int index)
{
	if (index < 0 || index > GetCount())
		return;
	Delete(index);
}
void TLFSemanticDictinary::DelWordForomDictinary(const char* lpWord)
{
	for (int i = 0; i < this->GetCount(); i++)
	{
		TLFSemanticDictinaryItem* di = this->GetWordFromDictinary(i);
		if (strcmp(lpWord, di->GetItemLabel()) == 0)
		{
			Delete(i);
			break;
		};
	}
}

TLFSemanticDictinaryItem* TLFSemanticDictinary::GetWordFromDictinary(int index)
{
	if (index < 0 || index > GetCount())
		return NULL;
	return (TLFSemanticDictinaryItem*)Get(index);
}

TLFSemanticDictinaryItem* TLFSemanticDictinary::GetWordFromDictinary(const char* lpWord)
{
	for (int i = 0; i < GetCount(); i++)
	{
		TLFSemanticDictinaryItem* di = this->GetWordFromDictinary(i);
		if (strcmp(lpWord, di->GetItemLabel()) == 0)
			return di;
	}
	return NULL;
}

bool TLFSemanticDictinary::SaveXML(const char* lpFileName)
{

	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
	doc.LinkEndChild(decl);
	TiXmlElement* dscr = new TiXmlElement(this->GetName());

	dscr->SetAttribute("Description", this->m_Description.c_str());
	for (int i = 0; i < this->GetCount(); i++)
	{
		TLFSemanticDictinaryItem* di = this->GetWordFromDictinary(i);
		TiXmlElement* e = di->SaveXML();
		if (e != NULL)
			dscr->LinkEndChild(e);
	}
	doc.LinkEndChild(dscr);

	return doc.SaveFile(lpFileName);

}
bool TLFSemanticDictinary::LoadXML(const char* lpFileName)
{
	this->Clear();

	TiXmlDocument doc(lpFileName);
	if (!doc.LoadFile())
		return false;
	TiXmlHandle hDoc(&doc);
	TiXmlElement* pElem;// = NULL;

	pElem = hDoc.FirstChildElement().Element();
	if (pElem == NULL)
		return false;
	const char* lpValue = pElem->Value();
	const char* lpName = GetName();
	if (strcmp(lpValue, lpName) != 0)
		return false;

	//int w, h;
	this->m_Description = pElem->Attribute("Description");


	for (TiXmlNode* child = pElem->FirstChild(); child; child = child->NextSibling())
	{
		TLFSemanticDictinaryItem* di = new TLFSemanticDictinaryItem();
		if (strcmp(child->Value(), di->GetName()) != 0)
		{
			Clear();
			return false;
		}
		if (!di->LoadXML(child->ToElement()))
		{
			Clear();
			return false;
		}
		this->Add(di);
	}

	return true;
}

TiXmlElement* TLFSemanticDictinary::SaveXML()
{
	TiXmlElement* dscr = new TiXmlElement(this->GetName());

	dscr->SetAttribute("Description", this->m_Description.c_str());
	for (int i = 0; i < this->GetCount(); i++)
	{
		TLFSemanticDictinaryItem* di = this->GetWordFromDictinary(i);
		TiXmlElement* e = di->SaveXML();
		if (e != NULL)
			dscr->LinkEndChild(e);
	}
	return dscr;
}

bool TLFSemanticDictinary::LoadXML(TiXmlElement* parent)
{
	TiXmlElement* pElem = parent;

	if (pElem == NULL)
		return false;

	if (strcmp(pElem->Value(), GetName()) != 0)
		return false;

	m_Description = pElem->Attribute("Description");

	for (TiXmlNode* child = pElem->FirstChild(); child; child = child->NextSibling())
	{
		TLFSemanticDictinaryItem* di = new TLFSemanticDictinaryItem();
		if (strcmp(child->Value(), di->GetName()) != 0)
		{
			Clear();
			return false;
		}
		if (!di->LoadXML(child->ToElement()))
		{
			Clear();
			return false;
		}
		this->Add(di);
	}

	return true;
}

void TLFSemanticDictinary::Clear()
{
	this->m_Description = "";
	inherited::Clear();
}

bool TLFSemanticDictinary::CheckItem(TLFSemanticDictinaryItem* item)
{
	const char* lpWord = item->GetItemLabel();
	std::string id     = item->GetId();
	// check dublicates
	for (int i = 0; i < GetCount(); i++)
	{
		TLFSemanticDictinaryItem* di = GetWordFromDictinary(i);
		if (strcmp(lpWord, di->GetItemLabel()) == 0 && id != di->GetId())
		{
			return false;
		};
	}
	return true;
}

std::string TLFSemanticDictinary::GetWordByUUID(const char* uuid)
{
	std::string result = "";
	std::string str_uuid = uuid;
	for (int i = 0; i < GetCount(); i++)
	{
		TLFSemanticDictinaryItem* item = GetWordFromDictinary(i);
		std::string str_item_uuid = item->GetId();
		if (str_item_uuid == str_uuid)
		{
			result = item->GetItemLabel();
			break;
		}
	}
	return result;
}

std::string TLFSemanticDictinary::GetUUIDByWord(const char* label)
{
	std::string _str_label = label;
	std::string result = "";
	for (int i = 0; i < GetCount(); i++)
	{
		TLFSemanticDictinaryItem* item = GetWordFromDictinary(i);
		std::string str_item_label = item->GetItemLabel();
		if (_str_label == str_item_label)
		{
			result = item->GetId();
			break;
		}
	}
	return result;

}



