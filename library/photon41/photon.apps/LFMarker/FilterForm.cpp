//---------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "FilterForm.h"
#include "MainForm.h"
//---------------------------------------------------------------------
#pragma link "CSPIN"
#pragma link "FImage41"
#pragma resource "*.dfm"
TFilterDlg *FilterDlg;
//---------------------------------------------------------------------
__fastcall TFilterDlg::TFilterDlg(TComponent* AOwner)
	: TForm(AOwner)
{
}
//---------------------------------------------------------------------

void __fastcall TFilterDlg::FormShow(TObject *Sender)
{
    _AWP_SAFE_RELEASE_(m_image)
    Form1->PhImage2->GetAwpImage(&m_image);

    PhImage1->SetAwpImage(m_image);
    PhImage1->BestFit();
}
//---------------------------------------------------------------------------

void __fastcall TFilterDlg::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    _AWP_SAFE_RELEASE_(m_image)
}
//---------------------------------------------------------------------------


void __fastcall TFilterDlg::ListBox1Click(TObject *Sender)
{
   awpImage* src = NULL;
   awpImage* dst = NULL;
   awpCopyImage(m_image, &src);
   awpCopyImage(m_image, &dst);

   awpFilter(src, dst,  ListBox1->ItemIndex);

   PhImage1->SetAwpImage(dst);
   PhImage1->BestFit();
   awpReleaseImage(&src);
   awpReleaseImage(&dst);
}
//---------------------------------------------------------------------------

