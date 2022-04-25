/*M
//
//      awpipl2 image processing and image analysis library
//		File: _awpio.c
//		Purpose: load and save images 
//		Autors: Alex A.Telnykh (tlnykha@yahoo.com)
//      CopyRight 2004-2019 (c) NN-Videolab.net
//M*/

#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

#include "_awpipl.h"

#ifdef HAVE_JPEG
#   include <stdio.h>
#   include <setjmp.h>
#	include <jpeglib.h>
#ifdef WIN32
#	include "cdjpeg.h"
#endif 
#	endif

#pragma warning(disable: 4996)
/*utilites*/
AWPRESULT _awpGetFileExt(const char* lpFileName, char* lpFileExt)
{
    AWPINT ilen;
    AWPINT iextlen;
    AWPINT i;
    char test;
    if (lpFileName == NULL || lpFileExt == NULL)
        return AWP_BADARG;

    ilen = strlen(lpFileName);
    iextlen = 0;
    i = ilen-1;
    if (ilen == 0)
        return AWP_BADARG;

    while(iextlen < 5 || i > 0)
    {
        /**/
        test = lpFileName[i];
        if (test == '.')
            break;
        i--;
        iextlen++;
    }
    strncpy(lpFileExt, &lpFileName[i+1], iextlen);
    lpFileExt[iextlen] = '\0';
    return AWP_OK;
}

/*this function loads old awp image*/
typedef struct tagOldAwpColor
{
    AWPBYTE bRed;
    AWPBYTE bGreen;
    AWPBYTE bBlue;
}OldAwpColor;


AWPRESULT _awpLoadAWPImageV1(const char* lpFileName, awpImage** ppImage)
{
   /*common variables*/
   FILE* f; /*the file*/
   awpImage*  p_tmp_image;
   awpImage*  Header;
   AWPRESULT res;
   AWPDWORD dwPixelSize = 0;
   /*header variables*/
   AWPDWORD dwMagic; /*magic number. must be 0x0A0D5C */
   AWPBYTE  bVersion; /*version number. must be 1*/
   AWPWORD  wSizeX;   /*width of the image. Can be 1-65535*/
   AWPWORD  wSizeY;   /*height of the image. Can be 1-65535*/
   AWPDWORD dwType;   /*Type of the image. Can be
                        0 - AWP_COLOR
                        1 - AWP_GRAYSCALE
                        2 - AWP_SHORT
                        3 - AWP_PALETTE
                        4 - AWP_FLOAT*/
   AWPDWORD dwSize;     /*size of pixels*/
   AWPBYTE* pbBuffer;/*buffer for pixels*/
   /*awp image creation variables*/
   AWPBYTE  bNumChannels; /*number of channels*/
   AWPDWORD dwTypeImage;  /*type of image*/
   pbBuffer = NULL; /*init variable*/
   f = fopen(lpFileName, "r+b");
   if (f == NULL)
        return AWP_OPEN_FILE_ERROR;
   /*read file header varibles*/
   Header = (awpImage*)malloc(sizeof(awpImage));
   if (Header == NULL)
        return AWP_BADMEMORY;
   if (fread(Header, sizeof(awpImage), 1, f) != 1)
    {
        res = AWP_READ_FILE_ERROR;
        _ERR_EXIT_
    }
    /*1. magic*/
    dwMagic = Header->nMagic;

    if (dwMagic != 0x0A0D5C)
    {
        res = AWP_NOT_AWP_FILE;
        _ERR_EXIT_
    }
    /*2.version*/
    bVersion = Header->bChannels;
    if (bVersion != 1)
    {
        res = AWP_NOT_AWP_FILE;
        _ERR_EXIT_
    }
    /*3.sizes*/
    wSizeX = Header->sSizeX;
    if (wSizeX == 0)
    {
        res = AWP_CORRUPT_FILE;
        _ERR_EXIT_
    }
    wSizeY = Header->sSizeY;
    if (wSizeY == 0)
    {
        res = AWP_CORRUPT_FILE;
        _ERR_EXIT_
    }
    /*4. type*/
    dwType = Header->dwType;
    if (dwType > 4)
    {
        res = AWP_NOT_AWP_FILE;
        _ERR_EXIT_
    }
    dwSize = (AWPDWORD)Header->pPixels;
    /*find the size of one pixel and variables for creation awp image*/
    switch(dwType)
    {
        case 0:
            dwPixelSize = sizeof(OldAwpColor);
            bNumChannels = 3;
            dwTypeImage = AWP_BYTE;
            break;
        case 1:
            dwPixelSize = sizeof(AWPBYTE);
            bNumChannels = 1;
            dwTypeImage = AWP_BYTE;
            break;
        case 2:
            dwPixelSize = sizeof(AWPSHORT);
            bNumChannels = 1;
            dwTypeImage = AWP_SHORT;
            break;
        case 3:
            dwPixelSize = sizeof(AWPBYTE);
            bNumChannels = 1;
            dwTypeImage = AWP_BYTE;
            break;
        case 4:
            dwPixelSize = sizeof(AWPFLOAT);
            bNumChannels = 1;
            dwTypeImage = AWP_FLOAT;
            break;
    }
    /*check the sizes*/
    dwSize -= sizeof(awpImage);
    if (dwPixelSize*wSizeX*wSizeY != dwSize )
    {
        res = AWP_CORRUPT_FILE;
        _ERR_EXIT_
    }
    /*read the pixels*/
    pbBuffer = (AWPBYTE*)malloc(dwPixelSize*wSizeX*wSizeY);
    if (pbBuffer == NULL)
    {
        res = AWP_BADMEMORY;
        _ERR_EXIT_
    }
    if (fread(pbBuffer, dwPixelSize*wSizeX*wSizeY, 1, f) != 1)
    {
        res = AWP_READ_FILE_ERROR;
        _ERR_EXIT_
    }
    /*the file in the memory. now convert the memory to awpImage*/
    _CHECK_RESULT_((res = awpCreateImage(&p_tmp_image, wSizeX, wSizeY, bNumChannels, dwTypeImage)))
    /*copy the buffers*/
    memcpy((AWPBYTE*)p_tmp_image->pPixels,pbBuffer, dwPixelSize*wSizeX*wSizeY);
    *ppImage = p_tmp_image;
CLEANUP:
   _SAFE_FREE_(Header)
   _SAFE_FREE_(pbBuffer)
   if (f != NULL)
        fclose(f);
   return res;
}

