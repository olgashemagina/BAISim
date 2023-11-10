//---------------------------------------------------------------------------
#pragma hdrstop
#include "photondll.h"
#include "..\photon\FImage41.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "FImage41"

PHOTON_API HANDLE   photonCreate(HWND hWnd)
{
	TPhImage* p = new TPhImage(hWnd);
	return HANDLE(p);
}

PHOTON_API HRESULT  photonRelease(HANDLE photon)
{
	TPhImage* p = (TPhImage*)(photon);
	if (p == NULL)
		return E_FAIL;
	delete p;
	return S_OK;
}

PHOTON_API HRESULT photonInit(HANDLE photon, LPWSTR* names, int count)
{
	TPhImage* p = (TPhImage*)(photon);
	if (p == NULL)
		return E_FAIL;
	if (count <= 0 || names == NULL)
		return E_FAIL;

	TStringList* namesList = new TStringList();
	for (int i = 0; i < count; i++)
		namesList->Add(names[i]);

	bool result = p->Init(namesList);

	delete namesList;
	return result?S_OK:E_FAIL;
}

PHOTON_API HRESULT photonClose(HANDLE photon)
{
	TPhImage* p = (TPhImage*)(photon);
	if (p == NULL)
		return E_FAIL;

	p->Close();

	return S_OK;
}


