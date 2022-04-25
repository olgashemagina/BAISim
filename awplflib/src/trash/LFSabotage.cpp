#include "_LF.h"

#define SABOTAGE_IMG_SIZE 97 // 32 x 3 + 1

TLFSabotageWeak::TLFSabotageWeak(ILFFeature* feature, double sens)
{
	this->m_pFeature = LFCreateFeature(feature);//feature->Clone();
	this->m_sens = sens;

	m_bufSize = SABOTAGE_BUF_SIZE;

	m_story_buffer = new double[m_bufSize];
	memset(m_story_buffer, 0, SABOTAGE_BUF_SIZE*sizeof(double));
	m_classifyCount = 0;

	m_minValue = 0.4 - (1 - m_sens)*0.3;
	m_maxValue = 0.6 + (1 - m_sens)*0.3;
	m_diffValue = 0.05 + (1 - m_sens)*0.1;
	m_state = 0;
	m_bf = 0;

    memset(m_dt, 0, SABOTAGE_BUF_SIZE*sizeof(AWPDWORD));
}
TLFSabotageWeak::~TLFSabotageWeak()
{
	delete[] m_story_buffer;
}
/*
 лассифицирует участок изображени€, св€занный с TLFFeature
0 - все ок
1 - слишком темно
2 - слишком светло
3 - резкие изменени€
*/
int TLFSabotageWeak::Classify(TLFImage* pImage, double* value2)
{
	this->m_classifyCount++;
	double adt = 0;
	if (m_classifyCount > 1)
	{
        AWPDWORD ct = LFGetTickCount();
        AWPDWORD dt = ct - this->m_ct;
		m_ct = ct;
		int num = this->m_classifyCount >= SABOTAGE_BUF_SIZE ? SABOTAGE_BUF_SIZE : this->m_classifyCount;
		for (int i = 0; i < num - 1; i++)
			m_dt[i] = m_dt[i + 1];
		m_dt[num - 1] = dt;
		if (num == SABOTAGE_BUF_SIZE)
		{
			for (int i = 0; i < SABOTAGE_BUF_SIZE; i++)
			{
				adt += m_dt[i];
			}
			adt /= (double)SABOTAGE_BUF_SIZE;
		}
	}
	else
		m_ct = LFGetTickCount();
	

	double value = this->m_pFeature->fCalcValue(pImage);
	if (value < 0)
		return 0;



#ifdef _DEBUG
	printf("%f\t", value);
#endif 
	double vmin, vmax;
	vmin = value;
	vmax = value;
	int num = this->m_classifyCount >= m_bufSize ? m_bufSize : this->m_classifyCount;

	for (int i = 0; i < num - 1; i++)
	{
		this->m_story_buffer[i] = this->m_story_buffer[i + 1];
		if (vmin > this->m_story_buffer[i])
			vmin = this->m_story_buffer[i];
		if (vmax < this->m_story_buffer[i])
			vmax = this->m_story_buffer[i];
	}
	this->m_story_buffer[num - 1] = value;
	if (m_classifyCount  < 2 * SABOTAGE_BUF_SIZE)
		return 0;

	m_bf++;
	if (m_bf > SABOTAGE_BUF_SIZE)
		m_bf = 0;

	if (this->m_classifyCount < m_bufSize)
		return 0;

	if (m_state != 2 && (value < m_minValue || value > m_maxValue))
	{
		m_state = 1;
		return m_state;
	}
	int a = SABOTAGE_BUF_SIZE / 4;
	if (m_bf % a == 0)
	{
		double diff = fabs(vmax - vmin);
		//printf("Check diff %f.\n", diff);
		if (diff > this->m_diffValue)
		{
			//printf("Event.\n");
			m_state = 2;
			return m_state;
		}
		else m_state = 0;
	}
	return m_state;
}

void TLFSabotageWeak::SaveXML(TiXmlElement* parent)
{

}

bool TLFSabotageWeak::LoadXML(TiXmlElement* parent)
{
	return false;
}

void TLFSabotageWeak::Clear()
{
	m_classifyCount = 0;
	((TLFAvgFeature*)this->m_pFeature)->Clear();
}

