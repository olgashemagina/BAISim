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
//		File: LFDatabase.cpp
//		Purpose: implementation TLFDBLabeledImages
//
//      CopyRight 2004-1018 (c) NN-Videolab.net
//M*/
#include "LFDatabase.h"
#include "LFFileUtils.h"
#include "utils.h"
#include <algorithm>


TLFDBSemanticDescriptor::TLFDBSemanticDescriptor(const std::filesystem::path& filePath) : TLFSemanticImageDescriptor()
{
	m_strImageName = filePath;
	auto strXmlName = m_strImageName.replace_extension(".xml");
	if (std::filesystem::exists(strXmlName))
		this->LoadXML(strXmlName.u8string().c_str());
}
std::filesystem::path TLFDBSemanticDescriptor::GetImageFile()
{
	return this->m_strImageName;
}

TLFDBLabeledImages::TLFDBLabeledImages()
{
	m_progress = NULL;
}

TLFDBLabeledImages::TLFDBLabeledImages(const char* path)
{
	InitDB(path);
}
TLFDBLabeledImages::~TLFDBLabeledImages()
{

}

void TLFDBLabeledImages::SetProgress(TLFProgress progress)
{
	this->m_progress = progress;
}

void TLFDBLabeledImages::ClearDatabase()
{
    int c = m_dataFiles.GetCount();
    for(int i = 0; i < c; i++)
    {
         TLFDBSemanticDescriptor* d = (TLFDBSemanticDescriptor*)m_dataFiles.Get(i);
	 	 auto strXmlName = d->GetImageFile();
         strXmlName = strXmlName.replace_extension(".xml");
         if (std::filesystem::exists(strXmlName))
         {
            std::filesystem::remove(strXmlName);
			if (m_progress != NULL)
            {
                std::string msg = strXmlName.filename().u8string();
				m_progress(msg.c_str(), 100*i/c);
            }
         }
    }
    auto strDictFile = m_strPath/c_lpDictFileName;
	if (std::filesystem::exists(strDictFile))
		std::filesystem::remove(strDictFile);

    Clear();
}


bool TLFDBLabeledImages::LoadDatabase(const std::filesystem::path path)
{
	Clear();
	printf("open database %s\n", path);
	auto strDictinary = path/c_lpDictFileName;
	if (!m_dictinary.LoadXML(strDictinary.u8string().c_str()))
	{
		printf("error: cannot load dictinary \n");
		return false;
	}

	bool repair = false;
	for (int i = 0; i < m_dictinary.GetCount(); i++)
	{
		TLFSemanticDictinaryItem* sdi = (TLFSemanticDictinaryItem*)m_dictinary.Get(i);
		if (sdi->GetNeedRepairItem())
			repair = true;
	}

	auto strPath = path;
	std::vector<std::filesystem::path> filePaths;
	if(!LFGetDirFiles(strPath, filePaths))
		return false;
	if (filePaths.size() == 0)
		return false;
	for (unsigned int i = 0; i < filePaths.size(); i++)
	{
			if (utils::IsImageFile(filePaths[i]))
			{
				TLFDBSemanticDescriptor* d = new TLFDBSemanticDescriptor(filePaths[i]);
				if (repair)
				{
					// repair descriptor
					for (int j = 0; j < d->GetCount(); j++)
					{
						 TLFDetectedItem* di =  d->GetDetectedItem(j);
						 TLFString di_type = di->GetType();
						 for (int k = 0; k < m_dictinary.GetCount(); k++)
						 {
							TLFSemanticDictinaryItem* sdi = (TLFSemanticDictinaryItem*)m_dictinary.Get(k);
							TLFString str_sdi_type = sdi->GetItemLabel();
							if (di_type != "970C3A11-DD73-459B-A741-FB23AA23C883")
							{
								// repair here
								 di->SetType(sdi->GetId().c_str());

							}
						 }
					}


					std::string strXmlName = (filePaths[i].replace_extension(".xml")).u8string();
					if (!d->SaveXML(strXmlName.c_str()))
					{
						return false;
					}
				}
				this->m_dataFiles.Add(d);
			}
	}
	if (repair)
	{
		for (int i = 0; i < m_dictinary.GetCount(); i++)
		{
			TLFSemanticDictinaryItem* sdi = (TLFSemanticDictinaryItem*)m_dictinary.Get(i);
			sdi->SetNeedRepairItem(false);
		}
		m_dictinary.SaveXML(strDictinary.u8string().c_str());
	}
	m_strPath = path;
	return true;
}


