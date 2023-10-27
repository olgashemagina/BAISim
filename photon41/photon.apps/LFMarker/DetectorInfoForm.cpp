//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "DetectorInfoForm.h"
#include "MainForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "FImage41"
#pragma resource "*.dfm"
TDetectorForm *DetectorForm;
//---------------------------------------------------------------------------
__fastcall TDetectorForm::TDetectorForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TDetectorForm::FormShow(TObject *Sender)
{
//
	ILFDetectEngine* e = Form1->Engine;
	TSCObjectDetector* d = (TSCObjectDetector*)e->GetDetector(0);
   ComboBox2->Clear();
   ComboBox2->Items->Add(L"All");
   for (int i = 0; i < d->GetStagesCount(); i++)
   {
	   ComboBox2->Items->Add(L"Stage " + IntToStr(i));
   }
   ComboBox2->ItemIndex  = 0;
   DoDetectorInfo();

}
//---------------------------------------------------------------------------
void __fastcall TDetectorForm::DoDetectorInfo()
{
    Series1->Clear();
    Form1->Memo1->Lines->Add("Items count = " + IntToStr(Chart1->Series[0]->Count()));
	ILFDetectEngine* e = Form1->Engine;
	TSCObjectDetector* d = (TSCObjectDetector*)e->GetDetector(0);
	int w = d->GetBaseWidth()*10;
	int h = d->GetBaseHeight()*10;
	awpImage* img = NULL;
	awpCreateImage(&img, w, h, 1, AWP_DOUBLE);
	double* pix = (double*)img->pPixels;
    //Series1->ValuesList->Count = d->GetStagesCount();
	for (int i = 0; i < d->GetStagesCount(); i++)
	{
		switch(this->ComboBox1->ItemIndex)
		{
			case 0:
				  	Series1->AddY( d->GetSensorsCount(i));
			break;
			case 1:
				  	Chart1->Series[0]->AddY(d->GetStageThreshold(i));
			break;
			case 2:
				    Chart1->Series[0]->AddY(d->GetStageWeight(i));
			break;
		}
	}
    Series1->EndUpdate();

	TLFObjectList* strongs = d->GetStrongs();
	for (int i = 0; i < strongs->GetCount(); i++)
	{
		ILFStrong* s = (ILFStrong*)strongs->Get(i);
		if (ComboBox2->ItemIndex > 0 && ComboBox2->ItemIndex - 1 != i )
			continue;
		for (int j = 0; j < s->GetCount(); j++)
		{
			ILFWeak* ww = (ILFWeak*)s->GetWeak(j);
			ILFFeature* f = ww->Fetaure();
			awpRect r = f->GetRect();
			r.left *= 10;
			r.top *= 10;
			r.right *= 10;
			r.bottom *= 10;

			if (this->CheckBox2->Checked)
			{
				for (int y = r.top; y <= r.bottom; y++)
				{
					 for (int x = r.left; x <= r.right; x++)
					 {
						 if (this->CheckBox1->Checked)
							pix[x+y*w] += ww->Weight();
						 else
							pix[x+y*w] += 1;
					 }
				}
			}
			else
			{
				 TLFRect rect;
				 rect.SetRect(r);
				 awpPoint p = rect.Center();
				 awpRect r0;
				 r0.left = p.X-2 ;
				 r0.top  = p.Y-2 ;
				 r0.right = p.X+2;
				 r0.bottom = p.Y+2;
				 if (this->CheckBox1->Checked)
					awpDrawRect(img, &r0, 0, ww->Weight(), 1);
				 else
					awpDrawRect(img, &r0, 0, 1, 1);
			}
		}

	}
	awpConvert(img, AWP_CONVERT_TO_BYTE_WITH_NORM);
	PhImage1->SetAwpImage(img);
	PhImage1->BestFit();

	awpReleaseImage(&img);
}

void __fastcall TDetectorForm::ComboBox1Change(TObject *Sender)
{
    Series1->Clear();
	DoDetectorInfo();
}
//---------------------------------------------------------------------------

void __fastcall TDetectorForm::ComboBox2Change(TObject *Sender)
{
	DoDetectorInfo();
}
//---------------------------------------------------------------------------

void __fastcall TDetectorForm::CheckBox1Click(TObject *Sender)
{
	 DoDetectorInfo();
}
//---------------------------------------------------------------------------

void __fastcall TDetectorForm::CheckBox2Click(TObject *Sender)
{
	DoDetectorInfo();
}
//---------------------------------------------------------------------------

void __fastcall TDetectorForm::Button1Click(TObject *Sender)
{
    Series1->Clear();
}
//---------------------------------------------------------------------------

