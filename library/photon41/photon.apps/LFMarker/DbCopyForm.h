//----------------------------------------------------------------------------
#ifndef DbCopyFormH
#define DbCopyFormH
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
#include <Vcl.FileCtrl.hpp>
//----------------------------------------------------------------------------
class TDbCopyDlg : public TForm
{
__published:        
	TButton *OKBtn;
	TButton *CancelBtn;
	TBevel *Bevel1;
    TLabel *Label1;
    TEdit *Edit1;
    TCheckBox *CheckBox1;
    TCheckBox *CheckBox2;
    TCheckBox *CheckBox5;
	TDirectoryListBox *DirectoryListBox1;
	TDriveComboBox *DriveComboBox1;
  	void __fastcall FormShow(TObject *Sender);
	void __fastcall DirectoryListBox1Change(TObject *Sender);
	void __fastcall Edit1Change(TObject *Sender);
private:
public:
	virtual __fastcall TDbCopyDlg(TComponent* AOwner);
};
//----------------------------------------------------------------------------
extern PACKAGE TDbCopyDlg *DbCopyDlg;
//----------------------------------------------------------------------------
#endif    
