//---------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "dictinaryItemEditor.h"
//---------------------------------------------------------------------
#pragma resource "*.dfm"
TdictinaryItemDlg *dictinaryItemDlg;
//--------------------------------------------------------------------- 
__fastcall TdictinaryItemDlg::TdictinaryItemDlg(TComponent* AOwner)
	: TForm(AOwner)
{
}
//---------------------------------------------------------------------
static int ZoneTypeToComboIndex(TEZoneTypes z)
{
	switch(z)
	{
		case ZTContour:
			return 2;
		case ZTLineSegment:
			return 1;
		default:
			return 0;
	}
}

static  TEZoneTypes ComboIndexToZoneType(int index)
{
	switch(index)
	{
		case 1:
			return ZTLineSegment;
		case 2:
			return ZTContour;
		default:
			return ZTRect;
	}
}
bool __fastcall TdictinaryItemDlg::EditItem(TLFSemanticDictinaryItem* item)
{
    if (item == NULL )
    {
        return false;
    }

    Edit1->Text = item->GetItemLabel();
	int index = ColorGrid1->ColorToIndex(item->GetColor());
    ColorGrid1->ForegroundIndex = index;
	Panel1->Color = ColorGrid1->ForegroundColor;
	ILFScanner* s = item->GetScanner();
	SpinEdit1->Value = s->GetBaseWidth();
	SpinEdit2->Value = s->GetBaseHeight();
	ComboBox1->ItemIndex = ZoneTypeToComboIndex(item->GetZoneType());
	if (ShowModal() == mrOk)
	{
		item->SetColor(ColorGrid1->ForegroundColor);
		UnicodeString str = item->GetItemLabel();
		AnsiString _ansi = Edit1->Text;
		item->SetItemLabel(_ansi.c_str());
		item->SetZoneType(ComboIndexToZoneType(ComboBox1->ItemIndex));
		ILFScanner* s = item->GetScanner();
		assert(s);

		s->SetBaseHeight(SpinEdit2->Value);
		s->SetBaseWidth(SpinEdit1->Value);
        return true;
    }

    return false;
}
//
bool __fastcall TdictinaryItemDlg::AddItem(TLFSemanticDictinaryItem* item)
{
    if (ShowModal() == mrOk)
    {
		AnsiString _ansi = Edit1->Text;
		item->SetItemLabel(_ansi.c_str());
		item->SetColor(ColorGrid1->ForegroundColor);
		// setup scanner
		ILFScanner* s = item->GetScanner();
		assert(s);
        item->SetZoneType(ComboIndexToZoneType(ComboBox1->ItemIndex));
		s->SetBaseHeight(SpinEdit2->Value);
		s->SetBaseWidth(SpinEdit1->Value);
        return true;
    }
    return false;
}

void __fastcall TdictinaryItemDlg::ColorGrid1Click(TObject *Sender)
{
    Panel1->Color = this->ColorGrid1->ForegroundColor;
}
//---------------------------------------------------------------------------


