//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop


#include "BuildDetectorOptionsForm.h"
#include "SelectBuilderConfigForm.h"
#include "SelDirUnit.h"
#include "MainForm.h"
#include "LFFileUtils.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//#pragma link "cspin"
#pragma resource "*.dfm"
TCSBuildOptions *CSBuildOptions;
//---------------------------------------------------------------------------
__fastcall TCSBuildOptions::TCSBuildOptions(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TCSBuildOptions::Sel_bgrClick(TObject *Sender)
{
	String str = "";
	if (GetDirNamePreview(str))
	{
		this->bgr->Text = str;
	}
}
//---------------------------------------------------------------------------
void __fastcall TCSBuildOptions::negClick(TObject *Sender)
{
    String str = "";
	if (GetDirNamePreview(str))
	{
		this->neg->Text = str;
	}
}
//---------------------------------------------------------------------------
void __fastcall TCSBuildOptions::DatabaseClick(TObject *Sender)
{
	String str = pos->Text;
	if (GetDirNamePreview(str))
	{
		this->pos->Text = str;
	}
}
//---------------------------------------------------------------------------

void __fastcall TCSBuildOptions::NextButtonClick(TObject *Sender)
{
	if(NextButton->Caption == "Save && start")
	{
		if(SaveDialog1->Execute())
		{
			AnsiString tmpname = SaveDialog1->FileName;
			AnsiString tmp_value;
			TiXmlDocument doc;
			TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "utf-8", "" );
			doc.LinkEndChild( decl );
			TiXmlElement* de = new TiXmlElement("BuildDetector");
			double q = CSpinEdit9->Value/100.;

            TFormatSettings fmt = TFormatSettings::Create();
			fmt.DecimalSeparator = '.';
			tmp_value = FloatToStrF(q, ffFixed, 2, 1, fmt);

			de->SetAttribute("finish_far", tmp_value.c_str());
			tmp_value = IntToStr(int(CSpinEdit6->Value));
			de->SetAttribute("num_negative", tmp_value.c_str());
			tmp_value = IntToStr(int(CSpinEdit8->Value));
			de->SetAttribute("max_stages", tmp_value.c_str());
			tmp_value = Edit1->Text;
			de->SetAttribute("detector_name", tmp_value.c_str());
			tmp_value = IntToStr(int(CSpinEdit2->Value));
			de->SetAttribute("base_width", tmp_value.c_str());
			tmp_value = IntToStr(int(H_CSpinEdit->Value));
			de->SetAttribute("base_height", tmp_value.c_str());
			tmp_value = IntToStr(int(CSpinEdit7->Value));
			de->SetAttribute("min_negative", tmp_value.c_str());
			tmp_value = IntToStr(int(CSpinEdit3->Value));
			de->SetAttribute("num_samples_per_image", tmp_value.c_str());
			tmp_value = pos->Text;
			de->SetAttribute("positive_examples", tmp_value.c_str());
			tmp_value = neg->Text;
			de->SetAttribute("negative_examples", tmp_value.c_str());
			tmp_value = bgr->Text;
			de->SetAttribute("bkground_base", tmp_value.c_str());
			tmp_value = Edit2->Text;
			de->SetAttribute("log_name", tmp_value.c_str());


			TiXmlElement* fe = new TiXmlElement("Features");
			if(RadioGroup1->Items->IndexOf("TLFHFeature") != RadioGroup1->ItemIndex)
				fe->SetAttribute("TLFHFeature", "0");
			else
				fe->SetAttribute("TLFHFeature", "1");

			if(RadioGroup1->Items->IndexOf("TLFDAFeature") != RadioGroup1->ItemIndex)
				fe->SetAttribute("TLFDAFeature", "0");
			else
				fe->SetAttribute("TLFDAFeature", "1");

			if(RadioGroup1->Items->IndexOf("TLFHAFeature") != RadioGroup1->ItemIndex)
				fe->SetAttribute("TLFHAFeature", "0");
			else
				fe->SetAttribute("TLFHAFeature", "1");

			if(RadioGroup1->Items->IndexOf("TLFCAFeature") != RadioGroup1->ItemIndex)
				fe->SetAttribute("TLFCAFeature", "0");
			else
				fe->SetAttribute("TLFCAFeature", "1");

			if(RadioGroup1->Items->IndexOf("TLFVAFeature") != RadioGroup1->ItemIndex)
				fe->SetAttribute("TLFVAFeature", "0");
			else
				fe->SetAttribute("TLFVAFeature", "1");

			if(RadioGroup1->Items->IndexOf("CSFeature") != RadioGroup1->ItemIndex)
				fe->SetAttribute("CSFeature", "0");
			else
				fe->SetAttribute("CSFeature", "1");

			if(RadioGroup1->Items->IndexOf("TLFVFeature") != RadioGroup1->ItemIndex)
				fe->SetAttribute("TLFVFeature", "0");
			else
				fe->SetAttribute("TLFVFeature", "1");

			if(RadioGroup1->Items->IndexOf("TLFCFeature") != RadioGroup1->ItemIndex)
				fe->SetAttribute("TLFCFeature", "0");
			else
				fe->SetAttribute("TLFCFeature", "1");

			if(RadioGroup1->Items->IndexOf("TLFLBPFeature") != RadioGroup1->ItemIndex)
				fe->SetAttribute("TLFLBPFeature", "0");
			else
				fe->SetAttribute("TLFLBPFeature", "1");

			de->LinkEndChild(fe);
			doc.LinkEndChild(de);
			doc.SaveFile(tmpname.c_str());


			//CSBuilder.exe start
			UnicodeString str = ExtractFilePath(Application->ExeName);
			str = str + "CSBuilder.exe";
			AnsiString str1 = str + " -b \"" + tmpname + "\"";
			UINT result = WinExec(str1.c_str(), SW_SHOWNORMAL);
			//STARTUPINFO si; PROCESS_INFORMATION pi;
			//bool result = CreateProcessW(str.w_str(),str1.w_str(), NULL, NULL,
			//	 FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
			if (!result)
				ShowMessage(L"ERROR: cannot execute builder. Error code =  " + IntToStr((int)result));
            CSBuildOptions->Close();
		}
	}
	else
	{
        PageControl1->SelectNextPage(true);
		if(PageControl1->ActivePageIndex == PageControl1->PageCount-1)
		{
			NextButton->Caption = "Save && start";
		}
    }
}
//---------------------------------------------------------------------------

