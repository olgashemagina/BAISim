/*M
//
//      awpipl2 image processing and image analysis library
//		File: awpColor.c
//		Purpose: awpipl2 color space convertion 
//		Autors: Alex A.Telnykh (tlnykha@yahoo.com)
//      CopyRight 2004-2020 (c) NN-Videolab.net
//M*/

#include "_awpipl.h"

AWPRESULT awpRGBtoHSV(AWPBYTE r, AWPBYTE g, AWPBYTE b, AWPDOUBLE* h, AWPDOUBLE* s, AWPDOUBLE* v)
{
	return _awp_rgb_to_hsv(r,g,b,h,s,v);
}
AWPRESULT awpRGBtoHSL(AWPBYTE r, AWPBYTE g, AWPBYTE b, AWPDOUBLE* h, AWPDOUBLE* s, AWPDOUBLE* l)
{
	return _awp_rgb_to_hsl(r,g,b,h,s,l);
}
AWPRESULT awpRGBtoXYZ(AWPBYTE r, AWPBYTE g, AWPBYTE b, AWPDOUBLE* x, AWPDOUBLE* y, AWPDOUBLE* z)
{
	return _awp_rgb_to_xyz(r,g,b,x,y,z);
}
AWPRESULT awpRGBtoLAB(AWPBYTE r, AWPBYTE g, AWPBYTE b, AWPDOUBLE* l, AWPDOUBLE* a, AWPDOUBLE* bb)
{
	return _awp_rgb_to_lab(r,g,b,l,a,bb);
}
AWPRESULT awpRGBtoWeb(AWPBYTE r, AWPBYTE g, AWPBYTE b, AWPINT* web)
{
	AWPINT result;
	if (web == NULL)
		return AWP_BADARG;

	result = r; 
	result = result << 8;
	result |= g;
	result = result << 8;
	result |= b;

	*web = result;
	return AWP_OK;
}

AWPRESULT awpHSVtoRGB(AWPDOUBLE h, AWPDOUBLE s, AWPDOUBLE v, AWPBYTE* r, AWPBYTE* g, AWPBYTE* b)
{
	return _awp_hsv_to_rgb(h,s,v,r,g,b);
}
AWPRESULT awpHSLtoRGB(AWPDOUBLE h, AWPDOUBLE s, AWPDOUBLE l, AWPBYTE* r, AWPBYTE* g, AWPBYTE* b)
{
	return _awp_hsl_to_rgb(h,s,l,r,g,b);
}
AWPRESULT awpXYZtoRGB(AWPDOUBLE x, AWPDOUBLE y, AWPDOUBLE z, AWPBYTE* r, AWPBYTE* g, AWPBYTE* b)
{
	return _awp_xyz_to_rgb(x,y,z, r,g,b);
}
AWPRESULT awpLABtoRGB(AWPDOUBLE l, AWPDOUBLE a, AWPDOUBLE b0, AWPBYTE* r, AWPBYTE* g, AWPBYTE* b)
{
	return _awp_lab_to_rgb(l,a,b0,r,g,b);
}

AWPRESULT awpWebtoRGB(AWPINT web, AWPBYTE* r, AWPBYTE* g, AWPBYTE* b)
{
    AWPINT value;
	if (r == NULL || g == NULL || b == NULL)
		return AWP_BADARG;
    value = web & 0xFF0000;
	*r = value >> 16;
    value = web & 0x00FF00;
	*g = value >> 8;
	*b = web & 0x0000FF;
	return AWP_OK;
}

// Convert HSV image to RGB image
AWPRESULT awpHSVtoRGBImage(const awpImage* Image, awpImage** ppRGB)
{
	AWPINT i = 0;
	AWPINT c = 0;
	awpColor* pc = NULL;
	awpColor* pcRGB = NULL;
	AWPRESULT res = AWP_OK;
	AWPDOUBLE h, s, v;
	AWPBYTE r, g, b;

	res = awpCheckImage(Image);
	if (res != AWP_OK)
		return res;

	if (Image->dwType != AWP_BYTE && Image->bChannels != 3)
		return AWP_BADARG;

	res = awpCreateImage(ppRGB, Image->sSizeX,
		Image->sSizeY, 3, AWP_BYTE);
	if (res != AWP_OK)
		return res;

	pc = (awpColor*)Image->pPixels;
	pcRGB = (awpColor*)(*ppRGB)->pPixels;
	c = Image->sSizeX*Image->sSizeY;
	for (i = 0; i < c; i++)
	{
		h = pc[i].bRed / 255.0;
		s = pc[i].bGreen / 255.0;
		v = pc[i].bBlue / 255.0;
		_awp_hsv_to_rgb(h, s, v, &r, &g, &b);
		pcRGB[i].bRed = b;
		pcRGB[i].bGreen = g;
		pcRGB[i].bBlue = r;
	}

	return AWP_OK;
}

