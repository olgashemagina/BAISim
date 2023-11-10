//----------------------------------------------------------------------------
#ifndef dictinaryItemEditorH
#define dictinaryItemEditorH
//----------------------------------------------------------------------------
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Buttons.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Graphics.hpp>
#include <System.Classes.hpp>
#include <System.SysUtils.hpp>
#include <Winapi.Windows.hpp>
#include <System.hpp>
#include <Vcl.Samples.Spin.hpp>
#include "LF.h"
#include "PhImageMarkTool.h"
#include <Vcl.ColorGrd.hpp>
//----------------------------------------------------------------------------
class TdictinaryItemDlg : public TForm
{
__published:
	TButton *OKBtn;
	TButton *CancelBtn;
	TBevel *Bevel1;
	TLabel *Label1;
	TEdit *Edit1;
	TLabel *Label2;
	TSpinEdit *SpinEdit1;
	TSpinEdit *SpinEdit2;
	TLabel *Label3;
	TLabel *Label4;
	TColorGrid *ColorGrid1;
	TPanel *Panel1;
	TLabel *Label5;
	TComboBox *ComboBox1;
	void __fastcall ColorGrid1Click(TObject *Sender);
private:
public:
	virtual __fastcall TdictinaryItemDlg(TComponent* AOwner);
        bool __fastcall EditItem(TLFSemanticDictinaryItem* item);
        bool __fastcall AddItem(TLFSemanticDictinaryItem* item);
};
//----------------------------------------------------------------------------
extern PACKAGE TdictinaryItemDlg *dictinaryItemDlg;
//----------------------------------------------------------------------------
#endif
