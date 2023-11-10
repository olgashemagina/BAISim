//---------------------------------------------------------------------------
#pragma hdrstop

#include "_awpipl.h"
#include "stdlib.h"
//---------------------------------------------------------------------------
static  AWPBOOL _awpRectInRect(awpRect* r, awpRect* inr)
{
  return  inr->left >= r->left && inr->right <= r->right && inr->top >= r->top && inr->bottom <= r->bottom;
}

AWPRESULT _awpAllScanner(awpImage* pImage, awpRect* pRoi, awpScanner* pScanner, AWPINT* num, awpDetectItem** ppItems)
{
    int count, sy, sx, scale;
    awpRect  _rect;
    AWPSHORT _width;
    AWPSHORT _height;
    AWPFLOAT _alfa;
    if (pScanner->scannerType != AllScanner)
        return AWP_BADARG;

    *num = 0;
    *ppItems = NULL;
    _alfa = (AWPFLOAT)pScanner->baseWidth / (AWPFLOAT)pScanner->baseHeight;

    count = 0;
    scale = (int)pScanner->maxScale;

    while(scale >= pScanner->minScale)
    {
        _width  = pScanner->baseWidth >= pScanner->baseHeight ? scale : _alfa*scale;
        _height = pScanner->baseHeight > pScanner->baseWidth ? scale : scale / _alfa;
        sy = 0;
        while (sy + _height < pImage->sSizeY)
        {
            sx = 0;
            while (sx + _width < pImage->sSizeX)
            {

                _rect.left = sx;
                _rect.top  = sy;
                _rect.right = sx + _width;
                _rect.bottom = sy + _height;

                if (awpRectInImage(pImage, &_rect) == AWP_OK)
                {
                    if (pRoi != NULL)
                    {
                        if (_awpRectInRect(pRoi, &_rect))
                            count++;
                    }
                    else
                       count++;
                }

                sx += pScanner->xStep;
            }

            sy += pScanner->yStep;
        }

        scale -= pScanner->scaleStep;
    }

    if (count == 0)
        AWP_OK;

    *ppItems = (awpDetectItem*)malloc(count*sizeof(awpDetectItem));
    if (*ppItems == NULL)
        return AWP_BADMEMORY;
    *num = count;

    count = 0;
    scale = (int)pScanner->maxScale;

    while(scale >= pScanner->minScale)
    {
        _width  = pScanner->baseWidth >= pScanner->baseHeight ? scale : _alfa*scale;
        _height = pScanner->baseHeight > pScanner->baseWidth ? scale : scale / _alfa;
        sy = 0;
        while (sy + _height < pImage->sSizeY)
        {
            sx = 0;
            while (sx + _width < pImage->sSizeX)
            {

                _rect.left = sx;
                _rect.top  = sy;
                _rect.right = sx + _width;
                _rect.bottom = sy + _height;

                if (awpRectInImage(pImage, &_rect) == AWP_OK)
                {
                    if (pRoi != NULL)
                    {
                        if (_awpRectInRect(pRoi, &_rect))
                        {
                            (*ppItems)[count].rect = _rect;
                            (*ppItems)[count].hasObject = FALSE;
                            (*ppItems)[count].Height = -1;
                            (*ppItems)[count].Width = -1;
                            (*ppItems)[count].Distance = -1;
                            count++;
                        }
                    }
                    else
                    {
                      (*ppItems)[count].rect = _rect;
                      (*ppItems)[count].hasObject = FALSE;
                      (*ppItems)[count].Height = -1;
                      (*ppItems)[count].Width = -1;
                      (*ppItems)[count].Distance = -1;
                       count++;
                    }
                }

                sx += pScanner->xStep;
            }

            sy += pScanner->yStep;
        }

        scale -= pScanner->scaleStep;
    }


    return AWP_OK;
}
/**/
AWPRESULT _awpScaleScanner(awpImage* pImage, awpRect* pRoi, awpScanner* pScanner, AWPINT* num, awpDetectItem** ppItems)
{
    AWPINT  count, sy, sx, scale, stepx, stepy;
    AWPFLOAT xst, yst, grow;
    AWPFLOAT wi, hi, hmax, wmax, hmin, wmin;
    awpRect _rect;

    if (pScanner->scannerType != ScaleScanner)
        return AWP_BADARG;
    if (pScanner->scaleStep <= 1)
        return AWP_BADARG;
    if (pScanner->minScale < 1)
        return AWP_BADARG;

    xst = pScanner->xStep;
    yst = pScanner->yStep;
    grow = pScanner->scaleStep;

    wmax = pScanner->maxScale*pScanner->baseWidth;
    wmin = pScanner->minScale*pScanner->baseWidth;
    hmax = pScanner->maxScale*pScanner->baseHeight;
    hmin = pScanner->minScale*pScanner->baseHeight;

    wi = wmax;
    hi = hmax;

    stepx = xst*wi/100;
    stepy = yst*hi/100;

    stepx = stepx < pScanner->stepGranularity ? pScanner->stepGranularity:stepx;
    stepy = stepy < pScanner->stepGranularity ? pScanner->stepGranularity:stepy;
    count = 0;
    while (wi >= wmin && hi >= hmin)
    {
        sy = 0;
        while (sy + hi <= pImage->sSizeY)
        {

            sx = 0;
            while (sx + wi <= pImage->sSizeX)
            {
                _rect.left = sx;
                _rect.top  = sy;
                _rect.right = sx + wi;
                _rect.bottom = sy + hi;

                if (awpRectInImage(pImage, &_rect) == AWP_OK)
                {
                    if (pRoi != NULL)
                    {
                        if (_awpRectInRect(pRoi, &_rect))
                            count++;
                    }
                    else
                       count++;
                }

                sx += stepx;
            }
            sy += stepy;
        }

        wi /= grow;
        hi /= grow;

        stepx = xst*wi/100;
        stepy = yst*hi/100;

        stepx = stepx < pScanner->stepGranularity ? pScanner->stepGranularity:stepx;
        stepy = stepy < pScanner->stepGranularity ? pScanner->stepGranularity:stepy;
    }

    if (count == 0)
        AWP_OK;

    *ppItems = (awpDetectItem*)malloc(count*sizeof(awpDetectItem));
    if (*ppItems == NULL)
        return AWP_BADMEMORY;
    *num = count;

    wmax = pScanner->maxScale*pScanner->baseWidth;
    wmin = pScanner->minScale*pScanner->baseWidth;
    hmax = pScanner->maxScale*pScanner->baseHeight;
    hmin = pScanner->minScale*pScanner->baseHeight;

    wi = wmax;
    hi = hmax;

    stepx = xst*wi/100;
    stepy = yst*hi/100;

    stepx = stepx < pScanner->stepGranularity ? pScanner->stepGranularity:stepx;
    stepy = stepy < pScanner->stepGranularity ? pScanner->stepGranularity:stepy;
    count = 0;
    while (wi >= wmin && hi >= hmin)
    {
        sy = 0;
        while (sy + hi <= pImage->sSizeY)
        {

            sx = 0;
            while (sx + wi <= pImage->sSizeX)
            {
                _rect.left = sx;
                _rect.top  = sy;
                _rect.right = sx + wi;
                _rect.bottom = sy + hi;

                if (awpRectInImage(pImage, &_rect) == AWP_OK)
                {
                    if (pRoi != NULL)
                    {
                        if (_awpRectInRect(pRoi, &_rect))
                        {
                            (*ppItems)[count].rect = _rect;
                            (*ppItems)[count].hasObject = FALSE;
                            (*ppItems)[count].Height = -1;
                            (*ppItems)[count].Width = -1;
                            (*ppItems)[count].Distance = -1;
                            count++;
                        }
                    }
                    else
                    {
                      (*ppItems)[count].rect = _rect;
                      (*ppItems)[count].hasObject = FALSE;
                      (*ppItems)[count].Height = -1;
                      (*ppItems)[count].Width = -1;
                      (*ppItems)[count].Distance = -1;
                       count++;
                    }
                }

                sx += stepx;
            }
            sy += stepy;
        }

        wi /= grow;
        hi /= grow;

        stepx = xst*wi/100;
        stepy = yst*hi/100;

        stepx = stepx < pScanner->stepGranularity ? pScanner->stepGranularity:stepx;
        stepy = stepy < pScanner->stepGranularity ? pScanner->stepGranularity:stepy;
    }

    return AWP_OK;
}

