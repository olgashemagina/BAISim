#ifndef _AWPHSV_H_
#define _AWPHSV_H_
/*convert RGB to HSL*/
//void _awpRGBtoHSV(AWPBYTE rr,AWPBYTE gg,AWPBYTE bb,AWPFLOAT *h, AWPFLOAT *s, AWPFLOAT *l);
/*convert HSL to RGB */
void _awpHSVtoRGB(AWPDOUBLE h, AWPDOUBLE s, AWPDOUBLE l, AWPBYTE *r,AWPBYTE *g,AWPBYTE *b);
#endif
