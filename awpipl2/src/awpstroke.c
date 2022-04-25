/*
    AWP Image library v2
    
    $Source: /home/cvs/awp/imageproc/awpipl-v2/src/awpstroke.c,v $
    $Revision: 1.24 $
    $Date: 2005/04/02 15:48:48 $
		
    Authors: Eugene Eremin <medlab@appl.sci-nnov.ru>
    Alexander Telnykh <telnykha@yahoo.com>
    Alex Chaykin <gnull@awp.nnov.ru>

	CopyRight 2004-2019 (c) NN-Videolab.net


*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>

#include "_awpipl.h"
#include <assert.h>

#define AWP_EPS      0.00000000001

/*attrebutes  "jump"*/
typedef enum  ownEAttrDef{
    sD,						/**/
    sRL,					/**/
    sLR,					/**/
    uLL,					/**/
    dRR,					/**/
    nRL,					/**/
    nLR						/**/
} ownEAttr;

/*strokes with  attributes*/
typedef struct ownStrokeAttrDef
{
    AWPINT y;					/**/
    AWPINT xl;					/**/
    AWPINT xr;					/**/
    ownEAttr l_attr;		/**/
    ownEAttr r_attr;		/**/
    AWPINT l_index;			/**/
    AWPINT r_index;			/**/
}ownStrokeAttr;
/*Near stroke object with attr*/
typedef struct ownNearStDef
{
    AWPBOOL IsFound;           /*Near object found*/
    AWPBOOL IsLeft;            /*Near Object "left"*/
    AWPINT  x;                 /**/
    AWPINT  x1;                /**/
    AWPINT  index;             /**/
    AWPINT  index1;            /**/
}ownNearSt;

/*transform image in  massive strokes*/
static AWPRESULT ownGetStrokeArr(awpImage* src, AWPINT* Num, awpStroke** sa,
                                 AWPBYTE thr, awpRect* pRoi)
{
    AWPINT NumStr, i,j;
    AWPINT w,h,n, x, y,ww;
    AWPBOOL f;
    AWPBYTE* p;
    awpStroke*     s;
    awpPoint Pt;

    /*find number strokes */
	if (pRoi == NULL)
	{
		w = src->sSizeX;
		h = src->sSizeY;
		x = 0;
		y = 0;
	}
	else
	{
		x = pRoi->left;
		y = pRoi->top;
		w = pRoi->right -x;
		h = pRoi->bottom - y;
	}
	ww = src->sSizeX;
    NumStr = w*h / 4;
    s = (awpStroke*)malloc(NumStr*sizeof(awpStroke));
    if (s == NULL) return AWP_NOTENOUGH_MEM;

    p = (AWPBYTE*)src->pPixels;
    n = 0;

    for (i = y; i < h + y; i++)
    {
        Pt.X = x;
        Pt.Y = i;

        if (p[i*ww +x] >= thr)
        {
            s[n].xl = x;
            f = TRUE;
        }
        else
            f = FALSE;

        for (j = x; j < w + x - 1; j++)
        {
            Pt.X = j;

            if (f)
            {
                if (FALSE || (p[i*ww + j] >= thr && p[i*ww + j + 1] < thr))
                {
                    /*end stkores */
                    s[n].y  = i;
                    s[n].xr = j;
                    f = FALSE;
                    n++;
                }
            }
            else
            {
               // if (!TRUE) continue;


                if(p[i*ww + j] < thr && p[i*ww + j +1] >= thr)
                {
                    /*begin strokes */
                    s[n].xl = j+1;
                    f = TRUE;
                }


            }/*f*/

        }

        Pt.X = w-1;

        if (f && p[i*ww + ww - 1] >= thr)
        {
            s[n].y  = i;
            s[n].xr = ww-1;
            f = FALSE;
            n++;
        }
    }
	if (n == 0)
	{
		free(s);
		s = 0;
	}
    *Num = n;
    *sa  = s;
    return AWP_OK;
}

AWPRESULT awpCopyStrokeObj(awpStrokeObj* pSrc, awpStrokeObj** ppDst)
{
    if (pSrc == NULL || pSrc->Num == 0)
        return AWP_BADARG;

    *ppDst = (awpStrokeObj*)malloc(sizeof(awpStrokeObj));
    (*ppDst)->strokes = (awpStroke*)malloc(pSrc->Num*sizeof(awpStroke));
    (*ppDst)->Num = pSrc->Num;
    memcpy((*ppDst)->strokes,pSrc->strokes,pSrc->Num*sizeof(awpStroke));
    
    return AWP_OK;
}

void ownExchangeStr(awpStroke* sa, AWPINT i, AWPINT j)
{
    awpStroke s;

    s.y  = sa[i].y;
    s.xl = sa[i].xl;
    s.xr = sa[i].xr;

    sa[i].y  = sa[j].y;
    sa[i].xl = sa[j].xl;
    sa[i].xr = sa[j].xr;

    sa[j].y  = s.y;
    sa[j].xl = s.xl;
    sa[j].xr = s.xr;
}

/*select stkoke objects in massive stkokes*/
static AWPRESULT ownFindStrokeObject(awpStroke* sa, AWPINT st, AWPINT* fn, AWPINT Num)
{
    AWPINT    i,j;
    AWPBOOL   FL;
    AWPINT    s1, f1;/*local start and finish*/
    AWPRESULT				res;
    
    res=AWP_OK;

    *fn = st;
    (*fn)++;
    f1 = *fn - 1;
    s1 = f1;

    do
    {
        FL = FALSE;
        for (i = s1; i <= f1; i++)
        {
            for(j = *fn; j < Num; j++)
            {
                if (abs(sa[j].y - sa[i].y) == 1)
                    if (sa[j].xl <= sa[i].xr && sa[j].xr >= sa[i].xl)
                    {
                        ownExchangeStr(sa, *fn, j);
                        (*fn)++;
                        FL = TRUE;
                    }
            }
        }
        s1 = f1;
        f1 = *fn - 1;
    }while(FL);

    return res;
}


