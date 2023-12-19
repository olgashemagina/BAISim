//---------------------------------------------------------------------------

#ifndef BuildDetectorOptionsFormH
#define BuildDetectorOptionsFormH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.ComCtrls.hpp>
#include "cspin.h"
#include "tinyxml.h"
#include <Vcl.Dialogs.hpp>
#include <Vcl.Samples.Spin.hpp>
//---------------------------------------------------------------------------
class TCSBuildOptions : public TForm
{
__published:	// IDE-managed Components
	TEdit *pos;
	TButton *Sel_pos;
	TLabel *Label1;
	TLabel *Label2;
	TButton *Sel_bgr;
	TEdit *bgr;
	TEdit *neg;
	TButton *Sel_neg;
	TLabel *Label3;
	TPanel *Panel1;
	TPanel *Panel2;
	TPageControl *PageControl1;
	TTabSheet *DB_page;
	TButton *CancelButton;
	TButton *NextButton;
	TButton *BackButton;
	TTabSheet *Scanner_page;
	TLabel *Label4;
	TLabel *Label5;
	TCSpinEdit *H_CSpinEdit;
	TCSpinEdit *CSpinEdit2;
	TLabel *Label6;
	TCSpinEdit *CSpinEdit1;
	TCSpinEdit *CSpinEdit4;
	TCSpinEdit *CSpinEdit5;
	TLabel *Label7;
	TLabel *Label8;
	TLabel *Label9;
	TLabel *Label10;
	TLabel *Label11;
	TComboBox *ComboBox1;
	TTabSheet *Learning_page;
	TLabel *Label12;
	TLabel *Label13;
	TLabel *Label14;
	TLabel *Label15;
	TLabel *Label16;
	TLabel *Label17;
	TCSpinEdit *CSpinEdit3;
	TCSpinEdit *CSpinEdit6;
	TCSpinEdit *CSpinEdit7;
	TCSpinEdit *CSpinEdit8;
	TCSpinEdit *CSpinEdit9;
	TTabSheet *Features_page;
	TRadioGroup *RadioGroup1;
	TTabSheet *Detector_page;
	TLabel *Label18;
	TEdit *Edit1;
	TLabel *Label19;
	TEdit *Edit2;
	TSaveDialog *SaveDialog1;
	TSpinEdit *SpinEdit1;
	void __fastcall Sel_bgrClick(TObject *Sender);
	void __fastcall negClick(TObject *Sender);
	void __fastcall DatabaseClick(TObject *Sender);
	void __fastcall NextButtonClick(TObject *Sender);
	void __fastcall BackButtonClick(TObject *Sender);
	void __fastcall CancelButtonClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall PageControl1Change(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TCSBuildOptions(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TCSBuildOptions *CSBuildOptions;
//---------------------------------------------------------------------------
#endif
