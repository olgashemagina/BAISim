//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
//---------------------------------------------------------------------------
USEFORM("MainForm.cpp", Form1);
USEFORM("CopyForm.cpp", CopyDlg);
USEFORM("..\common\AboutForm.cpp", AboutBox);
USEFORM("..\common\DbInfoForm.cpp", DbInfoDlg);
//---------------------------------------------------------------------------
HANDLE hm = 0;
bool Check()
{
      bool res=  false;
//      hm = OpenMutex(MUTEX_ALL_ACCESS, false, "CFBROWSER_Mutex");
//      res = hm != 0;
//      if (hm == 0)
        hm = CreateMutex(NULL, false, "CFBROWSER_Mutex");

      return res;
}

WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    try
    {
         Check();
         Application->Initialize();
         Application->CreateForm(__classid(TForm1), &Form1);
		Application->CreateForm(__classid(TCopyDlg), &CopyDlg);
		Application->CreateForm(__classid(TCopyDlg), &CopyDlg);
		Application->CreateForm(__classid(TAboutBox), &AboutBox);
		Application->CreateForm(__classid(TDbInfoDlg), &DbInfoDlg);
		Application->Run();
         ReleaseMutex(hm);
         CloseHandle(hm);
    }
    catch (Exception &exception)
    {
         Application->ShowException(&exception);
    }
    catch (...)
    {
         try
         {
             throw Exception("");
         }
         catch (Exception &exception)
         {
             Application->ShowException(&exception);
         }
    }
    return 0;
}
//---------------------------------------------------------------------------
