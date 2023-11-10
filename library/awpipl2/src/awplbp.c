//---------------------------------------------------------------------------
#pragma hdrstop

#include "awpLBPUnit.h"
#include "awpInterpolation.h"


#include <math.h>
const AWPDOUBLE c_sin45 =  0.70710678118654752440084436210485;
const AWPDOUBLE c_cos45 =  0.70710678118654752440084436210485;
const AWPBYTE  c_LBPEpsSize = 8;
const AWPFLOAT c_ScaleBase = 1.25f;
const AWPBYTE c_LBPBitShift[8] = {1,2,4,8,16,32,64,128};
const AWPWORD c_LBPUniform[256] =
{
/*0*/1,/*1*/2,/*2*/3,/*3*/4,/*4*/5,/*5*/0,/*6*/6,/*7*/7,/*8*/8,/*9*/0,/*10*/0,
/*11*/0,/*12*/9,/*13*/0,/*14*/10,/*15*/11,/*16*/12,/*17*/0,/*18*/0,/*19*/0,
/*20*/0,/*21*/0,/*22*/0,/*23*/0,/*24*/13,/*25*/0,/*26*/0,/*27*/0,/*28*/14,/*29*/0,
/*30*/15,/*31*/16,/*32*/17,/*33*/0,/*34*/0,/*35*/0,/*36*/0,/*37*/0,/*38*/0,/*39*/0,
/*40*/0,/*41*/0,/*42*/0,/*43*/0,/*44*/0,/*45*/0,/*46*/0,/*47*/0,/*48*/18,/*49*/0,/*50*/0,
/*51*/0,/*52*/0,/*53*/0,/*54*/0,/*55*/0,/*56*/19,/*57*/0,/*58*/0,/*59*/0,/*60*/20,
/*61*/0,/*62*/21,/*63*/22,/*64*/23,/*65*/0,/*66*/0,/*67*/0,/*68*/0,/*69*/0,/*70*/0,
/*71*/0,/*72*/0,/*73*/0,/*74*/0,/*75*/0,/*76*/0,/*77*/0,/*78*/0,/*79*/0,/*80*/0,
/*81*/0,/*82*/0,/*83*/0,/*84*/0,/*85*/0,/*86*/0,/*87*/0,/*88*/0,/*89*/0,/*90*/0,
/*91*/0,/*92*/0,/*93*/0,/*94*/0,/*95*/0,/*96*/24,/*97*/0,/*98*/0,/*99*/0,/*100*/0,
/*101*/0,/*102*/0,/*103*/0,/*104*/0,/*105*/0,/*106*/0,/*107*/0,/*108*/0,/*109*/0,/*110*/0,
/*111*/0,/*112*/25,/*113*/0,/*114*/0,/*115*/0,/*116*/0,/*117*/0,/*118*/0,/*119*/0,/*120*/26,
/*121*/0,/*122*/0,/*123*/0,/*124*/27,/*125*/0,/*126*/28,/*127*/29,/*128*/30,/*129*/31,/*130*/0,
/*131*/32,/*132*/0,/*133*/0,/*134*/0,/*135*/33,/*136*/0,/*137*/0,/*138*/0,/*139*/0,/*140*/0,
/*141*/0,/*142*/0,/*143*/34,/*144*/0,/*145*/0,/*146*/0,/*147*/0,/*148*/0,/*149*/0,/*150*/0,
/*151*/0,/*152*/0,/*153*/0,/*154*/0,/*155*/0,/*156*/0,/*157*/0,/*158*/0,/*159*/35,/*160*/0,
/*161*/0,/*162*/0,/*163*/0,/*164*/0,/*165*/0,/*166*/0,/*167*/0,/*168*/0,/*169*/0,/*170*/0,
/*171*/0,/*172*/0,/*173*/0,/*174*/0,/*175*/0,/*176*/0,/*177*/0,/*178*/0,/*179*/0,/*180*/0,
/*181*/0,/*182*/0,/*183*/0,/*184*/0,/*185*/0,/*186*/0,/*187*/0,/*188*/0,/*189*/0,/*190*/0,
/*191*/36,/*192*/37,/*193*/38,/*194*/0,/*195*/39,/*196*/0,/*197*/0,/*198*/0,/*199*/40,/*200*/0,
/*201*/0,/*202*/0,/*203*/0,/*204*/0,/*205*/0,/*206*/0,/*207*/41,/*208*/0,/*209*/0,/*210*/0,
/*211*/0,/*212*/0,/*213*/0,/*214*/0,/*215*/0,/*216*/0,/*217*/0,/*218*/0,/*219*/0,/*220*/0,
/*221*/0,/*222*/0,/*223*/42,/*224*/43,/*225*/44,/*226*/0,/*227*/45,/*228*/0,/*229*/0,/*230*/0,
/*231*/46,/*232*/0,/*233*/0,/*234*/0,/*235*/0,/*236*/0,/*237*/0,/*238*/0,/*239*/47,/*240*/48,
/*241*/49,/*242*/0,/*243*/50,/*244*/0,/*245*/0,/*246*/0,/*247*/51,/*248*/52,/*249*/53,/*250*/0,
/*251*/54,/*252*/55,/*253*/56,/*254*/57,/*255*/58
};