/*transform image in massive stroke objects*/
AWPRESULT awpGetStrokes(const awpImage*		src,/*source image*/
                        AWPINT*			Num,/*number find objects*/
                        awpStrokeObj**  strokes,/*pointer on massive objects*/
                        AWPBYTE   thr,/*threshold*/
						awpRect*   pRoi
                       )
{

    awpStroke*				os;
    awpStrokeObj*			tmp;
    AWPINT*					so;
    AWPINT				        NumSt;
    AWPRESULT				res;
    AWPINT						s,f;
    AWPINT						i,j,k;
    AWPINT						n;

    awpImage* wImage = NULL;
    *strokes= NULL;
    *Num= 0;
    so = NULL;
    os = NULL;
    /*check input image*/
    _CHECK_RESULT_((res = awpCheckImage(src)))
    /*work with copy image*/
    _CHECK_RESULT_((res = awpCopyImage(src, &wImage)))

    /*check image on bag argument */
    if(wImage->dwType!=AWP_BYTE){
        res = AWP_NOTSUPPORT;
        _ERR_EXIT_
    }

    if(!(wImage->bChannels!=1 || wImage->bChannels!=3)){
        res = AWP_NOTSUPPORT;
        _ERR_EXIT_
    }

    /* if picture is color convert in grayscale */
    if (wImage->bChannels == 3)
    {
        _CHECK_RESULT_((res = awpConvert(wImage, AWP_CONVERT_3TO1_BYTE)))
    }
    /*transform image in  massive  strokes*/
    NumSt = 0;

    _CHECK_RESULT_(( res = ownGetStrokeArr(wImage, &NumSt, &os, thr, pRoi)))
    /********************************************************************************/
    if (res != AWP_OK)
    {
        awpReleaseImage(&wImage);
        return res;
    }
    if (NumSt == 0)
    {

        awpReleaseImage(&wImage);
        return AWP_STROKES_NOTFOUND;
    }

    so = (AWPINT*)malloc(NumSt*sizeof(AWPINT));
    if (so == NULL)
    {
        free((void*)os);
        awpReleaseImage(&wImage);
        return AWP_NOTENOUGH_MEM;
    }

    s = 0;f = 0;n = 0;
    so[n] = 0;
    do
    {
        ownFindStrokeObject(os, s, &f, NumSt);
        so[n] = f-s;
        s = f;
        n++;
    }while(f != NumSt);

    /*n--; */
    tmp = NULL;
    tmp = (awpStrokeObj*)malloc(n*sizeof(awpStrokeObj));
    if (tmp == NULL)
    {
        free((void*)so);
        free((void*)os);
        return AWP_NOTENOUGH_MEM;
    }

    k = 0;
    for(i = 0; i < n; i++)
    {
        tmp[i].Num = so[i];
        tmp[i].strokes = NULL;
        tmp[i].strokes = (awpStroke*)malloc(tmp[i].Num*sizeof(awpStroke));
        if (tmp->strokes == NULL)
        {
            /*delete already creating objects*/
            for (j = 0; j < i; j++)
                free(tmp[i].strokes);
            free((void*)tmp);
            free((void*)so);
            free((void*)os);
            return AWP_NOTENOUGH_MEM;
        }
        memcpy(tmp[i].strokes, &os[k], tmp[i].Num*sizeof(awpStroke));
        k += so[i];
    }

    *Num = n;
    *strokes = tmp;

CLEANUP:
    if(so!=NULL) free((void*)so);
    if(os!=NULL) free((void*)os);
    awpReleaseImage(&wImage);
    return res;
}

AWPRESULT awpFreeStrokes(AWPINT Num, awpStrokeObj** s)
{
    AWPINT j;
    awpStrokeObj* s1;
    
    if(Num<1) return AWP_OK;
    
    if(*s==NULL) return AWP_OK;
    
    s1 = *s;
    for (j =0; j < Num; j++)
        free(s1[j].strokes);
    free(*s);
    *s = NULL;

    return AWP_OK;
}


AWPRESULT awpCalcObjRect(const awpStrokeObj* s, awpRect* r)
{

    AWPINT i;
    if (s == NULL)  return AWP_BADMEMORY;
    /*if count point in stroke-objects eq 1=> exit*/
    if (s->Num == 1)
    {
        r->top    = s->strokes[0].y;
        r->bottom = s->strokes[0].y;
        r->left   = s->strokes[0].xl;
        r->right  = s->strokes[0].xr;
        return AWP_OK;
    }

    /*set begin velue coordinate  rect*/
    r->top    = s->strokes[0].y;
    r->bottom = s->strokes[0].y;
    r->left   = s->strokes[0].xl;
    r->right  = s->strokes[0].xr;

    /*execte cycle on all strokes for looking for
    around rect*/
    for (i = 0; i < (AWPINT)s->Num; i++)
    {
        if (s->strokes[i].y  <= r->top) r->top = s->strokes[i].y;
        if (s->strokes[i].y  >= r->bottom) r->bottom = s->strokes[i].y;
        if (s->strokes[i].xl <= r->left) r->left = s->strokes[i].xl;
        if (s->strokes[i].xr >= r->right) r->right = s->strokes[i].xr;
    }
    return AWP_OK;
}

AWPRESULT awpGetObjIntensity(const awpImage* pImg, const awpStrokeObj* pObj, AWPDOUBLE* I)
{
    AWPINT i,j, x, y,w;
    AWPINT q=0; /* count point */
    AWPRESULT res;
    awpImage* pcopy = NULL;
    AWPBYTE* p;

    pcopy=NULL;
    p=NULL;
    res=AWP_OK;

    /*check image on bag argument */
    if(pImg->dwType!=AWP_BYTE){
        res = AWP_NOTSUPPORT;
        _ERR_EXIT_
    }

    if(pImg->bChannels!=1){
        res = AWP_NOTSUPPORT;
        _ERR_EXIT_
    }

    pcopy = (awpImage*)pImg;

    p = (AWPBYTE*)pcopy->pPixels;
    w = pcopy->sSizeX;

    *I = 0.0;

    for (i = 0; i < (AWPINT)pObj->Num; i++)
    {
        for (j = pObj->strokes[i].xl; j < pObj->strokes[i].xr; j++)
        {
            q++;
            x = j;
            y = pObj->strokes[i].y;
            if (x > pcopy->sSizeX || y > pcopy->sSizeY){
                res=AWP_BADARG;
                _ERR_EXIT_
            }
            *I += p[y*w + x];
        }
    }
    if(q) *I=*I/(AWPDOUBLE)q;

CLEANUP:
    return res;
}

