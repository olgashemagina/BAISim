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
//		File: LF.h
//		Purpose: Contains abstracts and coomons for Locate Framework
//
//      CopyRight 2004-2018 (c) NN-Videolab.net
//M*/
#ifndef __LF_H__
#define __LF_H__

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
#include <string>
#include <vector>
#include <iostream>
#include <stdio.h>
#include "tinyxml.h"

typedef void (*TLFProgress)(const char* lpMessage, int progress);

static const char* c_lpDictFileName = "dictionary.xml";

/**
\Enum TEZoneTypes
\brief LF Zone types
*/
typedef enum {ZTUnknown, ZTRect, ZTContour, ZTLineSegment, ZTOpenPolygon, ZTCircle} TEZoneTypes;
class TLFZone;


using namespace std;


typedef struct
{
	bool	HasObject;
	int		Angle;
	int     ItemIndex;
	awpRect Rect;
    double  raiting;
}TLFBounds;

/*
Attribute classification result
*/
struct SLFAttrResult
{
	double  m_Raiting1;
	double  m_Raiting2;
	int    m_Result;     /*result of classificaton: -1 0 1*/
};



/*
    forward declaration
*/
//class  TLFObjectList;
class  TLFRect;
class  TLFParameter;
class  TLFTriangle;
class  TLFDetectedItem;
class  ILFDetectEngine;
class  TLFZones;
class  TLFDblVector;

#include "LFCore.h"

/** \defgroup LFInterfaces 
*	Interfaces of the Locate Framework.
*	Locate Framework defines 11 interfaces to expand its functionality.
*	These interfaces describe the basic abstractions that the 
*	library operates with: semantic analysis machine, detector, scanner, 
*	strong classifier, weak classifier, attributes, attribute detector, 
*	attribute classifier, predictor, cascade of strong classifiers 
*	and distance between objects. 
*   @{
*/
// distcance
class ILFDistance : public TLFObject
{
public:
	virtual double ro(TLFDblVector* v1, TLFDblVector* v2) = 0;
};

class ILFPredictor : public TLFObject
{
protected:
	TLFRect* m_pPredicted;
	virtual TLFDblVector* Features(ILFDetectEngine* engine, TLFRect* rect, int id) = 0;
	virtual bool Classify(TLFDblVector* d, double* err) = 0;
public:
	TLFRect* GetPredicted();
	virtual void Learn(ILFDetectEngine* engine, TLFRect* rect) = 0;
	virtual TLFRect* Predict(ILFDetectEngine* engine) = 0;
};


/*
	Feature abstraction
	Feature  = Receptor = Sensor
	todo: усложенение признака:
	1.  Возвращает число используемых бит < 32
	2.  возвращает массив действительных значений
	3.  Конструктор из XML элемента
*/
class ILFFeature : public TLFObject
{
protected:
	int m_sxBase;              // base point  within aperture
	int m_syBase;

	int m_wBase;              // size of feature unit  within aperture
	int m_hBase;

	int m_sx;                 // base point within image
	int m_sy;

	int m_w;                  // size of feature unit within image
	int m_h;

	double					m_fValue;
	unsigned int 			m_uValue;
public:
	// construction
	ILFFeature();
	ILFFeature(int sxbase, int sybase, int wbase, int hbase);
	ILFFeature(ILFFeature* pFeature);
	virtual ~ILFFeature();
	// calc features value
	virtual unsigned int      uCalcValue(TLFImage* pImage) = 0;
	virtual double            fCalcValue(TLFImage* pImage) = 0;

	unsigned int uValue();
	double       fValue();

	/**
		feature setup
	*/
	virtual void Setup(double scale_x, double scale_y,  AWPWORD dx, AWPWORD dy);
	virtual void Scale(double factor);
	virtual void Shift(int dx, int dy);
    /**
        Setting the feature inside the specified rectangle.
        The following values change:
        m_sx,m_sy,m_w,m_h
    */
    virtual void Setup(TLFRect& rect);
	/*
		XML io operations
	*/
	virtual bool LoadXML(TiXmlElement* parent);
	virtual bool SaveXML(TiXmlElement* parent);

