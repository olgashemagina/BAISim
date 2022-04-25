
#include "_LF.h"


using namespace std; 
class ProgressMeter
{
public:
    ProgressMeter():m_counter(0){}

    bool Update( )
    {
        if ( ++m_counter >= m_sum )
        {
            m_sum += m_unit;
            return true;
        }
        return false;
    }

    void Init( int units, int count )
    {
        m_counter = 0;
        m_unit = (double)count / (double)units;
        m_sum = m_unit;
    }


private:

    double      m_sum;
    double      m_unit;
    int         m_counter;
};




//---------------------------------------------------------------------------
TCSAdaBoost::TCSAdaBoost()
{
  	m_strArtefactsBase = "";
	m_strObjectsbase   = "";

    m_widthBase = 24;
    m_heightBase = 24;
    m_nFeaturesCount = 200;
	m_FinishFar = 0.2;
  	m_strOutName = "cascade.dat";
    m_nTestFaces = 0;
    m_nTestNonFaces = 0;
    m_nFaces = 0;
    m_nNonFaces = 0;
    m_pLog = NULL;
	string str = "adaboost.log";
	if (m_pLog == NULL)
		m_pLog = new ofstream(str.c_str());

	m_CSFeature  = true;
	m_LBPFeature = false;
	m_HFeature = false;
	m_VFeature = false;
	m_DFeature = false;
	m_CFeature = false;
	m_HFeature = false;
	m_VFeature = false;
	m_DFeature = false;
	m_CFeature = false;


}

bool TCSAdaBoost::LoadSamples()
{
	this->m_TrainingSamples.Clear();
	//Загрузка образцов для обучения
	DbgMsg( "Loading training sets...\n" );
	if (!LoadSample( m_TrainingSamples, 0, m_strArtefactsBase ))
        return false;
    m_nNonFaces = m_TrainingSamples.GetCount();
	if (!LoadSample( m_TrainingSamples, 1,  m_strObjectsbase))
        return false;

	m_nFaces = (int)m_TrainingSamples.GetCount( ) - m_nNonFaces;
    m_nTestFaces = m_nFaces;
    m_nTestNonFaces = m_nNonFaces;
	return true;
}


bool TCSAdaBoost::Load (TiXmlElement* node, int cascade)
{
	m_TrainingSamples.Clear();
    TiXmlElement* pElem = NULL;
    pElem = node;
	// создает новый файл протокола работы алгоритма
	string str = pElem->Attribute("logfile");
	if (m_pLog == NULL)
		m_pLog = new ofstream(str.c_str());


	m_strArtefactsBase = pElem->Attribute("database0");
    m_strObjectsbase = pElem->Attribute("database1");


    //
	m_widthBase = 70;
	m_heightBase = 20;

	m_strOutName = "";
    double ni = 5;
    pElem->Attribute("count_feat", &ni);
	m_nFeaturesCount = (int)ni;

    // инициализация исходного массива признаков
	InitFeatures( );
	return true;

}

bool TCSAdaBoost::Load  (const char* lpFileName)
{
   TiXmlDocument doc(lpFileName);
   if (!doc.LoadFile())
    return false;
   TiXmlHandle hDoc(&doc);
   TiXmlElement* pElem = NULL;
   pElem = hDoc.FirstChildElement().Element();
   if (!pElem)
      return false;
   const char* name = pElem->Value();
   if (strcmp(name, "csboost") != 0)
      return false;
   return Load( pElem, 0);
}