/*awp image io*/
AWPRESULT _awpLoadAWPImage(const char* lpFileName, awpImage** ppImage)
{
    FILE* f; /*file*/
    awpImage* p_tmp_image; /*image*/
    AWPRESULT res;
    AWPINT isize,isize1;
    AWPINT itmp;

    *ppImage = NULL;
    /*open file*/
    f = fopen (lpFileName, "r+b");
    if (f == NULL)
        return AWP_OPEN_FILE_ERROR;

    /*alloc memory for image */
//    p_tmp_image = NULL;
    p_tmp_image = (awpImage*)malloc(sizeof(awpImage));

    if (p_tmp_image == NULL)
    {
        fclose(f);
        return AWP_NOTENOUGH_MEM;
    }

    /*read header from file*/
    /*read magic number */
    if( fread( &p_tmp_image->nMagic, sizeof(p_tmp_image->nMagic), 1, f) != 1  )
    {
        fclose( f );
        free( p_tmp_image );
        return AWP_READ_FILE_ERROR;
    }

    /*check header*/
    if (p_tmp_image->nMagic != AWP_MAGIC2)
    {
        fclose( f );
        free( p_tmp_image );
        res =  _awpLoadAWPImageV1(lpFileName, ppImage);
        return res;
    }

    /*read number channels */
    if( fread( &p_tmp_image->bChannels, sizeof(p_tmp_image->bChannels), 1, f) != 1  )
    {
        fclose( f );
        free( p_tmp_image );
        return AWP_READ_FILE_ERROR;
    }

    /*read X size */
    if( fread( &p_tmp_image->sSizeX, sizeof(p_tmp_image->sSizeX), 1, f) != 1  )
    {
        fclose( f );
        free( p_tmp_image );
        return AWP_READ_FILE_ERROR;
    }

    /*read Y size */
    if( fread( &p_tmp_image->sSizeY, sizeof(p_tmp_image->sSizeY), 1, f) != 1  )
    {
        fclose( f );
        free( p_tmp_image );
        return AWP_READ_FILE_ERROR;
    }

    /*read type image */
    if( fread( &p_tmp_image->dwType, sizeof(p_tmp_image->dwType), 1, f) != 1  )
    {
        fclose( f );
        free( p_tmp_image );
        return AWP_READ_FILE_ERROR;
    }

    /*read size image */
    if( fread( &isize, sizeof(isize), 1, f) != 1  )
    {
        fclose( f );
        free( p_tmp_image );
        return AWP_READ_FILE_ERROR;
    }

    itmp = ftell(f);

    /*image sizes */
    fseek( f, 0, SEEK_SET);
    fseek( f, 0, SEEK_END);
    isize1 = ftell(f);
    fseek( f, itmp, SEEK_SET);

    if (isize != (AWPINT)(isize1-itmp) )
    {
        fclose( f );
        free( p_tmp_image );
        return AWP_CORRUPT_FILE;
    }

    /*find size of pixels*/
    isize = p_tmp_image->bChannels*p_tmp_image->sSizeX*p_tmp_image->sSizeY;
    switch(p_tmp_image->dwType)
    {
    case AWP_BYTE:   isize *= sizeof(AWPBYTE); break;
    case AWP_SHORT:  isize *= sizeof(AWPSHORT); break;
    case AWP_FLOAT:  isize *= sizeof(AWPFLOAT);break;
    case AWP_DOUBLE: isize *= sizeof(AWPDOUBLE);break;
    default:
        {
            fclose( f );
            free( p_tmp_image );
            return AWP_CORRUPT_FILE;
        }
    }
    /*alloc memory for pixels*/
    p_tmp_image->pPixels = NULL;
    p_tmp_image->pPixels = malloc(isize);
    if (p_tmp_image->pPixels == NULL)
    {
        fclose( f );
        free( p_tmp_image );
        return AWP_NOTENOUGH_MEM;
    }

    /*read data form file*/
    if (fread( p_tmp_image->pPixels, isize, 1, f) != 1)
    {
        fclose( f );
        free( p_tmp_image->pPixels );
        free( p_tmp_image );
        return AWP_READ_FILE_ERROR;
    }
    /*all went well*/
    *ppImage = p_tmp_image;
    fclose(f);
    return AWP_OK;
}

