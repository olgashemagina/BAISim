#include "_awpipl.h"
#ifdef _OPENCV_
int awpImageDepthToCvDepth(awpImage* src)
{
    switch(src->dwType)
    {
        case AWP_BYTE:
            return IPL_DEPTH_8U;
        case AWP_SHORT:
            return IPL_DEPTH_8S;
        case AWP_FLOAT:
            return IPL_DEPTH_32F;
		case AWP_DOUBLE:
			return 64;
        default:
            return -1;
    }
}
//converts awpImage.awpType to openCV depth
//in the error case returns -1
static int awpCvDepthToImageDepth(IplImage* src)
{
    switch(src->depth)
    {
        case IPL_DEPTH_8U:
            return AWP_BYTE;
        case IPL_DEPTH_8S:
            return AWP_SHORT;
        case IPL_DEPTH_32F:
            return AWP_FLOAT;
		case 64:
			return AWP_DOUBLE;
        default:
            return -1;
    }
}
//returns line width of awpImage in BYTES
//in the error case returns -1
static int awpWidthStep(awpImage* src)
{
    int c = src->bChannels*src->sSizeX;
    switch(src->dwType)
    {
        case AWP_BYTE:
            return c*sizeof(BYTE);
        case AWP_SHORT:
            return c*sizeof(short);
        case AWP_FLOAT:
            return c*sizeof(float);
		case AWP_DOUBLE:
			return c*sizeof(double);
        default:
            return -1;
    }
}
// awp to openCV image convertion
IplImage* awpConvertToIplImage(awpImage* src)
{
  IplImage* result = NULL;
  if (src == NULL)
    return result;
  CvSize size;
  if (awpCheckImage(src) != AWP_OK)
    return result;
  size.width  = src->sSizeX;
  size.height = src->sSizeY;
  int depth = awpImageDepthToCvDepth(src);
  if (depth == -1)
    return result;
  int awpStep = awpWidthStep(src);
  if (awpStep == -1)
    return result;
  result = cvCreateImage(size, depth, src->bChannels);
  if (result == NULL)
    return result;
  // copy image pixels
  BYTE* awp = (BYTE*)src->pPixels;
  BYTE* ipl = (BYTE*)result->imageData;

  int iplStep = result->widthStep;
  for (int i = 0; i < src->sSizeY; i++)
  {
    memcpy(ipl,awp, awpStep);
    ipl += iplStep;
    awp += awpStep;
  }
  return result;
}

void awpImageToOpenCvImage(awpImage* src, IplImage* dst)
{
  if (awpCheckImage(src) != AWP_OK)
    return;
  int awpStep = awpWidthStep(src);
  int iplStep = dst->widthStep;
  BYTE* awp = (BYTE*)src->pPixels;
  BYTE* ipl = (BYTE*)dst->imageData;
  for (int i = 0; i < src->sSizeY; i++)
  {
    memcpy(ipl,awp, awpStep);
    ipl += iplStep;
    awp += awpStep;
  }
}

//openCV to awp convertion
awpImage* awpConvertToAwpImage(IplImage* src)
{
    awpImage* result = NULL;
    if (src == NULL)
        return result;
    int type = awpCvDepthToImageDepth(src);
    if (type < 0)
        return result;
	if (awpCreateImage(&result, src->width, src->height, src->nChannels, type) != AWP_OK)
        return result;
    BYTE* awp = (BYTE*)result->pPixels;
    BYTE* ipl = (BYTE*)src->imageData;
    int awpStep = awpWidthStep(result);
    int iplStep = src->widthStep;
    for (int i = 0; i < result->sSizeY; i++)
    {
      memcpy(awp, ipl, awpStep);
      ipl += iplStep;
      awp += awpStep;
    }
     return result;

}


AWPRESULT awpImageToOpenCv(awpImage* pImage, IplImage** ppImage)
{
	AWP_RESULT res = AWP_OK;
CLEANUP:
	return res;
}

AWPRESULT awpOpenCvToAwpImage(IplImage* pImage, awpImage** ppImage)
{
	AWP_RESULT res = AWP_OK;
CLEANUP:
	return res;
}
#endif // _OPENCV_