AWPRESULT _awpCameraScanner(awpImage* pImage, awpRect* pRoi, awpScanner* pScanner, AWPINT* num, awpDetectItem** ppItems)
{
    awpCamera camera;

    awpPoint p1;
    awpPoint p2;
    awp3DPoint _3dPoint;
    awp2DPoint _2dPoint;

    AWPDOUBLE MaxWidth, stepX, len, minY, maxY, stepY, currY, minDist, maxDist;
    AWPDOUBLE stepDist, currDist, _radius;

    AWPINT count;

    awpRect _r;

    if (pScanner->scannerType != CameraScanner)
        return AWP_BADARG;
    if (pScanner->camera == NULL)
        return AWP_BADARG;

    camera   = *pScanner->camera;
    MaxWidth = pScanner->maxShift;
    stepX    = pScanner->stepShift;
    len      = MaxWidth / 2;
    minY     = pScanner->minHeight;
    maxY     = pScanner->maxHeight;
    stepY    = pScanner->stepHeight;
    currY    = minY;
    minDist  = pScanner->minDistance;
    maxDist  = pScanner->maxDistance;
    currDist = maxDist;
    stepDist = pScanner->stepDistance;
    _radius  = 0;
   count = 0;
  while (currDist >= minDist)
  {
      currY = minY;

      _3dPoint.X = 0;
      _3dPoint.Y = currY;
      _3dPoint.Z = currDist;
      awpSceneToImagePoint(&camera, pImage, &_3dPoint, &_2dPoint);
      p1.X = _2dPoint.X;
      p1.Y = _2dPoint.Y;
      _3dPoint.X = 0;
      _3dPoint.Y = currY + 120;
      _3dPoint.Z = currDist;
      awpSceneToImagePoint(&camera, pImage, &_3dPoint, &_2dPoint);
      p2.X = _2dPoint.X;
      p2.Y = _2dPoint.Y;
      _radius = p1.Y - p2.Y;
      if (_radius < 12)
      {
        currDist -=  stepDist;
        continue;
      }

      while (currY <= maxY)
      {
        len = -MaxWidth /2;
        while (len <= MaxWidth / 2 )
        {
            _3dPoint.X = len;
            _3dPoint.Y = currY;
            _3dPoint.Z = currDist;
            awpSceneToImagePoint(&camera, pImage, &_3dPoint, &_2dPoint);

            p1.X = _2dPoint.X;
            p1.Y = _2dPoint.Y;

            _r.left = p1.X - _radius;
            _r.right = p1.X + _radius;
            _r.top = p1.Y - _radius;
            _r.bottom = p1.Y+ _radius;

            if (awpRectInImage(pImage, &_r) == AWP_OK)
            {
                if (pRoi != NULL)
                {
                    if (_awpRectInRect(pRoi, &_r))
                        count++;
                }
                else
                   count++;
            }

            len += stepX;
        }
        currY +=  stepY;
      }
      currDist -=  stepDist;
  }

    if (count == 0)
        AWP_OK;

    *ppItems = (awpDetectItem*)malloc(count*sizeof(awpDetectItem));
    if (*ppItems == NULL)
        return AWP_BADMEMORY;
    *num = count;

    camera   = *pScanner->camera;
    MaxWidth = pScanner->maxShift;
    stepX    = pScanner->stepShift;
    len      = MaxWidth / 2;
    minY     = pScanner->minHeight;
    maxY     = pScanner->maxHeight;
    stepY    = pScanner->stepHeight;
    currY    = minY;
    minDist  = pScanner->minDistance;
    maxDist  = pScanner->maxDistance;
    currDist = minDist;
    _radius  = 0;
  count = 0;
  while (currDist <= maxDist)
  {
      currY = minY;

      _3dPoint.X = 0;
      _3dPoint.Y = currY;
      _3dPoint.Z = currDist;
      awpSceneToImagePoint(&camera, pImage, &_3dPoint, &_2dPoint);
      p1.X = _2dPoint.X;
      p1.Y = _2dPoint.Y;
      _3dPoint.X = 0;
      _3dPoint.Y = currY + 120;
      _3dPoint.Z = currDist;
      awpSceneToImagePoint(&camera, pImage, &_3dPoint, &_2dPoint);
      p2.X = _2dPoint.X;
      p2.Y = _2dPoint.Y;
      _radius = p1.Y - p2.Y;
      if (_radius < 12)
      {
        currDist -=  stepDist;
        continue;
      }

      while (currY <= maxY)
      {
        len = -MaxWidth /2;
        while (len <= MaxWidth / 2 )
        {
            _3dPoint.X = len;
            _3dPoint.Y = currY;
            _3dPoint.Z = currDist;
            awpSceneToImagePoint(&camera, pImage, &_3dPoint, &_2dPoint);

            p1.X = _2dPoint.X;
            p1.Y = _2dPoint.Y;

            _r.left = p1.X - _radius;
            _r.right = p1.X + _radius;
            _r.top = p1.Y - _radius;
            _r.bottom = p1.Y+ _radius;

            if (awpRectInImage(pImage, &_r) == AWP_OK)
            {
                if (pRoi != NULL)
                {
                    if (_awpRectInRect(pRoi, &_r))
                    {
                        (*ppItems)[count].rect = _r;
                        (*ppItems)[count].hasObject = FALSE;
                        (*ppItems)[count].Height = -1;
                        (*ppItems)[count].Width = -1;
                        count++;
                    }
                }
                else
                {
                    (*ppItems)[count].rect = _r;
                    (*ppItems)[count].hasObject = FALSE;
                    (*ppItems)[count].Height = -1;
                    (*ppItems)[count].Width = -1;
                    count++;
                }
            }

            len += stepX;
        }
        currY +=  stepY;
      }
      currDist +=  stepDist;
  }

  return AWP_OK;
}

