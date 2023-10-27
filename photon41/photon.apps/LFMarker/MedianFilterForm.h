//----------------------------------------------------------------------------
#ifndef MedianFilterFormH
#define MedianFilterFormH
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
class TMedianFilterDlg : public TForm
{
__published:
	TButton *OKBtn;
	TButton *CancelBtn;
	TBevel *Bevel2;
	TLabel *Label1;
	TCSpinEdit *CSpinEdit1;
private:
public:
	virtual __fastcall TMedianFilterDlg(TComponent* AOwner);
};
//----------------------------------------------------------------------------
extern PACKAGE TMedianFilterDlg *MedianFilterDlg;
//----------------------------------------------------------------------------
#endif    