int  TLFSabotageWeak::GetState()
{
	return this->m_state;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TLFSabotageStrong::SaveXML(TiXmlElement* parent)
{

}
bool TLFSabotageStrong::LoadXML(TiXmlElement* parent)
{
	return true;
}
/*
Classification
*/
int TLFSabotageStrong::Classify(TLFImage* pImage, double& err)
{
	int c = 0;
	for (int i = 0; i < this->GetCount(); i++)
	{
		ILFWeak* w = GetWeak(i);
		c += w->Classify(pImage);
	}
#ifdef _DEBUG
	printf("%i\n", c);
#endif 
	return c > GetCount() / 2;
}

void TLFSabotageStrong::Clear()
{
	for (int i = 0; i < GetCount(); i++)
	{
		TLFSabotageWeak* w = (TLFSabotageWeak*)Get(i);
		w->Clear();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TLFSabotage::TLFSabotage()
{
	m_CurrentImage = NULL;
	m_state = 0;
	m_srcBPP = 0;
	m_srcWidth = 0;
	m_srcHeight = 0;
	m_event_duration = 0;
	m_event_start = 0;
	this->m_time_thr = 500;
	double s = 0.5;
	this->m_classifyer.SetThreshold(s);
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			TLFAvgFeature f;
			TLFSabotageWeak* sw1 = new TLFSabotageWeak(&f, s);
			sw1->Setup(32, 32, j * 32, i * 32);
			m_classifyer.AddWeak(sw1);

			TLFSigmaFeature sf;

			TLFSabotageWeak* sw2 = new TLFSabotageWeak(&sf, s);
			sw2->Setup(32, 32, j * 32, i * 32);
			m_classifyer.AddWeak(sw2);
		}
	}
}
TLFSabotage::TLFSabotage(double s, double t)
{
	m_CurrentImage = NULL;
	m_state     = 0;
	m_srcBPP    = 0;
	m_srcWidth  = 0;
	m_srcHeight = 0;
	m_event_duration = 0;
	m_event_start = 0;
	this->m_time_thr = t;
	this->m_classifyer.SetThreshold((double)s);
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			TLFAvgFeature f;
			TLFSabotageWeak* sw1 = new TLFSabotageWeak(&f, s);
			sw1->Setup(32, 32, j * 32, i * 32);
			m_classifyer.AddWeak(sw1);

			TLFSigmaFeature sf;

			TLFSabotageWeak* sw2 = new TLFSabotageWeak(&sf, s);
			sw2->Setup(32, 32, j * 32, i * 32);
			m_classifyer.AddWeak(sw2);
		}
	}
}

TLFSabotage::~TLFSabotage()
{
	Clear();
}


void TLFSabotage::Clear()
{
	if (m_CurrentImage != NULL)
	{
		delete m_CurrentImage;
		m_CurrentImage = NULL;
	}
	m_state = 0;
}
// инициалици€ системы 
void TLFSabotage::Init(TLFImage* img)
{
	Clear();

	// 
	awpImage* awp = img->GetImage();
	if (awp != NULL)
	{
		m_srcWidth  = awp->sSizeX;
		m_srcHeight = awp->sSizeY;
		m_srcBPP    = awp->bChannels;
		this->m_CurrentImage = new TLFImage();
		this->Preprocess(awp);
	}

}
// предварительна€ обработка изображеи€ 
bool TLFSabotage::Preprocess(awpImage* img)
{
	if (img == NULL)
		return false;
	if (awpConvert(img, AWP_CONVERT_3TO1_BYTE) != AWP_OK)
		return false;
	if (awpResize(img, SABOTAGE_IMG_SIZE, SABOTAGE_IMG_SIZE))
		return false;
	this->m_CurrentImage->SetImage(img);
	return true;
}

bool TLFSabotage::PorocessImage(awpImage* img, bool* result)
{
	*result = false;
	if (img == NULL)
		return false;
	// преобразовани€. 
	awpImage* tmp = NULL;
	// копируем входное изображение во временное. 
	if (awpCopyImage(img, &tmp) != AWP_OK)
		return false;
	if (tmp->sSizeX != this->m_srcWidth || tmp->sSizeY != this->m_srcHeight || tmp->bChannels != this->m_srcBPP)
	{
		// переинициализаци€
		TLFImage initImage;
		initImage.SetImage(img);
		this->Init(&initImage);
		_AWP_SAFE_RELEASE_(tmp)
		return true;
	}
	// предварительна€ обработка изображеи€. 
	bool res = true;
	int state = this->m_state;
	double err;
	if (this->Preprocess(tmp))
	{
		state = this->m_classifyer.Classify(this->m_CurrentImage, err);
	}
	else
		res = false;

	if (res && state != m_state)
	{
		// состо€ние сцены изменилось
		if (this->m_event_duration == 0)
		{
			this->m_event_start = LFGetTickCount();
			this->m_event_duration = 1;
		}
		else
			this->m_event_duration = LFGetTickCount() - m_event_start;

		if (this->m_event_duration > this->m_time_thr)
		{
			this->m_state = state;
			*result = true;
		}
	}
	else
		this->m_event_duration = 0;
#ifdef _DEBUG
	//printf("state = %i\n", m_state);
#endif 

	_AWP_SAFE_RELEASE_(tmp)
	return res;
}

int TLFSabotage::GetState()
{
	if (m_state != 0)
		return 1;
	else
		return 0;
}
