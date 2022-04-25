/*
    AWP Image library v2
    
    $Source: /home/cvs/awp/imageproc/awpipl-v2/src/awpdraw.c,v $
    $Revision: 1.9 $
    $Date: 2005/05/04 10:24:35 $
	
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "_awpipl.h"
AWPRESULT _awpDrawPoint(awpImage* pImage, awpPoint p, AWPBYTE bChan, AWPDOUBLE dValue)
{
	AWPRESULT res;
	AWPBYTE*	b;
	AWPSHORT*	s;
	AWPFLOAT*  f;
	AWPDOUBLE* d;
	AWPWORD  width; /*width of image line in elements*/
	AWPDWORD pos; 
	res = AWP_OK;
	_CHECK_RESULT_(( res = awpCheckImage(pImage)))
	/*check the position*/
	if (p.X >= pImage->sSizeX || p.Y >= pImage->sSizeY)
		_ERROR_EXIT_RES_(AWP_BADARG)
	/*check the channel*/
	if (bChan >= pImage->bChannels)
		_ERROR_EXIT_RES_(AWP_BADARG)
	/*pointer to the channel*/
	width = pImage->bChannels*pImage->sSizeX;
	pos = p.Y*width + p.X*pImage->bChannels + bChan;
	switch (pImage->dwType)
	{
	case AWP_BYTE:
			b = (AWPBYTE*)pImage->pPixels;
			b[pos] = (AWPBYTE)dValue;
		break;
	case AWP_SHORT:
			s = (AWPSHORT*)pImage->pPixels;
			s[pos] = (AWPSHORT)dValue;
		break;
	case AWP_FLOAT:
			f = (AWPFLOAT*)pImage->pPixels;
			f[pos] = (AWPFLOAT)dValue;
 		break;
	case AWP_DOUBLE:
			d = (AWPDOUBLE*)pImage->pPixels;
			d[pos] = dValue;
		break;
	default:
		_ERROR_EXIT_RES_(AWP_BADARG)
	}
CLEANUP:
    return res;
}
/*awpDrawThickPointByte
paints the neighborhood of point p on an isobath containing AWPBYTE type data*/
static AWPRESULT _awpDrawThickPointByte(awpImage* pImage, awpPoint p, AWPBYTE bChan, AWPDOUBLE dValue, AWPBYTE radius)
{
	AWPBYTE* b = (AWPBYTE*)pImage->pPixels;
	AWPBYTE  v = (AWPBYTE)dValue;
	AWPWORD  x, y;
	for (y = p.Y - radius; y < p.Y + radius; y++)
	{
		for (x = p.X - radius; x < p.X + radius; x++)
		{
			b[(x + y*pImage->sSizeX)*pImage->bChannels + bChan] = v;
		}
	}
	return AWP_OK;
}
/*awpDrawThickPointByte
paints the neighborhood of point p on an isobath containing AWPSHORT type data*/
static AWPRESULT _awpDrawThickPointShort(awpImage* pImage, awpPoint p, AWPBYTE bChan, AWPDOUBLE dValue, AWPBYTE radius)
{
	AWPSHORT* b = (AWPSHORT*)pImage->pPixels;
	AWPSHORT  v = (AWPSHORT)dValue;
	AWPWORD  x, y;
	for (y = p.Y - radius; y < p.Y + radius; y++)
	{
		for (x = p.X - radius; x < p.X + radius; x++)
		{
			b[x + y*pImage->sSizeX + bChan] = v;
		}
	}
	return AWP_OK;
}
/*awpDrawThickPointByte
paints the neighborhood of point p on an isobath containing AWPFLOAT type data*/
static AWPRESULT _awpDrawThickPointFloat(awpImage* pImage, awpPoint p, AWPBYTE bChan, AWPDOUBLE dValue, AWPBYTE radius)
{
	AWPFLOAT* b = (AWPFLOAT*)pImage->pPixels;
	AWPFLOAT  v = (AWPFLOAT)dValue;
	AWPWORD  x, y;
	for (y = p.Y - radius; y < p.Y + radius; y++)
	{
		for (x = p.X - radius; x < p.X + radius; x++)
		{
			b[x + y*pImage->sSizeX + bChan] = v;
		}
	}
	return AWP_OK;
}
/*awpDrawThickPointByte
paints the neighborhood of point p on an isobath containing AWPDOUBLE type data*/
static AWPRESULT _awpDrawThickPointDouble(awpImage* pImage, awpPoint p, AWPBYTE bChan, AWPDOUBLE dValue, AWPBYTE radius)
{
	AWPDOUBLE* b = (AWPDOUBLE*)pImage->pPixels;
	AWPDOUBLE  v = (AWPDOUBLE)dValue;
	AWPWORD  x, y;
	for (y = p.Y - radius; y < p.Y + radius; y++)
	{
		for (x = p.X - radius; x < p.X + radius; x++)
		{
			b[x + y*pImage->sSizeX + bChan] = v;
		}
	}
	return AWP_OK;
}
// awpDrawThickPoint
// paints a neighborhood of a point p of radius r
AWPRESULT _awpDrawThickPoint(awpImage* pImage, awpPoint p, AWPBYTE bChan, AWPDOUBLE dValue, AWPBYTE radius)
{
    AWPRESULT res = AWP_OK;
	AWPINT maxx = p.X + radius;
	AWPINT minx = p.X - radius;
	AWPINT maxy = p.Y + radius;
	AWPINT miny = p.Y - radius;
	

	_CHECK_RESULT_(( res = awpCheckImage(pImage)))
	/*the coordinate of the point p with allowance for the radius of the point can not leave
beyond the image*/
	if (maxx >= pImage->sSizeX || minx < 0 || maxy >= pImage->sSizeY || miny < 0)
		_ERROR_EXIT_RES_(AWP_BADARG)
	/* The channel number on which the point should be drawn can not be
more than the number of channels of the input image */
	if (bChan >= pImage->bChannels)
		_ERROR_EXIT_RES_(AWP_BADARG)
	/*todo: The value of the anchor of the drawn point dValue can not exceed the allowable range defined by
selected image type*/
	switch (pImage->dwType)
	{
	case AWP_BYTE:
		_CHECK_RESULT_(( res = _awpDrawThickPointByte(pImage, p, bChan, dValue, radius)))
		break;
	case AWP_SHORT:
		_CHECK_RESULT_(( res = _awpDrawThickPointShort(pImage, p, bChan, dValue, radius)))
		break;
	case AWP_FLOAT:
		_CHECK_RESULT_(( res = _awpDrawThickPointFloat(pImage, p, bChan, dValue, radius)))
		break;
	case AWP_DOUBLE:
		_CHECK_RESULT_(( res = _awpDrawThickPointDouble(pImage, p, bChan, dValue, radius)))
		break;
	}
CLEANUP:
	return res;
}

