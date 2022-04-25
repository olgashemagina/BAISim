/*Locate From 4.0
File: LFWeak.cpp
Purpose: this module includes numeber of WEAK classificators
Copyright (c) AWPSoft.
*/
#include "_LF.h"

//---------------------------------------------------------------------------
// слабый классификатор, основанный на преобразовании Census
TCSWeak::TCSWeak() : ILFWeak()
{

   memset(m_Classifiacator, 0, sizeof(m_Classifiacator));
   m_pFeature = new TCSSensor();
}

TCSWeak::TCSWeak(ILFFeature* feature) : ILFWeak(feature)
{

   memset(m_Classifiacator, 0, sizeof(m_Classifiacator));
   this->m_pFeature = LFCreateFeature(feature);
}

TCSWeak::TCSWeak(TCSWeak& Weak) : ILFWeak()
{
  m_fWeight = Weak.m_fWeight;
  m_pFeature = LFCreateFeature(Weak.Fetaure());
  memcpy(m_Classifiacator, Weak.m_Classifiacator, sizeof(m_Classifiacator));
}

TCSWeak::TCSWeak(TCSWeak* weak) : ILFWeak()
{
	if (weak == NULL)
		return;
	m_fWeight = weak->m_fWeight;
	m_pFeature = LFCreateFeature(weak->Fetaure());
	memcpy(m_Classifiacator, weak->m_Classifiacator, sizeof(m_Classifiacator));

}

const unsigned int bit_mask[32] =
{
	0x00000001,
	0x00000002,
	0x00000004,
	0x00000008,
	0x00000010,
	0x00000020,
	0x00000040,
	0x00000080,
	0x00000100,
	0x00000200,
	0x00000400,
	0x00000800,
	0x00001000,
	0x00002000,
	0x00004000,
	0x00008000,
	0x00010000,
	0x00020000,
	0x00040000,
	0x00080000,
	0x00100000,
	0x00200000,
	0x00400000,
	0x00800000,
	0x01000000,
	0x02000000,
	0x04000000,
	0x08000000,
	0x10000000,
	0x20000000,
	0x40000000,
	0x80000000
};
static void convert_16int_512_byte (unsigned int* a, AWPBYTE* b)
{
  int c = 0;
  for (int i = 0; i < 16; i++)
  {
     for (int j = 0; j < 32; j++)
     {
        unsigned int m = a[i] & bit_mask[j];
        b[c] = m > 0 ? 1 : 0;
        c++;
     }
  }
}

static void convert_512byte_16int(unsigned int* a, AWPBYTE* b)
{
    memset(a, 0, 16*sizeof(int));
    int c = 0;
    for (int i = 0; i < 16; i++)
    {
        for (int j = 0; j < 32; j++)
        {
           if (b[c] > 0)
            a[i] |= bit_mask[j];

           c++;
        }
    }

}

// positioning
void TCSWeak::Scale(double factor)
{
  m_pFeature->Scale(factor);
}

void TCSWeak::Shift(int dx, int dy)
{
	m_pFeature->Shift(dx, dy);
}


// classification
int TCSWeak::Classify(TLFImage* pImage, double* value)
{
	int idx = m_pFeature->uCalcValue(pImage);
	if (value != NULL)
		*value = (double)idx;
	return m_Classifiacator[idx];
}
//
TCSWeak& TCSWeak::operator = (TCSWeak& Weak)
{
    TCSWeak* res = new TCSWeak(Weak);
    return *res;
}
// save to XML
void TCSWeak::SaveXML(TiXmlElement* parent)
{
     if (parent == NULL)
        return;
     TiXmlElement* weak = new TiXmlElement("CSWeak");
     weak->SetDoubleAttribute("weigth", m_fWeight);
     unsigned int Masks[16];
     memset(Masks, 0, sizeof(Masks));
     convert_512byte_16int(Masks, m_Classifiacator);
     char buf[16];
     char buf1[16];
     for (int i = 0; i < 16; i++)
     {
        sprintf(buf, "v%i", i);
        sprintf(buf1,"%u", Masks[i]);
        weak->SetAttribute(buf, buf1);
     }

     m_pFeature->SaveXML(weak);
     parent->LinkEndChild(weak);
}
// load from XML
bool TCSWeak::LoadXML(TiXmlElement* parent)
{
   if (parent == NULL)
    return false;
   // DONE: check name
   const char* name = parent->Value();
   if (strcmp(name, "CSWeak") != 0)
    return false;
   parent->QueryDoubleAttribute("weigth", &m_fWeight);
   char buf[16];
   const char* buf1;

   unsigned int Masks[16];
   memset(Masks, 0, sizeof(Masks));
   int v =0;
   for (int i = 0; i < 16; i++)
   {
       sprintf(buf, "v%i", i);
       buf1 = parent->Attribute(buf);
	   unsigned long ul = strtoul(buf1, NULL, 10);
       Masks[i] = (unsigned int)ul;
   }
   convert_16int_512_byte(Masks, m_Classifiacator);
   if (m_pFeature != NULL)
	   delete m_pFeature;
   TiXmlElement* e = parent->FirstChild()->ToElement();
   name = e->Value();
   m_pFeature = LFCreateFeature(name, 0,0,6,6);
   if (m_pFeature == NULL)
	   return false;
   if (!m_pFeature->LoadXML(parent->FirstChild()->ToElement()))
    return false;

   return true;
}

