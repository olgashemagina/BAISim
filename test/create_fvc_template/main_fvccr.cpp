#ifdef WIN32
#include <tchar.h>
#else
  typedef char _TCHAR;
  #define _tmain main
#endif

#include <stdio.h>

//#include <io.h>
#include <string>

#include "_LF.h"
#include "fvlcore.h"


void Usage()
{
	printf("Usage: fvccreate fvlcreate.xml\n");
}

 int main(int argc, char* argv[]) 
{
	if (argc < 2)
	{
		Usage();
		return 1;
	}

	// main program
	TiXmlDocument doc;
	if (!doc.LoadFile(argv[1]))
	{
		printf("error: cannot load configuration.\n");
		return 1;
	}

	TiXmlHandle hDoc(&doc);
	TiXmlElement* pElem = NULL;
	pElem = hDoc.FirstChildElement().Element();
	if (!pElem)
	{
		printf("error: invalid configuration file.\n");
		return 1;
	}
   const char* name = pElem->Value();
   if (strcmp(name, "fvccreate") != 0)
	{
		printf("error: invalid configuration file.\n");
		return 1;
	}

	string path 	     =  pElem->Attribute("in_database");
	string outName       =  pElem->Attribute("out_name");
	int width  = 24;
	int height = 24;
	int num_vct = 16;
	string format = pElem->Attribute("format");
	pElem->QueryIntAttribute("width",  &width);
	pElem->QueryIntAttribute("height",  &height);
	pElem->QueryIntAttribute("num_vct",  &num_vct);

	printf("params:\n");

	printf("input database: %s\n", path.c_str());
	printf("out name: %s\n", outName.c_str());
	printf("images format: %s\n", format.c_str());
	printf("width: %i\n", width);
	printf("height: %i\n", height);
	printf("num vectors: %i\n", num_vct);

   FvcTemplate* pTemplate = NULL;

	//string path( dirName );
   string path1 = path + "\\*." + format;
	_finddata_t filesInfo;
	int  numFiles = 0;
	intptr_t  handle = 0;
	if ( (handle = _findfirst( (char*)(path1.c_str()), &filesInfo)) != -1 )
	{
		do
		{
			numFiles++;

		}while(!_findnext( handle, &filesInfo ));
	}
	_findclose( handle );


	if( !numFiles ){
		cout << "Files not found" << endl;
		return -1;
	}
	int numVect = num_vct;
	awpImage** sourceImages = new awpImage* [numFiles];
	handle = _findfirst((char*)(path1.c_str()), &filesInfo );
	string fileName;


	for( int i=0; i<numFiles; i++  ){
		cout << filesInfo.name << endl;
		cout << ".";
		fileName = path + "\\" + filesInfo.name;

		AWPRESULT load_res = awpLoadImage( fileName.c_str(), &sourceImages[i] );
	   	awpConvert(sourceImages[i], AWP_CONVERT_TO_FLOAT);
		cout << fileName << endl;
		if( load_res )
			cout << load_res;
		if( sourceImages[i]->sSizeX != width && sourceImages[i]->sSizeY != height)
		{
			AWPRESULT resize_res = awpResize( sourceImages[i], width, height );
			cout << resize_res;
		}
		_findnext( handle, &filesInfo );
	}
	int create_res = fvcCreateTemplate( &pTemplate, width, height, numFiles, true );
	if( create_res != FVC_OK )
		cout << create_res;
	int build_res = fvcBuildTemplate(sourceImages, numFiles, pTemplate);
	if( build_res )
			cout << build_res;
	if( numVect )
		fvcResampleTemplate( pTemplate, numVect );
	int save_res = fvcSaveTemplate( outName.c_str(), pTemplate );
	if( save_res )
		cout << save_res;
	int n;
	char* tmplFileName = (char*)outName.c_str();
	string str( tmplFileName );
	n = str.rfind( '.', str.length()-1 );
	char * imageName = new char[ n+10 ];
	memcpy( imageName, str.c_str(), n+1 );
	memcpy( imageName+n+1, "jpg\0", 4 );
	int saveIm_res = fvcSaveTemplateAsImage( imageName, pTemplate );
	if( saveIm_res )
		cout << saveIm_res;
	for( int j = 0; j < numFiles; j++ )
	{
		if( sourceImages[j] != NULL )
			awpReleaseImage( &sourceImages[j] );
	}
	fvcFreeTemplate( &pTemplate );
	delete[] sourceImages;
	delete[] imageName;

	printf("done.\n");

	return 0;
}
