#include "_LF.h"
//-------------------------------ILFPredictor-----------------------------------
TLFRect* ILFPredictor::GetPredicted()
{
	return this->m_pPredicted;
}
//-------------------------------TLFAverageNNPredictor--------------------------
TLFAverageNNPredictor::TLFAverageNNPredictor()
{
	m_pPredicted = NULL;
	memset(&m_rect, 0, sizeof(awpRect));
}
void TLFAverageNNPredictor::Learn(ILFDetectEngine* engine, TLFRect* rect)
{
	this->m_list.Clear();

	if (engine == NULL)
		return;
	if (rect == NULL)
		return;

	ILFObjectDetector* detector = NULL;
	detector = engine->GetDetector(0);
	if (detector == NULL)
		return;
	ILFScanner* scanner = detector->GetScanner();
	if (scanner == NULL)
		return;

	int cp = 0;
	int cn = 0;
	TLFDblVector* d = this->Features(engine, rect, 1);
	m_list.Add(d);
	for (int i = 0; i < scanner->GetFragmentsCount(); i++)
	{
		//
		TLFBounds* b = scanner->GetFragment(i);
		if (b)
		{
			double overlap = rect->RectOverlap(b->Rect);
			TLFRect r(b->Rect);
			if (overlap > 0.8)
			{
				// добавляем positive sample
				TLFDblVector* d = this->Features(engine, &r, 1);
				if (d != NULL)
				{
					m_list.Add(d);
					cp++;
				}
			}
			else if (overlap > 0.4 && overlap < 0.5)
			{
				int rnd = rand() % 10 + 1;
				if (rnd > 8)
					continue;
				// добавляем negative sample
				TLFDblVector* d = this->Features(engine, &r, 0);
				if (d != NULL)
				{
					m_list.Add(d);
					cn++;
				}
			}
		}
	}
	this->m_rect = rect->GetRect();
}
TLFRect* TLFAverageNNPredictor::Predict(ILFDetectEngine* engine)
{
	if (this->m_list.GetCount() == 0)
		return NULL;
	if (engine == NULL)
		return NULL;

	if (m_pPredicted != NULL)
	{
		delete m_pPredicted;
		m_pPredicted = NULL;
	}

	ILFObjectDetector* detector = NULL;
	detector = engine->GetDetector(0);
	if (detector == NULL)
		return NULL;
	ILFScanner* scanner = detector->GetScanner();
	if (scanner == NULL)
		return NULL;
	TLFRect rect(m_rect);
	double min_err = FLT_MAX;

	awpRect result;

	for (int i = 0; i < scanner->GetFragmentsCount(); i++)
	{
		TLFBounds* b = scanner->GetFragment(i);
		TLFRect   rr(b->Rect);
		if (rect.RectOverlap(rr) > 0.3)
		{
			TLFDblVector* d = this->Features(engine, &rr, 0);
			if (d != NULL)
			{
				double e;
				if (this->Classify(d, &e))
				{
					if (e < min_err)
					{
						min_err = e;
						result = b->Rect;
					}
				}
				delete d;
			}
		}
	}
	if (min_err < 1)
	{
		TLFRect* r_result = new TLFRect(result);
		m_pPredicted = r_result;
		return r_result;
	}
	else
		return NULL;

}
TLFDblVector* TLFAverageNNPredictor::Features(ILFDetectEngine* engine, TLFRect* rect, int id)
{
	if (engine == NULL || rect == NULL)
		return NULL;
	ILFObjectDetector* d = engine->GetDetector(0);
	if (d == NULL)
		return NULL;
	TLFImage* img = d->GetImage();
	img->GetIntegralImage();
	if (img == NULL || img->GetImage() == NULL)
		return NULL;

	//
	int idx = d->GetStagesCount() - 1;
	TLFObjectList* stages = d->GetStrongs();

	ILFStrong* s = dynamic_cast<TCSStrong*>(stages->Get(idx));
	if (s == NULL)
		return NULL;
	int width = rect->Width();
	double scale_coef = (double)width / (double)d->GetBaseWidth();
	TLFRect fragment;
	TLFDblVector* data = new TLFDblVector(id);
	for (int i = 0; i < s->GetCount(); i++)
	{
		ILFWeak  * weak = s->GetWeak(i);
		if (weak != NULL)
		{
			TCSSensor* sensor = dynamic_cast<TCSSensor*>(weak->Fetaure());
			awpRect Fragment = sensor->GetRect();

			fragment.SetRect(Fragment);
			fragment.Scale(scale_coef);
			fragment.Shift(rect->Left(), rect->Top());

			Fragment = fragment.GetRect();
			double s = fragment.Width()*fragment.Height();
			double value = img->CalcLnSum(Fragment.left, Fragment.top, fragment.Width(), fragment.Height());
			value /= s;
			data->AddValue(value);
		}
	}
	return data;
}
bool TLFAverageNNPredictor::Classify(TLFDblVector* d, double* err)
{
	bool result = false;
	*err = 0;
	double min_positive = FLT_MAX;
	double min_negative = FLT_MAX;
	double value;
	TLFL2Distance dist;
	for (int i = 0; i < m_list.GetCount(); i++)
	{
		TLFDblVector* dd = dynamic_cast<TLFDblVector*>(m_list.Get(i));
		if (dd)
		{
			value = dist.ro(d,dd);
			if (dd->id() == 1)
			{
				// positive sample
				if (value < min_positive)
					min_positive = value;
			}
			else if (dd->id() == 0)
			{
				if (value < min_negative)
					min_negative = value;
			}
		}
	}
	if (min_positive == min_negative)
		return false;
	if (min_negative == 0)
		return false;
	*err = min_positive / min_negative;
	return *err < 1;

}
//-------------------------------TLFEmptyAverageNNPredictor----------------------
TLFDblVector* TLFEmptyAverageNNPredictor::Features(ILFDetectEngine* engine, TLFRect* rect, int id)
{
	if (engine == NULL || rect == NULL)
		return NULL;
	ILFObjectDetector* d = engine->GetDetector(0);
	if (d == NULL)
		return NULL;
	TLFImage* img = d->GetImage();
	if (img == NULL)
		return NULL;
	TLFDblVector* data = new TLFDblVector(id);
	double w = (double)rect->Width() / 8;
	double h = (double)rect->Height() /8;
	double s = w*h;
	for (int y = 0; y < 8; y++)
	{
		int yy = (int)floor(rect->Top() + y*w + 0.5);
		for (int x = 0; x < 8 ; x++)
		{
			int xx = (int)floor(rect->Left() + x*w + 0.5);
			double value = img->CalcLnSum(xx, yy, (int)w, (int)h)/s;
			data->AddValue(value);
		}
	}
	return data;
}
//-------------------------------TLFNullPredictor----------------------
TLFDblVector* TLFNullPredictor::Features(ILFDetectEngine* engine, TLFRect* rect, int id)
{
	return NULL;
}
bool TLFNullPredictor::Classify(TLFDblVector* d, double* err)
{
	return true;
}
TLFRect* TLFNullPredictor::GetPredicted()
{
	return NULL;
}
void TLFNullPredictor::Learn(ILFDetectEngine* engine, TLFRect* rect)
{
	
}
TLFRect* TLFNullPredictor::Predict(ILFDetectEngine* engine)
{
	return NULL;
}


