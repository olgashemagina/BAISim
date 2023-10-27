//---------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "SystemOptionsForm.h"
#include "MainForm.h"
//---------------------------------------------------------------------
#pragma link "CSPIN"
#pragma resource "*.dfm"
TPagesDlg *PagesDlg;
//--------------------------------------------------------------------- 
__fastcall TPagesDlg::TPagesDlg(TComponent* AOwner)
	: TForm(AOwner)
{
}
//---------------------------------------------------------------------
void __fastcall TPagesDlg::ScannerShow(TObject *Sender)
{
    // получим число фрагментов
    CalcFragmentsCount();
}
//---------------------------------------------------------------------------
void __fastcall TPagesDlg::CalcFragmentsCount()
{
   TLFScanner* scr = Form1->Scanner;

   for (int i = 0; i < scr->GetParamsCount(); i++)
   {
        TLFParameter* p = scr->GetParameter(i);
        for (int j = 0; j < ValueListEditor1->Strings->Count; j++)
        {
            AnsiString strName = p->GetPName();
            if (strName == ValueListEditor1->Keys[j+1])
            {
                p->SetValue(StrToFloat(ValueListEditor1->Values[strName]));
            }
        }
   }
   int bw = 24;
   int bh = 24;
   try
   {
        bw = StrToInt(Edit2->Text);
        bh = StrToInt(Edit3->Text);
   }
   catch(EConvertError& e)
   {
        bw = 24;
        bh = 24;
        Edit2->Text = "24";
        Edit3->Text = "24";
   }
   scr->SetBaseWidth(bw);
   scr->SetBaseHeight(bh);
   scr->Scan(CSpinEdit1->Value, CSpinEdit2->Value);
   Label3->Caption = IntToStr(scr->GetFragmentsCount());
}

void __fastcall TPagesDlg::ValueListEditor1Validate(TObject *Sender,
      int ACol, int ARow, const AnsiString KeyName,
      const AnsiString KeyValue)
{
    CalcFragmentsCount();
}
//---------------------------------------------------------------------------

void __fastcall TPagesDlg::Edit2Exit(TObject *Sender)
{
     CalcFragmentsCount();
}
//---------------------------------------------------------------------------

void __fastcall TPagesDlg::Edit3Exit(TObject *Sender)
{
    CalcFragmentsCount();
}
//---------------------------------------------------------------------------

void __fastcall TPagesDlg::CheckBox3Click(TObject *Sender)
{
    if (CheckBox3->Checked)
    {
        Edit4->Enabled = true;
        CalcDetectorFragmentsCount();
    }
    else
    {
       Edit4->Enabled = false;
       Label10->Caption = "xxxx";
       Label4->Caption = "xxxx";
    }
}
//---------------------------------------------------------------------------
void __fastcall TPagesDlg::CalcDetectorFragmentsCount()
{
     if (!CheckBox3->Checked)
        return;

     ILFDetectEngine* engine = Form1->Engine;

     ILFScanner* scr = engine->GetScanner();
     if (scr)
     {
       for (int i = 0; i < scr->GetParamsCount(); i++)
       {
            TLFParameter* p = scr->GetParameter(i);
            for (int j = 0; j < ValueListEditor2->Strings->Count; j++)
            {
                AnsiString strName = p->GetPName();
                if (strName == ValueListEditor2->Keys[j+1])
                {
                    p->SetValue(StrToFloat(ValueListEditor2->Values[strName]));
                }
            }
       }

       //
       int width  = 320;

       try
       {
            width = StrToInt(Edit4->Text );
       }
       catch(EConvertError& e)
       {
         width = 320;
         Edit4->Text = IntToStr(width);
       }
       int height = 3*width/4;
       Label10->Caption =  IntToStr(height);

       scr->Scan(width, height);
       int c = 1;
       if (CheckBox4->Checked)
        c = 3;
       Label4->Caption = IntToStr(c*scr->GetFragmentsCount());
     }
}


void __fastcall TPagesDlg::FormShow(TObject *Sender)
{
    Edit4->Enabled = CheckBox3->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TPagesDlg::CheckBox4Click(TObject *Sender)
{
   CalcDetectorFragmentsCount();
}
//---------------------------------------------------------------------------

void __fastcall TPagesDlg::Edit4Exit(TObject *Sender)
{
    CalcDetectorFragmentsCount();
}
//---------------------------------------------------------------------------

void __fastcall TPagesDlg::ValueListEditor2Validate(TObject *Sender,
      int ACol, int ARow, const AnsiString KeyName,
      const AnsiString KeyValue)
{
   CalcDetectorFragmentsCount();
}
//---------------------------------------------------------------------------



