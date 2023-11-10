//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include "PhPalette.h"
#include "awpipl.h"
#include "DIBImage41.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//

const int c_nLeftMargin    = 8;
const int c_nBottomMargin  = 10;
const int c_nTopMargin     = 10;

const int c_nVPaletteWidth  = 64;
const int c_nVPaletteHeight = 256;

const int c_nHPaletteWidth  = 256;
const int c_nHPaletteHeight = 64;


static inline void ValidCtrCheck(TPhPalette *)
{
	new TPhPalette(NULL);
}
/*
	Palettes:
	-Grayscale
	-Bluered
	-Extendet spectum
	-Ocean
	-Glow
	-Extendet spectrum 3.03
	-Traffic Light
	-False Colors
*/

static void FillColorsV(awpImage* image, int start, int finish, int sr, int sg, int sb, int er, int eg, int eb)
{
	AWPBYTE* b = (AWPBYTE*)image->pPixels;
	double dx = finish - start;
	double kr = (er -sr)/dx;
	double kg = (eg -sg)/dx;
	double kb = (eb -sb)/dx;

	for (int y = start; y < finish; y++)
	{
		for (int x = 0; x < c_nVPaletteWidth; x++)
		{
			b[y*3*c_nVPaletteWidth + 3*x] 		= sb + kb*(y-start);
			b[y*3*c_nVPaletteWidth + 3*x + 1] 	= sg + kg*(y-start);
			b[y*3*c_nVPaletteWidth + 3*x + 2] 	= sr + kr*(y-start);
		}
	}
}

static void FillColorsH(awpImage* image, int start, int finish, int sr, int sg, int sb, int er, int eg, int eb)
{
	AWPBYTE* b = (AWPBYTE*)image->pPixels;
	double dx = finish - start;
	double kr = (er -sr)/dx;
	double kg = (eg -sg)/dx;
	double kb = (eb -sb)/dx;

	for (int y = 0; y < c_nHPaletteHeight; y++)
	{
		for (int x = start; x < finish; x++)
		{
			b[y*3*c_nHPaletteWidth + 3*x] 		= sb + kb*(x-start);
			b[y*3*c_nHPaletteWidth + 3*x + 1] 	= sg + kg*(x-start);
			b[y*3*c_nHPaletteWidth + 3*x + 2] 	= sr + kr*(x-start);
		}
	}
}


/*	Veritical grayscale palette image
*/
static awpImage* CreateGrayPalette_v()
{
	awpImage* img = NULL;
	awpCreateImage(&img, c_nVPaletteWidth, c_nVPaletteHeight, 3, AWP_BYTE);
	FillColorsV(img, 0, 256, 0, 0,0, 255,255,255);
	awpFlip(&img, AWP_FLIP_VERT);
	return img;
}

/*	Horizontal grayscale palette image
*/
static awpImage* CreateGrayPalette_h()
{
	awpImage* img = NULL;
	awpCreateImage(&img, c_nHPaletteWidth, c_nHPaletteHeight, 3, AWP_BYTE);
	FillColorsH(img, 0, 256, 0, 0,0, 255,255,255);
	return img;
}

static awpImage* CreateBlueRedPalette_v()
{
	awpImage* img = NULL;
	awpCreateImage(&img, c_nVPaletteWidth, c_nVPaletteHeight, 3, AWP_BYTE);
	FillColorsV(img, 0, 255, 0, 0,255, 255,0,0);
	awpFlip(&img, AWP_FLIP_VERT);
	return img;
}

static awpImage* CreateBlueRedPalette_h()
{
	awpImage* img = NULL;
	awpCreateImage(&img, c_nHPaletteWidth, c_nHPaletteHeight, 3, AWP_BYTE);
	FillColorsH(img, 0, 255, 0, 0,255, 255,0,0);
	return img;
}

static awpImage* CreateOceanPalette_v()
{
	awpImage* img = NULL;
	awpCreateImage(&img, c_nVPaletteWidth, c_nVPaletteHeight, 3, AWP_BYTE);

	double dx = 86;
	FillColorsV(img, 0, dx, 0, 0,0, 0,0,255);
	FillColorsV(img, dx, 2*dx, 0, 0,255, 0,255,255);
	FillColorsV(img, 2*dx, c_nVPaletteHeight, 0, 255,255,255,255,255);
	awpFlip(&img, AWP_FLIP_VERT);
	return img;
}

