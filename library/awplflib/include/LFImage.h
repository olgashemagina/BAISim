#pragma once

#include "LFCore.h"

/** @brief Raster data in the awpImage format
*		   Supports Load, Save,
*		   LoadDump, SaveDump
*		   IntegralImage for each channel (for RGB 24 bit imiages)
*/
class  TLFImage : public TLFObject, public ILFImage
{
protected:

	double* m_lnpix;
	double* m_sqpix;
	double* m_rlnpix;
	double* m_glnpix;
	double* m_blnpix;

	double* m_rsqlnpix;
	double* m_gsqlnpix;
	double* m_bsqlnpix;


	awpImage* m_pImage;					/*source image in the grayscale*/
	awpImage* m_pIntegralImage;         /*integral image*/
	awpImage* m_pIntegralSquareImage;   /*integral square image*/

	awpImage* m_pRed;
	awpImage* m_pGreen;
	awpImage* m_pBlue;

	awpImage* m_pIntegralRed;
	awpImage* m_pIntegralGreen;
	awpImage* m_pIntegralBlue;

	awpImage* m_pSqIntegralRed;
	awpImage* m_pSqIntegralGreen;
	awpImage* m_pSqIntegralBlue;

public:
	TLFImage();
	TLFImage(TLFImage& pImage) = default;
	TLFImage& operator=(TLFImage& other);
	virtual  ~TLFImage();

	virtual bool LoadFromFile(const char* szFileName);
	virtual bool SaveToFile(const char* szFileName);

	// memory exchange
	virtual bool SaveDump(const char* szFileName);
	virtual bool LoadDump(const char* szFileName);
	virtual bool LoadMemDump(const unsigned char* pDump, int nDumpSize);

#ifdef WIN32
	//	virtual bool LoadResourceImage(char* lpName);
#endif

	//properties
	awpImage* GetImage();
	awpImage* GetIntegralImage();
	awpImage* GetSqIntegralImage();

	awpImage* GetRedImage();
	awpImage* GetGreenImage();
	awpImage* GetBlueImage();

	awpImage* GetRedIntegral();
	awpImage* GetGreenIntegral();
	awpImage* GetBlueIntegral();

	awpImage* GetSqRedIntegral();
	awpImage* GetSqGreenIntegral();
	awpImage* GetSqBlueIntegral();


	virtual bool SetImage(awpImage* pImage);
	bool         CopyImage(awpImage** ppImage);

	double CalcLnSum(int x, int y, int w, int h);
	double CalcSqSum(int x, int y, int w, int h);

	double CalcRLnSum(int x, int y, int w, int h);
	double CalcGLnSum(int x, int y, int w, int h);
	double CalcBLnSum(int x, int y, int w, int h);

	void FreeImages();

#ifdef WIN32
	void  Show(HDC dc, int x, int y, int w, int h, int from_x, int from_y);
	void  StretchShow(HDC dc, int x, int y, int w, int h, int from_x, int from_y);
#endif
	virtual const char* GetName()
	{
		return "TLFImage";
	}

	/*
		ILFImage implementation
	*/
	// [todo:] possible we don't need unsigned char ????
	virtual double* integral(int channel);
	virtual double* integral2(int channel);
	virtual int width();
	virtual int height();
	virtual int channels();
};
/** @brief List TLFImage objects
*/
class TLFImageList : public TLFObjectList
{
public:
	void AddImage(TLFImage* img);
	/*	void AddImage(awpImage* img)
	   {
		   TLFImage* qq = new TLFImage();
		   qq->SetImage(img);
		   AddImage(qq);
	   }*/
	TLFImage* GetImage(int index);
};
