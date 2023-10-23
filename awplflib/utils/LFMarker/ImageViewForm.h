//---------------------------------------------------------------------------

#ifndef ImageViewFormH
#define ImageViewFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Vcl.Buttons.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.AppEvnts.hpp>
#include "FImage41.h"
#include "_LF.h"
#include <Vcl.Samples.Spin.hpp>
//---------------------------------------------------------------------------
class TFragmentForm : public TForm
{
__published:	// IDE-managed Components
	TPanel *Panel1;
	TSpeedButton *SpeedButton1;
	TSpeedButton *SpeedButton2;
	TPanel *Panel2;
	TLabel *Label3;
	TLabel *Label4;
	TEdit *Edit1;
	TSpeedButton *SpeedButton3;
	TSpeedButton *SpeedButton4;
	TSpeedButton *SpeedButton5;
	TSpeedButton *SpeedButton6;
	TComboBox *ComboBox1;
	TApplicationEvents *ApplicationEvents1;
	TEdit *Edit2;
	TLabel *Label8;
	TSpeedButton *SpeedButton7;
	TPhImage *PhImage1;
	TLabel *Label7;
	TLabel *Label1;
	TComboBox *ComboBox2;
	TSpinEdit *SpinEdit1;
	void __fastcall SpeedButton1Click(TObject *Sender);
	void __fastcall SpeedButton2Click(TObject *Sender);
	void __fastcall CheckBox1Click(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall ComboBox1Change(TObject *Sender);
	void __fastcall ApplicationEvents1Idle(TObject *Sender, bool &Done);
	void __fastcall SpeedButton3Click(TObject *Sender);
	void __fastcall SpeedButton4Click(TObject *Sender);
	void __fastcall SpeedButton5Click(TObject *Sender);
	void __fastcall SpeedButton6Click(TObject *Sender);
	void __fastcall ComboBox1Exit(TObject *Sender);
	void __fastcall SpeedButton7Click(TObject *Sender);
	void __fastcall Edit2Change(TObject *Sender);
	void __fastcall ComboBox2Change(TObject *Sender);
	void __fastcall SpinEdit1Change(TObject *Sender);
private:	// User declarations
	TLFSemanticImageDescriptor* m_descr;
	void __fastcall UpdateTable();
public:		// User declarations
	__fastcall TFragmentForm(TComponent* Owner);
    void __fastcall DrawSelected();
    void __fastcall ChangeDictonary();
    __property TLFSemanticImageDescriptor* SDescriptor = {read = m_descr, write = m_descr};

};
//---------------------------------------------------------------------------
extern PACKAGE TFragmentForm *FragmentForm;
//---------------------------------------------------------------------------
#endif