AWPRESULT _awpDrawLine(awpImage* pImage, awpPoint p1, awpPoint p2, AWPBYTE bChan, AWPDOUBLE dValue)
{
	AWPRESULT res;
	AWPWORD t, distance;
	AWPSHORT incx, incy;

	AWPWORD startx=p1.X;
	AWPWORD starty=p1.Y;
	AWPWORD endx=p2.X;
	AWPWORD endy=p2.Y;
	AWPINT xerr = 0, yerr = 0, delta_x, delta_y;


	res = AWP_OK;
	_CHECK_RESULT_(( res = awpCheckImage(pImage)))
 	/*check the position*/
	if (p1.X >= pImage->sSizeX || p1.Y >= pImage->sSizeY || p1.X < 0 || p1.Y < 0)
   		_ERROR_EXIT_RES_(AWP_BADARG)
	/*check the position*/
	if (p2.X >= pImage->sSizeX || p2.Y >= pImage->sSizeY || p2.X < 0 || p2.Y < 0)
   		_ERROR_EXIT_RES_(AWP_BADARG)
	/*check the channel*/
	if (bChan >= pImage->bChannels)
		_ERROR_EXIT_RES_(AWP_BADARG)


	delta_x = endx - startx;
	delta_y = endy - starty;

	if (delta_x > 0) incx = 1;
	else if (delta_x == 0) incx = 0;
	else incx = -1;

	if (delta_y > 0) incy = 1;
	else if (delta_y == 0) incy = 0;
	else incy = -1;

	delta_x = abs(delta_x);
	delta_y = abs(delta_y);
	if (delta_x > delta_y) distance = delta_x;
	else distance = delta_y;

	for (t = 0; t <= distance + 1; t ++)
	{
      if (startx < pImage->sSizeX && starty < pImage->sSizeY )
      {
		switch (pImage->dwType)
		{
			case AWP_BYTE:
					_PIXEL(pImage, startx, starty, bChan, AWPBYTE) = (AWPBYTE)dValue;
				break;
			case AWP_SHORT:
					_PIXEL(pImage, startx, starty, bChan, AWPSHORT) = (AWPSHORT)dValue;
				break;
			case AWP_FLOAT:
					_PIXEL(pImage, startx, starty, bChan, AWPFLOAT) = (AWPFLOAT)dValue;
 				break;
			case AWP_DOUBLE:
					_PIXEL(pImage, startx, starty, bChan, AWPDOUBLE) = dValue;
				break;
			default:
				_ERROR_EXIT_RES_(AWP_BADARG)
		}
      }

		xerr += delta_x;
		yerr += delta_y;
		if (xerr > (AWPINT)distance){xerr -= (AWPINT)distance; startx += (AWPDWORD)incx;}
		if (yerr > (AWPINT)distance){yerr -= (AWPINT)distance; starty += (AWPDWORD)incy;}
	}

CLEANUP:
    return res;
}
//awpDrawThickLine
AWPRESULT _awpDrawThickLine(awpImage* pImage, awpPoint p1, awpPoint p2, AWPBYTE bChan, AWPDOUBLE dValue, AWPBYTE radius)
{
	AWPRESULT res;
	AWPWORD t, distance;
	AWPSHORT incx, incy;

	AWPWORD startx=p1.X;
	AWPWORD starty=p1.Y;
	AWPWORD endx=p2.X;
	AWPWORD endy=p2.Y;
	AWPINT xerr = 0, yerr = 0, delta_x, delta_y;
	awpPoint p;

	res = AWP_OK;
	_CHECK_RESULT_(( res = awpCheckImage(pImage)))
 	/*check the position*/
	if (p1.X > pImage->sSizeX || p1.Y > pImage->sSizeY || p1.X < 0 || p1.Y < 0)
   		_ERROR_EXIT_RES_(AWP_BADARG)
	/*check the position*/
	if (p2.X > pImage->sSizeX || p2.Y > pImage->sSizeY || p2.X < 0 || p2.Y < 0)
   		_ERROR_EXIT_RES_(AWP_BADARG)
	/*check the channel*/
	if (bChan >= pImage->bChannels)
		_ERROR_EXIT_RES_(AWP_BADARG)

	delta_x = endx - startx;
	delta_y = endy - starty;

	if (delta_x > 0) incx = 1;
	else if (delta_x == 0) incx = 0;
	else incx = -1;

	if (delta_y > 0) incy = 1;
	else if (delta_y == 0) incy = 0;
	else incy = -1;

	delta_x = abs(delta_x);
	delta_y = abs(delta_y);
	if (delta_x > delta_y) distance = delta_x;
	else distance = delta_y;

	for (t = 0; t <= distance + 1; t ++)
	{
      if (startx < pImage->sSizeX && starty < pImage->sSizeY )
      {
		p.X = startx;p.Y = starty;
		_awpDrawThickPoint(pImage,p,bChan, dValue,radius);  
	  }
		xerr += delta_x;
		yerr += delta_y;
		if (xerr > (AWPINT)distance){xerr -= (AWPINT)distance; startx += (AWPDWORD)incx;}
		if (yerr > (AWPINT)distance){yerr -= (AWPINT)distance; starty += (AWPDWORD)incy;}
	}

CLEANUP:
    return res;
}



