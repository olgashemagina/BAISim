//---------------------------------------------------------------------------

#ifndef PhVideoTrackBarH
#define PhVideoTrackBarH
//---------------------------------------------------------------------------
#include <System.SysUtils.hpp>
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
//---------------------------------------------------------------------------
class PACKAGE TPhVideoTrackBar : public TCustomControl
{
private:
	double m_pos;
	double m_duration;
	bool   m_mouse_down;
	int    m_progress;
protected:
	TNotifyEvent                m_Change;

	virtual void __fastcall SetEnabled(Boolean Value);

	void __fastcall SetPos(double value);
	void __fastcall SetDuration(double value);
	void __fastcall SetProgress(int value);

	virtual void __fastcall     Paint(void);
	DYNAMIC void __fastcall     Resize(void);
	DYNAMIC void __fastcall     MouseDown(TMouseButton Button,  TShiftState Shift, Integer X, Integer Y);
	DYNAMIC void __fastcall     MouseMove( TShiftState Shift, Integer X, Integer Y);
	DYNAMIC void __fastcall     MouseUp(TMouseButton Button,  TShiftState Shift, Integer X, Integer Y);

public:
	__fastcall TPhVideoTrackBar(TComponent* Owner);

	__property double pos = {read = m_pos, write = SetPos};
	__property double duration = {read = m_duration, write = SetDuration};
	__property int progress = {read = m_progress, write = SetProgress};
__published:

    __property Enabled;

	__property TNotifyEvent     OnChange = {read = m_Change, write = m_Change};
	__property OnMouseDown;
	__property OnMouseMove;
	__property OnMouseUp;
};
//---------------------------------------------------------------------------
#endif
