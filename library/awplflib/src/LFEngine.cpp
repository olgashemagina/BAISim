#include "LFEngine.h"
#include "LFZone.h"
#include "LFDetector.h"
#include "LFFileUtils.h"

#define BUFFER_SIZE     1000
#define DETECTORS_COUNT	5
#define ANGLE 15
//using namespace std;

//------------------------------------------ ILFDetectEngine -------------------------
ILFDetectEngine::ILFDetectEngine()
{
	m_needCluster = true;
	m_needResize  = false;
	m_needTrack = false;
	m_baseWidth   = 320;
	m_resizeCoef  = 1;
	m_minNumObjects = 1;
	m_outDatabase = "";
	m_strPredictorName = "";
	m_saveToDb = false;
	m_State = 0;
	m_zones = NULL;
}
ILFDetectEngine::~ILFDetectEngine()
{
	if (m_zones != NULL)
		delete m_zones;
}

/*perform a search operation objects in the image*/
bool ILFDetectEngine::SetSourceImage(TLFImage* pImage, bool detect)
{
	m_result.Clear();

	if (pImage == NULL)
		return false;
	if (pImage->GetImage() == NULL)
		return false;
	if (pImage->GetImage()->dwType != AWP_BYTE)
		return false;

	return SetSourceImage(pImage->GetImage(), detect);
}

void ILFDetectEngine::SetZones(TLFZones* zones)
{
    this->Clear();
    if (this->m_zones != NULL)
		delete m_zones;
	if (zones != NULL)
		m_zones = new TLFZones(*zones);
	else
		m_zones = NULL;

}

bool ILFDetectEngine::SetSourceImage(awpImage* pImage, bool detect)
{
	this->m_result.SetImage(pImage);
	m_tmp_result.SetImage(pImage);
	m_SourceImage.SetImage(pImage);
	awpImage* img = m_SourceImage.GetImage();

	if (this->m_needResize)
	{

		if (img != NULL)
		{
			m_resizeCoef = (double)img->sSizeX / (double)m_baseWidth;
			int height = (int)floor((double)img->sSizeY/m_resizeCoef  +0.5);

			AWPRESULT resize_res = awpResizeBilinear(img, this->m_baseWidth, height);
			if (resize_res != AWP_OK)
				return false;
		}
	}
	// apply zones and mask
	if (m_zones != NULL)
	{
		if (m_mask.GetImage() == NULL)
		{
#ifdef _DEBUG
			printf("Mask image == NULL \n");
#endif 
            awpImage* mask =  NULL;
// todo: scale scanner is not work here only tilt 
//            ILFObjectDetector* d = (ILFObjectDetector*)this->m_detectors.Get(0);
//            if (d != NULL && d->GetScanner() != NULL)
//				mask = m_zones->GetMaskImage(m_SourceImage.GetImage(), d->GetScanner());
//            else
				mask = m_zones->GetMaskImage(m_SourceImage.GetImage());

			if (mask != NULL)
			{
				m_mask.SetImage(mask);
				_AWP_SAFE_RELEASE_(mask)
			}
		}
		if (m_mask.GetImage() == NULL)
		{
			return false;
		}
	}

    if (m_mask.GetImage() != NULL)
    {
		if (m_mask.GetImage()->sSizeX != img->sSizeX || m_mask.GetImage()->sSizeY != img->sSizeY)
        {
        	awpResize(m_mask.GetImage(), img->sSizeX, img->sSizeY);
        }

        // apply mask
        AWPWORD w, h;
        w = m_SourceImage.GetImage()->sSizeX;
        h = m_SourceImage.GetImage()->sSizeY;
        AWPBYTE* a = (AWPBYTE*)(m_SourceImage.GetImage()->pPixels);
        AWPBYTE* b = (AWPBYTE*)(m_mask.GetImage()->pPixels);
        for (int i = 0; i < w*h; i++)
            a[i] = b[i] * a[i];

#ifdef _DEBUG
		awpSaveImage("source.awp", m_SourceImage.GetImage());
		awpSaveImage("mask_image.awp", m_mask.GetImage());
#endif	
	}


	this->InitDetectors();

	// setup roi
	ILFScanner* s = GetScanner(0);
	if (detect)
	{
		bool res =  FindObjects();
		// save log
		if (this->m_saveToDb && m_result.GetItemsCount() >= m_minNumObjects)
		{
			for (int k = 0; k < m_detectors.GetCount(); k++)
			{
				ILFObjectDetector* d = (ILFObjectDetector*)m_detectors.Get(k);
				d->SaveImages(this->m_outDatabase);
			}
		}
		return res;
	}
	return true;
}
int  ILFDetectEngine::GetItemsCount()
{
	return m_result.GetItemsCount();
}

int ILFDetectEngine::GetDetectorsCount()
{
	return this->m_detectors.GetCount();
}

