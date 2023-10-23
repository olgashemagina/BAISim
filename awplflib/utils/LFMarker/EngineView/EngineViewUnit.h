//---------------------------------------------------------------------------

#ifndef EngineViewUnitH
#define EngineViewUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.Dialogs.hpp>
#include "_LF.h"
#include "Cube3D.h"
#include <Vcl.CheckLst.hpp>
//---------------------------------------------------------------------------
class TEngineViewForm : public TForm
{
__published:	// IDE-managed Components
	TPopupMenu *PopupMenu1;
	TMenuItem *LoadEngine1;
	TMenuItem *N1;
	TMenuItem *Exit1;
	TOpenDialog *OpenDialog1;
	TPanel *Panel1;
	TPanel *Panel2;
	TPaintBox *PaintBox1;
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label3;
	TLabel *Label4;
	TLabel *Label5;
	TLabel *Label6;
	TPanel *Panel3;
	TCheckListBox *CheckListBox1;
	TCheckListBox *CheckListBox2;
	TSplitter *Splitter1;
	TRadioGroup *RadioGroup1;
	TGroupBox *GroupBox1;
	TCheckBox *CheckBox1;
	void __fastcall Exit1Click(TObject *Sender);
	void __fastcall LoadEngine1Click(TObject *Sender);
	void __fastcall PaintBox1Paint(TObject *Sender);
	void __fastcall CheckListBox1Click(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall CheckListBox2Click(TObject *Sender);
	void __fastcall CheckListBox2ClickCheck(TObject *Sender);
	void __fastcall RadioGroup1Click(TObject *Sender);
	void __fastcall PaintBox1MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall PaintBox1MouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
	void __fastcall PaintBox1MouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall Panel1Resize(TObject *Sender);
	void __fastcall CheckBox1Click(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);

private:	// User declarations
	TLFDetectEngine* m_engine;

	T3DTransform 		t;

	bool 	m_mdown;
	int	    m_dx;
	int 	m_dy;
	int     m_currentFrame;

	void __fastcall DrawCube(TCanvas* cnv);


	void __fastcall DrawLayer0(TCanvas* cnv, awpRect* pRect = NULL);
	void __fastcall DrawLayer1(TCanvas* cnv, awpRect* pRect = NULL);
	void __fastcall DrawLayer2(TCanvas* cnv, awpRect* pRect = NULL);
	void __fastcall DrawLayer3(TCanvas* cnv, awpRect* pRect = NULL);

	void __fastcall SetEngine(TLFDetectEngine* e);
public:		// User declarations
	__fastcall TEngineViewForm(TComponent* Owner);
	__property TLFDetectEngine* engine = {read = m_engine, write = SetEngine};
	void __fastcall DrawEngine(awpRect* pRect = NULL);
};
//---------------------------------------------------------------------------
extern PACKAGE TEngineViewForm *EngineViewForm;
//---------------------------------------------------------------------------
#endif
