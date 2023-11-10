//---------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "DbCopyForm.h"
#include "SelDirUnit.h"
//---------------------------------------------------------------------
#pragma resource "*.dfm"
TDbCopyDlg *DbCopyDlg;
//---------------------------------------------------------------------
__fastcall TDbCopyDlg::TDbCopyDlg(TComponent* AOwner)
	: TForm(AOwner)
{
}
//---------------------------------------------------------------------

void __fastcall TDbCopyDlg::FormShow(TObject *Sender)
{
 	Edit1->Text = DirectoryListBox1->Directory;
}
//---------------------------------------------------------------------------

void __fastcall TDbCopyDlg::DirectoryListBox1Change(TObject *Sender)
{
	Edit1->Text = DirectoryListBox1->Directory;
}
//---------------------------------------------------------------------------


void __fastcall TDbCopyDlg::Edit1Change(TObject *Sender)
{
//
}
//---------------------------------------------------------------------------

