//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "DictionaryForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TDictionaryDialog *DictionaryDialog;
//---------------------------------------------------------------------------
__fastcall TDictionaryDialog::TDictionaryDialog(TComponent* Owner)
	: TForm(Owner)
{
	m_pDictionary = NULL;
    m_NewItemReady = false;
}
//---------------------------------------------------------------------------
void __fastcall TDictionaryDialog::ButtonedEdit1RightButtonClick(TObject *Sender)

{
 	ListBox1->Items->Add(ButtonedEdit1->Text);
    if (this->m_pDictionary != NULL)
    {
        AnsiString _ansi = ButtonedEdit1->Text;
    	TLFSemanticDictinaryItem* item = new TLFSemanticDictinaryItem(_ansi.c_str());
        this->m_pDictionary->AddWordToDictinary(item);
    }
}
//---------------------------------------------------------------------------
void __fastcall TDictionaryDialog::ApplicationEvents1Idle(TObject *Sender, bool &Done)

{
	LabeledEdit1->Enabled = ListBox1->ItemIndex >= 0;
	LabeledEdit2->Enabled = ListBox1->ItemIndex >= 0;
    MaskEdit2->Enabled = ListBox1->ItemIndex >= 0;
    MaskEdit3->Enabled = ListBox1->ItemIndex >= 0;
    LabeledEdit5->Enabled = ListBox1->ItemIndex >= 0;
    LabeledEdit6->Enabled = ListBox1->ItemIndex >= 0;
    CheckBox1->Enabled    = ListBox1->ItemIndex >= 0;
    this->ButtonedEdit1->RightButton->Enabled = ButtonedEdit1->Text != "" && m_NewItemReady;
}
//---------------------------------------------------------------------------
void __fastcall TDictionaryDialog::FormShow(TObject *Sender)
{
	// clear all
    ListBox1->Items->Clear();
    ButtonedEdit1->Text = L"";
	LabeledEdit1->Text = L"";//ListBox1->ItemIndex >= 0;
	LabeledEdit2->Text = L"";//ListBox1->ItemIndex >= 0;

    Label1->Caption = L"";
    if (this->m_pDictionary != NULL)
    {
    	// setup controls
        for (int i = 0; i < this->m_pDictionary->GetCount(); i++)
        {
			TLFSemanticDictinaryItem* word = m_pDictionary->GetWordFromDictinary(i);
            ListBox1->Items->Add(word->GetItemLabel());
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TDictionaryDialog::Button2Click(TObject *Sender)
{
    if (this->m_pDictionary != NULL)
    {
		this->m_pDictionary->SaveXML(c_lpDictFileName);
    }
}
//---------------------------------------------------------------------------
void __fastcall TDictionaryDialog::ListBox1Click(TObject *Sender)
{
	if (ListBox1->ItemIndex >= 0)
    {
         ButtonedEdit1->Text = ListBox1->Items->Strings[ListBox1->ItemIndex];
	  	 TLFSemanticDictinaryItem* word = m_pDictionary->GetWordFromDictinary(ListBox1->ItemIndex);
         if (word != NULL && word->GetScanner() != NULL)
         {
            ILFScanner* scanner = word->GetScanner();
         	LabeledEdit1->Text = IntToStr(scanner->GetBaseWidth());
         	LabeledEdit2->Text = IntToStr(scanner->GetBaseHeight());
            //setup paramaters editor
            ValueListEditor1->Strings->Clear();
            for (int i = 0; i < scanner->GetParamsCount(); i++)
            {
                TLFParameter*  param = scanner->GetParameter(i);
                if (param)
                {
                      AnsiString str = param->GetPName();
                      str += "=";
                      str += param->GetValue();
                      ValueListEditor1->Strings->Add(str);
                      Valedit::TItemProp* p = ValueListEditor1->ItemProps[i];
                      p->EditMask = L"\0\0\,09;1; ";
                }
            }
/*
            // setup masks
			Valedit::TItemProp* p = ValueListEditor1->ItemProps[0];
            p->EditMask = "00;1;_";
            //\0\,09;1;
			p = ValueListEditor1->ItemProps[1];
            p->EditMask = "\0\,00;1;_";

			p = ValueListEditor1->ItemProps[2];
            p->EditMask = "99;1;_";

			p = ValueListEditor1->ItemProps[3];
            p->EditMask = "99;1;_";
*/

            CalcItemsCount();
         }
    }
}
//---------------------------------------------------------------------------
void __fastcall TDictionaryDialog::LabeledEdit1KeyPress(TObject *Sender, System::WideChar &Key)
{
	CheckDigitKey(Key);
}
//---------------------------------------------------------------------------

void __fastcall TDictionaryDialog::ButtonedEdit1Change(TObject *Sender)
{
//
	this->m_NewItemReady = CheckNewItem();
}
//---------------------------------------------------------------------------
bool __fastcall  TDictionaryDialog::CheckNewItem()
{
	UnicodeString str = this->ButtonedEdit1->Text;
    return ListBox1->Items->IndexOf(str) == -1;
}

void __fastcall  TDictionaryDialog::CheckDigitKey(System::WideChar &Key)
{
	if ((Key < '0' || Key > '9') && Key != '\b')
    	Key = '\0';

}
void __fastcall TDictionaryDialog::LabeledEdit2KeyPress(TObject *Sender, System::WideChar &Key)
{
	CheckDigitKey(Key);
}
//---------------------------------------------------------------------------

void __fastcall TDictionaryDialog::LabeledEdit6KeyPress(TObject *Sender, System::WideChar &Key)
{
    CheckDigitKey(Key);
}
//---------------------------------------------------------------------------

void __fastcall TDictionaryDialog::LabeledEdit5KeyPress(TObject *Sender, System::WideChar &Key)
{
    CheckDigitKey(Key);
}
//---------------------------------------------------------------------------

void __fastcall TDictionaryDialog::LabeledEdit1Exit(TObject *Sender)
{
	try
    {
    	int value = StrToInt(LabeledEdit1->Text);
        CalcItemsCount();
    }
    catch(EConvertError& e)
    {
         ShowMessage(e.Message);
         LabeledEdit1->SetFocus();
    }
}
//---------------------------------------------------------------------------

void __fastcall TDictionaryDialog::LabeledEdit2Exit(TObject *Sender)
{
 	try
    {
    	int value = StrToInt(LabeledEdit2->Text);
        CalcItemsCount();
    }
    catch(EConvertError& e)
    {
         ShowMessage(e.Message);
         LabeledEdit2->SetFocus();
    }
}
//---------------------------------------------------------------------------

void __fastcall TDictionaryDialog::LabeledEdit6Exit(TObject *Sender)
{
	try
    {
    	int value = StrToInt(LabeledEdit6->Text);
        CalcItemsCount();
    }
    catch(EConvertError& e)
    {
         ShowMessage(e.Message);
         LabeledEdit6->SetFocus();
    }
}
//---------------------------------------------------------------------------

void __fastcall TDictionaryDialog::LabeledEdit5Exit(TObject *Sender)
{
	try
    {
    	int value = StrToInt(LabeledEdit5->Text);
        CalcItemsCount();
    }
    catch(EConvertError& e)
    {
         ShowMessage(e.Message);
         LabeledEdit5->SetFocus();
    }
}
//---------------------------------------------------------------------------
void	__fastcall  TDictionaryDialog::CalcItemsCount()
{
     TLFSemanticDictinaryItem* word = m_pDictionary->GetWordFromDictinary(ListBox1->ItemIndex);
     if (word != NULL && word->GetScanner() != NULL)
     {
        ILFScanner* scanner = word->GetScanner();
		int bw = StrToInt(this->LabeledEdit1->Text);
		int bh = StrToInt(this->LabeledEdit2->Text);

		int w = StrToInt(this->LabeledEdit6->Text);
		int h = StrToInt(this->LabeledEdit5->Text);

        scanner->SetBaseHeight(bh);
        scanner->SetBaseWidth(bw);

       for (int i = 0; i < scanner->GetParamsCount(); i++)
       {
            TLFParameter* p = scanner->GetParameter(i);
            for (int j = 0; j < ValueListEditor1->Strings->Count; j++)
            {
                AnsiString strName = p->GetPName();
                if (strName == ValueListEditor1->Keys[j+1])
                {
                    p->SetValue(StrToFloat(ValueListEditor1->Values[strName]));
                }
            }
       }

        scanner->Scan(w,h);

		this->Label1->Caption = IntToStr(scanner->GetFragmentsCount());
     }
     else
     	this->Label1->Caption = L"0";

}
void __fastcall TDictionaryDialog::ValueListEditor1Exit(TObject *Sender)
{
    CalcItemsCount();
}
//---------------------------------------------------------------------------


void __fastcall TDictionaryDialog::ValueListEditor1Validate(TObject *Sender, int ACol,
          int ARow, const UnicodeString KeyName, const UnicodeString KeyValue)
{
	try
    {
    	double v = StrToFloat(KeyValue);
    }
    catch(EConvertError& e)
    {
        ShowMessage(e.Message);
        ValueListEditor1->RestoreCurrentRow();
    }
}
//---------------------------------------------------------------------------

