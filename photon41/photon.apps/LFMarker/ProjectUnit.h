//---------------------------------------------------------------------------
#ifndef ProjectUnitH
#define ProjectUnitH
#include <vcl.h>
#include "_LF.h"

//---------------------------------------------------------------------------
class TLFProject
{
protected:
    String m_strPath;
    TLFDBLabeledImages 			m_database;
public:
    TLFProject();
    ~TLFProject();

    bool __fastcall CreateProject(const char* lpPath);
    bool __fastcall CloseProject();
    bool __fastcall OpenProject(const char* lpFileName);

};
#endif