bool TLFDBLabeledImages::InitDB(const char* path)
{
	return LoadDatabase(path);
}

void TLFDBLabeledImages::Clear()
{
	this->m_dictinary.Clear();
	this->m_dataFiles.Clear();
	this->m_strPath = "";
}

int TLFDBLabeledImages::GetItemsCount()
{
	int count = 0;
	for (int i = 0; i < m_dataFiles.GetCount(); i++)
	{
		TLFDBSemanticDescriptor* d = (TLFDBSemanticDescriptor*)m_dataFiles.Get(i);
		count += d->GetCount();
	}

	return count;
}

int TLFDBLabeledImages::GetImagesCount()
{
	return m_dataFiles.GetCount();
}

TLFSemanticDictinary*	 TLFDBLabeledImages::GetDictinary()
{
	return &this->m_dictinary;
}

void TLFDBLabeledImages::GetFarHST(TLFDetectEngine& engine, TLFHistogramm& hst, int stage, bool all, double overlap)
{
	ILFObjectDetector* detector = engine.GetDetector();
	if (detector == NULL)
		return;
	if (stage < 0 || stage >= detector->GetStagesCount())
		return;  

	ILFScanner* scanner = detector->GetScanner();
	if (scanner == NULL)
		return;
	TLFObjectList* strongs = detector->GetStrongs();
	if (strongs == NULL)
		return;
	ILFStrong*     classifier = (ILFStrong*)strongs->Get(stage);
	if (classifier == NULL)
		return;

	hst.Setup(32, 0, classifier->GetThreshold() * 2);
	for (int i = 0; i < m_dataFiles.GetCount(); i++)
	{
		TLFDBSemanticDescriptor* d = (TLFDBSemanticDescriptor*)m_dataFiles.Get(i);
		TLFImage img;
		if (!img.LoadFromFile(d->GetImageFile().u8string().c_str()))
			continue;
		engine.SetSourceImage(&img, false);
		TLFImage* img1 = detector->GetImage();
		if (i % 100 == 0)
			printf(">");
		for (int j = 0; j < scanner->GetFragmentsCount(); j++)
		{
			double err = 0;
			double scale;// = 1;
			awpRect rect = scanner->GetFragmentRect(j);
			TLFRect lf_rect(rect);
			if (d->Overlap(lf_rect) < overlap)
			{
				scale = (double)(rect.right - rect.left) / (double)scanner->GetBaseWidth();
				if (!all)
				{					
					/*int result = */classifier->Classify(img1, TLFAlignedTransform(scale, scale, rect.left, rect.top), err);
				}
				else
				{
					for (int k = 0; k <= stage; k++)
					{
						ILFStrong* s = (ILFStrong*)strongs->Get(k);
						
						err = 0;
						if (s->Classify(img1, TLFAlignedTransform(scale, scale, rect.left, rect.top), err) == 0)
						{
							if (k!=stage) 
								err = 0;
							break;
						}
					}
				}
				hst.AddElememt(err);
			}
		}
	}
}
void TLFDBLabeledImages::GetFrrHST(TLFDetectEngine& engine, TLFHistogramm& hst, int stage, bool all, double overlap)
{
	ILFObjectDetector* detector = engine.GetDetector();
	if (detector == NULL)
		return;
	if (stage < 0 || stage >= detector->GetStagesCount())
		return;

	ILFScanner* scanner = detector->GetScanner();
	if (scanner == NULL)
		return;
	TLFObjectList* strongs = detector->GetStrongs();
	if (strongs == NULL)
		return;
	ILFStrong*     classifier = (ILFStrong*)strongs->Get(stage);
	if (classifier == NULL)
		return;

	hst.Setup(32, 0, classifier->GetThreshold() * 2);
	for (int i = 0; i < m_dataFiles.GetCount(); i++)
	{
		TLFDBSemanticDescriptor* d = (TLFDBSemanticDescriptor*)m_dataFiles.Get(i);
		TLFImage img;
		if (!img.LoadFromFile(d->GetImageFile().u8string().c_str()))
			continue;
		engine.SetSourceImage(&img, false);
		TLFImage* img1 = detector->GetImage();
		if (i % 100 == 0)
			printf(">");
		for (int j = 0; j < scanner->GetFragmentsCount(); j++)
		{
			double err = 0;
			double scale;// = 1;
			awpRect rect = scanner->GetFragmentRect(j);
			TLFRect lf_rect(rect);
			if (d->Overlap(lf_rect) > overlap)
			{
				scale = (double)(rect.right - rect.left) / (double)scanner->GetBaseWidth();
				if (!all)
				{					
					/*int result = */classifier->Classify(img1, TLFAlignedTransform(scale, scale, rect.left, rect.top), err);
				}
				else
				{
					for (int k = 0; k <= stage; k++)
					{
						ILFStrong* s = (ILFStrong*)strongs->Get(k);
						err = 0;
						if (s->Classify(img1, TLFAlignedTransform(scale, scale, rect.left, rect.top), err) == 0)
						{
							if (k != stage)
								err = 0;
							break;
						}
					}
				}
				hst.AddElememt(err);
			}
		}
	}
}
void TLFDBLabeledImages::GetFarFrrHST(TLFDetectEngine& engine, TLFHistogramm& far_hst, TLFHistogramm& frr_hst, int stage, bool all, double overlap)
{
	ILFObjectDetector* detector = engine.GetDetector();
	if (detector == NULL)
		return;
	if (stage < 0 || stage >= detector->GetStagesCount())
		return;

	ILFScanner* scanner = detector->GetScanner();
	if (scanner == NULL)
		return;
	TLFObjectList* strongs = detector->GetStrongs();
	if (strongs == NULL)
		return;
	ILFStrong*     classifier = (ILFStrong*)strongs->Get(stage);
	if (classifier == NULL)
		return;

	far_hst.Setup(32, 0, classifier->GetThreshold() * 2);
	frr_hst.Setup(32, 0, classifier->GetThreshold() * 2);
	for (int i = 0; i < m_dataFiles.GetCount(); i++)
	{
		TLFDBSemanticDescriptor* d = (TLFDBSemanticDescriptor*)m_dataFiles.Get(i);
		TLFImage img;
		if (!img.LoadFromFile(d->GetImageFile().u8string().c_str()))
			continue;
		engine.SetSourceImage(&img, false);
		TLFImage* img1 = detector->GetImage();
		if (i % 100 == 0)
			printf(">");
		for (int j = 0; j < scanner->GetFragmentsCount(); j++)
		{
			double err = 0;
			double scale;// = 1;
			awpRect rect = scanner->GetFragmentRect(j);
			TLFRect lf_rect(rect);
			double overlap_det = d->Overlap(lf_rect);
			scale = (double)(rect.right - rect.left) / (double)scanner->GetBaseWidth();
			if (!all)
			{				
				/*int result = */classifier->Classify(img1, TLFAlignedTransform(scale, scale, rect.left, rect.top), err);
			}
			else
			{
				for (int k = 0; k <= stage; k++)
				{
					ILFStrong* s = (ILFStrong*)strongs->Get(k);
					
					err = 0;
					if (s->Classify(img1, TLFAlignedTransform(scale, scale, rect.left, rect.top), err) == 0)
					{
						if (k != stage)
							err = 0;
						break;
					}
				}
			}


			if (overlap_det > overlap)
			{
				frr_hst.AddElememt(err);
			}
			else if (overlap_det < overlap- 0.5)
			{
				far_hst.AddElememt(err);
			}
		}
	}

}

