#pragma once
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
//		File: LFinterface.h
//		Purpose: Contains abstracts for Locate Framework
//
//      CopyRight 2004-2018 (c) NN-Videolab.net
//M*/


typedef struct _LFPoint {
	double x;
	double y;
} LFPoint, *LPLFPoint;

typedef struct _LFRect {
	double x;
	double y;
	double w;
	double h;
} LFRect, * LPLFRect;

class ILFImage
{
public:
	virtual bool LoadFromFile(const char* fileName)   = 0;
	virtual bool SaveToFile(const char* fileName) = 0;
	virtual double* integral(int channel)  = 0;
	virtual double* integral2(int channel) = 0;
	virtual int width() = 0;
	virtual int height() = 0;
	virtual int channels() = 0;
};

typedef void* ILFStorage;

// paremeters
class ILFParameter
{
public:
	virtual const char* name() = 0;
	virtual const char* comment() = 0;

	virtual const char* value() = 0;
	virtual const double doubleValue() = 0;
	virtual const double intValue() = 0;

	virtual void setValue(const char* value) = 0;
	virtual void setDouble(const double value) = 0;
	virtual void setInt(const int value) = 0;
};

class ILFParameters
{
public:
	virtual int Count() = 0;
	virtual ILFParameter* Parameter(unsigned int index) = 0;
	virtual ILFParameter* Parameter(const char* name) = 0;
};

typedef void(*OnMediaNewFrame)(int Frame, ILFImage* img);

class ILFMediaSource
{
protected:
	OnMediaNewFrame m_OnNewFrame;
public:
	ILFMediaSource(OnMediaNewFrame OnNewFrame)
	{
		m_OnNewFrame = OnNewFrame;
	}
	virtual bool Open(const char* name) = 0;
	virtual void Close() = 0;

	virtual unsigned int NumFrames() = 0;
	virtual unsigned int CurrentFrame() = 0;
	virtual bool SetCurrentFrame(unsigned int frame) = 0;

	virtual bool First() = 0;
	virtual bool Prev()  = 0;
	virtual bool Next()  = 0;
	virtual bool Last()  = 0;

	virtual ILFImage* Frame() = 0;
};

typedef enum _LF_SHAPE_TYPE {
	LFShapePoint = 0,
	LFShapeVector,
	LFShapeCircle,
	LFShapeRect,
	LFShapeEllipse, 
	LFShapePolyline, 
	LFShapePolygon, 
	LFShapelandmarks,
	LFShapeContext
} LF_SHAPE_TYPE;

typedef enum _LF_RACURS_TYPE {
    LFRacursFront = 0,
    LFRacursLeftSemiProfile,
    LFRacursLeftProfile,
	LFRacursLeftBackSemiProfile,
	LFRacursBack,
	LFRacursRightBackSemiProfile,
	LFRacursRightProfile,
	LFRacursRightSemProfile,
	LFRacursBottomFront,
	LFRacursBottomLeftSemiPropfile,
	LFRacursBottomLeftProfile,
	LFRacursBottomBackLeftSemiProfile,
	LFRacursBottomBack,
	LFRacursBottomBackRightSemiProfile,
	LFRacursBottomRightProfile,
	LFRacursBottomRightSemiPropfile,
	LFRacursBottom,
	LFRacursUpFront,
	LFRacursUpLeftSemiProfile,
	LFRacursUpLeftProfile,
	LFRacursUpLeftBackSemiProfile,
	LFRacursUpBack,
	LFRacursUpBackRightSemiProfile,
	LFRacursUpRightProfile,
	LFRacursUpRightSemiProfile,
	LFRacursUp
} LF_RACURS_TYPE;

// 
class ILFWordAttribute
{
public:
	virtual const char* attrLabel() = 0;
	virtual const char* attrID()	= 0;
	virtual double confidence()		= 0;
};

class ILFWordAttributes
{
public: 
	virtual int Count() = 0;
	virtual ILFWordAttribute* Attribute(int index) = 0;
	virtual bool Delete(int index) = 0;
	virtual bool Add(ILFWordAttribute* attribute) = 0;
};

//Word 
class _ILFDetector;

