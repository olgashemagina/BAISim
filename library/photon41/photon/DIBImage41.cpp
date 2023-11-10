//---------------------------------------------------------------------------
#pragma hdrstop
#include <Clipbrd.hpp>
#include "DIBImage41.h"
#include "ImportRaster.h"
#include "ExportRaster.h"
#pragma package(smart_init)

//---------------------------------------------------------------------------
__fastcall TDIBImage::TDIBImage()
{
    m_DIBPixels    = 0;
    m_OpenedPixels = NULL;
    memset(&m_DIBInfo, 0, sizeof(BITMAPINFO));
    InitializeCriticalSection(&m_Lock);
}

//---------------------------------------------------------------------------
__fastcall TDIBImage::~TDIBImage()
{
    DeleteCriticalSection(&m_Lock);

    if ( m_DIBPixels != 0 )
        ::GlobalFree( (HGLOBAL)m_DIBPixels );
}

//---------------------------------------------------------------------------
void __fastcall TDIBImage::Assign(Classes::TPersistent* Source)
{
    TDIBImage  *dib;
    TClipboard *clip;
    unsigned char *pDIB = NULL;
    this->Modified = false;
    if ( Source == NULL )
    {
        Clear();
        return;
    }

    // Check if assignment from another DIB image
    if ( (dib = dynamic_cast<TDIBImage *>(Source)) != 0 )
    {
        EnterCriticalSection(&m_Lock);

        Clear();
        
        long size   = 3 * dib->Width * dib->Height * sizeof(unsigned char);
        m_DIBPixels = (HDIB) ::GlobalAlloc( GMEM_FIXED | GMEM_ZEROINIT, size );
        if ( m_DIBPixels == 0 )
            return;
        pDIB = (unsigned char *) ::GlobalLock( (HGLOBAL)m_DIBPixels );
        memcpy( pDIB, dib->OpenPixels(), size);
        dib->ClosePixels();
        ::GlobalUnlock( (HGLOBAL)m_DIBPixels );
        m_DIBInfo   = dib->DIBInfo;

        LeaveCriticalSection(&m_Lock);
    }
    // Check if assingment form clipboard
    else if ( (clip = dynamic_cast<TClipboard *>(Source)) != 0 && Source != NULL )
    {
        EnterCriticalSection(&m_Lock);
        clip->Open();
        Clear();

        int DataHandle = clip->GetAsHandle(CF_DIB);
        if ( DataHandle != 0 )
            LoadFromClipboardFormat( CF_DIB, DataHandle, 0 );
            
        clip->Close();
        LeaveCriticalSection(&m_Lock);
    }
    else
        TGraphic::Assign(Source);
}

//---------------------------------------------------------------------------
//
void __fastcall TDIBImage::Clear()
{
    if ( m_DIBPixels != 0 /*&& GlobalLock((HGLOBAL)m_DIBPixels )*/)
    {
        ::GlobalFree( (HGLOBAL)m_DIBPixels );
        m_DIBPixels = NULL;
    }
    m_DIBPixels = 0;
    memset(&m_DIBInfo, 0, sizeof(BITMAPINFO));
}

//---------------------------------------------------------------------------
int __fastcall TDIBImage::GetHeight()
{
    return m_DIBInfo.bmiHeader.biHeight;
}

//---------------------------------------------------------------------------
int __fastcall TDIBImage::GetWidth()
{
    return m_DIBInfo.bmiHeader.biWidth;
}

//---------------------------------------------------------------------------
void __fastcall TDIBImage::GetAWPImage( awpImage **in_Image )
{
    if ( m_DIBPixels == 0 )
        return;
        
    awpImage *image = NULL;
    *in_Image       = NULL;

    unsigned char *pDIB = (unsigned char *)::GlobalLock( (HGLOBAL)m_DIBPixels );
    if ( awpDIBToImage( &m_DIBInfo, pDIB, &image ) == AWP_OK )
        *in_Image = image;
    ::GlobalUnlock( (HGLOBAL)m_DIBPixels );
}

