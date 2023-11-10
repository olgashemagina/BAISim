/*
    AWP Image library v2
    
    $Source: /home/cvs/awp/imageproc/awpipl-v2/src/awpfft.c,v $
    $Revision: 1.17 $
    $Date: 2005/04/14 11:20:56 $
    
    Authors: Eugene Eremin <medlab@appl.sci-nnov.ru>
*/

#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif

#ifdef HAVE_FFTW3

#include "_awpipl.h"

#include <fftw3.h>


AWPRESULT awpFFT(awpImage** pImage,awpImage** pImageRe,awpImage** pImageIm, AWPINT iFftDir)
{
    AWPRESULT res;
    fftw_plan p;
    AWPDOUBLE *in;
    fftw_complex *out;
    SHORT W,H;
    AWPINT i,j,count;
    AWPDOUBLE *pRe,*pIm;
    
    res = AWP_OK;/*initialize result */
    in=NULL;out=NULL;
    p=NULL;

    switch(iFftDir){
    case AWP_FFT_FORWARD:
        /* check image */
        _CHECK_RESULT_((res = awpCheckImage(*pImage)))
	
	/* only AWPDOUBLE image support */
	if((*pImage)->dwType!=AWP_DOUBLE){
	    res = AWP_NOTSUPPORT;
    	    _ERR_EXIT_
	}

        *pImageRe=NULL;
        *pImageIm=NULL;

        W=(*pImage)->sSizeX;
        H=(*pImage)->sSizeY;

        break;
    case AWP_FFT_INVERSE:

        /* check image */
        _CHECK_RESULT_((res = awpCheckImage(*pImageRe)))
        _CHECK_RESULT_((res = awpCheckImage(*pImageIm)))
	
	/* only AWPDOUBLE image support */
	if((*pImageRe)->dwType != AWP_DOUBLE || (*pImageIm)->dwType != AWP_DOUBLE ){
	    res = AWP_NOTSUPPORT;
    	    _ERR_EXIT_
	}
	
	if((*pImageRe)->sSizeX != (*pImageIm)->sSizeX || (*pImageRe)->sSizeY != (*pImageIm)->sSizeY ){
	    res = AWP_BADARG;
	    _ERR_EXIT_
	}

        W=(*pImageRe)->sSizeX;
        H=(*pImageRe)->sSizeY;

        *pImage=NULL;
	
        break;
    default:
        res = AWP_BADARG;
	_ERR_EXIT_
    }

    /* allocate memory for fft */
    out =(fftw_complex*)fftw_malloc(sizeof(fftw_complex)*W*(H/2+1));

    switch(iFftDir){
    case AWP_FFT_FORWARD:
        in=(AWPDOUBLE*)(*pImage)->pPixels;
	
	/* create plan */
	p= fftw_plan_dft_r2c_2d(W,H,in,out,FFTW_ESTIMATE);
	
        break;
    case AWP_FFT_INVERSE:
        /* fill dat from image */
        pRe=(AWPDOUBLE*)(*pImageRe)->pPixels;
        pIm=(AWPDOUBLE*)(*pImageIm)->pPixels;

        for(i=0;i<H;i++){
        for(j=0;j<W;j++){
           	    out[i*W+j][0]=pRe[i*W+j];
           	    out[i*W+j][1]=pIm[i*W+j];
        }
        }

        /* create image */
        _CHECK_RESULT_((res=awpCreateImage(pImage,W,H,1,AWP_DOUBLE)))
	
	in=(AWPDOUBLE*)(*pImage)->pPixels;
	
	/* create plan */
	p= fftw_plan_dft_c2r_2d(W,H,out,in,FFTW_ESTIMATE);
	
        break;
    }

    if(p==NULL){
        res = AWP_BADMEMORY;
        _ERR_EXIT_
    }

    /* execute fft transofm */
    fftw_execute(p);

    if(iFftDir==AWP_FFT_FORWARD){
        /* create image */
        _CHECK_RESULT_((res=awpCreateImage(pImageRe,W,H/2+1,1,AWP_DOUBLE)))
        _CHECK_RESULT_((res=awpCreateImage(pImageIm,W,H/2+1,1,AWP_DOUBLE)))

        pRe=(AWPDOUBLE*)(*pImageRe)->pPixels;
        pIm=(AWPDOUBLE*)(*pImageIm)->pPixels;
	
        for(i=0;i<W;i++){
	    count=W*(H/2+1);    
            for(j=0;j<H/2+1;j++){
                pRe[j*W+i]=out[count+j][0];
                pIm[j*W+i]=out[count+j][1];
            }
        }
    }

CLEANUP:
    if(p!=NULL){
        fftw_destroy_plan(p);
    }

    return res;
}