void __fastcall TCSBuildOptions::BackButtonClick(TObject *Sender)
{
	PageControl1->SelectNextPage(false);
	if(PageControl1->ActivePageIndex == PageControl1->PageCount-1)
		NextButton->Caption = "Save && start";
	else
	   NextButton->Caption = "Next";
}
//---------------------------------------------------------------------------

void __fastcall TCSBuildOptions::CancelButtonClick(TObject *Sender)
{
    CSBuildOptions->Close();
}
//---------------------------------------------------------------------------


void __fastcall TCSBuildOptions::FormShow(TObject *Sender)
{
     if(SelectBuildConfForm->m_configFilename != "")
	{
		AnsiString tmpstr = SelectBuildConfForm->m_configFilename;
		TiXmlDocument doc(tmpstr.c_str());
		if (!doc.LoadFile())
		{
			ShowMessage("Can't open configuration file.\n");
			return;
		}
		TiXmlHandle hDoc(&doc);

		/*разделы файла конфигурации*/
		TiXmlElement* pElem = NULL;

		pElem = hDoc.FirstChildElement().Element();
		if (!pElem)
		{
			ShowMessage("Invalid XML file");
			return;
		}
		const char* name = pElem->Value();
		if (strcmp(name, "BuildDetector") != 0)
		{
			ShowMessage("Invalid XML file");
			return;
		}

		bgr->Text = pElem->Attribute("bkground_base");
	//	m_strPathToBase += "\\";
		neg->Text = pElem->Attribute("negative_examples");
	//	m_strBKG += "\\";
		pos->Text = pElem->Attribute("positive_examples");
	//	m_strOBJ += "\\";

		Edit1->Text = pElem->Attribute("detector_name");
		Edit2->Text = pElem->Attribute("log_name");
		//pElem->Attribute("overlap_thr", &m_overlapThr);

		int tmp = 0;
		pElem->Attribute("num_samples_per_image", &tmp);
		CSpinEdit3->Value = tmp;
		pElem->Attribute("min_negative", &tmp);
		CSpinEdit7->Value = tmp;

		pElem->Attribute("num_negative", &tmp);

		pElem->Attribute("max_stages", &tmp);
		CSpinEdit8->Value = tmp;

		pElem->Attribute("base_width", &tmp);
		CSpinEdit2->Value = tmp;
		pElem->Attribute("base_height", &tmp);
		H_CSpinEdit->Value = tmp;
		double tmp1;
		pElem->Attribute("finish_far", &tmp1);
		CSpinEdit9->Value = int(tmp1*100);

		pElem = pElem->FirstChildElement("Features");
		if (pElem == NULL)
			RadioGroup1->ItemIndex = 5;
		int value = 0;
		pElem->QueryIntAttribute("CSFeature", &value);
		if(value != 0)
			RadioGroup1->ItemIndex = 5;
		pElem->QueryIntAttribute("TLFVFeature", &value);
		if(value != 0)
			RadioGroup1->ItemIndex = 6;
		pElem->QueryIntAttribute("TLFHFeature", &value);
		if(value != 0)
			RadioGroup1->ItemIndex = 0;

		pElem->QueryIntAttribute("TLFCFeature", &value);
		if(value != 0)
			RadioGroup1->ItemIndex = 7;
		pElem->QueryIntAttribute("TLFVAFeature", &value);
		if(value != 0)
			RadioGroup1->ItemIndex = 4;
		pElem->QueryIntAttribute("TLFHAFeature", &value);
		if(value != 0)
			RadioGroup1->ItemIndex = 2;
		pElem->QueryIntAttribute("TLFDAFeature", &value);
		if(value != 0)
			RadioGroup1->ItemIndex = 1;
		pElem->QueryIntAttribute("TLFCAFeature", &value);
		if(value != 0)
			RadioGroup1->ItemIndex = 3;

		pElem->QueryIntAttribute("TLFLBPFeature", &value);
		if(value != 0)
			RadioGroup1->ItemIndex = 8;
	}
	else
	{
		if(Form1->m_db.DbName!=NULL)
		{
			pos->Text = Form1->m_db.DbName + "\\dbexport";
			neg->Text = Form1->m_db.DbName + "\\dbexport\\negative";
            bgr->Text = Form1->m_db.DbName + "\\dbexport\\bg";
		}
    }
    if(PageControl1->ActivePageIndex == PageControl1->PageCount-1)
		NextButton->Caption = "Save && start";
	else
		NextButton->Caption = "Next";
}
//---------------------------------------------------------------------------


void __fastcall TCSBuildOptions::PageControl1Change(TObject *Sender)
{
	if(PageControl1->ActivePageIndex == PageControl1->PageCount-1)
		NextButton->Caption = "Save && start";
	else
	   NextButton->Caption = "Next";
}
//---------------------------------------------------------------------------

