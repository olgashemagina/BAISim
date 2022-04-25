#ifndef _AWP_DRAW_H_
#define _AWP_DRAW_H_
/*draw point*/
AWPRESULT _awpDrawPoint(awpImage* pImage, awpPoint p, AWPBYTE bChan, AWPDOUBLE dValue);
AWPRESULT _awpDrawThickPoint(awpImage* pImage, awpPoint p, AWPBYTE bChan, AWPDOUBLE dValue, AWPBYTE radius);
/*draw line*/
AWPRESULT _awpDrawLine(awpImage* pImage, awpPoint p1, awpPoint p2, AWPBYTE bChan, AWPDOUBLE dValue);
AWPRESULT _awpDrawThickLine(awpImage* pImage, awpPoint p1, awpPoint p2, AWPBYTE bChan, AWPDOUBLE dValue, AWPBYTE radius);
/*draw rect*/
AWPRESULT _awpDrawRect(awpImage* pImage, awpRect* pRect, AWPBYTE bChan, AWPDOUBLE dValue);
AWPRESULT _awpDrawThickRect(awpImage* pImage, awpRect* pRect, AWPBYTE bChan, AWPDOUBLE dValue, AWPBYTE radius);
/*draw cross*/
AWPRESULT _awpDrawCross(awpImage* pImage, awpRect* pRect, AWPBYTE bChan, AWPDOUBLE dValue);
AWPRESULT _awpDrawThickCross(awpImage* pImage, awpRect* pRect, AWPBYTE bChan, AWPDOUBLE dValue, AWPBYTE radius);
/*draw ellipse*/
AWPRESULT _awpDrawEllipse(awpImage* pImage, awpPoint center, AWPWORD width, AWPWORD height, AWPDOUBLE angle, AWPBYTE bChan, AWPDOUBLE dValue);
AWPRESULT _awpDrawThickEllipse(awpImage* pImage, awpPoint center, AWPWORD width, AWPWORD height, AWPDOUBLE angle, AWPBYTE bChan, AWPDOUBLE dValue, AWPBYTE radius);
/*draw ellipse v2*/
AWPRESULT _awpDrawEllipseV2(awpImage* pImage, awpPoint center, AWPWORD width, AWPWORD height, AWPDOUBLE angle, AWPBYTE bChan, AWPDOUBLE dValue);
AWPRESULT _awpDrawThickEllipseV2(awpImage* pImage, awpPoint center, AWPWORD width, AWPWORD height, AWPDOUBLE angle, AWPBYTE bChan, AWPDOUBLE dValue, AWPBYTE radius);
/*draw polygon*/
AWPRESULT _awpDrawPolygon(awpImage* pImage, awpContour* pContour, AWPBYTE bChan, AWPDOUBLE dValue);
AWPRESULT _awpDrawThickPolygon(awpImage* pImage, awpContour* pContour, AWPBYTE bChan, AWPDOUBLE dValue, AWPBYTE radius);

#endif /*_AWP_DRAW_H_*/