bool TCSAdaBoost::Boost(int stage)
{
    // Начало накачки.
    double wf   = 0.5/m_nFaces;
    double wnf  = 0.5/m_nNonFaces;

    DbgMsg("\nBoosting started.\n");

    // Установка начальных весов для обучающего набора m_TrainingSet
    for ( int i = 0; i < m_TrainingSamples.GetCount(); i++)
    {
        TCSSample* s = (TCSSample*)m_TrainingSamples.Get(i);
        if (s->GetFlag() == 1)
            s->SetWeight(wf);
        else if (s->GetFlag() == 0)
            s->SetWeight(wnf);
        else
            continue;
    }

    ProgressMeter progress;
    progress.Init( 30, (int)m_TrainingSamples.GetCount( ) );

	m_ResultClass.Clear();
	m_trainData.clear();

	if (m_Features.GetCount() == 0)
	{
		DbgMsg("Error: features set is empty.\n");
		return false;
	}

    //Main loop;
    for ( int i = 0; i < m_nFeaturesCount ; ++i )
    {

        DbgMsg( "\nUpdating: " );
        progress.Init( 30, m_Features.GetCount( ) );
	    int count = 0;

//Update
#ifdef _OMP_
#pragma omp parallel for reduction(+: count) num_threads(omp_get_max_threads())
#endif 
     	for ( int w = 0; w < m_Features.GetCount(); ++w )
        {
			//printf(".");
			++count;
            TCSWeakTraining* wcinfo = (TCSWeakTraining*)m_Features.Get(w);
            wcinfo->SetEpsilon(0);

            //Train section;
            wcinfo->StartTraining();
            for ( int i = 0; i < m_TrainingSamples.GetCount(); i++)
            {
               TCSSample* s = (TCSSample*)m_TrainingSamples.Get(i);
			   wcinfo->AddSample(s, s->GetFlag(), s->GetWeight(), this->m_widthBase, this->m_heightBase);
            }
            wcinfo->Train();

            //Calc error;
            for ( int i = 0; i < m_TrainingSamples.GetCount(); i++)
            {
                TCSSample* s = (TCSSample*)m_TrainingSamples.Get(i);

				double factor = std::min<double>((double)(s->GetImage()->sSizeX) / m_widthBase, (double)(s->GetImage()->sSizeY) / m_heightBase);
				wcinfo->Scale(factor);

				int res = wcinfo->Classify( s );

                if ( s->GetFlag() != res )
                {
                    wcinfo->SetEpsilon(wcinfo->GetEpsilon() + s->GetWeight());
                }
            }

            if ( progress.Update( ) )
	    	    DbgMsg(">");
        }

        DbgMsg( "\n" );
		printf( "Count stage: %d \n", count );
		// выбор лучшего признака, по минимальной ошибке классификации на обучающем наборе.
		int bestWc = 0;
		for ( int w = 0; w < m_Features.GetCount(); ++w )
		{
            TCSWeakTraining* best       = (TCSWeakTraining*)m_Features.Get(bestWc);
            TCSWeakTraining* current    = (TCSWeakTraining*)m_Features.Get(w);
            if ( best->GetEpsilon() > current->GetEpsilon()  )
                bestWc = w;
		}
        TCSWeakTraining* pWc       = (TCSWeakTraining*)m_Features.Get(bestWc);
		pWc->SetWeight( pWc->GetEpsilon() );

		// если ошибка лучшего классификатора превысила критический уровень 50%
		// прекращаем процесс накачки.
		if ( pWc->Weight() >= 0.5 ) break;


        //Update wc weight;
        double beta = pWc->Weight()/ (1 - pWc->Weight());
        double min_eps = pWc->Weight();
        pWc->SetWeight (log(1.0/beta));

        //Добавим "слабый" классификатор в "комитет".
        m_ResultClass.AddWeak(new TCSWeak(pWc));

        // распечатка атрибутов классификатора
        DbgMsg( "\nRound num = " + TypeToStr(i + 1) +
            ", min_eps = " + TypeToStr(min_eps) +
            ", beta = " + TypeToStr( beta ) +
		", alpha = "+ TypeToStr(pWc->Weight())+";\n" );
	// распечатка атрибутов выбранного признака
	if ( pWc->Fetaure() != NULL )
            PrintFeature(pWc->Fetaure());

		//распечатка ошибки на тестовом наборе.
		double afrr = 100;
        double FARV = PrintStatistics( m_ResultClass, afrr );
		//DbgMsg("afrr =" + TypeToStr(afrr) +"\n");

		// завершаем процесс, если ошибка уменьшилась до порогового значения
		if (FARV < m_FinishFar && afrr < 0.001)
		{
            //this->SaveFRRSamples(stage);
			return true;
		}

        // для каждого образца обновим веса
        double wt = 0;
        for ( int i = 0; i < m_TrainingSamples.GetCount(); i++ )
        {
            TCSSample* s = (TCSSample*)m_TrainingSamples.Get(i);

            int res = 0;
            double err = 0;
			double factor = (double)(s->GetImage()->sSizeX)/m_widthBase;
			pWc->Scale(factor);
            res = pWc->Classify( s );


            if ( res == s->GetFlag() )
                s->SetWeight(s->GetWeight()*beta);
            wt += s->GetWeight();
        }

        //Normalize;
        wt = 1.0/wt;

        for ( int i = 0; i < m_TrainingSamples.GetCount(); i++ )
        {
            TCSSample* s = (TCSSample*)m_TrainingSamples.Get(i);
            s->SetWeight(s->GetWeight()*wt);
        }

        //Удалим найденный признак из списка признаков
        m_Features.Delete(bestWc);

    }

	//this->SaveFRRSamples(stage);
    return true;
}

// распечатка сообщения на экране и в лог файле
void TCSAdaBoost::DbgMsg( std::string const& msg)
{
    (*m_pLog) << msg;
    cout << msg;
    flush( *m_pLog );
}

std::string TCSAdaBoost::GetArtefactsBase()
{
	return m_strArtefactsBase;
}
void TCSAdaBoost::SetArtefactsBase(std::string str)
{
	m_strArtefactsBase = str;
}
std::string TCSAdaBoost::GetObjectsBase()
{
	return m_strObjectsbase;
}

void TCSAdaBoost::SetObjectsBase(std::string str)
{
	m_strObjectsbase = str;
}

// возвращает ширину и высоту изображений, которые используются
// для обучения
AWPWORD TCSAdaBoost::WidthBase()
{
	return m_widthBase;
}
void TCSAdaBoost::SetWidthBase(AWPWORD value)
{
	m_widthBase = value;
}
AWPWORD TCSAdaBoost::HeightBase()
{
	return m_heightBase;
}
void TCSAdaBoost::SetHeightBase(AWPWORD value)
{
	m_heightBase = value;
}
TErrTrainData& TCSAdaBoost::GetTrainData()
{
	return m_trainData;
}

TCSStrong* TCSAdaBoost::GetClass()
{
	return &m_ResultClass;
}
int TCSAdaBoost::GetFeaturesCount()
{
	return m_nFeaturesCount;
}
void TCSAdaBoost::SetFeaturesCount(int value)
{
	m_nFeaturesCount = value;
}

double TCSAdaBoost::GetFinishFar()
{
	return this->m_FinishFar;
}
void TCSAdaBoost::SetFinishFar(double value)
{
	this->m_FinishFar = value;
}

std::string TCSAdaBoost::GetPath()
{
	return this->m_strPath;
}

void TCSAdaBoost::SetPath(const char* lpPath)
{
	this->m_strPath = lpPath;
}

static bool _IsImageFile(std::string& strFileName)
{
	std::string strExt = LFGetFileExt(strFileName);
	std::transform(strExt.begin(), strExt.end(), strExt.begin(), ::tolower);

	if (strExt == ".awp")
		return true;
	if (strExt == ".jpg")
		return true;
	if (strExt == ".jpeg")
		return true;
	if (strExt == ".png")
		return true;
	if (strExt == ".bmp")
		return true;
	return false;
}

