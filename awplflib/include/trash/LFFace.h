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
//		File: LFFace.h
//		Purpose: contains interface for face recognition system 
//
//      CopyRight 2004-2020 (c) NN-Videolab.net
//M*/

#ifndef _lf_face_h_
#define _lf_face_h_
/** \addtogroup LFDescriptors
*   @{
*/

/**
* @brief obsolete
*/
#define NUM_POINTS 26
/**
* @brief obsolete
*/
typedef enum  { turnNone = 0, turnLeft, turnRight } ELFFaceTurn;

/**
* @brief obsolete
*/
typedef struct
{
	int idx;
	int v1;
	int v2;
	int v3;
}SLFTriangle;

/**
* @brief obsolete
*/
class TLFFaceImageDescriptor : public TLFObject
{
protected:
	TLFRoi m_roi;
	awp2DPoint      m_points[NUM_POINTS - 2];
	awp2DPoint      m_fmodel[NUM_POINTS];
	TLFTriangle*    m_Triangles[32];

	bool            m_glasses;
	bool            m_bread;
	ELFFaceTurn     m_turn;
	bool            m_Empty;
	TLFTriangleList m_List;
	void            AddTriangle(TLFTriangle*  t);
public:
	TLFFaceImageDescriptor();
	TLFFaceImageDescriptor(const char* lpFileName);
	virtual ~TLFFaceImageDescriptor();

	// points
	TLFRoi* GetRoi();
	awpPoint GetPoint(int index);
	void     SetPoint(int index, awpPoint p);
	void     SetEyes(awpPoint le, awpPoint re);
	void     Reset();
	// box
	awpRect  GetFaceBoxMinMax();
	awpRect  GetFaceBoxForPredictor(awpRect* faceRect);
	// image
	awpImage* GetFaceImageForPredictor(awpImage* img, awpRect* faceBox);

	// model
	void     CreateModel();
	void     NormModel(awp2DPoint ole, awp2DPoint ore);
	void     UpdateModel(int sel, awp2DPoint le, awp2DPoint re, awp2DPoint ole, awp2DPoint ore);
	// triangles
	bool     CreateTriangles(int depth);
	TLFTriangle* GetTriangle(int index);
	TLFTriangleList* GetList(int index);
	SLFTriangle* GetTIndex();
	// shape
	void GetShape(awp2DPoint* shape);

	// io
	bool LoadFromFile(const char* lpFileName);
	void SaveToFile(const char* lpFileName);
	TiXmlElement* SaveXML(const char* lpFileName, awpRect* faceBox);

	// scale
	void Scale(double factor);
	void Shift(awpPoint pp);

	// properties
	bool IsGlasses();
	bool IsBread();
	bool IsTurnLeft();
	bool IsTurnRight();
	bool IsEmpty();
	void SetTurnLeft(bool value);
	void SetTurnRight(bool value);
	void SetGlasses(bool value);
	void SetBread(bool value);
public:
	SLFTriangle     m_tindex[32];
};
/** @} */ /*  end of LFFace group */
#endif //_lf_face_h_ 