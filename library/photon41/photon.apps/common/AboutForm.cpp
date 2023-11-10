//---------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "AboutForm.h"
//--------------------------------------------------------------------- 
#pragma resource "*.dfm"
TAboutBox *AboutBox;
//--------------------------------------------------------------------- 
__fastcall TAboutBox::TAboutBox(TComponent* AOwner)
	: TForm(AOwner)
{
}
//---------------------------------------------------------------------
void __fastcall TAboutBox::FormCreate(TObject *Sender)
{
//    Memo1->Lines->LoadFromFile("readme.txt");
}
//---------------------------------------------------------------------------