// загружает образцы для обучения из директории path и сохраняет их в списке объектов 
// SampleList. в зависимости от установленного флага flag образцы считаются образцами 
// объектов или образцами фонов. flag = 1 соответствует образцам объектов, flag = 0 - 
// соответствует образцам фонов.
bool TCSAdaBoost::LoadSample(TLFObjectList& SampleList, int flag, std::string const& path)
{
	DbgMsg( "Loading samples from " + path + "... \n" );
    int count = 0;
	TLFStrings names;
	if (!LFGetDirFiles(path.c_str(), names))
	{
		DbgMsg("Reading names failed.");
		return false;
	}
	if (names.size() == 0)
	{
		DbgMsg("Database is empty.");
		return false;
	}
	DbgMsg("Reading " + TypeToStr(names.size()) + " files.\n");
	for (int i = 0; i < names.size(); i++)
	{
		string name = names[i];
		if (!_IsImageFile(name))
			continue;
		TCSSample* pSample = new TCSSample();
		//DbgMsg(name + "\n");
		pSample->LoadFromFile((char*)name.c_str());
		double e = 0;

		if (pSample->GetImage()->sSizeX < this->m_widthBase || pSample->GetImage()->sSizeY < this->m_heightBase)
		{
			delete pSample;
			continue;
		}
		awpImage* img = pSample->GetIntegralImage();
		if (img == NULL)
			continue;
		pSample->GetSqIntegralImage();
		bool need_process = flag == 1 ? true : false;
		pSample->SetFlag(flag);
		SampleList.Add(pSample);
		++count;
	}

	DbgMsg(TypeToStr(count) + " ok\n");
	return count > 0;

}
//
void TCSAdaBoost::InitFeatures()
{
   
	DbgMsg("AdaBoost: init features .... ");
	srand(11);
	m_Features.Clear();

   int sx = 1;
   int sy = 1;
   int w = 3;
   int h = 3;
   for (h = m_heightBase; h >= 3; h-=1)
   {
	   for (w = m_widthBase; w >= 3; w-=1)
	   {
		   for (sy = 0; sy <= m_heightBase - h -3; sy++)
		   {
			   for (sx = 0; sx <= m_widthBase - w -3; sx++)
			   {
				   
				   bool skip = false;// (rand() % 10) > 2;
				   if (skip)
 					   continue;
				   TCSWeakTraining* wt = NULL;
				   TLFVFeature Sensor0(sx, sy, w/2, h);
				   wt = new TCSWeakTraining(&Sensor0);
				   if (this->m_VFeature)
					   m_Features.Add(wt);
				   else
					   delete wt;
				   TLFHFeature Sensor1(sx, sy, w, h/2);
				   wt = new TCSWeakTraining(&Sensor1);
				   if (this->m_HFeature)
					m_Features.Add(wt);
				   else
					   delete wt;
				   TLFDFeature Sensor2(sx, sy, w / 2, h / 2);
				   wt = new TCSWeakTraining(&Sensor2);
				   if (this->m_DFeature)
					m_Features.Add(wt);
				   else
					   delete wt;
				   TLFCFeature Sensor3(sx, sy, w / 3, h / 3);
				   wt = new TCSWeakTraining(&Sensor3);
				   if (this->m_CFeature)
					m_Features.Add(wt);
				   else
					   delete wt;
				   TCSSensor Sensor4(sx, sy, w / 3, h / 3);
				   wt = new TCSWeakTraining(&Sensor4);
				   if (this->m_CSFeature)
					m_Features.Add(wt);
				   else
					   delete wt;
				   TLFLBPFeature Sensor5(sx, sy, w / 3, h / 3);
				   wt = new TCSWeakTraining(&Sensor5);
				   if (this->m_LBPFeature)
					m_Features.Add(wt);
				   else
					   delete wt;

				   //TLFColorSensor9Bit Sensor10(sx,sy,w,h);
				   //wt = new TCSWeakTraining(&Sensor10);
				   //m_Features.Add(wt);
			   }
		   }
	   }
   }
   DbgMsg(" done .\n");
   DbgMsg( "AdaBoost: Initiated " + TypeToStr( m_Features.GetCount() )+" features.\n" );
}

void TCSAdaBoost::PrintFeature(ILFFeature* pSensor)
{
    DbgMsg( "Found feature:" + TypeToStr(pSensor->GetName()) + " x = " + TypeToStr( pSensor->sxBase() ) +
        ", y = " + TypeToStr( pSensor->syBase() ) +
        ", width = " + TypeToStr( pSensor->wUnitBase() ) +
        ", height = " + TypeToStr( pSensor->hUnitBase() ) + ";\n");
}

