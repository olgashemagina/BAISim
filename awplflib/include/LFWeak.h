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
//	File: LFWeak.h
//	Purpose: Weak classifiers
//	Copyright (C) 2004-2018, NN-Videolab.net, all rights reserved.
//M*/

#ifndef HLWEAKH
#define HLWEAKH
/** \defgroup LFWeaks 
*	Implementations of the weak classifiers in the Locate Framework
*   @{
*/

//---------------------------------------------------------------------------
// weak classifier based on  Census Transform
// https://siddhantahuja.wordpress.com/2009/05/10/census-transform-in-image-processing/
class TCSWeak : public ILFWeak
{
protected:
    AWPBYTE       m_Classifiacator[512]; //классификатор
public:
    TCSWeak();
	TCSWeak(ILFFeature* feature);
    TCSWeak(TCSWeak& Weak);
	TCSWeak(TCSWeak* weak);

    void SaveXML(TiXmlElement* parent);
    bool LoadXML(TiXmlElement* parent);
    // позиционирование
    void Scale(double factor);
    void Shift(int dx, int dy);

    // классификация
    //int Classify(awpImage* pImage, double avg = 0);
	virtual int Classify(TLFImage* pImage, double* value = NULL);

	AWPBYTE Classificator(int i)
	{
		return m_Classifiacator[i];
	}
    //
    TCSWeak& operator = (TCSWeak& Weak);
  //  TCSSensor* GetSensor()
  //  {
		//return dynamic_cast<TCSSensor*>(this->m_pFeature);
  //  }
	virtual const char* GetName()
	{
		return "TCSWeak";
	}
};

// weak classifier, based on census transform
// with ability of training 
class TCSWeakTraining : public TCSWeak
{
protected:
    double     m_Eps;                   //error while training 
    double     m_FacesDistrib[512];
    double     m_NonFacesDistrib[512];
public:
	TCSWeakTraining();
	TCSWeakTraining(ILFFeature* feature);
	TCSWeakTraining(TCSWeakTraining* weak);

    virtual void StartTraining();
    virtual void Train();
    virtual void AddSample(TCSSample* pSample,int  flag, double weight, int base_width, int base_height);

    void SetEpsilon(double Value)
    {
        m_Eps = Value;
    }
    double GetEpsilon()
    {
        return m_Eps;
    }
	virtual const char* GetName()
	{
		return "TCSWeakTraining";
	}
};

// weak classifier, based on census transform
// with ability of training with "soft" sheme 
class TCSSoftWeakTraining : public TCSWeakTraining
{
public:
	TCSSoftWeakTraining();
	TCSSoftWeakTraining(TCSSoftWeakTraining* weak);

    void AddSoftSample(TCSSoftSample* pSample);
	virtual const char* GetName()
	{
		return "TCSSoftWeakTraining";
	}
};
// weak classifier, based on census transform
// wich returns -1, 0 , 1
class TCSWeakSign : public ILFWeak
{
protected:
    int m_table[512];
public:
    TCSWeakSign();
    TCSWeakSign(TCSSensor& Sensor);
    TCSWeakSign(TCSWeakSign& Weak);
	TCSWeakSign(TCSWeakSign* weak);
    //io methods 
    void SaveXML(TiXmlElement* parent);
    bool LoadXML(TiXmlElement* parent);

    // positioning
    void Scale(double factor);
    void Shift(int dx, int dy);

    //classification
	
	virtual int Classify(TLFImage* pImage, double* value = NULL);
    //double ClassifyWeight(awpImage* pImage, double avg = 0);
	virtual const char* GetName()
	{
		return "TCSWeakSign";
	}
};
// weak classifier, based on census transform
// with ability of training and returns -1, 0, 1
class TCSWeakSignTrainig : public TCSWeakSign
{
protected:
    double     m_Eps;                    //error while training 
    double     m_DistribClass1[512];     //feature distibution for class 1
    double     m_DistribClass2[512];     //feature distributiom for class 2
public:
	TCSWeakSignTrainig();
    TCSWeakSignTrainig(TCSSensor& Sensor);
	TCSWeakSignTrainig(TCSWeakSignTrainig* weak);
    virtual void StartTraining();
    virtual void Train();
    virtual void AddSample(TCSSample* pSample,int  flag, double weight);
        void SetEpsilon(double Value)
    {
        m_Eps = Value;
    }
    double GetEpsilon()
    {
        return m_Eps;
    }
	virtual const char* GetName()
	{
		return "TCSWeakSignTrainig";
	}
};

// слабый классификатор для системы IEFS
class TIEFSWeak : public TLFObject
{
protected:
	double     m_Eps;                   //ошибка, полученная в процессе обучения
	double m_Wieght;
	double m_AvgClass0;
	double m_AvgClass1;

	double m_DspClass0;
	double m_DspClass1;

	double m_Theta;      // the threshold

