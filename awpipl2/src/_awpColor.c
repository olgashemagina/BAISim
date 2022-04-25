/*
//
//  RCS:
//      $Source: _awpColor.c
//      $Revision: 1.0 $
//      $Date:
//
//    Purpose: AWPIPL source file
//    Contents: internal source code for color space convertion 
//    Authors : Alexander Telnykh
*/

#include "_awpipl.h"

#define _AWP_NORM_RGB_ \
AWPDOUBLE r;\
AWPDOUBLE g;\
AWPDOUBLE b;\
r = (AWPDOUBLE)rr / 255.;\
g = (AWPDOUBLE)gg / 255.;\
b = (AWPDOUBLE)bb / 255.;\


AWPRESULT _awp_rgb_to_hsv(AWPBYTE rr, AWPBYTE gg, AWPBYTE bb, AWPDOUBLE* h, AWPDOUBLE* s, AWPDOUBLE* v)
{
		
	AWPDOUBLE max, min;
	
	_AWP_NORM_RGB_

	max = r > g ? r : g;
	max = max > b ? max : b;
	min = r < g ? r : g;
	min = min < b ? min : b;

	*v = max;
	if (max == 0)
	{
		*s = 0;
	}
	else
	{
		*s = 1 - min / max;
	}

	if (max == min)
	{
		*h = 0;
	}
	else if (max == r && g >= b)
	{
		*h = 60 * (g - b) / (max - min);
	}
	else if (max == r && g < b)
	{
		*h = 60 * (g - b) / (max - min) + 360;
	}
	else if (max == g)
	{
		*h = 60 * (b - r) / (max - min) + 120;
	}
	else if (max == b)
	{
		*h = 60 * (r - g) / (max - min) + 240;
	}


	return AWP_OK;
}
AWPRESULT _awp_rgb_to_hsl(AWPBYTE rr, AWPBYTE gg, AWPBYTE bb, AWPDOUBLE* h, AWPDOUBLE* s, AWPDOUBLE* l)
{
	AWPDOUBLE max, min;

	_AWP_NORM_RGB_

	max = r > g ? r : g;
	max = max > b ? max : b;
	min = r < g ? r : g;
	min = min < b ? min : b;
	*l = (max + min) / 2.;
	if (*l == 0 || max == min)
	{
		s = 0;
	}
	else if (*l <= 0.5)
	{
		*s = (max - min) / (2 * (*l));
	}
	else
	{
		*s = (max - min) / (2 - 2 * (*l));
	}

	if (max == min)
	{
		*h = 0;
	}
	else if (max == r && g >= b)
	{
		*h = 60 * (g - b) / (max - min);
	}
	else if (max == r && g < b)
	{
		*h = 60 * (g - b) / (max - min) + 360;
	}
	else if (max == g)
	{
		*h = 60 * (b - r) / (max - min) + 120;
	}
	else if (max == b)
	{
		*h = 60 * (r - g) / (max - min) + 240;
	}


	return AWP_OK;
}
AWPRESULT _awp_rgb_to_xyz(AWPBYTE rr, AWPBYTE gg, AWPBYTE bb, AWPDOUBLE* x, AWPDOUBLE* y, AWPDOUBLE* z)
{
   AWPDOUBLE var_R, var_G, var_B;
	_AWP_NORM_RGB_

	var_R = r;
	var_G = g;
	var_B = b;

	if (var_R > 0.04045)
		var_R = pow(((var_R + 0.055) / 1.055), 2.4);
	else
		var_R = var_R / 12.92;
	if (var_G > 0.04045)
		var_G = pow(((var_G + 0.055) / 1.055), 2.4);
	else
		var_G = var_G / 12.92;
	if (var_B > 0.04045)
		var_B = pow(((var_B + 0.055) / 1.055), 2.4);
	else
		var_B = var_B / 12.92;

	var_R = var_R * 100;
	var_G = var_G * 100;
	var_B = var_B * 100;

	*x = var_R * 0.4124 + var_G * 0.3576 + var_B * 0.1805;
	*y = var_R * 0.2126 + var_G * 0.7152 + var_B * 0.0722;
	*z = var_R * 0.0193 + var_G * 0.1192 + var_B * 0.9505;

	return AWP_OK;
}