double TCSAdaBoost::PrintStatistics(TCSStrong& Class, double& afrr)
{
    double          dFar = 0, dFrr = 0;
	double min_err = 100000;

    for (  int i = 0; i < m_TrainingSamples.GetCount(); i++ )
    {

        TCSSample* s = (TCSSample*)m_TrainingSamples.Get(i);

        double err = 0;
		awpRect r; r.left =0; r.top = 0;
		r.right = s->GetImage()->sSizeX;
		r.bottom = s->GetImage()->sSizeY;
		Class.Setup(r, m_widthBase);

		Class.Classify(s, err );

        if ( s->GetFlag() == 1 )
        {
			if (err < min_err)
				min_err = err;

            if ( err < Class.GetThreshold() ) dFrr += 1.0;
        }
        else
        {
            if ( err >= Class.GetThreshold() )
            {
                dFar += 1.0;
            }
        }


    }

    dFrr /= m_nTestFaces;
    dFar /= m_nTestNonFaces;

    DbgMsg( "\nFAR = " + TypeToStr( dFar ) +
            ", FRR = " + TypeToStr( dFrr ) +
            ", threshold = " + TypeToStr( Class.GetThreshold() )+ ";\n" );

	//todo:
	afrr = dFrr;
	
	TWclErrInfo err_info;
	err_info.m_far = dFar;
	err_info.m_frr = dFrr;
	err_info.m_thr = Class.GetThreshold();

	m_trainData.push_back(err_info);
    return dFar;

	double ff = 0;
	double dFar1 = 0;
	double dFrr1 = 0;

    for (  int i = 0; i < m_TrainingSamples.GetCount(); i++ )
    {

        TCSSample* s = (TCSSample*)m_TrainingSamples.Get(i);

        double err = 0;
		awpRect r; r.left =0; r.top = 0;
		r.right = s->GetImage()->sSizeX;
		r.bottom = s->GetImage()->sSizeY;
		Class.Setup(r, m_widthBase);


        Class.Classify( s, err );

        if ( s->GetFlag() == 1 )
        {
			if (err < min_err) dFrr1 += 1.0;
        }
        else
        {

            if ( err >= min_err )
            {
                dFar1 += 1.0;
            }
        }


    }

    dFrr1 /= m_nTestFaces;
    dFar1 /= m_nTestNonFaces;
	afrr = dFrr1;

	DbgMsg( "ro = " + TypeToStr( min_err ) + "\t frr = " + TypeToStr( dFrr1 ) +
		",\t far = " + TypeToStr( dFar1 ) + ";\n");

	//TWclErrInfo err_info;
	err_info.m_far = dFar1;
	err_info.m_frr = dFrr1;
	err_info.m_thr = min_err;

	m_trainData.push_back(err_info);

	return dFar1;
}
int SampleCompare(void * Item1, void * Item2)
{
	TCSSample* s1 = (TCSSample*)Item1;
	TCSSample* s2 = (TCSSample*)Item2;
	if (s1->GetRating() > s2->GetRating())
		return 1;
	else if (s1->GetRating() == s2->GetRating())
		return 0;
	else
		return -1;
}
void   TCSAdaBoost::SaveFRRSamples(int stage)
{
	if (stage < 0)
		return;
	std::string strPath = this->m_strPath;
	strPath += "\\frr\\";

	if (!LFDirExist(strPath.c_str()))
		LFCreateDir(strPath.c_str());

	char buf[32];
	sprintf(buf, "stage%d", stage);
	strPath += buf;
	if (!LFDirExist(strPath.c_str()))
		LFCreateDir(strPath.c_str());

	//forming an array of positive examples for each settable rating received strong classifier
	TLFObjectList positiveSamples;
	for (int i = 0; i < this->m_TrainingSamples.GetCount(); i++)
	{
		TCSSample* s = (TCSSample*)m_TrainingSamples.Get(i);
		if (s->GetFlag() == 1)
		{
			double err = 0;
			awpRect r; r.left =0; r.top = 0;
			r.right = s->GetImage()->sSizeX;
			r.bottom = s->GetImage()->sSizeY;

			this->m_ResultClass.Setup(r, m_widthBase);
			this->m_ResultClass.Classify(s, err);
			s->SetRating(err);
			
			positiveSamples.Add (new TCSSample(s));
		}
	}
	// sort the list
	positiveSamples.Sort(SampleCompare);
	// Save 1% of the list to disk
	std::string strFrrFileName = LFMakeFileName(strPath, "frr", ".txt");
	FILE* F = fopen(strFrrFileName.c_str(), "w+t");
	if (F == NULL)
		return;
	fprintf (F, "stage threshold = %f\n", this->m_ResultClass.GetThreshold());
	int numToSave = positiveSamples.GetCount() / 100;
	for (int i = 0; i < numToSave; i++)
	{
		TCSSample* s = (TCSSample*)positiveSamples.Get(i);
		std::string strFileName = LFMakeFileName(strPath, TypeToStr(i), ".jpg");
		double r = s->GetRating();
		fprintf (F, "%s\t %f\n", strFileName.c_str(),  r);
		awpSaveImage(strFileName.c_str(), s->GetImage());
	}
	fclose(F);
}

