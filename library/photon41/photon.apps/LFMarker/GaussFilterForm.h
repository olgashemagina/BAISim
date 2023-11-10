//----------------------------------------------------------------------------
#ifndef GaussFilterFormH
#define GaussFilterFormH
//----------------------------------------------------------------------------
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
#include <StdCtrls.hpp>
#include <Controls.hpp>
#include <Forms.hpp>
#include <Graphics.hpp>
#include <Classes.hpp>
#include <SysUtils.hpp>
#include <Windows.hpp>
#include <System.hpp>
#include "cspin.h"
//----------------------------------------------------------------------------
class TGaussFilterDlg : public TForm
{
__published:
	TButton *OKBtn;
	TButton *CancelBtn;
	TBevel *Bevel1;
	TLabel *Label1;
	TCSpinEdit *CSpinEdit1;
	TLabel *Label2;
	TLabel *Label3;
	TLabel *Label4;
	void __fastcall CSpinEdit1Change(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
private:
public:
	virtual __fastcall TGaussFilterDlg(TComponent* AOwner);
};
//----------------------------------------------------------------------------
extern PACKAGE TGaussFilterDlg *GaussFilterDlg;
//----------------------------------------------------------------------------
#endif    
