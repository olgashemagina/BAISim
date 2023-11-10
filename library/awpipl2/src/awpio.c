
/*
    AWP Image library v2

    $Source: /home/cvs/awp/imageproc/awpipl-v2/src/awpio.c,v $
    $Revision: 1.23 $
    $Date: 2005/04/05 14:47:25 $

    Authors: Eugene Eremin <medlab@appl.sci-nnov.ru>
             Alexander Telnykh <telnykha@yahoo.ru>
*/

#include "_awpipl.h"


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_JPEG
#   include <stdio.h>
#   include <setjmp.h>
#   include <jpeglib.h>
#endif

#ifdef HAVE_JPEG
GLOBAL(void) jpeg_buffer_src (j_decompress_ptr cinfo, const char* buffer, AWPINT len );
GLOBAL(void) jpeg_buffer_dest (j_compress_ptr cinfo,char* buffer, AWPINT len );
#endif

/*load image*/
AWPRESULT awpLoadImage(const char* lpFileName, awpImage** ppImage)
{
    /*variables*/
    AWPRESULT res;
    char	  lpExt[5];
    /*arguments*/
    if (lpFileName == NULL || ppImage == NULL)
        return AWP_BADARG;

    res   = AWP_OK;

    /*obtain file extention*/
    res = _awpGetFileExt(lpFileName, lpExt);
    if (res != AWP_OK)
        return AWP_BADARG;

    if (strcmp(lpExt, "awp") == 0 || strcmp(lpExt,"AWP") == 0)
        res = _awpLoadAWPImage(lpFileName, ppImage);
#ifdef HAVE_JPEG
#ifdef WIN32
//#if 0
    else if (strcmp(lpExt, "bmp") == 0 || strcmp(lpExt,"BMP") == 0)
        res = _awpLoadBMPImage(lpFileName, ppImage);
    else if (strcmp(lpExt, "ppm") == 0 || strcmp(lpExt,"PPM") == 0)
        res = _awpLoadPPMImage(lpFileName, ppImage);
    else if (strcmp(lpExt, "tga") == 0 || strcmp(lpExt,"TGA") == 0)
        res = _awpLoadTGAImage(lpFileName, ppImage);
//#endif
#endif 
    else if (strcmp(lpExt, "jpg") == 0 || strcmp(lpExt,"JPG") == 0 || strcmp(lpExt, "jpeg") == 0 || strcmp(lpExt,"jpeg") == 0 || strcmp(lpExt,"JPEG") == 0)
        res = _awpLoadJPEGImage(lpFileName, ppImage);
#endif /* HAVE_JPEG */
    else
        return _awpLoadAWPImage(lpFileName, ppImage);

    /*all went well*/
    return res;
}

/*save image*/
AWPRESULT awpSaveImage(const char* lpFileName, awpImage* pImage)
{
    AWPRESULT res;
    char	  lpExt[32];
    if (lpFileName == NULL && pImage == NULL)
        return AWP_BADARG;

    /*obtain file extention*/
    res = _awpGetFileExt(lpFileName, lpExt);
    if (res != AWP_OK)
        return AWP_BADARG;

    if (strcmp(lpExt, "awp") == 0 || strcmp(lpExt,"AWP") == 0)
        res = _awpSaveAWPImage(lpFileName, pImage);
#ifdef HAVE_JPEG
    else if (strcmp(lpExt, "jpg") == 0 || strcmp(lpExt,"JPG") == 0 || strcmp(lpExt, "jpeg") == 0 || strcmp(lpExt,"JPEG") == 0)
        res = _awpSaveJPEGImage(lpFileName, pImage);
#endif /* HAVE_JPEG */
    else
        return _awpSaveAWPImage(lpFileName, pImage);
    /*all went well*/
    return res;
}

#ifdef HAVE_JPEG

struct my_error_mgr
{
    struct jpeg_error_mgr pub;   //  "public" fields
    jmp_buf setjmp_buffer;       //  for return to caller
};
typedef struct my_error_mgr* my_error_ptr;