AWPRESULT _awpSaveAWPImage(const char* lpFileName, awpImage* pImage)
{
    AWPRESULT res;
    FILE* f;
    AWPINT  isize;

    if (lpFileName == NULL)
        return AWP_BADARG;


   // res = AWP_OK;/*initialize result */
    f	= NULL;
    isize = 0;
    
    _CHECK_RESULT_((res = awpCheckImage(pImage)))

    f = fopen(lpFileName, "w+b");
    if (f == NULL)
    {
        res = AWP_CREATE_FILE_ERROR;
        _ERR_EXIT_
    }

    _CHECK_RESULT_((res = awpGetImageSize(pImage, &isize)))

    /*write header*/
    /*write magic number */
    if( fwrite( &pImage->nMagic, sizeof(pImage->nMagic), 1, f ) != 1 )
    {

        res = AWP_WRITE_FILE_ERROR;
        _ERR_EXIT_
    }
    
    /*write number chanals*/
    if( fwrite( &pImage->bChannels, sizeof(pImage->bChannels), 1, f ) != 1 )
    {

        res = AWP_WRITE_FILE_ERROR;
        _ERR_EXIT_
    }
    
    /*write X size*/
    if( fwrite( &pImage->sSizeX, sizeof(pImage->sSizeX), 1, f ) != 1 )
    {

        res = AWP_WRITE_FILE_ERROR;
        _ERR_EXIT_
    }
    
    /*write Y size*/
    if( fwrite( &pImage->sSizeY, sizeof(pImage->sSizeY), 1, f ) != 1 )
    {

        res = AWP_WRITE_FILE_ERROR;
        _ERR_EXIT_
    }
    
    /*write type image*/
    if( fwrite( &pImage->dwType, sizeof(pImage->dwType), 1, f ) != 1 )
    {

        res = AWP_WRITE_FILE_ERROR;
        _ERR_EXIT_
    }
    
    /*write size image */
    if( fwrite( &isize, sizeof(isize), 1, f ) != 1 )
    {

        res = AWP_WRITE_FILE_ERROR;
        _ERR_EXIT_
    }

    /*write pixels*/
    if( fwrite( pImage->pPixels, isize, 1, f ) != 1 )
    {
        res = AWP_WRITE_FILE_ERROR;
        _ERR_EXIT_
    }

CLEANUP:
    if (f != NULL)
        fclose(f);

    return res;
}


