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
//		File: LFLandmarks.h
//		Purpose: Contains landmark  classes
//
//      CopyRight 2004-2021 (c) NN-Videolab.net
//M*/
#ifndef _lf_landmarks_h_
#define _lf_landmarks_h_

/** \defgroup LFLandmarks
*	Implementation LFLandmark
*   @{
*/

class TLFDBLandmarks;
class TLFLandmarkAttr;
class TLFLandmarkAttributes;
class TLFLandmarkFiles;
class TLFLandmark : public TLFObject
{
friend class TLFLandmarkFile;
protected:
	awp2DPoint		 m_landmark;
	AWPDOUBLE           m_status;
	TLFLandmarkAttr* m_attr;

	TiXmlElement* SaveXML();
	static TLFLandmark* LoadXML(TiXmlElement* parent, TLFLandmarkAttributes* attrs);

public:
	TLFLandmark(TLFLandmarkAttr* attr, awp2DPoint point, AWPDOUBLE status = 0);
	virtual ~TLFLandmark();
	virtual const char* GetName(){ return "TLFLandmark"; }
	
	const char* GetId();
	const char* ClassName();
	int			Color();

	awp2DPoint landmark();
	AWPDOUBLE  x();
	AWPDOUBLE  y();
	AWPDOUBLE  Status();

	void SetPoint(awp2DPoint point);
	void SetX(AWPDOUBLE x);
	void SetY(AWPDOUBLE y);
	void SetStatus(AWPDOUBLE value);
};

class TLFLandmarkAttr : public TLFObject
{
friend class TLFLandmarkAttributes;
protected:
	TLFString		m_ClassName;
	TLFString		m_id;
	int				m_color; //web color
	void SetID(TLFString& strUUID);

	TiXmlElement* SaveXML();
	static TLFLandmarkAttr* LoadXML(TiXmlElement* parent);
public:
	TLFLandmarkAttr();
	TLFLandmarkAttr(TLFString& id, int color, const char* className);
	virtual ~TLFLandmarkAttr();
	virtual const char* GetName(){ return "TLFLandmarkAttr"; }

	const char* ClassName();
	void SetClassName(const char* value);

	int Color();
	void SetColor(int value);
	void SetColor(AWPBYTE r, AWPBYTE g, AWPBYTE b);

	const char* GetID();
};

class TLFLandmarkAttributes : public TLFObject
{
	friend class TLFDBLandmarks;
protected:
	TLFObjectList	m_attributes;
	TiXmlElement* SaveXML();
	static TLFLandmarkAttributes* LoadXML(TiXmlElement* parent);
public: 
	TLFLandmarkAttributes();
	virtual ~TLFLandmarkAttributes();
	virtual const char* GetName(){ return "TLFLandmarkAttributes"; }

	void Clear();
	void Append(TLFLandmarkAttr* attr);
	TLFLandmarkAttr* Attribute(int index);
	TLFLandmarkAttr* Attribute(const TLFString& uuid);
	void Delete(int index, TLFLandmarkFiles* files);
	int Count();

	TLFObjectList* GetList();
};

class TLFLandmarkFile : public TLFObject
{
friend class TLFLandmarkFiles;
protected:
	TLFObjectList	m_landmarks;
	TLFString		m_fileName;

	TiXmlElement* SaveXML();
	static TLFLandmarkFile* LoadXML(TiXmlElement* parent, TLFLandmarkAttributes* attrs);
public:
	TLFLandmarkFile(const char* fileName);
	virtual ~TLFLandmarkFile();
	virtual const char* GetName(){ return "TLFLandmarkFile"; }

	const char* FileName();
	void SetFileName(const char* fileName);

	int Count();
	TLFLandmark* Landmark(int index);
	void Delete(int index);
	void Delete(const TLFString& strUUID);
	void Append(TLFLandmark* landmark);
};

class TLFLandmarkFiles : public TLFObject
{
	friend class TLFDBLandmarks;
protected:
	TLFObjectList	m_files;
	int GetFileIndex(const char* fileName);
	TiXmlElement* SaveXML();
	static TLFLandmarkFiles* LoadXML(TiXmlElement* parent, TLFLandmarkAttributes* attrs);
public:
	TLFLandmarkFiles();
	virtual ~TLFLandmarkFiles();
	virtual const char* GetName(){ return "TLFLandmarkFiles"; }

	int Count();
	TLFLandmarkFile* File(int index);
	TLFLandmarkFile* File(const char* fileName);
	void Delete(int index);
	void Delete(const char* fileName);
	void Append(TLFLandmarkFile* file);
	void Clear();
};

class TLFDBLandmarks : public TLFObject
{
private: 
	TLFString				m_fileName;
	TLFLandmarkAttributes*	m_attributes;
	TLFLandmarkFiles*		m_files;
protected:
	void SetAttributes(TLFLandmarkAttributes& attributes);
	void SetFileName(const char* fileName);
	bool Save();
	bool Load(const char* fileName);
public:
	TLFDBLandmarks();
	virtual ~TLFDBLandmarks();
	virtual const char* GetName(){ return "TLFDBLandmarks"; }

	bool Connect(const char* fileName);
	void Close();
	TLFLandmarkAttributes* Attributes();
	TLFLandmarkFiles*	   Files();
	const char*			   FileName();


	static TLFDBLandmarks* CreateDatabase(TLFLandmarkAttributes& attributes, const char* fileName);
};
/** @} */ /*  end of LFLandmarks group */
#endif //_lf_landmarks_h_