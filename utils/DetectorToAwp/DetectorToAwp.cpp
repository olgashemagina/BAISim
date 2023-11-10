// DetectorToAwp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "awpipl.h"
#include "LF.h"
#include "LFScanners.h"
#include "LFEngine.h"
#include "LFDetector.h"
#include "LFStrong.h"
#include "LFWeak.h"
#include "LFFeatures.h"


extern "C"
{
	#pragma comment(lib, "awplflib.lib")
	#pragma comment(lib, "tinyxml.lib")
	#pragma comment(lib, "awpipl2.lib")
	#pragma comment(lib, "JPEGLIB.lib")
}

#define AWP_NULL_DETECTOR 0
#define AWP_GMCT_DETECTOR 456
#define AWP_LMCT_DETECTOR 896
#define AWP_GLBP_DETECTOR 765
#define AWP_LLBP_DETECTOR 987

#define AWP_GMST_WIDTH 517
#define AWP_LMST_WIDTH 514

#define AWP_MCT_NUMBINS 512
#define AWP_LBP_NUMBINS 256



int _tmain(int argc, _TCHAR* argv[])
{
	if (argc < 2)
	{
		printf ("Usage: DetectorToAwp filename.xml");
		return 1;
	}
	
	TLFDetectEngine engine;
	if (!engine.Load(argv[1]))
	{
		
		printf("cannot load detector: %s\n", argv[1]);
		return 1;
	}

	TCSDetector* detector = (TCSDetector*)engine.GetDetector();
	int height = 1;
	for (int i = 0; i < detector->GetStagesCount(); i++)
	{
		height++;
		TCSStrong* strong = (TCSStrong*)detector->m_ClsList.Get(i);
		height += strong->WeakCount();
	}
	height++;
	printf ("num lines %d \n", height);

	awpImage* pimg = NULL;
	if (awpCreateImage(&pimg, AWP_GMST_WIDTH, height, 1, AWP_DOUBLE) != AWP_OK)
	{
		printf("Cannot create image.\n");
		return 1;
	}
	double* d= (double*)pimg->pPixels;
	d[0] = AWP_GMCT_DETECTOR;
	d[1] = detector->GetBaseWidth();
	d[2] = detector->GetBaseHeight();
	d[3] = detector->GetStagesCount();
	d+= AWP_GMST_WIDTH;

	for (int i = 0; i < detector->GetStagesCount(); i++)
	{
		printf("stage %d\n", i);
		TCSStrong* strong = (TCSStrong*)detector->m_ClsList.Get(i);
		d[0] = strong->WeakCount();
		d[1] = strong->GetThreshold();
		d += AWP_GMST_WIDTH;
		for (int j = 0; j < strong->WeakCount(); j++)
		{
			TCSWeak* weak= (TCSWeak*)strong->GetWeak(j);
			TCSSensor* s = weak->GetSensor();
			d[0] = s->sxBase();
			d[1] = s->syBase();
			d[2] = s->wBase();
			d[3] = s->hBase();
			d[4] = weak->GetWeight();
			for (int k = 0; k < AWP_MCT_NUMBINS; k++)
			{
				d[5+k] = (double)weak->m_Classifiacator[k];
			}
			d += AWP_GMST_WIDTH;		
		}
	}

	awpSaveImage("out.awp", pimg);
	return 0;
}

