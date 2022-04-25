// awpcvvideo.cpp : Defines the exported functions for the DLL application.
//

#include "awpcvvideo.h"
#include "opencv\cv.h"
#include "opencv\highgui.h"
#include "opencv2\core\core.hpp"
#include "opencv2\core\utility.hpp"
#include "opencv2\imgproc\imgproc.hpp"
#include "opencv2\imgproc.hpp"
#include "opencv2\highgui\highgui.hpp"

using namespace cv;
using namespace std;

extern "C"
{
	#pragma comment(lib, "opencv_world300.lib")
}

static int awpImageDepthToCvDepth(awpImage* src)
{
    switch(src->dwType)
    {
        case AWP_BYTE:
            return IPL_DEPTH_8U;
        case AWP_SHORT:
            return IPL_DEPTH_8S;
        case AWP_FLOAT:
            return IPL_DEPTH_32F;
		case AWP_DOUBLE:
			return 64;
        default:
            return -1;
    }
}
//converts awpImage.awpType to openCV depth
//in the error case returns -1
static int awpCvDepthToImageDepth(IplImage* src)
{
    switch(src->depth)
    {
        case IPL_DEPTH_8U:
            return AWP_BYTE;
        case IPL_DEPTH_8S:
            return AWP_SHORT;
        case IPL_DEPTH_32F:
            return AWP_FLOAT;
		case 64:
			return AWP_DOUBLE;
        default:
            return -1;
    }
}
//returns line width of awpImage in BYTES
//in the error case returns -1
static int awpWidthStep(awpImage* src)
{
    int c = src->bChannels*src->sSizeX;
    switch(src->dwType)
    {
        case AWP_BYTE:
            return c*sizeof(AWPBYTE);
        case AWP_SHORT:
            return c*sizeof(short);
        case AWP_FLOAT:
            return c*sizeof(float);
		case AWP_DOUBLE:
			return c*sizeof(double);
        default:
            return -1;
    }
}
// awp to openCV image convertion
static IplImage* awpConvertToIplImage(awpImage* src)
{
  IplImage* result = NULL;
  if (src == NULL)
    return result;
  CvSize size;
  if (awpCheckImage(src) != AWP_OK)
    return result;
  size.width  = src->sSizeX;
  size.height = src->sSizeY;
  int depth = awpImageDepthToCvDepth(src);
  if (depth == -1)
    return result;
  int awpStep = awpWidthStep(src);
  if (awpStep == -1)
    return result;
  result = cvCreateImage(size, depth, src->bChannels);
  if (result == NULL)
    return result;
  // copy image pixels
  AWPBYTE* awp = (AWPBYTE*)src->pPixels;
  AWPBYTE* ipl = (AWPBYTE*)result->imageData;

  int iplStep = result->widthStep;
  for (int i = 0; i < src->sSizeY; i++)
  {
    memcpy(ipl,awp, awpStep);
    ipl += iplStep;
    awp += awpStep;
  }
  return result;
}



//openCV to awp convertion
static awpImage* awpConvertToAwpImage(IplImage* src)
{
    awpImage* result = NULL;
    if (src == NULL)
        return result;
//    if (src->nChannels != 1)
//        return result;
    int type = awpCvDepthToImageDepth(src);
    if (type < 0)
        return result;
	if (awpCreateImage(&result, src->width, src->height, src->nChannels, type) != AWP_OK)
        return result;
    AWPBYTE* awp = (AWPBYTE*)result->pPixels;
    AWPBYTE* ipl = (AWPBYTE*)src->imageData;
    int awpStep = awpWidthStep(result);
    int iplStep = src->widthStep;
    for (int i = 0; i < result->sSizeY; i++)
    {
      memcpy(awp, ipl, awpStep);
      ipl += iplStep;
      awp += awpStep;
    }
     return result;

}


class CVVideoSource
{
private:
	CvCapture* capture;
	int        m_num_frames;
public:
	CVVideoSource();
	~CVVideoSource();

