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
//		File: LFDatabase.h
//		Purpose: Implementation of the Locate Framework XML labeled database
//
//      CopyRight 2004-2020 (c) NN-Videolab.net
//M*/

#ifndef _lf_database_h_
#define _lf_database_h_
/** \defgroup LFDatabase
*	Implementation of the Locate Framework XML labeled database
*   @{
*/
class TLFDBSemanticDescriptor : public TLFSemanticImageDescriptor
{
protected:
	std::string m_strImageName;
public:
	TLFDBSemanticDescriptor(const char* lpFileName);
	const char* GetImageFile();
};

class TLFDBLabeledImages : public TLFObject
{
protected:
	TLFProgress   m_progress;

	TLFString				m_strPath;
	TLFObjectList			m_dataFiles;
	TLFSemanticDictinary	m_dictinary;
	bool LoadDatabase(const char* path);

public:

	TLFDBLabeledImages(const char* path);
	TLFDBLabeledImages();
	virtual ~TLFDBLabeledImages();

	bool InitDB(const char* path);
	void Clear();
    void ClearDatabase();
	void CreateDictinary(const char* path);
    void UpdateUUIDsDatabase();
    void UpdateDatabase();

	// properties
	int GetItemsCount();
	int GetImagesCount();
    int GetClassesCount();
    int GetDescrFilesCount();
    int GetLabelCount(const char* class_label);

    TLFString GetPath();
    TLFDBSemanticDescriptor* GetDescriptor(int index);
	TLFSemanticDictinary*	 GetDictinary();


	// error distribution
	void GetFarHST(TLFDetectEngine& engine, TLFHistogramm& hst , int stage, bool all, double overlap);
	void GetFrrHST(TLFDetectEngine& engine, TLFHistogramm& hst, int stage, bool all, double overlap);
	void GetFarFrrHST(TLFDetectEngine& engine, TLFHistogramm& far_hst, TLFHistogramm& frr_hst, int stage, bool all, double overlap);
	void GetFarFrr(TLFDetectEngine& engine, double& Far, double& Frr, double& FFar, double& t,  double overlap);

	void CheckEngine(TLFDetectEngine& engine, double overlap);
	void SetLabel(const char* label);
	void SetProgress(TLFProgress progress);



	virtual const char* GetName()
	{
		return "TLFDBLabeledImages";
	}
};

class TLFDBlabeledMedia : public TLFObject
{
protected:
	TLFSemanticDictinary	m_dictinary;
	TLFString				m_strPath;
	TLFObjectList			m_dataFiles;
public: 
	TLFDBlabeledMedia(const char* path);
	TLFDBlabeledMedia();

	virtual const char* GetName()
	{
		return "TLFDBlabeledMedia";
	}
};
/** @} */ /*  end of LFDatabase group */
#endif 