/*M
//
//      awpipl2 image processing and image analysis library
//		File: awpcontour.c
//		Purpose: contour related routines
//		Autors: Alex A.Telnykh (tlnykha@yahoo.com)
//      CopyRight 2004-2019 (c) NN-Videolab.net
//M*/
#ifdef HAVE_CONFIG_H
        #include <config.h>
#endif

#include "_awpipl.h"
/*------------------------------------------------------------------------------------------------*/
static AWPINT mymax(AWPINT a, AWPINT b, AWPBOOL* Flag){
	*Flag = TRUE;
	if (a > b) return a;
	*Flag = FALSE;
	return b;
}

static AWPINT mymin(AWPINT a, AWPINT b){
	if (a < b) return a;
	return b;
}
// the function of belonging to the point of the domain // is not used
static AWPBOOL IsPixelBelongArea(AWPINT NumPoints, awpPoint* P, AWPINT x, AWPINT y){
	AWPINT Count, i, j, m, x0;
	AWPBOOL F;
	AWPDOUBLE t;
	Count = 0;
	for (i = 0; i < NumPoints - 1; i++)    {
		j = (i + 1);
		if (P[i].Y == P[j].Y) continue;
		if (P[i].Y > y && P[j].Y > y) continue;
		if (P[i].Y < y && P[j].Y < y) continue;

		m = mymax(P[i].Y, P[j].Y, &F);
		if (F) x0 = P[i].X; else x0 = P[j].X;
		if (m == y && x0 > x)
			Count = 1 - Count;
		else if (mymin(P[i].Y, P[j].Y) == y) continue;
		else{
			t = ((AWPDOUBLE)y - P[i].Y) / (P[j].Y - P[i].Y);
			if (t > 0 && t <1 && P[i].X + t*(P[j].X - P[i].X) >= x) Count = 1 - Count;
		}
	}
	return (Count == 0) ? FALSE : TRUE;
}

/*------------------------------------------------------------------------------------------------*/
static AWPDOUBLE mymaxd(AWPDOUBLE a, AWPDOUBLE b, AWPBOOL* Flag){
	*Flag = TRUE;
	if (a > b) return a;
	*Flag = FALSE;
	return b;
}

static AWPDOUBLE mymind(AWPDOUBLE a, AWPDOUBLE b){
	if (a < b) return a;
	return b;
}

static AWPBOOL IsPixelBelongAread(AWPINT NumPoints, awp2DPoint* P, AWPDOUBLE x, AWPDOUBLE y){
	AWPINT Count, i, j;
	AWPBOOL F;
	AWPDOUBLE t, m, x0;
	Count = 0;
	for (i = 0; i < NumPoints - 1; i++)    {
		j = (i + 1);
		if (P[i].Y == P[j].Y) continue;
		if (P[i].Y > y && P[j].Y > y) continue;
		if (P[i].Y < y && P[j].Y < y) continue;

		m = mymaxd(P[i].Y, P[j].Y, &F);
		if (F) x0 = P[i].X; else x0 = P[j].X;
		if (m == y && x0 > x)
			Count = 1 - Count;
		else if (mymind(P[i].Y, P[j].Y) == y) continue;
		else{
			t = ((AWPDOUBLE)y - P[i].Y) / (P[j].Y - P[i].Y);
			if (t > 0 && t <1 && P[i].X + t*(P[j].X - P[i].X) >= x) Count = 1 - Count;
		}
	}
	return (Count == 0) ? FALSE : TRUE;
}

/*------------------------------------------------------------------------------------------------*/