	bool Connect(LPSTR pszFilename);
	bool Disconnect();
	awpImage* QueryNextImage();
	int NumFrames();
	bool SetPos(int pos);
	double Duration();
	double CurrentTime();
};

CVVideoSource::CVVideoSource()
{
	capture = NULL;
	m_num_frames = 0;
}

CVVideoSource::~CVVideoSource()
{
	Disconnect();
}

bool CVVideoSource::Connect(LPSTR pszFilename)
{
	Disconnect();
	capture = cvCaptureFromFile(pszFilename);
	if (capture != NULL)
	{
		NumFrames();
		return true;
	}

	return false;
}
bool CVVideoSource::Disconnect()
{
	if (capture != NULL)
		cvReleaseCapture(&capture);
	capture = NULL;
	m_num_frames = 0;
	return true;
}
awpImage* CVVideoSource::QueryNextImage()
{
	if (capture)
	{
		IplImage* img = cvQueryFrame(capture);
		return awpConvertToAwpImage(img);
	}
	
	return NULL;
}

int CVVideoSource::NumFrames()
{
	if (capture)
	{
		double n = cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_COUNT);
		m_num_frames = (int)n;
		return (int)n;
	}

	return 0;
}
bool CVVideoSource::SetPos(int pos)
{

	if (pos < 0 || pos >= this->m_num_frames)
		return false;
	
	if (capture)
	{
		cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, (double)pos);
		return true;
	}
	
	return false;
}

double CVVideoSource::Duration()
{
	if (capture)
	{
		double n   = cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_COUNT);
		double fps = cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);
		return n*fps;
	}

	return 0;
}
double CVVideoSource::CurrentTime()
{
	if (capture)
	{
		double n = cvGetCaptureProperty(capture, CV_CAP_PROP_POS_MSEC);
		return n;
	}

	return 0;
}


//================================================================================================
typedef struct
{
	int size;
	CVVideoSource* video;
}THEVIDEO;

//================================================================================================

extern "C" AWPCVVIDEO_API HCVVIDEO awpcvConnect(LPSTR pszFilename)
{
	THEVIDEO* theVideo = new THEVIDEO;
	theVideo->size = sizeof(THEVIDEO);
	theVideo->video = new CVVideoSource();

	if (theVideo->video->Connect(pszFilename))
	{
		return (HCVVIDEO)theVideo;
	}
	else
	{
		delete theVideo->video;
		delete theVideo;
	}

	return NULL;
}
extern "C" AWPCVVIDEO_API HRESULT awpcvDisconnect(HCVVIDEO hvideo)
{
	if (hvideo == NULL)
		return E_FAIL;

	THEVIDEO* theVideo = (THEVIDEO*)hvideo;

	if (theVideo->size != sizeof(THEVIDEO))
		return E_FAIL;
	if (theVideo->video == NULL)
		return E_FAIL;

	theVideo->video->Disconnect();
	
	return S_OK;
}

extern "C" AWPCVVIDEO_API HRESULT awpcvQueryImage(HCVVIDEO hvideo, awpImage** image)
{

	if (hvideo == NULL)
		return E_FAIL;

	THEVIDEO* theVideo = (THEVIDEO*)hvideo;

	if (theVideo->size != sizeof(THEVIDEO))
		return E_FAIL;
	if (theVideo->video == NULL)
		return E_FAIL;

	awpImage* img = NULL;
	img = theVideo->video->QueryNextImage();

	if (img)
	{
		awpCopyImage(img, image);
		awpReleaseImage(&img);
	}
	else
		*image = NULL;

	return S_OK;
}

extern "C" AWPCVVIDEO_API HRESULT awpcvQueryImagePos(HCVVIDEO hvideo, awpImage** imgage, int pos)
{

	if (hvideo == NULL)
		return E_FAIL;

	THEVIDEO* theVideo = (THEVIDEO*)hvideo;

	if (theVideo->size != sizeof(THEVIDEO))
		return E_FAIL;
	if (theVideo->video == NULL)
		return E_FAIL;

	theVideo->video->SetPos(pos);

	return awpcvQueryImage(hvideo, imgage);
}

