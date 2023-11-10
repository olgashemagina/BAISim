//---------------------------------------------------------------------------
#ifndef photondllH
#define photondllH
//---------------------------------------------------------------------------

#include <windows.h>

#ifdef PHOTON_EXPORTS
	#define PHOTON_API __declspec(dllexport)
#else
	#define PHOTON_API __declspec(dllimport)
#endif

extern "C" PHOTON_API HANDLE   photonCreate(HWND hWnd);
extern "C" PHOTON_API HRESULT  photonRelease(HANDLE photon);

 extern "C" PHOTON_API HRESULT photonInit(HANDLE photon, LPWSTR* names, int count);
 extern "C" PHOTON_API HRESULT photonClose(HANDLE photon);

#endif