AWPRESULT awpCreateContour(awpContour** ppContour, AWPINT NumPoints, AWPBOOL dir)
{
    AWPRESULT res = AWP_OK;
    if (NumPoints < 0)
        _ERROR_EXIT_RES_(AWP_BADARG);
    if (ppContour == NULL)
        _ERROR_EXIT_RES_(AWP_BADARG);

    *ppContour = NULL;
    *ppContour = (awpContour*)malloc(sizeof(awpContour));
    if (*ppContour == NULL)
        _ERROR_EXIT_RES_(AWP_BADMEMORY);
    (*ppContour)->Points = NULL;
	if (NumPoints > 0)
	{
		(*ppContour)->Points = (awpPoint*)malloc(NumPoints*sizeof(awpPoint));
		if ((*ppContour)->Points == NULL)
			_ERROR_EXIT_RES_(AWP_BADMEMORY);
		memset((*ppContour)->Points, 0, NumPoints*sizeof(awpPoint));

	}

	(*ppContour)->NumPoints = (AWPDWORD)NumPoints;
    (*ppContour)->Direction = dir;
CLEANUP:
    if (res != AWP_OK)
    {
        /*cleanup variables*/
        *ppContour = NULL;
        NumPoints  = 0;
    }
    return res;
}

AWPRESULT awpFreeContour(awpContour** ppContour)
{
    AWPRESULT res = AWP_OK;
    if (ppContour == NULL)
        _ERROR_EXIT_RES_(AWP_BADARG);

	if ((*ppContour)->Points != NULL)
	{
		free((*ppContour)->Points);
		free(*ppContour);
	}

    *ppContour = NULL;
CLEANUP:
    return res;
}


AWPRESULT awpGetContourRect(const awpContour* pContour, awpRect* rect)
{
    AWPRESULT res = AWP_OK;
    
    AWPWORD min_x = 0;
    AWPWORD max_x = 0;
    AWPWORD min_y = 0;
    AWPWORD max_y = 0;
    AWPDWORD i = 0;

    if (pContour == NULL)
        _ERROR_EXIT_RES_(AWP_BADARG);
    if (pContour->NumPoints <= 0)
        _ERROR_EXIT_RES_(AWP_BADARG);
    if (rect == NULL)
        _ERROR_EXIT_RES_(AWP_BADARG);
    min_x = pContour->Points[0].X;
    min_y = pContour->Points[0].Y;
    max_x = min_x;
    max_y = min_y;

    for (i = 1; i < pContour->NumPoints; i++)
    {
        if (min_x > pContour->Points[i].X)
            min_x = pContour->Points[i].X;
        if (max_x < pContour->Points[i].X)
            max_x = pContour->Points[i].X;
        if (min_y > pContour->Points[i].Y)
            min_y = pContour->Points[i].Y;
        if (max_y < pContour->Points[i].Y)
            max_y = pContour->Points[i].Y;
    }

    rect->left = min_x;
    rect->top  = min_y;
    rect->right = max_x;
    rect->bottom = max_y;
    
CLEANUP:
    return res;

}

AWPRESULT awpIsPointInContour(const awpContour* pContour, const awpPoint* p, AWPBOOL* result)
{
    AWPRESULT res = AWP_OK;
    if (pContour == NULL)
        _ERROR_EXIT_RES_(AWP_BADARG);
    if (pContour->NumPoints <= 0)
        _ERROR_EXIT_RES_(AWP_BADARG);
    if (p == NULL)
        _ERROR_EXIT_RES_(AWP_BADARG);
    if (result == NULL)
        _ERROR_EXIT_RES_(AWP_BADARG);
	/*important note.The start and end points of the path must match */
    *result = IsPixelBelongArea(pContour->NumPoints, pContour->Points, p->X, p->Y);
CLEANUP:
    return res;
}

AWPRESULT awpGetContPerim(const awpContour* c, AWPDOUBLE* Perim)
{
	AWPINT i, x, y;
	AWPDOUBLE p;
	AWPRESULT res;

	res = AWP_OK;
	p = 0.0;

	if (c == NULL)
	{
		res = AWP_BADARG;
		_ERR_EXIT_
	}

	for (i = 0; i < (AWPINT)c->NumPoints - 1; i++)
	{
		x = c->Points[i + 1].X - c->Points[i].X;
		y = c->Points[i + 1].Y - c->Points[i].Y;

		p += sqrt((AWPFLOAT)(x*x + y*y));
	}

	x = c->Points[c->NumPoints - 1].X - c->Points[0].X;
	y = c->Points[c->NumPoints - 1].Y - c->Points[0].Y;
	p += sqrt((AWPFLOAT)(x*x + y*y));
	*Perim = p;

CLEANUP:
	return res;
}

