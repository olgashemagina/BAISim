//---------------------------------------------------------------------------

#ifndef PhPaneToolH
#define PhPaneToolH
//---------------------------------------------------------------------------
#include <System.SysUtils.hpp>
#include <System.Classes.hpp>
#include "PhImageTool.h"
//---------------------------------------------------------------------------
class PACKAGE TPhPaneTool : public TPhImageTool
{
private:
protected:
	int  m_X;
	int  m_Y;
	bool m_Pressed;
public:
	__fastcall TPhPaneTool(TComponent* Owner);
	virtual void Draw(TCanvas* Canvas);
	virtual void MouseDown(int X, int Y, TMouseButton Button = mbLeft);
	virtual void MouseUp(int X, int Y, TMouseButton Button = mbLeft);
	virtual void MouseMove(int X, int Y, TShiftState Shift);
	virtual void Reset();
__published:
};
//---------------------------------------------------------------------------
#endif