static AWPBYTE _awpLBPCodeU1(awpImage* src, awpPoint p, AWPBOOL interpolation, awp2DPoint* pp)
{
    AWPBYTE  lbp = 0;
    AWPBYTE* s   = (AWPBYTE*)src->pPixels;
    AWPWORD  ww = src->sSizeX*src->bChannels;
    AWPBYTE  cc = src->bChannels;
    AWPBYTE  pv = s[cc*p.X + src->bChannels*p.Y*src->sSizeX];
    AWPINT shift = 0;
    awpImage* intpl = NULL;
    AWPDOUBLE* di = NULL;
    lbp = 0;

    if (interpolation)
    {
        awpCreateImage(&intpl, src->bChannels, 1, 1, AWP_DOUBLE);
        di = _AWP_BPIX_(intpl, AWPDOUBLE);
    }


   if (!interpolation)
   {
       shift = (AWPINT)floor(cc*pp[0].X + pp[0].Y*ww + 0.5);
       if (pv < s[shift])
        lbp |= c_LBPBitShift[0];
   }
   else
   {
       awpBilinearInterpolation(src, &pp[0], intpl);
       if (pv < di[0])
         lbp |= c_LBPBitShift[0];
   }

   shift = (AWPINT)floor(cc*pp[1].X + pp[1].Y*ww);
   if (pv < s[shift])
    lbp |= c_LBPBitShift[1];

   if (!interpolation)
   {
     shift = (AWPINT)(cc*pp[2].X + pp[2].Y*ww);
     if (pv < s[shift])
      lbp |= c_LBPBitShift[2];
   }
   else
   {
       awpBilinearInterpolation(src, &pp[2], intpl);
       if (pv < di[0])
         lbp |= c_LBPBitShift[2];
   }

   shift = (AWPINT)floor(cc*pp[3].X + pp[3].Y*ww);
   if (pv <s[shift])
    lbp |= c_LBPBitShift[3];

  if (!interpolation)
  {
   shift = (AWPINT)floor(cc*pp[4].X  + pp[4].Y*ww);
   if (pv < s[shift])
    lbp |= c_LBPBitShift[4];
   }
   else
   {
       awpBilinearInterpolation(src, &pp[4], intpl);
       if (pv < di[0])
         lbp |= c_LBPBitShift[4];
   }

   shift = (AWPINT)floor(cc*pp[5].X + pp[5].Y*ww);
   if (pv < s[shift])
    lbp |= c_LBPBitShift[5];

   if (!interpolation)
   {
     shift = (AWPINT)floor(cc*pp[6].X + pp[6].Y*ww);
     if (pv < s[shift])
      lbp |= c_LBPBitShift[6];
   }
   else
   {
       awpBilinearInterpolation(src, &pp[6], intpl);
       if (pv < di[0])
         lbp |= c_LBPBitShift[6];
   }
   shift = (AWPINT)floor(cc*pp[7].X + pp[7].Y*ww);
   if (pv < s[shift])
    lbp |= c_LBPBitShift[7];


    if (interpolation)
    {
        awpReleaseImage(&intpl);
    }

   return lbp;
}