//---------------------------------------------------------------------------
TCSWeakTraining::TCSWeakTraining() : TCSWeak()
{
	m_Eps = 0;
	memset(m_FacesDistrib, 0, sizeof(m_FacesDistrib));
	memset(m_NonFacesDistrib, 0, sizeof(m_NonFacesDistrib));
}

TCSWeakTraining::TCSWeakTraining(ILFFeature* feature) : TCSWeak(feature)
{
    m_Eps = 0;
    memset (m_FacesDistrib, 0, sizeof(m_FacesDistrib));
    memset (m_NonFacesDistrib, 0, sizeof(m_NonFacesDistrib));
}
TCSWeakTraining::TCSWeakTraining(TCSWeakTraining* weak) : TCSWeak(weak)
{
	if (weak == NULL)
		return;
	m_Eps = weak->m_Eps;
	memcpy(m_FacesDistrib, weak->m_FacesDistrib, sizeof(m_FacesDistrib));
	memcpy(m_NonFacesDistrib, weak->m_NonFacesDistrib, sizeof(m_NonFacesDistrib));
}

void TCSWeakTraining::StartTraining()
{
    memset (m_FacesDistrib, 0, sizeof(m_FacesDistrib));
    memset (m_NonFacesDistrib, 0, sizeof(m_NonFacesDistrib));
}

void TCSWeakTraining::Train()
{
	for (int i = 0; i < 512; i++)
		m_Classifiacator[i] = m_NonFacesDistrib[i] >= m_FacesDistrib[i] ? 0: 1;
}

void TCSWeakTraining::AddSample(TCSSample* pSample,int  flag, double weight, int base_width, int base_height)
{
	double factor = std::min<double>(pSample->GetImage()->sSizeX / double(base_width), pSample->GetImage()->sSizeY / double(base_height));
	m_pFeature->Scale(factor);
	awpImage* img = pSample->GetIntegralImage();

	int idx = m_pFeature->uCalcValue(pSample);//sensor->CalcValue(img);
	if (flag == 1)
		m_FacesDistrib[idx] += weight;
	else if (flag == 0)
		m_NonFacesDistrib[idx] += weight;
}

//------------------------------------------------------------------------------
// trained weak classifier
TCSSoftWeakTraining::TCSSoftWeakTraining() :TCSWeakTraining()
{

}
TCSSoftWeakTraining::TCSSoftWeakTraining(TCSSoftWeakTraining* weak) : TCSWeakTraining(weak)
{
}

void TCSSoftWeakTraining::AddSoftSample(TCSSoftSample* pSample)
{
  {
	  double factor = (double)(pSample->GetFragment().right - pSample->GetFragment().left) / 24.0f;
       m_pFeature->Scale(factor);
	   m_pFeature->Shift(pSample->GetFragment().left, pSample->GetFragment().top);
  }
  TCSSensor* sensor = dynamic_cast<TCSSensor*>(this->m_pFeature);
  int idx = 0;//todo: !!! sensor->CalcValue(pSample->GetImage());
  if (pSample->GetFlag() == 1)
     m_FacesDistrib[idx] += pSample->GetWeight();
  else if (pSample->GetFlag() == 0)
     m_NonFacesDistrib[idx] += pSample->GetWeight();
}
////////////////////////////////////////////////////////////////////////////////
TCSWeakSign::TCSWeakSign() : ILFWeak()
{
   memset(m_table, 0, sizeof(m_table));
}
TCSWeakSign::TCSWeakSign(TCSSensor& Sensor) : ILFWeak()
{
   memset(m_table, 0, sizeof(m_table));
   m_pFeature = LFCreateFeature(&Sensor);
}
TCSWeakSign::TCSWeakSign(TCSWeakSign& Weak) : ILFWeak()
{
	m_pFeature = LFCreateFeature(Weak.Fetaure());
    memcpy(m_table, Weak.m_table, sizeof(m_table));
}
TCSWeakSign::TCSWeakSign(TCSWeakSign* weak) : ILFWeak()
{
	if (weak == NULL)
		return;
	m_pFeature = LFCreateFeature(weak->Fetaure());
	memcpy(m_table, weak->m_table, sizeof(m_table));
}
//saving - loading
void TCSWeakSign::SaveXML(TiXmlElement* parent)
{
    if (parent == NULL)
        return;
     TiXmlElement* weak = new TiXmlElement("CSWeakSign");
     weak->SetDoubleAttribute("weigth", this->m_fWeight);

     char buf[512];
     char buf1[512];
     for (int i = 0; i < 512; i++)
     {
        sprintf(buf, "v%i", i);
        sprintf(buf1,"%i", m_table[i]);
        weak->SetAttribute(buf, buf1);
     }

     m_pFeature->SaveXML(weak);
     parent->LinkEndChild(weak);
}
bool TCSWeakSign::LoadXML(TiXmlElement* parent)
{
    if (parent == NULL)
    return false;
   // DONE: check name
   const char* name = parent->Value();
   if (strcmp(name, "CSWeakSign") != 0)
    return false;
   double w = 0;
   parent->QueryDoubleAttribute("weigth", &w);
   this->m_fWeight = (double)w;
   char buf[16];
   const char* buf1;

   //unsigned int Masks[16];
   //memset(Masks, 0, sizeof(Masks));
   int v =0;
   for (int i = 0; i < 512; i++)
   {
       sprintf(buf, "v%i", i);
       buf1 = parent->Attribute(buf, &this->m_table[i]);
	   //unsigned long ul = strtoul(buf1, NULL, 10);
       //Masks[i] = (unsigned int)ul;
   }
  // convert_16int_512_byte(Masks, m_Classifiacator);

   if (!m_pFeature->LoadXML(parent->FirstChild()->ToElement()))
    return false;
   return true;

}

