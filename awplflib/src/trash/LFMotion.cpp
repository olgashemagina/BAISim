#include "_LF.h"

TLFMotionWeak::TLFMotionWeak(double t) : ILFWeak()
{
	this->m_sum = 0;
	m_counter = 0;
	m_threshold = t;
}
TLFMotionWeak::TLFMotionWeak(ILFFeature* feature, double t) :  ILFWeak(feature)
{
	this->m_sum = 0;
	m_counter = 0;
	m_threshold = t;
}
TLFMotionWeak::TLFMotionWeak(TLFMotionWeak& Weak)
{
	this->m_sum = 0;
	m_counter = 0;
	m_pFeature = LFCreateFeature(Weak.Fetaure());
	m_fWeight = 1;
	m_threshold = Weak.m_threshold;
}
TLFMotionWeak::TLFMotionWeak(TLFMotionWeak* weak) 
{
	this->m_sum = 0;
	m_counter = 0;
	if (weak != NULL)
	{
		m_pFeature = LFCreateFeature(weak->Fetaure());
		m_threshold = weak->m_threshold;
	}
	else
	{
		m_threshold = 5;
		m_counter = 0;
	}
}
TLFMotionWeak::~TLFMotionWeak()
{
}
int TLFMotionWeak::Classify(TLFImage* pImage,  double* value)
{
	if (pImage == NULL)
		return 0;

	double v = (double)m_pFeature->uCalcValue(pImage);
    *value = v;
	m_counter++;
    m_sum += v;
	if (m_counter <= 50)
    {

		return 0;
    }

	double a = m_sum / m_counter;
	double diff = fabs(v - a);

    if (m_counter == 5000)
    {
       m_sum = 0;
       m_counter = 0;
       return 0;
    }


	if (diff > 2*this->m_threshold)
		return 1; // motion detected
	else
		return 0;
}
void TLFMotionWeak::SaveXML(TiXmlElement* parent)
{
	if (parent == NULL)
		return;
//	TiXmlElement* e = new TiXmlElement(GetName());
	
}
bool TLFMotionWeak::LoadXML(TiXmlElement* parent)
{
	if (parent == NULL)
		return false;
	if (strcmp(parent->Value(), this->GetName()) != 0)
		return false;

	return true;
}

double TLFMotionWeak::GetThreshold()
{
	return this->m_threshold;
}
void TLFMotionWeak::SetThreshold(double v)
{
	this->m_threshold = v;
}

TLFMotionDetector::TLFMotionDetector() : ILFObjectDetector()
{
	m_counter = 0;
	if (m_scanner != NULL)
		delete m_scanner;
	m_scanner = new TLFTileScanner();
	m_scanner->GetParameter(0)->SetValue(50);
	m_threshold = 9;
	m_delay = 0;
	printf("INFO: create TLFMotionDetector\n");
}

