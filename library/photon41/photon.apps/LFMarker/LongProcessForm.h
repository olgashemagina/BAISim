//---------------------------------------------------------------------------

#ifndef LongProcessFormH
#define LongProcessFormH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>
//---------------------------------------------------------------------------
typedef enum {ptCopy, ptConvert, ptExport, ptSplit, ptAnalysis, ptClear, ptUpdate, ptNothing}EProcType;
class TLongProcDlg : public TForm
{
__published:	// IDE-managed Components
	TLabel *Label1;
	TProgressBar *ProgressBar1;
	TTimer *Timer1;
	TLabel *Label2;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall Timer1Timer(TObject *Sender);
private:	// User declarations
	EProcType m_procType;
public:		// User declarations
	__fastcall TLongProcDlg(TComponent* Owner);

	void __fastcall ProgressHandler(int Progress, AnsiString& aComment);
	__property EProcType ProcType = {read =m_procType, write = m_procType};
};
//---------------------------------------------------------------------------
extern PACKAGE TLongProcDlg *LongProcDlg;
//---------------------------------------------------------------------------
#endif
