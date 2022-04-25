/*
//
//  RCS:
//      $Source: _awpColor.h
//      $Revision: 1.0 $
//      $Date:
//
//    Purpose: AWPIPL header file
//    Contents: internal header file for color convertion
//    Authors : Alexander Telnykh
*/
#ifndef _AWPCOLOR_H_
#define _AWPCOLOR_H_
/*
	converts RGB color value to hsv, hsl, xyz and lab color spaces 
	RGB values should be in the range 0..255
*/
AWPRESULT _awp_rgb_to_hsv(AWPBYTE r, AWPBYTE g, AWPBYTE b, AWPDOUBLE* h, AWPDOUBLE* s, AWPDOUBLE* v);
AWPRESULT _awp_rgb_to_hsl(AWPBYTE r, AWPBYTE g, AWPBYTE b, AWPDOUBLE* h, AWPDOUBLE* s, AWPDOUBLE* l);
AWPRESULT _awp_rgb_to_xyz(AWPBYTE r, AWPBYTE g, AWPBYTE b, AWPDOUBLE* x, AWPDOUBLE* y, AWPDOUBLE* z);
AWPRESULT _awp_rgb_to_lab(AWPBYTE r, AWPBYTE g, AWPBYTE b, AWPDOUBLE* l, AWPDOUBLE* a, AWPDOUBLE* bb);
/*
	converts RGB color value to hsv, hsl, xyz and lab color spaces
	RGB values should be in the range 0..255
*/
AWPRESULT _awp_hsv_to_rgb(AWPDOUBLE h, AWPDOUBLE s, AWPDOUBLE v, AWPBYTE* r, AWPBYTE* g, AWPBYTE* b);
AWPRESULT _awp_hsl_to_rgb(AWPDOUBLE h, AWPDOUBLE s, AWPDOUBLE l, AWPBYTE* r, AWPBYTE* g, AWPBYTE* b);
AWPRESULT _awp_xyz_to_rgb(AWPDOUBLE x, AWPDOUBLE y, AWPDOUBLE z, AWPBYTE* r, AWPBYTE* g, AWPBYTE* b);
AWPRESULT _awp_lab_to_rgb(AWPDOUBLE l, AWPDOUBLE a, AWPDOUBLE bb, AWPBYTE* r, AWPBYTE* g, AWPBYTE* b);
/*
	colversion between lab and xyz color spaces 
*/
AWPRESULT _awp_lab_to_xyz(AWPDOUBLE l, AWPDOUBLE a, AWPDOUBLE b, AWPDOUBLE* x, AWPDOUBLE* y, AWPDOUBLE* z);
AWPRESULT _awp_xyz_to_lab(AWPDOUBLE x, AWPDOUBLE y, AWPDOUBLE z, AWPDOUBLE* l, AWPDOUBLE* a, AWPDOUBLE* b);

#endif //_AWPCOLOR_H_
