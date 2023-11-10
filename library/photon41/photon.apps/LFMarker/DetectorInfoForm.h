//---------------------------------------------------------------------------

#ifndef DetectorInfoFormH
#define DetectorInfoFormH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>

#include <VCLTee.Chart.hpp>
#include <VCLTee.Series.hpp>
#include <VclTee.TeeGDIPlus.hpp>
#include <VCLTee.TeEngine.hpp>
#include <VCLTee.TeeProcs.hpp>
//-----------------------------

#include "FImage41.h"
//---------------------------------------------------------------------------
class TDetectorForm : public TForm
{
__published:	// IDE-managed Components
	TPanel *Panel1;
	TPanel *Panel2;
	TLabel *Label1;
	TComboBox *ComboBox1;
	TLabel *Label2;
	TComboBox *ComboBox2;
	TLabel *Label3;
	TLabel *Label4;
	TLabel *Label5;
	TLabel *Label6;
	TCheckBox *CheckBox1;
	TCheckBox *CheckBox2;
	TLabel *Label7;
	TLabel *Label8;
	TPhImage *PhImage1;
	TChart *Chart1;
	TBarSeries *Series1;
	TButton *Button1;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall ComboBox1Change(TObject *Sender);
	void __fastcall ComboBox2Change(TObject *Sender);
	void __fastcall CheckBox1Click(TObject *Sender);
	void __fastcall CheckBox2Click(TObject *Sender);
	void __fastcall Button1Click(TObject *Sender);
private:	// User declarations
	void __fastcall DoDetectorInfo();
public:		// User declarations
	__fastcall TDetectorForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TDetectorForm *DetectorForm;
//---------------------------------------------------------------------------
#endif
