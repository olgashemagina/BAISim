#include "_awpipl.h"

//A macro that performs interpolation in the x - direction, regardless of
//type of image.

#define _AWP_X_INTERPOLATION_(type)\
{   AWPBYTE i = 0;\
	AWPDOUBLE *d;\
	AWPDOUBLE f1, f2;\
	type* b;\
	type* bp1;\
	type* bp2;\
    b = _AWP_BPIX_(src, type);\
    bp1 = &b[(AWPINT)((p1.X + p1.Y*src->sSizeX)*src->bChannels)];\
    bp2 = &b[(AWPINT)((p2.X + p2.Y*src->sSizeX)*src->bChannels)];\
    d = _AWP_BPIX_(res, AWPDOUBLE);\
    f1 = (p2.X - p.X) / (p2.X - p1.X);\
    f2 = (p.X - p1.X) / (p2.X - p1.X);\
    for (i = 0; i < src->bChannels; i++)\
    {\
          d[i] =  f1*bp1[i] + f2*bp2[i];\
    }}\


//A macro that performs interpolation in the y - direction, regardless of
//type of image.

#define _AWP_Y_INTERPOLATION_(type)\
{   AWPBYTE i = 0;\
	AWPDOUBLE *d;\
	AWPDOUBLE f1, f2;\
	type* b;\
	type* bp1;\
	type* bp2;\
    b = _AWP_BPIX_(src, type);\
    bp1 = &b[(AWPINT)((p1.X + p1.Y*src->sSizeX)*src->bChannels)];\
    bp2 = &b[(AWPINT)((p2.X + p2.Y*src->sSizeX)*src->bChannels)];\
    d = _AWP_BPIX_(res, AWPDOUBLE);\
    f1 = (p2.Y - p.Y) / (p2.Y - p1.Y);\
    f2 = (p.Y - p1.Y) / (p2.Y - p1.Y);\
    for (i = 0; i < src->bChannels; i++)\
    {\
          d[i] =  f1*bp1[i] + f2*bp2[i];\
    }}\

// without interpolation
#define _AWP_WITHOUT_INTERPOLATION_(type)\
{   AWPBYTE i = 0;\
	AWPDOUBLE *d;\
	type* b;\
    b = _AWP_BPIX_(src, type);\
    d = _AWP_BPIX_(res, AWPDOUBLE);\
    for (i = 0; i < src->bChannels; i++)\
    {\
          d[i] =  b[(AWPINT)((p.X + p.Y*src->sSizeX)*src->bChannels)];\
    }}\
// interpolate the x-axis interpolation between two points p1 and p2 at the point p
// the result is written to the res variable, depending on the type of image.

static  void _awpXInterpoalation(awpImage* src, awp2DPoint p1, awp2DPoint p2, awp2DPoint p, awpImage* res)
{

    switch(src->dwType)
    {
        case AWP_BYTE:
        _AWP_X_INTERPOLATION_(AWPBYTE)
        break;
        case AWP_SHORT:
        _AWP_X_INTERPOLATION_(AWPSHORT)
        break;
        case AWP_FLOAT:
        _AWP_X_INTERPOLATION_(AWPFLOAT)
        break;
        case AWP_DOUBLE:
        _AWP_X_INTERPOLATION_(AWPDOUBLE)
        break;

    }
}

static  void _awpSimpleYInterpoalation(awpImage* src, awp2DPoint p1, awp2DPoint p2, awp2DPoint p, awpImage* res)
{

    switch(src->dwType)
    {
        case AWP_BYTE:
        _AWP_Y_INTERPOLATION_(AWPBYTE)
        break;
        case AWP_SHORT:
        _AWP_Y_INTERPOLATION_(AWPSHORT)
        break;
        case AWP_FLOAT:
        _AWP_Y_INTERPOLATION_(AWPFLOAT)
        break;
        case AWP_DOUBLE:
        _AWP_Y_INTERPOLATION_(AWPDOUBLE)
        break;

    }
}

