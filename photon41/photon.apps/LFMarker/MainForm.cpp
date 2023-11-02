//---------------------------------------------------------------------------

#include <vcl.h>
#include <fstream>
#include <math.h>
#include <typeinfo.h>

#pragma hdrstop
#include "MainForm.h"
#include "AboutForm.h"
#include "TableViewForm.h"
#include "SelRectForm.h"
#include "RotateForm.h"
#include "DbInfoForm.h"
#include "DbLabeledImages.h"
#include "DbExportForm.h"
#include "ImageViewForm.h"
#include "SystemOptionsForm.h"
#include "DbCopyForm.h"
#include "DbConvertForm.h"
#include "SelDirUnit.h"
#include "DetectorInfoForm.h"
#include "EngineViewUnit.h"
#include "DictionaryForm.h"
#include "DbSplitForm.h"
#include "EngineErrForm.h"
#include <IniFiles.hpp>
#include "LongProcessForm.h"
#include "ImageUtils.h"
#include "FilterForm.h"
#include "GaussFilterForm.h"
#include "MedianFilterForm.h"
#include "ResizeForm.h"
#include "dictinaryEditor.h"
#include "PhImageMarkTool.h"
#include "VerInfoUnit.h"
#include "IniParamsUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
 extern "C"
{
    #pragma link "awpipl2b.lib"
	#pragma link "JPEGLIB.lib"
}
#pragma link "TinyXML.lib"

#pragma link "awplflibb.lib"
#pragma link "FImage41"
#pragma link "PhImageTool"
#pragma link "PhLenzTool"
#pragma link "PhPaneTool"
#pragma link "PhSelectRectTool"
#pragma link "PhZoomToRectTool"
#pragma link "PhRulerTool"
#pragma link "PhTriangleTool"
#pragma resource "*.dfm"
using namespace std;
TForm1 *Form1;
extern TMarkerIniParams* IniParams;
//---------------------------------------------------------------------------
// функция прогресса
void _stdcall progress(int p, const char* str_txt)
{
   if (Form1->ProgressBar1)
   {
	   Form1->ProgressBar1->Position = p;
   }
}



//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{
	UUID id;
	LF_NULL_UUID_CREATE(id);
	m_pBaseObject = new TLFDetectedItem(NULL, 0, "is marked by a person", 0, 0,  10, 10, "", id);
	m_strFileName = "";
	m_Selected = -1;
	m_DrawOverlaps = false;
	m_ViewSemanticOutput = false;

	m_objects = new TList();

	m_overlaps_thr = 0.8;
	m_nearlest_overlap = false;

	m_farthest_min_thr = 0.4;
	m_farthest_max_thr = 0.6;
	m_draw_farthest_overlaps = false;


	m_strLastPath = "";
	m_penWidth = 1;
	m_deltaWidth = 1;
	m_set_nearest_overlap = false;
	m_detect_in_rect = false;
    m_markTool = NULL;

	m_tableVisible = false;
    m_fragmentsVisible = false;
    m_markToolSelected = false;
}

__fastcall TForm1::~TForm1()
{
   if (m_pBaseObject != NULL)
	  delete m_pBaseObject;
   if (m_objects != NULL)
   {
   		m_objects->Clear();
		delete m_objects;
   }
   if (m_markTool != NULL)
    delete m_markTool;
}
void __fastcall TForm1::InitImageFile(AnsiString& strFileName)
{
    m_Descr.Clear();
    AnsiString strExt = ExtractFileExt(strFileName);
    strExt = strExt.UpperCase();
    try
    {
        if (strExt != "")
        {
            PhImage2->InitFile(strFileName);

            awpImage* tmp = NULL;
            PhImage2->GetAwpImage(&tmp);
            m_Descr.SetImage(tmp);
            awpReleaseImage(&tmp);

            AnsiString str =  "Image Marker";
            str += " [";
            str += m_strFileName;
            str += "]";
            Form1->Caption  = str;
            TableForm->UpdateTable();
            TPhImageMarkTool* tool = dynamic_cast< TPhImageMarkTool*>(PhImage2->PhTool);
            if (tool != NULL)
            {
                AnsiString strFileName = ChangeFileExt( m_strFileName, ".xml" );
                tool->SetFrame(strFileName.c_str());
            }
        }
    }
	catch(...)
    {
		Memo1->Lines->Add("ERROR: cannot open image " + strFileName);
	}
}

