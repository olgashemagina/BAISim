//---------------------------------------------------------------------------
#ifndef mainH
#define mainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "Comm.h"
#include "cspin.h"
#include <stdio.h>
#include <Vcl.ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
  TButton *OpenPort;
    TButton *ClosePort;
    TButton *Exit;
    TCSpinEdit *PortIndex;
    TLabel *Label1;
    TEdit *Edit1;
    TButton *SendData;
    TMemo *Memo1;
    TCheckBox *ShowCommEvent;
    TComm *Comm1;
    void __fastcall OpenPortClick(TObject *Sender);
    void __fastcall ClosePortClick(TObject *Sender);
    void __fastcall PortIndexChange(TObject *Sender);
    void __fastcall SendDataClick(TObject *Sender);
    void __fastcall Comm1RxChar(TObject *Sender, DWORD Count);
    void __fastcall Comm1Break(TObject *Sender);
    void __fastcall Comm1Cts(TObject *Sender);
    void __fastcall Comm1Dsr(TObject *Sender);
    void __fastcall Comm1Error(TObject *Sender, int Errors);
    void __fastcall Comm1Ring(TObject *Sender);
    void __fastcall Comm1Rlsd(TObject *Sender);
    void __fastcall Comm1RxFlag(TObject *Sender);
    void __fastcall Comm1TxEmpty(TObject *Sender);
    void __fastcall Comm1ComStatEvent(TObject *Sender, DWORD Status,
          COMSTAT ComStat);
    void __fastcall Memo1MouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
    void __fastcall ExitClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TForm1(TComponent* Owner);
    AnsiString rs;
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