//---------------------------------------------------------------------------
void __fastcall TDIBImage::SetAWPImage(awpImage* pImage )
{
    if ( pImage == NULL )
        return;

    // Convert awpImage data to DIB format
    unsigned char *pPixels = NULL;
    BITMAPINFO bInfo;
    if ( awpImageToDIB( pImage, &bInfo, &pPixels, false ) != AWP_OK )
        return;

    Clear();

    // Allocate global memory for converted pixels
    long w      = ((pImage->sSizeX * 24 + 31)/32) * 4;
    long size   = w * pImage->sSizeY * sizeof(unsigned char);
    m_DIBPixels = (HDIB) ::GlobalAlloc( GMEM_FIXED | GMEM_ZEROINIT, size );
    if ( m_DIBPixels == 0 )
    {
        Clear();
        free( pPixels );
        return;
    }

    // Copy converted pixels into global memory block
    unsigned char *pDIB = (unsigned char *) ::GlobalLock( (HGLOBAL)m_DIBPixels );
    if ( pDIB == NULL )
    {
        Clear();
        free( pPixels );
        return;
    }
    memcpy( pDIB, pPixels, size );
    ::GlobalUnlock( (HGLOBAL)m_DIBPixels );

    m_DIBInfo = bInfo;
    free( pPixels );
}

//---------------------------------------------------------------------------
void __fastcall TDIBImage::SetHeight(int in_Height)
{
    if ( in_Height <= 0 )
    {
        Clear();
        return;
    }

    if ( in_Height == Height )
        return;

    awpImage *src_image = NULL;
    awpImage *dst_image = NULL;
    unsigned char *pDIB = NULL;
    long size = 0;
    long w    = 0;

    if ( m_DIBPixels == 0 )
    {
        if ( awpCreateImage( &dst_image, 1, in_Height, 3, AWP_BYTE ) != AWP_OK )
            return;
    }
    else
    {
        pDIB = (unsigned char *) ::GlobalLock( (HGLOBAL)m_DIBPixels );
        if (awpDIBToImage( &m_DIBInfo, pDIB, &src_image ) != AWP_OK )
            return;
        ::GlobalUnlock( (HGLOBAL)m_DIBPixels );

        if ( in_Height > src_image->sSizeY )
        {
            awpRGBColor white_color;
            white_color.bRed   = 255;
            white_color.bGreen = 255;
            white_color.bBlue  = 255;

            awpPoint pnt;
            pnt.X = 0;
            pnt.Y = 0;

            awpCreateImage( &dst_image, src_image->sSizeX, in_Height,
                            src_image->bChannels, src_image->dwType );
            awpFill( dst_image, 255 );
            awpPasteRect( src_image, dst_image, pnt);
        }
        else
        {
            awpRect rect;
            rect.top = 0;
            rect.bottom = in_Height - 1;
            rect.left = 0;
            rect.right = src_image->sSizeX - 1;

            awpCopyRect( src_image, &dst_image, &rect);
        }
    }

    Clear();
    w           = ((dst_image->sSizeX * 24 + 31)/32) * 4;
    size        = w * dst_image->sSizeY * sizeof(unsigned char);
    m_DIBPixels = (HDIB) ::GlobalAlloc( GMEM_FIXED | GMEM_ZEROINIT, size );
    if ( m_DIBPixels == 0 )
    {
        awpReleaseImage( &src_image );
        awpReleaseImage( &dst_image );
        return;
    }
    pDIB = (unsigned char *) ::GlobalLock( (HGLOBAL)m_DIBPixels );
    awpImageToDIB( dst_image, &m_DIBInfo, &pDIB, true);
    ::GlobalUnlock( (HGLOBAL)m_DIBPixels );
    
    awpReleaseImage( &src_image );
    awpReleaseImage( &dst_image );
}

