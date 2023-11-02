//---------------------------------------------------------------------------
#include "PhUtils.h"
#include "System.SysUtils.hpp"
#include "Dialogs.hpp"
#pragma hdrstop
//---------------------------------------------------------------------------
static UnicodeString _ExtractFileNameWithoutExt(UnicodeString str)
{
	UnicodeString result = ExtractFileName(str);
	return ChangeFileExt(result, L"");
}
//---------------------------------------------------------------------------
static UnicodeString _makeName(int value, UnicodeString str)
{
	UnicodeString result = str;
	String dir  = ExtractFileDir(result);
	String name = _ExtractFileNameWithoutExt(result);

	int pos1 = name.Pos(L")");
	if (pos1 != 0)
	{
	  int pos2 = name.Pos(L" (");
	  if (pos2 != 0)
	  {
		name = name.SubString(0, pos2);
	  }

	}

	String ext = ExtractFileExt(result);
	result = dir + L"\\" + name;
	result += L" (";
	result += IntToStr(value);
	result += L")";
	result += ext;
	return result;
}
//---------------------------------------------------------------------------
UnicodeString MakeNewFileName(UnicodeString strFileName)
{
	UnicodeString result = strFileName;
	int value = 0;
	int pos1 = result.Pos(L").");
	if (pos1 != 0)
	{
	  int pos2 = result.Pos(L" (");
	  if (pos2 != 0)
	  {
		 UnicodeString str = result.SubString(pos2+2, pos1-pos2-2);
		 try
		 {
			 value = StrToInt(str);
		 }
		 catch(EConvertError& e)
		 {
			value = 0;
		 }
	  }
	}
	//------------
	 do
	 {
		value++;
		result = _makeName(value, strFileName);
		if (!FileExists(result, true))
			break;
	 }
	 while(true);
	return result;
}


bool PhCopyFile(UnicodeString strFileName, UnicodeString strFolder, EPhCopyActioin action)
{
	bool result = true;

	UnicodeString strSrcFile = strFileName;
	UnicodeString strDstFile = strFolder;

	strDstFile += ExtractFileName(strSrcFile);
	bool skipFile = false;
	if (FileExists(strDstFile))
	{
		switch(action)
		{
			case copySkip:
				skipFile = true;
			break;
			case copyReplace:
				skipFile = false;
			break;
			case copyNewName:
			   strDstFile = MakeNewFileName(strDstFile);
			break;
		}
	}

	if (skipFile)
		return result;
	result = CopyFile(strSrcFile.c_str(), strDstFile.c_str(), false) == S_OK;
	return result;
}

#pragma package(smart_init)
