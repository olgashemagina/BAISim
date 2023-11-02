//---------------------------------------------------------------------------
#pragma hdrstop
#include "ImportRaster.h"// <-- import TImageDecoder interface
#define Z_PREFIX
#include "pnglibio.h"
//---------------------------------------------------------------------------

void __fastcall TPngDecoder::Init(){
    pfFile=fopen(FFileName.c_str(),"rb");
    if(!pfFile)
        throw Exception ( "Can't open file" );
    
    // first check the eight byte PNG signature
    fread(pbSig, 1, 8, pfFile);
    if (!png_check_sig(pbSig, 8))
        throw Exception ( "Bad file format" );

    // create the two png(-info) structures
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL,
        NULL, NULL);
    if (!png_ptr)
        throw Exception( "Not enough memory" );

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        throw Exception( "Not enough memory" );
    }
    /*точка возврата после обработки ошибки или предупреждения*/
    if (setjmp(png_jmpbuf(png_ptr)))
    {
    //    awp_res=*((long*)png_ptr->error_ptr);
        if(row_pointers){
            for (i = 0; i < FNumScanLines; i++){
                if(row_pointers[i])
                    free(row_pointers[i]);
            }
            free(row_pointers);
        }
        png_destroy_read_struct(&png_ptr, &info_ptr,NULL);
        if(pfFile) fclose(pfFile);
        throw Exception( "" );;
    }
    
    // initialize the input png file
    png_init_io(png_ptr, pfFile);

    //8 байт сигнатуры уже прочитанно
    png_set_sig_bytes(png_ptr, 8);

   
    /* The call to png_read_info() gives us all of the information from the
    * PNG file before the first IDAT (image data chunk).  REQUIRED
    */
    png_read_info(png_ptr, info_ptr);
    unsigned long width, hight;
    png_get_IHDR(png_ptr, info_ptr, &width, &hight, &iBitDepth, &iColorType,
        NULL, NULL, NULL);
    FWidth = width;
    FNumScanLines = hight;

   /**** Set up the data transformations you want.  Note that these are all
    **** optional.  Only call them if you want/need them.  Many of the
    **** transformations only work on specific types of images, and many
    **** are mutually exclusive.
    ****/

    /* tell libpng to strip 16 bit/color files down to 8 bits/color */
    //может это не нужно?
    if (iBitDepth == 16)
        png_set_strip_16(png_ptr);
    if (iBitDepth < 8)
        png_set_expand(png_ptr);
    
    
    /* Expand paletted colors into true RGB triplets */
    if (iColorType == PNG_COLOR_TYPE_PALETTE)
        png_set_expand(png_ptr);
    
        /* Expand paletted or RGB images with transparency to full alpha channels
        * so the data will be available as RGBA quartets.
    */
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
        png_set_expand(png_ptr);
    
    /* tell libpng convert grayscale images to true rgd */
    if (iColorType == PNG_COLOR_TYPE_GRAY||
        iColorType == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png_ptr);
    /*remove alpha channel instead combine with background*/
    if (iColorType & PNG_COLOR_MASK_ALPHA)
        png_set_strip_alpha(png_ptr);

        /* Set the background color to draw transparent and alpha images over.
        * It is possible to set the red, green, and blue components directly
        * for paletted images instead of supplying a palette index.  Note that
        * even if the PNG file supplies a background, you are not required to
        * use it - you should use the (solid) application background if it has one.
    */

    // set the background color to draw transparent and alpha images over.
    if (png_get_bKGD(png_ptr, info_ptr, &pBackground))
        png_set_background(png_ptr, pBackground, PNG_BACKGROUND_GAMMA_FILE, 1, 1.0);
    /* flip the RGB pixels to BGR (or RGBA to BGRA) */
    if (iColorType & PNG_COLOR_MASK_COLOR)
        png_set_bgr(png_ptr);

    png_read_update_info(png_ptr, info_ptr);
    png_get_IHDR(png_ptr, info_ptr, &width, &hight, &iBitDepth, &iColorType,
        NULL, NULL, NULL);
    FWidth = width; FNumScanLines = hight;
    FBufferSize = png_get_rowbytes(png_ptr, info_ptr);
    FScanBuffer = new char[FBufferSize];
    if(width>0x7fff||hight>0x7fff)
        throw Exception( "Format not supported" );

}

void __fastcall TPngDecoder::ReadScanLine( int ScanLine ){

    png_read_row(png_ptr, FScanBuffer, NULL);
}

void __fastcall TPngDecoder::Close(){
    png_read_end(png_ptr,NULL);
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    fclose(pfFile);
}

#pragma package(smart_init)