//---------------------------------------------------------------------------
void __fastcall TDIBImage::SetWidth( int in_Width )
{
    if (in_Width == m_DIBInfo.bmiHeader.biWidth)
        return;
    if ( in_Width <= 0 )
    {
        Clear();
        return;
    }

    if ( in_Width == Width )
        return;

    awpImage *src_image = NULL;
    awpImage *dst_image = NULL;
    unsigned char *pDIB = NULL;
    long size = 0;
    long w    = 0;

    if ( m_DIBPixels == 0 )
    {
        if ( awpCreateImage( &dst_image, in_Width, 1, 3, AWP_BYTE ) != AWP_OK )
            return;
    }
    else
    {
        pDIB = (unsigned char *) ::GlobalLock( (HGLOBAL)m_DIBPixels );        
        if (awpDIBToImage( &m_DIBInfo, pDIB, &src_image ) != AWP_OK )
            return;
        ::GlobalUnlock( (HGLOBAL)m_DIBPixels );

        if ( in_Width > src_image->sSizeX  )
        {
            awpRGBColor white_color;
            white_color.bRed   = 255;
            white_color.bGreen = 255;
            white_color.bBlue  = 255;

            awpPoint pnt;
            pnt.X = 0;
            pnt.Y = 0;

            awpCreateImage( &dst_image, in_Width, src_image->sSizeY,
                            src_image->bChannels, src_image->dwType );
            awpFill( dst_image, 255 );
            awpPasteRect( src_image, dst_image, pnt );
        }
        else
        {
            awpRect rect;
            rect.top = 0;
            rect.bottom = src_image->sSizeY - 1;
            rect.left = 0;
            rect.right = in_Width;

            awpCopyRect( src_image, &dst_image, &rect );
        }
    }
    
    Clear();

    w           = ((dst_image->sSizeX * 24 + 31)/32) * 4;
    size        = w * dst_image->sSizeY * sizeof(unsigned char);
    m_DIBPixels = (HDIB) ::GlobalAlloc( GMEM_FIXED | GMEM_ZEROINIT, size );
    if ( m_DIBPixels == 0 )
    {
        awpReleaseImage( &src_image );
        awpReleaseImage( &dst_image );
        return;
    }
    pDIB = (unsigned char *) ::GlobalLock( (HGLOBAL)m_DIBPixels );
    awpImageToDIB( dst_image, &m_DIBInfo, &pDIB, true);
    ::GlobalUnlock( (HGLOBAL)m_DIBPixels );

    awpReleaseImage( &src_image );
    awpReleaseImage( &dst_image );
}

//---------------------------------------------------------------------------
//
void __fastcall TDIBImage::LoadFromFile(const UnicodeString Filename)
{

      awpImage* tmp = NULL;
      AnsiString strExt = ExtractFileExt(Filename);
      strExt = strExt.LowerCase();
      if (strExt != ".awp" && strExt != ".jpg" && strExt != ".jpeg" && strExt != ".bmp" && strExt != ".ppm" && strExt != ".tga")
      {
          TImportRaster* import = new TImportRaster();
          try
          {
             import->FileName = Filename;
             import->Bitmap = this;
             import->Execute();
          }
          catch(...)
          {
                delete import;
                throw new Exception("Cannot load file " + Filename);
          }
          delete import;
          return;
      }
      else
      {
        AnsiString _ansi = Filename;
        if (awpLoadImage(_ansi.c_str(), &tmp) != AWP_OK)
        {
            throw new Exception("Cannot load file " + Filename);
        }
      }

      if (tmp->dwType != AWP_BYTE)
            if (awpConvert(tmp, AWP_CONVERT_TO_BYTE_WITH_NORM) != AWP_OK)
                throw new Exception("Cannot load file " + Filename);
      SetAWPImage(tmp);
      awpReleaseImage(&tmp);
}

//---------------------------------------------------------------------------
void __fastcall TDIBImage::SaveToFile(const UnicodeString FileName)
{
	awpImage* img = NULL;
	GetAWPImage(&img);
	if (img != NULL)
	{
		AnsiString _ansi = FileName;
		UnicodeString strExt = ExtractFileExt(FileName);
		strExt =strExt.LowerCase();
		if (strExt == ".awp" || strExt == ".jpg")
			awpSaveImage(_ansi.c_str(), img);
		else
		{
			strExt =strExt.UpperCase();
			if (strExt == ".TIF" || strExt == ".TIFF")
				saveToTiff( _ansi.c_str(), this );
			if (strExt == ".PNG")
				saveToPng( _ansi.c_str(), this );
		}
		awpReleaseImage(&img);
	 }
}

