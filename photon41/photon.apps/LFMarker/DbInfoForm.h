//----------------------------------------------------------------------------
#ifndef DbInfoFormH
#define DbInfoFormH
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
#include "DbLabeledImages.h"
#include <VCLTee.Chart.hpp>
#include <VCLTee.Series.hpp>
#include <VclTee.TeeGDIPlus.hpp>
#include <VCLTee.TeEngine.hpp>
#include <VCLTee.TeeProcs.hpp>
#include <Vcl.Grids.hpp>
#include <Vcl.ValEdit.hpp>

//----------------------------------------------------------------------------
class TDbInfoDialog : public TForm
{
__published:        
	TButton *OKBtn;
	TBevel *Bevel1;
        TLabel *Label1;
        TLabel *Label2;
        TLabel *labelTotalImages;
        TLabel *Label5;
        TLabel *Label6;
    TLabel *labelTotalXmlFiles;
    TLabel *labelTotalXmlItems;
	TBevel *Bevel2;
	TLabel *Label3;
	TLabel *labelClasses;
	TLabel *Label7;
	TLabel *labelDatabase;
	TValueListEditor *ValueListEditor1;
private:
public:
	virtual __fastcall TDbInfoDialog(TComponent* AOwner);
	void __fastcall ProgressHandler(int Progress, AnsiString& aComment);
};
//----------------------------------------------------------------------------
extern PACKAGE TDbInfoDialog *DbInfoDialog;
//----------------------------------------------------------------------------
#endif
