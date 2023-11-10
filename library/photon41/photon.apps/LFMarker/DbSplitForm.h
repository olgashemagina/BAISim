//----------------------------------------------------------------------------
#ifndef DbSplitFormH
#define DbSplitFormH
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
#include <Vcl.Samples.Spin.hpp>
//----------------------------------------------------------------------------
class TDbSplitDlg : public TForm
{
__published:
	TButton *OKBtn;
	TButton *CancelBtn;
	TBevel *Bevel1;
	TEdit *Edit1;
	TEdit *Edit2;
	TLabel *Label1;
	TLabel *Label2;
	TSpinEdit *SpinEdit1;
	TLabel *Label3;
private:
public:
	virtual __fastcall TDbSplitDlg(TComponent* AOwner);
};
//----------------------------------------------------------------------------
extern PACKAGE TDbSplitDlg *DbSplitDlg;
//----------------------------------------------------------------------------
#endif    