AWPRESULT awpRGBtoHSVImage(const awpImage* Image, awpImage** ppHSV)
{
	AWPINT i = 0;
	AWPINT c = 0;
	awpColor* pc = NULL;
	awpColor* pcHSV = NULL;
	AWPRESULT res = AWP_OK;
	AWPDOUBLE h, s, v;

	res = awpCheckImage(Image);
	if (res != AWP_OK)
		return res;
	if (Image->dwType != AWP_BYTE || Image->bChannels != 3)
		return AWP_BADARG;

	res = awpCreateImage(ppHSV, Image->sSizeX,
		Image->sSizeY, 3, AWP_BYTE);
	if (res != AWP_OK)
		return res;

	pc = (awpColor*)Image->pPixels;
	pcHSV = (awpColor*)(*ppHSV)->pPixels;
	c = Image->sSizeX*Image->sSizeY;
	for (i = 0; i < c; i++)
	{
		_awp_rgb_to_hsv(pc[i].bBlue, pc[i].bGreen, pc[i].bRed, &h, &s, &v);
		if (h == -1)
		{
			pcHSV[i].bRed = 0;
			pcHSV[i].bGreen = 0;
			pcHSV[i].bBlue = 0;

			continue;
		}
		h /=360.;
        h *=255;
        s *= 255.0;
        v *= 255.0;
		pcHSV[i].bRed = (AWPBYTE)h;
		pcHSV[i].bGreen = (AWPBYTE)s;
		pcHSV[i].bBlue = (AWPBYTE)v;
	}
	return AWP_OK;
}


#define SHIFT 12

static const AWPINT div_table[] =
{
	0, 1044480, 522240, 348160, 261120, 208896, 174080, 149211,
	130560, 116053, 104448, 94953, 87040, 80345, 74606, 69632,
	65280, 61440, 58027, 54973, 52224, 49737, 47476, 45412,
	43520, 41779, 40172, 38684, 37303, 36017, 34816, 33693,
	32640, 31651, 30720, 29842, 29013, 28229, 27486, 26782,
	26112, 25475, 24869, 24290, 23738, 23211, 22706, 22223,
	21760, 21316, 20890, 20480, 20086, 19707, 19342, 18991,
	18651, 18324, 18008, 17703, 17408, 17123, 16846, 16579,
	16320, 16069, 15825, 15589, 15360, 15137, 14921, 14711,
	14507, 14308, 14115, 13926, 13743, 13565, 13391, 13221,
	13056, 12895, 12738, 12584, 12434, 12288, 12145, 12006,
	11869, 11736, 11605, 11478, 11353, 11231, 11111, 10995,
	10880, 10768, 10658, 10550, 10445, 10341, 10240, 10141,
	10043, 9947, 9854, 9761, 9671, 9582, 9495, 9410,
	9326, 9243, 9162, 9082, 9004, 8927, 8852, 8777,
	8704, 8632, 8561, 8492, 8423, 8356, 8290, 8224,
	8160, 8097, 8034, 7973, 7913, 7853, 7795, 7737,
	7680, 7624, 7569, 7514, 7461, 7408, 7355, 7304,
	7253, 7203, 7154, 7105, 7057, 7010, 6963, 6917,
	6872, 6827, 6782, 6739, 6695, 6653, 6611, 6569,
	6528, 6487, 6447, 6408, 6369, 6330, 6292, 6254,
	6217, 6180, 6144, 6108, 6073, 6037, 6003, 5968,
	5935, 5901, 5868, 5835, 5803, 5771, 5739, 5708,
	5677, 5646, 5615, 5585, 5556, 5526, 5497, 5468,
	5440, 5412, 5384, 5356, 5329, 5302, 5275, 5249,
	5222, 5196, 5171, 5145, 5120, 5095, 5070, 5046,
	5022, 4998, 4974, 4950, 4927, 4904, 4881, 4858,
	4836, 4813, 4791, 4769, 4748, 4726, 4705, 4684,
	4663, 4642, 4622, 4601, 4581, 4561, 4541, 4522,
	4502, 4483, 4464, 4445, 4426, 4407, 4389, 4370,
	4352, 4334, 4316, 4298, 4281, 4263, 4246, 4229,
	4212, 4195, 4178, 4161, 4145, 4128, 4112, 4096
};

#define mmax(a,b)    ((b) + (((a)-(b))&((a)>(b)?-1:0)))
#define mmin(a,b)    ((a) - (((a)-(b))&((a)>(b)?-1:0)))


