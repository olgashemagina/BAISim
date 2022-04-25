#include "_LF.h"

ILFImageProcessor::ILFImageProcessor()
{}
ILFImageProcessor::~ILFImageProcessor()
{}

int ILFImageProcessor::GetNumParameters()
{
	return m_Params.GetCount();
}
TLFParameter* ILFImageProcessor::GetParameter(int index)
{
	if (index < 0 || index > m_Params.GetCount())
		return NULL;
	return (TLFParameter*)m_Params.Get(index);
}
bool ILFImageProcessor::SetParameterValue(double Value, int index)
{
	if (index < 0 || index > m_Params.GetCount())
		return false;
	return ((TLFParameter*)m_Params.Get(index))->SetValue(Value);
}

TLFRect ILFImageProcessor::GetRoi()
{
	return m_ImageRoi;
}
void ILFImageProcessor::SetRoi(TLFRect& Rect)
{
	//
	m_ImageRoi = Rect;
}

TLFImage* ILFImageProcessor::GetSourceImage()
{
	TLFImage* pimg = new TLFImage();
	pimg->SetImage(m_SourceImage.GetImage());
	return pimg;
}

TLFImage* ILFImageProcessor::GetResultImage()
{
	TLFImage* pimg = new TLFImage();
	pimg->SetImage(m_ResultImage.GetImage());
	return pimg;
}

bool ILFImageProcessor::SetSourceImage(TLFImage* pImage)
{
	if (pImage == NULL)
		return false;
	if (pImage->GetImage() == NULL)
		return false;
	// проверка изображения 
	awpImage* img = pImage->GetImage();
	if (awpCheckImage(img) != AWP_OK)
		return false;
	m_SourceImage.SetImage(img);
	return Process();
}