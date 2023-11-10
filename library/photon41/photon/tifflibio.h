#ifndef tifflibioH
#define tifflibioH

#include "ImportRaster.h"// <-- import TImageDecoder interface
#ifdef __cplusplus
        extern "C" {
#endif
#include "tiffio.h"
#ifdef __cplusplus
        }
#endif
//class TTiffDecoder ---------------------------------------------
//Декодер растровых файлов в формате tiff, tif
class TTiffDecoder : public TImageDecoder
{
protected:
    TIFF* tif;
    char* buf;
	uint32 tiff_buf_size;
    uint16 planar;
    uint16 nsamples;
    uint32 depth;
    uint16 bits_per_sample;
    virtual void __fastcall ReadScanLine(int ScanLine);
public:
    TTiffDecoder(const AnsiString& FileName):TImageDecoder(FileName){}
    virtual void __fastcall Init();
    virtual void __fastcall Close();
};
#endif
