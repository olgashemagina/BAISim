//----------------------------------------------------------------------------
#ifndef dictinaryEditorH
#define dictinaryEditorH
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
#include "LF.h"
#include "PhImageMarkTool.h"
//----------------------------------------------------------------------------
class TdictinaryEditDlg : public TForm
{
__published:
	TButton *OKBtn;
	TButton *AddWordButton;
	TButton *DeleteWordButton;
	TButton *EditWordButton;
	TListBox *ListBox1;
	TButton *Button1;
	void __fastcall EditWordButtonClick(TObject *Sender);
	void __fastcall AddWordButtonClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall DeleteWordButtonClick(TObject *Sender);
private:
    TLFSemanticDictinary m_dictionary;
    TLFSemanticDictinary* __fastcall GetSemanticDictionary();
public:
	virtual __fastcall TdictinaryEditDlg(TComponent* AOwner);
    bool    __fastcall CreateDatabase();
    bool    __fastcall EditDatabase();

    __property TLFSemanticDictinary* Dictionary = {read = GetSemanticDictionary};
};
//----------------------------------------------------------------------------
extern PACKAGE TdictinaryEditDlg *dictinaryEditDlg;
//----------------------------------------------------------------------------
#endif