void my_error_exit(j_common_ptr cinfo)
{
    my_error_ptr myerr = (my_error_ptr) cinfo->err;
    longjmp(myerr->setjmp_buffer, 1);
}

AWPRESULT awpJpegToImage( const AWPBYTE* pJpegData, AWPDWORD length, awpImage** ppImage )
{
    AWPRESULT res;

    struct my_error_mgr jpeg_err;
    struct jpeg_decompress_struct in_jpeg;
    AWPBYTE* pDst;
    JSAMPARRAY buffer;      /* Output row buffer */
    AWPINT row_stride;
    AWPINT rows;
    JDIMENSION i;
    AWPBYTE* src = NULL;

    //set error function
    in_jpeg.err = jpeg_std_error( &jpeg_err.pub );
    jpeg_err.pub.error_exit = my_error_exit;

    if (setjmp(jpeg_err.setjmp_buffer))
    {
        jpeg_destroy_decompress(&in_jpeg);
        return AWP_BADIMAGE_FORMAT;
    }

    res   = AWP_OK;

    jpeg_create_decompress( &in_jpeg );

    /* read from buffer */
    jpeg_buffer_src( &in_jpeg, (char*)pJpegData, length );
    jpeg_read_header( &in_jpeg, TRUE );


    if(!(in_jpeg.out_color_space != JCS_RGB || in_jpeg.out_color_space != JCS_GRAYSCALE)){
        res = AWP_NOTSUPPORT;
        _ERR_EXIT_
    }

    jpeg_start_decompress( &in_jpeg );

    row_stride = in_jpeg.output_width*in_jpeg.output_components;
    rows = in_jpeg.output_height;

    /* create awp image */
    _CHECK_RESULT_((res = awpCreateImage(ppImage,(AWPWORD)in_jpeg.output_width,(AWPWORD)in_jpeg.output_height,(AWPBYTE)(in_jpeg.out_color_space==JCS_RGB?3:1),AWP_BYTE)))

    buffer = (*in_jpeg.mem->alloc_sarray)
             ((j_common_ptr) &in_jpeg, JPOOL_PERMANENT, row_stride,rows );


    pDst = (AWPBYTE*)((*ppImage)->pPixels);
    while (in_jpeg.output_scanline < in_jpeg.output_height)
    {
        jpeg_read_scanlines(&in_jpeg, buffer, 1);
        src = (AWPBYTE*)buffer[0];
        for (i = 0; i < in_jpeg.output_width; i++)
        {
            pDst[3*i + 2] = src[0];
            pDst[3*i + 1] = src[1];
            pDst[3*i]     = src[2];
            src += 3;
        }

        pDst += row_stride;
    }


CLEANUP:

    jpeg_finish_decompress( &in_jpeg );
    jpeg_destroy_decompress( &in_jpeg );

    return res;

}



AWPRESULT awpImageToJpeg( const awpImage* pImage, AWPBYTE** ppJpegData, AWPDWORD* length, AWPBYTE quality )
{
    AWPRESULT res;


    struct jpeg_error_mgr jpeg_err;
    struct jpeg_compress_struct out_jpeg;
    AWPBYTE* pSrc;
    AWPINT i;

    JSAMPARRAY fp;      /* Output row buffer */
    AWPINT stride;
    quality=100;
    res   = AWP_OK;
    pSrc=NULL;

    out_jpeg.err = jpeg_std_error( &jpeg_err );
    jpeg_create_compress( &out_jpeg );

    stride=pImage->sSizeX*pImage->bChannels;

    fp = out_jpeg.mem->alloc_sarray( (j_common_ptr)&out_jpeg,
                                     JPOOL_PERMANENT, stride, pImage->sSizeX );

    pSrc=(AWPBYTE*)pImage->pPixels;

    for(i=0;i<pImage->sSizeY;i++){
        memcpy(&fp[i],&pSrc[i*pImage->sSizeX*pImage->bChannels],pImage->sSizeX*pImage->bChannels);
    }

    jpeg_buffer_dest( &out_jpeg, (char*) ppJpegData,*length );
    out_jpeg.image_width = pImage->sSizeX;
    out_jpeg.image_height = pImage->sSizeY;
    out_jpeg.input_components = pImage->bChannels;

    out_jpeg.in_color_space = JCS_RGB;

    jpeg_set_defaults( &out_jpeg );
    jpeg_start_compress( &out_jpeg, TRUE );

    for( i=0; i<pImage->sSizeY; i++ )
        jpeg_write_scanlines( &out_jpeg, &fp[i], 1 );

    jpeg_finish_compress( &out_jpeg );


    jpeg_destroy_compress( &out_jpeg );

    return res;
}


