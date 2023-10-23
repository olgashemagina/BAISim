//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Hst2DWindow.h"
#include "mainUnit.h"
#include "Histogramm2DUnit.h"
#include "errorUnit.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "FImage"
#pragma resource "*.dfm"
THsv2DForm *Hsv2DForm;
//---------------------------------------------------------------------------
__fastcall THsv2DForm::THsv2DForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall THsv2DForm::FormShow(TObject *Sender)
{
	UpdatePicture();
}
//---------------------------------------------------------------------------
void __fastcall THsv2DForm::UpdatePicture()
{
    m_2Dhst.FreeImages();
  	if (viewSourceHSTAction->Checked)
    {
    	this->RenederSource();
    }
    else if (viewSourceCondHSTAction->Checked)
    {
         this->RnderSourceCond();
    }
    else if (viewOutsideAction->Checked)
    {
    	this->RenderOutsideSource();
    }
    else if (viewCondOutsideAction->Checked)
    {
    	this->RenderOutsideCond();
    }
    else if (viewPredictorAction->Checked)
    {
        this->RenderPredictor();
    }

    if (m_2Dhst.GetImage() == NULL)
    {
        FImage1->Bitmap->Assign(NULL);
        FImage1->Paint();
    	return;
    }
	awpImage* img = NULL;
    awpCopyImage(m_2Dhst.GetImage(), &img);

    awpConvert(img, AWP_CONVERT_TO_BYTE_WITH_NORM);
    FImage1->Bitmap->SetAWPImage(img);
    _AWP_SAFE_RELEASE_(img)
    FImage1->BestFit();

    mainForm->FImage1->Bitmap->GetAWPImage(&m_src);
}

void THsv2DForm::ProcessImage()
{
     awpImage* img = m_model.Predict(m_src,  double(SpinEdit1->Value)); //MakeMask(m_src, m_2Dhst.GetImage(), double(SpinEdit1->Value));
     if (img != NULL)
     {
            AWPBYTE* src_pix = _AWP_BPIX_(m_src, AWPBYTE)
            AWPBYTE* img_pix = _AWP_BPIX_(img, AWPBYTE)

            if (!CheckBox2->Checked)
		         mainForm->FImage2->Bitmap->SetAWPImage(img);
            else
            {
                  for (int i = 0; i < img->sSizeX*img->sSizeY; i++)
                  {
                  		if (img_pix[i] > 0)
                        {
                     		src_pix[3*i] = 255;
                        }
                  }
                  mainForm->FImage2->Bitmap->SetAWPImage(m_src);
            }
         mainForm->FImage2->BestFit();
	     _AWP_SAFE_RELEASE_(img)

     }
}

void __fastcall THsv2DForm::SetImage(awpImage* img)
{
 	awpCopyImage(img, &m_src);
}

void __fastcall THsv2DForm::CheckBox2Click(TObject *Sender)
{
   	hsPredictActionExecute(NULL);
}
//---------------------------------------------------------------------------

void __fastcall THsv2DForm::hsNewModelActionExecute(TObject *Sender)
{
	m_model.Clear();
    UpdatePicture();
}
//---------------------------------------------------------------------------

