//---------------------------------------------------------------------------

#pragma hdrstop

#include "ProjectUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

TLFProject::TLFProject()
{
    m_strPath = "";
}
TLFProject::~TLFProject()
{

}

bool __fastcall TLFProject::CreateProject(const char* lpPath)
{
    return false;
}
bool __fastcall TLFProject::CloseProject()
{
    return false;
}
bool __fastcall TLFProject::OpenProject(const char* lpFileName)
{
    return false;
}
