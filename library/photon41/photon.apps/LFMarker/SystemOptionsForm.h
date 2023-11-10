//----------------------------------------------------------------------------
#ifndef SystemOptionsFormH
#define SystemOptionsFormH
//----------------------------------------------------------------------------
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include <Buttons.hpp>
#include <StdCtrls.hpp>
#include <Controls.hpp>
#include <Forms.hpp>
#include <Graphics.hpp>
#include <Classes.hpp>
#include <SysUtils.hpp>
#include <Windows.hpp>
#include <System.hpp>
#include "CSPIN.h"
#include <Grids.hpp>
#include <ValEdit.hpp>
#include <VCLTee.Chart.hpp>
#include <VCLTee.Series.hpp>
#include <VclTee.TeeGDIPlus.hpp>
#include <VCLTee.TeEngine.hpp>
#include <VCLTee.TeeProcs.hpp>
//----------------------------------------------------------------------------
class TPagesDlg : public TForm
{
__published:
	TPanel *Panel1;
	TPanel *Panel2;
	TPageControl *PageControl1;
    TTabSheet *Scanner;
	TTabSheet *TabSheet2;
	TButton *OKBtn;
	TButton *CancelBtn;
    TGroupBox *GroupBox2;
    TCSpinEdit *CSpinEdit1;
    TCSpinEdit *CSpinEdit2;
    TLabel *Label1;
    TLabel *Label2;
    TGroupBox *GroupBox3;
    TLabel *Label3;
    TValueListEditor *ValueListEditor1;
    TValueListEditor *ValueListEditor2;
    TGroupBox *GroupBox4;
    TLabel *Label4;
    TLabel *Label5;
    TEdit *Edit2;
    TLabel *Label6;
    TEdit *Edit3;
    TGroupBox *GroupBox5;
    TCheckBox *CheckBox2;
    TCheckBox *CheckBox3;
    TLabel *Label11;
    TEdit *Edit4;
    TGroupBox *GroupBox6;
    TLabel *Label8;
    TLabel *Label9;
    TGroupBox *GroupBox7;
    TCheckBox *CheckBox4;
    TLabel *Label7;
    TLabel *Label10;
	TTabSheet *TabSheet1;
	TGroupBox *GroupBox1;
	TEdit *Edit1;
	TCheckBox *CheckBox1;
	TLabel *Label12;
	TLabel *Label13;
	TLabeledEdit *LabeledEdit1;
	TLabeledEdit *LabeledEdit2;
	TGroupBox *GroupBox8;
	TCheckBox *CheckBox5;
    void __fastcall ScannerShow(TObject *Sender);
    void __fastcall ValueListEditor1Validate(TObject *Sender, int ACol,
          int ARow, const AnsiString KeyName, const AnsiString KeyValue);
    void __fastcall Edit2Exit(TObject *Sender);
    void __fastcall Edit3Exit(TObject *Sender);
    void __fastcall CheckBox3Click(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall CheckBox4Click(TObject *Sender);
    void __fastcall Edit4Exit(TObject *Sender);
    void __fastcall ValueListEditor2Validate(TObject *Sender, int ACol,
          int ARow, const AnsiString KeyName, const AnsiString KeyValue);
private:
    void __fastcall CalcFragmentsCount();
    void __fastcall CalcDetectorFragmentsCount();
 //   ILFScanner* GetScanner(bool internal);
public:
	virtual __fastcall TPagesDlg(TComponent* AOwner);
};
//----------------------------------------------------------------------------
extern PACKAGE TPagesDlg *PagesDlg;
//----------------------------------------------------------------------------
#endif    
