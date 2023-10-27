//----------------------------------------------------------------------------
#ifndef DbConvertFormH
#define DbConvertFormH
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
//----------------------------------------------------------------------------
class TDbConvertDlg : public TForm
{
__published:        
	TButton *OKBtn;
	TButton *CancelBtn;
	TBevel *Bevel1;
    TCheckBox *CheckBox1;
    TLabel *Label1;
    TEdit *Edit1;
    TCheckBox *CheckBox2;
    TCheckBox *CheckBox3;
    TLabel *Label2;
    TComboBox *ComboBox1;
private:
public:
	virtual __fastcall TDbConvertDlg(TComponent* AOwner);
};
//----------------------------------------------------------------------------
extern PACKAGE TDbConvertDlg *DbConvertDlg;
//----------------------------------------------------------------------------
#endif    
