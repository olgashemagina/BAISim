//---------------------------------------------------------------------------
#ifndef PhLenzToolH
#define PhLenzToolH
//---------------------------------------------------------------------------
#include <System.SysUtils.hpp>
#include <System.Classes.hpp>
#include <vfw.h>
#include "PhImageTool.h"
class PACKAGE TPhLenzTool : public TPhImageTool
{
private:
    float               Zoom;
    int                 LenzSize;
    bool                Applied;
    HDRAWDIB            FDrawHandle;
    void                ApplyLenz(int X, int Y);

public:
    __fastcall TPhLenzTool(TComponent* Owner);
   virtual __fastcall ~TPhLenzTool();

    virtual void Draw(TCanvas* Canvas);
    virtual void MouseDown(int X, int Y, TMouseButton Button = mbLeft);
    virtual void MouseUp(int X, int Y, TMouseButton Button = mbLeft);
    virtual void MouseMove(int X, int Y, TShiftState Shift);
	virtual void Reset();
};
#endif