	// feature rectangle
	virtual awpRect GetRect();
	// aperture position
	int sxBase();
	int syBase();
	int wUnitBase();
	int hUnitBase();
	// image position
	int sx();
	int sy();
	int wUnit();
	int hUnit();

	virtual const char* GetName()
	{
		return "ILFFeature";
	}
};
/*
	Weak - classificator  abstraction
	на данном этапе разработки используем два типа слабых классификаторов
	1й работает на одном действительном признаке
	2й работает на коде
*/
class ILFWeak : public TLFObject
{
protected:
	ILFFeature* m_pFeature; // pointer to rectangle feature
	double      m_fWeight;  // weight
public:
   ILFWeak(const char* lpFeatureName = "");
   ILFWeak(ILFFeature* pFeature);
   virtual ~ILFWeak();
   /*
	  Clssification image. Classificatoin result
	  0 - object present
	  1 - object absence
   */
   virtual int Classify(TLFImage* pImage, double* value = NULL) = 0;
   /*
	  positioning feature on image
   */
   void  Setup(double scale_x, double scale_y,  AWPWORD dx, AWPWORD dy);
   /*
		io operations
   */
   virtual void SaveXML(TiXmlElement* parent) = 0;
   virtual bool LoadXML(TiXmlElement* parent) = 0;
   ILFFeature* Fetaure();
   double Weight();
   void   SetWeight(double value);

   virtual const char* GetName()
   {
	return "ILFWeak";
   }
};
/**
	ILFStrongStrong - TLU logic classificator abstraction. Implements simply artificial neurons, 
	such as the McCulloch–Pitts model, based on the list of ILFWeak input elements.
*/
class ILFStrong : public TLFObjectList
{
protected:
	/**
		Fire threshold 
	*/
	double m_threshold;                  
public:
	/**
	\brief Construct the strong object with default parameres
	*/
   ILFStrong();
   /**
   \brief Destructs the strong object 
   */
   virtual ~ILFStrong();
   /**
   \brief Scales and shift neuron over image
   \param scale_x - x scale factor
   \param scale_y - y scale factor 
   \param dx - x shift in the pixels 
   \param dy - y shift in the pixels 
   */
   void Setup(double scale_x, double scale_y,  AWPWORD dx, AWPWORD dy);
   /*
		XML io operations
   */
   virtual void SaveXML(TiXmlElement* parent) = 0;
   virtual bool LoadXML(TiXmlElement* parent) = 0;
   /*
	  Classification
	  0 - object present
	  1 - object absence
   */
   virtual int Classify(TLFImage* pImage, double& err) = 0;
   /*
		weak list access
   */
   ILFWeak* GetWeak(int index);
   void     AddWeak(ILFWeak* pWeak);

   /*
		threshold access
   */
   double GetThreshold();
   void   SetThreshold(double Value);
   virtual const char* GetName()
   {
	return "ILFStrong";
   }
};

class TLFCascade : public TLFObjectList
{
protected: 
public: 
	TLFCascade();
	virtual ~TLFCascade();

	ILFStrong* GetStrong(int index);
	void       AddStrong(ILFStrong* strong);
	/**
	\brief Scales and shift cascade over image
	\param scale_x - x scale factor
	\param scale_y - y scale factor
	\param dx - x shift in the pixels
	\param dy - y shift in the pixels
	*/
	void Setup(double scale_x, double scale_y, AWPWORD dx, AWPWORD dy);
	virtual int Classify(TLFImage* pImage, double& err, double* vector = NULL);

	/*
	XML io operations
	*/
	virtual void SaveXML(TiXmlElement* parent);
	virtual bool LoadXML(TiXmlElement* parent);