#ifdef HAVE_JPEG
/*jpeg image io*/

struct _awp_jpg_error_mgr
{
    struct jpeg_error_mgr pub;   /*  "public" fields */
    jmp_buf setjmp_buffer;       /*  for return to caller */
};

typedef struct _awp_jpg_error_mgr* _awp_jpg_error_ptr;

void _awp_jpg_error_exit(j_common_ptr cinfo)
{
    _awp_jpg_error_ptr myerr = (_awp_jpg_error_ptr) cinfo->err;
    longjmp(myerr->setjmp_buffer, 1);
}
static void SwapBytes(awpImage* pImage)
{
    AWPINT i;
    AWPBYTE b;
    awpColor* p = (awpColor*)pImage->pPixels;
    for (i = 0; i< pImage->sSizeX*pImage->sSizeY; i++)
    {
        b = p[i].bRed;
        p[i].bRed = p[i].bBlue;
        p[i].bBlue = b;
    }
}

AWPRESULT _awpLoadJPEGImage(const char* lpFileName, awpImage** ppImage)
{
    awpImage* pImage;
    AWPRESULT res;

    struct jpeg_decompress_struct cinfo;
    struct _awp_jpg_error_mgr jerr;
    AWPBYTE* pDst;
    AWPBYTE* src;
    AWPINT type_image;
    AWPBYTE iChannels;
    JDIMENSION i;
    FILE* infile;		/* source file */
    JSAMPARRAY buffer;		/* Output row buffer */
    AWPINT row_stride;		/* physical row width in output buffer */

 //   res = AWP_OK;/*initialize result */
    infile=NULL;

    if (lpFileName == NULL){
        res = AWP_BADARG;
        _ERR_EXIT_
    }

    infile = fopen(lpFileName, "rb");
    if (infile == NULL){
        res = AWP_OPEN_FILE_ERROR;
        _ERR_EXIT_
    }

    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = _awp_jpg_error_exit;

    if (setjmp(jerr.setjmp_buffer))
    {
        jpeg_destroy_decompress(&cinfo);
        fclose(infile);
        res = AWP_BADIMAGE_FORMAT;
        _ERR_EXIT_
    }

    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, infile);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);

    row_stride = cinfo.output_width * cinfo.output_components;

    type_image=AWP_BYTE;

    switch(cinfo.output_components){
    case 1:
        iChannels=1;
        break;
    case 3:
        iChannels=3;
        break;
    default:
        res=AWP_BADIMAGE_FORMAT;
        _ERR_EXIT_
    }

    _CHECK_RESULT_(( res=awpCreateImage(&pImage,(AWPWORD)cinfo.output_width,(AWPWORD)cinfo.output_height,iChannels,type_image)))

    buffer = (*cinfo.mem->alloc_sarray)
             ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

    pDst = (AWPBYTE*)pImage->pPixels;
    while (cinfo.output_scanline < cinfo.output_height)
    {
        jpeg_read_scanlines(&cinfo, buffer, 1);
        src = (AWPBYTE*)buffer[0];
        if (iChannels==3)
        {
            for (i = 0; i < cinfo.output_width; i++)
            {
                pDst[3*i + 2] = src[0];
                pDst[3*i + 1] = src[1];
                pDst[3*i]     = src[2];
                src += 3;
            }
        }
        else
            memcpy(pDst, buffer[0], row_stride);

        pDst += row_stride;
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    /* all OK */
    *ppImage=pImage;

CLEANUP:
    if (infile != NULL)
        fclose(infile);

    return res;
}