AWPRESULT _awpDrawRect(awpImage* pImage, awpRect* pRect, AWPBYTE bChan,AWPDOUBLE dValue)
{
    AWPRESULT res;
    AWPINT iImageType;
    AWPBYTE bValue;AWPBYTE* pbData;
    AWPSHORT* psData;
    AWPFLOAT* pfData;
    AWPDOUBLE* pdData;
    AWPINT i;

    res = AWP_OK;

    pbData=NULL;
    psData=NULL;
    pfData=NULL;
    pdData=NULL;
    bValue=0;

    /* check argument */
    _CHECK_RESULT_(( res = awpCheckImage(pImage)))
    _CHECK_RESULT_(( res = awpCheckRect(pRect)))
    _CHECK_RESULT_(( res = awpRectInImage(pImage, pRect)))

    if(bChan>=pImage->bChannels){
        res = AWP_BADARG;
        _ERR_EXIT_
    }


    /* calculate shift data */
    _CHECK_RESULT_((res = awpGetImagePixelType(pImage, &iImageType)))

    switch(iImageType) {
	case AWP_BYTE :
	    bValue= (AWPBYTE)dValue;
	    pbData=(AWPBYTE*)pImage->pPixels;
	    pbData+=bChan;
	    break;
	case AWP_SHORT:
	    psData=(AWPSHORT*)pImage->pPixels;
	    psData+=bChan;
	    break;
	case AWP_FLOAT:
	    pfData=(AWPFLOAT*)pImage->pPixels;
	    pfData+=bChan;
	    break;
	case AWP_DOUBLE:
	    pdData=(AWPDOUBLE*)pImage->pPixels;
	    pdData+=bChan;
	    break;
    }

    /* draw rect */
    /* draw horizontal line */
    for(i=pRect->left;i<=pRect->right;i++){
	switch(iImageType){
	    case AWP_BYTE:
		/* top line */
		pbData[(pRect->top*pImage->sSizeX+i)*pImage->bChannels]=bValue;
		/* bottom line */
		pbData[(pRect->bottom*pImage->sSizeX+i)*pImage->bChannels]=bValue;
		break;
	}
    }
    for(i=pRect->top;i<=pRect->bottom;i++){
	switch(iImageType){
	    case AWP_BYTE:
		/* left line */
		pbData[(i*pImage->sSizeX+pRect->left)*pImage->bChannels]=bValue;
		pbData[(i*pImage->sSizeX+pRect->right)*pImage->bChannels]=bValue;
		break;
	    }
    }
CLEANUP:
    return res;
}
// todo: implement awpDrawThickRect
AWPRESULT _awpDrawThickRect(awpImage* pImage, awpRect* pRect, AWPBYTE bChan, AWPDOUBLE dValue, AWPBYTE radius)
{
	AWPRESULT res;
	awpPoint p1, p2, p3, p4;
	_CHECK_RESULT_(( res = awpCheckImage(pImage)))
	_CHECK_RESULT_(( res = awpCheckRect(pRect)))
	if (bChan >= pImage->bChannels)
		_ERROR_EXIT_RES_(AWP_BADARG)

    res = awpRectInImage(pImage, pRect);
    if (res != AWP_OK)
    {
		if (pRect->top < 0)
        	pRect->top = 2*radius;
		if (pRect->left < 0)
        	pRect->left = 2*radius;
        if (pRect->right >= pImage->sSizeX)
            pRect->right =  pImage->sSizeX-2*radius-1;
        if (pRect->bottom >= pImage->sSizeY)
            pRect->bottom=  pImage->sSizeY-2*radius-1;
    }
	_CHECK_RESULT_(( res = awpRectInImage(pImage, pRect)))

	p1.X = pRect->left;
	p1.Y = pRect->top;

	p2.X = pRect->right;
	p2.Y = pRect->top;

	p3.X = pRect->right;
	p3.Y = pRect->bottom;

	p4.X = pRect->left;
	p4.Y = pRect->bottom;

	_CHECK_RESULT_(( res = _awpDrawThickLine(pImage, p1, p2, bChan, dValue, radius)))
	_CHECK_RESULT_(( res = _awpDrawThickLine(pImage, p2, p3, bChan, dValue, radius)))
	_CHECK_RESULT_(( res = _awpDrawThickLine(pImage, p3, p4, bChan, dValue, radius)))
	_CHECK_RESULT_(( res = _awpDrawThickLine(pImage, p4, p1, bChan, dValue, radius)))

CLEANUP:
    return res;
}


AWPRESULT _awpDrawCross(awpImage* pImage, awpRect* pRect, AWPBYTE bChan, AWPDOUBLE dValue)
{
    AWPRESULT res = AWP_OK;
    awpPoint p1;
    awpPoint p2;
    p1.X = pRect->left;
    p1.Y = pRect->top + (pRect->bottom - pRect->top)/2;
    p2.X = pRect->right;
    p2.Y = p1.Y;
    _CHECK_RESULT_(_awpDrawLine(pImage, p1, p2, bChan, dValue))
    p1.X = (pRect->left + pRect->right)/2;
    p1.Y = pRect->top;
    p2.X = p1.X;
    p2.Y = pRect->bottom;
    _CHECK_RESULT_(_awpDrawLine(pImage, p1, p2, bChan, dValue))
CLEANUP:
    return res;
}
// todo: implement awpDrawThickCross
AWPRESULT _awpDrawThickCross(awpImage* pImage, awpRect* pRect, AWPBYTE bChan, AWPDOUBLE dValue, AWPBYTE radius)
{
    AWPRESULT res = AWP_OK;
    awpPoint p1;
    awpPoint p2;
    p1.X = pRect->left;
    p1.Y = pRect->top + (pRect->bottom - pRect->top)/2;
    p2.X = pRect->right;
    p2.Y = p1.Y;
    _CHECK_RESULT_(_awpDrawThickLine(pImage, p1, p2, bChan, dValue, radius))
    p1.X = (pRect->left + pRect->right)/2;
    p1.Y = pRect->top;
    p2.X = p1.X;
    p2.Y = pRect->bottom;
    _CHECK_RESULT_(_awpDrawThickLine(pImage, p1, p2, bChan, dValue, radius))
CLEANUP:
    return res;}

