//---------------------------------------------------------------------------

#ifndef VerInfoUnitH
#define VerInfoUnitH
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <Controls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
typedef enum  {vtCompanyName, vtFileDescription, vtFileVersion,
    vtInternalName,
    vtLegalCopyright, vtLegalTradeMark, vtOriginalFileName,
    vtProductName, vtProductVersion, vtComments} EVersionInfoType;
class TVersionInfo : public TComponent
{
private:
	UnicodeString FVersionInfo[vtComments + 1];
protected:
    UnicodeString  __fastcall GetCompanyName();
    UnicodeString  __fastcall GetFileDescription();
    UnicodeString  __fastcall GetFileVersion();
    UnicodeString  __fastcall GetInternalName();
    UnicodeString  __fastcall GetLegalCopyright();
    UnicodeString  __fastcall GetLegalTradeMark();
    UnicodeString  __fastcall GetOriginalFileName();
    UnicodeString  __fastcall GetProductName();
    UnicodeString  __fastcall GetProductVersion();
    UnicodeString  __fastcall GetComments();

    virtual  UnicodeString GetVersionInfo( EVersionInfoType VersionType);
    virtual void  SetVersionInfo();
public:
   __fastcall TVersionInfo(TComponent* Owner);
__published:

    __property UnicodeString CompanyName={read  = GetCompanyName};
    __property UnicodeString FileDescription={read = GetFileDescription};
    __property UnicodeString FileVersion={read =GetFileVersion};
    __property UnicodeString InternalName={read =GetInternalName};
    __property UnicodeString LegalCopyright={read =GetLegalCopyright};
    __property UnicodeString LegalTradeMark={read =GetLegalTradeMark};
    __property UnicodeString OriginalFileName={read =GetOriginalFileName};
    __property UnicodeString ProductName={read =GetProductName};
    __property UnicodeString ProductVersion={read =GetProductVersion};
    __property UnicodeString Comments={read =GetComments};
};
#endif
