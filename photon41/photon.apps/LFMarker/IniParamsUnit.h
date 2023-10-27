//---------------------------------------------------------------------------
#ifndef IniParamsUnitH
#define IniParamsUnitH
//---------------------------------------------------------------------------
class TMarkerIniParams
{
protected:
	UnicodeString m_strLastPath;
	UnicodeString  m_UITheme;
	void __fastcall LoadParams();
	void __fastcall SaveParams();

	void __fastcall SetUITheme(const UnicodeString value);
	void __fastcall SetLastPath(const UnicodeString& value);
	UnicodeString __fastcall GetIniPath();
public:
	TMarkerIniParams();

	__property UnicodeString UITheme = {read = m_UITheme, write = SetUITheme};
	__property UnicodeString LastPath = {read = m_strLastPath, write = SetLastPath};
	__property UnicodeString IniPath = {read = GetIniPath};
};
#endif
