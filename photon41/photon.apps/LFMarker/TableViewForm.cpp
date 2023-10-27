//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TableViewForm.h"
#include "MainForm.h"
#include "ImageViewForm.h"
#include "_LF.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TTableForm *TableForm;
static AnsiString RacursToStringHepler(int racurs)
{
	switch (racurs)
    {
		case 0:
            return "Front";
        case 1:
            return "Left semi-profile";
        case 2:
            return "Left profile";
        case 3:
            return "Left back semi-profile";
        case 4:
            return "Back";
        case 5:
            return "Right back semi-profile";
        case 6:
            return "Right profile";
        case 7:
            return "Right semi-profile";
        case 8:
            return "Bottom front";
        case 9:
            return "Bottom left semi-propfile";
        case 10:
            return "Bottom left profile";
        case 11:
            return "Bottom back left semi-profile";
        case 12:
            return "Bottom back";
        case 13:
            return "Bottom back right semi profile";
        case 14:
            return "Bottom right profile";
		case 15:
            return "Bottom right semi propfile";
        case 16:
            return "Bottom";
        case 17:
            return "Up front";
        case 18:
            return "Up left semi profile";
        case 19:
            return "Up left profile";
        case 20:
            return "Up left back semi profile";
        case 21:
            return "Up back";
        case 22:
            return "Up back right semi profile";
        case 23:
            return "Up right profile";
        case 24:
            return "Up right semi profile";
        case 25:
            return "Up";
    }
}