AWPRESULT awpGetObjTotalIntensity(const awpImage* pImg, const awpStrokeObj* pObj, AWPDOUBLE* I)
{
    AWPINT i,j, x, y,w;
    AWPRESULT res;
    awpImage* pcopy = NULL;
    AWPBYTE* p;

    pcopy=NULL;
    p=NULL;
    res=AWP_OK;

    /*check image on bag argument */
    if(pImg->dwType!=AWP_BYTE){
        res = AWP_NOTSUPPORT;
        _ERR_EXIT_
    }

    if(pImg->bChannels!=1){
        res = AWP_NOTSUPPORT;
        _ERR_EXIT_
    }

    pcopy = (awpImage*)pImg;

    p = (AWPBYTE*)pcopy->pPixels;
    w = pcopy->sSizeX;

    *I = 0.0;

    for (i = 0; i < (AWPINT)pObj->Num; i++)
    {
        for (j = pObj->strokes[i].xl; j < pObj->strokes[i].xr; j++)
        {
            x = j;
            y = pObj->strokes[i].y;
            if (x > pcopy->sSizeX || y > pcopy->sSizeY){
                res=AWP_BADARG;
                _ERR_EXIT_
            }
            *I += p[y*w + x];
        }
    }
CLEANUP:
    return res;
}


AWPRESULT awpStrObjSquare(const awpStrokeObj* s, AWPINT* Sq)
{
    AWPINT i;
    AWPRESULT res;

    res =AWP_OK;

    if (s == NULL){
        res =AWP_BADMEMORY;
        _ERR_EXIT_
    }

    *Sq = 0;

    for (i =0; i < (AWPINT)s->Num; i++)
    {
        *Sq += (s->strokes[i].xr - s->strokes[i].xl);
    }

CLEANUP:
    return res;
}

AWPINT AWP_CDECL compare_str(const void* elem1, const void* elem2)
{
    awpStroke* s1;
    awpStroke* s2;

    s1 = (awpStroke*)elem1;
    s2 = (awpStroke*)elem2;

    if (s1->y == s2->y && s1->xl > s2->xl)
        return 1;
    else if (s1->y > s2->y)
        return 1;
    else
        return -1;
}

static void ownFindNearXrY0(awpStrokeObj* s0, AWPINT index, ownNearSt*  s_near)
{
    s_near->IsFound = FALSE;
    s_near->x = -1;
    s_near->index = -1;

	if (index+1 < (AWPINT)s0->Num && s0->strokes[index].y == s0->strokes[index+1].y)
    {
        s_near->IsFound = TRUE;
        s_near->x = s0->strokes[index + 1].xl;
        s_near->index = index + 1;
    }

}

static void ownFindNearXrY1(awpStrokeObj* s0, AWPINT index, ownNearSt*  s_near)
{
    AWPDWORD i;
    AWPINT   Min, Min1;
    s_near->IsFound = FALSE;
    s_near->x = -1;
    s_near->index = -1;
    s_near->x1=-1;
    s_near->index=-1;
    Min = 100000;
    Min1 = 100000;

    i = index + 1;
    while (i < s0->Num && s0->strokes[i].y <= s0->strokes[index].y + 1)
    {

        if ((s0->strokes[i].y == s0->strokes[index].y + 1 ) &&
                (s0->strokes[i].xr >= s0->strokes[index].xl) &&
                (s0->strokes[i].xl <= s0->strokes[index].xr) &&
                abs(s0->strokes[index].xr - s0->strokes[i].xr) < Min)
        {

            s_near->IsFound = TRUE;
            s_near->x = s0->strokes[i].xr;
            s_near->index = i;
            Min = abs(s0->strokes[index].xr - s0->strokes[i].xr);
        }

        if ((s0->strokes[i].y == s0->strokes[index].y + 1 ) &&
                (s0->strokes[i].xr >= s0->strokes[index].xl) &&
                (s0->strokes[i].xl <= s0->strokes[index].xr) &&
                abs(s0->strokes[index].xr - s0->strokes[i].xl) < Min1)
        {
            s_near->IsFound = TRUE;
            s_near->x1 = s0->strokes[i].xl;
            s_near->index1 = i;
            Min1 = abs(s0->strokes[index].xr - s0->strokes[i].xl);
        }
        i++;
    }
}

static void ownFindNearXlY0(awpStrokeObj* s0, AWPINT index, ownNearSt*  s_near)
{

    s_near->IsFound = FALSE;
    s_near->x = -1;
    s_near->index = -1;

    if (index == 0) return;
    if (s0->strokes[index].y == s0->strokes[index-1].y)
    {
        s_near->IsFound = TRUE;
        s_near->x = s0->strokes[index - 1].xr;
        s_near->index = index - 1;
    }

}

static void ownFindNearXlY1(awpStrokeObj* s0, AWPINT index, ownNearSt*  s_near)
{
    AWPINT i;
    AWPINT	  Min, Min1;
    s_near->IsFound = FALSE;

    s_near->x = -1;		
    s_near->index = -1;	
    s_near->x1=-1;		
    s_near->index1=-1;
    Min = 100000; 	
    Min1 = 100000; 	

    if (index == 0) return;
    i= index - 1;
    while (i >= 0 && s0->strokes[i].y >= s0->strokes[index].y-1 )
    {
        if ((s0->strokes[i].y  == s0->strokes[index].y-1) &&
                (s0->strokes[i].xr >= s0->strokes[index].xl) &&
                (s0->strokes[i].xl <= s0->strokes[index].xr) &&
                abs(s0->strokes[index].xl - s0->strokes[i].xl) < Min)
        {
            s_near->IsFound = TRUE;
            Min  = abs(s0->strokes[index].xl - s0->strokes[i].xl);
            s_near->x  = s0->strokes[i].xl;
            s_near->index = i;
        }

        if ((s0->strokes[i].y  == s0->strokes[index].y-1) &&
                (s0->strokes[i].xr >= s0->strokes[index].xl) &&
                (s0->strokes[i].xl <= s0->strokes[index].xr) &&
                abs(s0->strokes[index].xl - s0->strokes[i].xr) < Min1)
        {
            s_near->IsFound = TRUE;
            Min1  = abs(s0->strokes[index].xl - s0->strokes[i].xr);
            s_near->x1  = s0->strokes[i].xr;
            s_near->index1 = i;
        }
        i--;
    }
}


