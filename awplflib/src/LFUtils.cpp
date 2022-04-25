//---------------------------------------------------------------------------
#include "_LF.h"


#ifndef __BCPLUSPLUS__
inline void randomize( )
{
    srand((unsigned)time(NULL));
}

inline int random( int d )
{
    return (int)((double)rand()/(double)RAND_MAX * d);
}
#endif



/*
   DONE: перенести эту функцию в специальный файл
   геометрических функций
   возвращает высоту прямоугольника
*/
awp2DPoint awpGetPointsCentroid(awp2DPoint* p, int count)
{
    awp2DPoint res;
    res.X = 0;
    res.Y = 0;
    if (count <= 0)
        return res;
    for (int i = 0; i < count; i++)
    {
        res.X += p[i].X;
        res.Y += p[i].Y;
    }
    res.X /= count;
    res.Y /= count;
    return res;
}
awpPoint  awpGetPointsCentroid(awpPoint* p, int count)
{
    awpPoint res;
    res.X = 0;
    res.Y = 0;
    if (count <= 0)
        return res;
    for (int i = 0; i < count; i++)
    {
        res.X += p[i].X;
        res.Y += p[i].Y;
    }
    res.X /= count;
    res.Y /= count;
    return res;
}


/*
   DONE: перенести эту функцию в специальный файл
   геометрических функций
   возвращает высоту прямоугольника
*/


/*
   Возвращает левый верхний угол прямоугольника
*/
awpPoint awpGetLeftTopRectCorner(awpRect r)
{
    awpPoint result;
    result.X = r.left;
    result.Y = r.top;
    return result;
}

awpPoint awpGetRightBottomRectCorner(awpRect r)
{
    awpPoint result;
    result.X = r.right;
    result.Y = r.bottom;
    return result;
}

/*
   DONE: перенести эту функцию в специальный файл
   геометрических функций
   выполняет параллельный перенос прямоугольника в точку p
*/
awpRect awpTranslateRect(awpRect r, awpPoint p)
{
    r.left += p.X;
    r.top  += p.Y;
    r.right += p.X;
    r.bottom += p.Y;
    return r;
}
/*
   DONE: перенести эту функцию в специальный файл
   геометрических функций
   выполняет параллельный перенос прямоугольника в точку p
*/
awpRect awpTranslateRect(awpRect r, awp2DPoint p)
{
    r.left = AWPSHORT(p.X + r.left);
    r.top  = AWPSHORT(p.Y + r.top);
    r.right = AWPSHORT(p.X + r.right);
    r.bottom = AWPSHORT(p.Y + r.bottom);
    return r;
}
/*
   DONE: перенести эту функцию в специальный файл
   геометрических функций
   переносит точку на dx, dy
*/
awpPoint awpTranslatePoint (awpPoint p, int dx, int dy)
{
    awpPoint result;
    result.X = p.X + dx;
    result.Y = p.Y + dy;
    return result;
}
/*
   DONE: перенести эту функцию в специальный файл
   геометрических функций
   переносит точку на dx, dy
*/
awpPoint awpTranslatePoint (awp2DPoint p, int dx, int dy)
{
    awpPoint result;
	result.X = AWPSHORT(p.X + dx);
    result.Y = AWPSHORT(p.Y + dy);
    return result;
}
/*
   DONE: перенести эту функцию в специальный файл
   геометрических функций
   возвращает щирину прямоугольника
*/
int awpRectWidth(awpRect r)
{
    return r.right - r.left;
}
/*
   DONE: перенести эту функцию в специальный файл
   геометрических функций
   возвращает высоту прямоугольника
*/
int awpRectHeight(awpRect r)
{
    return r.bottom - r.top;
}

awpRect  awpCreateRect(awpPoint lt, awpPoint rb)
{
    awpRect r;

    r.left = lt.X;
    r.top  = lt.Y;
    r.right = rb.X;
    r.bottom = rb.Y;
    return r;
}

awpRect awpResizeRect(awpRect r, double scale)
{
    r.left = AWPSHORT(scale*r.left);
    r.top  = AWPSHORT(scale*r.top);
    r.right = AWPSHORT(scale*r.right);
    r.bottom = AWPSHORT(scale*r.bottom);
    return r;
}