void TLFDBLabeledImages::GetFarFrr(TLFDetectEngine& engine, double& Far, double& Frr, double& FFar, double& t, double overlap)
{
	Far = 0;
	Frr = 0;
	if (GetItemsCount() == 0)
		return;
    int num_fragments = 0;
	unsigned long  ptime = 0;
	unsigned long  ctime;// = 0;
	for (int i = 0; i < m_dataFiles.GetCount(); i++)
	{
		TLFDBSemanticDescriptor* d = (TLFDBSemanticDescriptor*)m_dataFiles.Get(i);
		auto strImageName = d->GetImageFile();
		printf("processing: %s\n", strImageName.u8string().c_str());
		TLFImage img;
		img.LoadFromFile(strImageName.u8string().c_str());
		ctime = LFGetTickCount();
		engine.SetSourceImage(&img, true);
		ptime += (LFGetTickCount() - ctime);
		ILFScanner* sc = engine.GetScanner();
		num_fragments += sc->GetFragmentsCount();
		TLFSemanticImageDescriptor* d1 = engine.GetSemantic();
		Far += d1->Compare(d, overlap);
		double a = d->Compare(d1, overlap);
		if (a > 0)
			printf("failed.\n");
		Frr += a;// d->Compare(d1, overlap);
//		if (i % 100 == 0)
//			printf(">");

		if (m_progress != NULL)
        {
			m_progress(strImageName.filename().u8string().c_str(), 100*i/ m_dataFiles.GetCount());
        }

	}
	printf("\n");
	t = ptime / this->GetImagesCount();
	FFar = Far / num_fragments;
	Far /= GetItemsCount();
	Frr /= GetItemsCount();
}

