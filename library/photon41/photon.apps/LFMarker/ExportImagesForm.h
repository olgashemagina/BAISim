//----------------------------------------------------------------------------
#ifndef ExportImagesFormH
#define ExportImagesFormH
//----------------------------------------------------------------------------
#include <vcl\System.hpp>
#include <vcl\Windows.hpp>
#include <vcl\SysUtils.hpp>
#include <vcl\Classes.hpp>
#include <vcl\Graphics.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\Controls.hpp>
#include <vcl\Buttons.hpp>
#include <vcl\ExtCtrls.hpp>
#include "CSPIN.h"
//----------------------------------------------------------------------------
class TOKBottomDlg : public TForm
{
__published:        
	TButton *OKBtn;
	TButton *CancelBtn;
    TGroupBox *GroupBox1;
    TCheckBox *CheckBox1;
    TLabel *Label1;
    TComboBox *ComboBox1;
    TLabel *Label3;
    TEdit *Edit2;
    TButton *Button1;
    TCheckBox *CheckBox2;
    TLabel *Label2;
    TCSpinEdit *CSpinEdit1;
    TCheckBox *CheckBox3;
private:
public:
	virtual __fastcall TOKBottomDlg(TComponent* AOwner);
};
//----------------------------------------------------------------------------
extern PACKAGE TOKBottomDlg *OKBottomDlg;
//----------------------------------------------------------------------------
#endif    
