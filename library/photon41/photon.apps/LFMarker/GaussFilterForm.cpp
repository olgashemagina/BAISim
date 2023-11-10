//---------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "GaussFilterForm.h"
//---------------------------------------------------------------------
#pragma link "cspin"
#pragma resource "*.dfm"
TGaussFilterDlg *GaussFilterDlg;
//--------------------------------------------------------------------- 
__fastcall TGaussFilterDlg::TGaussFilterDlg(TComponent* AOwner)
	: TForm(AOwner)
{
}
//---------------------------------------------------------------------

void __fastcall TGaussFilterDlg::CSpinEdit1Change(TObject *Sender)
{
	Label4->Caption = FormatFloat("0.00",0.1*(double)CSpinEdit1->Value);
}
//---------------------------------------------------------------------------

void __fastcall TGaussFilterDlg::FormShow(TObject *Sender)
{
	Label4->Caption = FormatFloat("0.00",0.1*(double)CSpinEdit1->Value);
}
//---------------------------------------------------------------------------