bool TLFMotionDetector::Init(awpImage* pImage, bool scan)
{
	if (pImage == NULL || pImage->dwType != AWP_BYTE || m_scanner == NULL)
		return false;
	bool changed = m_Image.GetImage() == NULL || m_Image.GetImage()->sSizeX != pImage->sSizeX || m_Image.GetImage()->sSizeY != pImage->sSizeY || m_scanner->GetFragmentsCount() == 0;
	m_Image.SetImage(pImage);
	if (changed)
	{
		m_counter = 0;
		m_objects.Clear();
		this->m_weaks_a.Clear();
		this->m_weaks_s.Clear();
		this->m_weaks_h.Clear();
		this->m_weaks_v.Clear();
		this->m_weaks_d.Clear();

		m_scanner->ScanImage(&m_Image);

		for (int i = 0; i < m_scanner->GetFragmentsCount(); i++)
		{
			awpRect rect = m_scanner->GetFragmentRect(i);
			UUID id;

			ILFFeature* feature = new TLFAFeature(rect.left, rect.top, (rect.right - rect.left), (rect.bottom - rect.top));
			TLFMotionWeak* weak = new TLFMotionWeak(feature, m_threshold);

			m_weaks_a.Add(weak);
			delete feature;

			feature = new TLFSFeature(rect.left, rect.top, (rect.right - rect.left), (rect.bottom - rect.top));
			weak = new TLFMotionWeak(feature, m_threshold);

			m_weaks_s.Add(weak);
			delete feature;

			feature = new TLFHFeature(rect.left, rect.top, (rect.right - rect.left), (rect.bottom - rect.top) / 2);
			weak = new TLFMotionWeak(feature, m_threshold);
			m_weaks_h.Add(weak);
			delete feature;

			feature = new TLFVFeature(rect.left, rect.top, (rect.right - rect.left) / 2, (rect.bottom - rect.top));
			weak = new TLFMotionWeak(feature, m_threshold);
			m_weaks_v.Add(weak);
			delete feature;

			feature = new TLFDFeature(rect.left, rect.top, (rect.right - rect.left) / 2, (rect.bottom - rect.top) / 2);
			weak = new TLFMotionWeak(feature, m_threshold);
			m_weaks_d.Add(weak);
			delete feature;

			LF_NULL_UUID_CREATE(id);

			TLFDetectedItem* de = new TLFDetectedItem(&rect, 0, "Motion", 0, 0,
				this->m_baseWidth, this->m_baseHeight, this->m_strDetName.c_str(), id);
			de->SetHasObject(false);
			m_objects.Add(de);
		}
	}
	m_counter++;
	return m_Image.GetImage() != NULL;
}
// classification
int  TLFMotionDetector::ClassifyRect(awpRect Fragmnet, double* err, int* vect)
{
	*err = 0;
    *vect = 0;

	return 0;
}
int  TLFMotionDetector::Detect()
{
	for (int i = 0; i < m_objects.GetCount(); i++)
	{
		TLFDetectedItem* di = (TLFDetectedItem*)m_objects.Get(i);

		if (di != NULL)
		{
			di->SetHasObject(false);
			TLFRect* rect = di->GetBounds();
			if (rect != NULL)
			{
				double e = di->GetRaiting();
				int va, vs, vh, vv, vd;
				ILFWeak* weak_a = (ILFWeak*)this->m_weaks_a.Get(i);
				ILFWeak* weak_s = (ILFWeak*)this->m_weaks_s.Get(i);
				ILFWeak* weak_h = (ILFWeak*)this->m_weaks_h.Get(i);
				ILFWeak* weak_v = (ILFWeak*)this->m_weaks_v.Get(i);
				ILFWeak* weak_d = (ILFWeak*)this->m_weaks_d.Get(i);

				va = weak_a->Classify(&this->m_Image);
				vs = weak_s->Classify(&this->m_Image);
				vh = weak_h->Classify(&this->m_Image);
				vv = weak_v->Classify(&this->m_Image);
				vd = weak_d->Classify(&this->m_Image);

				//di->SetHasObject(vv + vh + vd > 0 && va + vs > 0);
				di->SetHasObject(vv + vh + vd + va + vs > 2);
				di->SetRaiting(0);
			}
		}
	}
	return 0;
}
// properties
int	  TLFMotionDetector::GetStagesCount()
{
	return 0;
}
bool  TLFMotionDetector::AddStrong(ILFStrong* strong)
{
	return false;
}
double TLFMotionDetector::GetThreshold()
{
	return m_threshold;
}
void TLFMotionDetector::SetThreshold(double Value)
{
	if (Value < 0 || Value > 1)
		return;
	m_threshold = 18 * (1 - Value);
	Value = m_threshold;
	TLFMotionWeak* weak = NULL;
	for (int i = 0; i < m_weaks_a.GetCount(); i++)
	{
		weak = (TLFMotionWeak*)this->m_weaks_a.Get(i);
		if (weak != NULL)
			weak->SetThreshold(Value);
		weak = (TLFMotionWeak*)this->m_weaks_s.Get(i);
		if (weak != NULL)
			weak->SetThreshold(Value);
		weak = (TLFMotionWeak*)this->m_weaks_h.Get(i);
		if (weak != NULL)
			weak->SetThreshold(Value);
		weak = (TLFMotionWeak*)this->m_weaks_v.Get(i);
		if (weak != NULL)
			weak->SetThreshold(Value);
		weak = (TLFMotionWeak*)this->m_weaks_d.Get(i);
		if (weak != NULL)
			weak->SetThreshold(Value);
	}

}