// проверяет принадлежность "целой" точки прямоугольнику
AWPBOOL IsPointInRect(awpPoint p, awpRect r)
{
	if (p.X >= r.left && p.X <= r.right && p.Y >= r.top && p.Y <= r.bottom)
		return TRUE;
	else
		return FALSE;
}
//находит центр прямоугольника
awpPoint RectCenter (awpRect r)
{
    awpPoint p;
    p.X = (r.left + r.right) /2;
    p.Y = (r.top + r.bottom) /2;
    return p;
}


static awpPoint _awpGetCenterMass(awpImage* img, awpRect* r)
{
    awpPoint res;
    res.X = (r->left + r->right) / 2;
    res.Y = (r->top + r->bottom) / 2;
    AWPBYTE* b = (AWPBYTE*)img->pPixels;
    double sum, x0, y0;
    sum = 0;
    x0 = 0;
    y0 = 0;

    for (int i = r->top; i < r->bottom; i++)
    {
        for (int j = r->left; j < r->right; j++)
        {
            sum+= b[i*img->sSizeX + j];
            x0 += j*b[i*img->sSizeX + j];
            y0 += i*b[i*img->sSizeX + j];
        }
    }
    res.X = (AWPSHORT)floor(x0 / (double)sum + 0.5);
	res.Y = (AWPSHORT)floor(y0 / (double)sum + 0.5);
    return res;
}
// depricated
static awpPoint _awpGetCenterMass1(awpImage* imgh, awpImage* imgv, awpRect* r)
{
    awpPoint res;
    res.X = (r->left + r->right) / 2;
    res.Y = (r->top + r->bottom) / 2;
    AWPDOUBLE* v = (AWPDOUBLE*)imgv->pPixels;
    AWPDOUBLE* h = (AWPDOUBLE*)imgh->pPixels;
    double sum, x0, y0;
    sum = 0;
    x0 = 0;
    y0 = 0;

    for (int i = r->top; i < r->bottom; i++)
    {
        for (int j = r->left; j < r->right; j++)
        {
           // if (/*v[i*imgh->sSizeX + j] > 0 && */h[i*imgh->sSizeX + j] < 0)
            {
              double g = sqrt(v[i*imgh->sSizeX + j]*v[i*imgh->sSizeX + j] + h[i*imgh->sSizeX + j]*h[i*imgh->sSizeX + j]);
              sum += g;
              x0 += j*g;//*fabs(v[i*imgh->sSizeX + j]) + fabs(h[i*imgh->sSizeX + j]);
              y0 += i*g;//*fabs(v[i*imgh->sSizeX + j]) + fabs(h[i*imgh->sSizeX + j]);
            }
        }
    }
    if (sum > 0 )
    {
		res.X = (AWPSHORT)floor(x0 / (double)sum + 0.5);
		res.Y = (AWPSHORT)floor(y0 / (double)sum + 0.5);
    }
    return res;
}

static awpPoint _awpGetCenterMax(awpImage* imgh, awpImage* imgv, awpRect* r)
{
    awpPoint res;
    res.X = (r->left + r->right) / 2;
    res.Y = (r->top + r->bottom) / 2;
    AWPDOUBLE* v = (AWPDOUBLE*)imgv->pPixels;
    AWPDOUBLE* h = (AWPDOUBLE*)imgh->pPixels;
    double max = -100;
    double y0 = 0;

    for (int i = r->top; i < r->bottom; i++)
    {
        for (int j = r->left; j < r->right; j++)
        {
            if (max < sqrt(v[i*imgh->sSizeX + j]*v[i*imgh->sSizeX + j] + h[i*imgh->sSizeX + j]*h[i*imgh->sSizeX + j]))
            {
                max  =  sqrt(v[i*imgh->sSizeX + j]*v[i*imgh->sSizeX + j] + h[i*imgh->sSizeX + j]*h[i*imgh->sSizeX + j]);;
                y0 = i;
            }
        }
    }

	res.Y = (AWPSHORT)floor(y0 + 0.5);
    return res;
}



#ifdef __BCPLUSPLUS__
    #pragma package(smart_init)
#endif
