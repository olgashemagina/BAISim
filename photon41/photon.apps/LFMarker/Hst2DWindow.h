//---------------------------------------------------------------------------

#ifndef Hst2DWindowH
#define Hst2DWindowH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "FImage.h"
#include <Vcl.Dialogs.hpp>
#include <Vcl.Samples.Spin.hpp>
#include <System.Actions.hpp>
#include <Vcl.ActnList.hpp>
#include <Vcl.Menus.hpp>

#include "segmentModel.h"


//---------------------------------------------------------------------------
class THsv2DForm : public TForm
{
__published:	// IDE-managed Components
	TFImage *FImage1;
	TButton *LoadButton;
	TButton *SaveButton;
	TButton *ApplyButton;
	TOpenDialog *OpenDialog1;
	TButton *NewButton;
	TSaveDialog *SaveDialog1;
	TSpinEdit *SpinEdit1;
	TLabel *Label1;
	TCheckBox *CheckBox1;
	TCheckBox *CheckBox2;
	TButton *Button1;
	TButton *Button2;
	TActionList *ActionList1;
	TAction *hsNewModelAction;
	TAction *hsSaveModelAction;
	TAction *hsLoadModelAction;
	TAction *hsBuildModelAction;
	TAction *hsEstErrorAction;
	TAction *hsPredictAction;
	TAction *hsAddSampleAction;
	TPopupMenu *PopupMenu1;
	TAction *viewSourceHSTAction;
	TAction *viewSourceCondHSTAction;
	TMenuItem *N1;
	TMenuItem *N2;
	TAction *viewOutsideAction;
	TMenuItem *N3;
	TAction *viewCondOutsideAction;
	TMenuItem *N4;
	TAction *viewPredictorAction;
	TMenuItem *N5;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall CheckBox2Click(TObject *Sender);
	void __fastcall hsNewModelActionExecute(TObject *Sender);
	void __fastcall hsSaveModelActionExecute(TObject *Sender);
	void __fastcall hsSaveModelActionUpdate(TObject *Sender);
	void __fastcall hsLoadModelActionExecute(TObject *Sender);
	void __fastcall hsBuildModelActionExecute(TObject *Sender);
	void __fastcall hsBuildModelActionUpdate(TObject *Sender);
	void __fastcall hsEstErrorActionExecute(TObject *Sender);
	void __fastcall hsEstErrorActionUpdate(TObject *Sender);
	void __fastcall hsPredictActionExecute(TObject *Sender);
	void __fastcall hsPredictActionUpdate(TObject *Sender);
	void __fastcall FormHide(TObject *Sender);
	void __fastcall viewSourceHSTActionExecute(TObject *Sender);
	void __fastcall viewSourceHSTActionUpdate(TObject *Sender);
	void __fastcall viewSourceCondHSTActionExecute(TObject *Sender);
	void __fastcall viewSourceCondHSTActionUpdate(TObject *Sender);
	void __fastcall viewOutsideActionExecute(TObject *Sender);
	void __fastcall viewOutsideActionUpdate(TObject *Sender);
	void __fastcall viewCondOutsideActionExecute(TObject *Sender);
	void __fastcall viewCondOutsideActionUpdate(TObject *Sender);
	void __fastcall viewPredictorActionExecute(TObject *Sender);
	void __fastcall viewPredictorActionUpdate(TObject *Sender);
	void __fastcall SpinEdit1Change(TObject *Sender);
private:	// User declarations
    // изображение для показа.
	TLFImage  m_2Dhst;
   	// изображение для обработки.
    awpImage* m_src;
    // модель.
    THSSegmentModel m_model;

    void __fastcall RenederSource();
    void __fastcall RnderSourceCond();
    void __fastcall RenderOutsideSource();
    void __fastcall RenderOutsideCond();
    void __fastcall RenderPredictor();
    void __fastcall UpdatePicture();

public:		// User declarations
	__fastcall THsv2DForm(TComponent* Owner);
    void ProcessImage();
    void __fastcall SetImage(awpImage* img);
};
//---------------------------------------------------------------------------
extern PACKAGE THsv2DForm *Hsv2DForm;
//---------------------------------------------------------------------------
#endif