//#warning check memory allocate with malloc and russin text
AWPRESULT awpGetObjCountour(const awpStrokeObj* s, awpContour* c)
{
    AWPINT i,k,j,x,y;
    AWPBOOL UP;//,F;
    ownNearSt nXrY0;
    ownNearSt nXrY1;
    ownNearSt nXlY0;
    ownNearSt nXlY1;
    AWPRESULT res;
    ownStrokeAttr* s_attr;
    ownStrokeAttr  Next;

    res=AWP_OK;
    s_attr=NULL;


    if (s == NULL || c == NULL ){
        res = AWP_BADARG;
        _ERR_EXIT_
    }

    qsort((void*)s->strokes, s->Num, sizeof(awpStroke), compare_str);

    s_attr = (ownStrokeAttr*)malloc(s->Num*sizeof(ownStrokeAttr));
    for (i = 0; i < (AWPINT)s->Num; i++)
    {
        s_attr[i].y = s->strokes[i].y;
        ownFindNearXrY0((awpStrokeObj*)s, i, &nXrY0);
        ownFindNearXrY1((awpStrokeObj*)s, i, &nXrY1);
        s_attr[i].xr = s->strokes[i].xr;
        if (nXrY0.IsFound == FALSE && nXrY1.IsFound == FALSE)
        {
            s_attr[i].r_attr  = sRL;
            s_attr[i].r_index = i;
        }
        else if (nXrY0.IsFound  && nXrY1.IsFound == FALSE)
        {
            s_attr[i].r_attr  = sRL;
            s_attr[i].r_index = i;
        }
        else if (nXrY0.IsFound == FALSE && nXrY1.IsFound )
        {
            if (nXrY1.index == nXrY1.index1)
            {
                s_attr[i].r_attr  = dRR;
                s_attr[i].r_index = nXrY1.index;
            }
            else
            {
                s_attr[i].r_attr  = dRR;
                s_attr[i].r_index = nXrY1.index1;
            }
        }
        else
        {
            if (nXrY1.index == nXrY1.index1)
            {
                if (nXrY1.x < nXrY0.x)
                {
                    s_attr[i].r_attr  = dRR;
                    s_attr[i].r_index = nXrY1.index;
                }
                else
                {
                    s_attr[i].r_attr  = nRL;
                    s_attr[i].r_index = nXrY0.index;
                }
            }
            else
            {
                if (s->strokes[nXrY1.index1].xr <= nXrY0.x)
                {
                    s_attr[i].r_attr  = dRR;
                    s_attr[i].r_index = nXrY1.index1;
                }
                else
                {
                    s_attr[i].r_attr  = nRL;
                    s_attr[i].r_index = nXrY0.index;
                }
            }
        }
        ownFindNearXlY0((awpStrokeObj*)s, i, &nXlY0);
        ownFindNearXlY1((awpStrokeObj*)s, i, &nXlY1);
        s_attr[i].xl = s->strokes[i].xl;
        if (nXlY0.IsFound == FALSE && nXlY1.IsFound == FALSE)
        {
            s_attr[i].l_attr  = sLR;
            s_attr[i].l_index = i;
        }
        else if (nXlY0.IsFound  && nXlY1.IsFound == FALSE)
        {
            s_attr[i].l_attr  = sLR;
            s_attr[i].l_index = i;
        }
        else if (nXlY0.IsFound == FALSE && nXlY1.IsFound )
        {
            if (nXlY1.index == nXlY1.index1)
            {
                s_attr[i].l_attr  = uLL;
                s_attr[i].l_index = nXlY1.index;
            }
            else
            {
                s_attr[i].l_attr  = uLL;
                s_attr[i].l_index = nXlY1.index1;
            }
        }
        else
        {	
            if (nXlY1.index == nXlY1.index1)
            {
                if (nXlY1.x <= nXlY0.x)
                {
                    s_attr[i].l_attr  = nLR;
                    s_attr[i].l_index = nXlY0.index;
                }
                else
                {
                    s_attr[i].l_attr  = uLL;
                    s_attr[i].l_index = nXlY1.index;
                }
            }
            else
            {
                if (s->strokes[nXlY1.index1].xl <= nXlY0.x)
                {
                    s_attr[i].l_attr  = nLR;
                    s_attr[i].l_index = nXlY0.index;
                }
                else
                {
                    s_attr[i].l_attr  = uLL;
                    s_attr[i].l_index = nXlY1.index1;
                }
            }
        }

    }

    c->Points = (awpPoint*)malloc(2*s->Num*sizeof(awpPoint));
    k = 0; UP = TRUE; i= 0; //F = TRUE;
    Next = s_attr[0];
    x = Next.xl;
    y = Next.y;
    do
    {
        if (UP)
        {
            j =i;

            c->Points[k].X =x;
            c->Points[k].Y =y;


            i = s_attr[i].l_index;
            Next = s_attr[i];


            if (s_attr[j].l_attr == sLR ||
                    s_attr[j].l_attr == nLR)
            {
                UP = FALSE;
                x = Next.xr;
            }
            else x = Next.xl;

            y = Next.y;

        }
        else
        {
            j = i;

            c->Points[k].X =x;
            c->Points[k].Y =y;


            i = s_attr[i].r_index;
            Next = s_attr[i];


            if (s_attr[j].r_attr == sRL ||
                    s_attr[j].r_attr == nRL)
            {
                UP = TRUE;
                x = Next.xl;
            }
            else x = Next.xr;
            y = Next.y;
        }
        k++;
        if (k == (AWPINT)(2*s->Num)) break;
    }while(c->Points[0].X != x || c->Points[0].Y != y || !UP);

    c->NumPoints = k;
CLEANUP:
    if(s_attr!=NULL) free(s_attr);
    return res;
}

