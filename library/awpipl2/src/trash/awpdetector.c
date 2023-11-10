
#include "_awpipl.h"
#include "stdlib.h"

typedef struct
{
	awpRect		r;
	AWPSHORT	c;
}cawpRect;

static AWPFLOAT _awpRectsOverlap(awpRect r1, awpRect r2)
{
 AWPFLOAT colInt, rowInt, intersection, area1, area2;
 AWPINT width = r1.right - r1.left;
 AWPINT height = r1.bottom - r1.top;
 AWPINT _width = r2.right - r2.left;
 AWPINT _height = r2.bottom - r2.top;
 if (r2.left > r1.left + width)
    return 0;
 if (r2.top > r1.top + height)
    return 0;
 if (r2.left + _width < r1.left)
    return 0;
 if (r1.top + _height < r1.top)
    return 0;

  colInt = (AWPFLOAT)AWP_MIN(r2.left + _width, r1.left + width) - AWP_MAX(r1.left, r1.left);
  rowInt = (AWPFLOAT)AWP_MIN(r2.top + _height, r1.top + height) - AWP_MAX(r2.top, r1.top);

  intersection = colInt*rowInt;
  area1 = (AWPFLOAT)(_width*_height);
  area2 = (AWPFLOAT)(width*height);

 return intersection / (area1+area2-intersection);
}

