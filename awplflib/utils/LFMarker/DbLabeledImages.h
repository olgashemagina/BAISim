//---------------------------------------------------------------------------
// database of labeled images
// ver 1.0
// definition of TDbLabeledImages

#ifndef DbLabeledImagesH
#define DbLabeledImagesH
#include "vcl.h"
#include <ComCtrls.hpp>
#include "_LF.h"


typedef void __fastcall (__closure *TDbProgressEvent)(int Progress, AnsiString& aComment);
enum EExportFormat {awp, jpeg};

///////////////////////////////////////////////////////////////////////////////
// export fragments options
struct SDbExportOptions
{
	AnsiString      strPathToExport;		//
	bool            useScanner;				//
	EExportFormat   exportFormat;           //
	float           scannerThreshold;       //
	bool            exportNearlest;         // Экспортировать ближайший
	bool 			exportCovered;			// Экспортировать только покрывающие
	bool            needFlip;               //
	int				racurs;                 //
	bool 			needResize;             //
	int				baseSize;               //
	bool 			copyIeye;               //
	bool 			copyRandom;             //
    bool 			copyBackground;			//
	int				random;                 //
	TStringList*    ClassLabels;			//
};
////////////////////////////////////////////////////////////////////////////////
// database copy options
struct SDbCopyOptions
{
	AnsiString strPathToCopy;
	bool copyImages;
	bool copySemantic;
};
////////////////////////////////////////////////////////////////////////////////
// convert database options
struct SDbConvertOptions
{
	bool    needResize;
	int     baseWidth;
	bool    interpolation;
	bool    RenameToUUID;
	EExportFormat format;
};

//---------------------------------------------------------------------------
// TDbLabeledImages
class TDbLabeledImages
{
protected:
	AnsiString 					m_strDbName;
    TLFDBLabeledImages          m_db;

	int        m_NumImages;
	int        m_NumXmlFiles;
	int        m_NumXmlItems;
	ILFDetectEngine* m_engine;
	TDbProgressEvent m_ProgressEvent;

	void __fastcall Clear();

	void __fastcall SaveFragment(awpImage* img, SDbExportOptions& options, int count, TLFRect& scanBox, /*TLFRoiImageDescriptor* roid = NULL, */const char* lpClassLabel = NULL);
    void __fastcall SaveBackground(SDbExportOptions& options);
	AnsiString 		MakeExportFileName(SDbExportOptions& options, int num, bool fliped, const char* lpClassLabel = NULL);

	TLFSemanticDictinary* __fastcall GetDictionary();
    TLFDBLabeledImages*   __fastcall GetDatabase();
    int __fastcall GetNumLabels();
public:
	TDbLabeledImages();
	~TDbLabeledImages();

	bool __fastcall Init(AnsiString& strDbPath, ILFDetectEngine* engine = NULL);
	const char* __fastcall GetFileName(int index);

	//Export of image fragments to the specified directory
	void __fastcall ExportFragments(SDbExportOptions& options);
	void __fastcall ConvertDatabase(SDbConvertOptions& convert_options);
	void __fastcall SplitDatabase(UnicodeString strPath, UnicodeString strPath1, UnicodeString strpath2, double propValue );

	// копирование всех изображений базы данных в соответствии со спецификацией   SDbCopyOptions
	void __fastcall CopyDatabase(SDbCopyOptions& options);
    void __fastcall ClearDatabase();
    void __fastcall UpdateDatabase();

	awpImage* __fastcall GetDbThumbinals(int thmbWidth = 128, int thmbHeight = 96);
	awpImage* __fastcall MakeSemanticThumbinals(int thmbWidth = 64, int thmbHeight = 64);
	void __fastcall DoMarking(ILFDetectEngine* engine);


// properties
   __property AnsiString DbName = {read = m_strDbName};
   __property int NumImages    = {read  = m_NumImages};
   __property int NumXmlFiles  = {read  = m_NumXmlFiles};
   __property int NumXmlItems  = {read  = m_NumXmlItems};
   __property int NumLabels    = {read  = GetNumLabels};
   __property TLFSemanticDictinary* Dictionary = {read =  GetDictionary};
   __property TLFDBLabeledImages*   Data  = {read = GetDatabase};
// events
   __property  TDbProgressEvent OnProgress = {read = m_ProgressEvent, write = m_ProgressEvent};
};
#endif