static  void _awpWithoutInterpoalation(awpImage* src, awp2DPoint p, awpImage* res)
{

    switch(src->dwType)
    {
        case AWP_BYTE:
        _AWP_WITHOUT_INTERPOLATION_(AWPBYTE)
        break;
        case AWP_SHORT:
        _AWP_WITHOUT_INTERPOLATION_(AWPSHORT)
        break;
        case AWP_FLOAT:
        _AWP_WITHOUT_INTERPOLATION_(AWPFLOAT)
        break;
        case AWP_DOUBLE:
        _AWP_WITHOUT_INTERPOLATION_(AWPDOUBLE)
        break;

    }
}

// interpolate y interpolation between two points p1 and p2 at point p
// the result is written to the res variable, depending on the type of image.

static  void _awpYInterpoalation(awpImage* img1, awp2DPoint p1, awpImage* img2, awp2DPoint p2, awp2DPoint p, awpImage* res)
{
    AWPBYTE i = 0;
	AWPDOUBLE f1, f2;
	AWPDOUBLE* d1, *d2, *d;
    d1 = _AWP_BPIX_(img1, AWPDOUBLE);
    d2 = _AWP_BPIX_(img2, AWPDOUBLE);
    d =  _AWP_BPIX_(res, AWPDOUBLE);

    f1 =  (p2.Y - p.Y) / (p2.Y - p1.Y);
    f2 =  (p.Y - p1.Y) / (p2.Y - p1.Y);

    for (i = 0; i < res->bChannels; i++)
    {
        d[i] = f1*d1[i] + f2*d2[i];
    }

}
//---------------------------------------------------------------------------
AWPRESULT awpBilinearInterpolation (awpImage* src, awp2DPoint* p, awpImage* dst)
{
	// internal variables
    awp2DPoint pp[6];
    awpImage* I4 = NULL;
    awpImage* I5 = NULL;

	// check the validity of the arguments.
    if (src == NULL || p == NULL || dst == NULL)
        return AWP_BADARG;
	// the intensity value in the interpolated point p is always of the type
	// AWPDOUBLE, so the argument dst must be of type AWPDOUBLE
    if (dst->dwType != AWP_DOUBLE)
        return AWP_BADARG;
	// the number of channels of the original image should be equal to the width
	// image - result.
    if (src->bChannels != dst->sSizeX)
        return AWP_BADARG;
	// the number of channels of the resulting image must be 1
    if (dst->bChannels != 1)
        return AWP_BADARG;
    // точка должна лежать внутри изображения целиком.
    if (p->X < 1 || p->X > src->sSizeX -1 || p->Y < 1 || p->Y > src->sSizeY)
        return AWP_BADARG;

    memset(pp, 0, sizeof(pp));

	// find the coordinates of the surrounding points, including auxiliary
    pp[0].X = floor(p->X);
    pp[0].Y = floor(p->Y);

    pp[1].X = ceil(p->X);
    pp[1].Y = floor(p->Y);

    pp[2].X = floor(p->X);
    pp[2].Y = ceil(p->Y);

    pp[3].X = ceil(p->X);
    pp[3].Y = ceil(p->Y);

    pp[4].X = p->X;
    pp[4].Y = floor(p->Y);

    pp[5].X = p->X;
    pp[5].Y = ceil(p->Y);

	if( floor(p->X) == ceil(p->X) && floor(p->Y) == ceil(p->Y) )
	{
		_awpWithoutInterpoalation(src, *p, dst);
		return AWP_OK;
	}

	if( floor(p->X) == ceil(p->X) )
		_awpSimpleYInterpoalation(src, pp[2], pp[0], *p, dst);
	else if( floor(p->Y) == ceil(p->Y) )
		_awpXInterpoalation(src, pp[3], pp[2], *p, dst);
	else
	{
		// create buffers to store intermediate results
		awpCreateImage(&I4, src->bChannels, 1, 1, AWP_DOUBLE);
		awpCreateImage(&I5, src->bChannels, 1, 1, AWP_DOUBLE);
		// perform linear interpolation for points pp [5] pp [4]
		_awpXInterpoalation(src, pp[1], pp[0], pp[4], I4);
		_awpXInterpoalation(src, pp[3], pp[2], pp[5], I5);
		// execute linear interpolation for the point p
		_awpYInterpoalation(I5, pp[5], I4, pp[4], *p, dst);
		awpReleaseImage(&I4);
		awpReleaseImage(&I5);
	}
    return AWP_OK;
}

