#ifndef _awp_cvvideo_h_
#define _awp_cvvideo_h_

#include "awpipl.h"


#ifdef AWPCVVIDEO_EXPORTS
#define AWPCVVIDEO_API  __declspec(dllexport)  
#else
#define AWPCVVIDEO_API  __declspec(dllimport) 
#endif

DECLARE_HANDLE(HCVVIDEO);
DECLARE_HANDLE(HCVVIDEOREC);

extern "C" AWPCVVIDEO_API HCVVIDEO awpcvConnect(LPSTR pszFilename);
extern "C" AWPCVVIDEO_API HRESULT awpcvDisconnect(HCVVIDEO hvideo);
extern "C" AWPCVVIDEO_API HRESULT awpcvQueryImage(HCVVIDEO hvideo, awpImage** image);
extern "C" AWPCVVIDEO_API HRESULT awpcvQueryImagePos(HCVVIDEO hvideo, awpImage** image, int pos);
extern "C" AWPCVVIDEO_API HRESULT awpcvFreeImage(awpImage* img);
extern "C" AWPCVVIDEO_API HRESULT awpcvNumFrames(HCVVIDEO hvideo, int* num);
extern "C" AWPCVVIDEO_API HRESULT awpcvDuration(HCVVIDEO hvideo, double* d);
extern "C" AWPCVVIDEO_API HRESULT awpcvCurrentTime(HCVVIDEO hvideo, double* d);

extern "C" AWPCVVIDEO_API HRESULT awpcvLoadImage(LPCSTR pszFilename, awpImage** image);
extern "C" AWPCVVIDEO_API HRESULT awpcvSaveImage(LPCSTR pszFilename, awpImage* image);
extern "C" AWPCVVIDEO_API HCVVIDEOREC awpcvCreateWriter(LPSTR pszFilename, awpImage* img);
extern "C" AWPCVVIDEO_API HRESULT awpcvWriteFrame(HCVVIDEOREC hrec, awpImage* frame);
extern "C" AWPCVVIDEO_API HRESULT awpcvCloseWriter(HCVVIDEOREC hrec);

//
extern "C" AWPCVVIDEO_API HRESULT awpcvDetectLines(awpImage* image, awpLineSegment** segments, int* num); 
extern "C" AWPCVVIDEO_API HRESULT awpcvFreeSegments(awpLineSegment* segments);
extern "C" AWPCVVIDEO_API HRESULT awpcvCornerHarris(awpImage* image, awpImage* corners);

extern "C" AWPCVVIDEO_API HRESULT awpcvHoughCircles(awpImage* image, awpImage* circles, int* num);

//
extern "C" AWPCVVIDEO_API HRESULT awpcvFBFlow(awpImage* image1, awpImage* image2, awpImage* flow);

extern "C" AWPCVVIDEO_API HRESULT awpcvShowImage(const char* Title, awpImage* image);
extern "C" AWPCVVIDEO_API HRESULT awpcvWaitKey(int millis,int* nchar);



#endif //_awp_cvvideo_h_