AWPRESULT _awpCameraScanner1(awpImage* pImage, awpRect* pRoi, awpScanner* pScanner, AWPINT* num, awpDetectItem** ppItems)
{
    awpCamera camera;
    AWPINT count, ccount, i,h;
    AWPRESULT res;
    awpDetectItem* tmp;
    AWPDOUBLE v, vv, vvv;
    awpRect   r;

    if (pScanner->scannerType != CameraScanner)
        return AWP_BADARG;
    if (pScanner->camera == NULL)
        return AWP_BADARG;
    count = 0;
    tmp = NULL;
    pScanner->scannerType = ScaleScanner;
    res = _awpScaleScanner(pImage, pRoi, pScanner, &count,  &tmp);
    if (res != AWP_OK)
        return res;
    ccount = 0;
    for (i = 0; i < count; i++)
    {
        res = awpImageObjectHWidth(pScanner->camera, pImage, &tmp[i].rect, 1800, &v);
        if (res != AWP_OK)
            continue;
        if (v > 200 && v < 300)
        {
            awpPoint pp;
            pp.X = (tmp[i].rect.left + tmp[i].rect.right) / 2;
            pp.Y = tmp[i].rect.top;
            awpImagePointToShiftHX(pScanner->camera, pImage, &pp, 1800, &vv);
            if (vv > -1000 && vv < 1000)
                ccount++;
        }
    }

    *ppItems = (awpDetectItem*)malloc(count*sizeof(awpDetectItem));
    ccount = 0;
    for (i = 0; i < count; i++)
    {
        v = 0;
        res = awpImageObjectHWidth(pScanner->camera, pImage, &tmp[i].rect, 1800, &v);
        if (res != AWP_OK)
            continue;
        if (v > 130 && v < 400)
        {
            awpPoint pp;
            pp.X = (tmp[i].rect.left + tmp[i].rect.right) / 2;
            pp.Y = tmp[i].rect.top;


            awpImageYHToLength(pScanner->camera, pImage, tmp[i].rect.top, 1800, &vv);
            awpImageObjectHWidth(pScanner->camera, pImage, &tmp[i].rect, 1800, &v);
            awpImagePointToShiftHX(pScanner->camera, pImage, &pp, 1800, &vvv);

            if (vvv > -1000 && vvv < 1000)
            {
              (*ppItems)[ccount] = tmp[i];
              (*ppItems)[ccount].hasObject = FALSE;
              (*ppItems)[ccount].Distance  = vv;
              (*ppItems)[ccount].Width     = v;
              (*ppItems)[ccount].Height    = 1800;
              (*ppItems)[ccount].Shift = vvv;
              ccount++;
            }

        }
    }
    pScanner->scannerType = CameraScanner;
    *num = ccount;
    return AWP_OK;
}