static void _awpClusterRects(int numSrc, awpRect* pSrc, int* numDst, awpRect** ppDst)
{
	AWPINT i,j, NumClusters, idx, eCount, cidx;
	AWPBOOL found;
	AWPFLOAT overlap;
	awpRect r;
	cawpRect* crects = (cawpRect*)malloc(numSrc*sizeof(cawpRect));
	for ( i = 0; i < numSrc; i++)
	{
		crects[i].r = pSrc[i];
		crects[i].c = -1;
	}

	NumClusters = 0;
	found = FALSE;
    idx = 0;
	do
	{
		found = FALSE;
		for ( i = 0; i < numSrc; i++)
		{
			if (crects[i].c == -1)
			{
				found = TRUE;
				NumClusters++;
				crects[i].c = NumClusters;
				idx = i;
				break;
			}
		}
		if (found)
		{
			for ( i = 0; i < numSrc; i++)
			{
				if (i == idx)
					continue;
				if (crects[i].c != -1)
					continue;
			    overlap = _awpRectsOverlap(crects[i].r , crects[idx].r);
				if (overlap > 0.5)
				{
					crects[i].c = NumClusters;
				}
			}
		}
	}while(found);


    eCount = 0;
	cidx = 0;
	(*numDst) = NumClusters;
	*ppDst = (awpRect*)malloc(NumClusters*sizeof(awpRect));
	for ( i = 0; i < NumClusters; i++)
	{
		cidx = i+1;
		r.left = 0;
		r.right = 0;
		r.bottom = 0;
		r.top = 0;
		eCount = 0;
		for ( j = 0; j < numSrc; j++)
		{
			if (crects[j].c == cidx)
			{
				r.left += crects[j].r.left;
				r.bottom += crects[j].r.bottom;
				r.right += crects[j].r.right;
				r.top += crects[j].r.top;
				eCount++;
			}
		}
		r.left /= eCount;
		r.right /= eCount;
		r.bottom /= eCount;
		r.top /= eCount;
		(*ppDst)[i] = r;
	}

	free(crects);
}
 AWPDOUBLE __inline CalcSum(double* pix, int x, int y, int w, int h, int ww )
{
    AWPDOUBLE* p = pix + x - 1 + (y - 1)*ww;
    h = h*ww;
    return (p[0] + p[w + h] - p[h] - p[w]);
}
/**
* @brief Global Modified Census Transform Detector (GMCT).

*/
static AWPBOOL GMCTDetector(awpImage* pImage, awpImage* pImage1, awpRect* pRect, awpDetector* pDetector)
{
   AWPINT i,j,sx,sy, sw, sh, idx, x, y, width, height,ww;
   AWPDOUBLE tarea, area, total, alfa, sum, invarea,s2,m,disp2;
   AWPDOUBLE* pix;
   AWPDOUBLE* pix1;
   AWPINT    iv[9];
   awpWeak* weak;
   alfa = (AWPDOUBLE)(pRect->right - pRect->left)/(AWPDOUBLE)pDetector->width;
   pix =  (AWPDOUBLE*)pImage->pPixels;
   pix1 = NULL;
   if (pImage1)
        pix1 = (AWPDOUBLE*)pImage1->pPixels;

   x = pRect->left;
   y = pRect->top;
   width = pRect->right - x;
   height = pRect->bottom - y;
   ww = pImage->sSizeX;
   invarea = 1.0/(double)((width-1) * (height -1));
   s2 = 1;
   if (pix1 != NULL)
        s2 = CalcSum(pix1, x+1, y+1, width-1, height-1 , ww) * invarea;

     m =  CalcSum(pix, x+1, y+1, width-1, height-1, ww )*invarea;
     disp2 = s2 - m*m;

    if (pix1 != NULL)
    {
        if ( (disp2 <  25*25) || ( m < 25) || (m > 195 ) )
			return FALSE;
    }


	for ( i = 0; i < pDetector->nStages; i++)
	{
		sum = 0;
		for (j = 0; j < pDetector->pStrongs[i].nWeaks; j++)
		{
			memset(iv, 0, sizeof(iv));
			
			weak = &pDetector->pStrongs[i].pWeaks[j];
			tarea = 1.0/(9.0*alfa*alfa*weak->w*weak->h);
			area  = 1.0/(alfa*alfa*weak->w*weak->h);
			sx = pRect->left + (AWPINT)(alfa*weak->x + 0.5);
			sy = pRect->top  + (AWPINT)(alfa*weak->y + 0.5);
			sw = (AWPINT)(alfa*weak->w + 0.5);
			sh = (AWPINT)(alfa*weak->h + 0.5);

			total = CalcSum(pix, sx, sy, 3*sw, 3*sh, ww)*tarea;

			iv[0] = CalcSum(pix, sx, sy, sw, sh, ww) * area > total?1:0;
			iv[1] = CalcSum(pix, sx + sw, sy, sw, sh, ww) * area> total?1:0;
			iv[2] = CalcSum(pix, sx + 2*sw, sy, sw, sh, ww) * area> total?1:0;
			iv[3] = CalcSum(pix, sx, sy + sh, sw, sh, ww) * area> total?1:0;
			iv[4] = CalcSum(pix, sx + sw, sy + sh, sw, sh, ww) * area> total?1:0;
			iv[5] = CalcSum(pix, sx + 2*sw, sy + sh, sw, sh, ww) * area> total?1:0;
			iv[6] = CalcSum(pix, sx, sy + 2*sh, sw, sh, ww) * area> total?1:0;
			iv[7] = CalcSum(pix, sx + sw, sy + 2*sh, sw, sh,ww) * area> total?1:0;
			iv[8] = CalcSum(pix, sx + 2*sw, sy + 2*sh, sw, sh, ww) * area> total?1:0;

			
			idx = 0;
			idx |= iv[0];
			idx  =idx << 1;
			idx |= iv[1];
			idx  =idx << 1;
			idx |= iv[2];
			idx  =idx << 1;
			idx |= iv[3];
			idx  =idx << 1;
			idx |= iv[4];
			idx  =idx << 1;
			idx |= iv[5];
			idx  =idx << 1;
			idx |= iv[6];
			idx  =idx << 1;
			idx |= iv[7];
			idx  =idx << 1;
			idx |= iv[8];

			sum += weak->bins[idx] > 0 ? weak->alfa : 0;
		}

		if ((sum - pDetector->pStrongs[i].alfa) < -0.00001)
		return FALSE;
	}

	return TRUE;
}