//---------------------------------------------------------------------------
void __fastcall TDIBImage::LoadFromStream(Classes::TStream* Stream)
{
    if ( Stream->Size == 0 )
        return;

    Stream->Seek( 0, soFromBeginning );
    if ( Stream->Read( (void *)&m_DIBInfo, sizeof(BITMAPINFO) ) != sizeof(BITMAPINFO) )
        return;

    unsigned int width    = ((m_DIBInfo.bmiHeader.biWidth * m_DIBInfo.bmiHeader.biBitCount + 31)/32) * 4;
    int dib_size = 3*width*m_DIBInfo.bmiHeader.biHeight*sizeof(unsigned char);

    unsigned char *pDIB = (unsigned char *)::GlobalLock( (HGLOBAL)m_DIBPixels );
    if ( Stream->Read( (void *)pDIB, dib_size ) != dib_size )
    {
        ::GlobalUnlock( (HGLOBAL)m_DIBPixels );
        Clear();
        return;
    }
    ::GlobalUnlock( (HGLOBAL)m_DIBPixels );
}

//---------------------------------------------------------------------------
void __fastcall TDIBImage::SaveToStream(Classes::TStream* Stream)
{
    if ( Stream->Size != 0 )
        return;

    unsigned int width    = ((m_DIBInfo.bmiHeader.biWidth * m_DIBInfo.bmiHeader.biBitCount + 31)/32) * 4;
    unsigned int dib_size = width*m_DIBInfo.bmiHeader.biHeight*sizeof(unsigned char);

    Stream->Seek( 0, soFromBeginning );
    Stream->Write( (const void*)&m_DIBInfo, sizeof(BITMAPINFO) );
    unsigned char *pDIB = (unsigned char *)::GlobalLock( (HGLOBAL)m_DIBPixels );
    Stream->Write( (const void*)pDIB, dib_size );
    ::GlobalUnlock( (HGLOBAL)m_DIBPixels );
}

//---------------------------------------------------------------------------
void __fastcall TDIBImage::LoadFromClipboardFormat(System::Word AFormat, NativeUInt AData, HPALETTE APalette)
{
    if ( AFormat != CF_DIB )
        return;
    if ( APalette != 0 )
        return;

    BITMAPINFOHEADER header;
    char *pData = (char *)::GlobalLock( (HANDLE)AData );
    memcpy( &header, pData, sizeof(BITMAPINFOHEADER) );
    long img_size  = header.biHeight * ((header.biWidth * header.biBitCount + 31)/32) * 4;

    if ( header.biSize == sizeof(BITMAPINFOHEADER) )
    {
        Clear();
        memcpy( &m_DIBInfo.bmiHeader, &header, sizeof(BITMAPINFOHEADER) );

        m_DIBPixels = (HDIB) ::GlobalAlloc( GMEM_FIXED | GMEM_ZEROINIT, img_size );
        unsigned char *pDIB = (unsigned char *) ::GlobalLock( (HGLOBAL)m_DIBPixels );
        memcpy( pDIB, pData + sizeof(BITMAPINFOHEADER), img_size );
        ::GlobalUnlock( (HGLOBAL)m_DIBPixels );
    }
    ::GlobalUnlock((HGLOBAL)AData);
}

//---------------------------------------------------------------------------
void __fastcall TDIBImage::SaveToClipboardFormat(System::Word &AFormat, NativeUInt &AData, HPALETTE &APalette)
{
    long img_size = m_DIBInfo.bmiHeader.biHeight *
       ((m_DIBInfo.bmiHeader.biWidth * m_DIBInfo.bmiHeader.biBitCount + 31)/32) * 4;

    HANDLE handle = (HANDLE)::GlobalAlloc (GHND, sizeof(BITMAPINFOHEADER) + img_size);
    if ( handle == NULL )
        return;

    char *pData = (char *)::GlobalLock(handle);
    memcpy( pData, &m_DIBInfo.bmiHeader, sizeof(BITMAPINFOHEADER) );
    unsigned char *pDIB = (unsigned char*) ::GlobalLock( (HGLOBAL)m_DIBPixels );
    memcpy( pData + sizeof(BITMAPINFOHEADER), pDIB, img_size );
    ::GlobalUnlock( (HGLOBAL)m_DIBPixels );
    ::GlobalUnlock( (HGLOBAL)handle );

    AFormat  = CF_DIB;
    APalette = 0;
    AData    = (unsigned)handle;
}