AWPRESULT awpGetObjCentroid(const awpImage* pImg, const awpStrokeObj* pObj, awpPoint* p)
{
    AWPRESULT res;
    AWPDOUBLE sum,x0,y0;
    AWPBYTE* pix;
    AWPINT  x, y;
    AWPDWORD i;
    res = AWP_OK;
    sum = 0;
    x0 = 0;
    y0 = 0;

    _CHECK_RESULT_((res = awpCheckImage(pImg)))

    if(pImg->dwType!=AWP_BYTE)
    {
        res = AWP_NOTSUPPORT;
        _ERR_EXIT_
    }

    if(pImg->bChannels!=1)
    {
        res = AWP_NOTSUPPORT;
        _ERR_EXIT_
    }

    if (pObj == NULL || pObj->Num == 0)
    {
        res = AWP_BADARG;
        _ERR_EXIT_
    }

    pix = (AWPBYTE*)pImg->pPixels;
    for ( i = 0; i < pObj->Num; i++)
    {
       y = pObj->strokes[i].y;
       for (x = pObj->strokes[i].xl; x <= pObj->strokes[i].xr; x++)
       {
          sum += pix[y*pImg->sSizeX + x];
          x0  += x*pix[y*pImg->sSizeX + x];
          y0  += y*pix[y*pImg->sSizeX + x];
       }
    }

    p->X = (AWPSHORT)floor(x0 / (double)sum + 0.5);
	p->Y = (AWPSHORT)floor(y0 / (double)sum + 0.5);


CLEANUP:
    return res;
}

AWPRESULT awpGetObjOrientation(const awpImage* pImg, const awpStrokeObj* pObj, AWPDOUBLE* teta, AWPDOUBLE* mi, AWPDOUBLE* ma)
{
    /*local variables*/
    AWPRESULT res;
	AWPINT  x, y, j;
    AWPDOUBLE mxx, myy, mxy, intes; /*cental moments*/
	AWPBYTE* pixels;
	awpPoint center;
	AWPDWORD i;
    /*init out arguments */
    *teta = 0;
    *mi   = 0;
    *ma   = 0;
    res = AWP_OK;
    /*check the arguments*/
    _CHECK_RESULT_((res = awpCheckImage(pImg)))
    if (pObj == NULL || teta == NULL || mi == NULL || ma == NULL)
    {
        res = AWP_BADARG;
        _ERR_EXIT_
    }
    /*check the capabilities*/
    if (pImg->bChannels > 1 || pImg->dwType != AWP_BYTE)
    {
        res = AWP_BADARG;
        _ERR_EXIT_
    }
    /*init local variables*/
    mxx = 0; myy = 0; mxy = 0;
	if (awpGetObjCentroid(pImg, pObj, &center) != AWP_OK)
	{
		res = AWP_BADARG;
		_ERR_EXIT_
	}

	/*find the moment*/
	intes = 0;
	pixels = (AWPBYTE*)pImg->pPixels;
	for (i = 0; i < pObj->Num; i++)
	{
		for (j = pObj->strokes[i].xl; j <= pObj->strokes[i].xr; j++)
		{
			x = j;
			y = pObj->strokes[i].y;
			intes ++;//= pixels[y*pImg->sSizeX + x];
			mxx += (x - center.X)*(x - center.X);
			myy += (y - center.Y)*(y - center.Y);
			mxy += (x - center.X)*(y - center.Y);
		}
	}
	mxx /= intes;
	myy /= intes;
	mxy /= intes; 

	if (mxy == 0 && myy > mxx)
	{
		*teta = -AWP_PI / 2;
		*ma = 4 * sqrt(mxx);
		*mi = 4 * sqrt(myy);
	}
	else if (mxy == 0 && myy <= mxx) 
	{
		*teta = 0;
		*ma = 4 * sqrt(myy);
		*mi = 4 * sqrt(mxx);
	}
	else if (mxy != 0 && myy <= mxx)
	{
		*teta = atan(-2 * mxy/(sqrt((myy - mxx)*(myy - mxx) + 4 * mxy*mxy)));
		*ma = sqrt(8 * (myy + mxx + sqrt((myy - mxx)*(myy - mxx) + 4 * mxy*mxy)));
		*mi = sqrt(8 * (myy + mxx - sqrt((myy - mxx)*(myy - mxx) + 4 * mxy*mxy)));

	}
	else if (mxy != 0 && myy > mxx)
	{
		*teta = atan(sqrt(mxx + myy + sqrt((mxx - myy)*(mxx - myy) + 4 * mxy*mxy)) / (-2 * mxy));
		*ma = sqrt(8 * (myy + mxx + sqrt((myy - mxx)*(myy - mxx) + 4 * mxy*mxy)));
		*mi = sqrt(8 * (myy + mxx - sqrt((myy - mxx)*(myy - mxx) + 4 * mxy*mxy)));
	}

	*teta = 180.0*(*teta) / AWP_PI;

CLEANUP:
    return res;
}

AWPRESULT awpDrawStrokes(awpImage** ppimg, AWPWORD w, AWPWORD h, AWPINT num, awpStrokeObj* pobjs)
{
    AWPRESULT res;
    AWPINT i,k;
    unsigned long j;

    awpStrokeObj* cur_obj;
    awpStroke* cur_str;
    AWPBYTE* cur_pix;

    res=AWP_OK;

    _CHECK_RESULT_((res=awpCreateImage(ppimg, w, h, 1, AWP_BYTE)));

    for ( i=0; i<num; i+=1 ){
        cur_obj=pobjs+i;
        for( j=0; j<cur_obj->Num; j++){
            cur_str=cur_obj->strokes+j;

            for(k=cur_str->xl;k<=cur_str->xr;k++){
                if( (AWPWORD)k< w && k>=0 && cur_str->y>=0 && cur_str->y<h){
                    cur_pix=(AWPBYTE*)((**ppimg).pPixels)+k+(cur_str->y*w);
                    *cur_pix=255;
                }
            }
        }
    }
CLEANUP:
    return res;
}

