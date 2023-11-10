/*
//
// The code is based on that found (in C) on:
//
//   http:/www.r2m.com/win-developer-faq/graphics/8.html
//
//------------------------------------------------------------------------------
*/
#ifndef _AWPHSL_H_
#define _AWPHSL_H_
/*convert RGB to HSL*/
void _awpRGBtoHSL(AWPBYTE rr,AWPBYTE gg,AWPBYTE bb,AWPDOUBLE *h, AWPDOUBLE *s, AWPDOUBLE *l);
/*convert HSL to RGB */
void _awpHLStoRGB(AWPDOUBLE h, AWPDOUBLE s, AWPDOUBLE l, AWPBYTE *r,AWPBYTE *g,AWPBYTE *b);
#endif
