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
//		File: LFCore.h
//		Purpose: Contains coomons for Locate Framework
//
//      CopyRight 2004-2022 (c) NN-Videolab.net
//M*/

#pragma once
#ifndef  __lf_core_h__
#define  __lf_core_h__

#include "awpipl.h"
#include "LFInterface.h"
#include <string>
#include <vector>
typedef std::vector<std::string>      	TLFStrings;
typedef std::string 					TLFString;

/** \defgroup LFCommon
*	Commnon classes for Locate Framework
*   @{
*/
/** @brief Base class for all objects whithing library. Each object has the virtual destructor and name.
*/
class TLFObject
{
protected:
public:
	TLFObject();
	virtual ~TLFObject();
	virtual const char* GetName();
};
typedef int(*TLFListSortCompare)(void* Item1, void* Item2);
/** @brief List of LFObjects
*/
class TLFObjectList : public TLFObject
{
private:
	TLFObject** m_List;
	int              m_Count;
	int              m_Capacity;
protected:
	virtual void Grow();
	void Put(int index, TLFObject* pObject);
public:
	TLFObjectList();
	~TLFObjectList();

	int Add(TLFObject* pObject);
	virtual void Clear();
	void Delete(int index);

	void Exchange(int index1, int index2);
	TLFObjectList* Expand();
	TLFObject* Extract(TLFObject* pObject);
	TLFObject* Get(int index);

	TLFObject* First();
	int IndexOf(TLFObject* pObject);
	void Insert(int index, TLFObject* pObject);
	TLFObject* Last();
	void Move(int nCurIndex, int nNewIndex);
	int Remove(TLFObject* pObject);

	void Pack();
	void Sort(TLFListSortCompare Compare);

	// property
	int GetCapacity();
	void SetCapacity(int NewCapacity);

	int GetCount();
	void SetCount(int NewCount);

	TLFObject** GetList();
	virtual const char* GetName()
	{
		return "TLFObjectList";
	}
};
/** @brief Linked list node
*/
class TLFListNode : public TLFObject
{
private:
	TLFListNode* m_next;
	TLFObject* m_element;
public:
	TLFListNode(TLFObject* object);
	TLFObject* GetElement();
	void       SetElement(TLFObject* object);
	TLFListNode* NextNode();
	void		   SetNextNode(TLFListNode* node);

	virtual const char* GetName()
	{
		return "TLFNode";
	}
};
/** @brief Linked list
*/
class TLFList : TLFObject
{
	TLFListNode* m_headNode;
public:
	TLFList();
	virtual ~TLFList();

	TLFObject* First();
	TLFObject* Last();
	TLFObject* Next(TLFObject* object);
	/*
		полная очистка списка.
	*/
	void Clear(bool del = false);
	void PushBack(TLFObject* object);
	void PopBack();
	TLFObject* Pop(TLFObject* object);

	virtual const char* GetName()
	{
		return "TLFList";
	}
};
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
	TLFImage(TLFImage& pImage);
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
#endif 
/** @} */ /*  end of LFCommon group */