AWPRESULT awpObtainSpotsTableFromImage(awpImage* pImage, AWPINT* pNumSpots, awpSpotProperty** ppProperties, awpStrokeObj** ppStrokes,AWPBYTE thr)
{
    awpContour pContour;
    awpRect r;
    awpPoint pp;
    AWPDOUBLE teta = 0;
    AWPDOUBLE mi = 0;
    AWPDOUBLE ma = 0;

    AWPDOUBLE m10;
    AWPDOUBLE m11;
    AWPDOUBLE m01;
    AWPDOUBLE m20;
    AWPDOUBLE m02;
    AWPDOUBLE m21;
    AWPDOUBLE m12;
    AWPDOUBLE m30;
    AWPDOUBLE m03;


    AWPDOUBLE p = 0;
    AWPRESULT res = AWP_OK;
    AWPINT i;
    awpStrokeObj* pStrokes = NULL;
    AWPINT NumStrokes = 0;

    /*Check the source variables*/
    if (pImage == NULL || pNumSpots == NULL || ppProperties == NULL){
        res = AWP_BADARG;
        _ERR_EXIT_
    }

    /*check input image*/
    _CHECK_RESULT_((res = awpCheckImage(pImage)))

    /* initial values of variables */
    *pNumSpots = 0;
    *ppProperties = NULL;

    /* stroke objects search */

    _CHECK_RESULT_((res = awpGetStrokes(pImage, &NumStrokes, &pStrokes, thr, NULL)))

    /* if the image conains objects control */
    if (NumStrokes == 0){
        res = AWP_OK;
	_ERR_EXIT_
    }

    *ppProperties = (awpSpotProperty*)malloc(NumStrokes*sizeof(awpSpotProperty));
    *ppStrokes = (awpStrokeObj*)malloc(NumStrokes*sizeof(awpStrokeObj));

    if (*ppProperties == NULL)
    {
        res = AWP_BADMEMORY;
        _ERR_EXIT_
    }
    /* features extraction */
    for(i=0;i<NumStrokes;i++)
    {
        /* actual area */
        AWPINT s;
        res = awpStrObjSquare(&pStrokes[i], &s);
        if (res != AWP_OK)continue;

        (*ppProperties)[*pNumSpots].flSquare = (AWPFLOAT)s;
        /* perimeter */

        res = awpGetObjCountour(&pStrokes[i], &pContour);
        if (res != AWP_OK)continue;
	
        /* perimeter */
        res = awpGetContPerim(&pContour, &p);

        /* counture deleting */
        if (pContour.Points != NULL)
            free(pContour.Points);
	    
        if (res != AWP_OK)continue;

        (*ppProperties)[*pNumSpots].flPerim = (AWPFLOAT)p;
        /* sizes */
        res = awpCalcObjRect(&pStrokes[i], &r);
        if (res != AWP_OK)continue;
        (*ppProperties)[*pNumSpots].flLx = (AWPFLOAT)(r.right - r.left);
        (*ppProperties)[*pNumSpots].flLy = (AWPFLOAT)(r.bottom - r.top);
        (*ppProperties)[*pNumSpots].Rect = r;
        /* shape coefficient */
        (*ppProperties)[*pNumSpots].flSapeCoef = (AWPFLOAT)((p*p)/(4*AWP_PI*s));
        /* cearch of the centre */
        res = awpGetObjCentroid(pImage, &pStrokes[i], &pp);
        if (res != AWP_OK)continue;
        (*ppProperties)[*pNumSpots].Center = pp;
        /* axes and turning angle */
        res = awpGetObjOrientation(pImage, &pStrokes[i], &teta, &mi, &ma);
        if (res != AWP_OK)continue;
        (*ppProperties)[*pNumSpots].flMajor = (AWPFLOAT)ma;
        (*ppProperties)[*pNumSpots].flMinor = (AWPFLOAT)mi;
        (*ppProperties)[*pNumSpots].flTeta  = (AWPFLOAT)teta;
    
	/*find the set of central moments*/
	_CHECK_RESULT_((res = awpGetObjMoment(pImage, &pStrokes[i], 1,0, &m10)))
	_CHECK_RESULT_((res = awpGetObjMoment(pImage, &pStrokes[i], 0,1, &m01)))
        _CHECK_RESULT_((res = awpGetObjMoment(pImage, &pStrokes[i], 2,0, &m20)))
	_CHECK_RESULT_((res = awpGetObjMoment(pImage, &pStrokes[i], 1,1, &m11)))
	_CHECK_RESULT_((res = awpGetObjMoment(pImage, &pStrokes[i], 0,2, &m02)))
        _CHECK_RESULT_((res = awpGetObjMoment(pImage, &pStrokes[i], 3,0, &m30)))
	_CHECK_RESULT_((res = awpGetObjMoment(pImage, &pStrokes[i], 2,1, &m21)))
        _CHECK_RESULT_((res = awpGetObjMoment(pImage, &pStrokes[i], 1,2, &m12)))
	_CHECK_RESULT_((res = awpGetObjMoment(pImage, &pStrokes[i], 0,3, &m03)))

	(*ppProperties)[*pNumSpots].dM10 = m10;
	(*ppProperties)[*pNumSpots].dM01 = m01;
	(*ppProperties)[*pNumSpots].dM20 = m20;
	(*ppProperties)[*pNumSpots].dM11 = m11;
	(*ppProperties)[*pNumSpots].dM02 = m02;
	(*ppProperties)[*pNumSpots].dM30 = m30;
	(*ppProperties)[*pNumSpots].dM21 = m21;
	(*ppProperties)[*pNumSpots].dM12 = m12;
	(*ppProperties)[*pNumSpots].dM03 = m03;
	
        /*copy the stroke object to ppStrokes*/
        //res = awpCopyStrokeObj(&pStrokes[i], ppStrokes[*pNumSpots]);
        (*ppStrokes)[*pNumSpots].Num = pStrokes[i].Num;
        (*ppStrokes)[*pNumSpots].strokes = (awpStroke*)malloc((*ppStrokes)[*pNumSpots].Num*sizeof(awpStroke));
        memcpy((*ppStrokes)[*pNumSpots].strokes, pStrokes[i].strokes, pStrokes[i].Num*sizeof(awpStroke));
        if (res != AWP_OK)continue;
        (*pNumSpots)++;
    } // the end of circle on number of Spots
    
    if (*pNumSpots == 0)
    {
        free(*ppProperties);
        *ppProperties = NULL;
        res = AWP_BADARG;
	_ERR_EXIT_
    }else{
	res = AWP_OK;
    }
    
CLEANUP:    /*all went well*/
    awpFreeStrokes(NumStrokes, &pStrokes);
    return res;
}