static awpImage* CreateOceanPalette_h()
{
	awpImage* img = NULL;
	awpCreateImage(&img, c_nHPaletteWidth, c_nHPaletteHeight, 3, AWP_BYTE);

	double dx = 86;
	FillColorsH(img, 0, dx, 0, 0,0, 0,0,255);
	FillColorsH(img, dx, 2*dx, 0, 0,255, 0,255,255);
	FillColorsH(img, 2*dx, c_nHPaletteWidth, 0, 255,255,255,255,255);
	return img;
}

static awpImage* CreateTrafficLightPalette_v()
{
	awpImage* img = NULL;
	awpCreateImage(&img, c_nVPaletteWidth, c_nVPaletteHeight, 3, AWP_BYTE);
	double dx = 86.;
	FillColorsV(img, 0, dx, 0, 255,0,255,255,0);
	FillColorsV(img, dx, 2*dx, 255, 255,0,255,0,0);
	FillColorsV(img, 2*dx, c_nVPaletteHeight, 255, 0,0,255,0,0);
	awpFlip(&img, AWP_FLIP_VERT);
	return img;
}

static awpImage* CreateTrafficLightPalette_h()
{
	awpImage* img = NULL;
	awpCreateImage(&img, c_nHPaletteWidth, c_nHPaletteHeight, 3, AWP_BYTE);
	AWPBYTE* b = (AWPBYTE*)img->pPixels;
	double dx = 86.;
	FillColorsH(img, 0, dx, 0, 255,0,255,255,0);
	FillColorsH(img, dx, 2*dx, 255, 255,0,255,0,0);
	FillColorsH(img, 2*dx, c_nHPaletteWidth, 255, 0,0,255,0,0);

	return img;
}

static awpImage* CreateFalseColorsPalette_v()
{
	awpImage* img = NULL;
	awpCreateImage(&img, c_nVPaletteWidth, c_nVPaletteHeight, 3, AWP_BYTE);
	double dx = 64;
	FillColorsV(img, 0, dx, 0, 0,255,0,255,255);
	FillColorsV(img, dx, 2*dx, 0, 255,255,0,255,0);
	FillColorsV(img, 2*dx, 3*dx, 0, 255,0,255,255,0);
	FillColorsV(img, 3*dx, c_nVPaletteHeight, 255, 255,0,255,0,0);
	awpFlip(&img, AWP_FLIP_VERT);
	return img;
}

static awpImage* CreateFalseColorsPalette_h()
{
	awpImage* img = NULL;
	awpCreateImage(&img, c_nHPaletteWidth, c_nHPaletteHeight, 3, AWP_BYTE);
	double dx = 64;
	FillColorsH(img, 0, dx, 0, 0,255,0,255,255);
	FillColorsH(img, dx, 2*dx, 0, 255,255,0,255,0);
	FillColorsH(img, 2*dx, 3*dx, 0, 255,0,255,255,0);
	FillColorsH(img, 3*dx, c_nHPaletteWidth, 255, 255,0,255,0,0);
	return img;
}

static awpImage* CreateSpectrumPalette_v()
{
	awpImage* img = NULL;
	awpCreateImage(&img, c_nVPaletteWidth, c_nVPaletteHeight, 3, AWP_BYTE);
	double dx = 43;
	FillColorsV(img, 0, dx,
			0, 0, 0,
			0, 0,255);
	FillColorsV(img, dx, 2*dx,
			0, 0,255,
			0,255,255);
	FillColorsV(img, 2*dx, 3*dx,
			0, 255,255,
			0,255,0);
	FillColorsV(img, 3*dx, 4*dx,
			0, 255,0,
			255,255,0);
	FillColorsV(img, 4*dx, 5*dx,
			255, 255,0,
			255,128,0);
	FillColorsV(img, 5*dx, c_nVPaletteHeight,
			255, 128,0,
			255,0,0);

	awpFlip(&img, AWP_FLIP_VERT);
	return img;
}