	virtual const char* GetName()
	{
		return "TLFCascade";
	}
};

/*  Interface that defines the methods of systems
	decomposition the image into rectangular pieces
*/
class ILFScanner : public TLFObject
{
protected:
	TLFBounds*    m_Fragments;
	int		      m_FragmentsCount;
	TLFObjectList m_Params;
	unsigned int	   	   m_BaseWidth; /*Base object width. default 24*/
	unsigned int           m_BaseHeight;
	unsigned int		   m_minX;
	unsigned int		   m_maxX;
	unsigned int 		   m_minY;
	unsigned int		   m_maxY;
public:
	ILFScanner();
	virtual ~ILFScanner();
	/**
	\brief	Splits the entire TLFImage image into TLFBounds elements
	*/
	virtual bool ScanImage(TLFImage* pImage);
	/**
	\brief	Splits a section of an image into TLFBounds elements
	*/
	virtual bool ScanRect(awpRect& rect);
	/**
	\brief	Defines the abstract Scan (W, H) method to be overridden by children.
	*/
	virtual bool Scan(int width, int height) = 0;
	/**
	\brief Keeps only those TLFBounds elements that are completely inside rect
	*/
	bool Filter(awpRect& rect);

	virtual void Clear();
	int				GetFragmentsCount();
	awpRect			GetFragmentRect(int index);
	TLFBounds*		GetFragment(int index);
	int				GetParamsCount();
	TLFParameter* GetParameter(int index);

	int  GetBaseWidth();
	void SetBaseWidth(int BaseWidth);
	int  GetBaseHeight();
	void SetBaseHeight(int BaseHeight);

	unsigned int GetMinX();
	unsigned int GetMaxX();
	unsigned int GetMinY();
	unsigned int GetMaxY();

	bool LoadXML(TiXmlElement* parent);
	TiXmlElement* SaveXML();

	virtual const char* GetName()
	{
		return "ILFScanner";
	}
};

ILFScanner* CreateScanner(TiXmlElement* parent);

/*    Object detector. 
*/
class ILFObjectDetector : public TLFObject
{
protected:
	/**
	\brief Name of detector: for example FaceDetector, FireDetector etc.  
	*/
	std::string			m_strDetName;
	/**
	\brief Width of aperture
	*/
	int					m_baseWidth;
	/**
	\brief Height of aperture 
	*/
	int                 m_baseHeight;
	/**
	\brief Obsolete. Object type 
	*/
	std::string			m_Type;			
	/**
	\brief 	Rotation angle. Possible values -30...0...30 
	*/
	int					m_Angle;
	/**
	\brief 	detected racurs. Possible values:  
	*/
	int					m_racurs;
	/**
	\brief 	Image for analysis
	*/
	TLFImage			m_Image;
	/**
	\brief 	IScanner interface 
	*/
	ILFScanner*			m_scanner;
	/**
	\brief 	List of TLFDetectedItem 
	*/
	TLFObjectList		m_objects;
	/**
	\brief 	List of ILFStrong. This list can be empty. 
	*/
	TLFObjectList       m_Strongs;
public:
	/**
	\brief Construct the detector object with default parameres 
	*/
   ILFObjectDetector();

   //todo: add different construnction of the detector 