AWPRESULT awpDrawStrokeObj(awpImage* pImage, const awpStrokeObj* Obj, AWPBYTE v)
{
    AWPINT i,j;
    AWPRESULT res = AWP_OK;
    awpRect r;
    AWPBYTE* b;

    if(pImage == NULL || Obj == NULL || pImage->dwType != AWP_BYTE){
        res = AWP_BADARG;
	_ERR_EXIT_
    }
    if(pImage->bChannels != 1){
	res = AWP_BADARG;
	_ERR_EXIT_
    }

    awpCalcObjRect(Obj, &r);
    b = (AWPBYTE*)pImage->pPixels;
    if (r.right >= pImage->sSizeX || r.bottom >= pImage->sSizeY){
        res = AWP_BADARG;
	_ERR_EXIT_
    }
    /*draw all strokes*/
    for (i = 0; i < (AWPINT)Obj->Num; i++)
    {
        for (j = Obj->strokes[i].xl; j <= Obj->strokes[i].xr; j++)
        {
            b[Obj->strokes[i].y*pImage->sSizeX + j] = v;
        }
    }
CLEANUP:
    return res;
}

AWPRESULT awpDrawColorStrokeObj(awpImage* pImage, const awpStrokeObj* pObj, AWPBYTE r, AWPBYTE g, AWPBYTE b)
{
    AWPINT i,j;
    AWPRESULT res = AWP_OK;
    awpRect r1;
    AWPBYTE* b1;

    if(pImage == NULL || pObj == NULL || pImage->dwType != AWP_BYTE){
        res = AWP_BADARG;
	_ERR_EXIT_
    }
    if(pImage->bChannels != 3){
	res = AWP_BADARG;
	_ERR_EXIT_
    }

    awpCalcObjRect(pObj, &r1);
    b1 = (AWPBYTE*)pImage->pPixels;
    if (r1.right >= pImage->sSizeX || r1.bottom >= pImage->sSizeY){
        res = AWP_BADARG;
	_ERR_EXIT_
    }
    /*draw all strokes*/
    for (i = 0; i < (AWPINT)pObj->Num; i++)
    {
        for (j = pObj->strokes[i].xl; j <= pObj->strokes[i].xr; j++)
        {
            b1[pObj->strokes[i].y*pImage->sSizeX*3 + 3*j] = r;
            b1[pObj->strokes[i].y*pImage->sSizeX*3 + 3*j+1] = g;
            b1[pObj->strokes[i].y*pImage->sSizeX*3 + 3*j+2] = b;
        }
    }
CLEANUP:
    return res;
}


AWPRESULT awpFilterSpotsList(awpSpotProperty* pPropListIn, awpStrokeObj* pStrokesListIn,AWPINT iNumSpotsIn, awpSpotProperty** ppPropListOut,awpStrokeObj** ppStrokesListOut,AWPINT* iNumSpotsOut,AWPBOOL (*rule)(awpSpotProperty))
{
    AWPRESULT res;
    AWPBOOL* mas;
    AWPINT i,j;
    awpSpotProperty* pSpot;

    res =AWP_OK;
    *iNumSpotsOut=0;
    mas=NULL;
    pSpot=NULL;
    *ppStrokesListOut=NULL;
    
    //check count input spots
    if(iNumSpotsIn==0){
	//spots not exist	
	res=AWP_OK;
	_ERR_EXIT_
    }

    //allocate memory for spot and stroke objects
    mas = (AWPBOOL*)malloc(sizeof(AWPBOOL)*iNumSpotsIn);

    //main cyrcle on spots
    for(i=0;i<iNumSpotsIn;i++){
	if((*rule)(pPropListIn[i])!=TRUE){
	    mas[i]=FALSE;
	    continue;
	}
	mas[i]=TRUE;
	(*iNumSpotsOut)++;
    }
    
    //calculate spots satisfied condition
    if(iNumSpotsOut==0){
	//spots not exist
	res=AWP_OK;
	_ERR_EXIT_
    }
        
    //allocate memory for out spots and strokes
    *ppPropListOut = (awpSpotProperty*)malloc(sizeof(awpSpotProperty)**iNumSpotsOut);
    *ppStrokesListOut = (awpStrokeObj*)malloc(sizeof(awpStrokeObj)**iNumSpotsOut);
    
    pSpot=*ppPropListOut;
    
    //copy only conditions spots
    //memcpy(*ppPropListOut,pListSpot,sizeof(awpSpotProperty)**iNumSpotsOut);
    //memcpy(*ppStrokesListOut,pListStroke,sizeof(awpStrokeObj)**iNumSpotsOut);
    
    for(i=0,j=0;i<iNumSpotsIn;i++){
	if(mas[i]==TRUE){
	    //copy spots
	    memcpy(&pSpot[j],&pPropListIn[i],sizeof(awpSpotProperty));
	    //copy strokes
	    (*ppStrokesListOut)[j].Num = pStrokesListIn[i].Num;
    	    (*ppStrokesListOut)[j].strokes = (awpStroke*)malloc(pStrokesListIn[i].Num*sizeof(awpStroke));
    	    memcpy((*ppStrokesListOut)[j].strokes, pStrokesListIn[i].strokes, pStrokesListIn[i].Num*sizeof(awpStroke));

	    j++;
	}
    }


CLEANUP:
    free(mas);
    return res;
}
/*
calcalates total intensivity of stroke object
restrictions
working only AWP_BYTE and one channel images
*/
static AWPRESULT _awpGetObjTotalIntensivity(awpImage* pImg, awpStrokeObj* pObj, AWPDOUBLE* intensivity)
{
    /*local variables decalration */
    AWPRESULT res;
    AWPDWORD i;
    AWPWORD j;
    AWPWORD x,y;
    AWPBYTE* b;
    /*arguments checking*/
    _CHECK_RESULT_((res = awpCheckImage(pImg)))
    if (pObj == NULL || intensivity == NULL)
    {
         res = AWP_BADARG;
        _ERR_EXIT_
    }
    /*capabilities checking*/
    if (pImg->bChannels > 1 || pImg->dwType != AWP_BYTE)
    {
        res = AWP_NOTSUPPORT;
        _ERR_EXIT_
    }
    /*intialization of local variables*/
    i = 0; j = 0; x = 0; y = 0;
    b = (AWPBYTE*)pImg->pPixels;
    res = AWP_OK;
    *intensivity = 0;
    /*loop throught all strokes*/
    for(i = 0; i < pObj->Num; i++)
    {
        /*loop into one stroke*/
        for (j = pObj->strokes[i].xl; j < pObj->strokes[i].xr; j++)
        {
             x = j;
             y = pObj->strokes[i].y;
             *intensivity += b[y*pImg->sSizeX + x];
        }
    }

CLEANUP:
    return res;
}
/*
calculate moments of order p and q
restrictions:
working only with AWP_BYTE and one channel image
*/
AWPRESULT awpGetObjMoment(const awpImage* pImg, const awpStrokeObj* pObj, AWPINT p, AWPINT q, AWPDOUBLE* m)
{
    /*local variables*/
    AWPRESULT res;
    AWPDWORD i;
    AWPWORD  j;    /*counters*/
    AWPWORD x,y;   /*point position*/
    AWPDOUBLE total_intensivity;/*stroke object intensivity*/
    AWPDOUBLE d;
    awpPoint centroid;/*stroke object centroid*/
    AWPBYTE* b; /*pointer to pixels*/
    *m = 0;
    /*arguments checking*/
    _CHECK_RESULT_((res = awpCheckImage(pImg)))
    if (pObj == NULL || m == NULL)
    {
         res = AWP_BADARG;
        _ERR_EXIT_
    }

    /*capabilities checking*/
    if (pImg->bChannels > 1 || pImg->dwType != AWP_BYTE)
    {
        res = AWP_NOTSUPPORT;
        _ERR_EXIT_
    }

    /*total intensivity of stroke object*/
    total_intensivity = 0;
    _CHECK_RESULT_((res = _awpGetObjTotalIntensivity((awpImage*)pImg, (awpStrokeObj*)pObj, &total_intensivity)))
    if (total_intensivity == 0)
    {
        res =  AWP_BADARG;
        _ERR_EXIT_
    }
    /*find the centroid*/
    memset(&centroid, 0, sizeof(awpPoint));
    _CHECK_RESULT_((res = awpGetObjCentroid(pImg, pObj, &centroid)));
    b = (AWPBYTE*)pImg->pPixels;
    /*find the moment*/
    for (i = 0; i < pObj->Num; i++)
    {
        for (j = pObj->strokes[i].xl; j < pObj->strokes[i].xr; j++)
        {
            x = j;
            y = pObj->strokes[i].y;
			d =  pow(x + 0.5 - centroid.X, p);
			d *= pow(y + 0.5 - centroid.Y, q);
			d *= (AWPDOUBLE)b[y*pImg->sSizeX + x];
            *m += d / total_intensivity;
        }
    }

CLEANUP:
    return res;
}