// positioning
void TCSWeakSign::Scale(double factor)
{
    m_pFeature->Scale(factor);
}
void TCSWeakSign::Shift(int dx, int dy)
{
    m_pFeature->Shift(dx,dy);
}

//classification
int TCSWeakSign::Classify(TLFImage* pImage, double* value)
{
   int idx = m_pFeature->uCalcValue(pImage);
   return m_table[idx];
}

////////////////////////////////////////////////////////////////////////////////
TCSWeakSignTrainig::TCSWeakSignTrainig() : TCSWeakSign()
{
	m_Eps = 0;
	memset(m_DistribClass1, 0, sizeof(m_DistribClass1));
	memset(m_DistribClass2, 0, sizeof(m_DistribClass2));
}

TCSWeakSignTrainig::TCSWeakSignTrainig(TCSSensor& Sensor)
{
	m_pFeature = LFCreateFeature(&Sensor);
    m_Eps = 0;
    memset (m_DistribClass1, 0, sizeof(m_DistribClass1));
    memset (m_DistribClass2, 0, sizeof(m_DistribClass2));
}

TCSWeakSignTrainig::TCSWeakSignTrainig(TCSWeakSignTrainig* weak) : TCSWeakSign(weak)
{
	if (weak == NULL)
		return;
	m_Eps = weak->m_Eps;
	memcpy(m_DistribClass1, weak->m_DistribClass1, sizeof(m_DistribClass1));
	memcpy(m_DistribClass2, weak->m_DistribClass2, sizeof(m_DistribClass2));
}


void TCSWeakSignTrainig::StartTraining()
{
    memset (m_DistribClass1, 0, sizeof(m_DistribClass1));
    memset (m_DistribClass2, 0, sizeof(m_DistribClass2));
}
void TCSWeakSignTrainig::Train()
{
	for (int i = 0; i < 512; i++)
    {
        if (m_DistribClass1[i] > m_DistribClass2[i])
		    m_table[i] = 1;
        else if (m_DistribClass1[i] < m_DistribClass2[i])
            m_table[i] = -1;
        else
            m_table[i] = 0;
    }
}
void TCSWeakSignTrainig::AddSample(TCSSample* pSample,int  flag, double weight)
{
  int idx = m_pFeature->uCalcValue(pSample);
  if (flag == 1)
     m_DistribClass1[idx] += weight;
  else if (flag == -1)
     m_DistribClass2[idx] += weight;
}

TIEFSWeak::TIEFSWeak()
{
	m_AvgClass0 = 0;
	m_AvgClass1 = 0;

	m_DspClass0 = 0;
	m_DspClass1 = 0;

	m_Theta = 0;

	m_Class0Counter = 0;
	m_Class1Counter = 0;
	this->m_index = -1;
	m_free = true;
}

TIEFSWeak::TIEFSWeak(int index)
{
	m_AvgClass0 = 0;
	m_AvgClass1 = 0;

	m_DspClass0 = 0;
	m_DspClass1 = 0;

	m_Theta = 0;

	m_Class0Counter = 0;
	m_Class1Counter = 0;
	this->m_index = index;
	m_free = true;

}

TIEFSWeak::TIEFSWeak(TIEFSWeak* w)
{
	this->m_AvgClass0 = w->m_AvgClass0;
	this->m_AvgClass1 = w->m_AvgClass1;
	this->m_Class0Counter = w->m_Class0Counter;
	this->m_Class1Counter = w->m_Class1Counter;
	this->m_DspClass0 = w->m_DspClass0;
	this->m_DspClass1 = w->m_DspClass1;
	this->m_Eps = w->m_Eps;
	this->m_free = w->m_free;
	this->m_index = w->m_index;
	this->m_Theta = w->m_Theta;
	this->m_Wieght = w->m_Wieght;
}

int TIEFSWeak::Classify(double value, double* value2)
{
	double v = value;
	v = 0.5*((v - m_AvgClass1)*(v - m_AvgClass1)*m_DspClass1 - (v - m_AvgClass0)*(v - m_AvgClass0)*m_DspClass0);

	if (v >= m_Theta)
		return 0;
	else
		return 1;

}
double TIEFSWeak::GetWeight()
{
	return m_Wieght;
}

