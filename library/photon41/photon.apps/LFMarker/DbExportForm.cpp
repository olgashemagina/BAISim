//---------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "DbExportForm.h"
#include "SelDirUnit.h"
//--------------------------------------------------------------------- 
#pragma resource "*.dfm"
TDbExportDialog *DbExportDialog;
//---------------------------------------------------------------------
__fastcall TDbExportDialog::TDbExportDialog(TComponent* AOwner)
	: TForm(AOwner)
{
}
//---------------------------------------------------------------------

void __fastcall TDbExportDialog::Button1Click(TObject *Sender)
{
    String str = "";
    if (GetDirNamePreview(str))
    {
        this->Edit1->Text = str;
    }
}
//---------------------------------------------------------------------------

void __fastcall TDbExportDialog::ApplicationEvents1Idle(TObject *Sender, bool &Done)
{
    Edit2->Enabled = !CheckBox3->Checked;
}
//---------------------------------------------------------------------------

