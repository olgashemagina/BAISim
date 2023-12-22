//---------------------------------------------------------------------------

#ifndef SelectBuilderConfigFormH
#define SelectBuilderConfigFormH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Dialogs.hpp>
//---------------------------------------------------------------------------
class TSelectBuildConfForm : public TForm
{
__published:	// IDE-managed Components
	TRadioGroup *Choose;
	TEdit *Edit1;
	TButton *Button1;
	TButton *OKButton;
	TButton *CancelButton;
	TOpenDialog *OpenDialog1;
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall CancelButtonClick(TObject *Sender);
	void __fastcall ChooseClick(TObject *Sender);
	void __fastcall OKButtonClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
    UnicodeString m_configFilename;
	__fastcall TSelectBuildConfForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TSelectBuildConfForm *SelectBuildConfForm;
//---------------------------------------------------------------------------
#endif