TLFDetectedItem* ILFDetectEngine::GetItem(int index)
{
	if (index < 0 || index >= m_result.GetItemsCount())
		return NULL;
	return m_result.GetDetectedItem(index);
}
/*scanner*/
ILFScanner* ILFDetectEngine::GetScanner(int index)
{
	if (index < 0 || index >= this->m_detectors.GetCount())
		return NULL;
	ILFObjectDetector* detector = dynamic_cast<ILFObjectDetector*>(m_detectors.Get(index));
	if (detector)
		return detector->GetScanner();
	return NULL;
}
/*detector*/
void ILFDetectEngine::AddDetector(ILFObjectDetector* pDetector)
{
	m_detectors.Add(pDetector);
}
ILFObjectDetector* ILFDetectEngine::GetDetector(int index)
{
	if (index < 0 || index >= this->m_detectors.GetCount())
		return NULL;
	ILFObjectDetector* detector = dynamic_cast<ILFObjectDetector*>(m_detectors.Get(index));
	if (detector)
		return detector;
	return NULL;
}
void ILFDetectEngine::RemoveDetector(int index) {
	m_detectors.Put(index, NULL);
	m_detectors.Delete(index);
}

bool ILFDetectEngine::Save(const char* lpFileName)
{
	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );
    doc.LinkEndChild( decl );
	TiXmlElement* engine = SaveXML();
	if (engine == NULL)
		return false;
	doc.LinkEndChild(engine);
	//FILE* file = _wfopen(LFUtf8ConvertToUnicode(lpFileName).c_str(), L"w");
    FILE* file = std::fopen(lpFileName, "w");
	if (!file)
	{
		printf("Save engine failed!!!\n");
		return false;
	}
	bool result = doc.SaveFile(file);
	fclose(file);
	return result;
}

bool ILFDetectEngine::Load(const char* lpFileName)
{
	//FILE* file = _wfopen(LFUtf8ConvertToUnicode(lpFileName).c_str(), L"rb");
    FILE* file = std::fopen(lpFileName, "rb");
	if (!file)
	{
		printf("Load engine failed!!!\n");
		return false;
	}
    TiXmlDocument doc;
	bool result = doc.LoadFile(file, TIXML_ENCODING_UTF8);
	fclose(file);
    if (!result)
    {	
		printf("ILFDetectEngine::Load failed!!!\n");		
		return false;
    }
	this->m_strName = lpFileName;
	TiXmlHandle hDoc(&doc);
    TiXmlElement* pElem = NULL;
    TiXmlHandle hRoot(0);
	pElem = hDoc.FirstChildElement().Element();

	return this->LoadXML(pElem);
}

 
bool ILFDetectEngine::LoadStream(std::istream& in)
{
	TiXmlDocument doc;
  	in >> doc;


	TiXmlHandle hDoc(&doc);
	TiXmlElement* pElem = NULL;
	TiXmlHandle hRoot(0);
	pElem = hDoc.FirstChildElement().Element();

	return this->LoadXML(pElem);
}


 bool ILFDetectEngine::GetNeedCluster()
{
    return this->m_needCluster;
}
void ILFDetectEngine::SetNeedCluster(bool Value)
{
    this->m_needCluster = Value;
}
bool ILFDetectEngine::GetNeedTrack()
{
	return this->m_needTrack;
}
void ILFDetectEngine::SetNeedTrack(bool Value)
{
	this->m_needTrack = Value;
}
bool ILFDetectEngine::GetResize()
{
	return this->m_needResize;
}
void ILFDetectEngine::SetResize(bool Value)
{
	this->m_needResize = Value;
}
double ILFDetectEngine::GetResizeCoef()
{
	return this->m_resizeCoef;
}
int ILFDetectEngine::GetBaseImageWidth()
{
    return this->m_baseWidth;
}
void ILFDetectEngine::SetBaseImageWidth(int width)
{
    this->m_baseWidth = width;
}
std::string ILFDetectEngine::GetOutDatabase()
{
	return this->m_outDatabase;
}
void ILFDetectEngine::SetOutDatabase(std::string& strPath)
{
	this->m_outDatabase = strPath;
}
bool ILFDetectEngine::GetSaveToDb()
{
	return this->m_saveToDb;
}
void ILFDetectEngine::SetSaveToDb(bool value)
{
	this->m_saveToDb = value;
}
int  ILFDetectEngine::GetMinNumObjects()
{
	return this->m_minNumObjects;
}
void ILFDetectEngine::SetMinNumObjects(int value)
{
	if (value >= 0)
		this->m_minNumObjects = value;
}


TLFSemanticImageDescriptor* ILFDetectEngine::GetSemantic()
{
	return &m_result;
}
const char* ILFDetectEngine::GetPredictorName()
{
    return this->m_strPredictorName.c_str();
}

int ILFDetectEngine::GetState()
{
	return m_State;
}