AWPRESULT awpCopyStrokeObj1(awpStrokeObj* pSrc, awpStrokeObj* pDst)
{
    AWPRESULT res = AWP_OK;
    if (pSrc == NULL || pSrc->Num == 0)
        return AWP_BADARG;
    if (pDst == NULL)
        return AWP_BADARG;

    pDst->Num = pSrc->Num;
    pDst->strokes = NULL;
    pDst->strokes = (awpStroke*)malloc(pSrc->Num*sizeof(awpStroke));
    if (pDst->strokes == NULL)
        return AWP_BADMEMORY;
    memcpy(pDst->strokes, pSrc->strokes, pSrc->Num*sizeof(awpStroke));
    return res;
}

static AWPDOUBLE  _awpL2PointDistance(awpPoint p1, awpPoint p2)
{
   return sqrt((double)((p1.X - p2.X)*(p1.X - p2.X) + (p1.Y - p2.Y)*(p1.Y - p2.Y)));
}

static awpPoint _awpGetNearestPoint(awpPoint p, const awpStrokeObj* s)
{
    awpPoint  res;
    awpPoint  tmp;
    AWPDWORD    i;
    AWPDOUBLE mind,dst;
    tmp.X = s->strokes[0].xl;
    tmp.Y = s->strokes[0].y;
    mind = _awpL2PointDistance(p, tmp);
    res = tmp;
    for (i = 0; i < s->Num; i++)
    {
        tmp.X = s->strokes[i].xr;
        tmp.Y = s->strokes[i].y;
        dst = _awpL2PointDistance(p, tmp);
        if (dst < mind)
        {
            mind = dst;
            res = tmp;
        }
        tmp.X = s->strokes[i].xl;
        tmp.Y = s->strokes[i].y;
        dst = _awpL2PointDistance(p, tmp);
        if (dst < mind)
        {
            mind = dst;
            res = tmp;
        }
    }
    return res;
}
static awpPoint _awpGetFarestPoint(awpPoint p, const awpStrokeObj* s)
{
    awpPoint res;
    awpPoint  tmp;
    AWPDWORD    i;
    AWPDOUBLE mind,dst;
    tmp.X = s->strokes[0].xl;
    tmp.Y = s->strokes[0].y;
    mind = _awpL2PointDistance(p, tmp);
    res = tmp;
    for (i = 1; i < s->Num; i++)
    {
        tmp.X = s->strokes[i].xr;
        tmp.Y = s->strokes[i].y;
        dst = _awpL2PointDistance(p, tmp);
        if (dst > mind)
        {
            mind = dst;
            res = tmp;
        }
        tmp.X = s->strokes[i].xl;
        tmp.Y = s->strokes[i].y;
        dst = _awpL2PointDistance(p, tmp);
        if (dst > mind)
        {
            mind = dst;
            res = tmp;
        }
    }
    return res;
}


AWPRESULT awpGetObjPoint(const awpStrokeObj* s, awpPoint* src, awpPoint* dst, AWPBYTE options)
{
    awpPoint  result;
    AWPRESULT res = AWP_OK;

    if (s == NULL || src == NULL || dst == NULL)
       return AWP_BADARG;
    switch(options)
    {
        case AWP_FAREST_POINT:
            result =  _awpGetFarestPoint(*src, s);
        break;
        case AWP_NEAREST_POINT:
            result = _awpGetNearestPoint(*src, s);
        break;
        default:
            return  AWP_BADARG;
    }
    *dst = result;

    return res;
}

