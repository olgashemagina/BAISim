//---------------------------------------------------------------------------

#ifndef Histogramm2DUnitH
#define Histogramm2DUnitH
//---------------------------------------------------------------------------
#include "awpipl.h"
awpImage* Get2DHistogramm(awpImage* source);
awpImage* Get2DHistogrammMask(awpImage* source, awpImage* mask);
awpImage* Get2DHistogrammRect(awpImage* source, awpRect& rect);
awpImage* MakeMask(awpImage* source, awpImage* hst, double t, bool binary = false);



#endif

