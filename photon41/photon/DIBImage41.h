//---------------------------------------------------------------------------
#ifndef DIBImage41H
#define DIBImage41H

#include <Graphics.hpp>
#include "awpipl.h"
//---------------------------------------------------------------------------
DECLARE_HANDLE(HDIB);

//---------------------------------------------------------------------------
class  TDIBImage : public TGraphic
{
private:
    BITMAPINFO                  m_DIBInfo;
    _RTL_CRITICAL_SECTION       m_Lock;
    HANDLE                      m_DIBPixels;
    unsigned char               *m_OpenedPixels;
    void* __fastcall            GetScanline(int Row);
protected:
    virtual int __fastcall      GetHeight(void);
    virtual int __fastcall      GetWidth(void);
    virtual void __fastcall     SetHeight(int in_Height);
    virtual void __fastcall     SetWidth(int in_Width);

	virtual void __fastcall     Changed(System::TObject* Sender);
	virtual bool __fastcall     GetEmpty(void);
	virtual HPALETTE __fastcall GetPalette(void);
	virtual void __fastcall     ReadData(Classes::TStream* Stream);
	virtual void __fastcall     SetPalette(HPALETTE Value);
	virtual void __fastcall     WriteData(Classes::TStream* Stream);

public:
    __fastcall                  TDIBImage();
    __fastcall virtual          ~TDIBImage();

    virtual void __fastcall     Assign(Classes::TPersistent* Source);

	virtual void __fastcall     Draw(TCanvas* ACanvas, const Types::TRect &Rect);

	virtual void __fastcall     LoadFromFile(const UnicodeString Filename);
	virtual void __fastcall     SaveToFile(const UnicodeString Filename);
	virtual void __fastcall     LoadFromStream(Classes::TStream* Stream);
	virtual void __fastcall     SaveToStream(Classes::TStream* Stream);
	virtual void __fastcall 	LoadFromClipboardFormat(System::Word AFormat, NativeUInt AData, HPALETTE APalette);
	virtual void __fastcall 	SaveToClipboardFormat(System::Word &AFormat, NativeUInt &AData, HPALETTE &APalette);
    void __fastcall             Clear();
    void __fastcall             GetAWPImage( awpImage **in_Image );
    void __fastcall             SetAWPImage( awpImage* pImage );

    unsigned char*              OpenPixels();
    void __fastcall             ClosePixels();

    void __fastcall 			FlipVertical();
    void __fastcall 			FlipHorizontal();

    __property BITMAPINFO       DIBInfo            = {read = m_DIBInfo, write = m_DIBInfo};
    __property int              Height             = {read = GetHeight, write = SetHeight};
    __property int              Width              = {read = GetWidth, write = SetWidth};
	__property void*            ScanLine[int Row]  = {read = GetScanline};
    __property bool             IsEmpty            = {read = GetEmpty};
};

//---------------------------------------------------------------------------
#endif // DIBImageH