static awpImage* CreateSpectrumPalette_h()
{
	awpImage* img = NULL;
	awpCreateImage(&img, c_nHPaletteWidth, c_nHPaletteHeight, 3, AWP_BYTE);
	double dx = 43;
	FillColorsH(img, 0, dx,
			0, 0, 0,
			0, 0,255);
	FillColorsH(img, dx, 2*dx,
			0, 0,255,
			0,255,255);
	FillColorsH(img, 2*dx, 3*dx,
			0, 255,255,
			0,255,0);
	FillColorsH(img, 3*dx, 4*dx,
			0, 255,0,
			255,255,0);
	FillColorsH(img, 4*dx, 5*dx,
			255, 255,0,
			255,128,0);
	FillColorsH(img, 5*dx, c_nHPaletteWidth,
			255, 128,0,
			255,0,0);
	return img;
}

static awpImage* CreateGlowPalette_v()
{
	awpImage* img = NULL;
	awpCreateImage(&img, c_nVPaletteWidth, c_nVPaletteHeight, 3, AWP_BYTE);
	double dx = 52;
	FillColorsV(img, 0, dx,
			0, 0, 0,
			128, 0,0);
	FillColorsV(img, dx, 2*dx,
			128, 0,0,
			255,0,0);
	FillColorsV(img, 2*dx, 3*dx,
			255, 0,0,
			255,128,0);
	FillColorsV(img, 3*dx, 4*dx,
			255, 128,0,
			255,255,0);
	FillColorsV(img, 4*dx, c_nVPaletteHeight,
			255, 255,0,
			255,255,255);


	awpFlip(&img, AWP_FLIP_VERT);
	return img;
}

static awpImage* CreateGlowPalette_h()
{
	awpImage* img = NULL;
	awpCreateImage(&img, c_nHPaletteWidth, c_nHPaletteHeight, 3, AWP_BYTE);
	double dx = 52;
	FillColorsH(img, 0, dx,
			0, 0, 0,
			128, 0,0);
	FillColorsH(img, dx, 2*dx,
			128, 0,0,
			255,0,0);
	FillColorsH(img, 2*dx, 3*dx,
			255, 0,0,
			255,128,0);
	FillColorsH(img, 3*dx, 4*dx,
			255, 128,0,
			255,255,0);
	FillColorsH(img, 4*dx, c_nHPaletteWidth,
			255, 255,0,
			255,255,255);
	return img;
}

static awpImage* CreateSpectrum303Palette_v()
{
	awpImage* img = NULL;
	awpCreateImage(&img, c_nVPaletteWidth, c_nVPaletteHeight, 3, AWP_BYTE);
	double dx = 32;
	FillColorsV(img, 0, dx,
			0, 0, 0,
			0, 0,255);
	FillColorsV(img, dx, 2*dx,
			0, 0,255,
			0,255,0);
	FillColorsV(img, 2*dx, 3*dx,
			0, 255,0,
			128,255,0);
	FillColorsV(img, 3*dx, 4*dx,
			128, 255,0,
			255,255,0);
	FillColorsV(img, 4*dx, 5*dx,
			255, 255,0,
			255,192,0);
	FillColorsV(img, 5*dx, 6*dx,
			255, 192,0,
			255,128,0);
	FillColorsV(img, 6*dx, 7*dx,
			255, 128,0,
			255,92,0);

	FillColorsV(img,7*dx, c_nVPaletteHeight,
			255, 92,0,
			255,0,0);

	awpFlip(&img, AWP_FLIP_VERT);
	return img;
}

