//---------------------------------------------------------------------------

#ifndef PhTriangleToolH
#define PhTriangleToolH
//---------------------------------------------------------------------------
#include "PhImageTool.h"
class PACKAGE TPhTriangleTool : public TPhImageTool
{
private:
	TPoint m_sp;
	TPoint m_mp;
	TPoint m_ep;
	int    m_state;
	void DrawTriangle();
protected:
	double __fastcall GetAngle();
public:
	__fastcall TPhTriangleTool(TComponent* Owner);
   virtual __fastcall ~TPhTriangleTool();

	virtual void Draw(TCanvas* Canvas);
	virtual void MouseDown(int X, int Y, TMouseButton Button = mbLeft);
	virtual void MouseUp(int X, int Y, TMouseButton Button = mbLeft);
	virtual void MouseMove(int X, int Y, TShiftState Shift);
	virtual void Reset();

	__property double Angle = {read = GetAngle};
	__property TPoint StartPoint = {read = m_sp};
	__property TPoint EndPoint   = {read = m_ep};
	__property TPoint MiddlePoint = {read= m_mp};
};
#endif
