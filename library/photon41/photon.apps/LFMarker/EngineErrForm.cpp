//---------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "EngineErrForm.h"
#include "MainForm.h"
//---------------------------------------------------------------------
#pragma resource "*.dfm"
TEngineErrDlg *EngineErrDlg;
//---------------------------------------------------------------------
__fastcall TEngineErrDlg::TEngineErrDlg(TComponent* AOwner)
	: TForm(AOwner)
{
}
//---------------------------------------------------------------------

void __fastcall TEngineErrDlg::Button2Click(TObject *Sender)
{

  double thr = 0.5;
  thr = StrToFloat(Edit2->Text);
  double far1, ffar, frr, t;

  Label16->Visible = true;
  ProgressBar1->Visible = true;

  OKBtn->Enabled = false;
  Button2->Enabled = false;

  m_database.GetFarFrr(*m_engine, far1, frr, ffar, t, thr);

  Label9->Caption = FormatFloat("###.###", far1*100)+" %";
  Label11->Caption = FormatFloat("#.###E-00", ffar) + " %";
  Label13->Caption = FormatFloat("###.###", frr*100)+" %";
  Label15->Caption = FloatToStr(t) + " ms";


  Label16->Caption = "";
  ProgressBar1->Position = 0;

  Label16->Visible = false;
  ProgressBar1->Visible = false;

  Button2->Enabled = true;
  OKBtn->Enabled = true;


  Chart1->Series[0]->Clear();
  Chart1->Series[0]->Add(100*far1, "Far = " + FormatFloat("###.#", far1*100)+" %", clRed);
  Chart1->Series[0]->Add(100*frr, "Frr = " + FormatFloat("###.#", frr*100)+" %", clGreen);
}
//---------------------------------------------------------------------------
void TEngineErrDlg::SetEngine( TLFDetectEngine* engine)
{
	m_engine = engine;
}
void __fastcall TEngineErrDlg::FormShow(TObject *Sender)
{
    AnsiString str  = Form1->DirectoryListBox1->Directory;
    str += "\\";
    Edit1->Text = str;
    m_database.InitDB(str.c_str());
    m_database.SetProgress(Progress);
    Label6->Caption = IntToStr(m_database.GetImagesCount());
    Label7->Caption = IntToStr(m_database.GetItemsCount());

    Chart1->Series[0]->Clear();

  Label9->Caption = "*****";
  Label11->Caption= "*****";
  Label13->Caption = "*****";
  Label15->Caption = "*****";

}
//---------------------------------------------------------------------------
void Progress(const char* msg, int value)
{
	Application->ProcessMessages();
    EngineErrDlg->ProgressBar1->Position = value;
    EngineErrDlg->Label16->Caption = msg;
}

