//---------------------------------------------------------------------------

#ifndef PhZoomToRectToolH
#define PhZoomToRectToolH
#include <System.SysUtils.hpp>
#include <System.Classes.hpp>
#include "PhImageTool.h"
//---------------------------------------------------------------------------
class PACKAGE TPhZoomToRectTool : public TPhImageTool
{
private:
    TRect SelRect;
    void DrawSelRect();
    bool Pressed;
    int m_x;
    int m_y;
    void __fastcall SetVertexes(int x1, int x2, int y1, int y2);
public:
    __fastcall TPhZoomToRectTool(TComponent* Owner);
    virtual void Draw(TCanvas* Canvas);
    virtual void MouseDown(int X, int Y, TMouseButton Button = mbLeft);
    virtual void MouseUp(int X, int Y, TMouseButton Button = mbLeft);
    virtual void MouseMove(int X, int Y, TShiftState Shift);
    virtual void Reset();
};
#endif
