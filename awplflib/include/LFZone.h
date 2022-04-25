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
//	Locate Framework  4.0
//  File: LFZone.h
//  Purpose: Zones and masks support
//	Copyright (C) 2004-2018, NN-Videolab.net, all rights reserved.
//M*/

#ifndef LFZONEH
#define LFZONEH

/** \defgroup LFZones
*	Imlementationi zones of interest in the Locate Framework
*   @{
*/


typedef struct
{
	int        	index;
    TEZoneTypes type;
    awp2DPoint 	p;
}SZoneVertex;

/**
\class TLFZone
\brief LF Zone Descriptor
*/
class TLFZone : public TLFObject
{
protected:
	double				m_testRadius;

	TEZoneTypes    		m_ZoneType;
	TLF2DRect			m_Rect;
	TLF2DContour		m_contour;
    TLF2DLineSegment    m_segment;
    TLF2DOpenPolygon    m_openPolygon;
    int 				m_classID;		

protected:
	bool IsPointNearRect(TLF2DPoint& point);
    bool IsPointNearContour(TLF2DPoint& point);
    bool IsPointNearLineSegment(TLF2DPoint& point);
    bool IsPointNearOpenPolygon(TLF2DPoint& point);

    SZoneVertex SelectRectVertex(TLF2DPoint& point);
    SZoneVertex SelectContourVertex(TLF2DPoint& point);
    SZoneVertex SelectLineSegmentVertex(TLF2DPoint& point);
    SZoneVertex SelectOpenPolygonVertex(TLF2DPoint& point);

    void SetRectVertex(SZoneVertex& point);
    void SetContourVertex(SZoneVertex& point);
    void SetLineSegmentVertex(SZoneVertex& point);
    void SetOpenPolygonVertex(SZoneVertex& point);

public:
	/** Default constructor*/
    TLFZone();
    /** Constructor with zone type*/
	TLFZone(TEZoneTypes type);
	/** Construct zone with TLF2DRect*/
	TLFZone(TLF2DRect& rect);
	/** Construct zone with TLF2DContour*/
	TLFZone(TLF2DContour& contour);
	/** Construct zone with TLF2DLineSegment*/
    TLFZone(TLF2DLineSegment& segmnet);
	/** Construct zone with TLF2DOpenPolygon*/
    TLFZone(TLF2DOpenPolygon& polygon);
 	/** Copy consructor*/
	TLFZone(TLFZone& zone);
    TLFZone& operator =(TLFZone& zone);


	/** Rectangular zone*/
	bool IsRect();
    /** Contour zone*/
    bool IsContour();
    /** Line segmnet zone*/
    bool IsLineSegment();
    /** Open polygon zone*/
	bool IsOpenPolygon();
	/**Circle zone */
	bool IsCircle();
    /** Zone type */
    TEZoneTypes  GetZoneType();
    const char* ZoneType();

	/** Rectangle. NULL if not rect*/
	TLF2DRect* GetRect();
	/** Contour points. NULL if not contour*/
	TLF2DContour* GetContour();
	/** Line segment. NULL if not Line segment */
	TLF2DLineSegment* GetLineSegmnet();
    /** Open Polygon. NULL if not open polygon */
	TLF2DOpenPolygon* GetOpenPolygon();

	TLF2DRect* GetBounds();

	/** Square of Zone*/
	double Square();
	/** Square of Zone on particular image*/
    double SquareOnImage(awpImage* img);
    /** Is point near vertex*/
    bool IsPointNearVertex(TLF2DPoint& point);
    /** Select vertex near the point. vertex.index < 0 means
    vertex is not selected*/
    SZoneVertex SelectVertex(TLF2DPoint& point);
    /** sets new coordinates to specific vertex*/
    void SetVertex(SZoneVertex& vertex);

    /** Load form XML*/
    virtual bool LoadXML(TiXmlElement* parent);
    /** Save to XML*/
    virtual bool SaveXML(TiXmlElement* parent);

    int GetClassID()

    {
    	return m_classID;
    }

    void SetClassID(int id)
    {
		m_classID = id;
    }

	virtual const char* GetName()
	{
		return "TLFZone";
	}
};

/**
\class TLFZones
\brief LF Zones List
*/
class TLFZones : public TLFObjectList
{
private:
	int IsPointInZone(int x, int y, double dx = 1, double dy = 1);
	bool IsPointInContour(int x, int y, TLF2DContour* list, double dx = 1, double dy = 1);
public:
	/** Default constructor*/
	TLFZones();
	/** Copy consructor*/
	TLFZones(TLFZones& zones);
    TLFZones& operator=(TLFZones& zones);

    /** Add zone to list*/
	void AddZone(TLFZone* zone);
    /** Retrieve zone from list*/
	TLFZone* GetZone(int index);
    /** Make mask from particular image based on zones information*/
	awpImage* GetMaskImage(awpImage* source, bool use_class_info = false);
    /** Make mask from particular image based on zones information*/
	awpImage* GetMaskImage(awpImage* source, ILFScanner* scanner);
    /** Test point belonging paricular zone with index "idx"*/
	bool TestPoint(int x, int y, int idx);
    /** Test rect belonging zones*/
    int TestRect(awpRect r, int w, int h);
    /** Load form XML*/
    virtual bool LoadXML(TiXmlElement* parent);
    /** Save to XML*/
    virtual bool SaveXML(TiXmlElement* parent);
    /** Load form XML*/
    bool Load(const char* lpFileName);
    /** Save to XML*/
    bool Save(const char* lpFileName);

	virtual const char* GetName()
	{
		return "TLFZones";
	}
};
/** @} */ /*  end of LFZones group */

#endif //LFZONEH