//---------------------------------------------------------------------------
#ifndef PhRulerToolH
#define PhRulerToolH
#include "PhImageTool.h"
class PACKAGE TPhRulerTool : public TPhImageTool
{
private:
    TPoint m_sp;
    TPoint m_ep;
    bool   m_down;
    void DrawRuler();
protected:
    double __fastcall GetLenght();
public:
    __fastcall TPhRulerTool(TComponent* Owner);
   virtual __fastcall ~TPhRulerTool();

    virtual void Draw(TCanvas* Canvas);
    virtual void MouseDown(int X, int Y, TMouseButton Button = mbLeft);
    virtual void MouseUp(int X, int Y, TMouseButton Button = mbLeft);
    virtual void MouseMove(int X, int Y, TShiftState Shift);
	virtual void Reset();

	__property double Length = {read = GetLenght};
	__property TPoint StartPoint = {read = m_sp};
    __property TPoint EndPoint   = {read = m_ep};
};


//---------------------------------------------------------------------------
#endif