TLFIntegralImagePredictor::TLFIntegralImagePredictor()
{
	 this->m_pPredicted = NULL;
	 m_scanner = NULL;
}
TLFIntegralImagePredictor::~TLFIntegralImagePredictor()
{
	if (m_pPredicted != NULL)
		delete m_pPredicted;
	if (m_scanner != NULL)
		delete m_scanner;
}

TLFDblVector* TLFIntegralImagePredictor::Features(ILFDetectEngine* engine, TLFRect* rect, int id)
{
	return NULL;
}
bool TLFIntegralImagePredictor::Classify(TLFDblVector* d, double* err)
{
	return true;
}
void TLFIntegralImagePredictor::Learn(ILFDetectEngine* engine, TLFRect* rect)
{
	TLFFGEngine* e = dynamic_cast<TLFFGEngine*>(engine);
	if (e == NULL)
		return;
   //	if (m_pPredicted != NULL)
   //		return;

	m_rect.SetRect(rect->GetRect());
	if (m_scanner != NULL)
		delete m_scanner;
	m_scanner = new TLFTileScaleScanner(m_rect.Width(),m_rect.Height(), 0.5, 2, 1.2, 75);
	m_scanner->Scan(e->GetForeground()->sSizeX, e->GetForeground()->sSizeY);
}
TLFRect* TLFIntegralImagePredictor::Predict(ILFDetectEngine* engine)
{
	TLFFGEngine* e = dynamic_cast<TLFFGEngine*>(engine);
	if (e == NULL)
		return NULL;
	if (m_pPredicted != NULL)
	{
		delete m_pPredicted;
		m_pPredicted = NULL;
	}
	TLFImage* fg = e->GetForegroundImage();
	double max_s = 0;
	double value;
	double overlap;
	double s;
	int idx = -1;
	for (int i = 0; i < m_scanner->GetFragmentsCount(); i++)
	{
		awpRect rect = m_scanner->GetFragmentRect(i);
		s  = (rect.right - rect.left)*(rect.bottom - rect.top);

		overlap = m_rect.RectOverlap(rect);
		if (overlap > 0.5)
		{
			value = pow(1.1,overlap)*fg->CalcLnSum(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top) /s;
			if (value > max_s)
			{
				max_s = value;
				idx = i;
			}
		}
	}
	if (idx < 0)
	{
		m_pPredicted = NULL;
		return NULL;
	}
	else
	{
		TLFRect* r_result = new TLFRect();
		r_result->SetRect(m_scanner->GetFragmentRect(idx));
		awpRect rr = r_result->GetRect();
		//awpFillRect(e->GetForeground(), &rr, 0, 0);
		m_pPredicted = r_result;
		m_rect.SetRect(r_result->GetRect());
		return r_result;
	}
}
// -------------------- moments predictor -------------------------------
TLFMomentsPredictor::TLFMomentsPredictor()
{
	 this->m_pPredicted = NULL;
}
TLFMomentsPredictor::~TLFMomentsPredictor()
{
      if (this->m_pPredicted != NULL) {
          	delete this->m_pPredicted;
      }
}
void TLFMomentsPredictor::Learn(ILFDetectEngine* engine, TLFRect* rect)
{
	if (rect != NULL)
		m_rect.SetRect(rect->GetRect());
}
TLFRect* TLFMomentsPredictor::Predict(ILFDetectEngine* engine)
{
 	TLFFGEngine* e = dynamic_cast<TLFFGEngine*>(engine);
	if (e == NULL)
		return NULL;
	if (m_pPredicted != NULL)
	{
		delete m_pPredicted;
		m_pPredicted = NULL;
	}
	TLFImage* fg = e->GetForegroundImage();
	if (fg != NULL)
	{
	   SLFBinaryMoments m;
	   TLFThresholdProc* proc = e->GetThresholdProc();
	   if (e != NULL)
	   {
			TLFRect r;
			r.SetRect(m_rect.GetRect());
			r.Inflate(2,2);
			proc->ProcessMoments(r, m);
			if (m.area > 0)
			{
				awpRect o_rect;
				o_rect.left = (AWPSHORT)GetMinMomnetsX(m); if (o_rect.left < 0)  o_rect.left = 0; 
				o_rect.top = (AWPSHORT)GetMinMomnetsY(m);  if (o_rect.top < 0) o_rect.top = 0;
				o_rect.right = (AWPSHORT)GetMaxMomnetsX(m); if (o_rect.right >= fg->GetImage()->sSizeX) o_rect.right = fg->GetImage()->sSizeX - 1;
				o_rect.bottom = (AWPSHORT)GetMaxMomnetsY(m); if (o_rect.bottom >= fg->GetImage()->sSizeY) o_rect.bottom = fg->GetImage()->sSizeY - 1;
				double w  = o_rect.right - o_rect.left;
				double h  = o_rect.bottom - o_rect.top;
				double s = (o_rect.right - o_rect.left)*(o_rect.bottom - o_rect.top);
                if (s == 0) {
                	return NULL;
                }
				double s1 = fg->GetImage()->sSizeX*fg->GetImage()->sSizeY;
   				if (m.area/s > 0)
				{

					TLFRect* r_result = new TLFRect();
					r_result->SetRect(o_rect);
					r_result->Inflate(2,2);
					m_pPredicted = r_result;
					m_rect.SetRect(r_result->GetRect());
					return r_result;

				}
				else
		  			return NULL;
			}
			else
				return NULL;
	   }
	   else
		return NULL;
	}
	else
		return NULL;
}

