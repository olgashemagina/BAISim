#define TIXML_USE_STL
#include <string>
#include "_LF.h"

/*начало, поехали*/
int main()
{
	AWPDWORD awp_int = 0;	
	printf("Hello Locate Framework!\n");
	printf("Size if AWPDWORD %d \n", sizeof(awp_int));			
	TLFImage image;
	TLFDetectEngine engine;
	printf("Loading engine....");
	if (!engine.Load("/mnt/awplflib/engine.xml"))
	{
		printf("cannot load engine!\n");
		return -1;
	}
	printf("done.\n");
	printf("Loading image....");
	if (!image.LoadFromFile("/mnt/awplflib/6004.awp"))
	{
		printf("cannot load image\n");
		return -1;
	}
	image.SaveToFile("test1.awp");
	image.SaveToFile("test1.jpg");
	image.LoadFromFile("test1.awp");
	printf("done.\n");
	printf("Process image....");
	if (!engine.SetSourceImage(&image, true))
	{
		printf("cannot process image\n");
		return -1;
	}
	printf("done.\n");

	if (engine.GetItemsCount() > 0)
	{
		printf("number of detected objects = %i\n", engine.GetItemsCount());
		TLFSemanticImageDescriptor* si = engine.GetSemantic();
		if (si != NULL)
			si->SaveXML("faces.xml");
	}
	else 
printf("!!!");

	printf("Work done.\n");			

	return 0;
}
/*вот и все*/
