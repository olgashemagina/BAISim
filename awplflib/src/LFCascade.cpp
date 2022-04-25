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
//		File: LFCascade.cp
//		Purpose: implements TLFCascade class 
//
//      CopyRight 2004-2018 (c) NN-Videolab.net
//M*/
#include "_LF.h"

TLFCascade::TLFCascade()
{
}
TLFCascade::~TLFCascade()
{

}

ILFStrong* TLFCascade::GetStrong(int index)
{
	if (index < 0 || index >= this->GetCount())
		return NULL;
	return dynamic_cast <ILFStrong*>(Get(index));
}
// todo: make a copy of strong and add it. 
void       TLFCascade::AddStrong(ILFStrong* strong)
{
	Add(strong);
}
// shift and scale all data in the cascade
void TLFCascade::Setup(double scale_x, double scale_y, AWPWORD dx, AWPWORD dy)
{
	ILFStrong** cascade = (ILFStrong**)GetList();
	int count = this->GetCount();
	for (int i = 0; i < count; i++)
		cascade[i]->Setup(scale_x, scale_y, dx, dy);
}

int TLFCascade::Classify(TLFImage* pImage, double& err, double* vector)
{
	if (pImage == NULL)
		return 0;
	err = 0;
	double error = 0;
	ILFStrong** cascade = (ILFStrong**)GetList();
	int count = this->GetCount();
	int result = 0;
	for (int i = 0; i < count; i++)
	{
		result = cascade[i]->Classify(pImage, error);
		err += error;
		if (vector != NULL)
			vector[i] = error;
		if (result == 0)
			return 0;
	}
	return 1;
}

/*
XML io operations
*/
void TLFCascade::SaveXML(TiXmlElement* parent)
{
	if (parent == NULL)
		return;
	TiXmlElement* element = new TiXmlElement(GetName());
	if (element == NULL)
		return;
	for (int i = 0; i < GetCount(); i++)
	{
		ILFStrong* strong = this->GetStrong(i);
		if (strong != NULL)
			strong->SaveXML(element);
	}
	parent->LinkEndChild(element);
}
bool TLFCascade::LoadXML(TiXmlElement* parent)
{
	if (parent == NULL)
		return false;
	const char* name = parent->Value();
	if (strcmp(name, GetName()) != 0)
		return false;
	for (TiXmlNode* child = parent->FirstChild(); child; child = child->NextSibling())
	{
		//
		ILFStrong* strong = LFCreateStrong(child->ToElement()->Value());
		if (strong == NULL)
			return false;
		if (!strong->LoadXML(child->ToElement()))
		{
			delete strong;
			return false;
		}
		this->AddStrong(strong);
	}
	return true;
}

