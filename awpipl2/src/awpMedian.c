/*M
//
//      awpipl2 image processing and image analysis library
//		File: awpMedian.c
//		Purpose: median filter implementation
//		Autors: Alex A.Telnykh (tlnykha@yahoo.com)
//      CopyRight 2004-2019 (c) NN-Videolab.net
//M*/
#include "_awpipl.h"

AWPINT compare(const void* a, const void* b)
{
	AWPDOUBLE aa = *(AWPDOUBLE*)a;
	AWPDOUBLE bb = *(AWPDOUBLE*)b;
	if (aa > bb)
		return 1;
	else if (aa < bb)
		return -1;
	else
		return 0;
}


/*
	implementation median filter over double image 
*/
AWPRESULT __awpMedian_double(awpImage* src, awpImage* dst, AWPBYTE radius)
{
	AWPRESULT res = AWP_OK;

    AWPDOUBLE* _dst    = NULL;
    AWPDOUBLE* _src    = NULL;
    AWPDOUBLE* _buffer = NULL;
	AWPINT buffer_size = 0;
    AWPINT median_idx = 0;
    AWPINT 		c = 0;
    AWPSHORT    y = 0;
    AWPSHORT    x = 0;
    AWPBYTE		k = 0;
    AWPSHORT    xx = 0;
    AWPSHORT    yy = 0;
    AWPBYTE     _nc = 0;
    AWPWORD		_w = 0;
    AWPWORD     _h = 0;
	AWPINT sx = radius;
	AWPINT sy = radius;
    AWPINT _x = 0;
    AWPINT _y = 0;

    _dst = (AWPDOUBLE*)dst->pPixels;
    _src  = (AWPDOUBLE*)src->pPixels;
    _nc = src->bChannels;
    _w  = src->sSizeX;
    _h  = src->sSizeY;
    buffer_size = (2*radius + 1)*(2*radius + 1)*sizeof(AWPDOUBLE);
    _buffer = (AWPDOUBLE*)malloc(buffer_size);
    median_idx = buffer_size/sizeof(AWPDOUBLE) / 2;
	for (k = 0; k < _nc; k++)
    {
        for ( y = 0; y < _h; y++)
        {
            for ( x = 0; x < _w; x++)
            {
                c   = 0;
                for (yy = y - sy; yy <= y + sy; yy++)
                {
                   _y = yy < 0 ? abs(yy) : yy;
                   if (yy >= _h)
                   	_y =_h - 1;
                   for (xx = x- sx; xx <= x + sx; xx++)
                   {
						_x = xx < 0 ? abs(xx):xx >= _w ? _w -1 : xx;
                        _buffer[c] =_src[k+_nc*_x +_y*_w*_nc];
                        c++;
                   }
                }
                qsort(_buffer, buffer_size/sizeof(AWPDOUBLE), sizeof(AWPDOUBLE), compare);
                _dst[k+_nc*x+y*_nc*_w] = _buffer[median_idx];
            }
        }
    }
	_SAFE_FREE_(_buffer)
	return res;
}

/*
	multichannel median filter 
*/
AWPRESULT awpMedian(awpImage* src, awpImage* dst, AWPBYTE radius)
{
	AWPRESULT res = AWP_OK;
	awpImage* median_src = NULL;
	awpImage* median_dst = NULL;
	_CHECK_RESULT_(res = awpCheckImage(src))
	_CHECK_RESULT_(res = awpCheckImage(dst))
	_CHECK_SAME_SIZES(src, dst)
	_CHECK_SAME_TYPE(src, dst)
	_CHECK_NUM_CHANNELS(src, dst)

	_CHECK_RESULT_(res = awpCreateImage(&median_dst, src->sSizeX, src->sSizeY, src->bChannels, AWP_DOUBLE))
	_CHECK_RESULT_(res = awpCopyImage(src, &median_src))
    if (median_src->dwType != AWP_DOUBLE)
    	_CHECK_RESULT_(res = awpConvert(median_src, AWP_CONVERT_TO_DOUBLE))

	_CHECK_RESULT_(res = __awpMedian_double(median_src, median_dst, radius))

	switch (src->dwType)
	{
	case AWP_BYTE:
		_CHECK_RESULT_(res = awpConvert(median_dst, AWP_CONVERT_TO_BYTE_WITH_NORM));
		break;
	case AWP_SHORT:
		_CHECK_RESULT_(res = awpConvert(median_dst, AWP_CONVERT_TO_SHORT));
		break;
	case AWP_FLOAT:
		_CHECK_RESULT_(res = awpConvert(median_dst, AWP_CONVERT_TO_FLOAT));
		break;
	}
	_CHECK_RESULT_(res = awpCopyImage(median_dst, &dst));

CLEANUP:
	_SAFE_RELEASE_(median_src);
	_SAFE_RELEASE_(median_dst);

	return res;
}