//---------------------------------------------------------------------------

#ifndef ExportRasterH
#define ExportRasterH
//---------------------------------------------------------------------------
#include "DIBImage41.h"
void saveToJpeg( char* filename, TDIBImage* FBitmap);
void saveToTiff( char* filename, TDIBImage* FBitmap);
void saveToPng( char* filename, TDIBImage* FBitmap);
//void saveToBmp( char* filename, Graphics::TBitmap* FBitmap);
void rgb_to_bgr(BYTE *ptr,int count);
#endif
