//---------------------------------------------------------------------------

#ifndef pnglibioH
#define pnglibioH
//---------------------------------------------------------------------------

#ifdef __cplusplus
        extern "C" {
#endif
#include "png.h"
#ifdef __cplusplus
        }
#endif


class TPngDecoder : public TImageDecoder
{
protected:
    png_byte    pbSig[8];
    png_structp png_ptr;
    png_infop   info_ptr;
    png_color_16    *pBackground;
    
    FILE    *pfFile;
    int iBitDepth;
    int iColorType;
    png_uint_32 ulRowBytes;
    int i;
    png_bytepp  row_pointers;
    virtual void __fastcall ReadScanLine(int ScanLine);
public:
    TPngDecoder(const AnsiString& FileName):TImageDecoder(FileName){
        png_ptr = NULL;
        info_ptr = NULL;
        pBackground=NULL;

        pfFile=NULL;
        iBitDepth=0;
        iColorType=0;
        ulRowBytes=0;
        i=0;
        row_pointers=NULL;

    }
    virtual void __fastcall Init();
    virtual void __fastcall Close();
};

#endif