static AWPBYTE _awpLBPCodeU2(awpImage* src, awpPoint p, AWPBOOL interpolation, awp2DPoint* pp )
{
    AWPBYTE  lbp = _awpLBPCodeU1(src, p, interpolation, pp);
    return (AWPBYTE)c_LBPUniform[lbp];
}


#define _awp_neighborhood_interpolation_\
    pp[0].X = _p.X - radius*c_cos45;\
    pp[0].Y = _p.Y - radius*c_sin45;\
    pp[1].X = _p.X;\
    pp[1].Y = _p.Y - radius;\
    pp[2].X = _p.X + radius*c_cos45;\
    pp[2].Y = _p.Y - radius*c_sin45;\
    pp[3].X = _p.X + radius;\
    pp[3].Y = _p.Y ;\
    pp[4].X = _p.X + radius*c_cos45;\
    pp[4].Y = _p.Y + radius*c_sin45;\
    pp[5].X = _p.X;\
    pp[5].Y = _p.Y + radius;\
    pp[6].X = _p.X - radius*c_cos45;\
    pp[6].Y = _p.Y + radius*c_sin45;\
    pp[7].X = _p.X - radius;\
    pp[7].Y = _p.Y;


#define _awp_neighborhood_\
    pp[0].X = _p.X - radius;\
    pp[0].Y = _p.Y - radius;\
    pp[1].X = _p.X;\
    pp[1].Y = _p.Y - radius;\
    pp[2].X = _p.X + radius;\
    pp[2].Y = _p.Y - radius;\
    pp[3].X = _p.X + radius;\
    pp[3].Y = _p.Y ;\
    pp[4].X = _p.X + radius;\
    pp[4].Y = _p.Y + radius;\
    pp[5].X = _p.X;\
    pp[5].Y = _p.Y + radius;\
    pp[6].X = _p.X - radius;\
    pp[6].Y = _p.Y + radius;\
    pp[7].X = _p.X - radius;\
    pp[7].Y = _p.Y;


AWPRESULT awpLPBSPoints(awpPoint p, AWPWORD shiftX, AWPWORD shiftY, AWPFLOAT scale, AWPBOOL interpolation, awp2DPoint* out)
{
    awpPoint _p;
    AWPDOUBLE radius = pow(c_ScaleBase, scale);
    awp2DPoint* pp = out;

    if (pp == NULL)
        return AWP_BADARG;

    _p.X = p.X + shiftX;
    _p.Y = p.Y + shiftY;

    if ( interpolation)
    {
      _awp_neighborhood_interpolation_
    }
    else
    {
        _awp_neighborhood_
    }

    return AWP_OK;
}

AWPRESULT awpLPBPoints(awpPoint p, AWPBYTE options, AWPBOOL interpolation, awp2DPoint* out)
{
    AWPFLOAT radius = 0;
    awpPoint _p;

    awp2DPoint* pp = out;
    if (pp == NULL)
        return AWP_BADARG;


    if (options == AWP_LBP_U1R1 || options == AWP_LBP_U2R1)
        radius = 1;
    else if (options == AWP_LBP_U1R2 || options == AWP_LBP_U2R2)
        radius = 2;
    else
        return AWP_BADARG;

    _p = p;

    if (interpolation)
    {
        _awp_neighborhood_interpolation_
    }
    else
    {
        _awp_neighborhood_
    }
    return AWP_OK;
}