   /**
   \brief Destruct the detector object
   */
   virtual ~ILFObjectDetector();
   /**
   \brief Initialize detector with awpImage structure and calls scanner if nessesary
   \param pImage - pointer to awpImage structure 
   \param scan   - initializes scanner  
   \retrurn  true if detector has valid image and scanner creates all nessesary 
   \structures
   */
   virtual bool Init(awpImage* pImage, bool scan = true)					   		=0;
   /**
   \brief performs classification withing rectangle
   \param Fragmnet -  rect withing image. Image was initialized by ILFObjectDetector::Init
   \param err   - pointer to double array with GetStagesCount() + 1 size, 
   \contains responce of each stage  and their sum
   \param vect  pointer to int array with GetStagesCount() + 1 size, contains disision of 
   \each stage 
   \retrurn 1 - detection success, 0 - detection fail
   */
   virtual int  ClassifyRect(awpRect Fragmnet, double* err, int* vect)				= 0;
   /**
   \brief performs classification withing image with all rectangles generates by IScanner 
   \retrurn positive - num positive responce, 0 - nothing found, negative - function fail
   */
   virtual int  Detect()								   							=0;
   /**
   \brief returns number of stages (strong classificators) in the detector structure 
   \return count of stages
   */
   int 		GetStagesCount();
   /**
   \brief returns number of virtual sensors in the k-th Stage 
   \param Stage - index of stage to analyse
   \return count of virtual sensors (features)
   */
   int		GetSensorsCount(int Stage);
   /**
   \brief returns sum of weights of all weaks in the k-th stage 
   \param Stage - index of stage to analyse
   \return total wight of stage 
   */
   double  	GetStageWeight(int Stage);
   /**
   \brief returns threshold value for k-th stage
   \param Stage - index of stage to analyse
   \return threshold value 
   */
   double	GetStageThreshold(int Stage);
   /**
   \brief returns all stages as TLFObjectList 
   \return List of stages 
   */
   TLFObjectList* GetStrongs();
   /**
   \brief returns all stages as TLFObjectList
   \return List of stages
   */
   virtual  bool	AddStrong(ILFStrong* strong)		   = 0;
   /**
   \brief  returns threshold of the detetector 
   \return threshold value 
   */
   virtual  double  GetThreshold()						   = 0;
   /**
   \brief  setup threshold to the detetector
   */
   virtual  void SetThreshold(double Value)			       = 0;
   /**
   \brief  clears internal image, scanner data and results
   */
   virtual  void Clear();
   /**
   \brief access to aperture minimum horizontal size
   \retrun aperture width
   */
   int  GetBaseWidth();
   /**
   \brief deprecated
   \brief setup aperture minimum horizontal size
   */
   void SetBaseWidht(int w);
   /**
   \brief access to aperture minimum vertical  size
   \retrun aperture width
   */
   int  GetBaseHeight();
   /**
   \brief deprecated
   \brief setup aperture minimum vertical size
   */
   void SetBaseHeight(int h);
   /**
   \brief access to aperture rotation angle
   */
   int  GetAngle();
   /**
   \brief deprecated
   \brief setup aperture rotation angle
   */
   void SetAngle(int value);
   /**
   \brief access to the view of the object to which the detector was trained
   \retrun object view
   */
   int  GetRacurs();
   /**
   \brief deprecated
   \brief setup the view of the object to which the detector was trained
   */
   void SetRacurs(int value);
   /**
   \brief deprecated
   \brief access to object type
   \retrun aperture width
   */
   std::string GetObjectType();
   /**
   \brief access to the internal image for analysis
   \retrun pointer to TLFImage object
   */
   TLFImage*	 GetImage();
   /**
   \brief access to the internal scanner
   \retrun pointer to ILFScanner object
   */
   ILFScanner*   GetScanner();
   /**
   \brief setup the internal scanner
   \internal scanner is not created, just assigns this pointer
   \destructor of this detector calls destructor of this scanner
   */
   void SetScanner(ILFScanner* scanner);
   /**
   \brief number of items
   \retrun number of detected items,Different detectors can return a different
   \number of items. Only such items for which a successful classification
   \was performed, others are all items considered by the detector.
   \See TLFDetectedItem for more info
   \retrun number of items
   */
   int  GetNumItems();
   /**
   \brief Pointer to the specific item, defined by its index
   \ the index should be >=0 and < value, returned by GetNumItems()
   \retrun Pointer to the detected item.
   */
   TLFDetectedItem* GetItem(int index);

   //todo: add full functional log support

   /**
   \brief detector can save their images to the specific path
   */
   void SaveImages(std::string path);