AWPRESULT awpFill(awpImage *pImage, AWPDOUBLE dValue)
{
	AWPBYTE  bValue;
	AWPSHORT sValue;
	AWPFLOAT fValue;
	AWPINT   i;
	AWPFLOAT* fData;
	AWPDOUBLE* dData;
	AWPRESULT res = AWP_OK;
    AWPINT imageSize = 0;

	 if (pImage == NULL)
		 _ERROR_EXIT_RES_(AWP_BADARG)
	
     _CHECK_RESULT_(res = awpGetImageSize(pImage, &imageSize))
	 
	 if (pImage->dwType == AWP_BYTE)
	 {
		if (dValue < 0 || dValue > 255)
		 _ERROR_EXIT_RES_(AWP_BADARG)
		bValue = (AWPBYTE)dValue;
		memset(pImage->pPixels, bValue, imageSize);
	 }
	 else if (pImage->dwType == AWP_SHORT)
	 {
		if (dValue < AWP_SHRT_MIN || dValue > AWP_SHRT_MAX)
		 _ERROR_EXIT_RES_(AWP_BADARG)
		 sValue = (AWPSHORT)dValue;
 		 memset(pImage->pPixels, sValue, imageSize);
	 }
	 else if (pImage->dwType == AWP_FLOAT)
	 {
		 if (dValue < AWP_FLOAT_MIN || dValue > AWP_FLOAT_MAX)
			 _ERROR_EXIT_RES_(AWP_BADARG)
		 fValue = (AWPFLOAT)dValue;
		 fData = (AWPFLOAT*)pImage->pPixels;
         imageSize /= sizeof(AWPFLOAT);
		 for (i =0; i < imageSize; i++)
			fData[i] = fValue;
	 }
	 else
	 {
		 dData = (AWPDOUBLE*)pImage->pPixels;
         imageSize /= sizeof(AWPDOUBLE);
		 for (i =0; i < imageSize; i++)
			dData[i] = dValue;
	 }

CLEANUP:
    return res;
}

AWPRESULT awpFillRect(awpImage *pImage, awpRect* rect, AWPBYTE bChan, AWPDOUBLE dValue)
{
	AWPRESULT res = AWP_OK;
	AWPWORD sx, sy, w, h, y, x, ws;
	AWPBYTE* data;
	AWPBYTE  value;
	if (pImage == NULL || rect == NULL)
		_ERROR_EXIT_RES_(AWP_BADARG)
	if (awpRectInImage(pImage, rect) != AWP_OK)
		_ERROR_EXIT_RES_(AWP_BADARG)
	if (bChan >= pImage->bChannels)
		_ERROR_EXIT_RES_(AWP_BADARG)
	// todo: extend all possible data types
	if (pImage->dwType != AWP_BYTE)
		_ERROR_EXIT_RES_(AWP_BADARG)

	sx = rect->left;
	sy = rect->top;
	w = rect->right - rect->left;
    ws = pImage->bChannels*pImage->sSizeX;
	h = rect->bottom - rect->top;
	data = (AWPBYTE*)pImage->pPixels;
	value = (AWPBYTE)dValue;
	for (y = sy; y < sy + h; y++)
	{
        for (x = sx; x < sx + w; x++)
            data[y*ws +x*pImage->bChannels + bChan] = value;
//		memset(&data[y*pImage->sSizeX + sx], value, w*sizeof(AWPBYTE));
	}

CLEANUP:
	return res;
}

AWPRESULT _awpDrawEllipse(awpImage* pImage, awpPoint center, AWPWORD width, AWPWORD height, AWPDOUBLE angle, AWPBYTE bChan, AWPDOUBLE dValue)
{
	AWPRESULT res = AWP_OK;
	AWPINT y;
	AWPDOUBLE diskr;
	AWPDOUBLE c, e;
	AWPDOUBLE C_A, S_A;
	AWPDOUBLE QC_A, QS_A;
	AWPDWORD Qwidth, Qheight;
	AWPWORD l;
	AWPBOOL draw_start;
	awpPoint p_l, p_r, p;


	if ((width == 0) || (height == 0))
		_ERROR_EXIT_RES_(AWP_BADARG)
	_CHECK_RESULT_((res = awpCheckImage(pImage)))
	/*check the channel*/
	if (bChan >= pImage->bChannels)
		_ERROR_EXIT_RES_(AWP_BADARG)


	l = width;
	if (height > width) l = height;

	C_A = cos(angle);
	S_A = sin(angle);
	QC_A = C_A * C_A;
	QS_A = S_A * S_A;
	Qwidth = width * width;
	Qheight = height * height;

	c = Qheight * QC_A + Qwidth * QS_A;
	e = (Qwidth - Qheight) * S_A * C_A;

	draw_start = FALSE;
	for(y = -l; y <= l; y++)
	{
		diskr = Qwidth * Qheight * (- y * y + c);

		if (diskr < 0)
		{
			if (!draw_start) continue; else break;
		}

		if (!draw_start)
		{
			p_r.X = center.X + (AWPWORD) ((y * e + sqrt(diskr)) / c);
			if (p_r.X > pImage->sSizeX) p_r.X = pImage->sSizeX;
			if (p_r.X < 0) p_r.X = 0;
			p_l.X = center.X + (AWPWORD) ((y * e - sqrt(diskr)) / c);
			if (p_l.X > pImage->sSizeX) p_l.X = pImage->sSizeX;
			if (p_l.X < 0) p_l.X = 0;
			p_r.Y = center.Y - y;
			if (p_r.Y > pImage->sSizeY) p_r.Y = pImage->sSizeY;
			if (p_r.Y < 0) p_r.Y = 0;
			p_l.Y = p_r.Y;
			_CHECK_RESULT_((_awpDrawLine(pImage, p_r, p_l, bChan, dValue)))
			draw_start = TRUE;
			continue;
		}
		p.Y = center.Y - y;
		if (p.Y > pImage->sSizeY) p.Y = pImage->sSizeY;
		if (p.Y < 0) p.Y = 0;

		p.X = center.X + (AWPWORD) ((y * e + sqrt(diskr)) / c);
		if (p.X > pImage->sSizeX) p.X = pImage->sSizeX;
		if (p.X < 0) p.X = 0;
		_CHECK_RESULT_((_awpDrawLine(pImage, p_r, p, bChan, dValue)))
		p_r = p;

		p.X = center.X + (AWPWORD) ((y * e - sqrt(diskr)) / c);
		if (p.X > pImage->sSizeX) p.X = pImage->sSizeX;
		if (p.X < 0) p.X = 0;
		_CHECK_RESULT_((_awpDrawLine(pImage, p_l, p, bChan, dValue)))
		p_l = p;
	}
	_CHECK_RESULT_((_awpDrawLine(pImage, p_l, p_r, bChan, dValue)))

CLEANUP:
    return res;
}