AWPRESULT awpConvFFTCircular(awpImage* pSrcImage,awpImage* pMask,awpImage** pDstImage)
{

    AWPRESULT res;
    AWPDOUBLE *im,*m,*ms,*ic;
    fftw_complex* IM,*M,*C;
    awpImage* pBigMask;
    AWPINT cmx,cmy;/* center mask */
    AWPINT i,j;/* cyrcle value */
    WORD W,H;
    awpImage* pImCp;
    fftw_plan pi=NULL,pm=NULL,pc=NULL;
    AWPDOUBLE scale;
    AWPINT ij,ij_j;

    res = AWP_OK;/*initialize result */
    IM=NULL;M=NULL;C=NULL;
    *pDstImage=NULL;
    
    /* check image */
    _CHECK_RESULT_((res = awpCheckImage(pSrcImage)))
    _CHECK_RESULT_((res = awpCheckImage(pMask)))    
    
    /* check type and channels images */
    if(pSrcImage->dwType!=AWP_DOUBLE || pSrcImage->bChannels!=1 || pMask->dwType!=AWP_DOUBLE || pMask->bChannels!=1){
	res = AWP_NOTSUPPORT;
    	_ERR_EXIT_
    }
    
    /* generate big mask */
    /* create image eq pSrcImage */
    _CHECK_RESULT_((res=awpCreateImage(&pBigMask,pSrcImage->sSizeX,pSrcImage->sSizeY,1,AWP_DOUBLE)))
    
    /* calculate center mask */
    cmx=pMask->sSizeX/2;
    cmy=pMask->sSizeY/2;
    
    m=(AWPDOUBLE*)pBigMask->pPixels;
    ms=(AWPDOUBLE*)pMask->pPixels;
    
    /* fill big mask */
    /* fill top left angle big mask */
    for(i=0;i<cmy+1;i++){
	for(j=0;j<cmx+1;j++){
	    m[i*pBigMask->sSizeX+j]=ms[(i+cmy-1)*pMask->sSizeX+j+cmx-1];
	}
    }
    
    /* fill top right angle big mask */
    for(i=0;i<cmy+1;i++){
	for(j=0;j<cmx;j++){	    
	    m[i*pBigMask->sSizeX+(pBigMask->sSizeX-cmx+j)]=ms[(i+cmy)*pMask->sSizeX+j];
	}
    }
    
    for(i=0;i<cmy;i++){
	for(j=0;j<cmx+1;j++){
	    /* fill bottom left angle big mask */
	    m[(pBigMask->sSizeY-cmy+i)*pBigMask->sSizeX+j]=ms[i*pMask->sSizeX+j+cmx];
	}
    }
    
    for(i=0;i<cmy;i++){
	for(j=0;j<cmx;j++){
	    /* fill bottom right angle big mask */
	    m[(pBigMask->sSizeY-cmy+i)*pBigMask->sSizeX+(pBigMask->sSizeX-cmx+j)]=ms[i*pMask->sSizeX+j];
	}
    }
    
    awpCopyImage(pBigMask,&pImCp);
    awpConvert(pImCp,AWP_CONVERT_TO_BYTE_WITH_NORM);
    awpSaveImage("mask_big.jpg",pImCp);
    awpReleaseImage(&pImCp);
    
    im=(AWPDOUBLE*)pSrcImage->pPixels;
    
    W=pSrcImage->sSizeX;
    H=pSrcImage->sSizeY;
    scale = 1.0/(W*H);
    
    /* allocate data for complex data */
    IM = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*H*(W/2+1));
    M = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*H*(W/2+1)); 
    C = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*H*(W/2+1)); 
    
    /* create plan  for image */
    pi = fftw_plan_dft_r2c_2d(H,W,im,IM,FFTW_ESTIMATE);
	
    if(pi==NULL){
        res = AWP_BADMEMORY;
        _ERR_EXIT_
    }

    /* execute fft transofm */
    fftw_execute(pi);
    
    /* create plan  for image */
    pm = fftw_plan_dft_r2c_2d(H,W,m,M,FFTW_ESTIMATE);
	
    if(pm==NULL){
        res = AWP_BADMEMORY;
        _ERR_EXIT_
    }

    /* execute fft transofm */
    fftw_execute(pm);
    
    for(i=0;i<H;i++){
	ij_j=i*(W/2+1);
	for(j=0;j<W/2+1;j++){
	    ij=ij_j+j;    
	    C[ij][0]=( IM[ij][0]*M[ij][0] - IM[ij][1]*M[ij][1] )*scale;
	    C[ij][1]=( IM[ij][0]*M[ij][1] + IM[ij][1]*M[ij][0] )*scale;
	}
    }
    
    _CHECK_RESULT_((res=awpCreateImage(pDstImage,pSrcImage->sSizeX,pSrcImage->sSizeY,1,AWP_DOUBLE)))
    
    ic=(AWPDOUBLE*)(*pDstImage)->pPixels;
    
    pc = fftw_plan_dft_c2r_2d(H,W,C,ic,FFTW_ESTIMATE);
    
    if(pc==NULL){
        res = AWP_BADMEMORY;
        _ERR_EXIT_
    }

    /* execute fft transofm */
    fftw_execute(pc);

