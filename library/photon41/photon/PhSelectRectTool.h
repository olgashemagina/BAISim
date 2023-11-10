//---------------------------------------------------------------------------
#ifndef PhSelectRectToolH
#define PhSelectRectToolH
#include <System.SysUtils.hpp>
#include <System.Classes.hpp>
#include "PhImageTool.h"
//---------------------------------------------------------------------------
// select rect tool
class PACKAGE TPhSelRectTool : public TPhImageTool
{
private:
    void DrawSelRect();
	bool Pressed;
    int  vertex;
    int  m_x;
    int  m_y;
    TRect m_SelRect;
    void __fastcall SetVertexes(int x1, int x2, int y1, int y2);
public:
    __fastcall TPhSelRectTool(TComponent* Owner);
    virtual void Draw(TCanvas* Canvas);
    virtual void MouseDown(int X, int Y, TMouseButton Button = mbLeft);
    virtual void MouseUp(int X, int Y, TMouseButton Button = mbLeft);
    virtual void MouseMove(int X, int Y, TShiftState Shift);
    virtual void Reset();

    __property TRect SelectRect = {read = m_SelRect};
};
//---------------------------------------------------------------------------
#endif