void TLFDBLabeledImages::CheckEngine(TLFDetectEngine& engine, double overlap)
{
	bool need_cluster = engine.GetNeedCluster();
	engine.SetNeedCluster(false);
	ILFObjectDetector* detector = engine.GetDetector(0);
	for (int i = 0; i < m_dataFiles.GetCount(); i++)
	{
		TLFDBSemanticDescriptor* d = (TLFDBSemanticDescriptor*)m_dataFiles.Get(i);
		if (d == NULL)
			continue;
		auto strImageName = d->GetImageFile();
		printf("processing: %s\n", strImageName.u8string().c_str());
		TLFImage img;
		img.LoadFromFile(strImageName.u8string().c_str());

		for (int j = 0; j < d->GetCount(); j++)
		{
			TLFDetectedItem* item = d->GetDetectedItem(j);
			TLFRect* lf_rect = item->GetBounds();
			awpRect rect = lf_rect->GetRect();
			detector->Init(img.GetImage());
		    int result = detector->ClassifyRect(rect, NULL, NULL);
			awpImage* fragment = NULL;
			awpCopyRect(img.GetImage(), &fragment, &rect);
			if (fragment == NULL)
			{
				printf("failed fragment.\n");
				continue;
			}
			rect.left = 0;
			rect.top  = 0;
			rect.right  = fragment->sSizeX;
			rect.bottom = fragment->sSizeY;
			detector->Init(fragment);
			int result1 = detector->ClassifyRect( rect, NULL, NULL);
			if (result1 != result || result == 0)
				printf("failed with result %i.\n", result);
			else
				printf("success.\n");
		}

	}

	engine.SetNeedCluster(need_cluster);
}

void TLFDBLabeledImages::SetLabel(const char* label)
{
	for (int i = 0; i < m_dataFiles.GetCount(); i++)
	{
		TLFDBSemanticDescriptor* d = (TLFDBSemanticDescriptor*)m_dataFiles.Get(i);
		if (d == NULL)
			continue;
		for (int j = 0; j < d->GetCount(); j++)
		{
			TLFDetectedItem* item = d->GetDetectedItem(j);
			item->SetType(label);
		}
		auto fname = d->GetImageFile();
		d->SaveXML(fname.replace_extension(".xml").u8string().c_str());
	}


}
TLFDBSemanticDescriptor* TLFDBLabeledImages::GetDescriptor(int index)
{
     if (index < 0 || index >= m_dataFiles.GetCount())
		return NULL;
     return dynamic_cast<TLFDBSemanticDescriptor*> ( m_dataFiles.Get(index));
}