AWPRESULT _awpSaveJPEGImage(const char* lpFileName, awpImage* pImage)
{
    struct jpeg_compress_struct cinfo;
    struct _awp_jpg_error_mgr jerr;
    FILE *fp;
    AWPINT i;
    AWPBYTE *line;
    AWPINT iBytes;
    AWPRESULT res;

    if (lpFileName == NULL)
        return AWP_BADARG;

  //  res = AWP_OK;/*initialize result */
    fp=NULL;
    line=NULL;

    _CHECK_RESULT_((res = awpCheckImage(pImage)))

    if( (pImage->dwType==AWP_BYTE) && ( (pImage->bChannels==1) || (pImage->bChannels==3))){
        iBytes=pImage->bChannels;
    }else{
        res= AWP_NOTSUPPORT;
        _ERR_EXIT_
    }

    if(pImage->dwType==AWP_BYTE && pImage->bChannels==3)
        SwapBytes(pImage);

    if (NULL == (fp = fopen(lpFileName,"wb"))){
        res = AWP_CREATE_FILE_ERROR;
        _ERR_EXIT_
    }

    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = _awp_jpg_error_exit;

    if (setjmp(jerr.setjmp_buffer))
    {
        jpeg_destroy_compress(&cinfo);
        fclose(fp);
        res = AWP_BADIMAGE_FORMAT;
        _ERR_EXIT_
    }

    jpeg_create_compress(&cinfo);
    jpeg_stdio_dest(&cinfo, fp);
    cinfo.image_width = pImage->sSizeX;
    cinfo.image_height = pImage->sSizeY;
    cinfo.input_components = iBytes;
    cinfo.in_color_space = iBytes==1 ? JCS_GRAYSCALE : JCS_RGB;
    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, 100, TRUE);
    jpeg_start_compress(&cinfo, TRUE);

    for (i = 0, line = (AWPBYTE*)pImage->pPixels; i < pImage->sSizeY; i++, line += pImage->sSizeX*iBytes)
        jpeg_write_scanlines(&cinfo, &line, 1);

    jpeg_finish_compress(&(cinfo));
    jpeg_destroy_compress(&(cinfo));

CLEANUP:
    if (fp != NULL)
        fclose(fp);

    return res;
}

METHODDEF(void)
init_source (j_decompress_ptr cinfo)
{

}

METHODDEF(boolean)
fill_input_buffer (j_decompress_ptr cinfo)
{
    return TRUE;
}

METHODDEF(void)
skip_input_data (j_decompress_ptr cinfo, long num_bytes)
{
}

METHODDEF(void)
term_source (j_decompress_ptr cinfo)
{
    /* no work necessary here */
}

GLOBAL(void)
jpeg_buffer_src (j_decompress_ptr cinfo, const char* buffer, AWPINT len )
{
    struct jpeg_source_mgr* src;

    /* The source object and input buffer are made permanent so that a series
       * of JPEG images can be read from the same file by calling jpeg_stdio_src
          * only before the first one.  (If we discarded the buffer at the end of
      * one image, we'd likely lose the start of the next one.)
         * This makes it unsafe to use this manager and a different source
     * manager serially with the same JPEG object.  Caveat programmer.
        */
    if (cinfo->src == NULL) { /* first time for this JPEG object? */
        cinfo->src = (struct jpeg_source_mgr *)
                     (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
                                                 sizeof(struct jpeg_source_mgr));
    }

    src = cinfo->src;
    src->init_source = init_source;
    src->fill_input_buffer = fill_input_buffer;
    src->skip_input_data = skip_input_data;
    src->resync_to_restart = jpeg_resync_to_restart; /* use default method */
    src->term_source = term_source;
    src->next_input_byte = (JOCTET *)buffer;
    src->bytes_in_buffer = len;
}

METHODDEF(void)
init_destination (j_compress_ptr cinfo)
{

}

METHODDEF(boolean)
empty_output_buffer (j_compress_ptr cinfo)
{
    return TRUE;
}

METHODDEF(void)
term_destination (j_compress_ptr cinfo)
{

}