TiXmlElement* TLFMotionDetector::SaveXML()
{
	return NULL;
}
bool TLFMotionDetector::LoadXML(TiXmlElement* parent)
{
	return false;
}

TLFMotionEngine::TLFMotionEngine() : ILFDetectEngine()
{
	TLFMotionDetector* d = new TLFMotionDetector();
	this->m_detectors.Add(d);
	m_strPredictorName = "TLFMomentsPredictor";

	m_minSize = 1;
	m_maxSize = 5;
}
void TLFMotionEngine::Clear()
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
bool TLFMotionEngine::LoadXML(TiXmlElement* parent)
{
	return false;
}
bool TLFMotionEngine::FindObjects()
{
	if (this->m_SourceImage.GetImage() == NULL)
		return false;
	if (this->m_SourceImage.GetImage()->dwType != AWP_BYTE)
		return false;


	BuildForeground();

/*	if (this->m_needTrack)
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
	}*/
	return true;
}
TiXmlElement*  TLFMotionEngine::SaveXML()
{
	return NULL;
}


double TLFMotionEngine::GetMinSize()
{
	return m_minSize;
}
double TLFMotionEngine::GetMaxSize()
{
	return m_maxSize;
}

void TLFMotionEngine::SetMinSize(double value)
{
	if (value < 0 || value > m_maxSize)
		return;
	m_minSize = value;
}
void TLFMotionEngine::SetMaxSize(double value)
{
	if (value < 0 || value > 100)
		return;
	m_maxSize = value;
}


awpImage* TLFMotionEngine::GetForeground()
{
	return m_foreground.GetImage();
}
TLFImage* TLFMotionEngine::GetForegroundImage()
{
	return &m_foreground;
}

TLFThresholdProc* TLFMotionEngine::GetThresholdProc()
{
	return &m_processor;
}

void TLFMotionEngine::OverlapsFilter(TLFSemanticImageDescriptor* descriptor)
{
	awpImage* img = m_foreground.GetImage();
	awpStrokeObj* obj = NULL;
	int num = 0;
	awpGetStrokes(img, &num, &obj, 128, NULL);
	if (num == 0)
		return;
	for (int i = 0; i < num; i++)
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
				TLFRect* di_rect = di->GetBounds();
				if (di_rect->RectOverlap(r)>0 || di_rect->Distance(r) < 5)
				{
					found = true;
					break;
				}
			}
		}

		if (found)
			continue;

		if (SquareFilter(rect))
		{
			UUID id;
			LF_NULL_UUID_CREATE(id);
			TLFDetectedItem* dd = new  TLFDetectedItem(&rect, 0, "Motion", 0, 0, 24, 24, "TLFFGBGDetector", id);
			descriptor->AddDetectedItem(dd);
			delete dd;
		}
	}
	awpFreeStrokes(num, &obj);
}
void TLFMotionEngine::InitDetectors()
{
	ILFObjectDetector* d = (ILFObjectDetector*)m_detectors.Get(0);
	if (d != NULL)
	{
		awpImage* img = m_SourceImage.GetImage();
		d->Init(img);
	}
}

bool 	TLFMotionEngine::SquareFilter(awpRect& rect)
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
void    TLFMotionEngine::BuildForeground()
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

	if (d->GetNumItems() > 0)
	{
		for (int i = 0; i < d->GetNumItems(); i++)
		{
			TLFDetectedItem* item = d->GetItem(i);
			if (item != NULL && item->HasObject())
			{
				awpRect    item_rect = item->GetBounds()->GetRect();
				awpImage*  fg = m_foreground.GetImage();
				awpFillRect(fg, &item_rect, 0, 255);
				// todo: Here is memory leak!!!!!! 
				//TLFDetectedItem* di = new TLFDetectedItem(item);
				//di->Resize(m_resizeCoef);
				//m_tmpList.Add(di);
				count++;
			}
		}
	}
}