//---------------------------------------------------------------------------

#ifndef PhZonesToolH
#define PhZonesToolH
//---------------------------------------------------------------------------
#include "PhImageTool.h"
#include "_LF.h"

typedef void __fastcall (__closure *TPhAddRoiEvent)(System::TObject* Sender, TLFZone* item);
typedef void __fastcall (__closure *TPhChangeRoiEvent)(System::TObject* Sender, int index, bool update);

class PACKAGE TPhZonesTool : public TPhImageTool
{
private:
	bool m_down;
	int  m_si; // selected index
	int  m_sv; // selectes vertex
	int  m_selected;
    bool m_CanAddZone;
	TLFZone* m_newZone;
    TEZoneTypes m_mode;
	TPhAddRoiEvent      m_OnAddRoi;
	TPhChangeRoiEvent   m_OnChangeRoi;
protected:
	TLFZones 		m_rois;
    void __fastcall AddNewRoi();
	awp2DPoint __fastcall Get2DPoint(int X, int Y);
	double _2D_Dist(double x1,double y1,double x2,double y2);
	bool   _is_near_vertex(int X, int Y, int& idx1, int& idx2);
	TPoint GetRectPoint(int index, TRect& rect);
	void __fastcall SetVertex(int x, int y, bool update);
	TRect __fastcall GetBoundsRect(TLFZone* zone);
	int __fastcall GetRoiCount();
	void __fastcall SetSelected(int value);
public:
	__fastcall TPhZonesTool(TComponent* Owner);
   virtual __fastcall ~TPhZonesTool();

	virtual void Draw(TCanvas* Canvas);
	virtual void MouseDown(int X, int Y, TMouseButton Button = mbLeft);
	virtual void MouseUp(int X, int Y, TMouseButton Button = mbLeft);
	virtual void MouseMove(int X, int Y, TShiftState Shift);
	virtual void Reset();

	// сохрание roi в файл
	bool __fastcall SaveZones(const UnicodeString fileName);
	// загрузка roi из файла
	bool __fastcall LoadZones(const UnicodeString fileName);


__property TPhAddRoiEvent     OnAddRoi = {read = m_OnAddRoi, write = m_OnAddRoi};
__property TPhChangeRoiEvent  OnChangeRoi = {read = m_OnChangeRoi, write = m_OnChangeRoi};
};

#endif
