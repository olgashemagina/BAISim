//----------------------------------------------------------------------------
#ifndef ResizeFormH
#define ResizeFormH
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
#include <Vcl.Samples.Spin.hpp>
//----------------------------------------------------------------------------
class TResizeDlg : public TForm
{
__published:
	TButton *OKBtn;
	TButton *CancelBtn;
	TBevel *Bevel1;
	TCheckBox *CheckBox1;
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label3;
	TRadioButton *RadioButton1;
	TRadioButton *RadioButton2;
	TSpinEdit *SpinEdit1;
	TSpinEdit *SpinEdit2;
	TGroupBox *GroupBox1;
	TGroupBox *GroupBox2;
	TLabel *currentHeightLabel;
	TLabel *newWidthLabel;
	TLabel *newHeightLabel;
	TLabel *currenWidthLabel;
	TCheckBox *CheckBox2;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall RadioButton1Click(TObject *Sender);
	void __fastcall RadioButton2Click(TObject *Sender);
	void __fastcall SpinEdit1Change(TObject *Sender);
	void __fastcall SpinEdit2Change(TObject *Sender);
	void __fastcall CheckBox1Click(TObject *Sender);
private:
	int m_currentWidth;
	int m_currentHeight;
	int m_newWidth;
	int m_newHeight;

	bool m_locked;
	void __fastcall SetCurrentWidth(int value);
	void __fastcall SetCurrentHeight(int value);
	void __fastcall SetNewWidth(int value);
	void __fastcall SetNewHeight(int value);

	void __fastcall CalculateSizes();

public:
	virtual __fastcall TResizeDlg(TComponent* AOwner);

	__property int CurrentWidth = {read = m_currentWidth, write = SetCurrentWidth};
	__property int CurrentHeight = {read = m_currentHeight, write = SetCurrentHeight};
	__property int NewWidth = {read = m_newWidth, write = SetNewWidth};
	__property int NewHeight = {read = m_newHeight, write = SetNewHeight};
};
//----------------------------------------------------------------------------
extern PACKAGE TResizeDlg *ResizeDlg;
//----------------------------------------------------------------------------
#endif    