CLEANUP:

    if(pi!=NULL){
        fftw_destroy_plan(pi);
    }
    
    if(pm!=NULL){
        fftw_destroy_plan(pm);
    }
    
    if(pc!=NULL){
        fftw_destroy_plan(pc);
    }
    
    if(IM!=NULL){
	fftw_free(IM);
    }
    
    if(M!=NULL){
	fftw_free(M);
    }
    
    if(C!=NULL){
	fftw_free(C);
    }

    return res;    

}

AWPRESULT awpConvFFTLinear(awpImage* pSrcImage,awpImage* pMask,awpImage** pDstImage)
{
    AWPRESULT res;
    awpImage* pBigMask=NULL,*pTmpImage=NULL;
    AWPINT i,j;
    AWPDOUBLE *im,*m,*ms,*ic,*id;
    fftw_complex* IM,*M,*C;
    WORD W,H,W_2;
    fftw_plan pi=NULL,pm=NULL,pc=NULL;
    AWPDOUBLE scale;
    AWPINT ij,ij_j;
    AWPINT ii,ii1;
    
    IM=NULL;M=NULL;C=NULL;
    *pDstImage=NULL;
    pTmpImage=NULL;
    res = AWP_OK;
    
    /* check image */
    _CHECK_RESULT_((res = awpCheckImage(pSrcImage)))
    _CHECK_RESULT_((res = awpCheckImage(pMask)))    
    
    /* check type and channels images */
    if(pSrcImage->dwType!=AWP_DOUBLE || pSrcImage->bChannels!=1 || pMask->dwType!=AWP_DOUBLE || pMask->bChannels!=1){
	res = AWP_NOTSUPPORT;
    	_ERR_EXIT_
    }
    
    /* check size image and mask */
    if( pSrcImage->sSizeX<pMask->sSizeX || pSrcImage->sSizeY<pMask->sSizeY){
	res = AWP_BADARG;
	_ERR_EXIT_
    }
    
    /* generate big mask */
    /* create image eq pSrcImage */
    _CHECK_RESULT_((res=awpCreateImage(&pBigMask,pSrcImage->sSizeX,pSrcImage->sSizeY,1,AWP_DOUBLE)))
    
    m=(AWPDOUBLE*)pBigMask->pPixels;
    ms=(AWPDOUBLE*)pMask->pPixels;
    
    for(i=0;i<pMask->sSizeY;i++){
	for(j=0;j<pMask->sSizeX;j++){
	    m[i*pBigMask->sSizeX+j]=ms[i*pMask->sSizeX+j];
	}
    }
    
    im=(AWPDOUBLE*)pSrcImage->pPixels;
    
    W=pSrcImage->sSizeX;
    H=pSrcImage->sSizeY;
    scale = 1.0/(W*H);
    W_2=W/2+1;
    
    /* allocate data for complex data */
    IM = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*H*W_2);
    M = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*H*W_2); 
    C = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*H*W_2); 
    
    /* create plan  for image */
    pi = fftw_plan_dft_r2c_2d(H,W,im,IM,FFTW_ESTIMATE);
	
    if(pi==NULL){
        res = AWP_BADMEMORY;
        _ERR_EXIT_
    }

    /* execute fft transofm */
    fftw_execute(pi);
    
    /* create plan  for image */
    pm = fftw_plan_dft_r2c_2d(H,W,m,M,FFTW_ESTIMATE);
	
    if(pm==NULL){
        res = AWP_BADMEMORY;
        _ERR_EXIT_
    }

    /* execute fft transofm */
    fftw_execute(pm);
    
    for(i=0;i<H;i++){
	ij_j=i*W_2;
	for(j=0;j<W_2;j++){
	    ij=ij_j+j;    
	    C[ij][0]=( IM[ij][0]*M[ij][0] - IM[ij][1]*M[ij][1] )*scale;
	    C[ij][1]=( IM[ij][0]*M[ij][1] + IM[ij][1]*M[ij][0] )*scale;
	}
    }
    
    _CHECK_RESULT_((res=awpCreateImage(&pTmpImage,pSrcImage->sSizeX,pSrcImage->sSizeY,1,AWP_DOUBLE)))
    
    ic=(AWPDOUBLE*)pTmpImage->pPixels;
    
    pc = fftw_plan_dft_c2r_2d(H,W,C,ic,FFTW_ESTIMATE);
    
    if(pc==NULL){
        res = AWP_BADMEMORY;
        _ERR_EXIT_
    }

    /* execute fft transofm */
    fftw_execute(pc);
    
    _CHECK_RESULT_((res=awpCreateImage(pDstImage,pSrcImage->sSizeX,pSrcImage->sSizeY,1,AWP_DOUBLE)))
    
    id=(AWPDOUBLE*)(*pDstImage)->pPixels;
    
    for(i=0;i<pSrcImage->sSizeY-pMask->sSizeY;i++){
	ii=(i+pMask->sSizeY/2)*pSrcImage->sSizeX+pMask->sSizeX/2;
	ii1=(i+pMask->sSizeY-1)*pSrcImage->sSizeX+pMask->sSizeX-1;
	for(j=0;j<pSrcImage->sSizeX-pMask->sSizeX;j++){
	    id[ii+j]=ic[ii1+j];
	}
    }
    