//---------------------------------------------------------------------------
void __fastcall TForm1::FileListBox1Change(TObject *Sender)
{
	 m_strFileName = FileListBox1->FileName;
	 if (m_strFileName != "")
		 InitImageFile(m_strFileName);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FFaceEditor1AfterOpen(TObject *Sender)
{

    m_Selected = -1;
    UpdateSatatusBar();
    this->m_objects->Clear();
    this->m_rois.Clear();
     if( FileExists(ChangeFileExt( m_strFileName, ".xml" )))
     {
        AnsiString strFileName = ChangeFileExt( m_strFileName, ".xml" );
        if (!this->m_Descr.LoadXML(strFileName.c_str()))
        {
            ShowMessage("Cannot load description " + strFileName);
            return;
        }
        FragmentForm->SDescriptor = &this->m_Descr;
     }

    Detect();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormShow(TObject *Sender)
{
     PhImage2->SelectPhTool(PhPaneTool1);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::CloseActionExecute(TObject *Sender)
{
    PhImage2->Close();
    PhImage2->Empty = true;
    Close();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormResize(TObject *Sender)
{
    this->RepaintImage();
    this->DrawScene();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::BestFitActionExecute(TObject *Sender)
{
    this->m_NeedBestFit = true;
    this->RepaintImage();
    this->DrawScene();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::PaneActionExecute(TObject *Sender)
{
	PhImage2->SelectPhTool(PhPaneTool1);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ZoomToRectActionExecute(TObject *Sender)
{
	PhImage2->SelectPhTool(PhZoomToRectTool1);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ActualSizeActionExecute(TObject *Sender)
{
    PhImage2->ActualSize();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormCreate(TObject *Sender)
{
	m_strEditCurrentDir = "";

	 LoadIniFile();

	 m_NeedBestFit = true;

	m_ProgressBar1 = new  TProgressBar ( StatusBar1 );
	ProgressBar1->Parent = StatusBar1;
	ProgressBar1->Visible = false;
	UpdateSatatusBar();
	TabSheet3->TabVisible = false;
	TabSheet2->TabVisible = false;
	InitDbView();
	DirectoryListBox1->Directory =  IniParams->LastPath;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::DtDetectActionExecute(TObject *Sender)
{
	Detect();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::DtDetectActionUpdate(TObject *Sender)
{
   DtDetectAction->Enabled = m_ObjectEngine.GetDetector(0) != NULL;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::DtClassifyActionExecute(TObject *Sender)
{
    Detect();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ModeBestFitActionExecute(TObject *Sender)
{
      m_NeedBestFit = true;
      this->RepaintImage();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ModeActualSizeActionExecute(TObject *Sender)
{
      m_NeedBestFit = false;
      this->RepaintImage();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::AboutActionExecute(TObject *Sender)
{
//        AboutBox->ShowModal();
  TVersionInfo* vi = new TVersionInfo(NULL);
  AboutBox->ProductName->Caption = vi->ProductName;
  AboutBox->Version->Caption = vi->FileVersion;
  AboutBox->Copyright->Caption = vi->LegalCopyright;
  AboutBox->Comments->Caption = vi->Comments;
  AboutBox->Memo1->Lines->LoadFromFile(ExtractFilePath(Application->ExeName) + L"\\lgpl.txt");
  AboutBox->ShowModal();
  delete vi;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::DbSaveMarkActionExecute(TObject *Sender)
{
    if (m_Descr.GetItemsCount() > 0)
    {
       AnsiString strFileName = ChangeFileExt(m_strFileName,".xml");
       if (!m_Descr.SaveXML(strFileName.c_str()))
            ShowMessage("Cannot save image description to " + strFileName);
	  TPhImageMarkTool* tool = dynamic_cast< TPhImageMarkTool*>(PhImage2->PhTool);
      if (tool != NULL)
      {
        tool->SetFrame(strFileName.c_str());
      }
    }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ImageDelImageActionExecute(TObject *Sender)
{
	UnicodeString _unicode = m_strFileName;
	if (Application->MessageBox(L"Are you sure delete? ", L"Warning", MB_YESNO) == IDNO)
		return;
// delete data
	if (FileExists(m_strFileName))
		DeleteFile(m_strFileName);
	AnsiString strName = ChangeFileExt(m_strFileName, ".ieye");
	if (FileExists(strName))
		DeleteFile(strName);

	strName = ChangeFileExt(m_strFileName, ".xml");
	if (FileExists(strName))
		DeleteFile(strName);

	UnicodeString    strFileName = ExtractFileName(m_strFileName);
	UnicodeString    strFilePath = ExtractFilePath(m_strFileName);
	int idx = FileListBox1->Items->IndexOf(strFileName);

// update ui
	if (TabSheet3->Visible)
	{
#ifdef _DEBUG
		UnicodeString _message = L"Database view. Selected file: ";
		_message += m_strFileName;
		ShowMessage(_message);
#endif
		DbView->Items->Delete(idx);
		DbView->ItemIndex = idx;
        DbView->Selected = DbView->Items->Item[idx];
	}
	// update filelistview
	if (idx >= 0)
	{
		FileListBox1->Items->Delete(idx);
		FileListBox1->ItemIndex = idx;
		FileListBox1->Selected[idx] = true;
		m_strFileName = FileListBox1->Items->Strings[idx];
	}
	m_strFileName = strFilePath + L"\\" + FileListBox1->Items->Strings[idx];

	this->InitImageFile(m_strFileName);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ImageDelImageActionUpdate(TObject *Sender)
{
   ImageDelImageAction->Enabled = !PhImage2->Empty && m_strFileName != "";
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ModePaneActionUpdate(TObject *Sender)
{
	ModePaneAction->Checked = dynamic_cast< TPhPaneTool*>(PhImage2->PhTool) != NULL;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ModeZoomActionUpdate(TObject *Sender)
{
		ModeZoomAction->Checked = dynamic_cast< TPhZoomToRectTool*>(PhImage2->PhTool) != NULL;;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ModeBestFitActionUpdate(TObject *Sender)
{
        ModeBestFitAction->Checked = m_NeedBestFit;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ModeActualSizeActionUpdate(TObject *Sender)
{
   ModeActualSizeAction->Checked = !this->m_NeedBestFit;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::LoadIniFile()
{
     AnsiString strName = ChangeFileExt(Application->ExeName, ".ini");
     TIniFile* ini = new TIniFile(strName);
     delete ini;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::StatusBar1Resize(TObject *Sender)
{
    RECT Rect;
    StatusBar1->Perform ( SB_GETRECT,  0,  (LPARAM)&Rect );

	m_ProgressBar1->Top = Rect.top+2;
	m_ProgressBar1->Left = Rect.left + StatusBar1->Panels->Items[ 0 ]->Width + 4;
	m_ProgressBar1->Width = StatusBar1->Panels->Items [ 1 ]->Width - 6;
	m_ProgressBar1->Height = Rect.bottom - Rect.top - 4;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ApplicationEvents1Hint(TObject *Sender)
{
   StatusBar1->Panels->Items[0]->Text =  GetLongHint(Application->Hint);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::UpdateSatatusBar()
{
    AnsiString strStatusText = "";
    awpImage* pImage = NULL;
    if (PhImage2->Empty)
        return;
    PhImage2->GetAwpImage(&pImage);
    if (pImage != NULL)
    {
        strStatusText = IntToStr(pImage->sSizeX) + ":" + IntToStr(pImage->sSizeY) + ":" + IntToStr(pImage->bChannels);
        awpReleaseImage(&pImage);
    }
     StatusBar1->Panels->Items[2]->Text = strStatusText;
}
void __fastcall TForm1::ModeMarkRectActionExecute(TObject *Sender)
{
	if (m_markTool != NULL)
		delete m_markTool;
	m_markTool = new TPhImageMarkTool(NULL);
	m_markTool->PhImage = PhImage2;
    m_markTool->Mode = MTRect;
	m_markTool->dictinary = m_db.Dictionary;
	m_markTool->OnChange = ToolChange;
	PhImage2->SelectPhTool(m_markTool);
	AnsiString strFileName = ChangeFileExt( m_strFileName, ".xml" );
	std::string str = strFileName.c_str();
	if (LFFileExists(str.c_str()))
		m_markTool->SetFrame(str.c_str());
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ModeMarkRectActionUpdate(TObject *Sender)
{
	ModeMarkRectAction->Enabled = this->PageControl1->ActivePageIndex == 2;
	TPhImageMarkTool* tool = dynamic_cast< TPhImageMarkTool*>(PhImage2->PhTool);
	ModeMarkRectAction->Checked = tool != NULL && tool->Mode == MTRect;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ViewToolbarActionExecute(TObject *Sender)
{
    this->ToolBar1->Visible = !this->ToolBar1->Visible;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ViewToolbarActionUpdate(TObject *Sender)
{
    ViewToolbarAction->Checked = this->ToolBar1->Visible;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ViewStatusbarActionExecute(TObject *Sender)
{
    this->StatusBar1->Visible = !this->StatusBar1->Visible;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ViewStatusbarActionUpdate(TObject *Sender)
{
   ViewStatusbarAction->Checked =  this->StatusBar1->Visible;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ViewDirectoryActionExecute(TObject *Sender)
{
    this->Panel1->Visible = !this->Panel1->Visible;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ViewDirectoryActionUpdate(TObject *Sender)
{
    ViewDirectoryAction->Checked =  this->Panel1->Visible;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ViewTableActionExecute(TObject *Sender)
{
    TableForm->Visible = !TableForm->Visible;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ViewTableActionUpdate(TObject *Sender)
{
   ViewTableAction->Enabled = PageControl1->ActivePageIndex == 2;
   ViewTableAction->Checked = TableForm->Visible;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormKeyPress(TObject *Sender, char &Key)
{
        if (Key == 'S' || Key == 's')
                DbSaveMarkActionExecute(NULL);
}
//---------------------------------------------------------------------------
bool __fastcall TForm1::RemoveMarkupHelper()
{
        if (this->m_Descr.GetItemsCount() > 0)
        {
			 if (Application->MessageBox(L"This operation will remove the marking image. Are you sure to delete the markup?",
			 L"Warning", MB_YESNO) == IDNO)
                  return false;
             if (this->m_Descr.GetItemsCount() > 0)
             {
                DeleteFile(ChangeFileExt( FileListBox1->FileName, ".xml" ));
                m_Descr.Clear();
                TableForm->ListView1->Clear();
                this->RepaintImage();
             }
        }
        return true;
}

//---------------------------------------------------------------------------
void __fastcall TForm1::DbInfoActionExecute(TObject *Sender)
{
    TLFDBLabeledImages* db = m_db.Data;

    DbInfoDialog->labelDatabase->Caption = db->GetPath().c_str();
	DbInfoDialog->labelTotalImages->Caption = IntToStr(db->GetImagesCount());
	DbInfoDialog->labelTotalXmlFiles->Caption = IntToStr(db->GetDescrFilesCount());
	DbInfoDialog->labelClasses->Caption = IntToStr(db->GetClassesCount());
	DbInfoDialog->labelTotalXmlItems->Caption = IntToStr(db->GetItemsCount());
    TLFSemanticDictinary* d = m_db.Dictionary;
    DbInfoDialog->ValueListEditor1->Strings->Clear();
    for (int i = 0; i < d->GetCount(); i++)
    {
        TLFSemanticDictinaryItem* w = d->GetWordFromDictinary(i);
        AnsiString strPair = w->GetItemLabel();
        strPair += L"=";
        strPair += IntToStr(db->GetLabelCount(w->GetId().c_str()));
        DbInfoDialog->ValueListEditor1->Strings->Add(strPair);
    }
	DbInfoDialog->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::DbInfoActionUpdate(TObject *Sender)
{
    DbInfoAction->Enabled = m_db.NumImages > 0;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ProgressHandler(int Progress, AnsiString& aComment)
{
	m_ProgressBar1->Position  = Progress;
    StatusBar1->Panels->Items[0]->Text =  aComment;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ViewOlerlapsActionExecute(TObject *Sender)
{
	m_DrawOverlaps = !m_DrawOverlaps;
	//DrawScene();
    PhImage2->Paint();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ViewOlerlapsActionUpdate(TObject *Sender)
{
   ViewOlerlapsAction->Checked  = m_DrawOverlaps;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::DbExportFragmentsActionExecute(TObject *Sender)
{
//
	AnsiString strExportPath = DirectoryListBox1->Directory;
    strExportPath += "\\dbexport";
	DbExportDialog->Edit1->Text = strExportPath;
	TLFSemanticDictinary* d = m_db.Dictionary;
	if (d->GetCount() == 0)
	{
		ShowMessage("Semantig dictionary is empty.");
		return;
	}
	else
	{
		DbExportDialog->CheckListBox1->Clear();
		for (int i = 0; i < d->GetCount(); i++)
		{
			TLFSemanticDictinaryItem* word = d->GetWordFromDictinary(i);
			DbExportDialog->CheckListBox1->Items->Add(word->GetItemLabel());
			DbExportDialog->CheckListBox1->Checked[i] = true;
		}
	}
	if (DbExportDialog->ShowModal() == mrOk)
    {
        strExportPath = DbExportDialog->Edit1->Text;

	   if (m_db.NumXmlFiles < 1 || m_db.NumImages < 1)
	   {
			ShowMessage("In the database there is no markup files in the format of xml.");
			return;
	   }
	   // setup options
	   //SDbExportOptions options;
	   export_options.ClassLabels = new TStringList();
	   for (int i = 0; i < DbExportDialog->CheckListBox1->Count; i++)
	   {
		   if (DbExportDialog->CheckListBox1->Checked[i])
		   {
				export_options.ClassLabels->Add(DbExportDialog->CheckListBox1->Items->Strings[i]);
		   }
	   }
	   export_options.strPathToExport = strExportPath;
	   export_options.useScanner   = DbExportDialog->ComboBox1->ItemIndex == 0;
	   export_options.exportFormat = DbExportDialog->ComboBox2->ItemIndex == 0?awp:jpeg;
	   export_options.exportNearlest = DbExportDialog->CheckBox3->Checked;
	   export_options.racurs = DbExportDialog->ComboBox4->ItemIndex;
	   export_options.copyBackground =  DbExportDialog->CheckBox4->Checked;
	   export_options.exportCovered = DbExportDialog->CheckBox8->Checked;
	   try
	   {
		 export_options.scannerThreshold = StrToFloat(DbExportDialog->Edit2->Text);
		 export_options.baseSize = StrToInt(DbExportDialog->Edit3->Text);
		 export_options.random = StrToInt(DbExportDialog->Edit4->Text);
	   }
	   catch(EConvertError& e)
	   {
		export_options.scannerThreshold = 0.7;
		export_options.baseSize = 128;
	   }

	   export_options.needFlip = DbExportDialog->CheckBox1->Checked;
	   export_options.needResize =  DbExportDialog->CheckBox7->Checked;
	   export_options.copyRandom =  DbExportDialog->CheckBox6->Checked;
       export_options.copyIeye =  DbExportDialog->CheckBox5->Checked;
/*
	   m_ProgressBar->Position  = 0;
	   m_ProgressBar->Visible = true;
	   m_db.OnProgress = ProgressHandler;
	   m_db.ExportFragments(options);
	   delete options.ClassLabels;
	   m_ProgressBar->Visible = false;
	   m_ProgressBar->Position  = 0;
*/
	   LongProcDlg->ProcType = ptExport;
	   LongProcDlg->ShowModal();

		if (DbExportDialog->CheckBox2->Checked)
		{
			AnsiString strCommand = "explorer.exe ";
			strCommand += strExportPath;
			WinExec(strCommand.c_str(), SW_SHOWNORMAL);
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::DbExportFragmentsActionUpdate(TObject *Sender)
{
//
}
//---------------------------------------------------------------------------


void __fastcall TForm1::ViewSemanticOutputActionExecute(TObject *Sender)
{
	m_ViewSemanticOutput = !m_ViewSemanticOutput;
    PhImage2->Paint();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ViewSemanticOutputActionUpdate(TObject *Sender)
{
    ViewSemanticOutputAction->Checked = m_ViewSemanticOutput;
}
//---------------------------------------------------------------------------


void __fastcall TForm1::DbConvertDatabaseActionExecute(TObject *Sender)
{
	if (DbConvertDlg->ShowModal() == mrOk)
    {
	  // SDbConvertOptions options;

	   convert_options.needResize    = DbConvertDlg->CheckBox1->Checked;
	   try
	   {
		convert_options.baseWidth     = StrToInt(DbConvertDlg->Edit1->Text);
	   }
	   catch(EConvertError& e)
	   {
			convert_options.baseWidth     = 340;
	   }

	   convert_options.interpolation  = DbConvertDlg->CheckBox2->Checked;
	   convert_options.RenameToUUID   = DbConvertDlg->CheckBox3->Checked;

	   switch(DbConvertDlg->ComboBox1->ItemIndex)
	   {
			case 0:
				convert_options.format = jpeg;
			break;
			case 1:
				convert_options.format = awp;
			break;
	   }
		LongProcDlg->ProcType = ptConvert;
		LongProcDlg->ShowModal();
		FileListBox1->Update();
	}

}
//---------------------------------------------------------------------------


void __fastcall TForm1::FileListBox1KeyUp(TObject *Sender, WORD &Key,
	  TShiftState Shift)
{
	if (Key == VK_DELETE)
	{
		ImageDelImageActionExecute(NULL);
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FileOpenDetectorActionExecute(TObject *Sender)
{
// open external detector
    if (OpenDialog1->Execute())
    {
		AnsiString str = OpenDialog1->FileName;
		if (!this->m_ObjectEngine.Load(str.c_str()))
            ShowMessage("Cannot load detect engine " + ExtractFileName(OpenDialog1->FileName));
		else
		{
        	m_strEngineName = str;
			Detect();
		}
    }

}
//---------------------------------------------------------------------------
void __fastcall TForm1::BottomDocPanelDockDrop(TObject *Sender,
      TDragDockObject *Source, int X, int Y)
{
    ShowDockPanel(BottomDocPanel, true, NULL);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::BottomDocPanelDockOver(TObject *Sender,
      TDragDockObject *Source, int X, int Y, TDragState State,
      bool &Accept)
{
//
  Accept = (Source->Control) != NULL;
  if (Accept)
  {
    // Modify the DockRect to preview dock area.
    Types::TPoint TopLeft = BottomDocPanel->ClientToScreen(Point(0, -Panel6->Height / 3));
    Types::TPoint BottomRight = BottomDocPanel->ClientToScreen(
      Point(this->BottomDocPanel->Width, 0));
    Source->DockRect = Types::TRect(TopLeft, BottomRight);
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::BottomDocPanelGetSiteInfo(TObject *Sender,
      TControl *DockClient, TRect &InfluenceRect, TPoint &MousePos,
	  bool &CanDock)
{
//
}
//---------------------------------------------------------------------------

void __fastcall TForm1::BottomDocPanelUnDock(TObject *Sender,
      TControl *Client, TWinControl *NewTarget, bool &Allow)
{
    ShowDockPanel(BottomDocPanel, false, NULL);
}
//---------------------------------------------------------------------------
void TForm1::ShowDockPanel(TPanel* APanel, bool MakeVisible, TControl* Client)
{
  if (!MakeVisible && (APanel->VisibleDockClientCount > 1))
    return;
  if (MakeVisible)
  {
      APanel->Height = ClientHeight / 3;
      HSplitter->Top = ClientHeight - APanel->Height - HSplitter->Width;
  }
  else
  {
      APanel->Height = 0;
  }

  if (MakeVisible && (Client != NULL))
    Client->Show();
}


void __fastcall TForm1::ModePaneActionExecute(TObject *Sender)
{
	PhImage2->SelectPhTool(PhPaneTool1);
    PhImage2->Paint();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ModeZoomActionExecute(TObject *Sender)
{
	PhImage2->SelectPhTool(PhZoomToRectTool1);
	PhImage2->Paint();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ObjectDetectorHelper()
{
	if (this->DetectInRect)
		return;

	if (m_ObjectEngine.GetDetector() != NULL)
    {
        //
        m_objects->Clear();
		awpImage* img = NULL;
		PhImage2->GetAwpImage(&img);
		if (img)
		{
			 m_ObjectEngine.SetSourceImage(img, true);
             int num = m_ObjectEngine.GetItemsCount();
             if (num > 0)
             {
                for (int i = 0; i < num; i++)
                {
					TLFDetectedItem* di = m_ObjectEngine.GetItem(i);
                    if (di)
                    {
						awpRect rect = di->GetBounds()->GetRect();
						TRect*   r = new TRect(rect.left, rect.top, rect.right, rect.bottom);
						m_objects->Add(r);
                    }
                }
             }

            _AWP_SAFE_RELEASE_(img)
        }
	}
}

void __fastcall TForm1::DrawScene()
{

    TCanvas* cnv = PhImage2->Canvas;
	DrawObjects(cnv);
	DrawSemantic(cnv);
	DrawFarthestOverlaps(cnv);
	DrawOverlaps(cnv);
	DrawRois(cnv);
}

void __fastcall TForm1::FImage1Pane(TObject *Sender)
{
//
    this->DrawScene();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FImage1ScaleChange(TObject *Sender)
{
    this->DrawScene();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::DrawRois(TCanvas* cnv)
{
    if (cnv == NULL)
        return;

    TBrushStyle bstyle = cnv->Brush->Style;
    cnv->Brush->Style = bsClear;
    TColor penColor = cnv->Pen->Color;
	cnv->Pen->Color = clLime;
	cnv->Pen->Width = 1;

	for (int i = 0; i < m_rois.GetItemsCount(); i++)
    {
		  TLFRect* roi = m_rois.GetDetectedItem(i)->GetBounds();
          if (roi != NULL)
          {
				awpRect r = roi->GetRect();

		   //     awpPoint le = r.p;
		   //     awpPoint re = r.p1;
				TRect rect;
				rect.init(r.left, r.top, r.right, r.bottom);
				TRect Rect2 = PhImage2->GetScreenRect(rect); //FImage1->GetScreenRect(rect);
                cnv->Rectangle(Rect2);
		   //		rect.init(re.X - 2, re.Y - 2, re.X + 2, re.Y + 2);
		   //     Rect2 = PhImage2->GetScreenRect(rect);
		   //     cnv->Rectangle(Rect2);
          }

    }

    cnv->Brush->Style = bstyle;
    cnv->Pen->Color = penColor;
}


void __fastcall TForm1::DrawObjects(TCanvas* cnv)
{
	if (cnv == NULL)
        return;

    TBrushStyle bstyle = cnv->Brush->Style;
	cnv->Brush->Style = bsClear;
	TColor penColor = cnv->Pen->Color;
	cnv->Pen->Color = clLime;

	for (int i = 0; i < m_objects->Count; i++)
	{

		TRect* rect = (TRect*)m_objects->Items[i];
		TRect Rect2 = PhImage2->GetScreenRect(*rect);
		TLFDetectedItem* di = m_ObjectEngine.GetItem(i);
		cnv->Pen->Color = clLime;
		cnv->Rectangle(Rect2);
	}
	cnv->Font->Color = clLime;
    cnv->TextOutW(10,10, "Min size = ");

	cnv->Brush->Style = bstyle;
    cnv->Pen->Color = penColor;

}

void __fastcall TForm1::DrawSemantic(TCanvas* cnv)
{
    if (cnv == NULL)
        return;
    if (!m_ViewSemanticOutput)
        return;

    TBrushStyle bstyle = cnv->Brush->Style;
    cnv->Brush->Style = bsClear;
    TColor penColor = cnv->Pen->Color;
	cnv->Pen->Color = clRed;

	for (int i = 0; i < m_Descr.GetItemsCount(); i++)
	{
		TLFDetectedItem* di = m_Descr.GetDetectedItem(i);
		TLFRect* rr = di->GetBounds();
		awpRect  r  = rr->GetRect();
		TRect  rect(r.left, r.top, r.right, r.bottom);
		TRect  Rect2 = PhImage2->GetScreenRect(rect);
		cnv->Pen->Width = 1;
		if (i == this->SelectedIndex)
		{
			cnv->Pen->Width = 3;
		}
		else
		{
			cnv->Pen->Width = 1;
		}
		cnv->Rectangle(Rect2);
	}

	cnv->Brush->Style = bstyle;
	cnv->Pen->Color = penColor;
	cnv->Pen->Width = 1;
}
void __fastcall TForm1::DrawFarthestOverlaps(TCanvas* cnv)
{
 cnv->Brush->Style = bsClear;
 cnv->Pen->Width = 1;
 if (this->m_draw_farthest_overlaps)
 {
	for (int i = 0; i < m_Descr.GetItemsCount(); i++)
	{
		TLFDetectedItem* di = m_Descr.GetDetectedItem(i);
		TLFRect* rr = di->GetBounds();
		awpRect  r  = rr->GetRect();
		TRect  rect(r.left, r.top, r.right, r.bottom);
		float ar = (float)di->GetBH()/(float)di->GetBW();
		m_scanner.Scan(PhImage2->Bitmap->Width, PhImage2->Bitmap->Height);
		 for (int j = 0; j < m_scanner.GetFragmentsCount(); j++)
		 {
			awpRect r1 = m_scanner.GetFragmentRect(j);
			TLFRect dr;
			dr.SetRect(r1);
			float overlap = rr->RectOverlap(dr);
			if (overlap >= this->m_farthest_min_thr && overlap <= this->m_farthest_max_thr)
			{
				  cnv->Pen->Color = clTeal;
				  TRect  rect3(r1.left, r1.top, r1.right, r1.bottom);
				  TRect  Rect4 = PhImage2->GetScreenRect(rect3);
				  cnv->Rectangle(Rect4);
			}
		 }
	}
  }
}
void __fastcall TForm1::DrawOverlaps(TCanvas* cnv)
{
    if (!m_DrawOverlaps)
    	return;

	cnv->Brush->Style = bsClear;
	cnv->Pen->Width = 1;
	for (int i = 0; i < m_Descr.GetItemsCount(); i++)
	{
		TLFDetectedItem* di = m_Descr.GetDetectedItem(i);
		string uuid = di->GetType();
		TLFRect* rr = di->GetBounds();
		awpRect  r  = rr->GetRect();
		TRect  rect(r.left, r.top, r.right, r.bottom);
		TLFDBLabeledImages* db = m_db.Data;
		if (db != NULL)
		{
			TLFSemanticDictinary* sd  = db->GetDictinary();
			for (int k = 0; k < sd->GetCount(); k++)
			{
					TLFSemanticDictinaryItem* sid = sd->GetWordFromDictinary(k);
					if (sid != NULL)
					{
						if (sid->GetId() == uuid)
						{
							//
							//Memo1->Lines->Add("scanner settings:" + IntToStr(sid->GetScanner()->GetBaseHeight()));
							m_scanner.SetBaseHeight(sid->GetScanner()->GetBaseHeight());
							m_scanner.SetBaseWidth(sid->GetScanner()->GetBaseWidth());
						}
					}
			}
		}
	   m_scanner.Scan(PhImage2->Bitmap->Width, PhImage2->Bitmap->Height);
	   if (!this->m_nearlest_overlap)
       {
         for (int j = 0; j < m_scanner.GetFragmentsCount(); j++)
         {
            awpRect r1 = m_scanner.GetFragmentRect(j);
            TLFRect dr;
            dr.SetRect(r1);
            float overlap = rr->RectOverlap(dr);
            if (overlap > m_overlaps_thr)
            {
                  cnv->Pen->Color = clYellow;
                  TRect  rect3(r1.left, r1.top, r1.right, r1.bottom);
                  TRect  Rect4 = PhImage2->GetScreenRect(rect3);

                  cnv->Rectangle(Rect4);
                  cnv->Pen->Color = clRed;
            }
         }
       }
       else
       {
         TRect nearlest_rect;
         double nearlest = 0;
         for (int j = 0; j < m_scanner.GetFragmentsCount(); j++)
         {
            awpRect r1 = m_scanner.GetFragmentRect(j);
            TLFRect dr;
            dr.SetRect(r1);
			float overlap = rr->RectOverlap(dr);
            if (overlap > nearlest)
            {
              nearlest = overlap;
              TRect  rect3(r1.left, r1.top, r1.right, r1.bottom);
              nearlest_rect = PhImage2->GetScreenRect(rect3);
            }
         }
         // drawing
        cnv->Pen->Color = clYellow;
        cnv->Rectangle(nearlest_rect);
        cnv->Pen->Color = clRed;
       }
   }
}
void __fastcall TForm1::Options1Click(TObject *Sender)
{
	PagesDlg->Edit1->Text = FormatFloat("", this->m_overlaps_thr);
	PagesDlg->CheckBox1->Checked = this->m_nearlest_overlap;
	PagesDlg->LabeledEdit1->Text = FormatFloat("", this->m_farthest_min_thr);
	PagesDlg->LabeledEdit2->Text = FormatFloat("", this->m_farthest_max_thr);
	PagesDlg->ValueListEditor1->Strings->Clear();

    for (int i = 0; i < m_scanner.GetParamsCount(); i++)
    {
        TLFParameter*  param = m_scanner.GetParameter(i);
        if (param)
        {
              AnsiString str = param->GetPName();
              str += "=";
              str += param->GetValue();
              PagesDlg->ValueListEditor1->Strings->Add(str);
        }
    }
	PagesDlg->Edit2->Text = IntToStr(m_scanner.GetBaseWidth());
    PagesDlg->Edit3->Text = IntToStr(m_scanner.GetBaseHeight());
	PagesDlg->CheckBox5->Checked = this->DetectInRect;
    if (this->m_ObjectEngine.GetDetector() != NULL)
    {
        PagesDlg->ValueListEditor2->Strings->Clear();
        ILFScanner* scr = m_ObjectEngine.GetScanner();
		for (int i = 0; i < scr->GetParamsCount(); i++)
        {
            TLFParameter*  param = scr->GetParameter(i);
            if (param)
            {
                  AnsiString str = param->GetPName();
                  str += "=";
                  str += param->GetValue();
                  PagesDlg->ValueListEditor2->Strings->Add(str);
            }
        }
		PagesDlg->Label8->Caption = "Base Width = " + IntToStr(scr->GetBaseWidth());
		PagesDlg->Label9->Caption = "Base Height = " + IntToStr(scr->GetBaseHeight());
		PagesDlg->CheckBox2->Checked = m_ObjectEngine.GetNeedCluster();
		PagesDlg->CheckBox3->Checked = m_ObjectEngine.GetResize();
		PagesDlg->Edit4->Text = IntToStr(m_ObjectEngine.GetBaseImageWidth());
		//PagesDlg->CheckBox4->Checked = m_ObjectEngine.GetDetector()->GetUseTilt();

	}

	if (PagesDlg->ShowModal() == mrOk)
	{
		this->m_overlaps_thr        = StrToFloat(PagesDlg->Edit1->Text);
		this->m_nearlest_overlap    = PagesDlg->CheckBox1->Checked;
		m_farthest_min_thr = StrToFloat(PagesDlg->LabeledEdit1->Text);//
		m_farthest_max_thr = StrToFloat(PagesDlg->LabeledEdit2->Text);

		if (this->m_ObjectEngine.GetDetector() != NULL)
		{
		   ILFScanner* scr = m_ObjectEngine.GetScanner();
		   for (int i = 0; i < scr->GetParamsCount(); i++)
		   {
				TLFParameter* p = scr->GetParameter(i);
				for (int j = 0; j < PagesDlg->ValueListEditor2->Strings->Count; j++)
				{
					AnsiString strName = p->GetPName();
					if (strName == PagesDlg->ValueListEditor2->Keys[j+1])
					{
						p->SetValue(StrToFloat(PagesDlg->ValueListEditor2->Values[strName]));
					}
				}
		   }

		   //this->m_ObjectEngine.GetDetector()->SetUseTilt(PagesDlg->CheckBox4->Checked);
		   this->m_ObjectEngine.SetNeedCluster(PagesDlg->CheckBox2->Checked);
		   this->m_ObjectEngine.SetResize(PagesDlg->CheckBox3->Checked);
		   this->m_ObjectEngine.SetBaseImageWidth(StrToInt(PagesDlg->Edit4->Text));
		   this->DetectInRect = PagesDlg->CheckBox5->Checked;
		}

		Detect();
	}
}
//---------------------------------------------------------------------------
TLFScanner* __fastcall TForm1::GetScanner()
{
	return &this->m_scanner;
}

void __fastcall TForm1::Detect()
{
	this->RepaintImage();

	this->m_objects->Clear();
	this->m_rois.Clear();

	if (ToolButton8->Down)
	   this->ObjectDetectorHelper();

   DrawScene();
}
void __fastcall TForm1::DbCopyActionExecute(TObject *Sender)
{
	if (DbCopyDlg->ShowModal() == mrOk)
	{
	   // Setup options
	   copy_options.strPathToCopy = DbCopyDlg->Edit1->Text;
	   copy_options.copyImages    = DbCopyDlg->CheckBox1->Checked;
	   copy_options.copySemantic  = DbCopyDlg->CheckBox2->Checked;

	   if (m_db.NumImages == 0)
	   {
		  ShowMessage("There are not images in the database.");
		  return;
	   }

	   if (m_db.DbName == copy_options.strPathToCopy)
	   {
		  ShowMessage("Cannot copy database into myself.");
		  return;
	   }

	   if (!DbCopyDlg->CheckBox1->Checked && !DbCopyDlg->CheckBox2->Checked &&
		   !DbCopyDlg->CheckBox5->Checked)
	   {
		  ShowMessage("Nothing to do.");
		  return;
	   }
	   LongProcDlg->ProcType = ptCopy;
	   LongProcDlg->ShowModal();

	   if (DbCopyDlg->CheckBox5->Checked)
		  DirectoryListBox1->Directory =  copy_options.strPathToCopy;

	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::DbCopyActionUpdate(TObject *Sender)
{
//
}
//---------------------------------------------------------------------------
void __fastcall TForm1::RepaintImage()
{
	if (m_NeedBestFit)
        PhImage2->BestFit();
    else
        PhImage2->ActualSize();
}
ILFDetectEngine*  __fastcall TForm1::GetEngine()
{
    return &this->m_ObjectEngine;
}
// копирование изображение из одного каталога в другой, вместе со всеми
// информационными файлами.
void __fastcall TForm1::ImageCopyImageActionExecute(TObject *Sender)
{
	if (GetDirNamePreview(m_strLastPath))
	{
		String strFileName = FileListBox1->FileName;
		String strNewFileName = m_strLastPath + "\\" + ExtractFileName(FileListBox1->FileName);
		CopyFile(strFileName.c_str(), strNewFileName.c_str(), true);
    }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ImageCopyImageActionUpdate(TObject *Sender)
{
    ImageCopyImageAction->Enabled = !PhImage2->Empty && FileListBox1->FileName != "";
}
//---------------------------------------------------------------------------
void __fastcall TForm1::SetSelectedIndex(int Value)
{
    this->m_Selected = Value;
    this->DrawScene();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Timer1Timer(TObject *Sender)
{
	do
	{
	   m_current_rect++;

	   TRect r = PhImage2->GetSelRect();

		awpRect rr;

		rr.left = r.left;
		rr.top  = r.top;
		rr.right = r.right;
		rr.bottom = r.bottom;

		TLFRect rect;
		rect.SetRect(rr);


		ILFScanner* s = this->m_ObjectEngine.GetScanner(0);
		if (m_current_rect >= s->GetFragmentsCount())
		{
			this->Timer1->Enabled = false;
			m_current_rect = 0;
			this->DrawScene();
		}
		awpRect r1 = s->GetFragmentRect(m_current_rect);
		TLFRect dr;
		dr.SetRect(r1);
		TRect  rect3(r1.left, r1.top, r1.right, r1.bottom);
		TRect  rect4 = PhImage2->GetScreenRect(rect3);
		float overlap = rect.RectOverlap(dr);
		if (overlap > 0.5)
		{
			  //if (this->m_ObjectEngine.DetectInRect(&r1))
			  {
				awpRect rect = r1;
				TRect*   r = new TRect(rect.left, rect.top, rect.right, rect.bottom);
				m_objects->Add(r);
			  }
			  if (EngineViewForm->Visible)
				EngineViewForm->DrawEngine(&r1);
			  DrawScene();
			  TColor oldColor = PhImage2->Canvas->Pen->Color;
			  PhImage2->Canvas->Pen->Color = clSilver;
			  PhImage2->Canvas->Rectangle(rect4);
			  PhImage2->Canvas->Pen->Color = oldColor;

			  break;
		}
	}while (true);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::EditCopy1Execute(TObject *Sender)
{
	PhImage2->SaveToClipBoard();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::EditCopy1Update(TObject *Sender)
{
	EditCopy1->Enabled = !PhImage2->Empty;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ModeLenzActionExecute(TObject *Sender)
{
      PhImage2->SelectPhTool(PhLenzTool1);
      PhImage2->Paint();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ModeLenzActionUpdate(TObject *Sender)
{
	ModeLenzAction->Checked = dynamic_cast< TPhLenzTool*>(PhImage2->PhTool) != NULL;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::DirectoryListBox1Change(TObject *Sender)
{
	ModePaneActionExecute(NULL);
	AnsiString str = DirectoryListBox1->Directory;

	// open database
	OpenDatabase(str.c_str());
	if (FragmentForm!= NULL && FragmentForm->Visible)
	{
		FragmentForm->ChangeDictonary();
	}
    IniParams->LastPath = DirectoryListBox1->Directory;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Panel1Resize(TObject *Sender)
{
	PhImage2->BestFit();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::DbClearSelectionActionExecute(TObject *Sender)
{
	// todo: clear selection
}
//---------------------------------------------------------------------------
void __fastcall TForm1::DtInfoActionExecute(TObject *Sender)
{
	DetectorForm->Label5->Caption = ExtractFileName(this->m_strEngineName);
	DetectorForm->Label6->Caption = IntToStr(this->m_ObjectEngine.GetDetector(0)->GetStagesCount());
	int count = 0;
	TSCObjectDetector* d = (TSCObjectDetector*)this->m_ObjectEngine.GetDetector(0);
	for (int i = 0; i < this->m_ObjectEngine.GetDetector(0)->GetStagesCount(); i++)
		count += d->GetSensorsCount(i);
	DetectorForm->Label7->Caption = IntToStr(count);
	DetectorForm->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::DtInfoActionUpdate(TObject *Sender)
{
	DtInfoAction->Enabled = m_ObjectEngine.GetDetector(0) != NULL;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ViewFarthestOverlapsActionExecute(TObject *Sender)
{
	m_draw_farthest_overlaps = !m_draw_farthest_overlaps;
	//this->DrawScene();
	PhImage2->Paint();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ViewFarthestOverlapsActionUpdate(TObject *Sender)
{
//
	ViewFarthestOverlapsAction->Checked = m_draw_farthest_overlaps;
}
//---------------------------------------------------------------------------
awpRect __fastcall TForm1::FindnearestOverlap(awpRect rr)
{
   awpRect result = rr;
	TLFRect rect;
	rect.SetRect(rr);
	 double nearlest = 0;
	 m_scanner.Scan(PhImage2->Bitmap->Width, PhImage2->Bitmap->Height);
	 for (int j = 0; j < m_scanner.GetFragmentsCount(); j++)
	 {
		awpRect r1 = m_scanner.GetFragmentRect(j);
		TLFRect dr;
		dr.SetRect(r1);
		float overlap = rect.RectOverlap(dr);
		if (overlap > nearlest)
		{
		  nearlest = overlap;
		  result = dr.GetRect();
		}
	 }


   return result;
}

void __fastcall TForm1::DbMarkingActionExecute(TObject *Sender)
{
 		LongProcDlg->ProcType = ptAnalysis;
		LongProcDlg->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::DbMarkingActionUpdate(TObject *Sender)
{
	DbMarkingAction->Enabled = m_ObjectEngine.GetDetector(0) != NULL;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::DtViewActionExecute(TObject *Sender)
{
	EngineViewForm->Visible = !EngineViewForm->Visible;
	if (EngineViewForm->Visible)
	  EngineViewForm->engine = &this->m_ObjectEngine;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::DtViewActionUpdate(TObject *Sender)
{
	DtViewAction->Enabled =  m_ObjectEngine.GetDetector(0) != NULL;
	DtViewAction->Checked =  EngineViewForm->Visible;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ViewSemanticEditorActionExecute(TObject *Sender)
{
	FragmentForm->Visible = !FragmentForm->Visible;
    if (FragmentForm->Visible)
       FragmentForm->DrawSelected();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::DtErrorsActionExecute(TObject *Sender)
{
	EngineErrDlg->Edit1->Text = DirectoryListBox1->Directory;
	EngineErrDlg->SetEngine(&m_ObjectEngine);
	if (EngineErrDlg->ShowModal() == mrOk)
	{

    }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::dbDictionaryActionExecute(TObject *Sender)
{
	if (dictinaryEditDlg->EditDatabase())
	{
		//update database with new dictioanry
		AnsiString _astr = m_db.DbName;
		_astr += "\\";
		_astr += c_lpDictFileName;
		if (dictinaryEditDlg->Dictionary->SaveXML(_astr.c_str()))
		{
		   Memo1->Lines->Add(L"INFO: start update database " + m_db.DbName);
           m_db.Dictionary->LoadXML(_astr.c_str());
           //
           LongProcDlg->ProcType = ptUpdate;
		   LongProcDlg->ShowModal();
		   Memo1->Lines->Add(L"INFO: done. ");
		}
		else
		{
			Memo1->Lines->Add(L"ERROR: cannot update database " + m_db.DbName);
		}
		OpenDatabase(m_db.DbName.c_str());
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm1::dbDictionaryActionUpdate(TObject *Sender)
{
   UnicodeString _ustr = this->DirectoryListBox1->Directory;
   _ustr += L"\\";
   _ustr += c_lpDictFileName;
   AnsiString _astr = _ustr;
   std::string fileName = _astr.c_str();
   dbDictionaryAction->Enabled = LFFileExists(fileName);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ViewSemanticEditorActionUpdate(TObject *Sender)
{
   ViewSemanticEditorAction->Enabled = PageControl1->ActivePageIndex == 2;//
   ViewSemanticEditorAction->Checked = FragmentForm->Visible;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::DbSplitDatabaseExecute(TObject *Sender)
{
	DbSplitDlg->Edit1->Text = DirectoryListBox1->Directory +"\\train\\";
	DbSplitDlg->Edit2->Text = DirectoryListBox1->Directory +"\\test\\";
	if(DbSplitDlg->ShowModal() == mrOk)
	{
		LongProcDlg->ProcType = ptSplit;
		LongProcDlg->ShowModal();
		this->DirectoryListBox1->Update();
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FImage1MouseMove(TObject *Sender, TShiftState Shift, int X,
          int Y)
{
	// отображение текущей информации о местоположении мыши внутри изображения.
	if (PhImage2->Empty)
    	return;
    int x = PhImage2->GetImageX(X);
    int y = PhImage2->GetImageY(Y);
    AnsiString strStatusText = "x=" + IntToStr(x) + ":y=" + IntToStr(y);
    if (PhImage2->HasSelection())
    {
    	strStatusText += "roi: ";
        TRect selRect = PhImage2->GetSelRect();
        strStatusText += IntToStr((int)selRect.left);
    	strStatusText += ":";
        strStatusText += IntToStr((int)selRect.top);
    	strStatusText += ":";
        strStatusText += IntToStr(selRect.Width());
    	strStatusText += ":";
        strStatusText += IntToStr(selRect.Height());
    }
	StatusBar1->Panels->Items[3]->Text = strStatusText;
}
//---------------------------------------------------------------------------
// Select ROI tool enable-disable
void __fastcall TForm1::ModeSelectRectActionExecute(TObject *Sender)
{
	PhImage2->SelectPhTool(PhSelRectTool1);
    PhImage2->Paint();
}
//---------------------------------------------------------------------------
//
void __fastcall TForm1::ModeSelectRectActionUpdate(TObject *Sender)
{
//
}
//---------------------------------------------------------------------------


void __fastcall TForm1::ApplicationEvents1Idle(TObject *Sender, bool &Done)
{
  if (PhImage2->PhTool != NULL)
   {
	   StatusBar1->Panels->Items[1]->Text = PhImage2->PhTool->ToolName;
   }
   Panel2->Caption = L"Total: " + IntToStr(DbView->Items->Count) + L" selected: " + IntToStr(DbView->SelCount);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ImageCropActionExecute(TObject *Sender)
{
	Memo1->Lines->Add("crop image...");
    if (!RemoveMarkupHelper())
         return;

    //Crop Image
    awpImage* pImage = NULL;
    PhImage2->GetAwpImage(&pImage);
    if (pImage != NULL)
    {
      TRect r = PhImage2->GetSelRect();
	  awpRect rect = TRect2awpRect(r);
      awpImage* fragment = NULL;
      if (awpCopyRect(pImage, &fragment, &rect) == AWP_OK)
      	ImageProcessingHelper(fragment);
      else
       Memo1->Lines->Add("cannot crop rectange.");
       awpReleaseImage(&pImage);
    }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ImageCropActionUpdate(TObject *Sender)
{
	ImageCropAction->Enabled = !PhImage2->Empty && PhImage2->HasSelection();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ImageProcessingHelper(awpImage* pImage)
{
      AnsiString strExt = ExtractFileExt(FileListBox1->FileName);
      AnsiString strFileName = m_strFileName;//FileListBox1->FileName;
#ifdef _DEBUG
    Memo1->Lines->Add("FileName = " + strFileName);
#endif

	  if (strExt != ".awp" && strExt != ".jpg" && strExt != ".jpeg")
          strFileName = ChangeFileExt(strFileName, ".awp");

      if (awpSaveImage(strFileName.c_str(), pImage) == AWP_OK)
      {
        if (m_strFileName != strFileName)
        {
            DeleteFile(m_strFileName);
            m_strFileName = strFileName;
            int ItemIndex = FileListBox1->ItemIndex;
            FileListBox1->Items->Strings[FileListBox1->ItemIndex] = ExtractFileName(strFileName);
            FileListBox1->Selected[ItemIndex] = true;
        }

        PhImage2->InitFile(m_strFileName);
        this->RepaintImage();
      }
      else
       Memo1->Lines->Add("ERROR: cannot save image.");
}

void __fastcall TForm1::ImageInvertActionExecute(TObject *Sender)
{
	Memo1->Lines->Add("ACTION: invert image." + m_strFileName);
    awpImage* pImage = NULL;
    PhImage2->GetAwpImage(&pImage);
    if (pImage != NULL)
    {
      awpImage* white = NULL;
	  awpCopyImage(pImage, &white);
      awpFill(white, 255);
      if (awpCalcImage(white, pImage, NULL, AWP_CALC_SUBIMAGES, AWP_CALC_INPLACE) == AWP_OK)
      {
      	awpConvert(pImage, AWP_CONVERT_TO_BYTE);
      	ImageProcessingHelper(pImage);
      }
      else
       Memo1->Lines->Add("ERROR: cannot invert image.");
       awpReleaseImage(&pImage);
    }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ImageInvertActionUpdate(TObject *Sender)
{
	ImageInvertAction->Enabled = !PhImage2->Empty;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ImageGrayscaleActionExecute(TObject *Sender)
{
	Memo1->Lines->Add("ACTION: convert image to grayscale." + m_strFileName);

    awpImage* pImage = NULL;
    PhImage2->GetAwpImage(&pImage);
    if (pImage != NULL)
    {
      if (awpConvert(pImage, AWP_CONVERT_3TO1_BYTE) == AWP_OK)
      	ImageProcessingHelper(pImage);
      else
       Memo1->Lines->Add("ERROR: cannot convert image to grayscale.");

      awpReleaseImage(&pImage);
    }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ImageGrayscaleActionUpdate(TObject *Sender)
{
	ImageGrayscaleAction->Enabled =  !PhImage2->Empty;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ImageMirrorActionExecute(TObject *Sender)
{
       if (!RemoveMarkupHelper())
             return;
	Memo1->Lines->Add("ACTION: mirror image." + m_strFileName);
    awpImage* pImage = NULL;
    PhImage2->GetAwpImage(&pImage);
    if (pImage != NULL)
    {
	  if (awpFlip(&pImage, AWP_FLIP_HRZT) == AWP_OK)
      	ImageProcessingHelper(pImage);
      else
       Memo1->Lines->Add("ERROR: cannot mirror image.");
      awpReleaseImage(&pImage);
    }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ImageMirrorActionUpdate(TObject *Sender)
{
	ImageMirrorAction->Enabled = !PhImage2->Empty;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ImageFlipActionExecute(TObject *Sender)
{
   if (!RemoveMarkupHelper())
         return;
	Memo1->Lines->Add("ACTION: flip vertical image." + m_strFileName);
    awpImage* pImage = NULL;
    PhImage2->GetAwpImage(&pImage);
    if (pImage != NULL)
    {
      if (awpFlip(&pImage, AWP_FLIP_VERT) == AWP_OK)
      	ImageProcessingHelper(pImage);
      else
       Memo1->Lines->Add("ERROR: flip vertical image.");
      awpReleaseImage(&pImage);
    }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ImageFlipActionUpdate(TObject *Sender)
{
	ImageFlipAction->Enabled = !PhImage2->Empty;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Flip1Click(TObject *Sender)
{
        if (!RemoveMarkupHelper())
			 return;

        //FlipHelper
        awpImage* pImage = NULL;
        PhImage2->GetAwpImage(&pImage);
        if (pImage != NULL)
        {
          awpFlip(&pImage, AWP_FLIP_HRZT);
          AnsiString strExt = ExtractFileExt(FileListBox1->FileName);
          AnsiString strFileName = FileListBox1->FileName;

          if (strExt != ".awp" && strExt != ".jpg" && strExt != ".jpeg")
              strFileName = ChangeFileExt(strFileName, ".awp");

          if (awpSaveImage(strFileName.c_str(), pImage) == AWP_OK)
          {
            if (m_strFileName != strFileName)
            {
                DeleteFile(m_strFileName);
                m_strFileName = strFileName;
                FileListBox1->Items->Strings[FileListBox1->ItemIndex] = ExtractFileName(strFileName);
            }

            PhImage2->InitFile(m_strFileName);
            this->RepaintImage();
          }

          awpReleaseImage(&pImage);
        }
}
void __fastcall TForm1::FlipLeft1Click(TObject *Sender)
{
  if (!RemoveMarkupHelper())
         return;
	Memo1->Lines->Add("ACTION: flip left image." + m_strFileName);
    awpImage* pImage = NULL;
    PhImage2->GetAwpImage(&pImage);
    if (pImage != NULL)
    {
      if (awpFlip(&pImage, AWP_FLIP_LEFT) == AWP_OK)
      	ImageProcessingHelper(pImage);
	  else
       Memo1->Lines->Add("ERROR: flip left image.");
      awpReleaseImage(&pImage);
    }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ImageFlipLeftActionUpdate(TObject *Sender)
{
	ImageFlipLeftAction->Enabled = !PhImage2->Empty;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ImageFlipRightActionExecute(TObject *Sender)
{
  if (!RemoveMarkupHelper())
         return;
	Memo1->Lines->Add("ACTION: flip right image." + m_strFileName);
    awpImage* pImage = NULL;
    PhImage2->GetAwpImage(&pImage);
    if (pImage != NULL)
    {
      if (awpFlip(&pImage, AWP_FLIP_RGHT) == AWP_OK)
      	ImageProcessingHelper(pImage);
      else
       Memo1->Lines->Add("ERROR: flip right image.");
      awpReleaseImage(&pImage);
    }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ImageFlipRightActionUpdate(TObject *Sender)
{
	ImageFlipRightAction->Enabled = !PhImage2->Empty;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ImageRotateActionExecute(TObject *Sender)
{
    RotateDlg->CSpinEdit1->Value = 0;
    if (RotateDlg->ShowModal() != mrOk)
	   return;

    if (!RemoveMarkupHelper())
         return;
    int angle = RotateDlg->CSpinEdit1->Value;
	Memo1->Lines->Add("ACTION: rotate image." + m_strFileName);
    awpImage* pImage = NULL;
    PhImage2->GetAwpImage(&pImage);
    if (pImage != NULL)
    {
      if (awpRotateBilinear(pImage, angle) == AWP_OK)
      	ImageProcessingHelper(pImage);
      else
       Memo1->Lines->Add("ERROR: rotate image.");
      awpReleaseImage(&pImage);
    }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ImageRotateActionUpdate(TObject *Sender)
{
	ImageRotateAction->Enabled = !PhImage2->Empty;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ImageSobelActionExecute(TObject *Sender)
{
	Memo1->Lines->Add("ACTION: sobel filter." + m_strFileName);
    awpImage* pImage = NULL;
    PhImage2->GetAwpImage(&pImage);
    if (pImage != NULL)
    {
      awpImage* imgAmpl = NULL;
      awpImage* imgDir  = NULL;
      try
      {
          if (awpConvert(pImage, AWP_CONVERT_3TO1_BYTE) != AWP_OK)
            throw Exception("Cannot convert image");
       	  if (awpCreateImage(&imgAmpl, pImage->sSizeX, pImage->sSizeY, pImage->bChannels, AWP_BYTE) != AWP_OK)
          	throw Exception("Cannot create image");
       	  if (awpCreateImage(&imgDir, pImage->sSizeX, pImage->sSizeY, pImage->bChannels, AWP_BYTE) != AWP_OK)
			throw Exception("Cannot create image");
          if (awpEdgeSobel(pImage, imgAmpl, NULL) == AWP_OK)
            ImageProcessingHelper(imgAmpl);
          else
          	throw Exception("Cannot do sobel filter.");
      }
      catch(Exception& e)
      {
	       Memo1->Lines->Add("ERROR: sobel filter. " + e.Message);
      }
      _AWP_SAFE_RELEASE_(pImage)
      _AWP_SAFE_RELEASE_(imgAmpl)
      _AWP_SAFE_RELEASE_(imgDir)
    }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ImageSobelActionUpdate(TObject *Sender)
{
	ImageSobelAction->Enabled = !PhImage2->Empty;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ImageFilterActionExecute(TObject *Sender)
{
    if (FilterDlg->ShowModal() != mrOk)
       return;

    int filterIndex  = FilterDlg->ListBox1->ItemIndex;
	Memo1->Lines->Add("ACTION: linear image filtering." + m_strFileName);
    awpImage* pImage = NULL;
    awpImage* dst = NULL;
    PhImage2->GetAwpImage(&pImage);
    PhImage2->GetAwpImage(&dst);
    if (pImage != NULL)
    {
      if (awpFilter(pImage, dst,  filterIndex) == AWP_OK)
      	ImageProcessingHelper(dst);
      else
       Memo1->Lines->Add("ERROR: linear image filtering.");
      awpReleaseImage(&pImage);
	  awpReleaseImage(&dst);
    }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ImageFilterActionUpdate(TObject *Sender)
{
	ImageFilterAction->Enabled = !PhImage2->Empty;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ImageGaussFilterActionExecute(TObject *Sender)
{
    if (GaussFilterDlg->ShowModal() != mrOk)
       return;

    double sigma  = 0.1*(double)GaussFilterDlg->CSpinEdit1->Value;
	Memo1->Lines->Add("ACTION: gauss filter." + m_strFileName);
    awpImage* pImage = NULL;
    awpImage* dst = NULL;
    PhImage2->GetAwpImage(&pImage);
    PhImage2->GetAwpImage(&dst);
    if (pImage != NULL)
    {
      if (awpGaussianBlur(pImage, dst,  sigma) == AWP_OK)
      	ImageProcessingHelper(dst);
      else
       Memo1->Lines->Add("ERROR: gauss filter.");
      awpReleaseImage(&pImage);
      awpReleaseImage(&dst);
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ImageGaussFilterActionUpdate(TObject *Sender)
{
	ImageGaussFilterAction->Enabled = !PhImage2->Empty;
}
//---------------------------------------------------------------------------


void __fastcall TForm1::ImageMedianFilterActionExecute(TObject *Sender)
{
    if (MedianFilterDlg->ShowModal() != mrOk)
       return;

    unsigned char radius = MedianFilterDlg->CSpinEdit1->Value;
	Memo1->Lines->Add("ACTION: median filter." + m_strFileName);
	awpImage* pImage = NULL;
	awpImage* dst = NULL;
	PhImage2->GetAwpImage(&pImage);
	PhImage2->GetAwpImage(&dst);
	if (pImage != NULL)
	{
	  if (awpMedian(pImage, dst,  radius) == AWP_OK)
		ImageProcessingHelper(dst);
	  else
	   Memo1->Lines->Add("ERROR: median filter.");
	  awpReleaseImage(&pImage);
	  awpReleaseImage(&dst);
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ImageMedianFilterActionUpdate(TObject *Sender)
{
	ImageMedianFilterAction->Enabled = !PhImage2->Empty;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ImageResizeActionExecute(TObject *Sender)
{
	ResizeDlg->CurrentWidth = PhImage2->Bitmap->Width;
	ResizeDlg->CurrentHeight = PhImage2->Bitmap->Height;
	if (!ResizeDlg->CheckBox2->Checked)
	{
		ResizeDlg->CheckBox1->Checked = true;
		ResizeDlg->RadioButton1->Checked = true;
	}

	if (ResizeDlg->ShowModal() != mrOk)
		return;
	 int _width = ResizeDlg->NewWidth;
	 int _height = ResizeDlg->NewHeight;

	Memo1->Lines->Add("ACTION: image resize." + m_strFileName);
	awpImage* pImage = NULL;
	awpImage* dst = NULL;
	PhImage2->GetAwpImage(&pImage);
	if (pImage != NULL)
	{
	  if (awpResizeBilinear(pImage, _width,  _height) == AWP_OK)
		ImageProcessingHelper(pImage);
	  else
	   Memo1->Lines->Add("ERROR: image resize.");
	  awpReleaseImage(&pImage);
	  awpReleaseImage(&dst);
	}

}
//---------------------------------------------------------------------------

void __fastcall TForm1::ImageResizeActionUpdate(TObject *Sender)
{
	ImageResizeAction->Enabled = !PhImage2->Empty;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::PageControl1Change(TObject *Sender)
{
    if (PageControl1->TabIndex == 1)
        ValueListEditor1->Visible = true;
    else
        ValueListEditor1->Visible = false;

    // control TableView
    if (PageControl1->ActivePageIndex == 2)
    {
       TableForm->Visible = TableVisible;
       FragmentForm->Visible = FragmentsVisible;
       if (m_markToolSelected)
       		ModeMarkRectActionExecute(NULL);
    }
    else
	{
       TableVisible = TableForm->Visible;
       FragmentsVisible = FragmentForm->Visible;
       m_markToolSelected = ModeMarkRectAction->Checked;
       ModePaneActionExecute(NULL);

       TableForm->Visible = false;
       FragmentForm->Visible = false;
    }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::InitDbView()
{
    DbView->Columns->Clear();
    TListColumn  *NewColumn;
    TListItem  *ListItem;

    NewColumn = DbView->Columns->Add();
    NewColumn->Caption = "File Name";

    NewColumn = DbView->Columns->Add();
    NewColumn->Caption = "Items count";
}

void __fastcall TForm1::DbViewClick(TObject *Sender)
{
//
}
//---------------------------------------------------------------------------
void __fastcall TForm1::UpdateDbView()
{
	TListItem  *ListItem;
    DbView->Items->Clear();
    DbView->Columns->Items[0]->Width = 100;
    for (int i = 0; i < m_db.NumImages; i++)
	{
        TLFDBSemanticDescriptor* d = m_db.Data->GetDescriptor(i);
        std::string str = LFGetFileName(d->GetImageFile());
        str += LFGetFileExt(d->GetImageFile());
        ListItem = DbView->Items->Add();
        ListItem->Caption = str.c_str();
		ListItem->SubItems->Add(d->GetCount());
    }
    DbView->Columns->Items[0]->Width = -1;
}

void __fastcall TForm1::DbViewSelectItem(TObject *Sender, TListItem *Item, bool Selected)
{
	assert(Item != NULL);
    if (!Selected)
        return;
    UnicodeString str = m_db.Data->GetPath().c_str();
	str += Item->Caption;
	m_strFileName = str;
	UnicodeString strFileName = ExtractFileName(Item->Caption);
	InitImageFile(m_strFileName);

 //	int idx = FileListBox1->Items->IndexOf(strFileName);
 //	FileListBox1->Selected[idx] = true;
 //   if (m_strFileName != "")
 //        InitImageFile(m_strFileName);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::PhImage2AfterOpen(TObject *Sender)
{
	UpdateSatatusBar();
	PhImage2->BestFit();

//---
    m_Selected = -1;
    this->m_objects->Clear();
    this->m_rois.Clear();
     if( FileExists(ChangeFileExt( m_strFileName, ".xml" )))
     {
        AnsiString strFileName = ChangeFileExt( m_strFileName, ".xml" );
        if (!this->m_Descr.LoadXML(strFileName.c_str()))
        {
            ShowMessage("Cannot load description " + strFileName);
            return;
        }
        FragmentForm->SDescriptor = &this->m_Descr;
     }

	Detect();

}
//---------------------------------------------------------------------------
void __fastcall TForm1::DbCreateActionExecute(TObject *Sender)
{
	if (dictinaryEditDlg->CreateDatabase())
	{
	   UnicodeString _ustr = this->DirectoryListBox1->Directory;
	   _ustr += L"\\";
	   _ustr += c_lpDictFileName;
	   AnsiString _astr = _ustr;
	   std::string fileName = _astr.c_str();
	   if (dictinaryEditDlg->Dictionary->SaveXML(fileName.c_str()))
	   {
			Memo1->Lines->Add(L"INFO: database was created in the " + _ustr);
	   }
	   else
	   {
			Memo1->Lines->Add(L"ERROR: database was not created in the " + _ustr);
			return;
	   }
	   // try to open database
	   _astr = DirectoryListBox1->Directory;
	   OpenDatabase(_astr.c_str());
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm1::DbCreateActionUpdate(TObject *Sender)
{
   UnicodeString _ustr = this->DirectoryListBox1->Directory;
   _ustr += L"\\";
   _ustr += c_lpDictFileName;
   AnsiString _astr = _ustr;
   std::string fileName = _astr.c_str();
   DbCreateAction->Enabled = !LFFileExists(fileName);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::OpenDatabase(const char* lpDbName)
{
	AnsiString str = lpDbName;
	if (!m_db.Init(str))
	{
		Memo1->Lines->Add(L"ERROR: cannot open database... " + str);
		TabSheet3->TabVisible = false;
		//N1->Visible = false;
		DbView->Items->Clear();
	}
	else
	{
		TabSheet3->TabVisible = true;
		Memo1->Lines->Add(L"INFO: open database... " + str);
		//N1->Visible = true;
		// update visual table
		UpdateDbView();
	}
}

void __fastcall TForm1::DbClearActionExecute(TObject *Sender)
{
    String msg = L"Are you sure you want to delete all markup data from the database? This operation cannot be undone.";
    if (Application->MessageBoxW(msg.c_str(), Application->Name.c_str(), MB_YESNO  | MB_ICONQUESTION) == IDNO)
		return;

       LongProcDlg->ProcType = ptClear;
	   LongProcDlg->ShowModal();
       DirectoryListBox1Change(NULL);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::DbClearActionUpdate(TObject *Sender)
{
   UnicodeString _ustr = m_db.DbName;
   _ustr += L"\\";
   _ustr += c_lpDictFileName;
   AnsiString _astr = _ustr;
   std::string fileName = _astr.c_str();
   DbClearAction->Enabled = LFFileExists(fileName);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ToolChange(TObject *Sender)
{
    TPhImageMarkTool* tool = (TPhImageMarkTool*)Sender;
	m_Descr.LoadXML(tool->DescrFile.c_str());
	TableForm->UpdateTable();
	if (DbView->ItemIndex >= 0)
	{
		TListItem* li = DbView->Items->Item[DbView->ItemIndex];
		li->SubItems->Strings[0] = IntToStr(m_Descr.GetCount());
	}
}

void __fastcall TForm1::DbViewKeyUp(TObject *Sender, WORD &Key, TShiftState Shift)

{
	if (Key == VK_DELETE)
	{
		ImageDelImageActionExecute(NULL);
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::PhImage2Paint(TObject *Sender)
{
	DrawScene();
}
//---------------------------------------------------------------------------


void __fastcall TForm1::ModeMarkVectorActionExecute(TObject *Sender)
{
	if (m_markTool != NULL)
		delete m_markTool;
	m_markTool = new TPhImageMarkTool(NULL);
	m_markTool->Mode = MTVector;
	m_markTool->PhImage = PhImage2;
	m_markTool->dictinary = m_db.Dictionary;
	m_markTool->OnChange = ToolChange;
	PhImage2->SelectPhTool(m_markTool);
	AnsiString strFileName = ChangeFileExt( m_strFileName, ".xml" );
	std::string str = strFileName.c_str();
	if (LFFileExists(str.c_str()))
		m_markTool->SetFrame(str.c_str());
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ModeMarkVectorActionUpdate(TObject *Sender)
{
	ModeMarkVectorAction->Enabled = this->PageControl1->ActivePageIndex == 2;
	TPhImageMarkTool* tool = dynamic_cast< TPhImageMarkTool*>(PhImage2->PhTool);
	ModeMarkVectorAction->Checked = tool != NULL && tool->Mode == MTVector;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ModeMarkPolygonActionExecute(TObject *Sender)
{
	if (m_markTool != NULL)
		delete m_markTool;
	m_markTool = new TPhImageMarkTool(NULL);
	m_markTool->Mode = MTContour;
	m_markTool->PhImage = PhImage2;
	m_markTool->dictinary = m_db.Dictionary;
	m_markTool->OnChange = ToolChange;
	PhImage2->SelectPhTool(m_markTool);
	AnsiString strFileName = ChangeFileExt( m_strFileName, ".xml" );
	std::string str = strFileName.c_str();
	if (LFFileExists(str.c_str()))
		m_markTool->SetFrame(str.c_str());
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ModeMarkPolygonActionUpdate(TObject *Sender)
{
	ModeMarkPolygonAction->Enabled = this->PageControl1->ActivePageIndex == 2;
	TPhImageMarkTool* tool = dynamic_cast< TPhImageMarkTool*>(PhImage2->PhTool);
	ModeMarkPolygonAction->Checked = tool != NULL && tool->Mode == MTContour;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::DbOpenActionExecute(TObject *Sender)
{
	if (OpenDialog1->Execute())
	{
        AnsiString str = ExtractFileDir(OpenDialog1->FileName);
		//open database
		OpenDatabase(str.c_str());
		if (FragmentForm->Visible)
		{
			FragmentForm->ChangeDictonary();
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ModeAngleActionExecute(TObject *Sender)
{
	PhImage2->SelectPhTool(PhTriangleTool1);
	PhImage2->Paint();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ModeAngleActionUpdate(TObject *Sender)
{
	ModeAngleAction->Checked = dynamic_cast< TPhTriangleTool*>(PhImage2->PhTool) != NULL;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ModeRulerActionExecute(TObject *Sender)
{
	PhImage2->SelectPhTool(PhRulerTool1);
	PhImage2->Paint();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ModeRulerActionUpdate(TObject *Sender)
{
	ModeRulerAction->Checked = dynamic_cast< TPhRulerTool*>(PhImage2->PhTool) != NULL;
}
//---------------------------------------------------------------------------


void __fastcall TForm1::DtCreateActionExecute(TObject *Sender)
{
	AnsiString str = ExtractFilePath(Application->ExeName);
	str += "\\CSBuilder64.exe";
	UINT result = WinExec(str.c_str(), SW_SHOWNORMAL);
	if (result < 31)
		Memo1->Lines->Add(L"ERROR: cannot execute builder. Error code =  " + IntToStr((int)result));
}
//---------------------------------------------------------------------------

void __fastcall TForm1::DtCreateActionUpdate(TObject *Sender)
{
//
}
//---------------------------------------------------------------------------

void __fastcall TForm1::DtUpdateActionExecute(TObject *Sender)
{
//
}
//---------------------------------------------------------------------------

void __fastcall TForm1::DtUpdateActionUpdate(TObject *Sender)
{
//
}
//---------------------------------------------------------------------------