TLFZones* ILFDetectEngine::GetZones()
{
	return m_zones;
}
awpImage* ILFDetectEngine::GetMaskImage()
{
	return m_mask.GetImage();
}

void ILFDetectEngine::SetMaskImage(awpImage* img)
{
	m_mask.SetImage(img);
}

awpImage* ILFDetectEngine::GetSourceImage()
{
	return m_SourceImage.GetImage();
}



//-------------------------------TLFDetectEngine--------------------------------
TLFDetectEngine::TLFDetectEngine() :ILFDetectEngine()
{
}
TLFDetectEngine::~TLFDetectEngine()
{}

void TLFDetectEngine::Clear()
{
	this->m_tmpList.Clear();
	this->m_result.Clear();
	this->m_tmp_result.Clear();
	for (int i = 0; i < this->m_detectors.GetCount(); i++)
	{
		ILFObjectDetector* d = dynamic_cast<ILFObjectDetector*>(m_detectors.Get(i));
		if (d)
		{
			d->Clear();
		}
	}
    delete this->m_zones;
    this->m_zones = NULL;
    this->m_mask.FreeImages();
}

///////////////
bool TLFDetectEngine::LoadXML(TiXmlElement* parent)
{
    //printf("TLFDetectEngine: LoadXML.\n");

    try
    {
      if (parent == NULL)
          return false;
      const char* str =  parent->Attribute("type");
      if (strcmp(str, this->GetName()) != 0)
		  return false;
	  this->m_strPredictorName = parent->Attribute("predictor");

	  this->m_detectors.Clear();

	  ILFObjectDetector* dt = NULL;
	  TiXmlNode* elem = parent->FirstChild("ILFObjectDetector");
	  if (elem == NULL)
	  {
			  dt = new TSCObjectDetector();
			  this->m_detectors.Add(dt);
	  }
	  else
	  {
		  do
		  {
			  dt = this->LoadDetector(elem->ToElement());
			  if (dt != NULL)
				  this->m_detectors.Add(dt);

			  elem = elem->NextSibling();
		  } while (elem != NULL);
	  }
	 }
     catch(...)
     {
        printf("TLFDetectEngine: exeption while loading.");
	return false;
     }
	return true;
}
// 
ILFObjectDetector* TLFDetectEngine::LoadDetector(TiXmlElement* parent)
{
   	if (parent == NULL)
    	return NULL;

	const char* type = parent->Attribute("type");

	if (strcmp(type, "TSCObjectDetector") == 0)
	{
        TiXmlElement* elem = parent->FirstChild("TSCObjectDetector")->ToElement();
        if (elem == NULL)
            return NULL;

		TSCObjectDetector* detector = new TSCObjectDetector();
		//TiXmlElement* e = elem->FirstChildElement();
		if (!detector->LoadXML(elem))
		{
			delete detector;
			return NULL;
		}
		return detector;	
	}
	return NULL;
}
// save engine to xml element
TiXmlElement* TLFDetectEngine::SaveXML()
{
    TiXmlElement* parent = new TiXmlElement(GetName());
	ILFObjectDetector* detector = GetDetector();
	
	if (detector == NULL)
		detector = new TSCObjectDetector();
	parent->SetAttribute("type", GetName());
	parent->SetAttribute("predictor", "TLFAverageNNPredictor");

    TiXmlElement* de = new TiXmlElement("ILFObjectDetector");
	de->SetAttribute("type", detector->GetName());
	TiXmlElement* e = detector->SaveXML();
    de->LinkEndChild(e);
	parent->LinkEndChild(de);
	return parent;        
}

void TLFDetectEngine::InitDetectors()
{
	for (int i = 0; i < this->m_detectors.GetCount(); i++)
	{
		ILFObjectDetector* d = (ILFObjectDetector*)m_detectors.Get(i);
		d->Init(&m_SourceImage);
	}
}