AWPRESULT awpContAddPoint(awpContour* c, awpPoint* point)
{
	AWPRESULT res;
	res = AWP_OK;

	if (c == NULL || point == NULL)
	{
		res = AWP_BADARG;
		_ERR_EXIT_
	}
	c->NumPoints++;
	c->Points = (awpPoint*)realloc(c->Points, c->NumPoints*sizeof(awpPoint));
	c->Points[c->NumPoints - 1] = *point;
CLEANUP:
	return res;
}

AWPRESULT awpContRemovePoint(awpContour* c, AWPDWORD idx)
{
	awpPoint* new_points;
	AWPRESULT res;
	AWPDWORD  new_count, i, k;
	res = AWP_OK;

	if (c == NULL || idx < 0 || idx >= c->NumPoints)
	{
		res = AWP_BADARG;
		_ERR_EXIT_
	}

	new_count = c->NumPoints - 1;
	if (new_count == 0)
	{
		c->NumPoints = 0;
		free(c->Points);
		c->Points = 0;
		return res;
	}

	new_points = (awpPoint*)malloc(new_count*sizeof(awpPoint));
	k = 0;
	for (i = 0; i < c->NumPoints; i++)
	{
		if (i == idx)
			continue;
		new_points[k] = c->Points[i];
		k++;
	}

	free(c->Points);
	c->Points = new_points;
	c->NumPoints--;

CLEANUP:
	return res;
}


AWPRESULT awpCreate2DContour(awp2DContour** ppContour, AWPINT NumPoints, AWPBOOL dir)
{
	AWPRESULT res = AWP_OK;
	if (NumPoints < 0)
		_ERROR_EXIT_RES_(AWP_BADARG);
	if (ppContour == NULL)
		_ERROR_EXIT_RES_(AWP_BADARG);

	*ppContour = NULL;
	*ppContour = (awp2DContour*)malloc(sizeof(awp2DContour));
	if (*ppContour == NULL)
		_ERROR_EXIT_RES_(AWP_BADMEMORY);
	(*ppContour)->Points = NULL;
	if (NumPoints > 0)
	{
		(*ppContour)->Points = (awp2DPoint*)malloc(NumPoints*sizeof(awp2DPoint));
		if ((*ppContour)->Points == NULL)
			_ERROR_EXIT_RES_(AWP_BADMEMORY);
		memset((*ppContour)->Points, 0, NumPoints*sizeof(awp2DPoint));

	}

	(*ppContour)->NumPoints = (AWPDWORD)NumPoints;
	(*ppContour)->Direction = dir;
CLEANUP:
	if (res != AWP_OK)
	{
		/*cleanup variables*/
		*ppContour = NULL;
		NumPoints = 0;
	}
	return res;

}

AWPRESULT awpFree2DContour(awp2DContour** ppContour)
{
	AWPRESULT res = AWP_OK;
	if (ppContour == NULL)
		_ERROR_EXIT_RES_(AWP_BADARG);

	if ((*ppContour)->Points != NULL)
	{
		free((*ppContour)->Points);
	}

    free(*ppContour);

	*ppContour = NULL;
CLEANUP:
	return res;
}

