#ifndef _awp_cvvideo_h_
#define _awp_cvvideo_h_

#include "awpipl.h"

#ifdef AWPCVVIDEO_EXPORTS
#define AWPCVVIDEO_API  __declspec(dllexport)  
#else
#define AWPCVVIDEO_API  __declspec(dllimport) 
#endif

DECLARE_HANDLE(HCVVIDEO);

extern "C" AWPCVVIDEO_API HCVVIDEO awpcvConnect(LPSTR pszFilename);
extern "C" AWPCVVIDEO_API HRESULT awpcvDisconnect(HCVVIDEO hvideo);
extern "C" AWPCVVIDEO_API HRESULT awpcvQueryImage(HCVVIDEO hvideo, awpImage** imgage);
extern "C" AWPCVVIDEO_API HRESULT awpcvFreeImage(awpImage* img);
extern "C" AWPCVVIDEO_API HRESULT awpcvNumFrames(HCVVIDEO hvideo, int* num);


#endif //_awp_cvvideo_h_