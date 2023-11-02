//---------------------------------------------------------------------------

#ifndef PhVideoMarkToolH
#define PhVideoMarkToolH
//---------------------------------------------------------------------------
#include "PhImageTool.h"
#include "PhVideo.h"
#include "_LF.h"
/*
*/
class TMarkItem : public TLFObject
{
protected:
	TRect    m_rect;
	String   m_label;
	int      m_frame;
    TColor   m_color;
    int      m_id;
public:
	TMarkItem();
	virtual ~TMarkItem();

   __property String label = {read = m_label, write = m_label};
   __property TRect  rect  = {read = m_rect, write = m_rect};
   __property int frame = {read = m_frame, write = m_frame};
   __property TColor color = {read = m_color, write = m_color};
   __property int id = {read = m_id, write = m_id};
};
/*
    Frame item - the collection of market items on specific frame
*/
class TFrameItem : public TLFObjectList
{
protected:
    /*frame number*/
	int m_frame;
public:
   TFrameItem();
   virtual ~TFrameItem();

   TiXmlElement* SaveXML(int w, int h);
   bool LoadXML(TiXmlElement* parent);
   TLFSemanticImageDescriptor* GetDescriptor(int w, int h);

   __property int FrameNum = {read = m_frame, write = m_frame};
};

typedef void __fastcall (__closure *TPhAddDataEvent)(System::TObject* Sender, TFrameItem* item);
typedef void __fastcall (__closure *TPhDelFrameEvent)(System::TObject* Sender, TFrameItem* item);
typedef void __fastcall (__closure *TPhExportProgressEvent)(System::TObject* Sender, int progress, String& comment);

class PACKAGE TPhVideoMarkTool : public TPhImageTool
{
private:
	bool m_down;
	int  m_si; // selected index
	int  m_sv; // selectes vertex
    int  m_selected;
	TRect* m_newRect;
	TPoint GetRectPoint(int index, TRect& rect);
	void __fastcall SetVertex(int x, int y);
	bool m_edited;
protected:
     TPopupMenu *PopupMenu;
     /*collection of frame items*/
	 TLFObjectList m_frames;
     /*collection of items on the current frame*/
	 TLFObjectList m_data;
     /*dictinary*/
     TList*        m_classes;
     TLFSemanticDictinary* m_dictinary;

	 double _2D_Dist(double x1,double y1,double x2,double y2);
	 bool   _is_near_vertex(int X, int Y, int& idx1, int& idx2);

	 String _get_label();
     TColor GetItemColor(TMarkItem* itm);

	 TLFObjectList* __fastcall GetData();
	 TLFObjectList* __fastcall GetFrames();

     void __fastcall DoPopup(int X, int Y);
     void __fastcall PopupClick(TObject* sender);

	TNotifyEvent 		m_OnChange;
	TNotifyEvent 		m_OnLoad;
	TPhAddDataEvent     m_OnAddData;
    TPhDelFrameEvent    m_OnDelFrame;
    TPhExportProgressEvent m_OnProgress;
protected:
	TPhMediaSource* m_mediaSource;
	virtual __fastcall SetMediaSource(TPhMediaSource* source);
	TLFSemanticDictinary* MakeDictinary();
    TFrameItem* GetFrameItem(int index);
public:
	__fastcall TPhVideoMarkTool(TComponent* Owner);
   virtual __fastcall ~TPhVideoMarkTool();

	virtual void Draw(TCanvas* Canvas);
	virtual void MouseDown(int X, int Y, TMouseButton Button = mbLeft);
	virtual void MouseUp(int X, int Y, TMouseButton Button = mbLeft);
	virtual void MouseMove(int X, int Y, TShiftState Shift);
	virtual void Reset();

	// Mark Tool Specific

	//methods
	void __fastcall SaveData();
	void __fastcall LoadData();
    void __fastcall SortData();

    // Default dictinary
    void __fastcall CreateDefaultDictinary();

	// add data to frames
	void __fastcall  NewFrame(bool keepData);
    // go to specific frame in the media source
	void __fastcall  SetFrame(int index);
    // delete frame data from frames
	void __fastcall  DeleteEntry(int index);
    //
    void __fastcall  DeleteObject(int index);
    // delete class label
    void __fastcall DeleteLabel(String label);
    // label statistics
    void __fastcall LabelStatistics(String label, int& numFrames, int& numEntries);
	void __fastcall  ExportToLFDatabase(const char* path);

    // show selected item
    void __fastcall  SetSelected(int index);

    //
    void __fastcall ChangeLabel(String& src_label, String& dst_label);

	//properties
	__property TPhMediaSource* MediaSource = {read = m_mediaSource, write = SetMediaSource};
	__property TLFObjectList*  data = {read = GetData};
	__property TLFObjectList*  frames = {read = GetFrames};
	__property TList* classes = {read = m_classes};
    __property TLFSemanticDictinary* dictinary = {read = m_dictinary};

	__property TNotifyEvent OnChange= {read = m_OnChange, write = m_OnChange};
	__property TNotifyEvent OnLoad  = {read = m_OnLoad, write = m_OnLoad};
	__property TPhAddDataEvent     OnAddData = {read = m_OnAddData, write = m_OnAddData};
	__property TPhDelFrameEvent    OnDelFrame = {read = m_OnDelFrame, write = m_OnDelFrame};
    __property TPhExportProgressEvent OnProgress = {read = m_OnProgress, write = m_OnProgress};

};
#endif
