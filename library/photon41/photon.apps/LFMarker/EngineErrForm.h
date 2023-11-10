//----------------------------------------------------------------------------
#ifndef EngineErrFormH
#define EngineErrFormH
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
#include <ComCtrls.hpp>
#include <Dialogs.hpp>
#include "DbLabeledImages.h"
#include <VCLTee.ArrowCha.hpp>
#include <VCLTee.Chart.hpp>
#include <VCLTee.Series.hpp>
#include <VclTee.TeeGDIPlus.hpp>
#include <VCLTee.TeEngine.hpp>
#include <VCLTee.TeeProcs.hpp>
void Progress(const char* msg, int value);
//----------------------------------------------------------------------------
class TEngineErrDlg : public TForm
{
__published:
	TButton *OKBtn;
	TBevel *Bevel1;
	TLabel *Label1;
	TEdit *Edit1;
	TChart *Chart1;
	TLabel *Label2;
	TEdit *Edit2;
	TLabel *Label3;
	TBarSeries *Series1;
	TGroupBox *GroupBox1;
	TLabel *Label4;
	TLabel *Label5;
	TLabel *Label6;
	TLabel *Label7;
	TLabel *Label8;
	TLabel *Label9;
	TLabel *Label10;
	TLabel *Label11;
	TLabel *Label12;
	TLabel *Label13;
	TLabel *Label14;
	TLabel *Label15;
	TButton *Button2;
	TProgressBar *ProgressBar1;
    TOpenDialog *OpenDialog1;
	TLabel *Label16;
	void __fastcall Button2Click(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
private:
    TLFDBLabeledImages m_database;
	TLFDetectEngine*   m_engine;
public:
	virtual __fastcall TEngineErrDlg(TComponent* AOwner);
    void SetEngine( TLFDetectEngine* engine);
};
//----------------------------------------------------------------------------
extern PACKAGE TEngineErrDlg *EngineErrDlg;
//----------------------------------------------------------------------------
#endif    
