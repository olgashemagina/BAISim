//---------------------------------------------------------------------------

#pragma hdrstop
#include "tifflibio.h"
#include "ExportRaster.h"

//---------------------------------------------------------------------------

void __fastcall TTiffDecoder::Init()
{
    tif = TIFFOpen(FFileName.c_str(), "r");
    if ( tif == NULL )
        throw Exception("Cannot open file");
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &FNumScanLines);
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &FWidth);
    TIFFGetField(tif, TIFFTAG_PLANARCONFIG, &planar);
    TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &nsamples);
    TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bits_per_sample);
    buf = NULL;
	tiff_buf_size = TIFFScanlineSize(tif);
	buf = new char[tiff_buf_size];

	FBufferSize = 3*FWidth;
	FScanBuffer = new char[FBufferSize];
}

void __fastcall TTiffDecoder::ReadScanLine( int ScanLine )
{
	if( planar != PLANARCONFIG_CONTIG)
		throw Exception( "This tiff type is not supported" );

	if ( TIFFReadScanline(tif, buf, ScanLine)== -1 )
		throw Exception( "Reading file error" );

	if (nsamples == 3 && bits_per_sample == 8)
	{
	  memcpy(FScanBuffer, buf, tiff_buf_size);
	  rgb_to_bgr( FScanBuffer, FWidth );
	}
	else if (nsamples == 1 && bits_per_sample == 16)
	{
		 //
		 WORD* w = (WORD*)buf;
		 for (int i = 0; i < FWidth; i++)
		 {
			FScanBuffer[3*i] = (AWPBYTE)(255*w[i] / 4096);
			FScanBuffer[3*i+1] = (AWPBYTE)(255*w[i] / 4096);
			FScanBuffer[3*i+2] = (AWPBYTE)(255*w[i] / 4096);
		 }
	}
	else if (nsamples == 1 && bits_per_sample == 8)
	{
		 char* w = (char*)buf;
		 for (int i = 0; i < FWidth; i++)
		 {
			FScanBuffer[3*i] = w[i];
			FScanBuffer[3*i+1] = w[i];
			FScanBuffer[3*i+2] = w[i];
		 }
	}


//    for ( int i = 0, j = 0; j < FBufferSize; i += 4, j += 3 )
//    {
//            *( FScanBuffer + j ) = *( buf + i + 3 );
//            *( FScanBuffer + j + 1 ) = *( buf + i + 2 );
//            *( FScanBuffer + j + 2 ) = *( buf + i + 1 );
//        }
//     }
//     else if ( planar == PLANARCONFIG_SEPARATE && nsamples == 4 );
}

void __fastcall TTiffDecoder::Close()
{

    if (buf != NULL)
        delete [] buf;
	TIFFClose(tif);
}