static awpImage* CreateSpectrum303Palette_h()
{
	awpImage* img = NULL;
	awpCreateImage(&img, c_nHPaletteWidth, c_nHPaletteHeight, 3, AWP_BYTE);
	double dx = 32;
	FillColorsH(img, 0, dx,
			0, 0, 0,
			0, 0,255);
	FillColorsH(img, dx, 2*dx,
			0, 0,255,
			0,255,0);
	FillColorsH(img, 2*dx, 3*dx,
			0, 255,0,
			128,255,0);
	FillColorsH(img, 3*dx, 4*dx,
			128, 255,0,
			255,255,0);
	FillColorsH(img, 4*dx, 5*dx,
			255, 255,0,
			255,192,0);
	FillColorsH(img, 5*dx, 6*dx,
			255, 192,0,
			255,128,0);
	FillColorsH(img, 6*dx, 7*dx,
			255, 128,0,
			255,92,0);
	FillColorsH(img,7*dx, c_nHPaletteWidth,
			255, 92,0,
			255,0,0);

	return img;
}

//---------------------------------------------------------------------------
__fastcall TPhPalette::TPhPalette(TComponent* Owner)
	: TCustomControl(Owner)
{
	m_color = clGray;
	m_textColor = clBlack;
	m_ticksVisible = true;
	m_min = 0;
	m_max =  255;


	TDIBImage* dibv = new TDIBImage();
	TDIBImage* dibh = new TDIBImage();
	m_vimage = dibv;
	m_himage = dibh;

//	SetPaletteType(phpalGrayscale);
	m_paletteType = phpalUnknown;
//	PaletteType = phpalGrayscale;
}
void __fastcall TPhPalette::DrawVTicks(TCanvas* c)
{
	 if (!m_ticksVisible) {
		 return;
	 }
	 c->Pen->Color = m_textColor;
	 c->Font->Color = m_textColor;
	 int w = 4;
	 for (int i = 0; i <= 50; i++)
	 {
		if (i %10 == 0)
		{
			w = 10;
			c->TextOutW(PaletteRight() + 2 + w, Height-20 - i*(Height-20)/50, FormatFloat("0.00", m_min + i*(m_max-m_min) / 50));
		}
		else
			w = 4;
		c->MoveTo(PaletteRight(),     Height-10 - i*(Height -20) / 50);
		c->LineTo(PaletteRight() + w, Height-10 - i*(Height-20)/50);
	 }
}

void __fastcall TPhPalette::DrawHTicks(TCanvas* c)
{
	 if (!m_ticksVisible) {
		 return;
	 }
	 c->Pen->Color = m_textColor;
	 c->Font->Color = m_textColor;
	 int h = 4;

	 for (int i = 0; i <= 50; i++)
	 {
		if (i %10 == 0)
		{
			h = 10;
			UnicodeString str = FormatFloat("0.00", m_min + i*(m_max-m_min) / 50);
			int x = c_nLeftMargin + i*(Width - 2*c_nLeftMargin) /50 - c->TextWidth(str) /2;
			if (x < 0) {
				x = c_nLeftMargin;
			}
			if (x  +  c->TextWidth(str) > Width) {
				x = Width - 2 - c->TextWidth(str);
			}
			c->TextOutW(x, PaletteBottom(c)+h+2, str);
		}
		else
			h = 4;
		c->MoveTo(c_nLeftMargin + i*(Width - 2*c_nLeftMargin) /50, PaletteBottom(c));
		c->LineTo(c_nLeftMargin + i*(Width - 2*c_nLeftMargin) /50, PaletteBottom(c)+h);
	 }

}

int __fastcall TPhPalette::PaletteRight()
{
   int w = Width -10;
   if (m_ticksVisible) {
	   w -=  TextWidth();
	   w -= 5;
   }
   return  w;
}

int __fastcall TPhPalette::PaletteBottom(TCanvas* c)
{
	int h = Height - c_nBottomMargin;
	if (m_ticksVisible) {
		h -= c->TextHeight("00.00");
		h -= 10; // ticks height
		h -= 2;  // margin
	}

    return h;
}


void __fastcall TPhPalette::SetTicksVisible(bool value)
{
	m_ticksVisible = value;
    Paint();
}


int __fastcall TPhPalette::TextWidth()
{
	int wmin = Canvas->TextWidth(FormatFloat("0.00", m_min));
	int wmax = Canvas->TextWidth(FormatFloat("0.00", m_max));
	return wmin>wmax?wmin:wmax;
}

void __fastcall TPhPalette::SetColor(TColor color)
{
	m_color = color;
	Paint();
}

