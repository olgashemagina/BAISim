//---------------------------------------------------------------------------

#ifndef awpInterpolationH
#define awpInterpolationH
#include "awpipl.h"
//---------------------------------------------------------------------------
/*
    Ѕилинейна€ интерпол€ци€.
    ¬ходные параметры:
    src - исходное изображение  awpImage
    p - точка, дл€ которой требуетс€ найти значение €крости  awp2DPoint
    ¬ыходной параметр:
    dst - значени€ €ркости упакованные в awpImage (один канал, одна строка,
          тип AWP_DOUBLE
*/

AWPRESULT awpBilinearInterpolation(awpImage* src, awp2DPoint* p, awpImage* dst);

#endif
