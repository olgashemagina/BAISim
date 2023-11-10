// Player.cpp: определяет точку входа для консольного приложения.
//

#define _USE_MATH_DEFINES // для C++
#include <cmath>

#include "stdafx.h"


//#include "awpcvvideo.cpp"
#include "opencv\cv.h"
#include "opencv\highgui.h"
#include "opencv2\opencv.hpp"
#include "opencv2\core\utility.hpp"
#include "opencv2\core\core.hpp"
#include "opencv2\imgproc\imgproc.hpp"
#include "opencv2\imgproc.hpp"
#include "opencv2\highgui\highgui.hpp"

using namespace cv;
using namespace std;

extern "C"
{
#pragma comment(lib, "awpipl2.lib")
#pragma comment(lib, "opencv_world300.lib")
}


AWPRESULT rotateRect(awpImage* pImage, awpRect* rect, AWPBYTE bChan, AWPDOUBLE dValue, int angle) {

	AWPRESULT res = AWP_OK;
	AWPWORD sx, sy, w, h, y, x, ws, x_c, y_c, x_new, y_new, h_new, w_new, sy_new, sx_new;
	AWPBYTE* data;
	AWPBYTE  value;

	sx = rect->left;
	sy = rect->top;
	w = rect->right - rect->left;
	ws = pImage->bChannels * pImage->sSizeX;
	h = rect->bottom - rect->top;
	x_c = (rect->left + rect->right) / 2;
	y_c = (rect->top + rect->bottom) / 2;
	w_new = (w - x_c) * cos(angle * M_PI / 180) - (h - y_c) * sin(angle * M_PI / 180) + x_c;
	h_new = (w - x_c) * sin(angle * M_PI / 180) + (h - y_c) * cos(angle * M_PI / 180) + y_c;
	sx_new = (sx - x_c) * cos(angle * M_PI / 180) - (sy - y_c) * sin(angle * M_PI / 180) + x_c;
	sy_new = (sx - x_c) * sin(angle * M_PI / 180) + (sy - y_c) * cos(angle * M_PI / 180) + y_c;
	data = (AWPBYTE*)pImage->pPixels;
	value = (AWPBYTE)dValue;
	for (y = sy; y < sy + h; y++)
	{
		for (x = sx; x < sx + w; x++)
			x_new = (x - x_c) * cos(angle * M_PI / 180) - (y - y_c) * sin(angle * M_PI / 180) + x_c;
			y_new = (x - x_c) * sin(angle * M_PI / 180) + (y - y_c) * cos(angle * M_PI / 180) + y_c;
		//		memset(&data[y*pImage->sSizeX + sx], value, w*sizeof(AWPBYTE));
	}

	for (y = sy_new; y < sy_new + h_new; y++)
	{
		for (x = sx_new; x < sx_new + w_new; x++)
			//x_new = (x - x_c) * cos(angle * M_PI / 180) - (y - y_c) * sin(angle * M_PI / 180) + x_c;
			//y_new = (x - x_c) * sin(angle * M_PI / 180) + (y - y_c) * cos(angle * M_PI / 180) + y_c;
			data[y * ws + x * pImage->bChannels + bChan] = value;
		//		memset(&data[y*pImage->sSizeX + sx], value, w*sizeof(AWPBYTE));
	}

CLEANUP:
	return res;
}



