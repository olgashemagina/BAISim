//---------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "dictinaryEditor.h"
#include "dictinaryItemEditor.h"
#include "MainForm.h"
//---------------------------------------------------------------------
#pragma resource "*.dfm"
TdictinaryEditDlg *dictinaryEditDlg;
//---------------------------------------------------------------------
__fastcall TdictinaryEditDlg::TdictinaryEditDlg(TComponent* AOwner)
	: TForm(AOwner)
{

}
//---------------------------------------------------------------------
void __fastcall TdictinaryEditDlg::EditWordButtonClick(TObject *Sender)
{
	TLFSemanticDictinaryItem* item = NULL;
	if (ListBox1->ItemIndex >= 0)
	{
	   item = (TLFSemanticDictinaryItem*)m_dictionary.Get(ListBox1->ItemIndex);
	   std::string strOldName = item->GetItemLabel();
	   if (dictinaryItemDlg->EditItem(item))
	   {
			// edit item in the dictinary
			if (m_dictionary.CheckItem(item))
				ListBox1->Items->Strings[ListBox1->ItemIndex] = dictinaryItemDlg->Edit1->Text;
			else
			{
				UnicodeString str = item->GetItemLabel();
				Form1->Memo1->Lines->Add("ERROR: dublicate items ..." + str);
				item->SetItemLabel(strOldName.c_str());
			}
	   }
	}
}
//---------------------------------------------------------------------------
//
void __fastcall TdictinaryEditDlg::AddWordButtonClick(TObject *Sender)
{
	TLFSemanticDictinaryItem* item = new TLFSemanticDictinaryItem();
	if (dictinaryItemDlg->AddItem(item))
	{
		// add item to dictinary
		if (!m_dictionary.AddWordToDictinary(item))
		{
			UnicodeString str = item->GetItemLabel();
			Form1->Memo1->Lines->Add("ERROR: dublicate items ..." + str);
			delete item;
			return;
		}
		ListBox1->Items->Add(item->GetItemLabel());
	}
	else
		delete item;
}
//---------------------------------------------------------------------------

void __fastcall TdictinaryEditDlg::FormShow(TObject *Sender)
{
	//
	ListBox1->Clear();
	m_dictionary.Clear();
	if (Form1->m_db.Dictionary != NULL)
	{
		for (int i = 0; i < Form1->m_db.Dictionary->GetCount(); i++)
		{
			TLFSemanticDictinaryItem* mi = (TLFSemanticDictinaryItem*)Form1->m_db.Dictionary->Get(i);
			ListBox1->Items->Add(mi->GetItemLabel());
			TLFSemanticDictinaryItem* item = new TLFSemanticDictinaryItem(*mi);
			m_dictionary.Add(item);
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TdictinaryEditDlg::DeleteWordButtonClick(TObject *Sender)
{
    if (ListBox1->ItemIndex < 0)
    {
        ShowMessage("No class label selected for deletion.");
        return;
    }
    String del_label = this->ListBox1->Items->Strings[this->ListBox1->ItemIndex];
    String msg = L"Are you sure you want to delete all data associated with the class label ";
    msg += del_label;
    msg += L"? ";

    if (Application->MessageBoxW(msg.c_str(), Application->Name.c_str(), MB_YESNO  | MB_ICONQUESTION) == IDNO)
		return;
    ListBox1->Items->Delete(this->ListBox1->ItemIndex);
    AnsiString _astr = del_label;
    this->m_dictionary.DelWordForomDictinary(_astr.c_str());
}
//---------------------------------------------------------------------------
TLFSemanticDictinary* __fastcall TdictinaryEditDlg::GetSemanticDictionary()
{
    return &m_dictionary;
}
bool    __fastcall TdictinaryEditDlg::CreateDatabase()
{
	m_dictionary.Clear();
	return this->ShowModal() == mrOk;
}

bool    __fastcall TdictinaryEditDlg::EditDatabase()
{
	m_dictionary.Clear();
	return this->ShowModal() == mrOk;
}