//searches the objects in the image.
bool TLFDetectEngine::FindObjects()
{
	if (this->m_SourceImage.GetImage() == NULL)
		return false;
	if (this->m_SourceImage.GetImage()->dwType != AWP_BYTE)
		return false;

	m_tmpList.Clear();
	if (this->m_needTrack)
		this->m_result.Predict(this);
	else
		m_result.Clear();
	// detecting
	int cc = 0;
	
//#ifdef _OMP_
	//omp_set_num_threads(m_detectors.GetCount());
	//#pragma omp parallel for  reduction(+: cc) num_threads(m_detectors.GetCount())
//#endif
	for (int k = 0; k < m_detectors.GetCount(); k++)
	{
		++cc;
		ILFObjectDetector* d = (ILFObjectDetector*)m_detectors.Get(k);
		d->Detect();
	}
	// receiving information from the detectors
	for (int k = 0; k < m_detectors.GetCount(); k++)
	{
		ILFObjectDetector* d = (ILFObjectDetector*)m_detectors.Get(k);
		if (d->GetNumItems() > 0)
		{
			for (int i = 0; i < d->GetNumItems(); i++)
			{
				TLFDetectedItem* item = d->GetItem(i);
				TLFDetectedItem* di = new TLFDetectedItem(item);
				di->Resize(m_resizeCoef);
				m_tmpList.Add(di);
			}
		}
	}
	if (this->m_needTrack)
	{
		m_tmp_result.Clear();
		this->OverlapsFilter(&m_tmp_result);
		this->m_result.Update(this, &m_tmp_result);
	}
	else
	{
		if (!this->m_needCluster)
		{
			m_result.Clear();
			for (int i = 0; i < m_tmpList.GetCount();i++)
			{
				TLFDetectedItem* di = (TLFDetectedItem*)m_tmpList.Get(i);
				TLFDetectedItem* dd = new  TLFDetectedItem(di);
				m_result.AddDetectedItem(dd);
			}
			m_tmpList.Clear();
		}
		else
			OverlapsFilter(&this->m_result);
	}
	return true;
}
//
void TLFDetectEngine::OverlapsFilter(TLFSemanticImageDescriptor* descriptor)
{
	if (descriptor == NULL)
		return;

	TLFObjectList objectDescr;
	TLFObjectList objects;

    while (m_tmpList.GetCount() > 0 )
    {
        objectDescr.Clear();
        TLFDetectedItem* de = new TLFDetectedItem(( TLFDetectedItem* )m_tmpList.First());
        m_tmpList.Delete(0);
        objectDescr.Add(de);
        // overlap analysis
        for (int i = m_tmpList.GetCount() - 1; i >= 0; i--)
		{
            TLFDetectedItem* di = (TLFDetectedItem*)m_tmpList.Get(i);
            if (di->GetBounds().IsCover(de->GetBounds()) || de->GetBounds().IsCover(di->GetBounds()) ||  de->GetBounds().RectOverlap(di->GetBounds()) > 0.1)// <- this is overlap threshold
            {
               //
                TLFDetectedItem* dd = new  TLFDetectedItem(di);
                objectDescr.Add(dd);
                m_tmpList.Delete(i);
            }
        }
        // objectDescr analysis
        awpPoint p; p.X = 0; p.Y = 0;
        int xsum = 0, ysum = 0, wsum = 0, hsum = 0;
        AWPSHORT w = 0;
        AWPSHORT h = 0;
        double max_raiting = 0;

        TLFDetectedItem* firstObject = (TLFDetectedItem*)objectDescr.Get(0);
        max_raiting = firstObject->GetRaiting();
        for (int i = 0; i < objectDescr.GetCount(); i++)
        {
             TLFDetectedItem* d =  (TLFDetectedItem*)objectDescr.Get(i);
			 xsum += d->GetBounds().Center().X;
             ysum += d->GetBounds().Center().Y;
             wsum += d->GetBounds().Width();
             hsum +=  d->GetBounds().Height();
             if (max_raiting < d->GetRaiting())
                max_raiting =  d->GetRaiting();
        }
		p.X = xsum/objectDescr.GetCount();
        p.Y = ysum/objectDescr.GetCount();
        w = wsum/objectDescr.GetCount();
        h = hsum/objectDescr.GetCount();
        // Object attributes
        awpRect r;
        double raiting;
        int   bw;
        int   bh;
        int   angle;
		std::string   type;
        r.left = p.X - w/2;
        r.right = p.X + w/2;
        r.top = p.Y - h/2;
		r.bottom = p.Y + h / 2;
        angle = firstObject->GetAngle();
        type  = firstObject->GetType();
        raiting = max_raiting;
        bw = firstObject->GetBW();
        bh = firstObject->GetBH();
        UUID id;
       // memset(id, 0, sizeof(UUID));
		LF_NULL_UUID_CREATE(id);
        // add object to result list
        TLFDetectedItem* dd = new  TLFDetectedItem(r, raiting, type, firstObject->GetAngle(), firstObject->GetRacurs(), bw, bh, firstObject->GetDetectorName(), id);
		descriptor->AddDetectedItem(dd);
		delete dd;
    }
}


//-------------------------------TLFFGEngine-----------------------------------
TLFFGEngine::TLFFGEngine() : ILFDetectEngine()
{
	TLFFGBGDetector* d = new TLFFGBGDetector();
	this->m_detectors.Add(d);
	m_strPredictorName = "TLFMomentsPredictor";

	m_minSize = 1;
	m_maxSize = 5;
	m_minAR	  = 1;
	m_maxAR   = 2.5;
    m_relax = false;
    m_relax_count = 0;
}
void TLFFGEngine::Clear()
{
	this->m_tmpList.Clear();
	this->m_result.Clear();
	this->m_tmp_result.Clear();
	for (int i = 0; i < this->m_detectors.GetCount(); i++)
	{
		ILFObjectDetector* d = dynamic_cast<ILFObjectDetector*>(m_detectors.Get(i));
		if (d)
		{
			d->Clear();
		}
	}
}