extern "C" AWPCVVIDEO_API HRESULT awpcvFreeImage(awpImage* image)
{
	if (image!= NULL)
		awpReleaseImage(&image);
	return S_OK;
}

extern "C" AWPCVVIDEO_API HRESULT awpcvNumFrames(HCVVIDEO hvideo, int* num)
{
	*num = 0;

	if (hvideo == NULL)
		return E_FAIL;

	THEVIDEO* theVideo = (THEVIDEO*)hvideo;

	if (theVideo->size != sizeof(THEVIDEO))
		return E_FAIL;
	if (theVideo->video == NULL)
		return E_FAIL;

	*num = theVideo->video->NumFrames();
	return S_OK;
}



extern "C" AWPCVVIDEO_API HRESULT awpcvLoadImage(LPCSTR pszFilename, awpImage** image)
{
	IplImage* img = NULL;

	img = cvLoadImage(pszFilename);
	if (img == NULL)
		return E_FAIL;

	awpImage* tmp = awpConvertToAwpImage(img);
	if (tmp == NULL)
	{
		cvReleaseImage(&img);
		return E_FAIL;
	}

	*image = tmp;
	cvReleaseImage(&img);
	return S_OK;
}

extern "C" AWPCVVIDEO_API HRESULT awpcvSaveImage(LPCSTR pszFilename, awpImage* image)
{
	IplImage* img = NULL;
	img = awpConvertToIplImage(image);
	if (img == NULL)
		return E_FAIL;
	cvSaveImage(pszFilename, img);
	cvReleaseImage(&img);
	return S_OK;
}

typedef struct
{
	int size;
	CvVideoWriter* writer;
}THEWRITER;

extern "C" AWPCVVIDEO_API HCVVIDEOREC awpcvCreateWriter(LPSTR pszFilename, awpImage* img)
{
	THEWRITER* theVideo = new THEWRITER;
	theVideo->size = sizeof(THEWRITER);
	double fps = 20;
	CvSize size;
	size.width = img->sSizeX;
	size.height = img->sSizeY;

	theVideo->writer = cvCreateVideoWriter(pszFilename, CV_FOURCC('D', 'I', 'V', 'X'), fps, size);

	if (theVideo->writer == NULL)
		return NULL;

	IplImage* img1 = NULL;
	img1 = awpConvertToIplImage(img);
	if (img1 == NULL)
	{
		cvReleaseVideoWriter(&theVideo->writer);
		delete theVideo;
		return NULL;
	}

	cvWriteFrame(theVideo->writer, img1);
	cvReleaseImage(&img1);

	return (HCVVIDEOREC)theVideo;
}
extern "C" AWPCVVIDEO_API HRESULT awpcvWriteFrame(HCVVIDEOREC hrec, awpImage* frame)
{
	if (hrec == NULL)
		return E_FAIL;

	THEWRITER* theVideo = (THEWRITER*)hrec;
	if (theVideo->size != sizeof(THEWRITER))
		return E_FAIL;

	IplImage* img1 = NULL;
	img1 = awpConvertToIplImage(frame);
	if (img1 == NULL)
	{
		cvReleaseVideoWriter(&theVideo->writer);
		delete theVideo;
		return NULL;
	}

	cvWriteFrame(theVideo->writer, img1);
	cvReleaseImage(&img1);

	return S_OK;
}
extern "C" AWPCVVIDEO_API HRESULT awpcvCloseWriter(HCVVIDEOREC hrec)
{
	if (hrec == NULL)
		return E_FAIL;

	THEWRITER* theVideo = (THEWRITER*)hrec;
	if (theVideo->size != sizeof(THEWRITER))
		return E_FAIL;

	cvReleaseVideoWriter(&theVideo->writer);
	delete theVideo;

	return S_OK;
}