	/**
	\brief save detector to XML structure
	*/
	virtual TiXmlElement* SaveXML() = 0;
	/**
	\brief Load detector from XML structure
	*/
	virtual bool          LoadXML(TiXmlElement* parent) = 0;
	/**
	\brief retrurns internal calss name
	*/
	virtual const char* GetName()
	{
		return "ILFObjectDetector";
	}
};
/** @} */ /*  end of LFInterfaces group */

/** \defgroup LFDescriptors
*	Object descriptors of the Locate Framework.
*   The library provides several varieties of object descriptors:
*	(obsolete)TLFRoiImageDescriptor
*   TLFDetectedItem - an object is what was found by a detector of any 
*	type generated by ILFObjectDetector
*	TLFSemanticImageDescriptor - list of objects of type TLFDetectedItem 
*	that describes the state of the scene or image in its entirety.
*   @{
*/

/** @brief Description of detect item. Can be any rectangle in the image.
*   Used to describe the result of the operation of all types of detectors.
*   See ILFObjectDetector.
*	Includes detector name and type, bounding box, tilt angle, view type, status,
*	rating, confidence, and unique identifier.
*
*/
class TLFDetectedItem: public TLFObject
{
protected:
	TLFString		m_strDetectorName; /*ID detector*/
	TLFString 		m_strComment;      /*additional description*/
	TLFRect*		m_pRect;		   /*bounding box*/
	double			m_Raiting;         /*рейтинг 0..1*/
	TLFString		m_type;            /*type of detection. set by the detector*/
	int				m_angle;           /*detection angle*/
	int				m_racurs;		   /**/
	bool			m_hasObject;       /**/
	ILFPredictor*   m_predictor;
	UUID			m_id;
	int				m_health;
	int				m_state;
	int				m_cluster_idx;
	int				m_color;
	TLFZone*        m_zone;

public:

	TLFDetectedItem();
	TLFDetectedItem(TLFDetectedItem& item);
	TLFDetectedItem(TLFDetectedItem* pItem);
	TLFDetectedItem(awpRect* pRect, double raiting, TLFString, int angle, int racurs, int bw, int bh, TLFString strDetector, UUID id, ILFPredictor* predictor = NULL);
	virtual ~TLFDetectedItem();
	/*data exchange*/
	TLFRect*	GetBounds();
	void		SetBounds(awpRect& rect, int iw = 640, int ih = 480);

	double		GetRaiting();
	void		SetRaiting(double value);

	std::string	GetType();
	void		SetType(const char* value);

	std::string GetComment();
	void		SetComment(const char* value);

	int         GetAngle();
	void		SetAngle(int value);

	int         GetRacurs();
	void		SetRacurs(int value);

	int         GetBW();
	int         GetBH();

	void		GetId(UUID& id);

	int  GetClusterIdx();
	void SetClusterIdx(int value);

	int GetColor();
	void SetColor(int color);

	const char*       GetDetectorName();
    void SetDetectorName(const char* lpName);

	bool HasObject();
	void SetHasObject(bool v);

	int GetState();
	void SetState(int state);

	void Resize(double factor);
	void Flip(int w);
	void Delete();
	void Rotate(awpPoint c, int angle);
	TLFRect* Predict(ILFDetectEngine* engine);
	void Update(ILFDetectEngine* engine, TLFRect* rect);
	int GetHealth();

	void SetZone(TLFZone* zone, int w = 640, int h = 480);
	TLFZone* GetZone();

	/*работа с  xml файлом*/
	TiXmlElement* SaveXML();
	bool LoadXML(TiXmlElement* parent);

	int				m_bh;			   /*базовая высота объекта*/
	int				m_bw;			   /*базовая ширина объекта*/