#endif /* HAVE_JPEG */

AWPRESULT awpImagePack( const awpImage* pImage, AWPBYTE** ppData, AWPINT* length )
{
    AWPRESULT res;
    long c;
    AWPBYTE* pData;
    AWPINT isize;

    res   = AWP_OK;
    c=0;
    pData=NULL;
    *length=0;

    /*check input image*/
    _CHECK_RESULT_((res = awpCheckImage(pImage)))

    /* calculate size image data */
    _CHECK_RESULT_((res = awpGetImageSize(pImage, length)))
    
    /* get size header */
    _CHECK_RESULT_((res = awpGetImageHeaderSize(pImage,&isize)))

    /* allocate data */
    
    *length+=isize;
    pData = (AWPBYTE*)malloc (*length);

    if(pData==NULL){
        res=AWP_BADMEMORY;
        _ERR_EXIT_
    }

    /* copy all member in massive*/
    memcpy(&pData[c],&pImage->nMagic,sizeof(pImage->nMagic));
    c+=sizeof(pImage->nMagic);
    memcpy(&pData[c],&pImage->bChannels,sizeof(pImage->bChannels));
    c+=sizeof(pImage->bChannels);
    memcpy(&pData[c],&pImage->sSizeX,sizeof(pImage->sSizeX));
    c+=sizeof(pImage->sSizeX);
    memcpy(&pData[c],&pImage->sSizeY,sizeof(pImage->sSizeY));
    c+=sizeof(pImage->sSizeY);
    memcpy(&pData[c],&pImage->dwType,sizeof(pImage->dwType));
    c+=sizeof(pImage->dwType);
    /* copy image data*/
    memcpy(&pData[c],pImage->pPixels,*length-isize);

    *ppData=pData;

CLEANUP:

    return res;
}


AWPRESULT awpImageUnpack( const AWPBYTE* pData, AWPINT length, awpImage** ppImage )
{
    AWPRESULT res;
    awpImage *pImage;
    long c;
    AWPDWORD nMagic;
    AWPBYTE bChannels;
    AWPWORD sSizeX,sSizeY;
    AWPDWORD dwType;
    AWPINT iSize;

    res   = AWP_OK;
    c=0;
    pImage=NULL;

    /* check input params */
    if(pData==NULL || length==0){
        res = AWP_BADARG;
        _ERR_EXIT_
    }

    /* get image header in massive */
    memcpy(&nMagic,&pData[c],sizeof(nMagic));
    c+=sizeof(nMagic);

    if(nMagic!=AWP_MAGIC2){
        res=AWP_NOTAWPIMAGE;
        _ERR_EXIT_
    }

    memcpy(&bChannels,&pData[c],sizeof(bChannels));
    c+=sizeof(bChannels);
    memcpy(&sSizeX,&pData[c],sizeof(sSizeX));
    c+=sizeof(sSizeX);
    memcpy(&sSizeY,&pData[c],sizeof(sSizeY));
    c+=sizeof(sSizeY);
    memcpy(&dwType,&pData[c],sizeof(dwType));
    c+=sizeof(dwType);

    /* create awp image */
    _CHECK_RESULT_((res = awpCreateImage(&pImage,sSizeX,sSizeY,bChannels,dwType)))
    
    /* get size header */
    _CHECK_RESULT_((res = awpGetImageSize(pImage,&iSize)))

    /* copy image data*/
    memcpy(pImage->pPixels,&pData[c],iSize);

    *ppImage=pImage;

CLEANUP:

    return res;
}

