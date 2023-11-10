//---------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "DbInfoForm.h"
#include "MainForm.h"
//--------------------------------------------------------------------- 
#pragma resource "*.dfm"
TDbInfoDialog *DbInfoDialog;
//---------------------------------------------------------------------
__fastcall TDbInfoDialog::TDbInfoDialog(TComponent* AOwner)
	: TForm(AOwner)
{
}
//---------------------------------------------------------------------

void __fastcall TDbInfoDialog::ProgressHandler(int Progress, AnsiString& aComment)
{

}