CLEANUP:
    awpReleaseImage(&pBigMask);
    awpReleaseImage(&pTmpImage);    
    
    if(pi!=NULL){
        fftw_destroy_plan(pi);
    }
    
    if(pm!=NULL){
        fftw_destroy_plan(pm);
    }
    
    if(pc!=NULL){
        fftw_destroy_plan(pc);
    }
    
    if(IM!=NULL){
	fftw_free(IM);
    }
    
    if(M!=NULL){
	fftw_free(M);
    }
    
    if(C!=NULL){
	fftw_free(C);
    }
    
    return res;    
}


AWPRESULT awpSpectrum(awpImage* pImageRe,awpImage* pImageIm,awpImage** pImage)
{
    AWPRESULT res;
    long lSize;
    long i;
    AWPDOUBLE* pD,*pRe,*pIm;
    
    res = AWP_OK;/*initialize result */
	
    lSize=pImageRe->sSizeX*pImageRe->sSizeY;
    
    _CHECK_RESULT_((res = awpCreateImage(pImage,pImageRe->sSizeX,pImageIm->sSizeY,1,AWP_DOUBLE)))
    
    pD=(AWPDOUBLE*)(*pImage)->pPixels;
    pRe=(AWPDOUBLE*)pImageRe->pPixels;
    pIm=(AWPDOUBLE*)pImageIm->pPixels;
    
    for(i=0;i<lSize;i++){
	pD[i]=sqrt(pRe[i]*pRe[i]+pIm[i]*pIm[i]);		
    }
    
CLEANUP:

    return res;
}
#endif /* HAVE_FFTW3 */
