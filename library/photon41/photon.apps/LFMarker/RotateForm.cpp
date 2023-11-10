//---------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "RotateForm.h"
#include "MainForm.h"
//---------------------------------------------------------------------
#pragma link "CSPIN"
#pragma link "FImage41"
#pragma resource "*.dfm"
TRotateDlg *RotateDlg;
//---------------------------------------------------------------------
__fastcall TRotateDlg::TRotateDlg(TComponent* AOwner)
	: TForm(AOwner)
{
}
//---------------------------------------------------------------------

void __fastcall TRotateDlg::FormShow(TObject *Sender)
{
    _AWP_SAFE_RELEASE_(m_image)
    Form1->PhImage2->GetAwpImage(&m_image);

    PhImage1->SetAwpImage(m_image);
    PhImage1->BestFit();
}
//---------------------------------------------------------------------------
void __fastcall TRotateDlg::CSpinEdit1Change(TObject *Sender)
{

   int angle = 0;

   try
   {
        angle = CSpinEdit1->Value;
   }
   catch(EConvertError& e)
   {
        return;
   }

   awpImage* pImage = NULL;
   awpCopyImage(m_image, &pImage);
   awpRotate(pImage, angle);
   PhImage1->SetAwpImage(pImage);
   PhImage1->BestFit();
   awpReleaseImage(&pImage);
}
//---------------------------------------------------------------------------

void __fastcall TRotateDlg::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    _AWP_SAFE_RELEASE_(m_image)
}
//---------------------------------------------------------------------------