//---------------------------------------------------------------------------
__fastcall TTableForm::TTableForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TTableForm::FormShow(TObject *Sender)
{
    this->ListView1->Columns->Clear();
    TListColumn  *NewColumn;
    TListItem  *ListItem;

	NewColumn = ListView1->Columns->Add();
    NewColumn->Caption = "ID";

	NewColumn = ListView1->Columns->Add();
	NewColumn->Caption = "Detector";

	NewColumn = ListView1->Columns->Add();
    NewColumn->Caption = "Class label";

	NewColumn = ListView1->Columns->Add();
	NewColumn->Caption = "Racurs";

	NewColumn = ListView1->Columns->Add();
	NewColumn->Caption = "Angle";

	NewColumn = ListView1->Columns->Add();
    NewColumn->Caption = "left";

	NewColumn = ListView1->Columns->Add();
    NewColumn->Caption = "top";

	NewColumn = ListView1->Columns->Add();
    NewColumn->Caption = "width";

	NewColumn = ListView1->Columns->Add();
	NewColumn->Caption = "height";

	NewColumn = ListView1->Columns->Add();
	NewColumn->Caption = "Comment";

    this->UpdateTable();
}
//---------------------------------------------------------------------------
void __fastcall TTableForm::sbDeleteRowClick(TObject *Sender)
{
   // delete object from descriptos and save descriptor on the disk.
   Form1->m_Descr.DeleteDetectedItem(Form1->SelectedIndex);
   AnsiString str =  ChangeFileExt(Form1->m_strFileName, ".xml");
   Form1->m_Descr.SaveXML(str.c_str());
   // update list view
   ListView1->DeleteSelected();
   TPhImageMarkTool* tool = dynamic_cast< TPhImageMarkTool*>(Form1->PhImage2->PhTool);
   if (tool != NULL)
     tool->SetFrame(str.c_str());
   // update DbView
   TListItem* li = Form1->DbView->Items->Item[Form1->DbView->ItemIndex];
   li->SubItems->Strings[0] = IntToStr(Form1->m_Descr.GetCount());
}
//---------------------------------------------------------------------------
void __fastcall TTableForm::sbClearClick(TObject *Sender)
{

	Form1->m_Descr.Clear();
	AnsiString str =  ChangeFileExt(Form1->m_strFileName, ".xml");
	Form1->m_Descr.SaveXML(str.c_str());
	ListView1->Clear();
	TPhImageMarkTool* tool = dynamic_cast< TPhImageMarkTool*>(Form1->PhImage2->PhTool);
	if (tool != NULL)
	  tool->SetFrame(str.c_str());
	// update DbView
	TListItem* li = Form1->DbView->Items->Item[Form1->DbView->ItemIndex];
	li->SubItems->Strings[0] = IntToStr(Form1->m_Descr.GetCount());
}
//---------------------------------------------------------------------------
void __fastcall TTableForm::AddNewItem(TLFDetectedItem* di)
{
	TLFSemanticDictinary* dict = Form1->m_db.Dictionary;

	TListItem  *ListItem;
	ListItem = ListView1->Items->Add();
	ListItem->Caption = IntToStr(Form1->m_Descr.GetItemsCount()-1);

	ListItem->SubItems->Add(di->GetDetectorName());
	//
	std::string str_uuid = di->GetType();
	std::string str_label = dict->GetWordByUUID(str_uuid.c_str());
	ListItem->SubItems->Add(str_label.c_str());
	ListItem->SubItems->Add(RacursToStringHepler(di->GetRacurs()));
	ListItem->SubItems->Add(IntToStr(di->GetAngle()));

	TLFRect* rr = di->GetBounds();
	awpRect r = rr->GetRect();
	ListItem->SubItems->Add(IntToStr(r.left));
	ListItem->SubItems->Add(IntToStr(r.top));
	ListItem->SubItems->Add(IntToStr(r.right - r.left));
	ListItem->SubItems->Add(IntToStr(r.bottom - r.top));
	ListItem->SubItems->Add(di->GetComment().c_str());

}
void __fastcall TTableForm::ListView1Change(TObject *Sender,
	  TListItem *Item, TItemChange Change)
{
	TPhImageMarkTool* tool = dynamic_cast< TPhImageMarkTool*>(Form1->PhImage2->PhTool);
	if (tool != NULL)
	{
		if (ListView1->Selected != NULL)
		{
		 tool->SetSelected(Item->Index);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TTableForm::UpdateTable()
{
	if (!Visible)
		return;
	ListView1->Items->Clear();
	TListColumn  *NewColumn;
	TListItem  *ListItem;

	TLFSemanticDictinary* dict = Form1->m_db.Dictionary;
	for (int i = 0; i < Form1->m_Descr.GetItemsCount(); i++)
	{

	  TLFDetectedItem* di = Form1->m_Descr.GetDetectedItem(i);
	  ListItem = ListView1->Items->Add();
	  ListItem->Caption = IntToStr(i);
	  ListItem->SubItems->Add(di->GetDetectorName());
	  //----------------
	  std::string str_uuid = di->GetType();
	  std::string str_label = dict->GetWordByUUID(str_uuid.c_str());
	  ListItem->SubItems->Add(str_label.c_str());
	  ListItem->SubItems->Add(RacursToStringHepler(di->GetRacurs()));
	  ListItem->SubItems->Add(IntToStr(di->GetAngle()));
	  TLFRect* rr = di->GetBounds();
	  awpRect r = rr->GetRect();

	  ListItem->SubItems->Add(IntToStr(r.left));
	  ListItem->SubItems->Add(IntToStr(r.top));
	  ListItem->SubItems->Add(IntToStr(r.right - r.left));
	  ListItem->SubItems->Add(IntToStr(r.bottom - r.top));
	  ListItem->SubItems->Add(di->GetComment().c_str());
	}

	if (Form1->m_Descr.GetItemsCount() > 0)
	{
	   ListView1->Selected =   ListView1->Items->Item[0];
	   Form1->SelectedIndex = 0;
	}

	FragmentForm->DrawSelected();
}


void __fastcall TTableForm::FragmentViewActionExecute(TObject *Sender)
{
	FragmentForm->Visible = !FragmentForm->Visible;
	if (FragmentForm->Visible)
	   FragmentForm->DrawSelected();
}
//---------------------------------------------------------------------------

void __fastcall TTableForm::FragmentViewActionUpdate(TObject *Sender)
{
    FragmentViewAction->Checked =  FragmentForm->Visible;
}
//---------------------------------------------------------------------------

void __fastcall TTableForm::ListView1SelectItem(TObject *Sender, TListItem *Item,
          bool Selected)
{
	if (!Visible)
    	return;
    if (ListView1->Selected != NULL)
    {
	 Form1->SelectedIndex = Item->Index;
     FragmentForm->DrawSelected();
    }
}
//---------------------------------------------------------------------------
void __fastcall TTableForm::ChangeItem(int index, const char* value, int racurs, int angle, const char* comment)
{
	TListItem  *ListItem = this->ListView1->Items->Item[index];
	if (ListItem != NULL)
	{
		ListItem->SubItems->Strings[1] = value;
		ListItem->SubItems->Strings[2] = RacursToStringHepler(racurs);
		ListItem->SubItems->Strings[3] = IntToStr(angle);

		if (comment != NULL)
			ListItem->SubItems->Strings[8] = comment;
    }
}

void __fastcall TTableForm::ListView1DblClick(TObject *Sender)
{
	FragmentForm->Visible = true;
    FragmentForm->DrawSelected();
}
//---------------------------------------------------------------------------