AWPRESULT _awpDrawEllipseV2(awpImage* pImage, awpPoint center, AWPWORD width, AWPWORD height, AWPDOUBLE angle, AWPBYTE bChan, AWPDOUBLE dValue)
{
	AWPRESULT res = AWP_OK;
	AWPDOUBLE phase, freq;
	AWPDOUBLE W_S, W_C, H_S, H_C;
	AWPDOUBLE C_T, S_T;
	awpPoint p_last, p_first, p;

	if ((width == 0) || (height == 0))
		_ERROR_EXIT_RES_(AWP_BADARG)
	_CHECK_RESULT_((res = awpCheckImage(pImage)))
	/*check the channel*/
	if (bChan >= pImage->bChannels)
		_ERROR_EXIT_RES_(AWP_BADARG)

	if (width > height)
		freq = 1 * AWP_PI / width;
	else
		freq = 1 * AWP_PI / height;

	W_S = width * sin(angle);
	W_C = width * cos(angle);
	H_S = height * sin(angle);
	H_C = height * cos(angle);

	phase = 0;
	p_last.X = -1;
	while(phase <= 2 * AWP_PI)
	{
		C_T = cos(phase);
		S_T = sin(phase);

		p.Y = center.Y - (AWPWORD) (W_S * C_T - H_C * S_T);
		if (p.Y > pImage->sSizeY) p.Y = pImage->sSizeY;
		if (p.Y < 0) p.Y = 0;

		p.X = center.X + (AWPWORD) (W_C * C_T + H_S * S_T);
		if (p.X > pImage->sSizeX) p.X = pImage->sSizeX;
		if (p.X < 0) p.X = 0;

		if (p_last.X == -1)
		{
			p_last = p;
			p_first = p;
		}
		else
			_CHECK_RESULT_((res = _awpDrawLine(pImage, p_last, p, bChan, dValue)))

		p_last = p;

		phase += freq * 1;
	}

	_CHECK_RESULT_((res = _awpDrawLine(pImage, p_last, p_first, bChan, dValue)))

CLEANUP:
    return res;
}


AWPRESULT _awpDrawThickEllipse(awpImage* pImage, awpPoint center, AWPWORD width, AWPWORD height, AWPDOUBLE angle, AWPBYTE bChan, AWPDOUBLE dValue, AWPBYTE radius)
{
	AWPRESULT res = AWP_OK;
	AWPINT y;
	AWPDOUBLE diskr;
	AWPDOUBLE c, e;
	AWPDOUBLE C_A, S_A;
	AWPDOUBLE QC_A, QS_A;
	AWPDWORD Qwidth, Qheight;
	AWPWORD l;
	AWPBOOL draw_start;
	awpPoint p_l, p_r, p;


	if ((width == 0) || (height == 0))
		_ERROR_EXIT_RES_(AWP_BADARG)
	_CHECK_RESULT_((res = awpCheckImage(pImage)))
	/*check the channel*/
	if (bChan >= pImage->bChannels)
		_ERROR_EXIT_RES_(AWP_BADARG)


	l = width;
	if (height > width) l = height;

	C_A = cos(angle);
	S_A = sin(angle);
	QC_A = C_A * C_A;
	QS_A = S_A * S_A;
	Qwidth = width * width;
	Qheight = height * height;

	c = Qheight * QC_A + Qwidth * QS_A;
	e = (Qwidth - Qheight) * S_A * C_A;

	draw_start = FALSE;
	for(y = -l; y <= l; y++)
	{
		diskr = Qwidth * Qheight * (- y * y + c);

		if (diskr < 0)
		{
			if (!draw_start) continue; else break;
		}

		if (!draw_start)
		{
			p_r.X = center.X + (AWPWORD) ((y * e + sqrt(diskr)) / c);
			if (p_r.X > pImage->sSizeX) p_r.X = pImage->sSizeX;
			if (p_r.X < 0) p_r.X = 0;
			p_l.X = center.X + (AWPWORD) ((y * e - sqrt(diskr)) / c);
			if (p_l.X > pImage->sSizeX) p_l.X = pImage->sSizeX;
			if (p_l.X < 0) p_l.X = 0;
			p_r.Y = center.Y - y;
			if (p_r.Y > pImage->sSizeY) p_r.Y = pImage->sSizeY;
			if (p_r.Y < 0) p_r.Y = 0;
			p_l.Y = p_r.Y;
			_CHECK_RESULT_((_awpDrawThickLine(pImage, p_r, p_l, bChan, dValue, radius)))
			draw_start = TRUE;
			continue;
		}
		p.Y = center.Y - y;
		if (p.Y > pImage->sSizeY) p.Y = pImage->sSizeY;
		if (p.Y < 0) p.Y = 0;

		p.X = center.X + (AWPWORD) ((y * e + sqrt(diskr)) / c);
		if (p.X > pImage->sSizeX) p.X = pImage->sSizeX;
		if (p.X < 0) p.X = 0;
		_CHECK_RESULT_((_awpDrawThickLine(pImage, p_r, p, bChan, dValue,radius)))
		p_r = p;

		p.X = center.X + (AWPWORD) ((y * e - sqrt(diskr)) / c);
		if (p.X > pImage->sSizeX) p.X = pImage->sSizeX;
		if (p.X < 0) p.X = 0;
		_CHECK_RESULT_((_awpDrawThickLine(pImage, p_l, p, bChan, dValue, radius)))
		p_l = p;
	}
	_CHECK_RESULT_((_awpDrawThickLine(pImage, p_l, p_r, bChan, dValue, radius)))

CLEANUP:
    return res;
}