void    TLFFGEngine::BuildForeground()
{
	ILFObjectDetector* d = (ILFObjectDetector*)m_detectors.Get(0);
	if (!d)
		return;

	awpImage* fg = NULL;
	TLFImage* img = d->GetImage();

	awpCreateImage(&fg, img->GetImage()->sSizeX, img->GetImage()->sSizeY, 1, AWP_BYTE);
	m_foreground.SetImage(fg);
	awpReleaseImage(&fg);
	int count = 0;

	d->Detect();
    m_tmpList.Clear();
	if (d->GetNumItems() > 0)
	{
		for (int i = 0; i < d->GetNumItems(); i++)
		{
			TLFDetectedItem* item = d->GetItem(i);
			if (item != NULL && item->HasObject())
			{
				awpRect    item_rect = item->GetBounds().GetRect();
				awpImage*  fg = m_foreground.GetImage();
				awpFillRect(fg, &item_rect, 0, 255);

				TLFDetectedItem* di = new TLFDetectedItem(item);
				di->Resize(m_resizeCoef);
				m_tmpList.Add(di);
				count++;
			}
		}
	    if ((double)count / (double)d->GetNumItems() > 0.5)
        {
            m_relax = true;
	    	this->Clear();
        }
	}
}


bool TLFFGEngine::LoadXML(TiXmlElement* parent)
{
	return false;
}
bool TLFFGEngine::FindObjects()
{

	if (this->m_SourceImage.GetImage() == NULL)
		return false;
	if (this->m_SourceImage.GetImage()->dwType != AWP_BYTE)
		return false;


	BuildForeground();

    if (m_relax)
    {
		m_relax_count++;
        if (m_relax_count > 100)
        {
        	m_relax_count = 0;
            m_relax = false;
        }
        return true;
    }

	if (this->m_needTrack)
	{
		m_processor.SetSourceImage(&m_foreground);
		m_result.Predict(this);
		m_tmp_result.Clear();
		this->OverlapsFilter(&m_tmp_result);
		this->m_result.Update(this, &m_tmp_result);
	}
	else
	{
		m_result.Clear();
		if (!this->m_needCluster)
		{
			for (int i = 0; i < m_tmpList.GetCount(); i++)
			{
				TLFDetectedItem* di = (TLFDetectedItem*)m_tmpList.Get(i);
				m_result.AddDetectedItem(di);
			}
			m_tmpList.Clear();
		}
		else
			OverlapsFilter(&this->m_result);
	}
	return true;
}
TiXmlElement*  TLFFGEngine::SaveXML()
{
	return NULL;
}
void TLFFGEngine::OverlapsFilter(TLFSemanticImageDescriptor* descriptor)
{
	awpImage* img = m_foreground.GetImage();
#ifdef _DEBUG
	awpSaveImage("foreground.awp", img);
#endif
	awpStrokeObj* obj = NULL;
	int num = 0;
	awpGetStrokes(img, &num, &obj, 128, NULL);
	if (num == 0)
		return;
	for (int i  =0; i < num; i++)
	{
		awpRect rect;
		awpCalcObjRect(&obj[i], &rect);
		TLFRect  r(rect);
		r.Scale(m_resizeCoef);
		rect = r.GetRect();
		bool found = false;

		if (descriptor != &this->m_result)
		{
			for (int i = 0; i < this->GetItemsCount(); i++)
			{
				TLFDetectedItem* di = this->GetItem(i);
				
				if (di->GetBounds().RectOverlap(r)>0 || di->GetBounds().Distance(r) < 5)
				{
   					found = true;
					break;
				}
			}
		}

		if (found)
			continue;

	  	if (SquareFilter(rect) && ARFilter(rect))
		{
			UUID id;
			LF_NULL_UUID_CREATE(id);
			TLFDetectedItem* dd = new  TLFDetectedItem(rect, 0, "Foreground", 0, 0, 24, 24, "TLFFGBGDetector", id);
			descriptor->AddDetectedItem(dd);
			delete dd;
		}
	}
	awpFreeStrokes(num, &obj);
}

double TLFFGEngine::GetMinSize()
{
	return m_minSize;
}
double TLFFGEngine::GetMaxSize()
{
	return m_maxSize;
}
void TLFFGEngine::SetMinSize(double value)
{
	if (value < 0 || value > 100)
		return;
	m_minSize = value;
}
void TLFFGEngine::SetMaxSize(double value)
{
	if (value < 0 || value > 100)
		return;
	m_maxSize = value;
}

double TLFFGEngine::GetMinAR()
{
	return m_minAR;
}
double TLFFGEngine::GetMaxAR()
{
	return m_maxAR;
}

void TLFFGEngine::SetMinAR(double value)
{
	if (value < 0.1 || value > 10)
		return;
	m_minAR = value;
}
void TLFFGEngine::SetMaxAR(double value)
{
	if (value < 0.1 || value > 10)
		return;
	m_maxAR = value;
}

