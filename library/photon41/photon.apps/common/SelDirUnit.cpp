//---------------------------------------------------------------------------
#define NO_WIN32_LEAN_AND_MEAN
#pragma hdrstop
#include <vcl.h>
#include "SelDirUnit.h"
#define MAX_PATH_LEN 2048
//---------------------------------------------------------------------------
bool __fastcall GetDirNamePreview(String& DirName)
{
  BROWSEINFO info;
  TCHAR chDir[MAX_PATH_LEN];
  LPITEMIDLIST ItemID = NULL;
  DirName = "";
  memset(&info,0, sizeof(info));
  memset(chDir,0, sizeof(chDir));
  info.hwndOwner = NULL;
  info.ulFlags = info.ulFlags | BIF_RETURNONLYFSDIRS |  BIF_NEWDIALOGSTYLE;
  ItemID = SHBrowseForFolder (&info);
  if (ItemID)
  {
		if (SHGetPathFromIDList(ItemID, chDir))
        {
          DirName = chDir;
          return true;
        }
        else
        {
		   String _ansi = Application->Name;
		   MessageBox(NULL, L"Cannot open folder ", _ansi.c_str(), MB_OK |MB_ICONEXCLAMATION);
           return false;
		}
  }
  else
        return false;
}

AnsiString __fastcall GetSpacialFolderPath(int folder)
{
 String result = "";
 TCHAR   path[MAX_PATH];
 if (SHGetFolderPath( NULL, folder, NULL, SHGFP_TYPE_CURRENT, path) == S_OK)
    result = path;
 return result;
}
#pragma package(smart_init)
 