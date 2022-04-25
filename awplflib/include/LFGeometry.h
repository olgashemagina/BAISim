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
//		File: LFFeatures.h
//		Purpose: Declares Geometry classes
//
//		TLFPoint			- awpPoint wrapper class 
//		TLF2DPoint			- awp2DPoint wrapper class 
//		TLFRect				- awpRect wrapper class 
//		TLF2DRect			- awp2DRect wrapper class 
//		TLFTriangle			- the traingle with awp2DPoint vertexes 
//		TLFContour          - awpContour wrapper class 
//		TLF2DContour		- awp2DContour wrapper class 
//		TLFLineSegment      - awpLineSegment wrapper class 
//		TLF2DLineSegment	- awp2DLineSegmnet wrapper class 
//		TLF2DOpenPolygon	- open polygon class 
//      TLFTriangleList		- list of triangles 
//
//      CopyRight 2004-2018 (c) NN-Videolab.net
//M*/
#ifndef _LF_GEOMETRY_H_
#define _LF_GEOMETRY_H_

/** \defgroup LFGeometry
*	Implementation geometry objects in the Locate Framework 
*   @{
*/

/**
\class TLFPoint
\brief LF Point Descriptor based on awpPoint structure 
*/
class TLFPoint : public TLFObject
{
protected:
	awpPoint m_Point;
public:
	TLFPoint();
	TLFPoint(awpPoint p);
	TLFPoint(TLFPoint& p);
	virtual ~TLFPoint();


	awpPoint GetPoint();
	void SetPoint(awpPoint Point);
	double Distance(TLFPoint& p);
	short GetX();
	short GetY();

	/** Load form XML*/
	bool LoadXML(TiXmlElement* parent);
	/** Save to XML*/
	bool SaveXML(TiXmlElement* parent);
	virtual const char* GetName()
	{
		return "TLFPoint";
	}
};
/**
\class TLF2DPoint
\brief LF 2D Point Double Descriptor based on awp2DPoint structure 
*/
class TLF2DPoint : public TLFObject
{
protected:
	awp2DPoint m_point;
public:
	TLF2DPoint();
	TLF2DPoint(awpPoint p);
	TLF2DPoint(awp2DPoint p);
	TLF2DPoint(TLFPoint& p);
	TLF2DPoint(TLF2DPoint& p);
	TLF2DPoint(double x, double y);

	virtual ~TLF2DPoint();
	awp2DPoint GetPoint();
	void SetPoint(awp2DPoint Point);
	double Distance(TLF2DPoint& p);

	double GetX();
	double GetY();
	/** Load form XML*/
	bool LoadXML(TiXmlElement* parent);
	/** Save to XML*/
	bool SaveXML(TiXmlElement* parent);

	virtual const char* GetName()
	{
		return "TLF2DPoint";
	}
};
/**
\class TLFRect
\brief LF Rect Descriptor based on awpRect structure
*/
class TLFRect : public TLFObject
{
protected:
	awpRect m_Rect;
public:
	TLFRect();
    TLFRect(awpRect r);
	TLFRect(int left, int top, int w, int h);
	TLFRect(TLFRect& r);
	virtual ~TLFRect();
	awpRect GetRect();
	void SetRect(awpRect Rect);
	void SetRect(int left, int top, int w, int h);

	double RectOverlap(TLFRect& rect);
	double RectOverlap(awpRect& rect);
	double Distance(TLFRect& rect);
	bool  IsCover(TLFRect& rect);
	bool  IsCover(awpRect& rect);
	bool  Touch(TLFRect& rect);
    double CoverSquare(TLFRect& rect);
    double CoverSquare(awpRect& rect);
	double Square();

	void Shift(int dx, int dy);
	void Scale(double coef);
	void Unioun(awpRect rect);
	void Inflate(int x_value, int y_value);

	bool IsEmpty();
	void Clear();

	AWPSHORT Width();
	AWPSHORT Height();

	AWPSHORT  Left();
	AWPSHORT  Top();
	AWPSHORT Right();
	AWPSHORT Bottom();
	awpPoint Center();

	/** Load form XML*/
	bool LoadXML(TiXmlElement* parent);
	/** Save to XML*/
	bool SaveXML(TiXmlElement* parent);

   virtual const char* GetName()
   {
	return "TLFRect";
   }
};
/**
\class TLF2DRect
\brief LF 2D Rect Double Descriptor based on couple of awp2DPoint structures
*/
class TLF2DRect : public TLFObject
{
	awp2DPoint m_LeftTop;
	awp2DPoint m_RightBottom;
public:
	TLF2DRect();
	TLF2DRect(awp2DPoint left_top, awp2DPoint right_bottom);
	TLF2DRect(TLF2DRect& rect);
    TLF2DRect(awpRect r);

	void SetRect(TLF2DRect& rect);

	awp2DPoint GetLeftTop();
	awp2DPoint GetRightBottom();

    void SetVertexes(double x1, double x2, double y1, double y2);

	double Width();
	double Height();
	awp2DPoint Center();

	double left();
	double right();
	double top();
	double bottom();

	/** Load form XML*/
	bool LoadXML(TiXmlElement* parent);
	/** Save to XML*/
	bool SaveXML(TiXmlElement* parent);

