//---------------------------------------------------------------------------

#ifndef PhLandmarksToolH
#define PhLandmarksToolH
//---------------------------------------------------------------------------
#include "PhImageTool.h"
#include "PhPaneTool.h"
#include "_LF.h"
class PACKAGE TPhLandmarksTool : public TPhPaneTool
{
private:
	bool   m_down;
	TLFDBLandmarks m_db;
	TLFLandmarkFile* m_file;
	int    m_selected;

	int    m_skin;
protected:
	bool __fastcall IsNearPoint(int x, int y, int& idx);
	bool __fastcall GetConnected();
	void __fastcall SetSkin(const int value);
	TLFDBLandmarks* __fastcall GetDatabase();
	TNotifyEvent 		m_OnChange;
public:
	__fastcall TPhLandmarksTool(TComponent* Owner);
   virtual __fastcall ~TPhLandmarksTool();

	virtual void Draw(TCanvas* Canvas);
	virtual void MouseDown(int X, int Y, TMouseButton Button = mbLeft);
	virtual void MouseUp(int X, int Y, TMouseButton Button = mbLeft);
	virtual void MouseMove(int X, int Y, TShiftState Shift);
	virtual void Reset();

	// database wrapper
	bool __fastcall Connect(const UnicodeString& fileName);
	void __fastcall Close();
	bool __fastcall SelectFile(const UnicodeString& fileName);

	__property bool Connected = {read = GetConnected};
	__property TLFDBLandmarks* db = {read = GetDatabase};
	__property int Selected = {read = m_selected};
	__property int Skin = {read = m_skin, write = SetSkin};
__published:
	__property TNotifyEvent OnChange = {read = m_OnChange, write= m_OnChange};
};
#endif
