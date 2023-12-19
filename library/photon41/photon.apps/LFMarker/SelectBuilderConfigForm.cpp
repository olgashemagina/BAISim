//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "SelectBuilderConfigForm.h"
#include "BuildDetectorOptionsForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TSelectBuildConfForm *SelectBuildConfForm;
//---------------------------------------------------------------------------
__fastcall TSelectBuildConfForm::TSelectBuildConfForm(TComponent* Owner)
	: TForm(Owner)
{
	m_configFilename = "";
}
//---------------------------------------------------------------------------
void __fastcall TSelectBuildConfForm::Button1Click(TObject *Sender)
{
	if (OpenDialog1->Execute())
	{
		this->Edit1->Text = OpenDialog1->FileName;
		m_configFilename = OpenDialog1->FileName;
	}
}
//---------------------------------------------------------------------------
void __fastcall TSelectBuildConfForm::CancelButtonClick(TObject *Sender)
{
	this->Close();
}
//---------------------------------------------------------------------------
void __fastcall TSelectBuildConfForm::ChooseClick(TObject *Sender)
{
	if(Choose->ItemIndex == 1)
	{
		Edit1->Visible = true;
		Button1->Visible = true;
	}
	else
	{
		Edit1->Visible = false;
		Button1->Visible = false;
    }
}
//---------------------------------------------------------------------------
void __fastcall TSelectBuildConfForm::OKButtonClick(TObject *Sender)
{
	SelectBuildConfForm->Close();
	CSBuildOptions->Show();
}
//---------------------------------------------------------------------------