AWPRESULT awpScanImage(awpImage* pImage, awpRect* pRoi, awpScanner* pScanner, AWPINT* num, awpDetectItem** ppItems)
{
    AWPRESULT res;
    if (pImage == NULL)
        return AWP_BADARG;
    if (pScanner == NULL)
        return AWP_BADARG;
    if (num == NULL)
        return AWP_BADARG;
    if (ppItems == NULL)
        return AWP_BADARG;

    if (awpCheckImage(pImage) != AWP_OK)
        return AWP_BADARG;
    if (pRoi != NULL && awpRectInImage(pImage, pRoi) != AWP_OK)
        return AWP_BADARG;

    res = AWP_OK;

    switch(pScanner->scannerType)
    {
        case AllScanner:
            res =  _awpAllScanner(pImage, pRoi, pScanner, num, ppItems);
        break;
        case ScaleScanner:
            res =  _awpScaleScanner(pImage, pRoi, pScanner, num, ppItems);
        break;
        case CameraScanner:
            res =  _awpCameraScanner1(pImage, pRoi, pScanner, num, ppItems);
        break;
    }

    return res;
}

AWPRESULT awpExteractFound(AWPINT num, awpDetectItem* pItems, AWPINT* numFound, awpDetectItem** ppFound)
{
    int i, count;
    if (num < 0 || pItems == NULL)
        return AWP_BADARG;
    if (numFound == NULL || ppFound == NULL)
        return AWP_BADARG;

    count = 0;
    for (i = 0; i < num; i++)
    {
        if (pItems[i].hasObject == TRUE)
            count++;
    }
    if (count == 0)
    {
        *numFound = 0;
        *ppFound = NULL;
        return AWP_OK;
    }

    *ppFound = NULL;
    *ppFound = (awpDetectItem*)malloc(count*sizeof(awpDetectItem));
    if (*ppFound == NULL)
        return AWP_BADMEMORY;
    count = 0;
    for (i = 0; i < num; i++)
    {
        if (pItems[i].hasObject == TRUE)
        {
            (*ppFound)[count] = pItems[i];
            pItems[i].hasObject = FALSE;
            count++;
        }
    }
    *numFound = count;
    return AWP_OK;
}