void   TCSAdaBoost::SaveNegativeSamples(const char* lpPath)
{
	string path = lpPath;
	int total = 0;
	LFCreateDir(lpPath);
	for (int i = 0; i < this->m_TrainingSamples.GetCount(); i++)
	{
		TCSSample* s = (TCSSample*)m_TrainingSamples.Get(i);
		if (s->GetFlag() == 0)
		{
			string filename = path;
			filename += TypeToStr(total);
			filename += ".awp";
			total++;
			s->SaveToFile(filename.c_str());
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
TCSAdaBoostSign::TCSAdaBoostSign()
{
    m_widthBase = 24;
    m_heightBase = 24;
    m_maxFeatures = 50;
    m_strBase1 = "";
    m_strBase2 = "";
    m_numSamplesBase1  = 0;
    m_numSamplesBase1  = 0;

	m_flog.open("log.txt");
}
bool TCSAdaBoostSign::Load(const char* lpConfigName)
{
   if (lpConfigName == NULL)
		return false;

   TiXmlDocument doc(lpConfigName);
   if (!doc.LoadFile())
    return false;

   TiXmlHandle hDoc(&doc);
   TiXmlElement* pElem = NULL;
   pElem = hDoc.FirstChildElement().Element();

   if (!pElem)
      return false;
   const char* name = pElem->Value();
   if (strcmp(name, "csboostsign") != 0)
      return false;
   return Load( pElem);
}

bool    TCSAdaBoostSign::Load (TiXmlElement* node)
{
   if (node == NULL)
    return false;
	m_TrainingSamples.Clear();
	m_TestingSamples.Clear();
    TiXmlElement* pElem = NULL;
    pElem = node->FirstChildElement();
    //Загрузка образцов для обучения
	DbgMsg( "Loading training sets...\n" );
	m_strBase2 = pElem->Attribute("database2");
	if (!LoadSamples( -1, m_strBase2 ))
        return false;
    m_numSamplesBase2 = m_TrainingSamples.GetCount();
    m_numSamplesTestBase2 = m_TestingSamples.GetCount();
    std::string m_strBase1 = pElem->Attribute("database1");
	if (!LoadSamples( 1,  m_strBase1))
        return false;
	m_numSamplesBase1 = (int)m_TrainingSamples.GetCount( ) - m_numSamplesBase2;
	m_numSamplesTestBase1 = (int)m_TestingSamples.GetCount( ) - m_numSamplesTestBase2;

    //
    double v;
	m_widthBase = 24;
	m_heightBase = 24;
    pElem->Attribute("w", &v);
    m_widthBase = (int )v;
    pElem->Attribute("h", &v);
    m_heightBase = (int )v;

	pElem = pElem->NextSiblingElement();
    double ni = 5;
    pElem->Attribute("maxi", &ni);
	m_maxFeatures = (int)ni;


    DbgMsg( "Samples to training for class 1 " + TypeToStr(m_numSamplesBase1) +";\n" );
    DbgMsg( "Samples to training for class -1 " + TypeToStr(m_numSamplesBase2) +";\n" );
    DbgMsg( "Samples to testing for class 1 " + TypeToStr(m_numSamplesTestBase1) +";\n" );
    DbgMsg( "Samples to testing for class -1 " + TypeToStr(m_numSamplesTestBase2) +";\n" );

    // инициализация исходного массива признаков
	InitFeatures( );

	return true;
}

void TCSAdaBoostSign::SetLogName(char* lpLogName)
{
}

void TCSAdaBoostSign::SetLog1Name(char* lpLog1Name)
{
}


bool TCSAdaBoostSign::Boost()
{
    // Начало накачки.
    double wf   = 0.5/m_numSamplesBase1;
	double wnf = 0.5 / m_numSamplesBase2;

    DbgMsg("\nBoosting started.\n");

    // Установка начальных весов для обучающего набора m_TrainingSet
    for ( int i = 0; i < m_TrainingSamples.GetCount(); i++)
    {
        TCSSample* s = (TCSSample*)m_TrainingSamples.Get(i);
        if (s->GetFlag() == 1)
            s->SetWeight(wf);
        else if (s->GetFlag() == -1)
            s->SetWeight(wnf);
        else
            continue;
    }

    ProgressMeter progress;
    progress.Init( 30, (int)m_TrainingSamples.GetCount( ) );

	m_ResultClass.Clear();
	//m_trainData.clear();

    if ( m_Features.GetCount( ) == 0 ) return false;

    //Main loop;
    for ( int i = 0; i < m_maxFeatures ; ++i )
    {

        DbgMsg( "\nUpdating: " );
        progress.Init( 30, m_Features.GetCount( ) );
	    int count = 0;

//Update
#pragma omp parallel for reduction(+: count)
     	for ( int w = 0; w < m_Features.GetCount(); ++w )
        {
       	    ++count;
            TCSWeakSignTrainig* wcinfo = (TCSWeakSignTrainig*)m_Features.Get(w);
            wcinfo->SetEpsilon(0);

            //Train section;
            wcinfo->StartTraining();
            for ( int i = 0; i < m_TrainingSamples.GetCount(); i++)
            {
               TCSSample* s = (TCSSample*)m_TrainingSamples.Get(i);
   			   wcinfo->AddSample(s, s->GetFlag(), s->GetWeight());
            }
            wcinfo->Train();

            //Calc error;
            for ( int i = 0; i < m_TrainingSamples.GetCount(); i++)
            {
                TCSSample* s = (TCSSample*)m_TrainingSamples.Get(i);

                int res = wcinfo->Classify( s );

                if ( s->GetFlag() != res )
                {
                    wcinfo->SetEpsilon(wcinfo->GetEpsilon() + s->GetWeight());
                }
            }

            if ( progress.Update( ) )
	    	    DbgMsg(">");
        }

        DbgMsg( "\n" );
		printf( "Count stage: %d \n", count );
		// выбор лучшего признака, по минимальной ошибке классификации на обучающем наборе.
		int bestWc = 0;
		for ( int w = 0; w < m_Features.GetCount(); ++w )
		{
            TCSWeakSignTrainig* best       = (TCSWeakSignTrainig*)m_Features.Get(bestWc);
            TCSWeakSignTrainig* current    = (TCSWeakSignTrainig*)m_Features.Get(w);
            if ( best->GetEpsilon() > current->GetEpsilon()  )
                bestWc = w;
		}
        TCSWeakSignTrainig* pWc       = (TCSWeakSignTrainig*)m_Features.Get(bestWc);
		pWc->SetWeight((double)pWc->GetEpsilon());

		// если ошибка лучшего классификатора превысила критический уровень 50%
		// прекращаем процесс накачки.
		if ( pWc->Weight() > 0.49 ) break;


        //Update wc weight;
        double beta = pWc->Weight()/ (1 - pWc->Weight());
        double min_eps = pWc->Weight();
        pWc->SetWeight (log(1.0/beta));

        //Добавим "слабый" классификатор в "комитет".
        m_ResultClass.AddWeak(pWc);
		
        // распечатка атрибутов классификатора
        DbgMsg( "\nRound num = " + TypeToStr(i + 1) +
            ", min_eps = " + TypeToStr(min_eps) +
            ", beta = " + TypeToStr( beta ) +
		", alpha = "+ TypeToStr(pWc->Weight())+";\n" );

        this->Statistics();
        // для каждого образца обновим веса
        double wt = 0;
        for ( int i = 0; i < m_TrainingSamples.GetCount(); i++ )
        {
            TCSSample* s = (TCSSample*)m_TrainingSamples.Get(i);

            int res = 0;
            double err = 0;
            res = pWc->Classify( s );

            if ( res == s->GetFlag() )
                s->SetWeight(s->GetWeight()*beta);
            wt += s->GetWeight();
        }

        //Normalize;
        wt = 1.0/wt;

        for ( int i = 0; i < m_TrainingSamples.GetCount(); i++ )
        {
            TCSSample* s = (TCSSample*)m_TrainingSamples.Get(i);
            s->SetWeight(s->GetWeight()*wt);
        }

        //Удалим найденный признак из списка признаков
        m_Features.Delete(bestWc);

    }
	//SaveROC();
    return true;
}
//
TCSStrongSign* TCSAdaBoostSign::GetResult()
{
  return &m_ResultClass;
}

void    TCSAdaBoostSign::InitFeatures()
{
    m_Features.Clear();

   int sx = 1;
   int sy = 1;
   int w = 3;
   int h = 3;
   for (h = m_heightBase -3; h >= 3; h-=3)
   {
	   for (w = m_widthBase ; w >= 3; w-=3)
	   {
		   for (sy = 1; sy < m_heightBase - h - 1; sy++)
		   {
			   for (sx = 1; sx < m_widthBase - w - 1; sx++)
			   {
				   TCSSensor Sensor(sx, sy, w / 3, h / 3);
				   TCSWeakSignTrainig* wt = new TCSWeakSignTrainig(Sensor);
				   m_Features.Add(wt);
			   }
		   }
	   }
   }
}

bool    TCSAdaBoostSign::LoadSamples(int flag, std::string const& path)
{
	DbgMsg( "Loading samples from " + path + "... " );

	int count = 0;
	srand( (unsigned)time( NULL ) );
	int RANGE_MIN = 0;
	int RANGE_MAX = 3;

#ifdef WIN32
    intptr_t handle = 0;
    _finddata_t filesInfo;
    if ( (handle = _findfirst( (char*)((path+"*.jpg").c_str()), &filesInfo)) != -1 )
    {
		do
        {
	       string name = path + filesInfo.name;
		
            TCSSample* pSample = new TCSSample();
            pSample->LoadFromFile((char*)name.c_str());

    //        if (pSample->Process())
            {
                pSample->SetFlag(flag);
				int r = (int)(((double) rand() / (double) RAND_MAX) * RANGE_MAX + RANGE_MIN);

                //int  r = rand(0);
                if (r > 0)
                    m_TrainingSamples.Add(pSample);
                else
                    m_TestingSamples.Add(pSample);

                ++count;
            }
//            else
//printf( "Can't load image %s.\n", name.c_str( ) );
        }while(!_findnext( handle, &filesInfo ));


    }
    _findclose( handle );

#else

//    struct dirent** namelist;
//    if(( count = scandir( path.c_str(), &namelist, jpg_filter, alphasort )) < 0 )
//            return 0;

//    for( int i=0; i<count; i++ )
//    {
//            string name = path + namelist[i]->d_name;

//            TCSSample* pSample = new TCSSample();
//            pSample->LoadImage((char*)name.c_str());


//                pSample->SetFlag(flag);
//				int r = (((double) rand() / (double) RAND_MAX) * RANGE_MAX + RANGE_MIN);

//                //int  r = rand(0);
//                if (r > 0)
//                    m_TrainingSamples.Add(pSample);
//                else
//                    m_TestingSamples.Add(pSample);

//    }

#endif

    DbgMsg( TypeToStr( count ) + " ok\n" );
    return count > 0;
}
void    TCSAdaBoostSign::DbgMsg( std::string const& msg)
{
    std::cout<< msg;  
#ifdef WIN32
	m_flog << msg;
	m_flog.flush();
#endif
}

void    TCSAdaBoostSign::Statistics()
{
    double errt[2] = {0,0};
	double err1[2] = { 0, 0 };
	double err2[2] = { 0, 0 };
	double erru[2] = { 0, 0 };
    for (  int i = 0; i < m_TrainingSamples.GetCount(); i++ )
    {

        TCSSample* s = (TCSSample*)m_TrainingSamples.Get(i);

        double err = 0;
		//double factor = (double)(s->GetImage()->sSizeX)/24.0;
		awpRect r; r.left =0; r.top = 0;
		r.right = s->GetImage()->sSizeX;
		r.bottom = s->GetImage()->sSizeY;
		m_ResultClass.Setup(r, m_widthBase);

        int res = m_ResultClass.Classify( s->GetIntegralImage(), err );
        if (res != s->GetFlag())
        {
              errt[0]++;
              if (res == 0)
                erru[0]++;
              if (res == 1)
                err2[0]++;
              if (res == -1)
                err1[0]++;
        }
    }
    errt[0] /= m_TrainingSamples.GetCount();
    errt[0] *= 100;

    err1[0] /= (m_numSamplesBase1);
    err1[0] *= 100;

    err2[0] /= (m_numSamplesBase2);
    err2[0] *= 100;

    erru[0] /= m_TrainingSamples.GetCount();
    erru[0] *= 100;
    for (  int i = 0; i < m_TestingSamples.GetCount(); i++ )
    {

        TCSSample* s = (TCSSample*)m_TestingSamples.Get(i);

        double err = 0;
		//double factor = (double)(s->GetImage()->sSizeX)/24.0;
		awpRect r; r.left =0; r.top = 0;
		r.right = s->GetImage()->sSizeX;
		r.bottom = s->GetImage()->sSizeY;
		m_ResultClass.Setup(r, m_widthBase);

        int res = m_ResultClass.Classify( s->GetIntegralImage(), err );
        if (res != s->GetFlag())
        {
              errt[1]++;
              if (res == 0)
                erru[1]++;
              if (res == 1)
                err2[1]++;
              if (res == -1)
                err1[1]++;
        }
    }
    if (m_TestingSamples.GetCount() > 0)
    {
    errt[1] /= m_TestingSamples.GetCount();
    errt[1] *= 100;

    err1[1] /= (m_numSamplesTestBase1);
    err1[1] *= 100;
    err2[1] /= (m_numSamplesTestBase2);
    err2[1] *= 100;


    erru[1] /= m_TestingSamples.GetCount();
    erru[1] *= 100;
    }

    DbgMsg( "errt = " + TypeToStr( errt[0] ) + " % " + TypeToStr( errt[1] ) + "%;\n" );
    DbgMsg( "err1 = " + TypeToStr( err1[0] ) + " % " + TypeToStr( err1[1] ) + "%;\n" );
    DbgMsg( "err2 = " + TypeToStr( err2[0] ) + " % " + TypeToStr( err2[1] ) + "%;\n"  );
    DbgMsg( "erru = " + TypeToStr( erru[0] ) + " % " + TypeToStr( erru[1] ) + "%;\n" );
}


struct roc_entry
{
	double far1;
	double frr1;
	double far2;
	double frr2;
};
void TCSAdaBoostSign::SaveROC(char*lpName)
{
 //
	double* fSourceClass1 = NULL;
	double* fSourceClass2 = NULL;
	roc_entry roc[100];
	memset(roc,0, sizeof(roc));
	fSourceClass1 = (double*)malloc(m_numSamplesTestBase1*sizeof(double));
	fSourceClass2 = (double*)malloc(m_numSamplesTestBase2*sizeof(double));
	memset(fSourceClass1, 0, m_numSamplesTestBase1*sizeof(double));
	memset(fSourceClass2, 0, m_numSamplesTestBase2*sizeof(double));
	int nc1 =0;
	int nc2 =0;

	for (int i = 0; i < m_TestingSamples.GetCount(); i++)
	{
		TCSSample* s = (TCSSample*)m_TestingSamples.Get(i);

		awpRect r; r.left =0; r.top = 0;
		r.right = s->GetImage()->sSizeX;
		r.bottom = s->GetImage()->sSizeY;
		m_ResultClass.Setup(r, m_widthBase);
		double err = 0;
		m_ResultClass.Classify( s->GetIntegralImage(), err );
		if (s->GetFlag() == 1)
		{
			fSourceClass1[nc1] = err;
			nc1++;
		}
		else
		{
			fSourceClass2[nc2] = err;
			nc2++;
		}
	}

	double tau = m_ResultClass.GetSumWeakWeight();

	DbgMsg( TypeToStr( tau ) + "\n" );

	double delta = tau / 50.f;
	double stau = -tau;
	int c = 0;
	while(stau < tau)
	{
		for (int i=0; i< nc1; i++)
		{
			if (fSourceClass1[i] < stau)
				roc[c].frr1++;
			if (fSourceClass1[i] < stau)
				roc[c].far2++;
		}
		for (int i=0; i< nc2; i++)
		{
			if (fSourceClass2[i] > stau)
				roc[c].far1++;
			if (fSourceClass2[i] > stau)
				roc[c].frr2++;
		}

		stau += delta;
		c++;
	}
	DbgMsg( TypeToStr( c ) + "\n" );
	for (int i = 0; i < 100; i++)
	{
		roc[i].far1 = 100.f*roc[i].far1/nc2;
		roc[i].frr1 = 100.f*roc[i].frr1/nc1;
		roc[i].far2 = 100.f*roc[i].far2/nc1;
		roc[i].frr2 = 100.f*roc[i].frr2/nc2;
	}
//	std::ostream*       f;
//	f = new ofstream("roc.txt");
//	for (int i = 0; i < 100; i++)
//		(*f) << roc[i].far1 << "\t" << roc[i].frr1<< "\t" << roc[i].far2<< "\t" << roc[i].frr2 <<"\n";
//	delete f;
	FILE* f = fopen (lpName, "w+t");
	for (int i = 0; i < 100; i++)
	{
//		fprintf(f, "%f\n", roc[i].far1);
		fprintf(f, "%f\t%f\t%f\t%f\n", roc[i].far1, roc[i].frr1, roc[i].far2,roc[i].frr2);
	}
	//	(*f) << roc[i].far1 << "\t" << roc[i].frr1<< "\t" << roc[i].far2<< "\t" << roc[i].frr2 <<"\n";
	fclose(f);
	free(fSourceClass1);
	free(fSourceClass2);
}

TIEFSAdaBoost::TIEFSAdaBoost()
{
	m_numIFS = 0;
	m_numEFS = 0;
	m_strSourceFile = "";
	m_pLog = NULL;
}

bool TIEFSAdaBoost::Load(const char* lpFileName)
{
	/*попытка открыть файл конфигурации*/
	TiXmlDocument doc(lpFileName);
	if (!doc.LoadFile())
		return false;
	TiXmlElement* e = doc.FirstChildElement("IEFSBoost");
	if (e == NULL)
		return false;
	m_strSourceFile = e->Attribute("file");
	string str = e->Attribute("log");

	if (m_pLog == NULL)
		m_pLog = new ofstream(str.c_str());

	return LoadSamples();
}
// распечатка сообщения на экране и в лог файле
void TIEFSAdaBoost::DbgMsg(std::string const& msg)
{
	(*m_pLog) << msg;
	cout << msg;
	flush(*m_pLog);
}

bool TIEFSAdaBoost::Boost()
{
	// Начало накачки.
	double wf  = 0.5 / this->m_numIFS;
	double wnf = 0.5 / this->m_numEFS;

	DbgMsg("\nBoosting started.\n");

	// Установка начальных весов для обучающего набора m_TrainingSet
	for (int i = 0; i < m_TrainingSamples.GetCount(); i++)
	{
		TIEFSSample* s = (TIEFSSample*)m_TrainingSamples.Get(i);
		if (s->GetFlag() == 1)
			s->SetWeight(wf);
		else if (s->GetFlag() == 0)
			s->SetWeight(wnf);
		else
			continue;
	}

	ProgressMeter progress;
	progress.Init(30, (int)m_TrainingSamples.GetCount());
	// result class 
	m_ResultClass.Clear();

	if (m_Features.GetCount() == 0)
		return false;

	//Main loop;
	//for (int i = 0; i < this->m_Features.GetCount(); ++i)
	int i = 0;
	while (this->m_Features.GetCount() > 0)
	{
		i++;
		DbgMsg("\nUpdating: ");
		progress.Init(30, m_Features.GetCount());
		int count = 0;

		//Update
#pragma omp parallel for reduction(+: count)
		for (int w = 1; w < m_Features.GetCount(); ++w)
		{
			++count;
			TIEFSWeak2* wcinfo = (TIEFSWeak2*)m_Features.Get(w);
			if (!wcinfo->IsFree())
				continue;
			wcinfo->SetEpsilon(0);

			//Train section;
			wcinfo->StartTraining();
			for (int i = 0; i < m_TrainingSamples.GetCount(); i++)
			{
				TIEFSSample* s = (TIEFSSample*)m_TrainingSamples.Get(i);
				wcinfo->AddSample(s);
			}
			wcinfo->Train();

			//Calc error;
			for (int i = 0; i < m_TrainingSamples.GetCount(); i++)
			{
				TIEFSSample* s = (TIEFSSample*)m_TrainingSamples.Get(i);

				int res = wcinfo->Classify(s->GetValue(w));

				if (s->GetFlag() != res)
				{
					wcinfo->SetEpsilon(wcinfo->GetEpsilon() + s->GetWeight());
				}
			}

			if (progress.Update())
				DbgMsg(">");
		}

		DbgMsg("\n");
		DbgMsg("Count stage: %d \n" + TypeToStr(count));
		// выбор лучшего признака, по минимальной ошибке классификации на обучающем наборе.
		int bestWc = 1;
		while (!((TIEFSWeak2*)m_Features.Get(bestWc))->IsFree())
			bestWc++;
		for (int w = bestWc +1; w < m_Features.GetCount(); ++w)
		{
			
			TIEFSWeak2* best = (TIEFSWeak2*)m_Features.Get(bestWc);
			TIEFSWeak2* current = (TIEFSWeak2*)m_Features.Get(w);
			if (!current->IsFree())
				continue;
			if (best->GetEpsilon() > current->GetEpsilon())
				bestWc = w;
		}
		TIEFSWeak* pWc = (TIEFSWeak*)m_Features.Get(bestWc);
		pWc->SetWeight(pWc->GetEpsilon());

		// если ошибка лучшего классификатора превысила критический уровень 50%
		// прекращаем процесс накачки.
		if (pWc->GetWeight() >= 0.5)
		{
		
			DbgMsg("\n Min eps = 0.5. Break.\n");
			break;
		}


		//Update wc weight;
		double beta = pWc->GetWeight() / (1 - pWc->GetWeight());
		double min_eps = pWc->GetWeight();
		pWc->SetWeight(log(1.0 / beta)); 

		//Добавим "слабый" классификатор в "комитет".
		//result class  
		m_ResultClass.Add(pWc->Clone());
		m_ResultClass.SetThreshold(m_ResultClass.GetSumWeakWeight() / 2);
		pWc->Lock();
		// распечатка атрибутов классификатора
		DbgMsg("\nRound num = " + TypeToStr(i + 1) +
			", min_eps = " + TypeToStr(min_eps) +
			", beta = " + TypeToStr(beta) +
			", alpha = " + TypeToStr(pWc->GetWeight()) + ";\n");
		// распечатка атрибутов выбранного признака
		DbgMsg("Feature num = " + TypeToStr(pWc->GetIndex()) + "\n" );

		//распечатка ошибки на тестовом наборе.
		PrintStatistics();


		// для каждого образца обновим веса
		double wt = 0;
		for (int i = 0; i < m_TrainingSamples.GetCount(); i++)
		{
			TIEFSSample* s = (TIEFSSample*)m_TrainingSamples.Get(i);

			int res = 0;

			res = pWc->Classify(s->GetValue(pWc->GetIndex()));

			if (res == s->GetFlag())
			{
				s->SetWeight(s->GetWeight()*beta);
			}
			wt += s->GetWeight();
		}

		//Normalize;
		wt = 1.0 / wt;
		double sum = 0;
		for (int i = 0; i < m_TrainingSamples.GetCount(); i++)
		{
			TIEFSSample* s = (TIEFSSample*)m_TrainingSamples.Get(i);
			s->SetWeight(s->GetWeight()*wt);
			sum += s->GetWeight();
		}
		DbgMsg("sum  = " + TypeToStr(sum) + "\n");
		//Удалим найденный признак из списка признаков
		//m_Features.Delete(bestWc);

	}

    this->m_ResultClass.SaveXML("IEFSclass.xml");
	if (this->m_ResultClass.LoadXML("IEFSclass.xml"))
	{
		DbgMsg("Work done.\n\n");
		this->PrintStatistics();
		DbgMsg("Bye!\n");
		return true;
	}
	else
		return false;
}
double	TIEFSAdaBoost::PrintStatistics()
{
	double          dFar = 0, dFrr = 0;

	for (int i = 0; i < m_TrainingSamples.GetCount(); i++)
	{
		TIEFSSample* s = (TIEFSSample*)m_TrainingSamples.Get(i);
		double err = m_ResultClass.Raiting(s);
		if (s->GetFlag() == 1)
		{
			if (err < m_ResultClass.GetThreshold()) dFrr += 1.0;
		}
		else
		{
			if (err >= m_ResultClass.GetThreshold())
			{
				dFar += 1.0;
			}
		}
	}

	dFrr /= this->m_numIFS;
	dFar /= this->m_numEFS;

	DbgMsg("\nFAR = " + TypeToStr(dFar) +
		", FRR = " + TypeToStr(dFrr) +
		", threshold = " + TypeToStr(m_ResultClass.GetThreshold()) + ";\n");

	return 0;
}

bool TIEFSAdaBoost::InitFeatures(int num)
{
	if (num <= 0)
	{
		DbgMsg("Invalid num features: num = " +TypeToStr (num) + "\n");
		return false;
	}
	for (int i = 0; i < num; i++)
	{
		TIEFSWeak2* w = new TIEFSWeak2(i);
		this->m_Features.Add(w);
	}
	DbgMsg(TypeToStr(num) + "features created.\n");
	return true;
}
bool TIEFSAdaBoost::LoadSamples()
{
	DbgMsg("Loading samples...\n");
	awpImage* img = NULL;
	if (awpLoadImage(this->m_strSourceFile.c_str(), &img) != AWP_OK)
	{
		DbgMsg("Cannot load featues from " + m_strSourceFile + "\n");
		return false;
	}
	int num = img->sSizeX - 1;
	double* diff = (double*)malloc(num*sizeof(double));
	if (!this->InitFeatures(num))
	{
		awpReleaseImage(&img);
		free(diff);
		return false;
	}
	double* d = (double*)img->pPixels;
	int ifs_count = 0;
	int efs_count = 0;
	for (int i = 0; i < img->sSizeY; i++)
	{
		int id1 = (int)d[i*img->sSizeX];
		for (int j = i + 1; j < img->sSizeY; j++)
		{
			int id2 = (int)d[j*img->sSizeX];
			int flag = id1 == id2 ? 1 : 0;
			if (!flag && efs_count > 730000)
				continue;
			if (flag)
				ifs_count++;
			else
				efs_count++;

			for (int k = 1; k <= num; k++)
			{
				diff[k-1] = fabs(d[i*img->sSizeX + k] - d[j*img->sSizeX + k]);
			}
			TIEFSSample* s = new TIEFSSample(diff, num, flag);
			this->m_TrainingSamples.Add(s);
		}
	}
	this->m_numIFS = ifs_count;
	this->m_numEFS = efs_count;

	awpReleaseImage(&img);
	free(diff);
	DbgMsg("IFS samples loaded = " + TypeToStr(m_numIFS) + "\n");
	DbgMsg("EFS samples loaded = " + TypeToStr(m_numEFS) + "\n");
	return true;
}