void TIEFSWeak::SetWeight(double Value)
{
	m_Wieght = Value;
}
void TIEFSWeak::StartTraining()
{
	m_AvgClass0 = 0;
	m_AvgClass1 = 0;

	m_DspClass0 = 0;
	m_DspClass1 = 0;

	m_Theta = 0;

	m_Class0Counter = 0;
	m_Class1Counter = 0;
	this->m_Eps = 0;
}

void TIEFSWeak::AddSample(TIEFSSample* sample)
{
	if (sample != NULL)
	{
		double v =sample->GetValue(this->m_index);
		double Weight = sample->GetWeight();
		if (sample->GetFlag() == 0)
		{
			m_AvgClass0 += v*Weight;
			m_DspClass0 += v*v*Weight;
			m_Class0Counter += Weight;
		}
		else if (sample->GetFlag() == 1)
		{
			m_AvgClass1 += v*Weight;
			m_DspClass1 += v*v*Weight;
			m_Class1Counter += Weight;
		}
		else
			return;
	}

}
void TIEFSWeak::Train()
{
	if (m_Class0Counter == 0 || m_Class1Counter == 0)
		return;

	m_AvgClass0 /= m_Class0Counter;
	m_AvgClass1 /= m_Class1Counter;

	m_DspClass0 /= m_Class0Counter;
	m_DspClass1 /= m_Class1Counter;

	m_DspClass0 = m_DspClass0 - m_AvgClass0*m_AvgClass0;
	m_DspClass1 = m_DspClass1 - m_AvgClass1*m_AvgClass1;

	m_Class0Counter = 0;
	m_Class1Counter = 0;

	CalcTheta();

	m_DspClass0 = 1.0 / m_DspClass0;
	m_DspClass1 = 1.0 / m_DspClass1;
}

void      TIEFSWeak::CalcTheta()
{
	if (sqrt(m_DspClass0) / sqrt(m_DspClass1) <= 0)
		m_Theta = 0;
	else
		m_Theta = log(sqrt(m_DspClass0) / sqrt(m_DspClass1));
}

bool TIEFSWeak::IsFree()
{
	return this->m_free;
}
void TIEFSWeak::Lock()
{
	this->m_free = false;
}
void TIEFSWeak::SetEpsilon(double Value)
{
	m_Eps = Value;
}
double TIEFSWeak::GetEpsilon()
{
	return m_Eps;
}
int TIEFSWeak::GetIndex()
{
	return this->m_index;
}
TIEFSWeak* TIEFSWeak::Clone()
{
	return new TIEFSWeak(this);
}

TiXmlElement* TIEFSWeak::SaveXML()
{
	TiXmlElement* e = new TiXmlElement(this->GetName());
	e->SetDoubleAttribute("avg1", this->m_AvgClass1);
	e->SetDoubleAttribute("avg0", this->m_AvgClass0);
	e->SetDoubleAttribute("dsp1", this->m_DspClass1);
	e->SetDoubleAttribute("dsp0", this->m_DspClass0);
	e->SetDoubleAttribute("theta", this->m_Theta);
	e->SetAttribute("index", this->m_index);
	e->SetDoubleAttribute("weight", this->m_Wieght);
	return e;
}
bool TIEFSWeak::LoadXML(TiXmlElement* e)
{
	if (e == NULL)
		return false;
	const char* name = e->Value();
	if (strcmp(name, this->GetName()) != 0)
		return false;
	e->QueryDoubleAttribute("avg1", &this->m_AvgClass1);
	e->QueryDoubleAttribute("avg0", &this->m_AvgClass0);
	e->QueryDoubleAttribute("dsp1", &this->m_DspClass1);
	e->QueryDoubleAttribute("dsp0", &this->m_DspClass0);
	e->QueryDoubleAttribute("theta", &this->m_Theta);
	e->QueryDoubleAttribute("weight", &this->m_Wieght);
	e->QueryIntAttribute("index", &this->m_index);
	return true;
}


TIEFSWeak2::TIEFSWeak2()
{
	m_Eps = 0;
	memset(m_Distrib1, 0, sizeof(m_Distrib1));
	memset(m_Distrib2, 0, sizeof(m_Distrib2));
}
TIEFSWeak2::TIEFSWeak2(int index)
{
	m_Eps = 0;
	memset(m_Distrib1, 0, sizeof(m_Distrib1));
	memset(m_Distrib2, 0, sizeof(m_Distrib2));
	this->m_index = index;
	m_free = true;
}
TIEFSWeak2::TIEFSWeak2(TIEFSWeak2* w)
{

}
int TIEFSWeak2::Classify(double value,  double* value2)
{
	int idx = (int)value;
	return m_Classifiacator[idx];
}
double TIEFSWeak2::GetWeight()
{
	return this->m_Wieght;
}
void TIEFSWeak2::SetWeight(double Value)
{
	this->m_Wieght = Value;
}
void TIEFSWeak2::StartTraining()
{
	memset(m_Distrib1, 0, sizeof(m_Distrib1));
	memset(m_Distrib2, 0, sizeof(m_Distrib2));
}
void TIEFSWeak2::AddSample(TIEFSSample* sample)
{

	double v = sample->GetValue(this->m_index);
	double weight = sample->GetWeight();
	int flag = sample->GetFlag();
	if (flag == 1)
		m_Distrib1[(int)v] += weight;
	else if (flag == 0)
		m_Distrib2[(int)v] += weight;
}
void TIEFSWeak2::Train()
{
	for (int i = 0; i < 512; i++)
		m_Classifiacator[i] = m_Distrib2[i] >= m_Distrib1[i] ? 0 : 1;
}
bool TIEFSWeak2::IsFree()
{
	return this->m_free;
}
void TIEFSWeak2::Lock()
{
	this->m_free = false;
}
void TIEFSWeak2::SetEpsilon(double Value)
{
	this->m_Eps = Value;
}
double TIEFSWeak2::GetEpsilon()
{
	return this->m_Eps;
}
int TIEFSWeak2::GetIndex()
{
	return this->m_index;
}
TIEFSWeak2* TIEFSWeak2::Clone()
{
	return new TIEFSWeak2(this);
}
TiXmlElement* TIEFSWeak2::SaveXML()
{
	return NULL;
}
bool TIEFSWeak2::LoadXML(TiXmlElement* e)
{
	return false;
}