AWPRESULT awpLBPCode(awpImage* src, awpPoint p, AWPWORD shiftX, AWPWORD shiftY,
                     AWPFLOAT scale, AWPBYTE options, AWPBOOL interpolation, AWPBYTE* pcode)
{
    awp2DPoint pp[8];
    awpPoint _p;

    AWPDOUBLE radius = pow(c_ScaleBase, scale);


    if (src == NULL || pcode == NULL)
        return AWP_BADARG;
    if (p.X < 1 || p.Y < 1)
        return AWP_BADARG;
    if (p.X + radius + shiftX > src->sSizeX - 1)
        return AWP_BADARG;
    if (p.Y + radius + shiftY > src->sSizeY - 1)
        return AWP_BADARG;

     if (options == AWP_LBP_U1R1 || options == AWP_LBP_U2R1 ||
     options == AWP_LBP_U2R2 || options == AWP_LBP_U1R2)
     {
         _p = p;
         awpLPBPoints(p, options, interpolation,  pp);
     }
     else
     {
        _p.X = p.X + shiftX;
        _p.Y = p.Y + shiftY;
        awpLPBSPoints(p, shiftX, shiftY, scale, interpolation,  pp);
     }

    switch (options)
    {
        case AWP_LBP_U1R1:
        case AWP_LBP_U1R2:
            *pcode = _awpLBPCodeU1(src, _p, interpolation, pp);

        break;
        case AWP_LBP_U2R1:
        case AWP_LBP_U2R2:
            *pcode = _awpLBPCodeU2(src, _p,interpolation,  pp);
        break;
        default:
            return  AWP_BADARG;
    }
    return AWP_OK;
}
/*
    compute LBP descriptor of grayscale image
    the image type must be  AWP_BYTE
    number of channels must be  - 1.
    intput image size must be more then 8x8 pixels 
    output image  dst must be  AWP_DOUBLE type 
    sSizeY parameter must be = 1.
    sSizeX = 256 if AWP_LBP_U1XX options is set 
    SSizeX = 59  if AWP_LBP_U2XX optinos is set 
*/
AWPRESULT awpLBPDescriptor(awpImage* src, awpImage* dst, awpRect* roi, AWPBYTE options, AWPBOOL interpolation, AWPBOOL norm)
{
    AWPSHORT i;
    AWPDOUBLE sum;
    AWPWORD sx, sy, ex, ey;
    AWPWORD x,y;
    awpPoint p;
    AWPDOUBLE* hst = NULL;
    AWPRESULT res = AWP_OK;
    AWPBYTE lbp = 0;

    if (src == NULL || dst == NULL)
        return AWP_BADARG;
    if (src->bChannels != 1)
        return AWP_BADARG;
    if (src->dwType != AWP_BYTE)
        return AWP_BADARG;
    if (src->sSizeX < 8 || src->sSizeY < 8)
        return AWP_BADARG;
    if (dst->dwType != AWP_DOUBLE)
        return AWP_BADARG;
    if (dst->bChannels != 1)
        return AWP_BADARG;
    if (dst->sSizeY != 1)
        return AWP_BADARG;
    if (options == AWP_LBP_U1R1 || options == AWP_LBP_U1R2)
    {
        if (dst->sSizeX != 256)
           return AWP_BADARG;
    }
    if (options == AWP_LBP_U2R1 || options == AWP_LBP_U2R2)
    {
        if (dst->sSizeX != 59)
           return AWP_BADARG;
    }

   hst = (AWPDOUBLE*)dst->pPixels;

    if (roi != NULL)
    {
        if (awpRectInImage(src, roi) != AWP_OK)
            return AWP_BADARG;
        if (roi->right - roi->left < 8 || roi->bottom - roi->top < 8)
            return AWP_BADARG;
        sx = roi->left;
        ex = roi->right;
        sy = roi->top;
        ey = roi->bottom;
    }
    else
    {
        sx = 2;
        ex = src->sSizeX-2;
        sy = 2;
        ey = src->sSizeY-2;
    }

    for (y = sy + 1; y < ey -1; y++)
    {
        for (x = sx + 1; x < ex -1; x++)
        {
           lbp = 0;
           p.X = x;
           p.Y = y;
           awpLBPCode(src, p, 0,0,0,options, interpolation, &lbp);
           hst[lbp]++;
        }
    }

    if (res != AWP_OK)
        return res;

    if (norm)
    {
        //TODO: norm the descriptor
        hst = (AWPDOUBLE*)dst->pPixels;
        sum = 0;
        for(i = 0; i < dst->sSizeX; i++)
        {
            sum += hst[i];
        }

        for ( i = 0; i < dst->sSizeX; i++)
        {
            hst[i] /= sum;
        }
    }
    return AWP_OK;
}
/*
    Performs splitting image  or roi on n rows and  m columns 
	And for each area it calculates its own LBP descriptor, which stores in its
	In the image line dst.

    src - grayscale image AWPBYTE, number of channles = 1 
    size of src must be more then n*8\F5m*8
    output image must be AWPDOUBLE and has sSizeY =  n*n 
	sSizeX = 256 if AWP_LBP_U1XX options is set
	SSizeX = 59  if AWP_LBP_U2XX optinos is set

*/
AWPRESULT awpLBPGridDescriptor(awpImage* src, awpImage* dst, awpRect* roi, AWPBYTE n, AWPBYTE m,  AWPBYTE options, AWPBOOL interpolation, AWPBOOL norm)
{
    AWPWORD w,h, sx, sy, i, j;
    awpRect r;
    AWPRESULT res = AWP_OK;
    AWPDOUBLE ww,hh;
    awpImage* hst = NULL;
    AWPDOUBLE* b = NULL;
    AWPDOUBLE* d = NULL;
    if (src == NULL || dst == NULL)
        return AWP_BADARG;
    if (src->bChannels != 1)
        return AWP_BADARG;
    if (src->dwType != AWP_BYTE)
        return AWP_BADARG;
    if (dst->dwType != AWP_DOUBLE)
        return AWP_BADARG;
    if (dst->bChannels != 1)
        return AWP_BADARG;

    w = 0;
    h = 0;
    if (roi != NULL)
    {
        if (awpRectInImage(src, roi) != AWP_OK)
            return AWP_BADARG;
        w = roi->right - roi->left;
        h = roi->bottom - roi->top;
        sx = roi->left;
        sy = roi->top;
    }
    else
    {
        w = src->sSizeX;
        h = src->sSizeY;
        sx = 0;
        sy = 0;
    }

    if (w / m < 8 || h / n < 8)
        return AWP_BADARG;

    ww = (AWPDOUBLE)w / (AWPDOUBLE)n;
    hh = (AWPDOUBLE)h / (AWPDOUBLE)m;

    if ((res = awpCreateImage(&hst, dst->sSizeX, 1, 1, AWP_DOUBLE)) != AWP_OK)
        return res;
    b = (AWPDOUBLE*)hst->pPixels;
    d = (AWPDOUBLE*)dst->pPixels;
    for (i = 0; i < m; i++)
    {
        r.top = (AWPWORD)(sy + i*hh + 0.5);
        r.bottom = (AWPWORD)(r.top + hh + 0.5);
        for (j = 0; j < n; j++)
        {
            r.left = (AWPWORD)(sx + j*ww + 0.5);
            r.right = (AWPWORD)(r.left + ww + 0.5);
            awpZeroImage(hst);
            if ((res = awpLBPDescriptor(src, hst, &r, options,  interpolation, norm)) != AWP_OK)
            {
                awpReleaseImage(&hst);
                return res;
            }

            memcpy(d, b, dst->sSizeX*sizeof(AWPDOUBLE));
            d += dst->sSizeX;
       }
    }

    awpReleaseImage(&hst);
    return res;
}
