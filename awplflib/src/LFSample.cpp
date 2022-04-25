#include "_LF.h"
//---------------------------------------------------------------------------
TCSSample::TCSSample()
{
    m_Flag          = -1;
    m_Weight        =  0;
	this->m_rating  = 0;
}

TCSSample::TCSSample(TCSSample& sample)
{
	m_Flag = sample.m_Flag;
	m_Weight = sample.m_Weight;
	m_rating = sample.m_rating;

	m_pImage = NULL;

	if (sample.m_pImage == NULL)
		this->m_pImage = NULL;
	else
		awpCopyImage(sample.m_pImage, &m_pImage);
}

TCSSample::TCSSample(TCSSample* sample)
{
	m_Flag = sample->m_Flag;
	m_Weight = sample->m_Weight;
	m_rating = sample->m_rating;

	m_pImage = NULL;
	if (sample->m_pImage == NULL)
		this->m_pImage = NULL;
	else
		awpCopyImage(sample->m_pImage, &m_pImage);
}


TCSSample::~TCSSample()
{

	if (m_pImage != NULL)
		awpReleaseImage(&m_pImage);
}


int TCSSample::GetFlag()
{
    return m_Flag;
}
void TCSSample::SetFlag(int Value)
{
    m_Flag = Value;
}

double TCSSample::GetWeight()
{
    return m_Weight;
}
void TCSSample::SetWeight(double Value)
{
    m_Weight = Value;
}

double  TCSSample::GetRating()
{
	return this->m_rating;
}
void   TCSSample::SetRating(double rating)
{
	this->m_rating = rating;
}




//------------------------------------------------------------------------------
// образец, используемый в системе обучения по исходным изображениям
TCSSoftSample::TCSSoftSample(awpImage* pImage, awpRect& Rect, int Flag, double Weight)
{
        m_pImage = pImage;
        m_Fragment = Rect;
        m_Flag = Flag;
        m_Weight = Weight;
        m_Classifyed = 0;
}

awpImage* TCSSoftSample::GetImage()
{
	return m_pImage;
}

awpRect TCSSoftSample::GetFragment()
{
	return m_Fragment;
}

int TCSSoftSample::GetFlag()
{
	return m_Flag;
}

double TCSSoftSample::GetWeight()
{
	return  m_Weight;
}

void TCSSoftSample::SetWeight(double Value)
{
	m_Weight = Value;
}

int TCSSoftSample::GetClassifyed()
{
	return     m_Classifyed;
}

void TCSSoftSample::SetClassifyed(int Value)
{
	m_Classifyed = Value;
}

TIEFSSample::TIEFSSample(double* data, int num, int flag)
{
	this->m_flag = flag;
	this->m_wight = 0;
	m_deltas = NULL;
	m_deltas = (double*)malloc(num*sizeof(double));
	memcpy(m_deltas, data, num*sizeof(double));
}
TIEFSSample::~TIEFSSample()
{
	if (m_deltas != NULL)
		free(m_deltas);
}

int TIEFSSample::GetFlag()
{
	return this->m_flag;
}
void TIEFSSample::SetFlag(int Value)
{
	this->m_flag = Value;
}

double TIEFSSample::GetWeight()
{
	return this->m_wight;
}
void   TIEFSSample::SetWeight(double Value)
{
	this->m_wight = Value;
}

double TIEFSSample::GetValue(int index)
{
	return this->m_deltas[index];
}
