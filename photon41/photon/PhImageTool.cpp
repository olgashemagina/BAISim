//---------------------------------------------------------------------------

#include <vcl.h>

#pragma hdrstop

#include "PhImageTool.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//

static inline void ValidCtrCheck(TPhImageTool *)
{
//	new TPhImageTool(NULL);
}
//---------------------------------------------------------------------------
__fastcall TPhImageTool::TPhImageTool(TComponent* Owner)
	: TComponent(Owner)
{
	m_pImage = NULL;
	m_active = false;

    m_strToolName = L"NULL TOOL";

	m_BeforeActivate = NULL;
	m_AfterActivate = NULL;
	m_BeforeDeActivate = NULL;
	m_AfterDeActivate = NULL;
}
//
void __fastcall TPhImageTool::SetDisplay(TPhImage* display)
{
	if (m_pImage == display)
		return;
	if (display == NULL && m_pImage != NULL)
	{
	   //remove this from display list
	   m_pImage->RemovePhTool(this);
	   m_pImage = NULL;
	   return;
	}
	m_pImage = display;
	if (m_pImage != NULL)
		m_pImage->AddPhTool(this);
}
TPhImage* __fastcall TPhImageTool::GetDisplay(TPhImage* display)
{
	return   m_pImage;
}

void __fastcall TPhImageTool::SetActive(bool value)
{
   if (m_active == value)
	return;

   if (value)
   {
	  if (this->m_BeforeActivate != NULL)
		this->m_BeforeActivate(this);
   }
   else
   {
	  if (this->m_BeforeDeActivate != NULL)
		this->m_BeforeDeActivate(this);
   }

   this->m_active = value;

   if (!value)
   {
	  if (this->m_AfterActivate != NULL)
		this->m_AfterActivate(this);
   }
   else
   {
	  if (this->m_AfterDeActivate != NULL)
		this->m_AfterDeActivate(this);
   }
}
//---------------------------------------------------------------------------
// we dont need register this component in the components palette
namespace Phimagetool
{
/*	void __fastcall PACKAGE Register()
	{
		TComponentClass classes[1] = {__classid(TPhImageTool)};
		RegisterComponents(L"Photon", classes, 0);
	} */
}
//---------------------------------------------------------------------------