//---------------------------------------------------------------------------
void __fastcall TDIBImage::Changed(System::TObject* Sender)
{}

//---------------------------------------------------------------------------
void __fastcall TDIBImage::Draw(TCanvas* ACanvas, const Types::TRect &Rect)
{
    if ( m_DIBPixels == 0 )
        return;
        
    unsigned char *pDIB = (unsigned char*) ::GlobalLock( (HGLOBAL)m_DIBPixels );
    ::SetStretchBltMode(ACanvas->Handle, COLORONCOLOR);

       TRect src_r, dst_r;
       src_r.left = 0;//
       src_r.Top = 0;
       src_r.right = Width;
       src_r.Bottom = Height;
       dst_r = Rect;
       ::StretchDIBits(ACanvas->Handle,
          dst_r.Left,
          dst_r.Top,
          dst_r.Width(),
          dst_r.Height(),
          src_r.Left,
          src_r.Top,
          src_r.Width(),
          src_r.Height(),
          pDIB,
          &m_DIBInfo,
          DIB_RGB_COLORS,
          SRCCOPY);

    ::GlobalUnlock( (HGLOBAL)m_DIBPixels );
}

//---------------------------------------------------------------------------
bool __fastcall TDIBImage::GetEmpty(void)
{
    return ( m_DIBPixels == 0 ) ? true : false;
}

//---------------------------------------------------------------------------
HPALETTE __fastcall TDIBImage::GetPalette(void)
{
    return NULL;
}

//---------------------------------------------------------------------------
void __fastcall TDIBImage::ReadData(Classes::TStream* Stream)
{}

//---------------------------------------------------------------------------
void __fastcall TDIBImage::SetPalette(HPALETTE Value)
{}

//---------------------------------------------------------------------------
void __fastcall TDIBImage::WriteData(Classes::TStream* Stream)
{}

//---------------------------------------------------------------------------
void* __fastcall TDIBImage::GetScanline(int Row)
{
    if ( m_OpenedPixels == NULL )
        return NULL;
    if ( GetEmpty() == true )
        return NULL;
    if ( Row > GetHeight() - 1 || Row < 0 )
        return NULL;

    int inv_row = GetHeight() - 1 - Row;

    int width = ((m_DIBInfo.bmiHeader.biWidth * m_DIBInfo.bmiHeader.biBitCount + 31)/32) * 4;
    return (void *)(m_OpenedPixels + width*inv_row); // TODO: CHANGE THIS!
}

//---------------------------------------------------------------------------
unsigned char* TDIBImage::OpenPixels()
{
    if ( m_DIBPixels == 0 )
        return NULL;

    m_OpenedPixels = (unsigned char *) ::GlobalLock( (HGLOBAL)m_DIBPixels );
    return m_OpenedPixels;    
}

//---------------------------------------------------------------------------
void __fastcall TDIBImage::ClosePixels()
{
    ::GlobalUnlock( (HGLOBAL)m_DIBPixels );
    m_OpenedPixels = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TDIBImage::FlipVertical()
{
    if (m_OpenedPixels == NULL)
        return;
    int LineWidth = (((GetWidth()* 24 + 31)/32) * 4);
    int size = LineWidth*sizeof(unsigned char);
    unsigned char* buffer = (unsigned char*)malloc(size);
    if (buffer == NULL)
        return;
    for (int i = 0; i < GetHeight()/2; i++)
    {
        memcpy(buffer, GetScanline(i),size);
        memcpy(GetScanline(i),GetScanline(GetHeight()-1-i),size);
        memcpy(GetScanline(GetHeight()-1-i), buffer, size);
    }
    free(buffer);
}
//---------------------------------------------------------------------------
void __fastcall TDIBImage::FlipHorizontal()
{
    if (m_OpenedPixels == NULL)
        return;
}