class ILFWord
{
protected: 
	LF_SHAPE_TYPE		m_shape;
public:
	virtual const char* classLabel() = 0;
	virtual const char* classID() = 0;
	virtual int classColor() = 0;
	virtual ILFWordAttributes* Attributes() = 0;
	virtual _ILFDetector Detector() = 0;
	LF_SHAPE_TYPE Shape();
};

class ILFDictinary
{
public: 
	virtual const char* dictID() = 0;
	virtual int Count() = 0;
	virtual ILFWord* Word() = 0;
	virtual bool Delete(int index) = 0;
	virtual bool Add(ILFWord* word) = 0;
};

//item 
class ILFItem
{
protected:
	LF_RACURS_TYPE		m_racurs;
	double				m_raiting;
public:
	virtual ILFWord* Word() = 0;
	virtual int Count() = 0; 
	virtual LFPoint Point(int index) = 0;
};
// descriptor 
class ILFDescriptor
{
public:
	virtual int Count() = 0;
	virtual ILFItem*	Item(int index) = 0;
	virtual ILFImage*   Foreground() = 0;

	virtual bool LoadFromFile(const char* fileName) = 0;
	virtual bool SaveToFile(const char* fileName)   = 0;
};

// Zone
class ILFZone
{
public: 
	virtual int			Count() = 0;
	virtual LPLFPoint	Point(int index) = 0;
	virtual bool		Delete(int index) = 0;
	virtual bool		Add(LFPoint& point) = 0;
};
// Zones 
class ILFZones
{
public:
	virtual int Count() = 0;
	virtual ILFZone* Zone(int index) = 0;
	virtual bool Delete(int index)   = 0;
	virtual bool Add(ILFZone* zone)  = 0;
};
// feature
class _ILFFeature
{
public: 
	virtual void shift(LFPoint delta)		  = 0;
	virtual void scale(double factor)		  = 0;
	virtual void setup(double factor, LFPoint shift) = 0;
	virtual unsigned int uValue(ILFImage* image)			  = 0;
	virtual double dValue(ILFImage* image)	  = 0;
	virtual LFPoint corner()	  = 0;
	virtual int width()						  = 0;
	virtual int height()					  = 0;
};
// weak 
class _ILFWeak
{
public:
	virtual _ILFFeature* feature() = 0;
	virtual void setup(double factor, LFPoint shift) = 0;
	virtual double wight() = 0;
	virtual void setWeight(const double value) = 0;
	virtual bool classify(ILFImage* image, double& value) = 0;
};
// strong 
class _ILFStrong
{
public: 
	virtual int Count() = 0;
	virtual _ILFWeak* Weak(int index) = 0;
	virtual void AddWeak(_ILFWeak* weak) = 0;
	virtual double threshold() = 0;
	virtual double setThreshold(const double value) = 0;
	virtual void setup(double factor, LFPoint shift) = 0;
	virtual int classify(ILFImage* image, double& value) = 0;
};
// cascade 
class _ILFCascade
{
public: 
	virtual int Count()														= 0;
	virtual _ILFStrong* Strong(int index)									= 0;
	virtual int classify(ILFImage* image, LFRect& rect, double& value)		= 0;
	virtual void parallel(ILFImage* image, LFRect& rect, double* weights)   = 0;
	virtual bool AddStrong(_ILFStrong* strong)								= 0;
	virtual bool DeleteStrong(int index)									= 0;
};
class _ILFTrainer;

class _ILFDetector
{
public: 
	virtual int Count() = 0;
	virtual _ILFCascade* Cascade(int index) = 0;
	virtual bool Delete(int index)		    = 0;
	virtual bool Add(_ILFCascade* cascade)  = 0;
	virtual bool Detect(ILFImage* image)    = 0;
	virtual ILFDescriptor* Descriptor()     = 0;
	virtual _ILFTrainer* Trainer()			= 0;
};
 
//Model
class ILFModel
{
public:
	virtual ILFDictinary*	Dictinary() = 0;
	virtual ILFDescriptor*  Descriptor() = 0;
	virtual ILFParameters* Parameters() = 0;

	virtual bool Load(const char* fileName) = 0;
	virtual bool Forward(ILFImage* image) = 0; 
	virtual bool Update(const char* db) = 0;
};


// database 
// todo: add database imterface 

// trainer 
// todo: add trainer interface 

// Create Instance Functions 