AWPRESULT awpGet2DContourRect(const awp2DContour* pContour, awpQuadrangle* rect)
{
	AWPRESULT res = AWP_OK;
	AWPDOUBLE min_x = 0;
	AWPDOUBLE max_x = 0;
	AWPDOUBLE min_y = 0;
	AWPDOUBLE max_y = 0;
	AWPDWORD i = 0;
	/* check arguments*/
	if (pContour == NULL)
		_ERROR_EXIT_RES_(AWP_BADARG);
	if (pContour->NumPoints <= 0)
		_ERROR_EXIT_RES_(AWP_BADARG);
	if (rect == NULL)
		_ERROR_EXIT_RES_(AWP_BADARG);
	min_x = pContour->Points[0].X;
	min_y = pContour->Points[0].Y;
	max_x = min_x;
	max_y = min_y;
	/* search extemums*/
	for (i = 1; i < pContour->NumPoints; i++)
	{
		if (min_x > pContour->Points[i].X)
			min_x = pContour->Points[i].X;
		if (max_x < pContour->Points[i].X)
			max_x = pContour->Points[i].X;
		if (min_y > pContour->Points[i].Y)
			min_y = pContour->Points[i].Y;
		if (max_y < pContour->Points[i].Y)
			max_y = pContour->Points[i].Y;
	}
	/*save result*/
	rect->left_top.X = min_x;
	rect->left_top.Y = min_y;
	rect->left_bottom.X = min_x;
	rect->right_top.X = max_x;
	rect->right_top.Y = min_y;
	rect->right_bottom.X = max_x;
	rect->right_bottom.Y = max_y;
	rect->size = sizeof(awpQuadrangle);

CLEANUP:
	return res;

}

AWPRESULT awpIsPointIn2DContour(const awp2DContour* pContour, const awp2DPoint* p, AWPBOOL* result)
{
	AWPRESULT res = AWP_OK;
	if (pContour == NULL)
		_ERROR_EXIT_RES_(AWP_BADARG);
	if (pContour->NumPoints <= 0)
		_ERROR_EXIT_RES_(AWP_BADARG);
	if (p == NULL)
		_ERROR_EXIT_RES_(AWP_BADARG);
	if (result == NULL)
		_ERROR_EXIT_RES_(AWP_BADARG);
	/*important note.The start and end points of the path must match */
	*result = IsPixelBelongAread(pContour->NumPoints, pContour->Points, p->X, p->Y);
CLEANUP:
	return res;
}

AWPRESULT awpGet2DContPerim(const awp2DContour* c, AWPDOUBLE* Perim)
{
	AWPINT i;
	AWPDOUBLE p, x, y;
	AWPRESULT res;

	res = AWP_OK;
	p = 0.0;

	if (c == NULL)
	{
		res = AWP_BADARG;
		_ERR_EXIT_
	}

	for (i = 0; i < (AWPINT)c->NumPoints - 1; i++)
	{
		x = c->Points[i + 1].X - c->Points[i].X;
		y = c->Points[i + 1].Y - c->Points[i].Y;

		p += sqrt((AWPFLOAT)(x*x + y*y));
	}

	x = c->Points[c->NumPoints - 1].X - c->Points[0].X;
	y = c->Points[c->NumPoints - 1].Y - c->Points[0].Y;
	p += sqrt((AWPFLOAT)(x*x + y*y));
	*Perim = p;

CLEANUP:
	return res;
}

AWPRESULT awp2DContAddPoint(awp2DContour* c, awp2DPoint* point)
{
	AWPRESULT res;
	res = AWP_OK;

	if (c == NULL || point == NULL)
	{
		res = AWP_BADARG;
		_ERR_EXIT_
	}
	c->NumPoints++;
	c->Points = (awp2DPoint*)realloc(c->Points, c->NumPoints*sizeof(awp2DPoint));
	c->Points[c->NumPoints - 1] = *point;
CLEANUP:
	return res;
}

AWPRESULT awp2DContRemovePoint(awp2DContour* c, AWPDWORD idx)
{
	awp2DPoint* new_points;
	AWPRESULT res;
	AWPDWORD  new_count, i, k;
	res = AWP_OK;

	if (c == NULL || idx < 0 || idx >= c->NumPoints)
	{
		res = AWP_BADARG;
		_ERR_EXIT_
	}

	new_count = c->NumPoints - 1;
	if (new_count == 0)
	{
		c->NumPoints = 0;
		free(c->Points);
		c->Points = 0;
		return res;
	}

	new_points = (awp2DPoint*)malloc(new_count*sizeof(awp2DPoint));
	k = 0;
	for (i = 0; i < c->NumPoints; i++)
	{
		if (i == idx)
			continue;
		new_points[k] = c->Points[i];
		k++;
	}

	free(c->Points);
	c->Points = new_points;
	c->NumPoints--;

CLEANUP:
	return res;
}
