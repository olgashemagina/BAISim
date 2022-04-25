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
//	Locate Framework  4.0
//	File: LFBuilder.h
//	Purpose: Build cascade structure, based on database of labeled images 
//			 and AdaBoost trainig algorythm 
//	Copyright (C) 2004-2020, NN-Videolab.net, all rights reserved.
//M*/

#ifndef __lf_builder_h__
#define __lf_builder_h__
/** \defgroup LFBuilder
*	Build cascade structure, based on database of labeled images 
*   and AdaBoost trainig algorythm
*   @{
*/
// класс TCSBuildDetector предназначен дл€ построени€
// многокаскадного детектора с использованием
// алгоритма AdaBoost и преобразовани€ Census
class TCSBuildDetector
{
protected:
	// detector 
	bool			CreateDetector(const char* lpDetectorName);
	bool			UpdateDetector();
	bool		    CheckDetector();
	//   background
	bool			BuildBkground();
	bool			Boost();
	bool		    BuildDefaultBkGround();
	void		    RemoveBkground();
	int				GetNumObjects();
	// ключевые переменные
	TCSAdaBoost		m_AdaBoost;
	TLFDetectEngine	m_Engine;

	/*Builder configuration variables*/
	std::string     m_strDetectorName;   // им€ файла детектора
	std::string		m_strBKG;			 //источник объектов фона
	std::string     m_strOBJ;			 //размеченна€ база данных объектов
	std::string     m_strPathToBase;     // путь к базе данных изобжений,
	// служащих источником дл€ образцов фона
	std::string	    m_strConfigName;     //им€ файла конфигурации
	std::string     m_strLogName;		 // им€ лог файла. 

	int				m_SPI;			/*samples per image */
	int				m_NS;			/*num stages*/

	// управл€ющие параметры
	int             m_nBgrdCount;         // количество образцов фона
	int             m_nMaxSamplesPerImage;
	int             m_nMinBgrdCount;
	double			m_overlapThr;

public:
	TCSBuildDetector();
	bool		LoadConfig(std::string const& filename);
	bool		SaveConfig(std::string const& filename);
	bool		InitDetector();
	void		PrintDetectorInfo();
	void		AddNewClassifier();
	bool		Build();
};
/**
*/
class TLFBuilder
{
protected:
	TLFString			m_strDetectorName;
	TLFString			m_strBackGround;
	TCSAdaBoost			m_AdaBoost;
	TSCObjectDetector*  m_detector;

	bool LoadConfig(const char* fileName);
	bool BuildBackground();
	bool UpdateDetector();
public:
	TLFBuilder();
	~TLFBuilder();

	bool CreateDetector(ILFScanner* scanner, const char* fileName);
	bool LoadDetector(const char* fileName);
	bool SaveDetector(const char* fileName);

	//----
	bool LoadFromEngine(const char* fileName, int index=0);

	//
	void PrintInfo();

	void Build(const char* fileName);

};
/** @} */ /*  end of LFBuilder group */
#endif //__lf_builder_h__