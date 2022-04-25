#include "_awpipl.h"
static AWPDOUBLE blur_mask[9] =
	{1,1,1,
	 1,1,1,
	 1,1,1};

static AWPDOUBLE blur_more_mask[25] =
	{1,1,1,1,1,
	 1,1,1,1,1,
	 1,1,1,1,1,
	 1,1,1,1,1,
	 1,1,1,1,1};

static AWPDOUBLE sharpen_mask[9] =
	{0,-1,0,
	 -1,5,-1,
	 0,-1,0};

static AWPDOUBLE sharpen_more_mask[9] =
	{-1,-1,-1,
	 -1,9,-1,
	 -1,-1,-1};

static AWPDOUBLE sharpen_edges_mask[9] =
	{1,-2,1,
	 -2,5,-2,
	 1,-2,1};

static AWPDOUBLE find_edges_mask[9] =
	{-1,-1,-1,
	 -1,8,-1,
	 -1,-1,-1};

static AWPDOUBLE find_edges1_mask[9] =
	{1,-2,1,
	 -2,4,-2,
	 1,-2,1};

static AWPDOUBLE emboss_north_mask[9] =
	{1,1,1,
	 1,-2,1,
	 -1,-1,-1};

static AWPDOUBLE emboss_north_ost_mask[9] =
	{1,1,1,
	 -1,-2,1,
	 -1,-1,1};

static AWPDOUBLE emboss_ost_mask[9] =
	{-1,1,1,
	 -1,-2,1,
	 -1,1,1};

static AWPDOUBLE emboss_south_ost_mask[9] =
	{-1,-1,1,
	 -1,-2,1,
	 1,1,1};

static AWPDOUBLE emboss_south_mask[9] =
	{-1,-1,-1,
	  1,-2,1,
	  1,1,1};

static AWPDOUBLE emboss_south_west_mask[9] =
	{1,-1,-1,
	 1,-2,-1,
	 1,1,1};

static AWPDOUBLE emboss_west_mask[9] =
	{1,1,-1,
	 1,-2,-1,
	 1,1,-1};

static AWPDOUBLE emboss_north_west_mask[9] =
	{1,1,1,
	 1,-2,-1,
	 1,-1,-1};

static AWPDOUBLE prewitt_h_mask[9] =
	{1,1,1,
	 0,0,0,
	 -1,-1,-1};

static AWPDOUBLE prewitt_v_mask[9] =
	{-1,0,1,
	 -1,0,1,
	 -1,0,1};
static AWPDOUBLE sobel_v_mask[9] =
	{-1,0,1,
	 -2,0,2,
	 -1,0,1};
static AWPDOUBLE sobel_h_mask[9] =
	{1,2,1,
	 0,0,0,
	 -1,-2,-1};


/*
	performs convolution on the multichannel image pSrcImage
    mask should be single channel and AWP_DOUBLE type

*/
AWPRESULT awpMaskConvolution(awpImage* src, awpImage* dst, awpImage* mask)
{
    AWPRESULT res = AWP_OK;
    awpImage* img_conv = NULL;
    awpImage* img_src = NULL;
    AWPDOUBLE* _conv   = NULL;
    AWPDOUBLE* _mask   = NULL;
    AWPDOUBLE* _src   = NULL;
    AWPINT 		i = 0;
    AWPINT 		c = 0;
    AWPSHORT    y = 0;
    AWPSHORT    x = 0;
    AWPBYTE		k = 0;
    AWPSHORT    xx = 0;
    AWPSHORT    yy = 0;
    AWPDOUBLE   sum = 0;
    AWPDOUBLE	msum = 0;
    AWPBYTE     _nc = 0;
    AWPWORD		_w = 0;
    AWPWORD     _h = 0;
	AWPINT sx = 0;
	AWPINT sy = 0;
    AWPINT _x = 0;
    AWPINT _y = 0;

	/*
    	check src and dst image
    */
	_CHECK_RESULT_(res = awpCheckImage(src))
    _CHECK_RESULT_(res = awpCheckImage(dst))
    _CHECK_SAME_SIZES(src, dst)
    _CHECK_SAME_TYPE(src,dst)
    _CHECK_NUM_CHANNELS(src, dst)
    /*
    	check mask image
    */
    _CHECK_RESULT_(res = awpCheckImage(mask))
    if (mask->bChannels != 1 && mask->dwType != AWP_DOUBLE)
		_ERROR_EXIT_RES_(AWP_BADARG)

    sx = (AWPINT)floor((AWPDOUBLE)mask->sSizeX / 2.);
    sy = (AWPINT)floor((AWPDOUBLE)mask->sSizeY / 2.);
    /*
		prepare variables for convolution process
    */
    _CHECK_RESULT_(res = awpCreateImage(&img_conv, src->sSizeX, src->sSizeY, src->bChannels, AWP_DOUBLE))
    _CHECK_RESULT_(res = awpCopyImage(src, &img_src))
    if (img_src->dwType != AWP_DOUBLE)
    	_CHECK_RESULT_(res = awpConvert(img_src, AWP_CONVERT_TO_DOUBLE))

    _conv = (AWPDOUBLE*)img_conv->pPixels;
    _mask = (AWPDOUBLE*)mask->pPixels;
    _src  = (AWPDOUBLE*)img_src->pPixels;
    _nc = img_conv->bChannels;
    _w  = img_conv->sSizeX;
    _h  = img_conv->sSizeY;

    msum = 0;
    for (i = 0; i < mask->sSizeX*mask->sSizeY; i++)
    	msum += _mask[i];
    if (msum == 0)
        msum = 1;

	for (k = 0; k < _nc; k++)
    {
        for ( y = 0; y < _h; y++)
        {
            for ( x = 0; x < _w; x++)
            {
                sum = 0;
                c   = 0;
                for (yy = y - sy; yy <= y + sy; yy++)
                {
                   _y = yy < 0 ? abs(yy) : yy;
                   if (yy >= _h)
                   	_y =_h - 1;
                   for (xx = x- sx; xx <= x + sx; xx++)
                   {
						_x = xx < 0 ? abs(xx):xx >= _w ? _w -1 : xx;

                        sum += _mask[c++]*_src[k+_nc*_x +_y*_w*_nc];
                   }
                }
                _conv[k+_nc*x+y*_nc*_w] = sum/msum;
            }
        }
    }

    switch(src->dwType)
    {
     	case AWP_BYTE:
    		_CHECK_RESULT_(res = awpConvert(img_conv, AWP_CONVERT_TO_BYTE_WITH_NORM));
	    break;
        case AWP_SHORT:
    		_CHECK_RESULT_(res = awpConvert(img_conv, AWP_CONVERT_TO_SHORT));
        break;
        case AWP_FLOAT:
    		_CHECK_RESULT_(res = awpConvert(img_conv, AWP_CONVERT_TO_FLOAT));
        break;
    }
    _CHECK_RESULT_(res = awpCopyImage(img_conv, &dst));


CLEANUP:
	_SAFE_RELEASE_(img_conv);
	_SAFE_RELEASE_(img_src);
    return res;
}


