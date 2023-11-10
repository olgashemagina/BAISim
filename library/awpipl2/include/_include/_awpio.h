#ifndef _AWP_IO_H_
#define _AWP_IO_H_
/*utilites*/
AWPRESULT _awpGetFileExt(const char* lpFileName, char* lpFileExt);
/*awp image io*/
AWPRESULT _awpLoadAWPImage(const char* lpFileName, awpImage** ppImage);
AWPRESULT _awpSaveAWPImage(const char* lpFileName, awpImage* pImage);
#ifndef __GNUC__
/*bmp image io*/
AWPRESULT _awpLoadBMPImage(const char* lpFileName, awpImage** ppImage);
/*ppm image io*/
AWPRESULT _awpLoadPPMImage(const char* lpFileName, awpImage** ppImage);
/*tga image io*/
AWPRESULT _awpLoadTGAImage(const char* lpFileName, awpImage** ppImage);
/*gif image io*/
//AWPRESULT _awpLoadGIFImage(const char* lpFileName, awpImage** ppImage);
#endif
#ifdef HAVE_JPEG
/*jpeg image io*/
AWPRESULT _awpLoadJPEGImage(const char* lpFileName, awpImage** ppImage);
AWPRESULT _awpSaveJPEGImage(const char* lpFileName, awpImage* pImage);
#endif /* HAVE_JPEG */

#endif /*_AWP_IO_H_*/