extern "C" AWPCVVIDEO_API HRESULT awpcvDetectLines(awpImage* image, awpLineSegment** segments, int* num)
{
	if (image == NULL || num == NULL || segments == NULL)
		return E_FAIL;
//	if (awpCheckImage(image) != AWP_OK)
//		return E_FAIL;
	awpImage* _awp = NULL;
	awpCopyImage(image, &_awp);
	awpConvert(_awp, AWP_CONVERT_3TO1_BYTE);
	awpResize(_awp,480, 640);
	//awpGaussianBlur(_awp, _awp, 2);
	IplImage* img = awpConvertToIplImage(_awp);
	Mat _image;// = imread("1.png", 0);
	_image = cv::cvarrToMat(img);

	Mat dst;
	Ptr<LineSegmentDetector> ls = createLineSegmentDetector(LSD_REFINE_STD);
	vector<Vec4f> lines;
	// Detect the lines
	ls->detect(_image, lines);
	cvReleaseImage(&img);
	awpReleaseImage(&_awp);
	if (lines.size() == 0)
		return E_FAIL;
	// copy to 
	*num = lines.size();
	*segments = (awpLineSegment*)malloc((*num)*sizeof(awpLineSegment));
	for (unsigned int i = 0; i < lines.size(); i++)
	{
		(*segments)[i].strat.X = (AWPSHORT)lines[i][0];
		(*segments)[i].strat.Y = (AWPSHORT)lines[i][1];
		(*segments)[i].end.X = (AWPSHORT)lines[i][2];
		(*segments)[i].end.Y = (AWPSHORT)lines[i][3];

	}
	return S_OK;
}
extern "C" AWPCVVIDEO_API HRESULT awpcvFreeSegments(awpLineSegment* segments)
{
	if (segments == NULL)
		return E_FAIL;
	free(segments);
	return S_OK;
}

extern "C" AWPCVVIDEO_API HRESULT awpcvCornerHarris(awpImage* image, awpImage* corners)
{
	awpImage* _awp = NULL;
	awpCopyImage(image, &_awp);
	IplImage* img = awpConvertToIplImage(_awp);
	IplImage* harris = cvCreateImage(cvGetSize(img), IPL_DEPTH_32F, 1);
	IplImage* harris_norm = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
	cvCornerHarris(img, harris, 5, 15, 0.02);
	cvNormalize(harris, harris_norm, 0, 255, NORM_MINMAX);
	// copy data 
	AWPBYTE* b = (AWPBYTE*)corners->pPixels;
	unsigned char* bb = (unsigned char*)harris_norm->imageData;
	for (int i = 0; i < corners->sSizeY; i++)
	{
		for (int j = 0; j < corners->sSizeX; j++)
		{
			b[i*corners->sSizeX + j] = bb[i*harris_norm->widthStep + j];
		}
	}
	awpReleaseImage(&_awp);
	cvReleaseImage(&harris);
	cvReleaseImage(&img);
	cvReleaseImage(&harris_norm);
	return S_OK;
}

extern "C" AWPCVVIDEO_API HRESULT awpcvFBFlow(awpImage* image1, awpImage* image2, awpImage* flow)
{
	IplImage* img1 = awpConvertToIplImage(image1);
	IplImage* img2 = awpConvertToIplImage(image2);
	IplImage* img3 = awpConvertToIplImage(flow);

	// calc flow 
	cvCalcOpticalFlowFarneback(img1, img2, img3, 0.5,3, 15, 3, 5, 1.2, 0);

	// copy flow 
	AWPFLOAT* af = (AWPFLOAT*)flow->pPixels;
	float*    ifl = (float*)img3->imageData;
	for (int i = 0; i < flow->sSizeY; i++)
	{
		for (int j = 0; j < flow->sSizeX; j++)
		{
			af[2*i*flow->sSizeX + 2*j] = ifl[i*img3->widthStep / sizeof(float) + 2*j];
			af[2 * i*flow->sSizeX + 2 * j + 1] = ifl[i*img3->widthStep / sizeof(float) + 2 * j + 1];
		}
	}

	cvReleaseImage(&img1);
	cvReleaseImage(&img2);
	cvReleaseImage(&img3);
	return S_OK;
}


