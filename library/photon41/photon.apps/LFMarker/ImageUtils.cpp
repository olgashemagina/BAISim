//---------------------------------------------------------------------------
#include <vcl.h>

#pragma hdrstop
#include "ImageUtils.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

awpRect TRect2awpRect(TRect& rect)
{
	awpRect result;
    result.left = (AWPWORD)rect.Left;
    result.top  = (AWPWORD)rect.top;
    result.right = (AWPWORD)rect.Right;
    result.bottom = (AWPWORD)rect.bottom;
    return result;
}

TRect awpRect2TRect(awpRect r)
{
    TRect result;
    //result.init(r.left, r.top, r.right, r.bottom);
    result.Left = r.left;
    result.top = r.top;
    result.right = r.right;
    result.bottom = r.bottom;
    return result;
}

