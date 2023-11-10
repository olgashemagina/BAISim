//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "IniParamsUnit.h"
#include "System.IniFiles.hpp"
//---------------------------------------------------------------------------
#pragma package(smart_init)

const UnicodeString c_strSection = L"ObjectMarker";

static UnicodeString GetIniFileName()
{
	LPITEMIDLIST pidl;
	LPMALLOC pShellMalloc;
	wchar_t szDir[MAX_PATH];
	UnicodeString PathStr ;

	if (SUCCEEDED(SHGetMalloc(&pShellMalloc)))
	{
		if (SUCCEEDED(SHGetSpecialFolderLocation(NULL, CSIDL_APPDATA, &pidl)))
		{
			if (SHGetPathFromIDList(pidl, szDir))
			{
				PathStr = szDir ;
				PathStr = PathStr + "\\" ;
			}
			pShellMalloc->Free(pidl);
		}
		pShellMalloc->Release();
	}

	PathStr += L"\\NNVideolab\\";
	if (!DirectoryExists(PathStr))
		CreateDir(PathStr);

	PathStr += L"\\ObjectMarker\\";
	if (!DirectoryExists(PathStr))
		CreateDir(PathStr);


	return PathStr + L"\\ObjectMarker.ini";
}

TMarkerIniParams::TMarkerIniParams()
{
	LoadParams();
}

void __fastcall TMarkerIniParams::LoadParams()
{
	UnicodeString str = GetIniFileName();
	TIniFile *ini = NULL;
	ini = new TIniFile(str);
	this->m_UITheme      = ini->ReadString(c_strSection, L"theme", "Windows");
	this->m_strLastPath = ini->ReadString(c_strSection, L"LastPath", "C:\\");
	delete ini;
}

void __fastcall TMarkerIniParams::SaveParams()
{
	UnicodeString str = GetIniFileName();
	TIniFile *ini = NULL;
	ini = new TIniFile(str);
	ini->WriteString(c_strSection, L"theme", m_UITheme);
	ini->WriteString(c_strSection, L"LastPath", m_strLastPath);
	delete ini;
}

void __fastcall TMarkerIniParams::SetUITheme(const UnicodeString value)
{
 	this->m_UITheme = value;
	SaveParams();
}
void __fastcall TMarkerIniParams::SetLastPath(const UnicodeString& value)
{
	this->m_strLastPath = value;
	SaveParams();
}
UnicodeString __fastcall TMarkerIniParams::GetIniPath()
{
	LPITEMIDLIST pidl;
	LPMALLOC pShellMalloc;
	wchar_t szDir[MAX_PATH];
	UnicodeString PathStr ;

	if (SUCCEEDED(SHGetMalloc(&pShellMalloc)))
	{
		if (SUCCEEDED(SHGetSpecialFolderLocation(NULL, CSIDL_APPDATA, &pidl)))
		{
			if (SHGetPathFromIDList(pidl, szDir))
			{
				PathStr = szDir ;
				PathStr = PathStr + "\\" ;
			}
			pShellMalloc->Free(pidl);
		}
		pShellMalloc->Release();
	}

	PathStr += L"\\NNVideolab\\";
	if (!DirectoryExists(PathStr))
		CreateDir(PathStr);

	PathStr += L"\\ObjectMarker\\";
	if (!DirectoryExists(PathStr))
		CreateDir(PathStr);

	return PathStr;
}
