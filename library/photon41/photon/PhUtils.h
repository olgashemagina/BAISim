//---------------------------------------------------------------------------
#ifndef PhUtilsH
#define PhUtilsH
#include <System.hpp>
//---------------------------------------------------------------------------
typedef enum {copySkip, copyReplace, copyNewName} EPhCopyActioin;

UnicodeString MakeNewFileName(UnicodeString strFileName);
bool PhCopyFile(UnicodeString strFileName, UnicodeString strFolder, EPhCopyActioin action);
#endif