	virtual const char* GetName()
	{
		return "TLFDetectedItem";
	}
};
/** @brief The semantic description of the image or frame in the video file.
*   It is a list of elements of the form TLFDatectedItem. Represents the result of the 
*   semantic analysis engine ILFDetectEngine. Provides the ability to update and predict 
*   its status. It is a persistent basic element of the database and the table of contents 
*   of the video file.
*/
class TLFSemanticImageDescriptor : public TLFObjectList
{
protected:
	int			     m_imageWidth;
	int				 m_imageHeight;
	virtual bool     _LoadXml(TiXmlElement* parent);
	virtual bool	 _SaveXml(TiXmlElement* parent);
public:
	TLFSemanticImageDescriptor();
	TLFSemanticImageDescriptor(awpImage* pImage);
	TLFSemanticImageDescriptor(int w, int h);
	virtual ~TLFSemanticImageDescriptor();
	/*Working with image*/
	void SetImage(awpImage* pImage);
	/*работа с xml файлом*/
	virtual bool SaveXML(const char* lpFileName);
	virtual bool LoadXML(const char* lpFileName);
	virtual TiXmlElement* SaveXML();
	virtual bool LoadXML(TiXmlElement* e);
#ifdef LOAD_FROM_STREAM
	virtual bool LoadStream(std::istream& in);
#endif
	/*work with a list of found objects*/
	/*editing*/
	bool AddDetectedItem(TLFDetectedItem* item);
	bool DeleteDetectedItem(int Index);
	void Resize(double factor);
	/*properties*/
	int GetItemsCount();
	TLFDetectedItem* GetDetectedItem(int index);
	int GetItemSize(int index);
	int GetItemPos (int index);
	int Width();
	int Height();
	/*comparison of semantic descriptions of images*/
	double Compare(const char* lpFileName, double overlap);
	double Compare(TLFSemanticImageDescriptor* Descriptor, double overlap);
	double Overlap(TLFRect& rect);

	void Predict(ILFDetectEngine* engine);
	void Update(ILFDetectEngine* engine, TLFSemanticImageDescriptor* sid);

	virtual const char* GetName()
	{
		return "TLFSemanticImageDescriptor";
	}
};
/** @} */ /*  end of LFDescriptors group */

/** \addtogroup LFInterfaces
*   @{

/**
	Semantic Image Analysis Engine
*/
class ILFDetectEngine : public TLFObject
{
protected:
	std::string			m_strName;
	std::string			m_outDatabase;
	std::string 		m_strPredictorName;
	bool				m_saveToDb;
	int					m_minNumObjects;

	int					m_State;

	bool                m_needCluster;
	bool                m_needResize;
	bool				m_needTrack;

	int                 m_baseWidth;
	double              m_resizeCoef;
	TLFObjectList		m_detectors;
	TLFImage			m_SourceImage;
	TLFImage			m_mask;
	TLFZones*			m_zones;

	TLFSemanticImageDescriptor m_result;
	TLFSemanticImageDescriptor m_tmp_result;

	// virtual functions
	virtual void InitDetectors()    = 0;
	virtual bool FindObjects()		= 0;
	virtual void OverlapsFilter(TLFSemanticImageDescriptor* descriptor)	= 0;
public:
	ILFDetectEngine();
	virtual ~ILFDetectEngine();
	
	virtual void Clear() = 0;
	
	/*Loading from-to xml files*/
	virtual bool Load(const char* lpFileName);
#ifdef LOAD_FROM_STREAM
	virtual bool LoadStream(std::istream& in);
#endif
	virtual bool LoadXML(TiXmlElement* parent)= 0;
	virtual bool Save(const char* lpFileName);
	virtual TiXmlElement* SaveXML()  = 0;

	/*perform a search operation objects in the image*/
	bool SetSourceImage(TLFImage* pImage, bool detect = false);
	bool SetSourceImage(awpImage* pImage, bool detect = false);

	virtual void SetZones(TLFZones* zones);
	TLFZones* GetZones();
	//
	int  GetItemsCount();
	TLFDetectedItem* GetItem(int index);