TLFHysteresisWeak::TLFHysteresisWeak() : ILFWeak()
{
	// all fatures returns uValue between 0 and 128 
	// we setup thresholds to 32 and 64
	m_t1 = 32;
	m_t2 = 64;
	m_method = HYST_METHOD_MORE;
	m_state = 0;
	m_buffer = new 	TLFRingBuffer(HYST_BUF_LEN, 0);
	InitClassifier();
}

TLFHysteresisWeak::TLFHysteresisWeak(ILFFeature* feature) : ILFWeak(feature)
{
	m_t1 = 32;
	m_t2 = 64;
	m_method = HYST_METHOD_MORE;
	m_state = 0;
	m_buffer = new 	TLFRingBuffer(HYST_BUF_LEN, 0);
	InitClassifier();
}

TLFHysteresisWeak::TLFHysteresisWeak(TLFHysteresisWeak& Weak)
{
	this->m_fWeight = Weak.m_fWeight;
	this->m_pFeature = LFCreateFeature(Weak.Fetaure());
	this->m_t1 = Weak.m_t1;
	this->m_t2 = Weak.m_t2;
	this->m_method = Weak.m_method;
	m_state = 0;
	m_buffer = new 	TLFRingBuffer(HYST_BUF_LEN, 0);
	InitClassifier();
}
TLFHysteresisWeak::TLFHysteresisWeak(TLFHysteresisWeak* weak)
{
	m_t1 = 32;
	m_t2 = 64;
	m_fWeight = 1;
	m_pFeature = NULL;
	m_method = HYST_METHOD_MORE;
	m_state = 0;
	m_buffer = new 	TLFRingBuffer(HYST_BUF_LEN, 0);
	if (weak != NULL)
	{
		this->m_fWeight = weak->m_fWeight;
		this->m_pFeature = LFCreateFeature(weak->Fetaure());
		this->m_t1 = weak->m_t1;
		this->m_t2 = weak->m_t2;
		this->m_method = weak->m_method;
	}
	InitClassifier();
}
TLFHysteresisWeak::~TLFHysteresisWeak()
{
	delete m_buffer;
}

void TLFHysteresisWeak::InitClassifier()
{
	m_a1 = 13.58;
	m_d1 = 133.79;
	m_a2 = 3.28;
	m_d2 = 41.16;
	m_theta = log(sqrt(m_d1) / sqrt(m_d2));
}


void TLFHysteresisWeak::SaveXML(TiXmlElement* parent)
{
	TiXmlElement* e = new TiXmlElement(this->GetName());
	e->SetDoubleAttribute("t1", this->m_t1);
	e->SetDoubleAttribute("t2", this->m_t2);
	e->SetDoubleAttribute("weight", this->m_fWeight);
	e->SetAttribute("method", this->m_method);
	m_pFeature->SaveXML(e);
	parent->LinkEndChild(e);
}

bool TLFHysteresisWeak::LoadXML(TiXmlElement* parent)
{
	if (parent == NULL)
		return false;
	const char* name = parent->Value();
	if (strcmp(name, this->GetName()) != 0)
		return false;
	parent->QueryDoubleAttribute("t1", &this->m_t1);
	parent->QueryDoubleAttribute("t2", &this->m_t2);
	parent->QueryDoubleAttribute("weight", &this->m_fWeight);
	parent->QueryIntAttribute("method", &this->m_method);
	if (m_pFeature != NULL)
		delete m_pFeature;
	TiXmlElement* e = parent->FirstChild()->ToElement();
	name = e->Value();
	m_pFeature = LFCreateFeature(name, 0, 0, 6, 6);
	if (m_pFeature == NULL)
		return false;
	if (!m_pFeature->LoadXML(parent->FirstChild()->ToElement()))
		return false;
	return true;
}

