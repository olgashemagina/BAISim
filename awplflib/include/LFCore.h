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
#include <sstream>

#include "tinyxml.h"


#ifdef WIN32
#include <windows.h>
#endif

extern "C"
{
#include "awpipl.h"
#ifdef WIN32
#include <Rpc.h>
#else
#include <uuid/uuid.h>
	typedef uuid_t UUID;
#endif
}


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
	TLFObject* Get(int index) const;

	TLFObject* First() const;
	int IndexOf(TLFObject* pObject) const;
	void Insert(int index, TLFObject* pObject);
	TLFObject* Last() const;
	void Move(int nCurIndex, int nNewIndex);
	int Remove(TLFObject* pObject);

	void Pack();
	void Sort(TLFListSortCompare Compare);

	// property
	int GetCapacity() const;
	void SetCapacity(int NewCapacity);

	int GetCount() const;
	void SetCount(int NewCount);

	TLFObject** GetList() const;
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



template <class T>
std::string TypeToStr(const T& t)
{
	std::ostringstream oss; // create a stream
	oss << t; // insert value to stream
	return oss.str(); // extract string and copy
}

template <class T>
T StrToType(std::string const& val)
{
	std::stringstream ss; // create a stream
	ss << val; // insert value to stream
	T t;
	ss >> t;
	return t;
}

#endif 
/** @} */ /*  end of LFCommon group */