GLOBAL(void)
jpeg_buffer_dest (j_compress_ptr cinfo,char* buffer, AWPINT len )
{
    struct jpeg_destination_mgr* dest;

    /* The destination object is made permanent so that multiple JPEG images
       * can be written to the same file without re-executing jpeg_stdio_dest.
          * This makes it dangerous to use this manager and a different destination
      * manager serially with the same JPEG object, because their private object
         * sizes may be different.  Caveat programmer.
     */
    if (cinfo->dest == NULL) {    /* first time for this JPEG object? */
        cinfo->dest = (struct jpeg_destination_mgr *)
                      (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
                                                  sizeof(struct jpeg_destination_mgr));
    }

    dest = cinfo->dest;
    dest->init_destination = init_destination;
    dest->empty_output_buffer = empty_output_buffer;
    dest->term_destination = term_destination;
    dest->next_output_byte = (JOCTET *)buffer;
    dest->free_in_buffer = 0;
}
#ifndef __GNUC__

 static AWPRESULT _awpLoadFormatImage(const char* lpFileName, awpImage** ppImage, int format)
 {
    awpImage* pImage;
    AWPRESULT res;

    struct jpeg_compress_struct cinfo;
    struct _awp_jpg_error_mgr jerr;
    cjpeg_source_ptr src_mgr;

    AWPBYTE* pDst;
    AWPBYTE* src;
    AWPINT type_image, num_scanlines;
    AWPBYTE iChannels;
	AWPDWORD c;
    JDIMENSION i;
    FILE* infile;		/* source file */
    JSAMPARRAY buffer;		/* Output row buffer */
    AWPINT row_stride;		/* physical row width in output buffer */

    infile=NULL;

    if (lpFileName == NULL){
        res = AWP_BADARG;
        _ERR_EXIT_
    }

    infile = fopen(lpFileName, "rb");
    if (infile == NULL){
        res = AWP_OPEN_FILE_ERROR;
        _ERR_EXIT_
    }

    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = _awp_jpg_error_exit;

    if (setjmp(jerr.setjmp_buffer))
    {
        jpeg_destroy_compress(&cinfo);
        fclose(infile);
        res = AWP_BADIMAGE_FORMAT;
        _ERR_EXIT_
    }

    jpeg_create_compress(&cinfo);

    cinfo.in_color_space = JCS_RGB; /* arbitrary guess */
    jpeg_set_defaults(&cinfo);
    switch (format)
    {
        case 0:
	    src_mgr = jinit_read_bmp(&cinfo);
        break;
        case 1:
            src_mgr = jinit_read_ppm(&cinfo);
        break;
        case 2:
	   src_mgr = jinit_read_targa(&cinfo);
        break;
    }
    src_mgr->input_file = infile;

    /* Read the input file header to obtain file size & colorspace. */
    (*src_mgr->start_input) (&cinfo, src_mgr);
    jpeg_default_colorspace(&cinfo);
    /* Specify data destination for compression */
    jpeg_stdio_dest(&cinfo, stderr);
    /* Start compressor */
    jpeg_start_compress(&cinfo, TRUE);

    row_stride = cinfo.image_width*cinfo.input_components;

    type_image=AWP_BYTE;

    switch(cinfo.input_components)
    {
    case 1:
        iChannels=1;
        break;
    case 3:
        iChannels=3;
        break;
    default:
        res=AWP_BADIMAGE_FORMAT;
        _ERR_EXIT_
    }

    _CHECK_RESULT_(( res=awpCreateImage(&pImage,(AWPWORD)cinfo.image_width,(AWPWORD)cinfo.image_height,iChannels,type_image)))

    buffer = (*cinfo.mem->alloc_sarray)
             ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

    pDst = (AWPBYTE*)pImage->pPixels;
    c = 0;
    while (c < cinfo.image_height)
    {
        num_scanlines = (*src_mgr->get_pixel_rows) (&cinfo, src_mgr);
        src = (AWPBYTE*)src_mgr->buffer[0];
        if (iChannels==3)
        {
            for (i = 0; i < cinfo.image_width; i++)
            {
                pDst[3*i + 2] = src[0];
                pDst[3*i + 1] = src[1];
                pDst[3*i]     = src[2];
                src += 3;
            }
        }
        else
            memcpy(pDst, buffer[0], row_stride);
        c +=  num_scanlines;
        pDst += row_stride;
    }

      /* Finish compression and release memory */
      (*src_mgr->finish_input) (&cinfo, src_mgr);
      jpeg_destroy_compress(&cinfo);

    /* all OK */
    *ppImage=pImage;

CLEANUP:
    if (infile != NULL)
        fclose(infile);

    return res;
 }
AWPRESULT _awpLoadBMPImage(const char* lpFileName, awpImage** ppImage)
{
     return _awpLoadFormatImage(lpFileName, ppImage, 0);
}

AWPRESULT _awpLoadPPMImage(const char*      lpFileName, awpImage** ppImage)
{
      return  _awpLoadFormatImage(lpFileName, ppImage, 1);
}

AWPRESULT _awpLoadTGAImage(const char* lpFileName, awpImage** ppImage)
{
       return  _awpLoadFormatImage(lpFileName, ppImage, 2);
}
#endif

#endif /* HAVE_JPEG */