AWPRESULT _awpDrawThickEllipseV2(awpImage* pImage, awpPoint center, AWPWORD width, AWPWORD height, AWPDOUBLE angle, AWPBYTE bChan, AWPDOUBLE dValue, AWPBYTE radius)
{
	AWPRESULT res = AWP_OK;
	AWPDOUBLE phase, freq;
	AWPDOUBLE W_S, W_C, H_S, H_C;
	AWPDOUBLE C_T, S_T;
	awpPoint p_last, p_first, p;

	if ((width == 0) || (height == 0))
		_ERROR_EXIT_RES_(AWP_BADARG)
	_CHECK_RESULT_((res = awpCheckImage(pImage)))
	/*check the channel*/
	if (bChan >= pImage->bChannels)
		_ERROR_EXIT_RES_(AWP_BADARG)

	if (width > height)
		freq = 1 * AWP_PI / width;
	else
		freq = 1 * AWP_PI / height;

	W_S = width * sin(angle);
	W_C = width * cos(angle);
	H_S = height * sin(angle);
	H_C = height * cos(angle);

	phase = 0;
	p_last.X = -1;
	while(phase <= 2 * AWP_PI)
	{
		C_T = cos(phase);
		S_T = sin(phase);

		p.Y = center.Y - (AWPWORD) (W_S * C_T - H_C * S_T);
		if (p.Y > pImage->sSizeY) p.Y = pImage->sSizeY;
		if (p.Y < 0) p.Y = 0;

		p.X = center.X + (AWPWORD) (W_C * C_T + H_S * S_T);
		if (p.X > pImage->sSizeX) p.X = pImage->sSizeX;
		if (p.X < 0) p.X = 0;

		if (p_last.X == -1)
		{
			p_last = p;
			p_first = p;
		}
		else
			_CHECK_RESULT_((res = _awpDrawThickLine(pImage, p_last, p, bChan, dValue, radius)))

		p_last = p;

		phase += freq * 1;
	}

	_CHECK_RESULT_((res = _awpDrawThickLine(pImage, p_last, p, bChan, dValue, radius)))

CLEANUP:
    return res;
}


AWPRESULT _awpDrawPolygon(awpImage* pImage, awpContour* pContour, AWPBYTE bChan, AWPDOUBLE dValue)
{
	AWPRESULT res = AWP_OK;
	AWPDWORD i;

	if ((pContour == NULL) || (pContour->NumPoints == 0))
		_ERROR_EXIT_RES_(AWP_BADARG)
	_CHECK_RESULT_((res = awpCheckImage(pImage)))

	if (pContour->NumPoints == 1)
	{
		_CHECK_RESULT_((res = _awpDrawPoint(pImage, pContour->Points[0], bChan, dValue)))
	}
	else
	{
		for(i = 1; i < pContour->NumPoints; i++)
			_CHECK_RESULT_((res = _awpDrawLine(pImage, pContour->Points[i - 1], pContour->Points[i], bChan, dValue)))
	}

CLEANUP:
	return res;
}

AWPRESULT _awpDrawThickPolygon(awpImage* pImage, awpContour* pContour, AWPBYTE bChan, AWPDOUBLE dValue, AWPBYTE radius)
{
	AWPRESULT res = AWP_OK;
	AWPDWORD i;

	if ((pContour == NULL) || (pContour->NumPoints == 0))
		_ERROR_EXIT_RES_(AWP_BADARG)
	_CHECK_RESULT_((res = awpCheckImage(pImage)))

	if (pContour->NumPoints == 1)
	{
		_CHECK_RESULT_((res = _awpDrawThickPoint(pImage, pContour->Points[0], bChan, dValue, radius)))
	}
	else
	{
		for(i = 1; i < pContour->NumPoints; i++)
			_CHECK_RESULT_((res = _awpDrawThickLine(pImage, pContour->Points[i - 1], pContour->Points[i], bChan, dValue, radius)))
	}

CLEANUP:
	return res;
}


