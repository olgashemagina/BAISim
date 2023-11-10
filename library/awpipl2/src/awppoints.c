/*M
//
//      awpipl2 image processing and image analysis library
//		File: awpPointUtils.c
//		Purpose: calc euclidian distance between two points 
//		Autors: Alex A.Telnykh (tlnykha@yahoo.com)
//      CopyRight 2004-2019 (c) NN-Videolab.net
//M*/
#include "_awpipl.h"
#include "math.h"
/*
    Calculation of the Euclidean distance between two points
*/
AWPRESULT awpDistancePoints(awpPoint p1, awpPoint p2, AWPDOUBLE* distance)
{
    if (distance == NULL)
        return AWP_BADARG;
    *distance = sqrt((AWPFLOAT)((p1.X - p2.X)*(p1.X - p2.X)+(p1.Y - p2.Y)*(p1.Y - p2.Y)));
    return AWP_OK;
}