int TLFFGEngine::GetAverageBufferSize()
{
	TLFFGBGDetector* d = (TLFFGBGDetector*)GetDetector();
	if (d != NULL)
	{
		return d->GetAverageBufferSize();
	}
	else
		return 10;
}
void TLFFGEngine::SetAverageBufferSize(int value)
{
	TLFFGBGDetector* d = (TLFFGBGDetector*)GetDetector();
	if (d != NULL)
	{
		d->SetAverageBufferSize(value);
	}
}

int TLFFGEngine::GetBgStability()
{
	TLFFGBGDetector* d = (TLFFGBGDetector*)GetDetector();
	if (d != NULL)
	{
		return d->GetBgStability();
	}
	else
		return 0;
}
void TLFFGEngine::SetBgStability(int value)
{
	TLFFGBGDetector* d = (TLFFGBGDetector*)GetDetector();
	if (d != NULL)
	{
		d->SetBgStability(value);
	}
}

int TLFFGEngine::GetDelay()
{
	TLFFGBGDetector* d = (TLFFGBGDetector*)GetDetector();
	if (d != NULL)
	{
		return d->GetDelay();
	}
	else
		return 0;
 }

void TLFFGEngine::SetDelay(int value)
{
	TLFFGBGDetector* d = (TLFFGBGDetector*)GetDetector();
	if (d != NULL)
	{
		d->SetDelay(value);
	}
}

bool 	TLFFGEngine::SquareFilter(awpRect& rect)
{
	TLFRect LFRect(rect);
	LFRect.Scale(1 / this->m_resizeCoef);
	double square = LFRect.Square();
	double square0 = this->m_SourceImage.GetImage()->sSizeX*this->m_SourceImage.GetImage()->sSizeY;
	double ratio = 100.*square / square0;
	if (ratio >= this->m_minSize && ratio <= this->m_maxSize)
		return true;
	else
		return false;
}

bool 	TLFFGEngine::ARFilter(awpRect& rect)
{
	double ar = (double)(rect.right - rect.left) / (double)(rect.bottom - rect.top);
	if (ar >= m_minAR && ar <= m_maxAR)
		return true;
	else
		return false;
}

awpImage* TLFFGEngine::GetForeground()
{
	return m_foreground.GetImage();
}

TLFImage* TLFFGEngine::GetForegroundImage()
{
	return &m_foreground;
}

TLFThresholdProc* TLFFGEngine::GetThresholdProc()
{
	return &m_processor;
}