int TLFDBLabeledImages::GetClassesCount()
{
	return this->m_dictinary.GetCount();
}

int TLFDBLabeledImages::GetDescrFilesCount()
{
	int count = 0;
	for (int i = 0; i < m_dataFiles.GetCount(); i++)
	{
		TLFDBSemanticDescriptor* d = (TLFDBSemanticDescriptor*)m_dataFiles.Get(i);
        auto strImgName = d->GetImageFile();
        if (std::filesystem::exists(strImgName.replace_extension(".xml")))
			count++;
	}
	return count;
}

std::filesystem::path TLFDBLabeledImages::GetPath() const
{
    return m_strPath;
}

int TLFDBLabeledImages::GetLabelCount(const char* class_label)
{
	int count = 0;
	for (int i = 0; i < m_dataFiles.GetCount(); i++)
	{
		TLFDBSemanticDescriptor* d = (TLFDBSemanticDescriptor*)m_dataFiles.Get(i);
        for (int j = 0; j < d->GetCount(); j++)
        {
            TLFDetectedItem* di = d->GetDetectedItem(j);
            std::string str_type = di->GetType();
            if (strcmp(class_label, str_type.c_str()) == 0)
                count++;
        }
	}

	return count;
}

void TLFDBLabeledImages::UpdateUUIDsDatabase()
{
    int c = m_dataFiles.GetCount();
    for(int i = 0; i < c; i++)
    {
         TLFDBSemanticDescriptor* d = (TLFDBSemanticDescriptor*)m_dataFiles.Get(i);
         for (int j = 0; j < d->GetCount(); j++)
         {
            TLFDetectedItem* item = d->GetDetectedItem(j);
            std::string item_type = item->GetType();
            for (int k = 0; k < m_dictinary.GetCount(); k++)
            {
				TLFSemanticDictinaryItem* si = m_dictinary.GetWordFromDictinary(k);
                std::string noun = si->GetItemLabel();
                std::string id   = si->GetId();
                if (item_type == noun)
                    item->SetType(id.c_str());
            }
         }
		 auto strXmlName = d->GetImageFile();
		 strXmlName = strXmlName.replace_extension(".xml");
         if (m_progress != NULL)
         {
            m_progress(strXmlName.filename().u8string().c_str(), 100*i/c);
         }
         d->SaveXML(strXmlName.u8string().c_str());
    }
}

void TLFDBLabeledImages::UpdateDatabase()
{
    int c = m_dataFiles.GetCount();
    for(int i = 0; i < c; i++)
    {
         TLFDBSemanticDescriptor* d = (TLFDBSemanticDescriptor*)m_dataFiles.Get(i);
         for (int j = d->GetCount()-1; j >=0; j--)
         {
            TLFDetectedItem* item = d->GetDetectedItem(j);
            std::string item_type = item->GetType();
            bool found = false;
			for (int k = 0; k < m_dictinary.GetCount(); k++)
            {
                TLFSemanticDictinaryItem* si = m_dictinary.GetWordFromDictinary(k);
                std::string id   = si->GetId();
				if (item_type == id)
                {
                    item->SetType(id.c_str());
                    item->SetColor(si->GetColor());
                    ILFScanner* s = si->GetScanner();
                    item->m_bh = s->GetBaseHeight();
                    item->m_bw = s->GetBaseWidth();
                    found = true;
                }
            }
            if (!found)
                d->Delete(j);
         }
		 auto strXmlName = d->GetImageFile();
		 strXmlName = strXmlName.replace_extension(".xml");
         if (m_progress != NULL)
         {
            m_progress(strXmlName.filename().u8string().c_str(), 100*i/c);
         }
         d->SaveXML(strXmlName.u8string().c_str());
    }
}