/**
*	@brief Initialize Global Modified Census Transform (GMCT) 
*	sourse: cascade in the awpImage img 
*	result: cascade in the awpCascade sructure 
*/
AWPRESULT _awpInitGMCTCascade(awpImage* img, awpCascade* pCascade)
{
	AWPINT ns,i,j,k;
	awpDetector* d;
	AWPDOUBLE* data;
	if (img == NULL)
		return AWP_BADARG;
	if (pCascade == NULL)
		return AWP_BADARG;
	if (pCascade->detectorCascade.type != AWP_NULL_DETECTOR)
		return AWP_BADARG;
	if (img->sSizeX != AWP_GMST_WIDTH)
		return AWP_BADARG;
	if (img->dwType != AWP_DOUBLE)
		return AWP_BADARG;

	d = &pCascade->detectorCascade;
    data = (double*)img->pPixels;


	d->type   = AWP_GMCT_DETECTOR;
	d->width  = (AWPSHORT)data[1];
	d->height = (AWPSHORT)data[2];

	ns = (int)data[3];
	if (ns <= 0)
		return AWP_BADARG;
	d->nStages = ns;
	d->pStrongs = (awpStrong*)malloc(ns*sizeof(awpStrong));
	if (d->pStrongs == NULL)
		goto err;
	data += AWP_GMST_WIDTH;
	for (i = 0; i < ns; i++)
	{
		d->pStrongs[i].nWeaks = (AWPSHORT)data[0];
		d->pStrongs[i].pWeaks = (awpWeak*)malloc(d->pStrongs[i].nWeaks*sizeof(awpWeak));
		if (d->pStrongs[i].pWeaks == NULL)
			goto err;
		d->pStrongs[i].alfa   = data[1];
		data += AWP_GMST_WIDTH;
		for (j = 0; j < d->pStrongs[i].nWeaks; j++)
		{
			d->pStrongs[i].pWeaks[j].numBins = AWP_MCT_NUMBINS;
			d->pStrongs[i].pWeaks[j].bins = (AWPDOUBLE*)malloc(AWP_MCT_NUMBINS*sizeof(AWPDOUBLE));
			if (d->pStrongs[i].pWeaks[j].bins == NULL)
				goto err;
			d->pStrongs[i].pWeaks[j].x = (AWPSHORT)data[0];
			d->pStrongs[i].pWeaks[j].y = (AWPSHORT)data[1];
			d->pStrongs[i].pWeaks[j].w = (AWPSHORT)data[2];
			d->pStrongs[i].pWeaks[j].h = (AWPSHORT)data[3];
			d->pStrongs[i].pWeaks[j].alfa = data[4];
			for (k = 0; k < AWP_MCT_NUMBINS; k++)
				d->pStrongs[i].pWeaks[j].bins[k] = data[k+5];
			data += AWP_GMST_WIDTH;
		}
	}
	pCascade->detectFunc = GMCTDetector;
	return AWP_OK;
err:
	return AWP_BADARG;
}
AWPRESULT _awpInitLMCTCascade(awpImage* img, awpCascade* pCascade)
{
	// todo:
	return AWP_OK;
}
AWPRESULT _awpInitGLBPCascade(awpImage* img, awpCascade* pCascade)
{
	// todo:
	return AWP_OK;
}
AWPRESULT _awpInitLLBPCascade(awpImage* img, awpCascade* pCascade)
{
	//todo: 
	return AWP_OK;
}
//---------------------------------------------------------------------------------------
AWPRESULT awpCreateCascade(awpCascade** ppCascade)
{
	*ppCascade = (awpCascade*)malloc(sizeof(awpCascade));
	if (*ppCascade == NULL)
		return AWP_BADMEMORY;
	memset(*ppCascade, 0, sizeof(awpCascade));
	return AWP_OK;
}

