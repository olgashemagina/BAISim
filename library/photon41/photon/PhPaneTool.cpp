//---------------------------------------------------------------------------

#include <vcl.h>

#pragma hdrstop

#include "PhPaneTool.h"
#pragma link "PhImageTool"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//

static inline void ValidCtrCheck(TPhPaneTool *)
{
	new TPhPaneTool(NULL);
}
//---------------------------------------------------------------------------
__fastcall TPhPaneTool::TPhPaneTool(TComponent* Owner)
	: TPhImageTool(Owner)
{
	m_X = 0;
	m_Y = 0;
	m_Pressed = false;
    m_strToolName = L"ZOOM/PANE";
}

void TPhPaneTool::Draw(TCanvas* Canvas)
{
// dummy
}
void TPhPaneTool::MouseDown(int X, int Y, TMouseButton Button)
{
   if ( m_pImage == NULL )
	   return;
   if ( Button != mbLeft )
	   return;

   m_X      = X;
   m_Y      = Y;
   m_Pressed = true;
}

void TPhPaneTool::MouseUp(int X, int Y, TMouseButton Button)
{
  if ( m_pImage == NULL )
		return;
   if ( Button != mbLeft )
	   return;

   m_X = 0;
   m_Y = 0;
   m_Pressed = false;
}
void TPhPaneTool::MouseMove(int X, int Y, TShiftState Shift)
{
   if (m_pImage == NULL)
		return;

   if (m_Pressed)
	  m_pImage->MoveBy( m_X-X, m_Y-Y );

   m_X = X;
   m_Y = Y;
}
void TPhPaneTool::Reset()
{
 //dummy
}
//---------------------------------------------------------------------------
namespace Phpanetool
{
	void __fastcall PACKAGE Register()
	{
		TComponentClass classes[1] = {__classid(TPhPaneTool)};
		RegisterComponents(L"Photon", classes, 0);
	}
}
//---------------------------------------------------------------------------
