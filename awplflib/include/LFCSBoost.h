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
//	File: LFCSBoolt.h
//	Purpose: declares AdaBoost trainig classes 
//
//	Copyright (C) 2004-2018, NN-Videolab.net, all rights reserved.
//M*/

//---------------------------------------------------------------------------
#ifndef CSBoostH
#define CSBoostH
/** \defgroup LFCSBoost
*	Implementation of  AdaBoost trainig classes 
*   @{
*/
struct TWclErrInfo
{
	double m_thr;
	double m_far;
	double m_frr;
};
typedef std::vector<TWclErrInfo>      TErrTrainData;
// класс, реализующий процедуру обучения по алгоритму adaboost
// выходом класса является объект типа TCSStrong
class TCSAdaBoost
{
protected:
    std::ostream*       m_pLog;
	std::string			m_strPath;

    TLFObjectList       m_Features; 			// features list ILFfeature
    TLFObjectList       m_TrainingSamples;		// trainig samples 
    TErrTrainData       m_trainData; 			// Roc
    TCSStrong           m_ResultClass;			// strong classifier
  	std::string			m_strArtefactsBase;     // path to negative samples 
	std::string			m_strObjectsbase;       // path to positive samples 

    int                 m_widthBase;
    int                 m_heightBase;
    int                 m_nFeaturesCount;
  	std::string			m_strOutName;
    int                 m_nTestFaces;
    int                 m_nTestNonFaces;
    int                 m_nFaces;
    int                 m_nNonFaces;
	double				m_FinishFar;


    void   PrintFeature(ILFFeature* pSensor);
    bool   LoadSample(TLFObjectList& SampleList, int flag, std::string const& path);
    double PrintStatistics(TCSStrong& Class, double& afrr);
public:
    TCSAdaBoost();

	bool Load (TiXmlElement* node, int cascade);
    bool Load  (const char* lpFileName);
    bool LoadSamples();
	bool Boost(int stage);
	void InitFeatures();
	
    // print message on the screen and in the log file
    void DbgMsg( std::string const& );
    // returns path to negative samples 
	std::string GetArtefactsBase();
	void SetArtefactsBase(std::string str);
	// returns path to positive samples 
	std::string GetObjectsBase();
	void SetObjectsBase(std::string str);
    // возвращает ширину и высоту изображений, которые используются
    // для обучения
	AWPWORD WidthBase();
	void SetWidthBase(AWPWORD value);

	AWPWORD HeightBase();
	void SetHeightBase(AWPWORD value);

	TErrTrainData& GetTrainData();

	TCSStrong* GetClass();
	int GetFeaturesCount();
	void SetFeaturesCount(int value);

	double GetFinishFar();
	void SetFinishFar(double value);

	std::string GetPath();

	void SetPath(const char* lpPath);
	void   SaveFRRSamples(int stage);
	void   SaveNegativeSamples(const char* lpPath);
public: 
	// features 
	bool m_CSFeature;
	bool m_LBPFeature;
	bool m_HFeature;
	bool m_VFeature;
	bool m_DFeature;
	bool m_CFeature;
	bool m_HAFeature;
	bool m_VAFeature;
	bool m_DAFeature;
	bool m_CAFeature;

};


class TCSAdaBoostSign
{

protected:
    TLFObjectList       m_Features; 			// признаки TCSSensor
    TLFObjectList       m_TrainingSamples;		// образцы для обучения
    TLFObjectList       m_TestingSamples;       // Образцы для тестирования
    TCSStrongSign       m_ResultClass;			// результат работы процедуры накачки

    int                 m_widthBase;
    int                 m_heightBase;

    int                 m_maxFeatures;

    std::string         m_strBase1; //  1
    std::string         m_strBase2; // -1

    int                 m_numSamplesBase1;
    int                 m_numSamplesBase2;
    int                 m_numSamplesTestBase1;
    int                 m_numSamplesTestBase2;
	std::ofstream		m_flog;

    void    InitFeatures();
    bool    LoadSamples(int flag, std::string const& path);
   	bool    Load (TiXmlElement* node);
    void    DbgMsg( std::string const& );
    void    Statistics();
public:
    TCSAdaBoostSign();
	void SetLogName(char* lpLogName);
	void SetLog1Name(char* lpLog1Name);
    bool Load(const char* lpConfigName);
    bool Boost();
    TCSStrongSign* GetResult();
	void SaveROC(char*lpName);
};

class TIEFSAdaBoost
{
protected:
	std::ostream*       m_pLog;
	// распечатка сообщения на экране и в лог файле
	virtual void DbgMsg(std::string const&);

protected:
	TLFObjectList       m_TrainingSamples;		// образцы для обучения
	TLFObjectList       m_Features;				// слабые классификаторы
	int					m_numIFS;
	int					m_numEFS;
	std::string			m_strSourceFile;
	TIEFSStrong			m_ResultClass;
	bool				InitFeatures(int num);
	virtual bool		LoadSamples();
	double				PrintStatistics();
public:
	TIEFSAdaBoost();
	bool Load(const char* lpFileName);
	bool Boost();
};
/** @} */ /*  end of LFCSBoolt group */
#endif
