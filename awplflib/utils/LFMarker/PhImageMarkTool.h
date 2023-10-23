//---------------------------------------------------------------------------

#ifndef PhImageMarkToolH
#define PhImageMarkToolH
#include "PhImageTool.h"
#include "_LF.h"
//---------------------------------------------------------------------------
typedef enum {MTRect, MTVector, MTContour} TEMarkToolModes;
typedef void __fastcall (__closure *TPhAddDataEvent)(System::TObject* Sender, TLFDetectedItem* item);
typedef void __fastcall (__closure *TPhDelFrameEvent)(System::TObject* Sender, TLFDetectedItem* item);
typedef void __fastcall (__closure *TPhExportProgressEvent)(System::TObject* Sender, int progress, String& comment);
class PACKAGE TPhImageMarkTool : public TPhImageTool
{
private:
	bool m_down;
	int  m_si; // selected index
	int  m_sv; // selectes vertex
    int  m_selected;
	TRect* m_newRect;
	TLFZone* m_newZone;

	TPoint GetRectPoint(int index, TRect& rect);
	void __fastcall SetVertex(int x, int y);
	bool CheckZoneType(TLFSemanticDictinaryItem* sdi);
	bool m_edited;
	TEMarkToolModes m_mode;
protected:
	 TPopupMenu *PopupMenu;
     TLFSemanticDictinary* m_dictinary;
     AnsiString m_strName;
     TLFSemanticImageDescriptor m_descriptor;

	 double _2D_Dist(double x1,double y1,double x2,double y2);
	 bool   _is_near_vertex(int X, int Y, int& idx1, int& idx2);

	 String _get_label();
	 TColor GetItemColor(TLFDetectedItem* itm);
	 TEZoneTypes GetItemZoneType(TLFDetectedItem* itm);

	 void __fastcall DoPopup(int X, int Y);
	 void __fastcall PopupClick(TObject* sender);
	 void __fastcall SetMode(const TEMarkToolModes Value);

	TNotifyEvent 		m_OnChange;
	TNotifyEvent 		m_OnLoad;
	TPhAddDataEvent     m_OnAddData;
	TPhDelFrameEvent    m_OnDelFrame;
	TPhExportProgressEvent m_OnProgress;
protected:
	TLFSemanticDictinaryItem* __fastcall GetClasses(int index);
	void __fastcall SetDictionary(TLFSemanticDictinary* value);
public:
	__fastcall TPhImageMarkTool(TComponent* Owner);
   virtual __fastcall ~TPhImageMarkTool();

	virtual void Draw(TCanvas* Canvas);
	virtual void MouseDown(int X, int Y, TMouseButton Button = mbLeft);
	virtual void MouseUp(int X, int Y, TMouseButton Button = mbLeft);
	virtual void MouseMove(int X, int Y, TShiftState Shift);
	virtual void Reset();

	// go to specific frame in the media source
	void __fastcall  SetFrame(const char* lpFileName);
	// delete frame data from frames
	void __fastcall  DeleteEntry(int index);
	//
	void __fastcall  DeleteObject(int index);
	// delete class label
	void __fastcall DeleteLabel(String label);
	// label statistics
	void __fastcall LabelStatistics(String label, int& numFrames, int& numEntries);

	// show selected item
	void __fastcall  SetSelected(int index);
	//
	void __fastcall ChangeLabel(String& src_label, String& dst_label);

	//properties
	__property TLFSemanticDictinary* dictinary = {read = m_dictinary, write = SetDictionary};
	__property TLFSemanticDictinaryItem* classes[int index] = {read = GetClasses};

	__property TNotifyEvent OnChange= {read = m_OnChange, write = m_OnChange};
	__property TNotifyEvent OnLoad  = {read = m_OnLoad, write = m_OnLoad};
	__property TPhAddDataEvent     OnAddData = {read = m_OnAddData, write = m_OnAddData};
	__property TPhDelFrameEvent    OnDelFrame = {read = m_OnDelFrame, write = m_OnDelFrame};
	__property TPhExportProgressEvent OnProgress = {read = m_OnProgress, write = m_OnProgress};
	__property AnsiString DescrFile = {read = m_strName};
	__property TEMarkToolModes Mode = {read = m_mode, write = SetMode};
};


#endif
