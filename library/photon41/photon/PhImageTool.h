//---------------------------------------------------------------------------

#ifndef PhImageToolH
#define PhImageToolH
//---------------------------------------------------------------------------
#include <System.SysUtils.hpp>
#include <System.Classes.hpp>
#include "FImage41.h"
//---------------------------------------------------------------------------
class PACKAGE TPhImageTool : public TComponent
{
friend class TPhImage;
private:
	TNotifyEvent    m_BeforeActivate;
	TNotifyEvent    m_AfterActivate;
	TNotifyEvent    m_BeforeDeActivate;
	TNotifyEvent    m_AfterDeActivate;
protected:
	UnicodeString   m_strToolName;
	bool            m_active;
	TPhImage* m_pImage;

	void __fastcall SetActive(bool value);
	void __fastcall SetDisplay(TPhImage* display);
	TPhImage* __fastcall GetDisplay(TPhImage* display);
public:
	__fastcall TPhImageTool(TComponent* Owner);

	virtual void Draw(TCanvas* Canvas) = 0;
	virtual void MouseDown(int X, int Y, TMouseButton Button = mbLeft) = 0;
	virtual void MouseUp(int X, int Y, TMouseButton Button = mbLeft)   = 0;
	virtual void MouseMove(int X, int Y, TShiftState Shift) = 0;
	virtual void Reset() = 0;

	__property bool Active = {read = m_active};
	__property UnicodeString ToolName = {read = m_strToolName};
__published:
	__property TPhImage* PhImage = {read = GetDisplay, write = SetDisplay};

	__property TNotifyEvent     BeforeActivate = {read = m_BeforeActivate, write = m_BeforeActivate};
	__property TNotifyEvent     AfterActivate = {read = m_AfterActivate, write = m_AfterActivate};
	__property TNotifyEvent     BeforeDeActivate = {read = m_BeforeDeActivate, write = m_BeforeDeActivate};
	__property TNotifyEvent     AfterDeActivate = {read = m_AfterDeActivate, write = m_AfterDeActivate};
};
//---------------------------------------------------------------------------
#endif
