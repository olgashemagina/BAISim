#include "_LF.h"


TLFAvgFeature::TLFAvgFeature() : ILFFeature()
{
	this->m_CalcCount = 0;
	memset(this->m_buffer, 0, FEATURE_AVG_SIZE*sizeof(double));
}

TLFAvgFeature::TLFAvgFeature(TLFAvgFeature* sensor) : ILFFeature(sensor)
{
	if (sensor == NULL)
	{
		this->m_CalcCount = 0;
		memset(this->m_buffer, 0, FEATURE_AVG_SIZE*sizeof(double));
	}
	else
	{
		this->m_CalcCount = sensor->m_CalcCount;
		memcpy(this->m_buffer, sensor->m_buffer, FEATURE_AVG_SIZE*sizeof(double));
	}
}

TLFAvgFeature::TLFAvgFeature(int sxbase, int sybase, int wbase, int hbase) : ILFFeature(sxbase, sybase, wbase, hbase)
{
	this->m_CalcCount = 0;
	memset(this->m_buffer, 0, FEATURE_AVG_SIZE*sizeof(double));
}

unsigned int    TLFAvgFeature::uCalcValue(TLFImage* pImage)
{
	return 0;
}
double           TLFAvgFeature::fCalcValue(TLFImage* pImage)
{
	if (pImage == NULL || pImage->GetIntegralImage() == NULL)
		return -1;
	this->m_CalcCount++;
	double s = double(m_w*m_h);
	double value = pImage->CalcLnSum(this->m_sx, this->m_sy, this->m_w, this->m_h);
	value /= s;
	value /= 255;
	int num = m_CalcCount >= FEATURE_AVG_SIZE ? FEATURE_AVG_SIZE : this->m_CalcCount;
	double avg = 0;
	for (int i = 0; i < num - 1; i++)
	{
		this->m_buffer[i] = this->m_buffer[i + 1];
		avg += this->m_buffer[i];
	}
	this->m_buffer[num-1] = value; 
	avg += value;
	if (this->m_CalcCount <= FEATURE_AVG_SIZE)
		return -1.f;
	else
		return avg / FEATURE_AVG_SIZE;
}


/*
XML io operations
*/
bool TLFAvgFeature::LoadXML(TiXmlElement* parent)
{
	return false;
}
bool TLFAvgFeature::SaveXML(TiXmlElement* parent)
{
	return false;
}

awpRect TLFAvgFeature::GetRect()
{
	awpRect result;
	result.left = this->m_sx;
	result.top = this->m_sy;
	result.right = this->m_sx + this->m_w;
	result.bottom = this->m_sy + this->m_h;
	return result;
}

void TLFAvgFeature::Clear()
{
	this->m_CalcCount = 0;
	memset(this->m_buffer, 0, FEATURE_AVG_SIZE*sizeof(double));
}

TLFSigmaFeature::TLFSigmaFeature() : TLFAvgFeature()
{

}
TLFSigmaFeature::TLFSigmaFeature(int sxbase, int sybase, int wbase, int hbase) : TLFAvgFeature(sxbase, sybase, wbase, hbase)
{

}
TLFSigmaFeature::TLFSigmaFeature(TLFSigmaFeature* feature) : TLFAvgFeature(feature)
{

}

double TLFSigmaFeature::fCalcValue(TLFImage* pImage)
{
	if (pImage == NULL || pImage->GetIntegralImage() == NULL || pImage->GetSqIntegralImage() == NULL)
		return -1;
	this->m_CalcCount++;
	double s = m_w*m_h;
	double value1 = pImage->CalcLnSum(this->m_sx, this->m_sy, this->m_w, this->m_h);
	double value2 = pImage->CalcSqSum(this->m_sx, this->m_sy, this->m_w, this->m_h);
	
	value1 /= s;
	value2 /= s;

	double value = sqrt(value2 - value1*value1) / 100.;

	int num = this->m_CalcCount >= FEATURE_AVG_SIZE ? FEATURE_AVG_SIZE : this->m_CalcCount;
	double avg = 0;
	for (int i = 0; i < num - 1; i++)
	{
		this->m_buffer[i] = this->m_buffer[i + 1];
		avg += this->m_buffer[i];
	}
	this->m_buffer[num - 1] = value;
	avg += value;
	if (this->m_CalcCount <= FEATURE_AVG_SIZE)
		return -1.;
	else
		return avg / FEATURE_AVG_SIZE;
}