AWPRESULT awpFreeCascade(awpCascade** ppCascade)
{
	if (awpReleaseCascade(*ppCascade) != AWP_OK)
		return AWP_BADARG;
	free(*ppCascade);
	*ppCascade = NULL;
	return AWP_OK;
}
AWPRESULT awpInitCascade(awpCascade* pCascade, awpImage* img)
{
	AWPRESULT res = AWP_OK;
	AWPDOUBLE* pix;
	AWPINT     type;

	if (pCascade == NULL || img == NULL)
		return AWP_BADARG;

   pix = (double*)img->pPixels;
   type = (int)pix[0];
	if (img->dwType != AWP_DOUBLE || img->bChannels != 1)
	{
		res =  AWP_BADARG;
		return res;
	}
	switch (type)
	{
	case AWP_GMCT_DETECTOR: 
		res = _awpInitGMCTCascade(img, pCascade);
		break;
	case AWP_LMCT_DETECTOR:
		res = _awpInitLMCTCascade(img, pCascade);
		break;
	case AWP_GLBP_DETECTOR:
		res = _awpInitGLBPCascade(img, pCascade);
		break;
	case AWP_LLBP_DETECTOR:
		res = _awpInitLLBPCascade(img, pCascade);
		break;
	default:
		res = AWP_BADARG; 
	}

	return res;

}
/*
*/
AWPRESULT awpLoadCascade(awpCascade* pCascade, const char* lpFileName)
{
	AWPRESULT res = AWP_OK;
	awpImage* tmp = NULL;

	if (pCascade == NULL)
		return AWP_BADARG;
	if (awpLoadImage(lpFileName, &tmp) != AWP_OK)
		return AWP_BADARG;

	res =  awpInitCascade(pCascade,tmp);

cleanup:
	awpReleaseImage(&tmp);
	return res;
}

AWPRESULT awpReleaseCascade(awpCascade* pCascade)
{
	AWPINT i,j;
	awpWeak w;
	awpStrong s;
	if (pCascade == NULL)
		return AWP_BADARG;
	pCascade->detectFunc = NULL;
	for ( i = 0; i < pCascade->detectorCascade.nStages; i++)
	{
	    s = pCascade->detectorCascade.pStrongs[i];
		for ( j = 0; j < s.nWeaks; j++)
		{
		    w = s.pWeaks[j];
			w.numBins = 0;
			w.x = 0;
			w.h = 0;
			w.w = 0;
			w.h = 0;
			free(w.bins);
		}
		free(s.pWeaks);
		s.nWeaks = 0;
		s.alfa = 0;
	}
	free(pCascade->detectorCascade.pStrongs);
	pCascade->detectorCascade.type = AWP_NULL_DETECTOR;
	pCascade->detectorCascade.nStages = 0;
	pCascade->detectorCascade.width = 0;
	pCascade->detectorCascade.height  = 0;

	return AWP_OK;
}


/**
*@brief checking for an object in a given rectangle
* \awpImage - source integral image
* \pRect	- the area is interesting where the search for objects should be performed. NULL means that the search must be performed on the entire image
* \detector - object detector
* \res      - detection result. TRUE the object is present in the specified rectangle. FALSE the object is not in the given rectangle
*/
AWPRESULT awpDetectInRect(awpImage* pImage, AWPHANDLE detector, awpRect* pRect, AWPBOOL* res)
{
	*res = FALSE;
	awpCascade* pCascade = (awpCascade*)detector;
	if (pCascade == NULL)
		return AWP_BADARG;
	if (pCascade->detectFunc == NULL)
		return AWP_BADARG;
	if (pImage == NULL)
		return AWP_BADARG;
	if (awpRectInImage(pImage, pRect) != AWP_OK)
		return AWP_BADARG;
	*res = pCascade->detectFunc(pImage, NULL, pRect, &pCascade->detectorCascade);
	return AWP_OK;
}