AWPRESULT awpFilter(awpImage* src, awpImage* dst,  AWPINT options)
{
	AWPRESULT res = AWP_OK;
	awpImage* mask = NULL;
	AWPWORD wMSizeX, wMSizeY;
	void* pMaskArray = NULL;
	switch (options)
	{
	case AWP_BLUR:
		wMSizeX = wMSizeY = 3;
		pMaskArray = blur_mask;
		break;
	case AWP_BLUR_MORE:
		wMSizeX = wMSizeY = 5;
		pMaskArray = blur_more_mask;
		break;
	case AWP_SHARPEN:
		wMSizeX = wMSizeY = 3;
		pMaskArray = sharpen_mask;
		break;
	case AWP_SHARPEN_MORE:
		wMSizeX = wMSizeY = 3;
		pMaskArray = sharpen_more_mask;
		break;
	case AWP_SHARPEN_EDGES:
		wMSizeX = wMSizeY = 3;
		pMaskArray = sharpen_edges_mask;
		break;
	case AWP_FIND_EDGES:
		wMSizeX = wMSizeY = 3;
		pMaskArray = find_edges_mask;
		break;
	case AWP_FIND_EDGES1:
		wMSizeX = wMSizeY = 3;
		pMaskArray = find_edges1_mask;
		break;
	case AWP_EMBOSS_NORTH:
		wMSizeX = wMSizeY = 3;
		pMaskArray = emboss_north_mask;
		break;
	case AWP_EMBOSS_NORTH_OST:
		wMSizeX = wMSizeY = 3;
		pMaskArray = emboss_north_ost_mask;
		break;
	case AWP_EMBOSS_OST:
		wMSizeX = wMSizeY = 3;
		pMaskArray = emboss_ost_mask;
		break;
	case AWP_EMBOSS_SOUTH_OST:
		wMSizeX = wMSizeY = 3;
		pMaskArray = emboss_south_ost_mask;
		break;
	case AWP_EMBOSS_SOUTH:
		wMSizeX = wMSizeY = 3;
		pMaskArray = emboss_south_mask;
		break;
	case AWP_EMBOSS_SOUTH_WEST:
		wMSizeX = wMSizeY = 3;
		pMaskArray = emboss_south_west_mask;
		break;
	case AWP_EMBOSS_WEST:
		wMSizeX = wMSizeY = 3;
		pMaskArray = emboss_west_mask;
		break;
	case AWP_EMBOSS_NORTH_WEST:
		wMSizeX = wMSizeY = 3;
		pMaskArray = emboss_north_west_mask;
		break;
	case AWP_PREWITT_V:
		wMSizeX = wMSizeY = 3;
		pMaskArray = prewitt_v_mask;
		break;
	case AWP_PREWITT_H:
		wMSizeX = wMSizeY = 3;
		pMaskArray = prewitt_h_mask;
		break;
	case AWP_SOBEL_V:
		wMSizeX = wMSizeY = 3;
		pMaskArray = sobel_v_mask;
		break;
	case AWP_SOBEL_H:
		wMSizeX = wMSizeY = 3;
		pMaskArray = sobel_h_mask;
		break;
	default:
		res = AWP_BADARG;
		_ERR_EXIT_
	}

	_CHECK_RESULT_(res = awpCreateImage(&mask, wMSizeX, wMSizeY, 1, AWP_DOUBLE))
	memcpy((void*)mask->pPixels, pMaskArray, wMSizeY*wMSizeX*sizeof(AWPDOUBLE));
	_CHECK_RESULT_(res = awpMaskConvolution(src, dst, mask))

CLEANUP:
	_SAFE_RELEASE_(mask)
	return res;
}