void __fastcall TPhPalette::SetTextColor(TColor color)
{
	m_textColor = color;
	Paint();
}

void __fastcall TPhPalette::SetMinValue(double value)
{
  if (value < m_max)
  {
	 m_min = value;
	 Paint();
  }
}
void __fastcall TPhPalette::SetMaxValue(double value)
{
	if (value > m_min) {
		m_max = value;
		Paint();
	}
}


void __fastcall     TPhPalette::Paint(void)
{
   Graphics::TBitmap *bm = new Graphics::TBitmap();
   bm->Width  = Width;
   bm->Height = Height;
   bm->Canvas->Brush->Color = m_color;
   bm->Canvas->FillRect(ClientRect);
   TRect src_r, dst_r;
   if (Height > Width)
   {
		TDIBImage* dib = (TDIBImage*)m_vimage;
		if (!dib->Empty)
		{
		   dst_r.Left   = c_nLeftMargin;
		   dst_r.Right  = PaletteRight();
		   dst_r.Top    = c_nTopMargin;
		   dst_r.Bottom = Height-c_nBottomMargin;
		   src_r.init(0,0,c_nVPaletteWidth,c_nVPaletteHeight);
		   unsigned char* pDIB = dib->OpenPixels();

		   ::SetStretchBltMode(bm->Canvas->Handle, HALFTONE);
		   ::StretchDIBits(bm->Canvas->Handle,
			  dst_r.Left,
			  dst_r.Top,
			  dst_r.Width(),
			  dst_r.Height(),
			  src_r.Left,
			  src_r.Top,
			  src_r.Width(),
			  src_r.Height(),
			  pDIB,
			  &(dib->DIBInfo),
			  DIB_RGB_COLORS,
			  SRCCOPY);
		   dib->ClosePixels();
		   DrawVTicks(bm->Canvas);
		}
   }
   else
   {
		TDIBImage* dib = (TDIBImage*)m_himage;
		if (!dib->Empty)
		{
		   dst_r.Left   = c_nLeftMargin;
		   dst_r.Right  = Width - c_nLeftMargin;
		   dst_r.Top    = c_nTopMargin;
		   dst_r.Bottom = PaletteBottom(bm->Canvas);
		   src_r.init(0,0,c_nHPaletteWidth,c_nHPaletteHeight);
		   unsigned char* pDIB = dib->OpenPixels();

		   ::SetStretchBltMode(bm->Canvas->Handle, HALFTONE);
		   ::StretchDIBits(bm->Canvas->Handle,
			  dst_r.Left,
			  dst_r.Top,
			  dst_r.Width(),
			  dst_r.Height(),
			  src_r.Left,
			  src_r.Top,
			  src_r.Width(),
			  src_r.Height(),
			  pDIB,
			  &(dib->DIBInfo),
			  DIB_RGB_COLORS,
			  SRCCOPY);
		   dib->ClosePixels();
		   DrawHTicks(bm->Canvas);
		}
   }
   Canvas->CopyMode = cmSrcCopy;
   Canvas->Draw(0,0,bm);
   delete bm;
}
//phpalGrayscale, phpalBluered, phpalOcean, phpalGlow, phpalTraffic, phpalFalseColors, phpalSpectum, phpalSpectrum2} EPhPalette;

