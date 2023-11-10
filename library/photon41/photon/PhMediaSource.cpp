//---------------------------------------------------------------------------

#include <vcl.h>

#pragma hdrstop

#include "PhMediaSource.h"
#include "FImage41.h"

#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//

static inline void ValidCtrCheck(TPhMediaSource *)
{
	new TPhMediaSource(NULL);
}
//---------------------------------------------------------------------------
__fastcall TPhMediaSource::TPhMediaSource(TComponent* Owner)
	: TComponent(Owner)
{
        m_pDisplay = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TPhMediaSource::SetDisplay(TPhCustomImage* display)
{
        m_pDisplay = display;
}

//---------------------------------------------------------------------------
namespace Phmediasource
{
	void __fastcall PACKAGE Register()
	{
		TComponentClass classes[1] = {__classid(TPhMediaSource)};
		RegisterComponents(L"Photon", classes, 0);
	}
}
//---------------------------------------------------------------------------