int TLFHysteresisWeak::Classify(TLFImage* pImage, double* value2)
{
	unsigned int value = m_pFeature->uCalcValue(pImage);
	int dt = 0;
	if (m_method == HYST_METHOD_MORE)
	{
		if (value > m_t2)
		{
			if (m_state == 0)
			{
				m_state = 1;
				m_startTime = 0;
				m_buffer->Clear();
				return 0;
			}
			else
			{
				m_startTime++;
				dt = m_startTime;
				m_buffer->Push((double)value);
			}
			if (dt >= HYST_BUF_LEN)
			{
				double v = m_buffer->GetDisp();
				v = 0.5*((v - m_a2)*(v - m_a2)/m_d2 - (v - m_a1)*(v - m_a1)/m_d1);
				return v >= m_theta ? 1 : 0;
			}
			else
				return 0;
		}
		else
		{
			m_state = 0;
			m_buffer->Clear();
			return 0;
		}
	}
	else if (m_method == HYST_METHOD_LESS)
		return value < m_t1 ? 1 : 0;
	else
		return 0;
}

double TLFHysteresisWeak::GetT1()
{
	return m_t1;
}

void   TLFHysteresisWeak::SetT1(double value)
{
	m_t1 = value;
}

double TLFHysteresisWeak::GetT2()
{
	return m_t2;
}
void   TLFHysteresisWeak::SetT2(double value)
{
	m_t2 = value;
}

int TLFHysteresisWeak::GetMethod()
{
	return m_method;
}

void TLFHysteresisWeak::Setmethod(int value)
{
	m_method = value;
}

TLFRingBuffer* TLFHysteresisWeak::GetBuffer()
{
	return m_buffer;
}

TLFDynamicWeak::TLFDynamicWeak(int size1, int size2) : TLFHysteresisWeak()
{
	m_buf1Size = size1 > 0 ? size1 : 1;
	m_buf2Size = size2>  0 ? size2 : 1;;
	
	m_buf1 = (double*)malloc(m_buf1Size*sizeof(double));
	m_buf2 = (double*)malloc(m_buf2Size*sizeof(double));

	m_classifyCount = 0;

	m_buf1_avg = 0;
	m_buf2_avg = 0;

}

TLFDynamicWeak::TLFDynamicWeak(ILFFeature* feature, int size1, int size2) : TLFHysteresisWeak(feature)
{
	m_buf1Size = size1 > 0 ? size1 : 1;
	m_buf2Size = size2>  0 ? size2 : 1;;

	m_buf1 = (double*)malloc(m_buf1Size*sizeof(double));
	m_buf2 = (double*)malloc(m_buf2Size*sizeof(double));

	m_classifyCount = 0;
	m_buf1_avg = 0;
	m_buf2_avg = 0;

}
TLFDynamicWeak::TLFDynamicWeak(TLFDynamicWeak& Weak)
{
	m_t1 = Weak.m_t1;
	m_t2 = Weak.m_t2;
	m_fWeight = Weak.m_fWeight;
	m_method = Weak.m_method;
	m_buf1Size = Weak.m_buf1Size;
	m_buf2Size = Weak.m_buf2Size;

	m_buf1 = (double*)malloc(m_buf1Size*sizeof(double));
	m_buf2 = (double*)malloc(m_buf2Size*sizeof(double));

	memcpy(m_buf1, Weak.m_buf1, m_buf1Size*sizeof(double));
	memcpy(m_buf2, Weak.m_buf2, m_buf2Size*sizeof(double));

	m_classifyCount = Weak.m_classifyCount;
	m_pFeature = LFCreateFeature(Weak.Fetaure());
	m_buf1_avg = 0;
	m_buf2_avg = 0;

}

TLFDynamicWeak::TLFDynamicWeak(TLFDynamicWeak* Weak)
{
	if (Weak != NULL)
	{
		m_t1 = Weak->m_t1;
		m_t2 = Weak->m_t2;
		m_fWeight = Weak->m_fWeight;
		m_method = Weak->m_method;
		m_buf1Size = Weak->m_buf1Size;
		m_buf2Size = Weak->m_buf2Size;

		m_buf1 = (double*)malloc(m_buf1Size*sizeof(double));
		m_buf2 = (double*)malloc(m_buf2Size*sizeof(double));

		memcpy(m_buf1, Weak->m_buf1, m_buf1Size*sizeof(double));
		memcpy(m_buf2, Weak->m_buf2, m_buf2Size*sizeof(double));

		m_classifyCount = Weak->m_classifyCount;
		m_pFeature = LFCreateFeature(Weak->Fetaure());
	}
	else
	{
		m_fWeight = 1.f;
		m_pFeature = LFCreateFeature("", 0, 0, 4, 4);
		m_buf1Size =  1;
		m_buf2Size =  1;

		m_buf1 = (double*)malloc(m_buf1Size*sizeof(double));
		m_buf2 = (double*)malloc(m_buf2Size*sizeof(double));

		m_classifyCount = 0;

		m_t1 = 32;
		m_t2 = 64;
		m_method = HYST_METHOD_MORE;
	}

	m_buf1_avg = 0;
	m_buf2_avg = 0;
}
TLFDynamicWeak::~TLFDynamicWeak()
{
	if (m_buf1Size > 0)
		free(m_buf1);
	if (m_buf2Size > 0)
		free(m_buf2);
}