void __fastcall THsv2DForm::hsSaveModelActionExecute(TObject *Sender)
{
	if (SaveDialog1->Execute())
    {
    	// save hs histogramm
        AnsiString _ansi =  SaveDialog1->FileName;
        string str = _ansi.c_str();
        if (!m_model.Save(str))
        {
			ShowMessage("Не могу сохранить модель в файл: " + SaveDialog1->FileName);
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall THsv2DForm::hsSaveModelActionUpdate(TObject *Sender)
{
	hsSaveModelAction->Enabled = m_model.ImagePredictor() != NULL;
}
//---------------------------------------------------------------------------

void __fastcall THsv2DForm::hsLoadModelActionExecute(TObject *Sender)
{
	if (OpenDialog1->Execute())
    {
    	// Load HS histogramm
        AnsiString _ansi =  OpenDialog1->FileName;
		std::string str = _ansi.c_str();
        if (m_model.Load(str))
        {
	        UpdatePicture();
        }
        else
			ShowMessage("Не могу загрузить модель из файла: " + OpenDialog1->FileName);
    }
    UpdatePicture();
}
//---------------------------------------------------------------------------
void __fastcall THsv2DForm::hsBuildModelActionExecute(TObject *Sender)
{
//	ShowMessage(IntToStr(mainForm->FImage1->FileNames->Count));
    TLFStrings names;
    for (int i = 0; i < mainForm->FImage1->FileNames->Count; i++)
    {
    	AnsiString _ansi = mainForm->FImage1->FileNames->Strings[i];
        names.push_back(_ansi.c_str());
    }

    if (!m_model.BuildModel(names))
    {
    	  ShowMessage("не могу построить модель.");
    }

    this->UpdatePicture();
}
//---------------------------------------------------------------------------

void __fastcall THsv2DForm::hsBuildModelActionUpdate(TObject *Sender)
{
	hsBuildModelAction->Enabled = mainForm->FImage1->SourceType == vsSlideShow;
}
//---------------------------------------------------------------------------

void __fastcall THsv2DForm::hsEstErrorActionExecute(TObject *Sender)
{
    TLFStrings names;
    for (int i = 0; i < mainForm->FImage1->FileNames->Count; i++)
    {
    	AnsiString _ansi = mainForm->FImage1->FileNames->Strings[i];
        names.push_back(_ansi.c_str());
    }

    errorWindow->EstimateError(names, &m_model);
	errorWindow->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall THsv2DForm::hsEstErrorActionUpdate(TObject *Sender)
{
	hsEstErrorAction->Enabled = this->m_model.ImagePredictor() != NULL;
}
//---------------------------------------------------------------------------

void __fastcall THsv2DForm::hsPredictActionExecute(TObject *Sender)
{
    // apply hs histogramm
	awpImage* img = NULL;
    mainForm->FImage1->Bitmap->GetAWPImage(&img);
    awpCopyImage(img, &m_src);
	this->ProcessImage();
}
//---------------------------------------------------------------------------

void __fastcall THsv2DForm::hsPredictActionUpdate(TObject *Sender)
{
	hsPredictAction->Enabled = m_model.ImagePredictor() != NULL;
}
//---------------------------------------------------------------------------
void __fastcall THsv2DForm::FormHide(TObject *Sender)
{
   	mainForm->FImage2->Bitmap->Assign(NULL);
	mainForm->OnResize(NULL);
}
//---------------------------------------------------------------------------
void __fastcall THsv2DForm::viewSourceHSTActionExecute(TObject *Sender)
{
	this->UpdatePicture();
}
//---------------------------------------------------------------------------

void __fastcall THsv2DForm::viewSourceHSTActionUpdate(TObject *Sender)
{
	viewSourceHSTAction->Enabled = m_model.ImageClass1() != NULL;
}
//---------------------------------------------------------------------------

void __fastcall THsv2DForm::viewSourceCondHSTActionExecute(TObject *Sender)
{
    this->UpdatePicture();
}
//---------------------------------------------------------------------------

void __fastcall THsv2DForm::viewSourceCondHSTActionUpdate(TObject *Sender)
{
	viewSourceCondHSTAction->Enabled = m_model.ImageClass1() != NULL;
}
//---------------------------------------------------------------------------

void __fastcall THsv2DForm::viewOutsideActionExecute(TObject *Sender)
{
	this->UpdatePicture();
}
//---------------------------------------------------------------------------

void __fastcall THsv2DForm::viewOutsideActionUpdate(TObject *Sender)
{
	viewOutsideAction->Enabled = m_model.ImageClass0() != NULL;
}
//---------------------------------------------------------------------------

void __fastcall THsv2DForm::viewCondOutsideActionExecute(TObject *Sender)
{
	this->UpdatePicture();
}
//---------------------------------------------------------------------------

void __fastcall THsv2DForm::viewCondOutsideActionUpdate(TObject *Sender)
{
	viewCondOutsideAction->Enabled = m_model.ImageClass0() != NULL;
}
//---------------------------------------------------------------------------

void __fastcall THsv2DForm::viewPredictorActionExecute(TObject *Sender)
{
	this->UpdatePicture();
}
//---------------------------------------------------------------------------

void __fastcall THsv2DForm::viewPredictorActionUpdate(TObject *Sender)
{
	viewPredictorAction->Enabled = m_model.ImagePredictor() != NULL;
}
//---------------------------------------------------------------------------
void __fastcall THsv2DForm::RenederSource()
{
    if (m_model.ImageClass1() == NULL)
    	return;
    awpImage* img = NULL;
    awpCopyImage(m_model.ImageClass1(), &img);
    this->m_2Dhst.SetImage(img);
     _AWP_SAFE_RELEASE_(img)
}
void __fastcall THsv2DForm::RnderSourceCond()
{
    if (m_model.ImageClass1() == NULL)
    	return;
    awpImage* img = NULL;
    awpCopyImage(m_model.ImageClass1(), &img);
    AWPDOUBLE* img_pix = _AWP_BPIX_(img, AWPDOUBLE)
    AWPDOUBLE* hst_pix = _AWP_BPIX_(m_model.ImageClass1(), AWPDOUBLE)
    for (int i = 0; i < img->sSizeX*img->sSizeY; i++)
    {
        if (hst_pix[i] > this->SpinEdit1->Value)
        {
        	img_pix[i] = 255;
        }
        else
        {
			img_pix[i] = 0;
        }
    }
    this->m_2Dhst.SetImage(img);
     _AWP_SAFE_RELEASE_(img)
}
void __fastcall THsv2DForm::RenderOutsideSource()
{
    if (m_model.ImageClass0() == NULL)
    	return;
    awpImage* img = NULL;
    awpCopyImage(m_model.ImageClass0(), &img);
    this->m_2Dhst.SetImage(img);
     _AWP_SAFE_RELEASE_(img)
}
void __fastcall THsv2DForm::RenderOutsideCond()
{
    if (m_model.ImageClass0() == NULL)
    	return;
    awpImage* img = NULL;
    awpCopyImage(m_model.ImageClass0(), &img);
    AWPDOUBLE* img_pix = _AWP_BPIX_(img, AWPDOUBLE)
    AWPDOUBLE* hst_pix = _AWP_BPIX_(m_model.ImageClass0(), AWPDOUBLE)
    for (int i = 0; i < img->sSizeX*img->sSizeY; i++)
    {
        if (hst_pix[i] > this->SpinEdit1->Value)
        {
        	img_pix[i] = 255;
        }
        else
        {
			img_pix[i] = 0;
        }
    }
    this->m_2Dhst.SetImage(img);
     _AWP_SAFE_RELEASE_(img)

}
void __fastcall THsv2DForm::RenderPredictor()
{
    if (m_model.ImagePredictor() == NULL)
        return;
    awpImage* img = NULL;
    awpCopyImage(m_model.ImagePredictor(), &img);
    this->m_2Dhst.SetImage(img);
    _AWP_SAFE_RELEASE_(img)
}

void __fastcall THsv2DForm::SpinEdit1Change(TObject *Sender)
{
	this->UpdatePicture();
}
//---------------------------------------------------------------------------

