//---------------------------------------------------------------------------

#ifndef PhMediaSourceH
#define PhMediaSourceH
//---------------------------------------------------------------------------
#include <System.SysUtils.hpp>
#include <System.Classes.hpp>
//---------------------------------------------------------------------------
class TPhCustomImage;
class PACKAGE TPhMediaSource : public TComponent
{
friend class TPhCustomImage;
private:
        TPhCustomImage* m_pDisplay;
protected:
        void __fastcall SetDisplay(TPhCustomImage* display);
public:
	__fastcall TPhMediaSource(TComponent* Owner);

__published:
};
//---------------------------------------------------------------------------
#endif
