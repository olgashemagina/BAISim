//---------------------------------------------------------------------------
#pragma hdrstop
#include <stdio.h>
#include "ExportRaster.h"
#include "tiffio.h"
#define Z_PREFIX
#include "png.h"
//---------------------------------------------------------------------------

void rgb_to_bgr(BYTE *ptr,int count)
{
    int i;
    BYTE tmp;
    for(i = 0; i < count; i++, ptr += 3)
    {
        tmp     = *ptr;
        *ptr    = *(ptr+2);
        *(ptr+2)= tmp;
    }
};



void saveToTiff( char* filename, TDIBImage* FBitmap){
    uint32 w = 0;
    uint32 h = 0;
    BYTE* iraster = NULL;
    tstrip_t strip;
    tsize_t stripsize;
    DWORD row;
    DWORD	i;
    DWORD rowsperstrip = 1;
    TIFF    *tif = NULL;

  //  rgb_to_bgr((BYTE*)pImg->pPixels, pImg->sSizeX*pImg->sSizeY);

    TIFFSetErrorHandler(NULL );
    TIFFSetWarningHandler(NULL );

    w = FBitmap->Width;
    h = FBitmap->Height;
    tif = TIFFOpen(filename, "wH");
    if (!tif)
        throw Exception( "Can't open file" );

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH,  w);	
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, h);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC,  PHOTOMETRIC_RGB);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 8);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_LZW);
    TIFFSetField(tif, TIFFTAG_THRESHHOLDING, THRESHHOLD_HALFTONE);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 3);
    TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, 
        rowsperstrip = TIFFDefaultStripSize(tif, rowsperstrip));
    TIFFSetField(tif, TIFFTAG_RESOLUTIONUNIT, RESUNIT_NONE);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);

    iraster = new BYTE [ w*h*3*sizeof (BYTE)];
    if (!iraster)
        throw Exception ( "Can't allocate memory" );

    memset(iraster,0x00,w*h*3*sizeof(BYTE));
    BYTE* ptr = NULL;
    BYTE* begin = NULL;
//    int LineSize = TIFFScanlineSize(tif);
    begin = iraster;
    FBitmap->OpenPixels();
    for( i = 0; i < h; i++ )
    {
        ptr = (BYTE*)FBitmap->ScanLine[i];
        memcpy ( begin, ptr, w*3 );
        rgb_to_bgr( begin, w );
        begin +=  w*3;
    }
    FBitmap->ClosePixels();
    strip = 0;
    stripsize = TIFFStripSize(tif);
    for (row=0; row<h; row += rowsperstrip)
    {
        if (TIFFWriteEncodedStrip(tif, strip, iraster+row*w*3, stripsize) < 0)
        break;
        strip++;
    }
    delete[] iraster;
    TIFFClose(tif);
};

void saveToPng( char* filename, TDIBImage* FBitmap )
{
    png_structp         png_ptr = NULL;
    png_infop           info_ptr = NULL;
//    WORD                iWidth = 0;
    WORD                iHeight = 0;
    
    FILE        *pfFile=NULL;
    png_uint_32         i;
    png_bytepp          row_pointers=NULL;
    if(!filename)
        throw Exception ( "No such filename or directory" );

//    rgb_to_bgr((BYTE*)pImg->pPixels, pImg->sSizeX*pImg->sSizeY);

//    iWidth = FBitmap->Width;
    iHeight = FBitmap->Height;
    
    pfFile=fopen(filename,"wb");
    if(pfFile == NULL)
        throw Exception ( "Can't open file" );
        
        // create the two png(-info) structures
        
        png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,NULL,
        NULL, NULL);
    if (!png_ptr)
        throw Exception ( "Not enough memory" );

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        png_destroy_write_struct(&png_ptr, NULL);
        throw Exception ( "Not enough memory" );
    }
    if (setjmp(png_jmpbuf(png_ptr)))
    {   
        if(row_pointers)
            free(row_pointers);
        png_destroy_read_struct(&png_ptr, &info_ptr,NULL);
        if(pfFile) fclose(pfFile);
        throw Exception ( "Error!" );
    }
    
    // initialize the png structure
    png_init_io(png_ptr, pfFile);
    png_set_IHDR(
        png_ptr,
        info_ptr, 
        FBitmap->Width,// width
        FBitmap->Height,//height
        8,//bit_depth
        PNG_COLOR_TYPE_RGB,//PNG_COLOR_TYPE_???
        PNG_INTERLACE_NONE,//PNG_INTERLACE_???? 
        PNG_COMPRESSION_TYPE_BASE,//PNG_COMPRESSION_TYPE_BASE 
        PNG_FILTER_TYPE_BASE//PNG_FILTER_TYPE_BASE
        );
    ///* Write the file header information.  REQUIRED
    png_write_info(png_ptr, info_ptr);

    // flip BGR pixels to RGB 
    png_set_bgr(png_ptr);
    FBitmap->OpenPixels();
    if ((row_pointers = (png_bytepp) malloc(iHeight * sizeof(png_bytep))) == NULL)
        throw Exception ( "Not enough memory" );
    for (i = 0; i < (png_uint_32)iHeight; i++)
        row_pointers[i] = (BYTE*)FBitmap->ScanLine[i];

    png_write_image(png_ptr, row_pointers);
    FBitmap->ClosePixels();
    png_write_end(png_ptr, info_ptr);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose (pfFile);
};
#pragma package(smart_init)
