//---------------------------------------------------------------------------
#ifndef PhFramesH
#define PhFramesH
#include <System.Classes.hpp>
#include "awpipl.h"
#include "PhUtils.h"
//---------------------------------------------------------------------------
#define _FRAME_MIN_COUNT_ 3
typedef enum {jpegFormat, pngFormat, tiffFormat, tgaFormat, bmpFormat, awpFormat, ppmFormat} EPhImageFormats;

class TPhImage;
class TPhJobThread;
struct SFrameItem
{
    UnicodeString strFileName;
    int           width;
    int           height;
	bool          selected;
    awpImage*     img;
};

class PACKAGE TPhFrames : public TObject
{
private:
	TPhImage* 				m_pDisplay;
    TList*                  m_items;
	TGraphic*               m_pMosaic;
	TPhJobThread* 			m_pReader;
	int       				m_current;
	void __fastcall OnTerminateHelper(TObject *Sender);
protected:
	int __fastcall GetCount();
	void StartReadJobHelper();
public:
	TPhFrames(TPhImage* display);
	__fastcall virtual~TPhFrames();

	// init
	bool Init(TStrings* names);
	void Close();
	void Cancel();
	// navigation
	void __fastcall First();
	void __fastcall Next();
	void __fastcall Prev();
	void __fastcall Last();
	void __fastcall GoFrame(long num);

	// operations
	bool __fastcall DeleteImage(long num);
	bool __fastcall DeleteSelected();
	bool __fastcall CopySelected(const LPWSTR lpDirName);
	bool __fastcall MoveSelected(const LPWSTR lpDirName);
	bool __fastcall ConvertSelected(EPhImageFormats format, bool keepSource);

	// selectiom
	void __fastcall SelectAll();
	void __fastcall InvertSelection();
	void __fastcall ClearSelection();

	SFrameItem* GetFrameItem(long num);
	__property SFrameItem* Frame[long index] = {read = GetFrameItem};
	__property int Count = {read = GetCount};
	__property int CurrentFrame = {read = m_current};
	__property TGraphic* Mosaic = {read = m_pMosaic};
};
#endif