	/*scanner*/
	ILFScanner* GetScanner(int index = 0);
	/*detector*/
	void AddDetector(ILFObjectDetector* pDetector);
	ILFObjectDetector* GetDetector(int index = 0);


	bool GetNeedCluster();
	void SetNeedCluster(bool Value);

	bool GetResize();
	void SetResize(bool Value);
	double GetResizeCoef();

	bool GetNeedTrack();
	void SetNeedTrack(bool Value);

	int GetBaseImageWidth();
	void SetBaseImageWidth(int width);

	std::string GetOutDatabase();
	void SetOutDatabase(std::string& strPath);

	bool GetSaveToDb();
	void SetSaveToDb(bool value);

	int  GetMinNumObjects();
	void SetMinNumObjects(int value);
	const char* GetPredictorName();

	int GetState();

	TLFSemanticImageDescriptor* GetSemantic();

	awpImage* GetMaskImage();
	void SetMaskImage(awpImage* img);

	awpImage* GetSourceImage();
	int GetDetectorsCount();
	virtual const char* GetName()
	{
		return "ILFDetectEngine";
	}
};
/* forward declaration*/
class ILFAttrClassifier;
/* attribute detector interface
*/
class ILFAttrDetector : public TLFObject
{
protected:

	char*  m_Name;

	char* m_className1;
	char* m_className2;
	int   m_baseSize;
	double m_objectScale;
	bool  m_needResize;
	bool  m_fastComputing;
	/* expert list
	*/
	TLFObjectList m_Experts;
	virtual bool DoClassify(TLFImage* pImage, TLFRect* pRoi, SLFAttrResult& result) = 0;
public:
	ILFAttrDetector();
	virtual ~ILFAttrDetector();
	/* xml io support
	*/
	bool Create(const char* pClassName1, const char* pClassName2,
		int baseSize, double objectScale, bool needResize, const char* pXmlFileName);
	bool Load(const char* pXmlFileName);
	bool Save(const char* pXmlFileName);
	/* classify image
	*/
	virtual bool Classify(TLFImage* pImage, TLFRect* pRoi, SLFAttrResult& result);
	/* detector structure
	*/
	int GetNumExperts();
	ILFAttrClassifier* GetExpert(int nIndex);
	/* edit structure
	*/
	int AddExpert(ILFAttrClassifier* pExpert);
	bool RemoveExpert(int nIndex);
	/* properties
	*/
	char* GetClassName1();
	void SetClassName1(const char* lpClassName1);
	char* GetClassName2();
	void SetClassName2(const char* lpClassName2);
	/* read only properties
	*/
	int  GetBaseSize();
	double GetObjectSize();
	bool NeedScaleImage();
	/* computing control
	*/
	bool NeedFastComputing();
	void SetNeedFastComputing(bool Value);

	virtual const char* GetName();
};
/*
	Interface for attribute calssification expert
	ILFAttrCalssificator
*/
class ILFAttrClassifier : public TLFObject
{
protected:
	double m_threshold1;
	double m_threshold2;
	SLFAttrResult m_LastResult;
	virtual bool DoClassify(TLFImage* pImage, TLFRect* pRoi, SLFAttrResult& result) = 0;
public:
  ILFAttrClassifier();
  virtual ~ILFAttrClassifier();
  virtual ILFAttrClassifier* LoadFromNode(TiXmlElement* parent) = 0;
  virtual bool SaveToNode(TiXmlElement* parent) = 0;

  /*classify pRoi on the pImage*/
  virtual bool Classify(TLFImage* pImage, TLFRect* pRoi, SLFAttrResult& result);

  /*parameters exchange*/
  double GetThreshold1();
  virtual void  SetThreshold1(double value);
  double GetThreshold2();
  virtual void  SetThreshold2(double value);
  virtual const char* GetName () {return "ILFAttrClassifier";}
  SLFAttrResult GetlastResult();
};
/** @} */ /*  end of LFInterfaces group */
#endif
 
