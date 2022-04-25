// Player.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
extern "C"
{
#pragma comment(lib, "awpipl2.lib")
}



int main(int argc, char* argv[])
{
	if (argc < 3)return 2;
	HCVVIDEO video;
	TSCObjectDetector d;
	HRESULT res = S_OK;
	bool pause = false;
	int  num_frames;
	int  frames, delta = 0, nerr = 0, ierr = 0;
	const char _MODULE_[] = "";
	awpRect rect;
	awpImage* img = NULL;
	video = awpcvConnect(argv[1]);//"C:\\1.mp4"
	if (awpcvQueryImagePos(video, &img, 0) != S_OK)return 1;
	rect.bottom = img->sSizeY / 12;
	rect.top = 1;//  10;
	rect.left = 1;// ->sSizeX - 1;
	rect.right = img->sSizeX / 12;// /*img->sSizeX/12*/100;
	awpcvNumFrames(video, &num_frames);//(int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_COUNT);
	d.LoadDetector(argv[2]);//"C:\\Users\\Авас yuri\\Documents\\IPF\\readresearch\\road.xml"
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
			awpResizeBilinear(img, 160, 90);
			d.Init(img, true);
			int result = d.Detect();
			frames++;
			if (result > 0) {
				awpFillRect(img, &rect, 1, 255);
			}
			if (result <= 0) {
				awpFillRect(img, &rect, 2, 255);
				nerr++;
			}
			awpcvShowImage(_MODULE_, img);
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

