#include "_awpipl.h"

static  AWPDOUBLE CalcSum(AWPDOUBLE* pix, AWPINT x, AWPINT y, AWPINT w, AWPINT h, AWPINT ww )
{
    AWPDOUBLE* p = pix + x - 1 + (y - 1)*ww;
    h = h*ww;
    return (p[0] + p[w + h] - p[h] - p[w]);
}

//---------------------------------------------------------------------------
AWPRESULT awp4BitLBP(awpImage* pImage, awpRect* pRect, AWPBYTE* pCode)
{

	AWPINT x,y,w,h,ww;
	AWPRESULT res = AWP_OK;
	AWPDOUBLE tarea, area, total;
	AWPDOUBLE* pix;
	AWPINT iv[4];

    if (pImage == NULL)
        return AWP_BADARG;
    if (pImage->dwType != AWP_DOUBLE)
        return AWP_BADARG;
    if (pRect == NULL)
        return AWP_BADARG;
    if (awpRectInImage(pImage, pRect) != AWP_OK)
        return AWP_BADARG;


     x = pRect->left;
     y = pRect->top;
     w = pRect->right - x;
     h = pRect->bottom - y;
     ww = pImage->sSizeX;
     tarea = 1.0/((double)w*(double)h);
     area  = ((double)w*(double)h)/ 4.0;
     area  = 1/area;

      pix = (double*)pImage->pPixels;

      total =  CalcSum(pix, x, y, w, h, ww)*tarea;


     memset(iv, 0, sizeof(iv));
     w /=2;
     h /=2;
     iv[0] = CalcSum(pix, x, y, w, h, ww) * area > total?1:0;
     iv[1] = CalcSum(pix, x + w, y, w, h, ww) * area> total?1:0;
     iv[2] = CalcSum(pix, x +w, y + h, w, h, ww) * area> total?1:0;
     iv[3] = CalcSum(pix, x +w, y + h, w, h, ww) * area> total?1:0;

      *pCode = 0;
      *pCode |= iv[0];
      *pCode  = *pCode << 1;
      *pCode |= iv[1];
      *pCode  = *pCode << 1;
      *pCode |= iv[2];
      *pCode  = *pCode << 1;
      *pCode |= iv[3];

    return res;
}

AWPRESULT awp2BitLBP(awpImage* pImage, awpRect* pRect, AWPBYTE* pCode)
{
    AWPRESULT res = AWP_OK;
    AWPFLOAT  x,y,w,h,ww;
	AWPDOUBLE* pix;
	AWPDOUBLE area, iv[4];
    AWPBYTE c1;
    AWPBYTE c2;

	if (pImage == NULL)
        return AWP_BADARG;
    if (pImage->dwType != AWP_DOUBLE)
        return AWP_BADARG;
    if (pRect == NULL)
        return AWP_BADARG;
    if (awpRectInImage(pImage, pRect) != AWP_OK)
        return AWP_BADARG;


     x = pRect->left;
     y = pRect->top;
     w = pRect->right - x;
     h = pRect->bottom - y;
     ww = pImage->sSizeX;

     area  = ((double)w*(double)h)/ 2.0;
     area  = 1/area;

     pix = (double*)pImage->pPixels;

     memset(iv, 0, sizeof(iv));

	 iv[0] = (AWPDOUBLE)(CalcSum(pix, (AWPINT)x, (AWPINT)y, (AWPINT)w, (AWPINT)(h / 2), (AWPINT)ww)*area);
	 iv[1] = (AWPDOUBLE)(CalcSum(pix, (AWPINT)x, (AWPINT)y + (AWPINT)(h / 2), (AWPINT)w, (AWPINT)(h / 2), (AWPINT)ww)*area);

	 iv[2] = (AWPDOUBLE)(CalcSum(pix, (AWPINT)x, (AWPINT)y, (AWPINT)(w / 2), (AWPINT)h, (AWPINT)ww)*area);
	 iv[3] = (AWPDOUBLE)(CalcSum(pix, (AWPINT)x + (AWPINT)(w / 2), (AWPINT)y, (AWPINT)(w / 2), (AWPINT)h, (AWPINT)ww)*area);


     c1 = iv[0] > iv[1] ? 1:0;
     c2 = iv[2] > iv[3] ? 1:0;

     *pCode = 0;
     *pCode |= c1;
     *pCode = *pCode << 1;
     *pCode |= c2;


    return res;
}

AWPRESULT awpIntegralGrid(const awpImage* pImage, const awpRect* pRect, awpImage* pDst)
{
    AWPRESULT res = AWP_OK;
	AWPSHORT rwidth,rheight;
	AWPDOUBLE x_cell_size, y_cell_size, cell_square;
	AWPDOUBLE* in;
	AWPDOUBLE* out;
	awpRect r;
	AWPWORD x,y;

    if (pImage == NULL)
        return AWP_BADARG;
    if (pDst == NULL)
        return AWP_BADARG;
    if (pImage->dwType != AWP_DOUBLE)
        return AWP_BADARG;
    if (pDst->dwType != AWP_DOUBLE)
        return AWP_BADARG;
    if (pRect == NULL)
        return AWP_BADARG;
    if (awpRectInImage(pImage, pRect) != AWP_OK)
        return AWP_BADARG;

     rwidth = pRect->right - pRect->left;
     rheight = pRect->bottom - pRect->top;
        
     x_cell_size = (AWPDOUBLE)rwidth / (AWPDOUBLE)pDst->sSizeX;
     y_cell_size = (AWPDOUBLE)rheight / (AWPDOUBLE)pDst->sSizeY;
     cell_square =  x_cell_size*y_cell_size;

    cell_square = 1 / cell_square;

    if (x_cell_size < 2 || y_cell_size < 2)
        return AWP_BADARG;

    out = (AWPDOUBLE*)pDst->pPixels;
    in = (AWPDOUBLE*)pImage->pPixels;

    for ( y = 0; y < pDst->sSizeY; y++)
    {
        r.top = (AWPSHORT)ceil(pRect->top + y*y_cell_size + 0.5);
        r.bottom = (AWPSHORT)ceil(r.top + y_cell_size + 0.5);
        for ( x = 0; x < pDst->sSizeX; x++)
        {
             r.left = (AWPSHORT)ceil(pRect->left + x*x_cell_size + 0.5);
             r.right = (AWPSHORT)ceil(r.left + x_cell_size + 0.5);

			 out[y*pDst->sSizeX + x] = CalcSum(in, r.left, r.top, (AWPINT)x_cell_size, (AWPINT)y_cell_size, pImage->sSizeX);
             out[y*pDst->sSizeX + x] *= cell_square;
        }
    }


    return res;
}
