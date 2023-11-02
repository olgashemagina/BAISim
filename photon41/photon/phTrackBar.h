//---------------------------------------------------------------------------

#ifndef TrackBarExH
#define TrackBarExH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <ComCtrls.hpp>
#include <Controls.hpp>
//---------------------------------------------------------------------------
class PACKAGE TPhTrackBar : public TTrackBar
{
private:
protected:
public:
    __fastcall TPhTrackBar(TComponent* Owner);
__published:
    __property OnMouseUp;
	__property OnMouseMove;
    __property OnMouseDown;
};
//---------------------------------------------------------------------------
#endif
