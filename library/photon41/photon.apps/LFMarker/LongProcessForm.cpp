//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "LongProcessForm.h"
#include "MainForm.h"
#include "DbSplitForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TLongProcDlg *LongProcDlg;
//---------------------------------------------------------------------------
__fastcall TLongProcDlg::TLongProcDlg(TComponent* Owner)
	: TForm(Owner)
{
	m_procType = ptNothing;
}
//---------------------------------------------------------------------------
void __fastcall TLongProcDlg::ProgressHandler(int Progress, AnsiString& aComment)
{
	ProgressBar1->Position  = Progress;
	Label1->Caption =  aComment;
    Application->ProcessMessages();
}
void __fastcall TLongProcDlg::FormShow(TObject *Sender)
{
  Label1->Caption = L"";
  ProgressBar1->Position = 0;
  Form1->m_db.OnProgress = ProgressHandler;
  Timer1->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TLongProcDlg::Timer1Timer(TObject *Sender)
{
	Timer1->Enabled = false;
	switch(m_procType)
	{
	  case ptCopy:
		Label2->Caption = "Copy database...";
		Form1->m_db.CopyDatabase(Form1->copy_options);
	  break;
	  case ptConvert:
	  {
		Label2->Caption = "Convert database...";
		AnsiString str = Form1->m_db.DbName;
		Form1->m_db.ConvertDatabase(Form1->convert_options);
		Form1->OpenDatabase(str.c_str());
	  }
	  break;
	  case ptExport:
	   Label2->Caption = "Export fragments from database...";
	   Form1->m_db.ExportFragments(Form1->export_options);
	  break;
	  case ptSplit:
		Label2->Caption = "Split database...";
		Form1->m_db.SplitDatabase(Form1->DirectoryListBox1->Directory,
		DbSplitDlg->Edit1->Text,DbSplitDlg->Edit2->Text, DbSplitDlg->SpinEdit1->Value);
	  break;
	  case ptAnalysis:
		Label2->Caption = "Analysis database...";
		Form1->m_db.DoMarking(Form1->Engine);
	  break;
      case ptClear:
        Label2->Caption = "Clear database...";
        Form1->m_db.ClearDatabase();
      break;
      case ptUpdate:
        Label2->Caption = "Update database...";
        Form1->m_db.UpdateDatabase();
      break;
	}
	Close();
}
//---------------------------------------------------------------------------