/**
*@brief Ggeneral object detection function. This function does not use a second integral image
*		to estimate the variance of the input data
* \awpImage - source integral image
* \pRoi		- the area is interesting where the search for objects should be performed. NULL means that the search must be performed on the entire image
* \pCascade - cascade object detector
* \num      - number of objects found
* \ppResult - pointer to an array of rectangles found
*/
AWPRESULT awpObjectDetect(awpImage* pImage, awpRect* pRoi, AWPHANDLE detector, AWPINT* num, awpRect** ppResult)
{
	AWPDOUBLE st, grow,_height, _width, ar;
	AWPINT width, height, w,h,count, _num;
	AWPDOUBLE stepx, stepy;
	awpRect r;
	awpRect* _rects = NULL;
	awpRect fragment;
	awpCascade* pCascade = (awpCascade*)detector;

	if (pImage == NULL)
		return AWP_BADARG;
	if (pCascade == NULL)
		return  AWP_BADARG;

	 st = 10;
	 grow = AWP_DETECT_GROW;
	
	 width  = pImage->sSizeX;
	 height = pImage->sSizeY;
	 w =  pCascade->detectorCascade.width;
	 h =  pCascade->detectorCascade.height;
     _height = 0;
     _width = 0;
	 ar = 1;
	                                     
	*num = 0;
	*ppResult = NULL;

    if (w > h)
    {
        _width  = width-2;
        _height = _width*ar;
    }
    else
    {
       _height = height-2;
       _width = _height / ar;
    }

	 stepx = st*_width / 100;
	 stepy = st*_height / 100;

	if (pRoi != NULL)
		r = *pRoi;
	else 
	{
		r.left = 0;
		r.top  = 0;
		r.right = pImage->sSizeX;
		r.bottom = pImage->sSizeY;
	}

	stepx < 1 ? AWP_DETECT_MIN_STEP:stepx;
	stepy < 1 ? AWP_DETECT_MIN_STEP:stepy;
	count = 0;
	_num = 0;
	
	while (_width > pCascade->detectorCascade.width)
    {
		int y = r.top;
        int ey = y + _height;
        
		while (ey < r.bottom)
        {
			int x = r.left;
            int ex = x + _width;
            
			while(ex < r.right)
            {
			  fragment.left = x;
              fragment.top  = y;
              fragment.bottom = fragment.top +  _height;
              fragment.right  = fragment.left + _width;

				if (pCascade->detectFunc(pImage, NULL, &fragment, &pCascade->detectorCascade))
				{
					_num++;
					_rects = (awpRect*)realloc(_rects, _num*sizeof(awpRect));
					_rects[_num-1] = fragment;
				}
			  count++;
			  ex +=stepx;
              x += stepx;
			  
            }
            y += stepy;
            ey += stepy;
        }

        _width /= grow;
        _height /= grow;

		stepx = st*_width / 100;
		stepx = stepx < AWP_DETECT_MIN_STEP ? AWP_DETECT_MIN_STEP:stepx;
		stepy = st*_height / 100;
		stepy = stepy < AWP_DETECT_MIN_STEP ? AWP_DETECT_MIN_STEP:stepy;
    }
	*num = _num;
	*ppResult = _rects;
#ifdef _DEBUG
	printf("awpObjectDetect:  %d .\n", _num);
#endif 
	return AWP_OK;
}

AWPRESULT awpLoadDetector(const char* lpFileName, AWPHANDLE* detector)
{
	AWPRESULT res = AWP_OK;
	awpCascade* cascade = NULL;

	if (lpFileName == NULL || detector == NULL)
		_ERROR_EXIT_RES_(AWP_BADARG)

	_CHECK_RESULT_(res = awpCreateCascade(&cascade))
	_CHECK_RESULT_(res = awpLoadCascade(cascade, lpFileName))

	*detector = cascade;
    
CLEANUP:
	if (res != AWP_OK)
	{
		*detector = NULL;
		if (cascade != NULL)
			awpFreeCascade(&cascade);
	}
		*detector = NULL;
	return res; 
}

AWPRESULT awpFreeDetector(AWPHANDLE* detector)
{
	AWPRESULT res = AWP_OK;
	awpCascade* cascade = NULL;
	if (detector == NULL)
		_ERROR_EXIT_RES_(AWP_BADARG)
	cascade = (awpCascade*)(*detector);
	_CHECK_RESULT_(res = awpFreeCascade(&cascade))
CLEANUP:
	return res;
}
