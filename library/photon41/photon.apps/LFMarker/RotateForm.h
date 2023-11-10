//----------------------------------------------------------------------------
#ifndef RotateFormH
#define RotateFormH
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
#include <ComCtrls.hpp>
#include "CSPIN.h"
#include "FImage41.h"
//----------------------------------------------------------------------------
class TRotateDlg : public TForm
{
__published:        
	TButton *OKBtn;
	TButton *CancelBtn;
	TBevel *Bevel1;
        TLabel *Label1;
        TLabel *Label2;
        TCSpinEdit *CSpinEdit1;
	TPhImage *PhImage1;
        void __fastcall FormShow(TObject *Sender);
        void __fastcall CSpinEdit1Change(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
private:
    awpImage* m_image;
public:
	virtual __fastcall TRotateDlg(TComponent* AOwner);
};
//----------------------------------------------------------------------------
extern PACKAGE TRotateDlg *RotateDlg;
//----------------------------------------------------------------------------
#endif    