void TLFFGEngine::InitDetectors()
{
   ILFObjectDetector* d = (ILFObjectDetector*)m_detectors.Get(0);
   if (d != NULL)
   {
	   d->Init(&m_SourceImage);
   }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::shared_ptr<ILFObjectDetector> TLFTreeEngine::LoadDetector(TiXmlElement* node)
{
	if (node == NULL)
		return NULL;

	auto node_name = node->ValueStr();


	if (node_name == "TSCObjectDetector") {

		auto detector = std::make_shared<TSCObjectDetector>();
		//TiXmlElement* e = elem->FirstChildElement();
		if (!detector->LoadXML(node))
			return NULL;

		return detector;
	}
	return NULL;
}

std::shared_ptr<TLFAgent> TLFTreeEngine::LoadAgent(TiXmlElement* node) {
	if (node == NULL)
		return NULL;

	auto node_name = node->ValueStr();
	if (node_name == "TLFAgent") {
		auto agent = std::make_shared<TLFAgent>();
		if (!agent->LoadXML(node))
			return NULL;
		return agent;
	}
	return NULL;
}

bool TLFTreeEngine::SetAgent(const std::string& path, std::shared_ptr<TLFAgent> agent) {
	agent_tree_t* tree = &agent_tree_;
	auto splitted = SplitPath(path);

	auto agent_name = splitted.back();
	splitted.pop_back();

	for (const auto& name : splitted) {
		auto it = tree->find(name);
		if (it != tree->end()) {
			tree = &it->second.second;
		}
		else {
			//Cant find node in tree;
			return false;
		}

	}
	(*tree)[agent_name] = { std::move(agent), agent_tree_t{} };
	return true;
}

std::shared_ptr<TLFAgent> TLFTreeEngine::GetAgent(const std::string& path) const {
	agent_tree_t const* tree = &agent_tree_;
	std::shared_ptr<TLFAgent> agent;

	auto splitted = SplitPath(path);
	for (const auto& name : splitted) {
		auto it = tree->find(name);
		if (it != tree->end()) {
			tree = &it->second.second;
			agent = it->second.first;
		}
		else {
			//Cant find node in tree;
			return nullptr;
		}

	}

	return agent;
}


bool TLFTreeEngine::SetDetector(const std::string& path, std::shared_ptr<ILFObjectDetector> detector) {
	tree_t* tree = &tree_;
	auto splitted = SplitPath(path);

	auto detector_name = splitted.back();
	splitted.pop_back();

	for (const auto& name : splitted) {
		auto it = tree->find(name);
		if (it != tree->end()) {
			tree = &it->second.second;
		}
		else {
			//Cant find node in tree;
			return false;
		}

	}
	(*tree)[detector_name] = { std::move(detector), tree_t{} };
	return true;
}

std::shared_ptr<ILFObjectDetector> TLFTreeEngine::GetDetector(const std::string& path) const {
	tree_t const* tree = &tree_;
	std::shared_ptr<ILFObjectDetector> detector;

	auto splitted = SplitPath(path);
	for (const auto& name : splitted) {
		auto it = tree->find(name);
		if (it != tree->end()) {
			tree = &it->second.second;
			detector = it->second.first;
		}
		else {
			//Cant find node in tree;
			return nullptr;
		}

	}

	return detector;
}

std::vector<std::string> TLFTreeEngine::SplitPath(const std::string& s) {
	std::vector<std::string> result;
	std::stringstream ss(s);
	std::string item;

	while (std::getline(ss, item, '.')) {
		result.push_back(item);
	}

	return result;
}

std::optional<std::vector<detected_item_ptr>> TLFTreeEngine::DetectInRect(const tree_t& tree, TLFImage* image, awpRect* rect) {
	std::vector<detected_item_ptr>		detections;

	for (const auto& [name, node] : tree) {

		if (!node.first->Init(image, rect))
			//Cant initialize scanner or image;
			return std::nullopt;

		//TODO: add NMS
		//TODO: move Non Maximum Suppression to detector;
		if (node.first->Detect() > 0) {
			//Objects detected, run tree
			for (int i = 0; i < node.first->GetNumItems(); i++) {
				TLFDetectedItem* item = node.first->GetItem(i);
				auto leaf_rect = item->GetBounds().GetRect();

				auto parent = std::make_shared<TLFDetectedItem>(*item);

				detections.push_back(parent);

				auto detected = DetectInRect(node.second, image, &leaf_rect);

				if (!detected)
					return std::nullopt;

				for (auto child : *detected) {
					//TODO: move to TLFSemanticImageDescriptor
					if (!child->parent())
						child->set_parent(parent);

					detections.push_back(child);
				}

			}

		}
	}

	return detections;
}

bool TLFTreeEngine::Load(const std::string& filename) {
	FILE* file = fopen(filename.c_str(), "rb");
	if (!file)
	{
		printf("TLFTreeEngine::Load fopen failed!!!\n");
		return false;
	}
	TiXmlDocument doc;
	bool result = doc.LoadFile(file, TIXML_ENCODING_UTF8);
	fclose(file);
	if (!result)
	{
		printf("ILFDetectEngine::Load failed!!!\n");
		return false;
	}
	TiXmlHandle hDoc(&doc);
	TiXmlElement* pElem = NULL;
	TiXmlHandle hRoot(0);
	pElem = hDoc.FirstChildElement().Element();

	return this->LoadXML(pElem);
}

bool TLFTreeEngine::Save(const std::string& filename) {
	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
	doc.LinkEndChild(decl);
	TiXmlElement* engine = SaveXML();
	if (engine == NULL)
		return false;
	doc.LinkEndChild(engine);
	FILE* file = fopen(filename.c_str(), "w");
	if (!file)
	{
		printf("TLFTreeEngine::Save fopen failed!!!\n");
		return false;
	}
	bool result = doc.SaveFile(file);
	fclose(file);
	return result;
}

TiXmlElement* TLFTreeEngine::SaveXML(const agent_tree_t& tree, agents_t& agents) {
	TiXmlElement* xml_tree = new TiXmlElement("TLFTree");

	for (const auto& [name, node] : tree) {
		TiXmlElement* xml_node = new TiXmlElement("TLFTreeNode");
		xml_node->SetAttribute("name", name);

		auto agent_it = std::find(agents.begin(), agents.end(), node.first);

		if (agent_it != agents.end()) {
			xml_node->SetAttribute("agent_id", std::distance(agents.begin(), agent_it));
		}
		else {
			xml_node->SetAttribute("agent_id", agents.size());
			agents.push_back(node.first);
		}
		auto tree_elem = SaveXML(node.second, agents);

		xml_node->LinkEndChild(tree_elem);
		xml_tree->LinkEndChild(xml_node);
	}

	return xml_tree;

}

TLFTreeEngine::agent_tree_t TLFTreeEngine::LoadXML(TiXmlElement* node, const agents_t& agents) {
	agent_tree_t tree;
	for (TiXmlNode* child = node->FirstChild("TLFTreeNode"); child; child = child->NextSibling()) {

		auto elem = child->ToElement();

		if (elem) {
			int id = 0;
			elem->QueryIntAttribute("agent_id", &id);

			std::string name;

			elem->QueryValueAttribute("name", &name);

			if (agents.size() < id)
				return {};

			auto xml_tree = elem->FirstChild("TLFTree");

			agent_tree_t child_tree;

			if (xml_tree) {
				child_tree = LoadXML(xml_tree->ToElement(), agents);
			}

			tree.emplace(name, std::pair{ agents[id], std::move(child_tree) });

		}
	}
	return tree;
}

TiXmlElement* TLFTreeEngine::SaveXML(const tree_t& tree, detectors_t& detectors) {
	TiXmlElement* xml_tree = new TiXmlElement("TLFTree");

	for (const auto& [name, node] : tree) {
		TiXmlElement* xml_node = new TiXmlElement("TLFTreeNode");
		xml_node->SetAttribute("name", name);


		auto det_it = std::find(detectors.begin(), detectors.end(), node.first);

		if (det_it != detectors.end()) {
			xml_node->SetAttribute("detector_id", std::distance(detectors.begin(), det_it));
		}
		else {
			xml_node->SetAttribute("detector_id", detectors.size());
			detectors.push_back(node.first);
		}


		auto tree_elem = SaveXML(node.second, detectors);

		xml_node->LinkEndChild(tree_elem);
		xml_tree->LinkEndChild(xml_node);
	}

	return xml_tree;

}

TLFTreeEngine::tree_t TLFTreeEngine::LoadXML(TiXmlElement* node, const detectors_t& detectors) {

	tree_t tree;
	for (TiXmlNode* child = node->FirstChild("TLFTreeNode"); child; child = child->NextSibling()) {

		auto elem = child->ToElement();

		if (elem) {
			int id = 0;
			elem->QueryIntAttribute("detector_id", &id);

			std::string name;

			elem->QueryValueAttribute("name", &name);

			if (detectors.size() < id)
				return {};

			auto xml_tree = elem->FirstChild("TLFTree");

			tree_t child_tree;

			if (xml_tree) {
				child_tree = LoadXML(xml_tree->ToElement(), detectors);
			}

			tree.emplace(name, std::pair{ detectors[id], std::move(child_tree) });

		}
	}
	return tree;
}

bool TLFTreeEngine::LoadXML(TiXmlElement* parent) {
	printf("TLFTreeEngine: LoadXML.\n");

	try
	{
		if (parent == NULL)
			return false;

		if (parent->ValueStr() != GetName())
			return false;

		if (use_agent) {
			auto xml_agents = parent->FirstChild("TLFAgents");
			agents_t agents;
			if (xml_agents) {
				for (TiXmlNode* child = xml_agents->FirstChild(); child; child = child->NextSibling()) {

					auto agent = LoadAgent(child->ToElement());
					if (!agent)
						return false;

					agents.push_back(agent);
				}
			}
			auto xml_tree = parent->FirstChild("TLFTree");

			if (xml_tree) {
				agent_tree_ = LoadXML(xml_tree->ToElement(), agents);
			}

			if (agent_tree_.empty() && !agents.empty())
				return false;
		}
		else {
			auto xml_detectors = parent->FirstChild("TLFDetectors");

			detectors_t	detectors;

			if (xml_detectors) {

				for (TiXmlNode* child = xml_detectors->FirstChild(); child; child = child->NextSibling()) {

					auto detector = LoadDetector(child->ToElement());
					if (!detector)
						return false;

					detectors.push_back(detector);
				}
			}

			auto xml_tree = parent->FirstChild("TLFTree");

			if (xml_tree) {
				tree_ = LoadXML(xml_tree->ToElement(), detectors);
			}

			if (tree_.empty() && !detectors.empty())
				return false;
		}

	}
	catch (...)
	{
		printf("TLFDetectEngine: exeption while loading.");
		return false;
	}
	return true;
}




TiXmlElement* TLFTreeEngine::SaveXML() {	
	TiXmlElement* parent = new TiXmlElement(GetName());

	if (use_agent) {
		agents_t agents;
		auto xml_tree = SaveXML(agent_tree_, agents);
		parent->LinkEndChild(xml_tree);
		TiXmlElement* xml_agents = new TiXmlElement("TLFAgents");

		for (const auto& agent : agents) {
			TiXmlElement* xml_agent = agent->SaveXML();
			xml_agents->LinkEndChild(xml_agent);
		}
		parent->LinkEndChild(xml_agents);
	}
	else {
		detectors_t		detectors;
		auto xml_tree = SaveXML(tree_, detectors);
		parent->LinkEndChild(xml_tree);
		TiXmlElement* xml_detectors = new TiXmlElement("TLFDetectors");

		for (const auto& det : detectors) {
			TiXmlElement* xml_det = det->SaveXML();
			xml_detectors->LinkEndChild(xml_det);
		}

		parent->LinkEndChild(xml_detectors);
	}
	return parent;
}

void TLFTreeEngine::SetAgentMode() {
	use_agent = true;
}