void TLFDynamicWeak::SaveXML(TiXmlElement* parent)
{
	TiXmlElement* e = new TiXmlElement(this->GetName());
	e->SetDoubleAttribute("t1", this->m_t1);
	e->SetDoubleAttribute("t2", this->m_t2);
	e->SetDoubleAttribute("weight", this->m_fWeight);
	e->SetAttribute("method", this->m_method);
	e->SetAttribute("size1", this->m_buf1Size);
	e->SetAttribute("size2", this->m_buf2Size);
	m_pFeature->SaveXML(e);
	parent->LinkEndChild(e);

}
bool TLFDynamicWeak::LoadXML(TiXmlElement* parent)
{
	if (parent == NULL)
		return false;
	const char* name = parent->Value();
	if (strcmp(name, this->GetName()) != 0)
		return false;
	parent->QueryDoubleAttribute("t1", &this->m_t1);
	parent->QueryDoubleAttribute("t2", &this->m_t2);
	parent->QueryDoubleAttribute("weight", &this->m_fWeight);
	parent->QueryIntAttribute("method", &this->m_method);
	parent->SetAttribute("size1", this->m_buf1Size);
	parent->SetAttribute("size2", this->m_buf2Size);
	if (m_pFeature != NULL)
		delete m_pFeature;
	if (m_buf1Size > 0)
		free(m_buf1);
	if (m_buf2Size > 0)
		free(m_buf2);
	m_buf1 = (double*)malloc(m_buf1Size*sizeof(double));
	m_buf2 = (double*)malloc(m_buf2Size*sizeof(double));

	TiXmlElement* e = parent->FirstChild()->ToElement();
	name = e->Value();
	m_pFeature = LFCreateFeature(name, 0, 0, 6, 6);
	if (m_pFeature == NULL)
		return false;
	if (!m_pFeature->LoadXML(parent->FirstChild()->ToElement()))
		return false;
	return true;
}

int TLFDynamicWeak::Classify(TLFImage* pImage, double* value)
{
	int result = 0;
	if (m_classifyCount < m_buf1Size)
	{
		m_buf1[m_classifyCount] = m_pFeature->uCalcValue(pImage);

	}
	else if (m_classifyCount == m_buf1Size)
	{
		for (int i = 0; i < this->m_buf1Size; i++)
			m_buf1_avg += m_buf1[i];
		m_buf1_avg /= (double)m_buf1Size;
	}
	else
	{
		if (m_buf1_avg < m_t1)
			return 0;

		m_buf2[0] = m_pFeature->uCalcValue(pImage);
		m_buf2_avg = m_buf2[0];

		double t = m_t2*this->m_buf1_avg / 100.;

		if (m_method == HYST_METHOD_MORE)
		{
			return m_buf2_avg - m_buf1_avg > t ? 1 : 0;

		}
		else if (m_method == HYST_METHOD_LESS)
		{
			return m_buf1_avg - m_buf2_avg > t ? 1 : 0;
		}
		else
			return 0;
	}
	m_classifyCount++;
	return result;
}

void TLFDynamicWeak::Reset()
{
	m_classifyCount = 0;
	m_buf1_avg = 0;
	m_buf2_avg = 0;
}

/////////////////////////////////////////////////////////////////////////////////
TLFAccWeak::TLFAccWeak(double t) : ILFWeak()
{
	this->m_counter = 0;
	m_threshold = t;
	m_state = 0;
	m_bg_stability = 5;
    m_delay = 0;
	buf_size = 200;
	m_buffer = NULL;
	m_begin_counter = 0;
}
TLFAccWeak::TLFAccWeak(ILFFeature* feature, double t) : ILFWeak(feature)
{
	this->m_counter = 0;
	m_threshold = t;
	m_state = 0;
	m_bg_stability = 5;
    m_delay = 0;
	buf_size = 200;
	m_buffer = NULL;
	m_begin_counter = 0;
}
TLFAccWeak::TLFAccWeak(TLFAccWeak& Weak)
{
	m_pFeature = LFCreateFeature(Weak.Fetaure());
	m_counter = 0;
	m_fWeight = 1;
	m_threshold = Weak.m_threshold;
	m_bg_stability = Weak.m_bg_stability;
    m_delay = Weak.m_delay;
	m_state = Weak.m_state;
	buf_size = Weak.buf_size;
	m_buffer = NULL;
	m_begin_counter = 0;
}

TLFAccWeak::TLFAccWeak(TLFAccWeak* weak) : ILFWeak()
{
	if (weak != NULL)
	{
		m_pFeature = LFCreateFeature(weak->Fetaure());
		m_threshold = weak->m_threshold;
		m_state = weak->m_state;
		buf_size = weak->buf_size;
		m_bg_stability = weak->m_bg_stability;
        m_delay = weak->m_delay;
	}
	else
	{
		m_threshold = 5;
		m_counter = 0;
		buf_size = 200;
		m_bg_stability = 5;
        m_delay = 0;
	}
	m_buffer = NULL;
	m_begin_counter = 0;
}

