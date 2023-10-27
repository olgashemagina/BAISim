//----------------------------------------------------------------------------
#ifndef DbExportFormH
#define DbExportFormH
//----------------------------------------------------------------------------
#include <System.hpp>
#include <Windows.hpp>
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <Graphics.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Controls.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <Vcl.AppEvnts.hpp>
#include <Vcl.CheckLst.hpp>
//----------------------------------------------------------------------------
class TDbExportDialog : public TForm
{
__published:        
	TButton *OKBtn;
	TButton *CancelBtn;
	TBevel *Bevel1;
    TLabel *Label1;
    TEdit *Edit1;
    TButton *Button1;
    TGroupBox *GroupBox1;
    TEdit *Edit2;
    TCheckBox *CheckBox3;
	TApplicationEvents *ApplicationEvents1;
	TLabel *Label5;
	TCheckListBox *CheckListBox1;
	TComboBox *ComboBox1;
	TLabel *Label6;
	TGroupBox *GroupBox2;
	TComboBox *ComboBox2;
	TGroupBox *GroupBox3;
	TComboBox *ComboBox4;
	TGroupBox *GroupBox4;
	TCheckBox *CheckBox1;
	TEdit *Edit3;
	TLabel *Label3;
	TCheckBox *CheckBox7;
	TGroupBox *GroupBox5;
	TCheckBox *CheckBox5;
	TLabel *Label4;
	TEdit *Edit4;
	TCheckBox *CheckBox2;
	TCheckBox *CheckBox6;
	TCheckBox *CheckBox4;
	TCheckBox *CheckBox8;
    void __fastcall Button1Click(TObject *Sender);
	void __fastcall ApplicationEvents1Idle(TObject *Sender, bool &Done);
private:
public:
	virtual __fastcall TDbExportDialog(TComponent* AOwner);
};
//----------------------------------------------------------------------------
extern PACKAGE TDbExportDialog *DbExportDialog;
//----------------------------------------------------------------------------
#endif    