	virtual const char* GetName()
	{
		return "TLF2DRect";
	}
};
/**
\class TLFTriangle
\brief LF Trianglee Descriptor based on triple of awp2DPoint structures
*/
class TLFTriangle : public TLFObject
{
protected:
	awp2DPoint m_v1;
	awp2DPoint m_v2;
	awp2DPoint m_v3;
	bool m_hasPoint;
	awp2DPoint m_cm;
    double     m_v;
    double     m_h;
	int m_index;
    int m_depth;
	TLFTriangle* m_parent;
	TLFTriangle* m_child[4];
	TLFTriangle* CreateTriangle(int index, int depth, int gi);

public:
	TLFTriangle(awp2DPoint v1, awp2DPoint v2, awp2DPoint v3, TLFTriangle* parent, int depth, int index);
    TLFTriangle(TLFTriangle* t);
	virtual ~TLFTriangle();

	TLFTriangle* GetParent();
	TLFTriangle* GetChild(int index);
    void Process(awpImage* img, awpImage* vg, awpImage* hg);

	awp2DPoint _cm();
	awp2DPoint _v1();
	awp2DPoint _v2();
	awp2DPoint _v3();
	bool	   _hasPoint();
    int        _depth();
    int        _count();
    double     _v();
    double     _h();
    int        _index();
    double     _perim();
    double     _square();
    double     _AB();
    double     _BC();
    double     _CA();
};

class TLFTriangleList : public TLFObjectList
{
protected:
	awpImage* m_procImage;
	awpImage* m_vg;
	awpImage* m_vh;
	void ProcessImage(awpImage* img);
public:
	TLFTriangleList();
	~TLFTriangleList();

	void Process(TLFImage& img);
	void Process(awpImage* img);

	TLFTriangle* GetTriangle(int index);
	void AddTriangle(TLFTriangle* t);
	void Save(const char* lpFileName);
	double Ro(TLFTriangleList* list);
};
/**
\class TLFContour
\brief LF Contour (Polygon) Descriptor based on awpContour structure
*/
class TLFContour : public TLFObject
{
protected:
	awpContour* m_contour;
public:
	TLFContour();
	TLFContour(TLFContour& contour);
	virtual ~TLFContour();

	void AddPoint(awpPoint& point);
	void RemovePoint(AWPDWORD idx);

	int GetNumPoints();
	awpPoint GetPoint(AWPDWORD idx);
	awpContour* GetContour();

	/** Load form XML*/
	bool LoadXML(TiXmlElement* parent);
	/** Save to XML*/
	bool SaveXML(TiXmlElement* parent);

	virtual const char* GetName()
	{
		return "TLFContour";
	}
};

/**
\class TLF2DContour
\brief LF 2DContour (Polygon) Descriptor based on awp2DContour structure
*/
class TLF2DContour : public TLFObject
{
protected:
	awp2DContour* m_contour;
public:
	TLF2DContour();
	TLF2DContour(TLF2DContour& contour);
	virtual ~TLF2DContour();
    TLF2DContour& operator = (const TLF2DContour& other);

	void AddPoint(awp2DPoint& point);
	void RemovePoint(AWPDWORD idx);

	int GetNumPoints();
	awp2DPoint GetPoint(AWPDWORD idx);
    void SetPoint(AWPDWORD idx, awp2DPoint& p);
	awp2DContour* GetContour();

	/** Load form XML*/
	bool LoadXML(TiXmlElement* parent);
	/** Save to XML*/
	bool SaveXML(TiXmlElement* parent);

	virtual const char* GetName()
	{
		return "TLF2DContour";
	}
};
/**
\class TLFLineSegment
\brief LF Line Segment (vector) Descriptor based on awpLineSegment structure
*/
class TLFLineSegment : public TLFObject
{
protected:
	awpLineSegment m_segment;
public:
	TLFLineSegment();
    TLFLineSegment(TLFLineSegment& segment);

    void SetStart(awpPoint p);
    void SetFihish(awpPoint p);

    awpPoint GetStart();
    awpPoint GetFinish();

	/** Load form XML*/
	bool LoadXML(TiXmlElement* parent);
	/** Save to XML*/
	bool SaveXML(TiXmlElement* parent);

	virtual const char* GetName()
	{
		return "TLFLineSegment";
	}
};

/**
\class TLF2DLineSegment
\brief LF Line Segment (vector) Descriptor based on awp2DLineSegment structure
*/
class TLF2DLineSegment : public TLFObject
{
protected:
	awp2DLineSegment m_segment;
public:
	TLF2DLineSegment();
    TLF2DLineSegment(TLF2DLineSegment& segment);

    void SetStart(awp2DPoint p);
    void SetFihish(awp2DPoint p);

    awp2DPoint GetStart();
    awp2DPoint GetFinish();

    awp2DLineSegment GetSegment();


	/** Load form XML*/
	bool LoadXML(TiXmlElement* parent);
	/** Save to XML*/
	bool SaveXML(TiXmlElement* parent);

	virtual const char* GetName()
	{
		return "TLF2DLineSegment";
	}
};
/**
\class TLF2DLineSegment
\brief LF Line Segment (vector) Descriptor based on awp2DLineSegment structure
*/
class TLF2DOpenPolygon : public TLFObjectList
{
public:
    TLF2DOpenPolygon();
    TLF2DOpenPolygon& operator=(TLF2DOpenPolygon& other);
	TLF2DLineSegment* GetSegment(int idx);
    void AddSegment(TLF2DLineSegment* segment);

	/** Load form XML*/
	bool LoadXML(TiXmlElement* parent);
	/** Save to XML*/
	bool SaveXML(TiXmlElement* parent);

	virtual const char* GetName()
	{
		return "TLFOpenPolygon";
	}
};
/** @} */ /*  end of LFGeometry group */
#endif //_LF_GEOMETRY_H_