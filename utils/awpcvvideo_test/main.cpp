//---------------------------------------------------------------------------

extern "C"
{
#include "awpcvvideo.h"
#include "awpipl.h"
}



#include <stdio.h>
#include <string>



    int main(int argc, char* argv[])
    {
        if (argc < 2)
            return 0;

        HCVVIDEO hvideo = awpcvConnect(argv[1]);
        if (hvideo)
        {
            int num = -1;
            awpcvNumFrames(hvideo, &num);
            printf("num frames = %d\n", num);
            for(int i = 0; i < num; i++)
            { 
                awpImage* img = NULL;

                awpcvQueryImagePos(hvideo, &img, i);
                std::string filename = std::to_string(i) + "_out.jpg";
                awpSaveImage(filename.c_str(), img);

                awpcvFreeImage(img);
            }
            
        }
        return 0;
    }
//---------------------------------------------------------------------------
 