AWPRESULT awpFillPolygon(awpImage* pImage, awpContour* pContour, AWPBYTE bChan, AWPDOUBLE dValue, AWPDOUBLE dFillValue)
{
	typedef struct
	{
		awpPoint top;
		awpPoint bottom;
		AWPDOUBLE k;
	} PolygonVerge;

	AWPRESULT res = AWP_OK;
	AWPDWORD i, j;
	awpRect rect;
	awpPoint p1, p2;
	PolygonVerge* verges = NULL;
	AWPSHORT* cross_xs;
	AWPDWORD vrg_ind, crs_x_count;
	AWPSHORT min_x;
	AWPSHORT cur_y, max_y;

	if ((pContour == NULL) || (pContour->NumPoints == 0))
		_ERROR_EXIT_RES_(AWP_BADARG)
	_CHECK_RESULT_((res = awpCheckImage(pImage)))

	if (pContour->NumPoints == 1)
	{
		_CHECK_RESULT_((res = _awpDrawPoint(pImage, pContour->Points[0], bChan, dValue)))
	}
	else
	{
		for(i = 1; i < pContour->NumPoints; i++)
			_CHECK_RESULT_((res = _awpDrawLine(pImage, pContour->Points[i - 1], pContour->Points[i], bChan, dValue)))

		if ((pContour->Points[0].X == pContour->Points[pContour->NumPoints - 1].X) &&
			(pContour->Points[0].Y == pContour->Points[pContour->NumPoints - 1].Y))
		{
			_CHECK_RESULT_((res = awpGetContourRect(pContour, &rect)))
			if ((rect.left != rect.right) && (rect.top != rect.bottom))
			{
				//finding the first point
				cur_y = rect.top + 1;

				//forming array of verges

				verges = (PolygonVerge *) malloc(pContour->NumPoints * sizeof(PolygonVerge));
				if (verges == NULL)
					_ERROR_EXIT_RES_(AWP_BADMEMORY)
				cross_xs = (AWPSHORT *) malloc(pContour->NumPoints * sizeof(AWPSHORT));
				if (cross_xs == NULL)
					_ERROR_EXIT_RES_(AWP_BADMEMORY)

				while (cur_y < rect.bottom)
				{
					max_y = rect.bottom;
					vrg_ind = 0;

					for(i = 1; i < pContour->NumPoints; i++)
					{
						j = i - 1;

						if ((cur_y < pContour->Points[i].Y) && (max_y > pContour->Points[i].Y)) max_y = pContour->Points[i].Y;
						if ((pContour->Points[i].Y == pContour->Points[j].Y) && (pContour->Points[i].Y == cur_y)) max_y = cur_y;

						if (((pContour->Points[i].Y > cur_y) && (pContour->Points[j].Y > cur_y)) ||
							((pContour->Points[i].Y < cur_y) && (pContour->Points[j].Y < cur_y)))
							continue;

						if (pContour->Points[i].Y < pContour->Points[j].Y)
						{
							verges[vrg_ind].top = pContour->Points[i];
							verges[vrg_ind].bottom = pContour->Points[j];
							if (pContour->Points[i].X != pContour->Points[j].X)
								verges[vrg_ind].k = (AWPDOUBLE) (pContour->Points[i].Y - pContour->Points[j].Y) / (AWPDOUBLE) (pContour->Points[i].X - pContour->Points[j].X);
							else
								verges[vrg_ind].k = 0;
						}
						else
						{
							verges[vrg_ind].top = pContour->Points[j];
							verges[vrg_ind].bottom = pContour->Points[i];
							if (pContour->Points[j].X != pContour->Points[i].X)
								verges[vrg_ind].k = (AWPDOUBLE) (pContour->Points[j].Y - pContour->Points[i].Y) / (AWPDOUBLE) (pContour->Points[j].X - pContour->Points[i].X);
							else
								verges[vrg_ind].k = 0;
						}

						vrg_ind++;
					}

/*
					if (cur_y == 301)
					{
					for(i = 0; i < vrg_ind; i++)
					{
						if (verges[i].k != 0)
							awpDrawLine(pImage, verges[i].top, verges[i].bottom, 0, 127);
						else
							awpDrawLine(pImage, verges[i].top, verges[i].bottom, 0, 255);
						if (verges[i].k == 0)
						{
							printf("DK: %i - %i = %i\n",verges[i].top.Y, verges[i].bottom.Y, verges[i].top.Y - verges[i].bottom.Y);
							printf("DY: %i\n",verges[i].bottom.Y);
						}
					}

					break;
					}
*/
					//filling
					while (cur_y <= max_y)
					{
						crs_x_count = 0;
						for(i = 0; i < vrg_ind; i++)
						{
							if (verges[i].k != 0)
								cross_xs[crs_x_count] = (AWPSHORT) ((AWPDOUBLE) (cur_y - verges[i].bottom.Y) / verges[i].k) + verges[i].bottom.X;
							else if (verges[i].top.X == verges[i].bottom.X)
								cross_xs[crs_x_count] = verges[i].top.X;
							else if (verges[i].top.X > verges[i].bottom.X)
								cross_xs[crs_x_count] = verges[i].top.X;
							else
								cross_xs[crs_x_count] = verges[i].bottom.X;
							crs_x_count++;
						}
						i = 1;
						while(i < crs_x_count)
						{
							j = i - 1;
							min_x = cross_xs[j];
							if (min_x > cross_xs[i])
							{
								cross_xs[j] = cross_xs[i];
								cross_xs[i] = min_x;
								if (i > 1) i--;
							}
							else
								i++;
//							p2.X = cross_xs[i];
//							p2.Y = cur_y;
//							awpDrawPoint(pImage, p2, 0, 127);
						}
						i = 1;
						while(i < crs_x_count)
						{
							j = i - 1;
							p1.X = cross_xs[j] + 1;
							p1.Y = cur_y;
							p2.X = cross_xs[i] - 1;
							p2.Y = cur_y;
							if (p1.X <= p2.X)
								_CHECK_RESULT_((res = _awpDrawLine(pImage, p1, p2, bChan, dFillValue)))
							i +=2;
						}
						cur_y++;
					}
				}
			}
		}

	}

CLEANUP:
	if (verges != NULL)
		free(verges);
	if (cross_xs != NULL)
		free(cross_xs);
	return res;
}

