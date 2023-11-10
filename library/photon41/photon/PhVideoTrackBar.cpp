//---------------------------------------------------------------------------

#include <vcl.h>

#pragma hdrstop

#include "PhVideoTrackBar.h"
#pragma package(smart_init)
#define INFLATE_X -4
#define INFLATE_Y -10
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//

static inline void ValidCtrCheck(TPhVideoTrackBar *)
{
	new TPhVideoTrackBar(NULL);
}
//---------------------------------------------------------------------------
__fastcall TPhVideoTrackBar::TPhVideoTrackBar(TComponent* Owner)
	: TCustomControl(Owner)
{
	this->Align   = alClient;
	this->Height  = 24;
	m_pos = 0;
	m_duration = 0;
    m_Change = 0;
}

void __fastcall TPhVideoTrackBar::Resize(void)
{
	this->Height  = 24;
	Paint();
}
void __fastcall TPhVideoTrackBar::Paint(void)
{

	TRect rect = this->ClientRect;
	Canvas->Brush->Color = 0x00333333;

	Canvas->FillRect(rect);
	TColor oldBrushColor = this->Canvas->Brush->Color;
	TColor oldPenColor = this->Canvas->Pen->Color;
	this->Canvas->Brush->Color = clGray;
	this->Canvas->Pen->Color = clGray;

	rect.Inflate(INFLATE_X,INFLATE_Y);
	this->Canvas->Rectangle(rect);

	TRect prog_rect = rect;
	prog_rect.right = 0.01*m_progress*rect.Width();


	this->Canvas->Brush->Color = oldBrushColor;
	this->Canvas->Pen->Color = oldPenColor;

	this->Canvas->Brush->Color = clSilver;
	this->Canvas->Pen->Color = clSilver;

	this->Canvas->Rectangle(prog_rect);

	this->Canvas->Brush->Color = oldBrushColor;
	this->Canvas->Pen->Color = oldPenColor;


	 if (m_duration > 0)
	 {
		oldBrushColor = this->Canvas->Brush->Color;
		oldPenColor = this->Canvas->Pen->Color;
		this->Canvas->Brush->Color = clRed;
		this->Canvas->Pen->Color = clRed;
		TRect rect1 = rect;
		rect1.right = rect.Width()*m_pos / m_duration;
		this->Canvas->Rectangle(rect1);
		TRect rect2 = rect;
		rect2.Left  = rect.Width()*m_pos / m_duration;
		rect2.Right = rect2.Left + 12;
		rect2.Top -= 4;
		rect2.bottom += 4;
		if (this->Enabled) {
			this->Canvas->Ellipse(rect2);
			this->Canvas->Pen->Color = clWhite;
			this->Canvas->Pen->Width = 3;
			this->Canvas->Ellipse(rect2);
		}
		this->Canvas->Pen->Width = 1;
		this->Canvas->Brush->Color = oldBrushColor;
		this->Canvas->Pen->Color = oldPenColor;
	 }
}

void __fastcall     TPhVideoTrackBar::MouseDown(TMouseButton Button,  TShiftState Shift, Integer X, Integer Y)
{
   if (this->Enabled)
   {
	   m_mouse_down = true;
   }
   TCustomControl::MouseDown(Button, Shift,X,Y);
}
void __fastcall     TPhVideoTrackBar::MouseMove( TShiftState Shift, Integer X, Integer Y)
{
	if (m_mouse_down)
	{
		TRect rect = this->ClientRect;
		rect.Inflate(INFLATE_X,INFLATE_Y);
		pos = X*m_duration / rect.Width();
		Paint();
	}
	TCustomControl::MouseMove(Shift,X,Y);
}
void __fastcall     TPhVideoTrackBar::MouseUp(TMouseButton Button,  TShiftState Shift, Integer X, Integer Y)
{
   if (m_mouse_down)
   {
		TRect rect = this->ClientRect;
		rect.Inflate(INFLATE_X,INFLATE_Y);
		pos = X*m_duration / rect.Width();
		Paint();
   }
   m_mouse_down = false;
   TCustomControl::MouseUp(Button, Shift,X,Y);
}

void __fastcall TPhVideoTrackBar::SetPos(double value)
{

	if (value < 0 )
		m_pos = 0;
	if (value > m_duration)
		m_pos = m_duration;
	m_pos = value;
    Paint();
	if (Enabled && m_Change != NULL)
		m_Change(this);
}
void __fastcall TPhVideoTrackBar::SetDuration(double value)
{
	if (value < 0)
		return;
	m_duration = value;
	pos = 0;

}

void __fastcall TPhVideoTrackBar::SetProgress(int value)
{
	if (value < 0 || value > 100)
		return;
	m_progress = value;
	Paint();
}

void __fastcall TPhVideoTrackBar::SetEnabled(Boolean Value)
{
	TCustomControl::SetEnabled(Value);
	Paint();
}


//---------------------------------------------------------------------------
namespace Phvideotrackbar
{
	void __fastcall PACKAGE Register()
	{
		TComponentClass classes[1] = {__classid(TPhVideoTrackBar)};
		RegisterComponents(L"photon", classes, 0);
	}
}
//---------------------------------------------------------------------------
