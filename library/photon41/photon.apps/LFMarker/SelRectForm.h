//---------------------------------------------------------------------------

#ifndef SelRectFormH
#define SelRectFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TRectToolForm : public TForm
{
__published:	// IDE-managed Components
	TComboBox *ComboBox1;
	TComboBox *ComboBox2;
	TComboBox *ComboBox3;
	TComboBox *ComboBox4;
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label3;
	TLabel *Label4;
	TCheckBox *CheckBox1;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall ComboBox1Change(TObject *Sender);
	void __fastcall ComboBox3Change(TObject *Sender);
	void __fastcall ComboBox2Change(TObject *Sender);
	void __fastcall ComboBox4Change(TObject *Sender);
	void __fastcall CheckBox1Click(TObject *Sender);
	void __fastcall ComboBox3Exit(TObject *Sender);
private:	// User declarations
	void __fastcall UpdateDetectedItem();
public:		// User declarations
        __fastcall TRectToolForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TRectToolForm *RectToolForm;
//---------------------------------------------------------------------------
#endif
