//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ImageViewForm.h"
#include "MainForm.h"
#include "TableViewForm.h"
#include "_LF.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "FImage41"
#pragma resource "*.dfm"
TFragmentForm *FragmentForm;
//---------------------------------------------------------------------------
__fastcall TFragmentForm::TFragmentForm(TComponent* Owner)
    : TForm(Owner)
{
	m_descr = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TFragmentForm::DrawSelected()
{
    if (!Visible)
  		return;
	TLFDetectedItem* di = Form1->m_Descr.GetDetectedItem(Form1->SelectedIndex);
    if (di == NULL)
    {
        PhImage1->Close();
		PhImage1->Paint();
		return;
	}
	TLFRect* rr = di->GetBounds();
	awpRect  r  = rr->GetRect();

	awpImage* pImage = NULL;
	awpImage* pFragment = NULL;

	Form1->PhImage2->GetAwpImage(&pImage);
	if (pImage == NULL)
	{
		PhImage1->Close();
		PhImage1->Paint();
		return;
	}

	if (this->SpeedButton2->Down)
	{
        TLFScanner scanner;
        TLFImage   lfimg;
        scanner.SetBaseWidth(di->GetBW());
        scanner.SetBaseHeight(di->GetBH());
        lfimg.SetImage(pImage);
        scanner.Scan(lfimg.GetImage()->sSizeX, lfimg.GetImage()->sSizeY);

 		float maxOverlap = 0;
		TLFRect maxRect;
		TLFRect* bounds = di->GetBounds();

		for (int j = 0; j < scanner.GetFragmentsCount(); j++)
		{
		  awpRect r = scanner.GetFragmentRect(j);
		  TLFRect scanBox; scanBox.SetRect(r);
		  float overlap = bounds->RectOverlap(scanBox);
		  if (overlap > maxOverlap)
		  {
			  maxOverlap = overlap;
			  maxRect = scanBox;
		  }
		}
		r = maxRect.GetRect();
	}
	awpCopyRect(pImage, &pFragment, &r);
    PhImage1->SetAwpImage(pFragment);
	if (this->SpeedButton1->Down)
		PhImage1->BestFit();
	else
		PhImage1->ActualSize();
	awpReleaseImage(&pFragment);
	awpReleaseImage(&pImage);

    ///////////////////////////////////////////////
    Edit1->Text = di->GetDetectorName();
    TLFSemanticDictinary* dict = Form1->m_db.Dictionary;
    std::string str_uuid = di->GetType();
    std::string str_label = dict->GetWordByUUID(str_uuid.c_str());
    int index = ComboBox1->Items->IndexOf(str_label.c_str());
    if (index >= 0)
    {
     	ComboBox1->ItemIndex = index;
    }
	ComboBox2->ItemIndex = di->GetRacurs();
	Edit2->Text = di->GetComment().c_str();
}
void __fastcall TFragmentForm::SpeedButton1Click(TObject *Sender)
{
	DrawSelected();
}
//---------------------------------------------------------------------------

void __fastcall TFragmentForm::SpeedButton2Click(TObject *Sender)
{
	DrawSelected();
}
//---------------------------------------------------------------------------
void __fastcall TFragmentForm::CheckBox1Click(TObject *Sender)
{
	DrawSelected();
}
//---------------------------------------------------------------------------
void __fastcall TFragmentForm::FormShow(TObject *Sender)
{
	ChangeDictonary();
}
//---------------------------------------------------------------------------

void __fastcall TFragmentForm::ComboBox1Change(TObject *Sender)
{
/*	AnsiString strLabel = ComboBox1->Text;
	AnsiString strComment = Edit2->Text;
	TLFSemanticDictinary* dict = Form1->m_db.Dictionary;
	TLFSemanticDictinaryItem* sdi = dict->GetWordFromDictinary(strLabel.c_str());
	if (this->m_descr != NULL)
	{
		TLFDetectedItem* item = m_descr->GetDetectedItem(Form1->SelectedIndex);
		if (item != NULL)
		{
			 item->SetType(sdi->GetId().c_str());
			 TableForm->ChangeItem(Form1->SelectedIndex, strLabel.c_str(), 0, 0, strComment.c_str());
		}
	}*/

    UpdateTable();
}
//---------------------------------------------------------------------------

void __fastcall TFragmentForm::ApplicationEvents1Idle(TObject *Sender, bool &Done)

{
	SpeedButton3->Enabled = this->m_descr != NULL && this->m_descr->GetCount() > 1;
	SpeedButton4->Enabled = this->m_descr != NULL && this->m_descr->GetCount() > 1;
	SpeedButton5->Enabled = this->m_descr != NULL && this->m_descr->GetCount() > 1;
	SpeedButton6->Enabled = this->m_descr != NULL && this->m_descr->GetCount() > 1;
}
//---------------------------------------------------------------------------
void __fastcall TFragmentForm::ChangeDictonary()
{
	ComboBox1->Items->Clear();
    for (int i = 0; i < Form1->m_db.Dictionary->GetCount(); i++)
    {
        TLFSemanticDictinaryItem* word = Form1->m_db.Dictionary->GetWordFromDictinary(i);
		if (word != NULL)
        {
			ComboBox1->Items->Add(word->GetItemLabel());
        }
    }
    ComboBox1->ItemIndex = 0;
}

void __fastcall TFragmentForm::SpeedButton3Click(TObject *Sender)
{
	Form1->SelectedIndex = 0;
    TableForm->ListView1->ItemIndex = 0;
    this->DrawSelected();
}
//---------------------------------------------------------------------------

void __fastcall TFragmentForm::SpeedButton4Click(TObject *Sender)
{
	int idx = Form1->SelectedIndex;
    idx--;
    if (idx >= 0)
    {
		Form1->SelectedIndex = idx;
    }

    this->DrawSelected();
}
//---------------------------------------------------------------------------

void __fastcall TFragmentForm::SpeedButton5Click(TObject *Sender)
{
	int idx = Form1->SelectedIndex;
    idx++;
    if (idx <  Form1->m_Descr.GetCount())
    {
		Form1->SelectedIndex = idx;
    }

    this->DrawSelected();
}
//---------------------------------------------------------------------------

void __fastcall TFragmentForm::SpeedButton6Click(TObject *Sender)
{
	Form1->SelectedIndex = Form1->m_Descr.GetCount() - 1;
    this->DrawSelected();
}
//---------------------------------------------------------------------------



void __fastcall TFragmentForm::ComboBox1Exit(TObject *Sender)
{
	if (this->m_descr != NULL)
	{
		TLFDetectedItem* item = m_descr->GetDetectedItem(Form1->SelectedIndex);
		if (item != NULL)
		{
			 AnsiString _ansi = Edit2->Text;
			 item->SetComment(_ansi.c_str());
			 //TableForm->ChangeItem(Form1->SelectedIndex, strLabel.c_str());
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TFragmentForm::SpeedButton7Click(TObject *Sender)
{
	TLFDetectedItem* di = Form1->m_Descr.GetDetectedItem(Form1->SelectedIndex);
    //
    AnsiString _ansi = ComboBox1->Text;
    TLFSemanticDictinary* dict = Form1->m_db.Dictionary;
    TLFSemanticDictinaryItem* sdi = dict->GetWordFromDictinary(_ansi.c_str());

	di->SetType(sdi->GetId().c_str());
	_ansi = Edit2->Text;
	di->SetComment(_ansi.c_str());
    di->SetAngle(SpinEdit1->Value);
	di->SetRacurs(ComboBox2->ItemIndex);
    Form1->DbSaveMarkActionExecute(NULL);
}
//---------------------------------------------------------------------------


void __fastcall TFragmentForm::Edit2Change(TObject *Sender)
{
	UpdateTable();
}
//---------------------------------------------------------------------------
void __fastcall TFragmentForm::UpdateTable()
{
	AnsiString strLabel = ComboBox1->Text;
	AnsiString strComment = Edit2->Text;
	TLFSemanticDictinary* dict = Form1->m_db.Dictionary;
	TLFSemanticDictinaryItem* sdi = dict->GetWordFromDictinary(strLabel.c_str());
	if (this->m_descr != NULL)
	{
		TLFDetectedItem* item = m_descr->GetDetectedItem(Form1->SelectedIndex);
		if (item != NULL)
		{
			 item->SetType(sdi->GetId().c_str());
			 TableForm->ChangeItem(Form1->SelectedIndex, strLabel.c_str(), ComboBox2->ItemIndex, SpinEdit1->Value, strComment.c_str());
		}
	}
}

void __fastcall TFragmentForm::ComboBox2Change(TObject *Sender)
{
	UpdateTable();
}
//---------------------------------------------------------------------------

void __fastcall TFragmentForm::SpinEdit1Change(TObject *Sender)
{
	UpdateTable();
}
//---------------------------------------------------------------------------

