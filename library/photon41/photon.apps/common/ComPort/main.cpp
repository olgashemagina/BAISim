//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "main.h"
//---------------------------------------------------------------------------
//#pragma package(smart_init)
#pragma link "Comm"
#pragma link "cspin"
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TForm1::OpenPortClick(TObject *Sender)
{
	if (!Comm1->Enabled())
	{
		Comm1->Open();
		Comm1->SetRTSState(true);
		Comm1->SetDTRState(false);
		OpenPort->Enabled=!Comm1->Enabled();
		ClosePort->Enabled=Comm1->Enabled();
		SendData->Enabled=Comm1->Enabled();
        if (Comm1->Enabled()) Memo1->Lines->Add("Open "+Comm1->DeviceName+" OK");
    }
	else Memo1->Lines->Add("Port has already been opened!");
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ClosePortClick(TObject *Sender)
{
    if (Comm1->Enabled())
    {
		Comm1->Close();
        OpenPort->Enabled=!Comm1->Enabled();
        ClosePort->Enabled=Comm1->Enabled();
        SendData->Enabled=Comm1->Enabled();
		if (!Comm1->Enabled()) Memo1->Lines->Add("Close "+Comm1->DeviceName+" OK");
    }
    else Memo1->Lines->Add("Port has not been opened!");
}
//---------------------------------------------------------------------------
void __fastcall TForm1::PortIndexChange(TObject *Sender)
{
	if (!Comm1->Enabled()) Comm1->DeviceName=L"Com"+IntToStr((int)PortIndex->Value);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::SendDataClick(TObject *Sender)
{
	char wb[256] ;
    int i ;
    if (Comm1->Enabled())
	{
		AnsiString str = Edit1->Text.c_str();
		sprintf(wb,"%s",str.c_str());
        i = Comm1->Write(wb, Edit1->Text.Length()+1);
		if (ShowCommEvent->Checked) Memo1->Lines->Add("Write out: "+IntToStr(i));
	}
    else Memo1->Lines->Add("Open Port f1rst!");
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Comm1RxChar(TObject *Sender, DWORD Count)
{
	int i , Bytes ;
	char rb[5000], ts[5000];
	memset(rb, 0, sizeof(rb));
	memset(ts, 0, sizeof(ts));
	if (ShowCommEvent->Checked) Memo1->Lines->Add("Before Read : " + IntToStr(Comm1->InQueCount()));
	Bytes = Comm1->Read(rb, Count);

	if (Bytes>0)
	{
		if (ShowCommEvent->Checked) Memo1->Lines->Add("Received character bytes : "+IntToStr(Bytes)) ;
		for (i=0;i<Bytes;i++)
		{
			if ((rb[i]==0))
			  { ; }
			 else if (rb[i]=='\n' || rb[i] == 13 || rb[i] == '#')
				{ Memo1->Lines->Add(rs); rs="" ;}
			  else
			  {
				sprintf(ts,"%c",rb[i]); rs=rs+ts;
			  }
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Comm1Break(TObject *Sender)
{
    if (ShowCommEvent->Checked) Memo1->Lines->Add("Break  ") ;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Comm1Cts(TObject *Sender)
{
    if (ShowCommEvent->Checked) Memo1->Lines->Add("CTS  ") ;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Comm1Dsr(TObject *Sender)
{
    if (ShowCommEvent->Checked) Memo1->Lines->Add("DSR  ") ;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Comm1Error(TObject *Sender, int Errors)
{
    if (ShowCommEvent->Checked) Memo1->Lines->Add("ERROR  ") ;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Comm1Ring(TObject *Sender)
{
    if (ShowCommEvent->Checked) Memo1->Lines->Add("RING  ") ;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Comm1Rlsd(TObject *Sender)
{
    if (ShowCommEvent->Checked) Memo1->Lines->Add("RLSD  ") ;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Comm1RxFlag(TObject *Sender)
{
	if (ShowCommEvent->Checked) Memo1->Lines->Add("RxFlag  ") ;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Comm1TxEmpty(TObject *Sender)
{
    if (ShowCommEvent->Checked) Memo1->Lines->Add("TxEmpty  ") ;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Comm1ComStatEvent(TObject *Sender, DWORD Status,
      COMSTAT ComStat)
{
    // you can process all event here , if you do so ,
    // then dont process all others events: OnBreak,OnCts,.....
	if (ShowCommEvent->Checked) Memo1->Lines->Add("ComStatEvent Status : "+IntToStr((int)Status)) ;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Memo1MouseUp(TObject *Sender, TMouseButton Button,
      TShiftState Shift, int X, int Y)
{
    if (Button==mbRight)
        Memo1->Lines->Clear();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ExitClick(TObject *Sender)
{
    if (Comm1->Enabled())
        Comm1->Close();
    Close();
}
//---------------------------------------------------------------------------