typedef struct
{
	awpDetectItem		r;
	AWPSHORT        	c;
}cawpDetectItem;

float _awpItemsOverlap(awpDetectItem r1, awpDetectItem r2)
{

 AWPFLOAT colInt, rowInt, intersection, area1,area2;
 AWPINT   width,  height,_width,_height;

 width = r1.rect.right - r1.rect.left;
 height = r1.rect.bottom - r1.rect.top;
 _width = r2.rect.right - r2.rect.left;
 _height = r2.rect.bottom - r2.rect.top;
 if (r2.rect.left > r1.rect.left + width)
    return 0;
 if (r2.rect.top > r1.rect.top + height)
    return 0;
 if (r2.rect.left + _width < r1.rect.left)
    return 0;
 if (r1.rect.top + _height < r1.rect.top)
    return 0;

 colInt = AWP_MIN(r2.rect.left + _width, r1.rect.left + width) - AWP_MAX(r2.rect.left, r1.rect.left);
 rowInt = AWP_MIN(r2.rect.top + _height, r1.rect.top + height) - AWP_MAX(r2.rect.top, r1.rect.top);

 intersection = colInt*rowInt;
 area1 = _width*_height;
 area2 = width*height;

 return intersection / (area1+area2-intersection);
}

static void _awpClusterItems(int numSrc, awpDetectItem* pSrc, int* numDst, awpDetectItem** ppDst)
{
	//
	AWPINT i,j, NumClusters, idx, eCount, cidx;
	AWPBOOL found;
	AWPFLOAT overlap;
	awpDetectItem r;

	cawpDetectItem* crects = (cawpDetectItem*)malloc(numSrc*sizeof(cawpDetectItem));
	for (i = 0; i < numSrc; i++)
	{
		crects[i].r = pSrc[i];
		crects[i].c = -1;
	}
	//
	NumClusters = 0;
	found = FALSE;
	idx = 0;
	do
	{
		found = FALSE;
		for (i = 0; i < numSrc; i++)
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
			for (i = 0; i < numSrc; i++)
			{
				if (i == idx)
					continue;
				if (crects[i].c != -1)
					continue;
				overlap = _awpItemsOverlap(crects[i].r , crects[idx].r);
				if (overlap > 0.4)
				{
					crects[i].c = NumClusters;
				}
			}
		}
	}while(found);

	//
	eCount = 0;
	cidx = 0;
	(*numDst) = NumClusters;
	*ppDst = (awpDetectItem*)malloc(NumClusters*sizeof(awpDetectItem));
	for (i = 0; i < NumClusters; i++)
	{
		cidx = i+1;
		r.rect.left = 0;
		r.rect.right = 0;
		r.rect.bottom = 0;
		r.rect.top = 0;
		eCount = 0;
		for (j = 0; j < numSrc; j++)
		{
			if (crects[j].c == cidx)
			{
				r.rect.left += crects[j].r.rect.left;
				r.rect.bottom += crects[j].r.rect.bottom;
				r.rect.right += crects[j].r.rect.right;
				r.rect.top += crects[j].r.rect.top;
				eCount++;
			}
		}
		r.rect.left /= eCount;
		r.rect.right /= eCount;
		r.rect.bottom /= eCount;
		r.rect.top /= eCount;
		(*ppDst)[i] = r;
	}


	//
	free(crects);
}

AWPRESULT awpClusterFound(AWPINT numFound, awpDetectItem* pFound, AWPINT* numClusters, awpDetectItem** ppClusters)
{
    if (numFound < 0 || pFound == NULL)
        return AWP_BADARG;
    if (numClusters == NULL || ppClusters == NULL)
        return AWP_BADARG;

    _awpClusterItems(numFound, pFound, numClusters, ppClusters);

    return AWP_OK;
}