int main(int argc, char* argv[])
{
	if (argc < 3)return 2;
	HCVVIDEO video;
	
	TLFDetectEngine detectors[9];

	HRESULT res = S_OK;
	bool pause = false;
	int  num_frames;
	int  frames, delta = 0, nerr = 0, ierr = 0;
	const char _MODULE_[] = "";
	awpRect rect;
	awpImage* img = NULL;
	awpImage* imgRect = NULL;
	static IplImage* imgCV = NULL;
	video = awpcvConnect(argv[1]);//"C:\\1.mp4"
	if (awpcvQueryImagePos(video, &img, 0) != S_OK)return 1;
	rect.bottom = img->sSizeY/6;
	rect.top = 1;//  10;
	rect.left = 1;// ->sSizeX - 1;
	rect.right = img->sSizeX/6;// /*img->sSizeX/12*/100;
	awpcvNumFrames(video, &num_frames);//(int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_COUNT);
		
	const char* paths[9] = {"mybase.xml", "H90.xml", "H60.xml", "H45.xml", "H30.xml", "H0.xml", "H_30.xml", "H_45.xml", "H_60.xml" };

	double angles[] = { 90, 60, 45, 30, 0, -30, -45, -60 };

	for (int i = 0; i < 9; i++) {
		std::string det_path = argv[2];
		det_path = det_path + "/" + paths[i];

		if (!detectors[i].Load(det_path.c_str())) {
			std::cout << "Cant load detector by path " << det_path;
			return 2;
		}
	}

	int weight = 1280;
	int height = 720;
	Size frameSize = Size(weight, height);
	const string filename = "output.mp4";
	int fourcc = VideoWriter::fourcc('M', 'P', '4', 'V');
	double fps = 15.0;

	//out = VideoWriter('output.mp4', int VideoWriter_fourcc(*'MP4V'), )

	/*h90.Load("C:\\Dron\\H90\\dbexport\\H90.xml");
	h60.Load("C:\\Dron\\H60\\dbexport\\H60.xml");
	h45.Load("C:\\Dron\\H45\\dbexport\\H45.xml");
	h30.Load("C:\\Dron\\H30\\dbexport\\H30.xml");
	h0.Load("C:\\Dron\\H0\\dbexport\\H0.xml");
	h_30.Load("C:\\Dron\\H_30\\dbexport\\H_30.xml");
	h_45.Load("C:\\Dron\\H_45\\dbexport\\H_45.xml");
	h_60.Load("C:\\Dron\\H_60\\dbexport\\H_60.xml");*/
	frames = 0;
	for (int j = 0; j < num_frames - 1; j++)
	{

		int c;
		awpcvWaitKey(10, &c);//c = cvWaitKey(10);
		if ((char)c == ' ')
		{
			pause ^= true;
		}
		if ((char)c == 27)
		{
			break;
		}
		//IplImage* frame = NULL;
		//awpcvQueryImage(video, &img);
		//frame = cvQueryFrame(capture);
		if (pause == false) {
			img = NULL;
			res = awpcvQueryImage(video, &img);
			//awpResizeBilinear(img, 640, 360);
			//d.InitDetectors();

			bool result = detectors[0].SetSourceImage(img, true);
			int num = detectors[0].GetItemsCount();

			
		
			frames++;
			if (num > 0)
			{
				for (int i = 0; i < num; i++)
				{
					TLFDetectedItem* di = detectors[0].GetItem(i);

					if (di)
					{
											
						int h_detected[8] = {0, 0, 0, 0, 0, 0, 0, 0};
						double maxScore = 0;

					
						awpRect rectPlatform = di->GetBounds()->GetRect();		
						//awpCopyRect(img, &imgRect, &rectPlatform);

						awpFillRect(img, &rectPlatform, 0, 100);

						double score[8] = {0.0};
						int angle = -1;

						for (int k = 0; k < 8; k++) {

							detectors[k+1].SetSourceImage(img, false);
							h_detected[k] = detectors[k+1].GetDetector()->ClassifyRect(rectPlatform, &score[k], NULL);
							
							if (score[k] > maxScore) {
								maxScore = score[k];
								angle = k;
							}
						}

						if (angle >= 0) {
							awpDrawRectangle(img, &rectPlatform, 1, 255, 5, angles[angle]);
						}

						/*bool result90 = h90.SetSourceImage(img, false);
						int h90detected = h90.GetDetector(0)->ClassifyRect(rectPlatform, &score[8], NULL);
						bool result60 = h60.SetSourceImage(imgRect, false);
						bool result45 = h45.SetSourceImage(imgRect, false);
						bool result0 = h0.SetSourceImage(imgRect, false);
						bool result_60 = h_60.SetSourceImage(imgRect, false);
						bool result_45 = h_45.SetSourceImage(imgRect, false);
						bool result_30 = h_30.SetSourceImage(imgRect, false);
						bool result30 = h30.SetSourceImage(imgRect, false);*/

						/*ILFObjectDetector* score = 0;
						double s = score->takeScore();
						printf("s= %d\n", s);*/

						
						/*TLFDetectedItem* di90 = h90.GetItem(i);
						TLFDetectedItem* di60 = h60.GetItem(i);
						TLFDetectedItem* di45 = h45.GetItem(i);
						TLFDetectedItem* di30 = h30.GetItem(i);
						TLFDetectedItem* di0 = h0.GetItem(i);
						TLFDetectedItem* di_30 = h_30.GetItem(i);
						TLFDetectedItem* di_45 = h_45.GetItem(i);
						TLFDetectedItem* di_60 = h_60.GetItem(i);*/




						/*if (result == true) {

							if (di90) {
								if (result90 == true) {
									awpDrawRectangle(img, &rectPlatform, 1, 255, 5, 90);

								}
							}
							else if (di60) {
								if (result90 == true) {
									awpDrawRectangle(img, &rectPlatform, 1, 255, 5, 60);

								}
							}
							else if (di45) {
								if (result90 == true) {
									awpDrawRectangle(img, &rectPlatform, 1, 255, 5, 45);

								}
							}
							else if (di30) {
								if (result90 == true) {
									awpDrawRectangle(img, &rectPlatform, 1, 255, 5, 30);

								}
							}
							else if (di0) {
								if (result90 == true) {
									awpDrawRectangle(img, &rectPlatform, 1, 255, 5, 0);

								}
							}
							else if (di_30) {
								if (result90 == true) {
									awpDrawRectangle(img, &rectPlatform, 1, 255, 5, -30);

								}
							}
							else if (di_45) {
								if (result90 == true) {
									awpDrawRectangle(img, &rectPlatform, 1, 255, 5, -45);

								}
							}
							else if (di_60) {
								if (result90 == true) {
									awpDrawRectangle(img, &rectPlatform, 1, 255, 5, -60);

								}
							}
						}*/
					}
				}
			}
			awpcvShowImage(_MODULE_, img);
			//bool result = d.FindObjects();
			//d.Init(img, true);
			//int result = d.Detect();
			/*frames++;
			if (result == true) {
				awpFillRect(img, &rect, 1, 255);
			}
			if (result == false) {
				awpFillRect(img, &rect, 2, 255);
				nerr++;
			}
			awpcvShowImage(_MODULE_, img);*/
			//awpResize(img, 1280, 720);
			//string nameFile = to_string(j);
			//awpSaveImage(("C:\\Dron\\saveFrame\\" + nameFile + ".jpg").c_str(), img);
			if (frames % (num_frames / 100) == 0)printf(">");
			awpReleaseImage(&img);
		}
		//cvShowImage(_MODULE_, frame);
	}
	//nerr = 32000;
	//printf("\nNumber of errors is %d", nerr);

	awpcvDisconnect(video);
	return 0;
}