TLFDblVector* TLFMomentsPredictor::Features(ILFDetectEngine* engine, TLFRect* rect, int id)
{
	return NULL;
}

bool TLFMomentsPredictor::Classify(TLFDblVector* d, double* err)
{
	return false;
}


TLFCarPredictor::TLFCarPredictor()
{
	m_image = NULL;
	this->m_pPredicted = NULL;
}
TLFCarPredictor::~TLFCarPredictor()
{
	_AWP_SAFE_RELEASE_(m_image)
}
void TLFCarPredictor::Learn(ILFDetectEngine* engine, TLFRect* rect)
{
	if (m_image == NULL)
	{
		// copy image fragment with resize 
		awpImage* _img = NULL;
		_img = engine->GetDetector(0)->GetImage()->GetImage();
		awpRect _r = rect->GetRect();
		awpCopyRect(_img, &m_image, &_r);
		awpResize(m_image, 36,24);
        awpConvert(m_image, AWP_CONVERT_3TO1_BYTE);
        m_rect.SetRect(rect->GetRect());
	}
}
TLFRect* TLFCarPredictor::Predict(ILFDetectEngine* engine)
{
 	if (m_pPredicted != NULL)
	{
		delete m_pPredicted;
		m_pPredicted = NULL;
	}
   TLFTileScaleScanner scanner(36, 24, 2,5, 1.1, 90);
   awpImage* _img = NULL;
   _img = engine->GetDetector(0)->GetImage()->GetImage();
   scanner.Scan(_img->sSizeX, _img->sSizeY);
   AWPDOUBLE max_dist = 0;
   awpRect   max_rect;
	for (int i = 0; i < scanner.GetFragmentsCount(); i++)
	{
		awpRect r1 = scanner.GetFragmentRect(i);
		double overlap = m_rect.RectOverlap(r1);
		if (overlap > 0.5)
		{
			awpImage* test = NULL;
            awpCopyRect(_img, &test, &r1);
            awpResize(test, 36,24);
            awpConvert(test, AWP_CONVERT_3TO1_BYTE);

            //
            double d;
            awpDistance(m_image, test, AWP_DIST_NCC,&d);
            if (max_dist < d)
            {
				max_dist = d;
                max_rect = r1;
            }
            awpReleaseImage(&test);
		}
	}
  if (max_dist > 0.5)
  {
       TLFRect* r_result = new TLFRect();
       r_result->SetRect(max_rect);
        m_pPredicted = r_result;
        m_rect.SetRect(r_result->GetRect());
		return r_result;
  }
  else
   return NULL;
}
TLFDblVector* TLFCarPredictor::Features(ILFDetectEngine* engine, TLFRect* rect, int id)
{
	return NULL;
}
bool TLFCarPredictor::Classify(TLFDblVector* d, double* err)
{
	return false;
}



//-------------------------------Predictors factory----------------------
ILFPredictor* LFCreatePredictor(const char* lpName, ILFDetectEngine* engine)
{
	if (strcmp(lpName, "TLFAverageNNPredictor") == 0)
		return new TLFAverageNNPredictor();
	else if (strcmp(lpName, "TLFEmptyAverageNNPredictor") == 0)
		return new TLFEmptyAverageNNPredictor();
	else if (strcmp(lpName, "TLFNullPredictor") == 0)
		return new TLFNullPredictor();
	else if (strcmp(lpName, "TLFIntegralImagePredictor") == 0)
		return new TLFIntegralImagePredictor();
	else if (strcmp(lpName, "TLFMomentsPredictor") == 0)
		return new TLFMomentsPredictor();
	else if (strcmp(lpName, "TLFCarPredictor") == 0)
		return new TLFCarPredictor();	return NULL;
}
 