TLFAccWeak::~TLFAccWeak()
{
   if (m_buffer != NULL)
	delete m_buffer;
}

   
void TLFAccWeak::SaveXML(TiXmlElement* parent)
{
}
bool TLFAccWeak::LoadXML(TiXmlElement* parent)
{
	return false;
}

int TLFAccWeak::Classify(TLFImage* pImage, double* value)
{
	int result = 0;

	double v = (double)m_pFeature->uCalcValue(pImage);
	if (m_buffer == NULL)
		m_buffer = new TLFRingBuffer(buf_size, v);
	if (m_begin_counter == 0)
		m_begin_counter = LFGetTickCount();
	double a = m_buffer->GetAvg();

	if (LFGetTickCount() - m_begin_counter > 1000*m_delay)
	{
		if (m_state == 0)
		{
			if (v > a + 2 * this->m_threshold || v < a - 2 * this->m_threshold)
			{
				if (m_delay == 0)
				{

					m_state = 1;
				}
				else
				{
					m_state = -1;
					m_delay_counter = LFGetTickCount();
				}
			}
		}
		else if (m_state == -1)
		{
			if (v >= a - m_threshold && v <= a + m_threshold)
			{
				m_state = 0;
				m_delay_counter = 0;
			}
			else
			{
				unsigned long the_delay = LFGetTickCount() - m_delay_counter;
				if (the_delay > 1000 * m_delay)
				{
					m_state = 1;
				}
			}
		}
		else
		{
			// deactivate classifier 
			if (v >= a - this->m_threshold && v <= a + this->m_threshold)
				m_state = 0;
		}

	}
	m_counter++;
	if (m_counter % m_bg_stability == 0)
		m_buffer->Push(v);

	return m_state == 1;
}

void TLFAccWeak::Reset()
{
	m_counter = 0;
	if (m_buffer != NULL)
	{
		delete m_buffer;
		m_buffer = NULL;
	}
}

void TLFAccWeak::SetThreshold(double value)
{
	this->m_threshold = value;
}

double TLFAccWeak::GetThreshold()
{
	return this->m_threshold;
}

int  TLFAccWeak::GetBgStability()
{
	return this->m_bg_stability;
}

void TLFAccWeak::SetBgStability(int value)
{
	if (value >= 1 && value <= 1000)
	{
		this->m_bg_stability = value;
		Reset();
	}
}


int  TLFAccWeak::GetDelay()
{
	return this->m_delay;
}
void TLFAccWeak::SetDelay(int value)
{
	if (value >= 0)
    {
    	this->m_delay = value;
        Reset();
    }
}


int TLFAccWeak::GetBufSize()
{
	return buf_size;
}

void TLFAccWeak::SetBufSize(int value)
{
	if (value >= 1 && value <= 1000)
	{
		buf_size = value;
		Reset();
	}
}

// weak factory
ILFWeak*      LFCreateWeak(const char* lpName)
{
	const char* name = lpName;
    if (strcmp(name, "TCSWeak") == 0)
	{
		return new TCSWeak();
	}
	else if (strcmp(name, "TCSWeakTraining") == 0)
	{
		return new TCSWeakTraining();
	}
	else if (strcmp(name, "TCSSoftWeakTraining") == 0)
	{
		return new TCSSoftWeakTraining();
	}
	else if (strcmp(name, "TCSWeakSign") == 0)
	{
		return new TCSWeakSign();
	}
	else if (strcmp(name, "TCSWeakSignTrainig") == 0)
	{
		return new TCSWeakSignTrainig();
	}
	else if (strcmp(name, "TLFHysteresisWeak") == 0)
	{
		return new TLFHysteresisWeak();
	}
	else
		return NULL;

}
ILFWeak*      LFCreateWeak(ILFWeak* weak)
{
	if (weak == NULL)
		return NULL;
	const char* name = weak->GetName();
	if (strcmp(name, "TCSWeak") == 0)
	{
		TCSWeak* cs = dynamic_cast<TCSWeak*>(weak);
		return new TCSWeak(cs);
	}
	else if (strcmp(name, "TCSWeakTraining") == 0)
	{
		TCSWeakTraining* cs = dynamic_cast<TCSWeakTraining*>(weak);
		return new TCSWeakTraining(cs);
	}
	else if (strcmp(name, "TCSSoftWeakTraining") == 0)
	{
		TCSSoftWeakTraining* cs = dynamic_cast<TCSSoftWeakTraining*>(weak);
		return new TCSSoftWeakTraining(cs);
	}
	else if (strcmp(name, "TCSWeakSign") == 0)
	{
		TCSWeakSign* cs = dynamic_cast<TCSWeakSign*>(weak);
		return new TCSWeakSign(cs);
	}
	else if (strcmp(name, "TCSWeakSignTrainig") == 0)
	{
		TCSWeakSignTrainig* cs = dynamic_cast<TCSWeakSignTrainig*>(weak);
		return new TCSWeakSignTrainig(cs);
	}
	else if (strcmp(name, "TLFHysteresisWeak") == 0)
	{
		TLFHysteresisWeak* cs = dynamic_cast<TLFHysteresisWeak*>(weak);
		return new TLFHysteresisWeak(cs);
	}
	else
		return NULL;
}
