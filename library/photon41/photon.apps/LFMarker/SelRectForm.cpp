//---------------------------------------------------------------------------
#include <vcl.h>
#include "MainForm.h"
#pragma hdrstop
#include "SelRectForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TRectToolForm *RectToolForm;
//---------------------------------------------------------------------------
__fastcall TRectToolForm::TRectToolForm(TComponent* Owner)
		: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TRectToolForm::UpdateDetectedItem()
{
	int bw = StrToInt(ComboBox1->Text);
	int bh = StrToInt(ComboBox2->Text);
    UnicodeString str = ComboBox3->Text;
    AnsiString _ansi = str;
	std::string  type = _ansi.c_str();
	int angle = ComboBox4->ItemIndex;

	if (Form1->m_pBaseObject != NULL)
		delete Form1->m_pBaseObject;
	UUID id;
	LF_NULL_UUID_CREATE(id);
	Form1->m_pBaseObject = new TLFDetectedItem(NULL, 0, type, 0, angle,
	bw,bh,"is marked by a person", id);
 //	if (Form1->FImage1->CurrentTool == ftMarkRect)
 //	{
 //		 TMarkRectTool* theTool =   (TMarkRectTool*)Form1->FImage1->Tool;
 //		 theTool->SetAR(Form1->m_pBaseObject->GetBW(),Form1->m_pBaseObject->GetBH());
 //	}
}
//---------------------------------------------------------------------------
void __fastcall TRectToolForm::FormShow(TObject *Sender)
{
	int captionHeight = GetSystemMetrics( SM_CYCAPTION	);
	int menuHeight    = GetSystemMetrics( SM_CYMENU	);

	Left = Form1->Left + Form1->Width - Width;
	Top  = Form1->Top + Form1->ToolBar1->Height + captionHeight + menuHeight;
    ComboBox3->Items->Clear();
    for (int i = 0; i < Form1->m_db.Dictionary->GetCount(); i++)
    {
        TLFSemanticDictinaryItem* word = Form1->m_db.Dictionary->GetWordFromDictinary(i);
        if (word != NULL)
        {
			ComboBox3->Items->Add(word->GetItemLabel());
        }
    }
    ComboBox3->ItemIndex = 0;
    this->ComboBox3Change(NULL);
}
//---------------------------------------------------------------------------
void __fastcall TRectToolForm::FormClose(TObject *Sender,
	  TCloseAction &Action)
{
/*	if (Form1->FImage1->CurrentTool == ftMarkRect)
	{
		// переключим CurrentTool на ftPane
	  Form1->FImage1->CurrentTool     = ftNone;
	  Form1->FImage1->CurrentTool     = ftPane;
	  this->CheckBox1->Checked = Form1->SetNearestOverlap;
	}
    */
}
//---------------------------------------------------------------------------
void __fastcall TRectToolForm::ComboBox1Change(TObject *Sender)
{
	 	this->UpdateDetectedItem();
}
//---------------------------------------------------------------------------
void __fastcall TRectToolForm::ComboBox3Change(TObject *Sender)
{
	TLFSemanticDictinaryItem* word = Form1->m_db.Dictionary->GetWordFromDictinary(ComboBox3->ItemIndex);
    ComboBox1->Items->Clear();
    ComboBox2->Items->Clear();

	if (word != NULL)
    {
    	ILFScanner* s = word->GetScanner();
        ComboBox1->Items->Add(IntToStr(s->GetBaseWidth()));
        ComboBox2->Items->Add(IntToStr(s->GetBaseHeight()));
        ComboBox1->ItemIndex = 0;
        ComboBox2->ItemIndex = 0;
		this->UpdateDetectedItem();
    }
}
//---------------------------------------------------------------------------
 void __fastcall TRectToolForm::ComboBox2Change(TObject *Sender)
{
	this->UpdateDetectedItem();
}
//---------------------------------------------------------------------------
void __fastcall TRectToolForm::ComboBox4Change(TObject *Sender)
{
	this->UpdateDetectedItem();
}
//---------------------------------------------------------------------------
void __fastcall TRectToolForm::CheckBox1Click(TObject *Sender)
{
   Form1->SetNearestOverlap = this->CheckBox1->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TRectToolForm::ComboBox3Exit(TObject *Sender)
{
   if (ComboBox3->Text != L"")
   {
   		ComboBox3->Items->Add(ComboBox3->Text);
   }
}
//---------------------------------------------------------------------------