void __fastcall TPhPalette::SetPaletteType(EPhPalette value)
{
	this->m_paletteType = value;

	awpImage* img = NULL;
	TDIBImage* dibh = (TDIBImage*)m_himage;
	TDIBImage* dibv = (TDIBImage*)m_vimage;

	switch(this->m_paletteType)
	{
		case phpalGrayscale:
			img = CreateGrayPalette_v();
			dibv->SetAWPImage(img);
			awpReleaseImage(&img);
			img = CreateGrayPalette_h();
			dibh->SetAWPImage(img);
		break;
		case phpalBluered:
			img = CreateBlueRedPalette_v();
			dibv->SetAWPImage(img);
			awpReleaseImage(&img);
			img = CreateBlueRedPalette_h();
			dibh->SetAWPImage(img);
		break;
		case phpalOcean:
			img = CreateOceanPalette_v();
			dibv->SetAWPImage(img);
			awpReleaseImage(&img);
			img = CreateOceanPalette_h();
			dibh->SetAWPImage(img);
		break;
		case phpalGlow:
			img = CreateGlowPalette_v();
			dibv->SetAWPImage(img);
			awpReleaseImage(&img);
			img = CreateGlowPalette_h();
			dibh->SetAWPImage(img);
		break;
		case phpalTraffic:
			img = CreateTrafficLightPalette_v();
			dibv->SetAWPImage(img);
			awpReleaseImage(&img);
			img = CreateTrafficLightPalette_h();
			dibh->SetAWPImage(img);
		break;
		case phpalFalseColors:
			img = CreateFalseColorsPalette_v();
			dibv->SetAWPImage(img);
			awpReleaseImage(&img);
			img = CreateFalseColorsPalette_h();
			dibh->SetAWPImage(img);
		break;
		case phpalSpectum:
			img = CreateSpectrumPalette_v();
			dibv->SetAWPImage(img);
			awpReleaseImage(&img);
			img = CreateSpectrumPalette_h();
			dibh->SetAWPImage(img);
		break;
		case phpalSpectrum2:
			img = CreateSpectrum303Palette_v();
			dibv->SetAWPImage(img);
			awpReleaseImage(&img);
			img = CreateSpectrum303Palette_h();
			dibh->SetAWPImage(img);
		break;
	}
	CreatePalette((void*)img);
	awpReleaseImage(&img);
	Paint();
}
/*
	преобразование awp изображения в изображение в условных цветах
	входное изображение должно иметь либо один либо три канала типа AWP_BYTE
	выходное изображение - три канала AWP_BYTE
*/
void* __fastcall TPhPalette::ApplyPalette(void* image)
{
	awpImage* img = (awpImage*)image;
	if (awpCheckImage(img) != AWP_OK)
		return NULL;
	if (img->dwType != AWP_BYTE)
		return NULL;
	if (img->bChannels != 1 && img->bChannels != 3)
		return NULL;
	awpImage* img1 = NULL;
	if (awpCopyImage(img, &img1) != AWP_OK)
		return NULL;

	if (img1->bChannels == 3)
	{
		if (awpConvert(img1, AWP_CONVERT_3TO1_BYTE) != AWP_OK) {
			awpReleaseImage(&img1);
			return NULL;
		}
	}

	awpImage* res = NULL;
	if (awpCreateImage(&res, img->sSizeX, img->sSizeY, 3, AWP_BYTE) != AWP_OK)
		return NULL;
	AWPBYTE* br = (AWPBYTE*)res->pPixels;
	AWPBYTE* b  = (AWPBYTE*)img1->pPixels;

	for (int i = 0; i < res->sSizeX*res->sSizeY; i++) {
		br[3*i]   = m_palette[b[i]].m_blue;
		br[3*i+1] = m_palette[b[i]].m_green;
		br[3*i+2] = m_palette[b[i]].m_red;
	}

	awpReleaseImage(&img1);
	return res;
}

void __fastcall TPhPalette::CreatePalette(void* image)
{
	awpImage* img = (awpImage*)image;
	if (awpCheckImage(img) != AWP_OK)
		return;
	AWPBYTE* b = (AWPBYTE*)img->pPixels;
	if (img->sSizeX == 256)
	{
		// horizontal
		for (int i=0; i < 256; i++)
		{
			m_palette[i].m_blue = b[3*i];
			m_palette[i].m_green = b[3*i+1];
			m_palette[i].m_red = b[3*i+2];
		}
	}
	else if (img->sSizeY == 256) {
		// vertical
		for (int i=0; i < 256; i++)
		{
			m_palette[i].m_blue = b[256*3*i];
			m_palette[i].m_green = b[256*3*i+1];
			m_palette[i].m_red = b[256*3*i+2];
		}
	 }

}



//---------------------------------------------------------------------------
namespace Phpalette
{
	void __fastcall PACKAGE Register()
	{
		TComponentClass classes[1] = {__classid(TPhPalette)};
		RegisterComponents(L"Photon", classes, 0);
	}
}
//---------------------------------------------------------------------------
