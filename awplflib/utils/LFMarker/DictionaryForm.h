//---------------------------------------------------------------------------

#ifndef DictionaryFormH
#define DictionaryFormH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.ImgList.hpp>
#include <Vcl.Grids.hpp>
#include <Vcl.ValEdit.hpp>
#include <Vcl.AppEvnts.hpp>
#include "_LF.h"
#include <Vcl.Mask.hpp>
#include <System.ImageList.hpp>
//---------------------------------------------------------------------------
class TDictionaryDialog : public TForm
{
__published:	// IDE-managed Components
	TPanel *Panel1;
	TPanel *Panel2;
	TPanel *Panel3;
	TButton *Button1;
	TButton *Button2;
	TPanel *Panel4;
	TButtonedEdit *ButtonedEdit1;
	TListBox *ListBox1;
	TGroupBox *GroupBox1;
	TImageList *ImageList1;
	TGroupBox *GroupBox2;
	TGroupBox *GroupBox3;
	TGroupBox *GroupBox5;
	TLabeledEdit *LabeledEdit1;
	TLabeledEdit *LabeledEdit2;
	TCheckBox *CheckBox1;
	TValueListEditor *ValueListEditor1;
	TLabeledEdit *LabeledEdit5;
	TLabeledEdit *LabeledEdit6;
	TGroupBox *GroupBox4;
	TLabel *Label1;
	TLabel *Label2;
	TPanel *Panel5;
	TApplicationEvents *ApplicationEvents1;
	TLabel *Label3;
	TLabel *Label4;
	TMaskEdit *MaskEdit2;
	TMaskEdit *MaskEdit3;
	void __fastcall ButtonedEdit1RightButtonClick(TObject *Sender);
	void __fastcall ApplicationEvents1Idle(TObject *Sender, bool &Done);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall Button2Click(TObject *Sender);
	void __fastcall ListBox1Click(TObject *Sender);
	void __fastcall LabeledEdit1KeyPress(TObject *Sender, System::WideChar &Key);
	void __fastcall ButtonedEdit1Change(TObject *Sender);
	void __fastcall LabeledEdit2KeyPress(TObject *Sender, System::WideChar &Key);
	void __fastcall LabeledEdit6KeyPress(TObject *Sender, System::WideChar &Key);
	void __fastcall LabeledEdit5KeyPress(TObject *Sender, System::WideChar &Key);
	void __fastcall LabeledEdit1Exit(TObject *Sender);
	void __fastcall LabeledEdit2Exit(TObject *Sender);
	void __fastcall LabeledEdit6Exit(TObject *Sender);
	void __fastcall LabeledEdit5Exit(TObject *Sender);
	void __fastcall ValueListEditor1Exit(TObject *Sender);
	void __fastcall ValueListEditor1Validate(TObject *Sender, int ACol, int ARow, const UnicodeString KeyName,
          const UnicodeString KeyValue);

private:	// User declarations
	TLFSemanticDictinary* m_pDictionary;
    bool 				  m_NewItemReady;

    bool 	__fastcall 		CheckNewItem();
    void    __fastcall      CheckDigitKey(System::WideChar &Key);
    void	__fastcall 		CalcItemsCount();
public:		// User declarations
	__fastcall TDictionaryDialog(TComponent* Owner);
    __property TLFSemanticDictinary* Dictionary = {read = m_pDictionary, write = m_pDictionary};
};
//---------------------------------------------------------------------------
extern PACKAGE TDictionaryDialog *DictionaryDialog;
//---------------------------------------------------------------------------
#endif
