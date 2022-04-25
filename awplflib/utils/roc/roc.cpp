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
// Copyright (C) 2004-2017, Alt-Soft.net, all rights reserved.
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
// In no event shall the Alt-Soft.net or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//      Locate Framework  (LF) Computer Vision Library.
//		File: roc.cpp
//		Purpose: TLFEngine roc test application 
//
//      CopyRight 2004-2019 (c) NN-Videolab
//M*/
//


#include "_LF.h"
using namespace std;

void Usage()
{
	printf("sizeof CSFeature %d\n", sizeof(TCSSensor));
	printf("roc config.xml\n");
	TCSSensor* s = new TCSSensor[10000000];
	getchar();

}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		Usage();
		return 1;
	}

	TiXmlDocument doc;
	if (!doc.LoadFile(argv[1]))
	{
		printf("Cannot load file: %s \n", argv[1]);
		return 1;
	}

	TiXmlHandle hDoc(&doc);
	TiXmlElement* pElem = NULL;
	pElem = hDoc.FirstChildElement().Element();
	if (!pElem)
	{
		printf("error: invalid configuration file.\n");
		return 1;
	}

	const char* name = pElem->Value();
	if (strcmp(name, "roc") != 0)
	{
		printf("error: invalid configuration file.\n");
		return 1;
	}

	
	string strPath   = pElem->Attribute("database");
	string strEngine = pElem->Attribute("engine");
	string strOutput = pElem->Attribute("output");
	double overlap = 0;
	pElem->QueryDoubleAttribute("overlap", &overlap);
	int mode = 0;
	pElem->QueryIntAttribute("mode", &mode);
	int stage = 0;
	pElem->QueryIntAttribute("stage",&stage);
	int resize = 1;
	//pElem->QueryIntAttribute("resize", &resize);
	int resize_to = 0;
	//pElem->QueryIntAttribute("resize_to", &resize_to);
	int save_far = 0;
	pElem->QueryIntAttribute("save_far", &save_far);
	int save_frr = 0;
	pElem->QueryIntAttribute("save_frr", &save_frr);

	TLFDBLabeledImages db;
	if (!db.InitDB(strPath.c_str()))
	{
		printf("error: cannot init database.\n");
		return 1;
	}
	if (db.GetItemsCount() == 0)
	{
		printf("error: database is empty.\n");
		return 1;
	}
	else
	{
		printf("images count = %i\n", db.GetImagesCount());
		printf("items count  = %i\n", db.GetItemsCount());
	}

	TLFDetectEngine engine;

	if (!engine.Load(strEngine.c_str()))
	{
		printf("error: cannot load engine %s .\n", strEngine.c_str());
		return 1;
	}
	else
	{
		printf("engine: %s\n", strEngine.c_str());
		printf("stages = %i\n", engine.GetDetector()->GetStagesCount());
	}

	engine.SetNeedCluster(true);
	engine.SetResize(false);
	switch (mode)
	{
		case 1:
			{

				double Far, Frr , t, e;
				db.GetFarFrr(engine, Far, Frr, e, t, overlap);
				printf("++++++++ Far/Frr calculation. +++++++++++++\n");
				//printf("Database: name =%s\n", db.GetPath());
				printf("Database: images count = %i\n", db.GetImagesCount());
				printf("Database: items count  = %i\n", db.GetItemsCount());
				printf("Engine: name = %s\n", strEngine.c_str());
				printf("Engine: stages = %i\n", engine.GetDetector()->GetStagesCount());
				printf("Settings: Overlap = %f\n", overlap);
				printf("Result: Far = %f\n", (float)Far);
				printf("Result: Frr = %f\n", (float)Frr);
				printf("Result: Fpf = %e\n", (float)e);
				printf("Result: tau  = %f\n", (float)t);
				FILE* f = NULL;
				f = fopen( strOutput.c_str(), "w+t");
				if (f != NULL)
				{
					fprintf(f,"++++++++ Far/Frr calculation. +++++++++++++\n");
					fprintf(f,"Database: images count = %i\n", db.GetImagesCount());
					fprintf(f,"Database: items count  = %i\n", db.GetItemsCount());
					fprintf(f,"Engine: name = %s\n", strEngine.c_str());
					fprintf(f,"Engine: stages = %i\n", engine.GetDetector()->GetStagesCount());
					fprintf(f,"Settings: Overlap = %f\n", overlap);
					fprintf(f,"Result: Far = %f\n", (float)Far);
					fprintf(f,"Result: Frr = %f\n", (float)Frr);
					fprintf(f,"Result: Fpf = %e\n", (float)e);
					fprintf(f,"Result: tau  = %f\n", (float)t);
					fclose(f);
				}
			}
		break;
		case 2:
		{
			printf("roc curve for a particular strong classifier\n");
			printf("Overlap = %f\n", overlap);
			printf("Stage = %i\n", stage);
			TLFHistogramm far_hst(100, 0, 1);
			TLFHistogramm frr_hst(100, 0, 1);
			db.GetFarFrrHST(engine, far_hst, frr_hst, stage, false, overlap);

			// save
			FILE* f = NULL;
			f = fopen(strOutput.c_str(), "w+t");
			if (f != NULL)
			{
				for (int i = 0; i < far_hst.GetNumBins(); i++)
				{
					fprintf(f, "%f\t%f\n", far_hst.GetSumToBinBackward(i), frr_hst.GetSumToBinForward(i));
				}
				fclose(f);
			}

		}
		break;
		case 3:
		{
			printf("roc curve for detection of the %i classifications\n", stage);
			printf("Overlap = %f\n", overlap);
			printf("Stage = %i\n", stage);

			TLFHistogramm far_hst(100, 0, 1);
			TLFHistogramm frr_hst(100, 0, 1);
			db.GetFarFrrHST(engine, far_hst, frr_hst, stage, true, overlap);

			// save
			FILE* f = NULL;
			f = fopen( strOutput.c_str(), "w+t");
			if (f != NULL)
			{
				for (int i = 0; i < far_hst.GetNumBins(); i++)
				{
					fprintf(f, "%f\t%f\n", far_hst.GetSumToBinBackward(i), frr_hst.GetSumToBinForward(i));
				}
				fclose(f);
			}

		}
		break;
		case 4:
		{
			printf("far for the strong classifier\n");
			printf("Overlap = %f\n", overlap);
			printf("Stage = %i\n", stage);

			TLFHistogramm far_hst(100, 0, 1);
			db.GetFrrHST(engine, far_hst, stage, false, overlap);
			// save
			FILE* f = NULL;
			f = fopen(strOutput.c_str(), "w+t");
			if (f != NULL)
			{
				for (int i = 0; i < far_hst.GetNumBins(); i++)
				{
					fprintf(f, "%f\t%f\n",far_hst.GetThrBin(i),  far_hst.GetSumToBinBackward(i));
				}
				fclose(f);
			}
		}
		break;
		case 5:
		{
			printf("far to the cascade of %i strong  classifiers", stage);
			printf("Overlap = %f\n", overlap);
			printf("Stage = %i\n", stage);

			TLFHistogramm far_hst(100, 0, 1);
			db.GetFrrHST(engine, far_hst, stage, true, overlap);
			// save
			FILE* f = NULL;
			f = fopen(strOutput.c_str(), "w+t");
			if (f != NULL)
			{
				for (int i = 0; i < far_hst.GetNumBins(); i++)
				{
					fprintf(f, "%f\t%f\n", far_hst.GetThrBin(i), far_hst.GetSumToBinBackward(i));
				}
				fclose(f);
			}
		}
		break;
		case 6:
		{
			printf("frr for the strong classifier\n");
			printf("Overlap = %f\n", overlap);
			printf("Stage = %i\n", stage);

			TLFHistogramm frr_hst(100, 0, 1);
			db.GetFrrHST(engine, frr_hst, stage, false, overlap);
			// save
			FILE* f = NULL;
			f = fopen(strOutput.c_str(), "w+t");
			if (f != NULL)
			{
				for (int i = 0; i < frr_hst.GetNumBins(); i++)
				{
					fprintf(f, "%f\t%f\n", frr_hst.GetThrBin(i), frr_hst.GetSumToBinForward(i));
				}
				fclose(f);
			}
		}
		break;
		case 7:
		{
			printf("frr to the cascade of %i strong  classifiers", stage);
			printf("Overlap = %f\n", overlap);
			printf("Stage = %i\n", stage);

			TLFHistogramm frr_hst(100, 0, 1);
			db.GetFrrHST(engine, frr_hst, stage, true, overlap);
			// save
			FILE* f = NULL;
			f = fopen(strOutput.c_str(), "w+t");
			if (f != NULL)
			{
				for (int i = 0; i < frr_hst.GetNumBins(); i++)
				{
					fprintf(f, "%f\t%f\n", frr_hst.GetThrBin(i), frr_hst.GetSumToBinForward(i));
				}
				fclose(f);
			}
		}
		break;
		default:
			printf("Unknown mode. See readme.txt to set mode.\n");
	}
	return 0;
}
 