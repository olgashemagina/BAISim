//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "awpcvvideo.h"

#pragma link "awpcvvideo.lib"
#pragma link "awpipl2b.lib"
#pragma link "jpeglib.lib"


//---------------------------------------------------------------------------

#pragma argsused
int main(int argc, char* argv[])
{
    if (argc < 2)
        return 0;

    HCVVIDEO hvideo =  awpcvConnect(argv[1]);
    if (hvideo)
    {
      int num = -1;
      awpcvNumFrames(hvideo, &num);
      printf ("num frames = %d\n", num);

      awpImage* img = NULL;

      awpcvQueryImage(hvideo, &img);

      awpSaveImage("out.jpg",img);

      awpcvFreeImage(img);
     }
    return 0;
}
//---------------------------------------------------------------------------
 