// Added by Radzhabova Julia
// Converts RGB image to HSL image
AWPRESULT awpRGBtoHSLImage(const awpImage* Image, awpImage** ppHSL)
{

	const AWPINT planes = 3;
	AWPINT   x, y, x1;
	AWPINT b, g, r;
	AWPINT h, s, v;
	AWPINT vmin, diff;
	AWPINT vr, vg;
	AWPINT        min_ch_val[6];
	AWPINT        max_ch_val[6];

	AWPBYTE* src = (AWPBYTE*)Image->pPixels;
	AWPINT    src_pitch = Image->sSizeX;
	AWPBYTE* dst = NULL;
	awpImage* im = NULL;
	AWPINT    dst_pitch = 0;


	AWPINT i = 0;
	AWPRESULT res = AWP_OK;

	res = awpCheckImage(Image);
	if (res != AWP_OK)
		return res;

	if (Image->dwType != AWP_BYTE && Image->bChannels != 3)
		return res;

	res = awpCreateImage(&im, Image->sSizeX,
		Image->sSizeY, 3, AWP_BYTE);
	if (res != AWP_OK)
		return res;

	dst = (AWPBYTE*)im->pPixels;
	dst_pitch = im->sSizeX;

	for (i = 0; i < 6; i++)
	{
		min_ch_val[i] = 0;
		max_ch_val[i] = 255;
	}


	for (y = 0; y < Image->sSizeY; y++, src += src_pitch, dst += dst_pitch)
		for (x = 0, x1 = 0; x < Image->sSizeX * planes; x += planes, x1++)
		{

			b = src[x];
			g = src[x + 1];
			r = src[x + 2];

			// convert rgb->hsv
			{

				v = mmax(r, g);
				v = mmax(v, b);
				vmin = mmin(r, g);
				vmin = mmin(vmin, b);

				diff = v - vmin;
				vr = v == r ? -1 : 0;
				vg = v == g ? -1 : 0;

				s = diff*div_table[v] >> SHIFT;
				h = (vr&(g - b)) + (~vr&((vg&(b - r + 2 * diff)) + ((~vg)&(r - g + 4 * diff))));
				h = ((h*div_table[diff] * 15 + (1 << (SHIFT + 6))) >> (7 + SHIFT)) + (h < 0 ? 30 * 6 : 0);
			}

			if (s >= min_ch_val[1] && v >= min_ch_val[2] && v <= max_ch_val[2])
				dst[x1] = (AWPBYTE)h;
			else
				dst[x1] = 0;
		}
	/*
	pc = (awpColor*)Image->pPixels;
	pcHSL = (awpColor*)(*ppHSL)->pPixels;
	c = Image->sSizeX*Image->sSizeY;
	for (i = 0; i < c; i++)
	{
	_awpRGBtoHSL(pc[i].bBlue ,pc[i].bGreen ,pc[i].bRed ,&h, &s, &l );
	h *= 255.0; s *= 255.0; l *= 255.0;
	pcHSL[i].bRed = (AWPBYTE)h;
	pcHSL[i].bGreen = (AWPBYTE)s;
	pcHSL[i].bBlue = (AWPBYTE)l;
	}
	*/
	*ppHSL = im;
	return AWP_OK;
}

// Convert HSL image to RGB image
AWPRESULT awpHSLtoRGBImage(const awpImage* Image, awpImage** ppRGB)
{
	AWPINT i = 0;
	AWPINT c = 0;
	awpColor* pc = NULL;
	awpColor* pcRGB = NULL;
	AWPRESULT res = AWP_OK;
	AWPDOUBLE h, s, l;
	AWPBYTE r, g, b;

	res = awpCheckImage(Image);
	if (res != AWP_OK)
		return res;

	if (Image->dwType != AWP_BYTE && Image->bChannels != 3)
		return AWP_BADARG;

	res = awpCreateImage(ppRGB, Image->sSizeX,
		Image->sSizeY, 3, AWP_BYTE);
	if (res != AWP_OK)
		return res;

	pc = (awpColor*)Image->pPixels;
	pcRGB = (awpColor*)(*ppRGB)->pPixels;
	c = Image->sSizeX*Image->sSizeY;
	for (i = 0; i < c; i++)
	{
		h = pc[i].bRed / 255.0;
		s = pc[i].bGreen / 255.0;
		l = pc[i].bBlue / 255.0;
		_awp_hsl_to_rgb(h, s, l, &r, &g, &b);
		pcRGB[i].bRed = b;
		pcRGB[i].bGreen = g;
		pcRGB[i].bBlue = r;
	}

	return AWP_OK;
}
