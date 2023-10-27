//---------------------------------------------------------------------------
#ifndef PhPaletteH
#define PhPaletteH
//---------------------------------------------------------------------------
#include <System.SysUtils.hpp>
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
/*
	Palettes:
	-Grayscale
	-Bluered
	-Extendet spectum
	-Ocean
	-Glow
	-Extende spectrum 3.03
	-Traffic Light
	-False Colors
*/

//---------------------------------------------------------------------------
typedef enum {phpalUnknown, phpalGrayscale, phpalBluered, phpalOcean, phpalGlow, phpalTraffic, phpalFalseColors, phpalSpectum, phpalSpectrum2} EPhPalette;
class PACKAGE TPhPalette : public TCustomControl
{
public:
	struct SPhColorTriple
	{
		unsigned char m_red;
		unsigned char m_green;
		unsigned char m_blue;
	};
private:
	void*  m_vimage;
	void*  m_himage;
	double m_min;
	double m_max;
	TColor m_color;
	TColor m_textColor;
	bool   m_ticksVisible;
	EPhPalette m_paletteType;
    SPhColorTriple m_palette[256];

	int __fastcall PaletteRight();
	int __fastcall PaletteBottom(TCanvas* c);
	int __fastcall TextWidth();
	void __fastcall CreatePalette(void* image);
protected:
	void __fastcall SetColor(TColor color);
	void __fastcall SetTextColor(TColor color);
	void __fastcall DrawVTicks(TCanvas* c);
	void __fastcall DrawHTicks(TCanvas* c);
	void __fastcall SetMinValue(double value);
	void __fastcall SetMaxValue(double value);
	void __fastcall SetTicksVisible(bool value);
	void __fastcall SetPaletteType(EPhPalette value);
	virtual void __fastcall     Paint(void);
public:
	__fastcall TPhPalette(TComponent* Owner);
	void* __fastcall ApplyPalette(void* image);
//	__property SColorTriple
__published:
	__property Align;
	__property PopupMenu;

	__property 	TColor Color = {read = m_color, write = SetColor};
	__property 	TColor TextColor = {read = m_textColor, write = SetTextColor};
	__property  double MinValue = {read = m_min, write = SetMinValue};
	__property  double MaxValue = {read = m_max, write = SetMaxValue};
	__property bool TicksVisible = {read = m_ticksVisible, write = SetTicksVisible};
	__property EPhPalette PaletteType = {read = m_paletteType, write = SetPaletteType};
};
//---------------------------------------------------------------------------
#endif
