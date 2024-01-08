// Player.cpp: определяет точку входа для консольного приложения.
//

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
#include "LFEngine.h"

using namespace cv;
using namespace std;

extern "C"
{
#pragma comment(lib, "awpipl2.lib")
#pragma comment(lib, "opencv_world300.lib")
}



int main(int argc, char* argv[])
{
	if (argc < 3)return 2;
	HCVVIDEO video;
	TLFDetectEngine d;
	//TLFDetectEngine a;
	HRESULT res = S_OK;
	bool pause = false;
	int  num_frames;
	int  frames, delta = 0, nerr = 0, ierr = 0;
	const char _MODULE_[] = "";
	awpRect rect;
	awpImage* img = NULL;
	static IplImage* imgCV = NULL;
	video = awpcvConnect(argv[1]);//"C:\\1.mp4"
	if (awpcvQueryImagePos(video, &img, 0) != S_OK)return 1;
	rect.bottom = img->sSizeY/6;
	rect.top = 1;//  10;
	rect.left = 1;// ->sSizeX - 1;
	rect.right = img->sSizeX/6;// /*img->sSizeX/12*/100;
	awpcvNumFrames(video, &num_frames);//(int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_COUNT);
	d.Load(argv[2]);//"C:\\Users\\Авас yuri\\Documents\\IPF\\readresearch\\road.xml"
	//a.Load("C:\\data\\Dron\\H90\\dbexport\\H90.xml");
	frames = 0;

	d.GetDetector(0)->SetDescCallback([&](size_t index,
		const auto& bounds,
		const auto& desc) {

			if (index == 0) {
				int a = 0;
			}

			std::cout << index << " Result " << desc.result << " Score " << desc.score << std::endl;
						

	});

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
			awpResizeBilinear(img, 1280, 720);
			//d.InitDetectors();
			bool result = d.SetSourceImage(img, true);
			int num = d.GetItemsCount();
			//bool resultA = a.SetSourceImage(img, true);
			//int numA = a.GetItemsCount();
			//awpPoint p1;
			//p1.X = 300;
			//p1.Y = 20;

			frames++;
			if (num > 0)
			{
				for (int i = 0; i < num; i++)
				{
					TLFDetectedItem* di = d.GetItem(i);
					/*TLFDetectedItem* diA = a.GetItem(i);
					if (diA) {
						if (resultA == true) {
							awpFillRect(img, &rect, 2, 255);

							awpDrawPoint(img, p1, 0, 0, 10);
							
							imgCV = awpConvertToIplImage(img);
							CvFont font;
							cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 0.5, 0.5);
							cvPutText(imgCV, "5", cvPoint(300, 20), &font, cvScalar(0, 255, 0));
							cvWaitKey(0);
						}
					}*/
					if (di)
					{
						awpRect rect1 = di->GetBounds()->GetRect();
						bool result_90;
						if (result == true) 
						{
							awpFillRect(img, &rect1, 1, 255);

							/*
							awpImage* tmp = NULL;
							awpCopyRect(img, &tmp, &rect1);
							if(tmp!=NULL)
							{
								result_90 = a.SetSourceImage(tmp, true);
								if(result_90 == true)
								{ 
									int num_a = a.GetItemsCount();
									if (num_a > 0)
									{
										for (int k = 0; k < num_a; k++)
										{
											TLFDetectedItem* di_90 = a.GetItem(k);
											awpRect rect_90 = di_90->GetBounds()->GetRect();
											awpRect rectInImage = {rect1.left + rect_90.left, rect1.top + rect_90.top, rect1.left + rect_90.right, rect1.top + rect_90.bottom };
											awpFillRect(img, &rectInImage, 2, 255);
										}
									}
								}
								awpReleaseImage(&tmp); 
							}*/
							

						}
						/*if (result == false) {
							awpFillRect(img, &rect1, 2, 255);
							nerr++;
						}*/
						
						//TRect* r = new TRect(rect.left, rect.top, rect.right, rect.bottom);
						//m_objects->Add(r);
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
			if (frames % (num_frames / 100) == 0)printf(">");
			awpReleaseImage(&img);
		}
		//cvShowImage(_MODULE_, frame);
	}
	//nerr = 32000;
	printf("\nNumber of errors is %d", nerr);

	awpcvDisconnect(video);
	return 0;
}