extern "C" AWPCVVIDEO_API HRESULT awpcvDuration(HCVVIDEO hvideo, double* d)
{
	*d = 0;

	if (hvideo == NULL)
		return E_FAIL;

	THEVIDEO* theVideo = (THEVIDEO*)hvideo;

	if (theVideo->size != sizeof(THEVIDEO))
		return E_FAIL;
	if (theVideo->video == NULL)
		return E_FAIL;

	*d = theVideo->video->Duration();
	return S_OK;
}
/*

*/
extern "C" AWPCVVIDEO_API HRESULT awpcvCurrentTime(HCVVIDEO hvideo, double* d)
{
	*d = 0;

	if (hvideo == NULL)
		return E_FAIL;

	THEVIDEO* theVideo = (THEVIDEO*)hvideo;

	if (theVideo->size != sizeof(THEVIDEO))
		return E_FAIL;
	if (theVideo->video == NULL)
		return E_FAIL;

	*d = theVideo->video->CurrentTime();
	return S_OK;
}
/*
	Входное изображение  awpImage: image - на котором должны быть найдены окружности. 
	изображение должно иметь тип AWP_BYTE и один канал яркости. 
	Выходное изображение  awpImage: cirles - изображение в который записывается результат 
	работы алгоритма. Должно иметь тип AWP_DOUBLE, иметь один канал и высоту - 3 пикселя. 
	Ширина изоражения определяет количество найденных окружностей. 

	если функция cvHoughCircles найдет больше окружностей, чем ширина изображения 
	то записываются первые N окружностей из результата работы функции cvHoughCircles 
	где N = ширине изображения cirles 

	переменная num содержит актуальное число записанных окружностей. 
*/
extern "C" AWPCVVIDEO_API HRESULT awpcvHoughCircles(awpImage* image, awpImage* circles, int* num)
{
	if (image == NULL || circles == NULL || num == NULL)
		return E_FAIL;
	if (awpCheckImage(image) != AWP_OK)
		return E_FAIL;
	if (awpCheckImage(circles) != AWP_OK)
		return E_FAIL;

	if (image->dwType != AWP_BYTE || image->bChannels != 1)
		return E_FAIL;
	if (circles->dwType != AWP_DOUBLE || circles->bChannels != 1 || circles->sSizeY != 3)
		return E_FAIL;

	IplImage* img = awpConvertToIplImage(image);
	if (img == NULL)
		return E_FAIL;
	
	CvMemStorage* storage = cvCreateMemStorage(0);
	if (storage == NULL)
	{
		cvReleaseImage(&img);
		return E_FAIL;
	}
	*num = 0;
	// пробуем вызвать функцию 
	CvSeq* results = cvHoughCircles(img, storage, CV_HOUGH_GRADIENT, 2, 3, 200, 200, img->height/5, img->height / 2);
	AWPDOUBLE* data = (AWPDOUBLE*)circles->pPixels;
	for (int i = 0; i < results->total; i++) 
	{
		float* p = (float*)cvGetSeqElem(results, i);
		data[i] = p[0];
		data[circles->sSizeX + i] = p[1];
		data[2*circles->sSizeX + i] = p[2];
		(*num)++;
		if (*num >= circles->sSizeX)
			break;
	}
	cvReleaseImage(&img);
	cvReleaseMemStorage(&storage);
	return S_OK;
}
extern "C" AWPCVVIDEO_API HRESULT awpcvShowImage(const char* Title, awpImage* image)
{
	IplImage* img = NULL;
	img = awpConvertToIplImage(image);
	if (img == NULL)
		return E_FAIL;
	cvShowImage (Title, img);
	cvReleaseImage(&img);
	return S_OK;
}
extern "C" AWPCVVIDEO_API HRESULT awpcvWaitKey(int millis, int* nchar){
	*nchar = cvWaitKey(millis);
	return S_OK;
}
