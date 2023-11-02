//---------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "ResizeForm.h"
//---------------------------------------------------------------------
#pragma resource "*.dfm"
TResizeDlg *ResizeDlg;
//--------------------------------------------------------------------- 
__fastcall TResizeDlg::TResizeDlg(TComponent* AOwner)
	: TForm(AOwner)
{
	m_locked = false;
}
//---------------------------------------------------------------------
void __fastcall TResizeDlg::SetCurrentWidth(int value)
{
   this->m_currentWidth = value;
   this->currenWidthLabel->Caption = L"width = " + IntToStr(this->m_currentWidth);
}

void __fastcall TResizeDlg::SetCurrentHeight(int value)
{
	this->m_currentHeight = value;
	this->currentHeightLabel->Caption = L"height = " + IntToStr(this->m_currentHeight);
}

void __fastcall TResizeDlg::SetNewWidth(int value)
{
	this->m_newWidth = value;
	this->newWidthLabel->Caption = L"width = " + IntToStr(this->m_newWidth);
}

void __fastcall TResizeDlg::SetNewHeight(int value)
{
	this->m_newHeight = value;
	this->newHeightLabel->Caption = L"height = " + IntToStr(this->m_newHeight);
}
void __fastcall TResizeDlg::FormShow(TObject *Sender)
{
   CalculateSizes();
}
//---------------------------------------------------------------------------
void __fastcall TResizeDlg::CalculateSizes()
{
	if (RadioButton1->Checked)
	{
		int w;
		int h;
		w = SpinEdit1->Value*m_currentWidth  / 100;
		h = SpinEdit2->Value*m_currentHeight / 100;
		this->NewWidth = w;
		this->NewHeight = h;
	}
	else
	{
		this->NewWidth = SpinEdit1->Value;
		this->NewHeight = SpinEdit2->Value;
	}
}

void __fastcall TResizeDlg::RadioButton1Click(TObject *Sender)
{
	m_locked = true;
	SpinEdit1->MaxValue = 500;
	SpinEdit2->MaxValue = 500;
	SpinEdit1->Value = 100*(double)NewWidth / (double)CurrentWidth;
	SpinEdit2->Value = 100*(double)NewHeight / (double)CurrentHeight;
	m_locked = false;
}
//---------------------------------------------------------------------------

void __fastcall TResizeDlg::RadioButton2Click(TObject *Sender)
{
	m_locked = true;
	SpinEdit1->MaxValue = 5000;
	SpinEdit2->MaxValue = 5000;
	SpinEdit1->Value = NewWidth;
	SpinEdit2->Value = NewHeight;
	m_locked = false;
}
//---------------------------------------------------------------------------

void __fastcall TResizeDlg::SpinEdit1Change(TObject *Sender)
{
	if(m_locked)
		return;
	if (CheckBox1->Checked)
	{
		if (RadioButton1->Checked)
		{
		   SpinEdit2->Value = SpinEdit1->Value;
		}
		else
		{
		   double ar = (double)m_currentWidth / (double)m_currentHeight;
		   m_locked = true;
		   SpinEdit2->Value = int(SpinEdit1->Value / ar);
		   m_locked = false;
		}
	}
	else
	{

	}
	CalculateSizes();
}
//---------------------------------------------------------------------------

void __fastcall TResizeDlg::SpinEdit2Change(TObject *Sender)
{
	if(m_locked)
		return;

	if (CheckBox1->Checked)
	{
		if (RadioButton1->Checked)
		{
		   SpinEdit1->Value = SpinEdit2->Value;
		}
		else
		{
		   double ar = (double)m_currentWidth / (double)m_currentHeight;
		   m_locked = true;
		   SpinEdit1->Value = int(SpinEdit2->Value * ar);
		   m_locked = false;
		}
	}
	else
	{

	}

	CalculateSizes();
}
//---------------------------------------------------------------------------

void __fastcall TResizeDlg::CheckBox1Click(TObject *Sender)
{
	m_locked = true;
	if (CheckBox1->Checked)
	{
		if (RadioButton1->Checked)
		{
			SpinEdit1->Value = 100;
			SpinEdit2->Value = 100;
		}
		else
		{
			SpinEdit1->Value = this->m_currentWidth;
			SpinEdit2->Value = this->m_currentHeight;
		}
	}
	m_locked = false;
    CalculateSizes();
}
//---------------------------------------------------------------------------

