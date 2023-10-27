//---------------------------------------------------------------------------

#include <vcl.h>

#pragma hdrstop

#include "PhTrackBar.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//

static inline void ValidCtrCheck(TPhTrackBar *)
{
    new TPhTrackBar(NULL);
}
//---------------------------------------------------------------------------
__fastcall TPhTrackBar::TPhTrackBar(TComponent* Owner)
    : TTrackBar(Owner)
{
}
//---------------------------------------------------------------------------
namespace Phtrackbar
{
    void __fastcall PACKAGE Register()
    {

		TComponentClass classes[1] = {__classid(TPhTrackBar)};
		RegisterComponents(L"Photon", classes, 0);
    }
}
//---------------------------------------------------------------------------
 