	// training support
	double    m_Class0Counter;
	double    m_Class1Counter;
	int		  m_index;
	bool	  m_free;
	void      CalcTheta();

public:
	TIEFSWeak();
	TIEFSWeak(int index);
	TIEFSWeak(TIEFSWeak* w);
	int Classify(double value, double* value2 = NULL);
	double GetWeight();
	void SetWeight(double Value);
	void StartTraining();
	void AddSample(TIEFSSample* sample);
	void Train();
	bool IsFree();
	void Lock();
	void SetEpsilon(double Value);
	double GetEpsilon();
	int GetIndex();
	TIEFSWeak* Clone();
	TiXmlElement* SaveXML();
	bool LoadXML(TiXmlElement* e);
	virtual const char* GetName()
	{
		return "TIEFSWeak";
	}
};

// слабый классификатор для системы IEFS
class TIEFSWeak2 : public TLFObject
{
protected:
	double     m_Eps;                   //ошибка, полученная в процессе обучения
	double	   m_Wieght;
	double     m_Distrib1[512];
	double     m_Distrib2[512];
	AWPBYTE    m_Classifiacator[512]; //классификатор


	// training support
	double    m_Class0Counter;
	double    m_Class1Counter;
	int		  m_index;
	bool	  m_free;

public:
	TIEFSWeak2();
	TIEFSWeak2(int index);
	TIEFSWeak2(TIEFSWeak2* w);
	int Classify(double value, double* value2 = NULL);
	double GetWeight();
	void SetWeight(double Value);
	void StartTraining();
	void AddSample(TIEFSSample* sample);
	void Train();
	bool IsFree();
	void Lock();
	void SetEpsilon(double Value);
	double GetEpsilon();
	int GetIndex();
	TIEFSWeak2* Clone();
	TiXmlElement* SaveXML();
	bool LoadXML(TiXmlElement* e);
	virtual const char* GetName()
	{
		return "TIEFSWeak2";
	}
};

#define HYST_METHOD_MORE 0
#define HYST_METHOD_LESS 1
#define HYST_BUF_LEN 24
class TLFHysteresisWeak : public ILFWeak
{
protected:
	double m_t1;
	double m_t2;
	int    m_state;
	int    m_startTime;
	int	   m_method;

	double m_a1;
	double m_d1;
	double m_a2;
	double m_d2;
	double m_theta;

	TLFRingBuffer* m_buffer;
	void InitClassifier();
public:
	TLFHysteresisWeak();
	TLFHysteresisWeak(ILFFeature* feature);
	TLFHysteresisWeak(TLFHysteresisWeak& Weak);
	TLFHysteresisWeak(TLFHysteresisWeak* weak);
	virtual ~TLFHysteresisWeak();

	virtual void SaveXML(TiXmlElement* parent);
	virtual bool LoadXML(TiXmlElement* parent);

	virtual int Classify(TLFImage* pImage, double* value = NULL);

	double GetT1();
	void   SetT1(double value);

	double GetT2();
	void   SetT2(double value);

	int GetMethod();
	void Setmethod(int value);
	TLFRingBuffer* GetBuffer();

	virtual const char* GetName()
	{
		return "TLFHysteresisWeak";
	}
};

class TLFDynamicWeak : public TLFHysteresisWeak
{
protected:
	int m_buf1Size;
	int m_buf2Size;
	double* m_buf1;
	double* m_buf2;
	double m_buf1_avg;
	double m_buf2_avg;
	int m_classifyCount;
public:
	TLFDynamicWeak(int size1, int size2);
	TLFDynamicWeak(ILFFeature* feature, int size1, int size2);
	TLFDynamicWeak(TLFDynamicWeak& Weak);
	TLFDynamicWeak(TLFDynamicWeak* weak);
	virtual ~TLFDynamicWeak();

	virtual void SaveXML(TiXmlElement* parent);
	virtual bool LoadXML(TiXmlElement* parent);

	virtual int Classify(TLFImage* pImage, double* value = NULL);

	void Reset();

	virtual const char* GetName()
	{
		return "TLFDynamicWeak";
	}
};
//accumulating weak classifier
class TLFAccWeak : public ILFWeak
{
protected:
	int	   buf_size;
	int	   m_counter;
	double m_threshold;
	int	   m_state;
	int    m_begin_counter;
	TLFRingBuffer* m_buffer;
	int    m_bg_stability;
    unsigned int    m_delay;
    unsigned long    m_delay_counter;
public:
	TLFAccWeak(double t);
	TLFAccWeak(ILFFeature* feature, double t);
	TLFAccWeak(TLFAccWeak& Weak);
	TLFAccWeak(TLFAccWeak* weak);
	virtual ~TLFAccWeak();

	virtual void SaveXML(TiXmlElement* parent);
	virtual bool LoadXML(TiXmlElement* parent);

	virtual int Classify(TLFImage* pImage, double* value = NULL);

	void SetThreshold(double value);
	double GetThreshold();

	int  GetBgStability();
	void SetBgStability(int value);

	int  GetDelay();
	void SetDelay(int value);

	int GetBufSize();
	void SetBufSize(int value);

	TLFRingBuffer* GetBuffer()
	{
		return m_buffer;
	}


	void Reset();

	virtual const char* GetName()
	{
		return "TLFAccWeak";
	}
};
/** @} */ /*  end of LFBuffers group */
#endif