AWPRESULT _awp_rgb_to_lab(AWPBYTE rr, AWPBYTE gg, AWPBYTE bb, AWPDOUBLE* l, AWPDOUBLE* a, AWPDOUBLE* b0)
{
	AWPDOUBLE x, y, z;

	_awp_rgb_to_xyz(rr, gg, bb, &x, &y, &z);
	_awp_xyz_to_lab(x, y, z, l, a, b0);

	return AWP_OK;
}

AWPRESULT _awp_hsv_to_rgb(AWPDOUBLE h, AWPDOUBLE s, AWPDOUBLE v, AWPBYTE* rr, AWPBYTE* gg, AWPBYTE* bb)
{
	AWPINT i;
	AWPDOUBLE f, p, q, t;
	AWPDOUBLE r, g, b;

	if (s == 0)
	{
		// achromatic (grey)
		*rr = *gg = *bb = (AWPBYTE)(255 * v);
		return AWP_OK;
	}
	h *= 6;			// sector 0 to 5
	i = (AWPINT)floor(h);
	f = h - i;
	p = v * (1 - s);
	q = v * (1 - s * f);
	t = v * (1 - s * (1 - f));
	switch (i) {
	case 0:
		r = v;
		g = t;
		b = p;
		break;
	case 1:
		r = q;
		g = v;
		b = p;
		break;
	case 2:
		r = p;
		g = v;
		b = t;
		break;
	case 3:
		r = p;
		g = q;
		b = v;
		break;
	case 4:
		r = t;
		g = p;
		b = v;
		break;
	default:		// case 5:
		r = v;
		g = p;
		b = q;
		break;
	}

	*rr = (AWPBYTE)(255 * r);
	*gg = (AWPBYTE)(255 * g);
	*bb = (AWPBYTE)(255 * b);

	return AWP_OK;
}
AWPRESULT _awp_hsl_to_rgb(AWPDOUBLE h, AWPDOUBLE s, AWPDOUBLE l, AWPBYTE* rr, AWPBYTE* gg, AWPBYTE* bb)
{
	AWPDOUBLE v;
	AWPDOUBLE r, g, b;

	r = g = b = 0.0;

	v = (l <= 0.5) ? (l * (1.0 + s)) : (l + s - l * s);

	if (v <= 0) {
		r = g = b = 0.0;
	}
	else {
		AWPDOUBLE m;
		AWPDOUBLE sv;
		AWPINT sextant;
		AWPDOUBLE fract, vsf, mid1, mid2;

		m = l + l - v;
		sv = (v - m) / v;
		h *= 6.0;
		sextant = (AWPINT)h;
		fract = h - sextant;
		vsf = v * sv * fract;
		mid1 = m + vsf;
		mid2 = v - vsf;
		switch (sextant) {
		case 0: r = v; g = mid1; b = m; break;
		case 1: r = mid2; g = v; b = m; break;
		case 2: r = m; g = v; b = mid1; break;
		case 3: r = m; g = mid2; b = v; break;
		case 4: r = mid1; g = m; b = v; break;
		case 5: r = v; g = m; b = mid2; break;
		}
	}

	*rr = (AWPBYTE)(255 * r);
	*gg = (AWPBYTE)(255 * g);
	*bb = (AWPBYTE)(255 * b);

	return AWP_OK;
}
AWPRESULT _awp_xyz_to_rgb(AWPDOUBLE x, AWPDOUBLE y, AWPDOUBLE z, AWPBYTE* r, AWPBYTE* g, AWPBYTE* b)
{
	AWPDOUBLE rr, gg, bb;
	
	AWPDOUBLE var_x = x / 100.;
	AWPDOUBLE var_y = y / 100.;
	AWPDOUBLE var_z = z / 100.;

	AWPDOUBLE var_r = 3.2406*var_x - 1.5372*var_y - 0.4986*var_z;
	AWPDOUBLE var_g = -0.9689*var_x + 1.8758*var_y + 0.0415*var_z;
	AWPDOUBLE var_b = 0.0557*var_x - 0.2040*var_y + 1.0570*var_z;

	if (var_r > 0.0031308)
		var_r = 1.055*pow(var_r, 1. / 2.4) - 0.055;
	else
		var_r = 12.92*var_r;

	if (var_g > 0.0031308)
		var_g = 1.055*pow(var_g, 1. / 2.4) - 0.055;
	else
		var_g = 12.92*var_g;

	if (var_b > 0.0031308)
		var_b = 1.055*pow(var_b, 1. / 2.4) - 0.055;
	else
		var_b = 12.92*var_b;

	rr = var_r < 0 ? 0 : var_r;
	gg = var_g < 0 ? 0 : var_g;
	bb = var_b < 0 ? 0 : var_b;

	*r = (AWPBYTE)(floor(255.*rr + 0.5) > 255 ? 255 : floor(255.*rr + 0.5));
	*g = (AWPBYTE)(floor(255.*gg + 0.5)  > 255 ? 255 : floor(255.*gg + 0.5));
	*b = (AWPBYTE)(floor(255.*bb + 0.5)  > 255 ? 255 : floor(255.*bb + 0.5));

	return AWP_OK;
}
AWPRESULT _awp_lab_to_rgb(AWPDOUBLE l, AWPDOUBLE a, AWPDOUBLE bb, AWPBYTE* r, AWPBYTE* g, AWPBYTE* b)
{
	AWPDOUBLE x, y, z;

	_awp_lab_to_xyz(l, a, bb, &x, &y, &z);
	_awp_xyz_to_rgb(x, y, z, r, g, b);

	return AWP_OK;
}
AWPRESULT _awp_lab_to_xyz(AWPDOUBLE l, AWPDOUBLE a, AWPDOUBLE b, AWPDOUBLE* x, AWPDOUBLE* y, AWPDOUBLE* z)
{
	const AWPDOUBLE ref_x = 95.047;
	const AWPDOUBLE ref_y = 100.00;
	const AWPDOUBLE ref_z = 108.883;

	AWPDOUBLE var_y = (l + 16) / 116.;
	AWPDOUBLE var_x = a / 500. + var_y;
	AWPDOUBLE var_z = var_y - b / 200.;

	if (pow(var_y, 3) > 0.008856)
		var_y = pow(var_y, 3);
	else
		var_y = (var_y - 16. / 116.) / 7.787;

	if (pow(var_x, 3) > 0.008856)
		var_x = pow(var_x, 3);
	else
		var_x = (var_x - 16. / 116.) / 7.787;

	if (pow(var_z, 3) > 0.008856)
		var_z = pow(var_z, 3);
	else
		var_z = (var_z - 16. / 116.) / 7.787;

	*x = ref_x*var_x;
	*y = ref_y*var_y;
	*z = ref_z*var_z;


	return AWP_OK;
}
AWPRESULT _awp_xyz_to_lab(AWPDOUBLE x, AWPDOUBLE y, AWPDOUBLE z, AWPDOUBLE* l, AWPDOUBLE* a, AWPDOUBLE* b)
{
	AWPDOUBLE var_x = x / 95.047;
	AWPDOUBLE var_y = y / 100.;
	AWPDOUBLE var_z = z / 108.883;

	if (var_x > 0.008856)
		var_x = pow(var_x, 1. / 3.);
	else
		var_x = 7.787*var_x + 16. / 116.;
	if (var_y > 0.008856)
		var_y = pow(var_y, 1. / 3.);
	else
		var_y = 7.787*var_y + 16. / 116.;
	if (var_z > 0.008856)
		var_z = pow(var_z, 1. / 3.);
	else
		var_z = 7.787*var_z + 16. / 116.;
	*l = 116 * var_y - 16;
	*a = 500 * (var_x - var_y);
	*b = 200 * (var_y - var_z);

	return AWP_OK;
}