AWPRESULT awpDrawPoint(awpImage* pImage, awpPoint p, AWPBYTE bChan, AWPDOUBLE dValue, AWPBYTE radius)
{
	AWPRESULT res;
	res = AWP_OK;
	if (radius == 0)
	{
		_CHECK_RESULT_(res = _awpDrawPoint(pImage, p, bChan, dValue))
	}
	else
	{
		_CHECK_RESULT_(res = _awpDrawThickPoint(pImage, p, bChan, dValue, radius))
	}

CLEANUP:
	return res;
}
AWPRESULT awpDrawLine(awpImage* pImage, awpPoint p1, awpPoint p2, AWPBYTE bChan, AWPDOUBLE dValue, AWPBYTE radius)
{
	AWPRESULT res;
	res = AWP_OK;
	if (radius == 0)
	{
		_CHECK_RESULT_(res = _awpDrawLine(pImage, p1, p2,  bChan, dValue))
	}
	else
	{
		_CHECK_RESULT_(res = _awpDrawThickLine(pImage, p1, p2,  bChan, dValue, radius))
	}

CLEANUP:
	return res;
}
AWPRESULT awpDrawRect(awpImage* pImage, awpRect* pRect, AWPBYTE bChan, AWPDOUBLE dValue, AWPBYTE radius)
{
	AWPRESULT res;
	res = AWP_OK;
	if (radius == 0)
	{
		_CHECK_RESULT_(res = _awpDrawRect(pImage, pRect,  bChan, dValue))
	}
	else
	{
		_CHECK_RESULT_(res = _awpDrawThickRect(pImage, pRect,  bChan, dValue, radius))
	}

CLEANUP:
	return res;
}
AWPRESULT awpDrawCross(awpImage* pImage, awpRect* pRect, AWPBYTE bChan, AWPDOUBLE dValue, AWPBYTE radius)
{
	AWPRESULT res;
	res = AWP_OK;
	if (radius == 0)
	{
		_CHECK_RESULT_(res = _awpDrawCross(pImage, pRect,  bChan, dValue))
	}
	else
	{
		_CHECK_RESULT_(res = _awpDrawThickCross(pImage, pRect,  bChan, dValue, radius))
	}

CLEANUP:
	return res;
}
AWPRESULT awpDrawEllipse(awpImage* pImage, awpPoint center, AWPWORD width, AWPWORD height, AWPDOUBLE angle, AWPBYTE bChan, AWPDOUBLE dValue, AWPBYTE radius)
{
	AWPRESULT res;
	res = AWP_OK;
	if (radius == 0)
	{
		_CHECK_RESULT_(res = _awpDrawEllipseV2(pImage, center, width, height, angle,  bChan, dValue))
	}
	else
	{
		_CHECK_RESULT_(res = _awpDrawThickEllipseV2(pImage, center, width, height, angle,  bChan, dValue, radius))
	}

CLEANUP:
	return res;
}
AWPRESULT awpDrawPolygon(awpImage* pImage, awpContour* pContour, AWPBYTE bChan, AWPDOUBLE dValue, AWPBYTE radius)
{
	AWPRESULT res;
	res = AWP_OK;
	if (radius == 0)
	{
		_CHECK_RESULT_(res = _awpDrawPolygon(pImage, pContour,  bChan, dValue))
	}
	else
	{
		_CHECK_RESULT_(res = _awpDrawThickPolygon(pImage, pContour,  bChan, dValue, radius))
	}

CLEANUP:
	return res;
}

AWPRESULT awpDrawEllipse2(awpImage* pImage, awpPoint center, AWPWORD major, AWPWORD minor, AWPDOUBLE angle, AWPBYTE bChan, AWPDOUBLE dValue, AWPBYTE radius)
{
	AWPRESULT res = AWP_OK;
	AWPDOUBLE phase, freq;
	AWPDOUBLE W_S, W_C, H_S, H_C;
	AWPDOUBLE C_T, S_T;
	awpPoint p_last, p_first, p;

	if ((major == 0) || (minor == 0))
		_ERROR_EXIT_RES_(AWP_BADARG)
	_CHECK_RESULT_((res = awpCheckImage(pImage)))
	/*check the channel*/
	if (bChan >= pImage->bChannels)
		_ERROR_EXIT_RES_(AWP_BADARG)

	freq = AWP_PI / major;
	angle = AWP_PI*angle / 180;
	W_S = major * sin(angle);
	W_C = major * cos(angle);

	H_S = minor * sin(angle);
	H_C = minor * cos(angle);

	phase = 0;
	p_last.X = -1;
	while(phase <= 2 * AWP_PI)
	{
		C_T = cos(phase);
		S_T = sin(phase);

		p.Y = center.Y - (AWPWORD) (W_S * C_T - H_C * S_T);
		if (p.Y > pImage->sSizeY) p.Y = pImage->sSizeY;
		if (p.Y < 0) p.Y = 0;

		p.X = center.X + (AWPWORD) (W_C * C_T + H_S * S_T);
		if (p.X > pImage->sSizeX) p.X = pImage->sSizeX;
		if (p.X < 0) p.X = 0;

		if (p_last.X == -1)
		{
			p_last = p;
			p_first = p;
		}
		else
        {
            if (radius > 0)
				_CHECK_RESULT_((res = _awpDrawThickLine(pImage, p_last, p, bChan, dValue, radius)))
            else
				_CHECK_RESULT_((res = _awpDrawLine(pImage, p_last, p, bChan, dValue)))
        }

		p_last = p;
		phase += freq;
	}

    if (radius > 0)
        _CHECK_RESULT_((res = _awpDrawThickLine(pImage, p_last, p, bChan, dValue, radius)))
    else
        _CHECK_RESULT_((res = _awpDrawLine(pImage, p_last, p, bChan, dValue)))

CLEANUP:
    return res;
}

AWPRESULT awpDrawEllipseCross(awpImage* pImage, awpPoint center, AWPWORD major, AWPWORD minor, AWPDOUBLE angle, AWPBYTE bChan, AWPDOUBLE dValue, AWPBYTE radius)
{
    awpPoint p1;
    awpPoint p2;
    AWPINT ax_len;
    AWPDOUBLE a;

	AWPRESULT res;
	res = AWP_OK;

    a = AWP_PI*angle / 180.;
    ax_len = major / 2;

    p1.X = (AWPSHORT)floor(center.X + ax_len*cos(a));
	p1.Y = (AWPSHORT)floor(center.Y + ax_len*sin(a));

	p2.X = (AWPSHORT)floor(center.X - ax_len*cos(a));
	p2.Y = (AWPSHORT)floor(center.Y - ax_len*sin(a));

	if (radius == 0)
	{
		_CHECK_RESULT_(res = _awpDrawLine(pImage, p1, p2,  bChan, dValue))
	}
	else
	{
		_CHECK_RESULT_(res = _awpDrawThickLine(pImage, p1, p2,  bChan, dValue, radius))
	}


    ax_len = minor / 2;
    a += AWP_PI / 2;
	p1.X = (AWPSHORT)floor(center.X + ax_len*cos(a));
	p1.Y = (AWPSHORT)floor(center.Y + ax_len*sin(a));

	p2.X = (AWPSHORT)floor(center.X - ax_len*cos(a));
	p2.Y = (AWPSHORT)floor(center.Y - ax_len*sin(a));

	if (radius == 0)
	{
		_CHECK_RESULT_(res = _awpDrawLine(pImage, p1, p2,  bChan, dValue))
	}
	else
	{
		_CHECK_RESULT_(res = _awpDrawThickLine(pImage, p1, p2,  bChan, dValue, radius))
	}


CLEANUP:
	return res;
}

