//---------------------------------------------------------------------------

#ifndef MainFormH
#define MainFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "_LF.h"

#include <ExtCtrls.hpp>
#include <FileCtrl.hpp>
#include <Dialogs.hpp>
#include <ActnList.hpp>
#include <Menus.hpp>
#include <Buttons.hpp>
#include <Grids.hpp>
#include <ComCtrls.hpp>
#include <ToolWin.hpp>
#include <ImgList.hpp>
#include <AppEvnts.hpp>
#include <AppEvnts.hpp>
#include <ValEdit.hpp>
#include <System.Actions.hpp>
#include <Vcl.StdActns.hpp>

#include "DbLabeledImages.h"
#include <System.ImageList.hpp>
#include "FImage41.h"
#include "PhImageTool.h"
#include "PhLenzTool.h"
#include "PhPaneTool.h"
#include "PhSelectRectTool.h"
#include "PhZoomToRectTool.h"
#include "PhImageMarkTool.h"
#include "PhRulerTool.h"
#include "PhTriangleTool.h"
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
    TOpenDialog *OpenDialog1;
    TMainMenu *MainMenu1;
    TMenuItem *File1;
    TMenuItem *Close1;
    TMenuItem *Tool1;
    TActionList *ActionList2;
    TAction *CloseAction;
    TToolBar *ToolBar1;
    TImageList *ImageList1;
    TToolButton *ToolButton10;
    TToolButton *ToolButton11;
    TToolButton *ToolButton6;
    TToolButton *ToolButton9;
    TStatusBar *StatusBar1;
        TToolButton *ToolButton1;
        TMenuItem *N1;
        TMenuItem *N2;
        TMenuItem *N3;
        TAction *FileOpenDetectorAction;
        TAction *ModePaneAction;
        TAction *ModeZoomAction;
        TAction *ModeBestFitAction;
        TAction *ModeActualSizeAction;
        TMenuItem *N4;
        TMenuItem *N5;
        TMenuItem *N9;
        TMenuItem *N11;
    TAction *ImageDelImageAction;
        TAction *DtDetectAction;
        TAction *AboutAction;
        TMenuItem *N16;
        TMenuItem *N23;
        TMenuItem *N25;
        TToolButton *ToolButton12;
        TToolButton *ToolButton14;
        TApplicationEvents *ApplicationEvents1;
    TAction *ModeMarkRectAction;
    TMenuItem *SetRect1;
    TToolButton *ToolButton4;
    TMenuItem *View1;
    TAction *ViewToolbarAction;
    TAction *ViewStatusbarAction;
    TAction *ViewDirectoryAction;
    TAction *ViewTableAction;
    TMenuItem *Toolbar2;
    TMenuItem *StatusBar2;
    TMenuItem *DirectoryPane1;
    TMenuItem *ViewTable1;
    TSaveDialog *SaveDialog1;
        TMenuItem *I1;
        TMenuItem *N12;
        TAction *DbInfoAction;
        TMenuItem *DatabaseInfo1;
    TMenuItem *N13;
    TAction *ViewOlerlapsAction;
    TMenuItem *Overlaps1;
    TAction *DbExportFragmentsAction;
    TMenuItem *Exportimages1;
    TAction *DtErrorsAction;
    TMenuItem *Engineerrors1;
	TAction *ViewSubsetAction;
    TAction *ViewSemanticOutputAction;
    TAction *DbConvertDatabaseAction;
    TMenuItem *N10;
    TMenuItem *Exportawpimages1;
    TMenuItem *N15;
    TMenuItem *ViewSemanticOutput1;
    TToolButton *ToolButton8;
    TMenuItem *Tools1;
    TMenuItem *Options1;
    TMenuItem *N17;
    TAction *DbCopyAction;
    TMenuItem *CopyDatabase1;
    TPopupMenu *PopupMenu1;
    TAction *ImageCopyImageAction;
    TMenuItem *CopyImage1;
    TMenuItem *N8;
    TMenuItem *DeleteImage1;
    TTimer *Timer1;
    TAction *ViewDbTubinalsAction;
	TEditCopy *EditCopy1;
	TMenuItem *Copy1;
	TAction *ModeLenzAction;
	TMenuItem *ModeLenzAction1;
	TAction *ModeFaceAction;
	TAction *ViewSemanticThumbinalsAction;
	TMenuItem *ViewSemanticThumbnail1;
	TAction *ViewListAction;
	TAction *DbClearSelectionAction;
	TAction *DbInvertSelectionAction;
	TAction *DbDeleteSelectedAction;
	TAction *DbCopySelectedToAction;
	TAction *DbMoveSelectedToAction;
	TMenuItem *ClearSelection1;
	TMenuItem *InvertSelection1;
	TMenuItem *DeleteSelected1;
	TMenuItem *N18;
	TMenuItem *CopySelectedTo1;
	TMenuItem *DbMoveSelectedTo1;
	TAction *DbSelectAllAction;
	TMenuItem *SelectAll1;
	TAction *DtInfoAction;
	TMenuItem *Detectorinfo1;
	TAction *ViewFarthestOverlapsAction;
	TMenuItem *ViewFarthestOverlaps1;
	TMenuItem *N19;
	TAction *DbMarkingAction;
	TMenuItem *DoAnalysis1;
	TAction *DtViewAction;
	TMenuItem *Viewdetector1;
	TAction *ViewSemanticEditorAction;
	TMenuItem *ViewSemanticEditorAction1;
	TAction *dbDictionaryAction;
	TMenuItem *Dictionary1;
	TAction *DbSplitDatabase;
	TMenuItem *SplitDatabase1;
	TAction *ModeSelectRectAction;
	TMenuItem *SelectROI1;
	TToolButton *ToolButton2;
	TAction *ImageCropAction;
	TMenuItem *Crop1;
	TMenuItem *Copyfiles1;
	TMenuItem *N14;
	TMenuItem *Actualsize1;
	TMenuItem *BestFit1;
	TAction *ImageInvertAction;
	TMenuItem *Invert1;
	TAction *ImageGrayscaleAction;
	TMenuItem *Grayscale1;
	TAction *ImageMirrorAction;
	TMenuItem *Mirror1;
	TAction *ImageFlipAction;
	TMenuItem *FlipVertical1;
	TAction *ImageFlipLeftAction;
	TMenuItem *FlipLeft1;
	TAction *ImageFlipRightAction;
	TMenuItem *FlipRight1;
	TAction *ImageRotateAction;
	TMenuItem *Rotate2;
	TAction *ImageSobelAction;
	TMenuItem *SobelFilter1;
	TAction *ImageFilterAction;
	TMenuItem *ImageFilterAction1;
	TAction *ImageGaussFilterAction;
	TMenuItem *GaussFilter1;
	TAction *ImageMedianFilterAction;
	TMenuItem *MedianFilter1;
	TAction *ImageResizeAction;
	TMenuItem *Resize1;
	TPanel *Panel5;
	TPanel *Panel7;
	TMemo *Memo1;
	TSplitter *Splitter4;
	TPanel *Panel1;
	TPanel *Panel6;
	TPanel *Panel11;
	TSplitter *HSplitter;
	TSplitter *Splitter7;
	TPanel *BottomDocPanel;
	TPanel *Panel13;
	TSplitter *Splitter2;
	TPageControl *PageControl1;
	TTabSheet *TabSheet1;
	TTabSheet *TabSheet2;
	TDirectoryListBox *DirectoryListBox1;
	TSplitter *Splitter5;
	TPanel *Panel9;
	TDriveComboBox *DriveComboBox1;
	TFilterComboBox *FilterComboBox1;
	TFileListBox *FileListBox1;
	TTreeView *TreeView1;
	TTabSheet *TabSheet3;
	TPanel *Panel3;
	TValueListEditor *ValueListEditor1;
	TMenuItem *NewProject1;
	TListView *DbView;
	TPanel *Panel2;
	TSpeedButton *SpeedButton2;
	TSpeedButton *SpeedButton1;
	TPhImage *PhImage1;
	TPhImage *PhImage2;
	TPhZoomToRectTool *PhZoomToRectTool1;
	TPhSelRectTool *PhSelRectTool1;
	TPhPaneTool *PhPaneTool1;
	TPhLenzTool *PhLenzTool1;
	TAction *DbCreateAction;
	TMenuItem *CreateDatabase1;
	TAction *DbClearAction;
	TMenuItem *ClearDatabase1;
	TToolButton *ToolButton3;
	TToolButton *ToolButton5;
	TAction *ModeMarkVectorAction;
	TAction *ModeMarkPolygonAction;
	TAction *DbOpenAction;
	TMenuItem *Opendatabase1;
	TAction *ModeAngleAction;
	TPhRulerTool *PhRulerTool1;
	TPhTriangleTool *PhTriangleTool1;
	TAction *ModeRulerAction;
	TMenuItem *RulerRuler1;
	TMenuItem *AngleIAngle1;
	TAction *DtCreateAction;
	TAction *DtUpdateAction;
	TMenuItem *UpdateDetector1;
	TMenuItem *CreateDetector1;
    void __fastcall FileListBox1Change(TObject *Sender);
    void __fastcall FFaceEditor1AfterOpen(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall CloseActionExecute(TObject *Sender);
    void __fastcall FormResize(TObject *Sender);
    void __fastcall BestFitActionExecute(TObject *Sender);
    void __fastcall PaneActionExecute(TObject *Sender);
    void __fastcall ZoomToRectActionExecute(TObject *Sender);
    void __fastcall ActualSizeActionExecute(TObject *Sender);
        void __fastcall FormCreate(TObject *Sender);
		void __fastcall DtDetectActionExecute(TObject *Sender);
        void __fastcall DtDetectActionUpdate(TObject *Sender);
        void __fastcall DtClassifyActionExecute(TObject *Sender);
        void __fastcall ModeBestFitActionExecute(TObject *Sender);
        void __fastcall ModeActualSizeActionExecute(TObject *Sender);
        void __fastcall AboutActionExecute(TObject *Sender);
        void __fastcall DbSaveMarkActionExecute(TObject *Sender);
        void __fastcall ImageDelImageActionExecute(TObject *Sender);
        void __fastcall ImageDelImageActionUpdate(TObject *Sender);
        void __fastcall ModePaneActionUpdate(TObject *Sender);
        void __fastcall ModeZoomActionUpdate(TObject *Sender);
        void __fastcall ModeBestFitActionUpdate(TObject *Sender);
        void __fastcall ModeActualSizeActionUpdate(TObject *Sender);
        void __fastcall StatusBar1Resize(TObject *Sender);
        void __fastcall ApplicationEvents1Hint(TObject *Sender);
    void __fastcall ModeMarkRectActionExecute(TObject *Sender);
    void __fastcall ModeMarkRectActionUpdate(TObject *Sender);
    void __fastcall ViewToolbarActionExecute(TObject *Sender);
    void __fastcall ViewToolbarActionUpdate(TObject *Sender);
    void __fastcall ViewStatusbarActionExecute(TObject *Sender);
    void __fastcall ViewStatusbarActionUpdate(TObject *Sender);
    void __fastcall ViewDirectoryActionExecute(TObject *Sender);
    void __fastcall ViewDirectoryActionUpdate(TObject *Sender);
    void __fastcall ViewTableActionExecute(TObject *Sender);
    void __fastcall ViewTableActionUpdate(TObject *Sender);
        void __fastcall FormKeyPress(TObject *Sender, char &Key);
        void __fastcall Flip1Click(TObject *Sender);
        void __fastcall DbInfoActionExecute(TObject *Sender);
        void __fastcall DbInfoActionUpdate(TObject *Sender);
    void __fastcall ViewOlerlapsActionExecute(TObject *Sender);
    void __fastcall ViewOlerlapsActionUpdate(TObject *Sender);
    void __fastcall DbExportFragmentsActionExecute(TObject *Sender);
    void __fastcall DbExportFragmentsActionUpdate(TObject *Sender);
    void __fastcall ViewSemanticOutputActionExecute(TObject *Sender);
    void __fastcall ViewSemanticOutputActionUpdate(TObject *Sender);
    void __fastcall DbConvertDatabaseActionExecute(TObject *Sender);
    void __fastcall FileListBox1KeyUp(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall FileOpenDetectorActionExecute(TObject *Sender);
    void __fastcall BottomDocPanelDockDrop(TObject *Sender,
          TDragDockObject *Source, int X, int Y);
    void __fastcall BottomDocPanelDockOver(TObject *Sender,
          TDragDockObject *Source, int X, int Y, TDragState State,
          bool &Accept);
    void __fastcall BottomDocPanelGetSiteInfo(TObject *Sender,
          TControl *DockClient, TRect &InfluenceRect, TPoint &MousePos,
          bool &CanDock);
    void __fastcall BottomDocPanelUnDock(TObject *Sender, TControl *Client,
          TWinControl *NewTarget, bool &Allow);
    void __fastcall ModePaneActionExecute(TObject *Sender);
    void __fastcall ModeZoomActionExecute(TObject *Sender);
    void __fastcall FImage1Pane(TObject *Sender);
    void __fastcall FImage1ScaleChange(TObject *Sender);
    void __fastcall Options1Click(TObject *Sender);
    void __fastcall DbCopyActionExecute(TObject *Sender);
    void __fastcall DbCopyActionUpdate(TObject *Sender);
    void __fastcall ImageCopyImageActionExecute(TObject *Sender);
    void __fastcall ImageCopyImageActionUpdate(TObject *Sender);
    void __fastcall Timer1Timer(TObject *Sender);
	void __fastcall EditCopy1Execute(TObject *Sender);
	void __fastcall EditCopy1Update(TObject *Sender);
	void __fastcall ModeLenzActionExecute(TObject *Sender);
	void __fastcall ModeLenzActionUpdate(TObject *Sender);
	void __fastcall DirectoryListBox1Change(TObject *Sender);
	void __fastcall Panel1Resize(TObject *Sender);
	void __fastcall DbClearSelectionActionExecute(TObject *Sender);
	void __fastcall DtInfoActionExecute(TObject *Sender);
	void __fastcall DtInfoActionUpdate(TObject *Sender);
	void __fastcall ViewFarthestOverlapsActionExecute(TObject *Sender);
	void __fastcall ViewFarthestOverlapsActionUpdate(TObject *Sender);
	void __fastcall DbMarkingActionExecute(TObject *Sender);
	void __fastcall DbMarkingActionUpdate(TObject *Sender);
	void __fastcall DtViewActionExecute(TObject *Sender);
	void __fastcall DtViewActionUpdate(TObject *Sender);
	void __fastcall ViewSemanticEditorActionExecute(TObject *Sender);
	void __fastcall DtErrorsActionExecute(TObject *Sender);
	void __fastcall dbDictionaryActionExecute(TObject *Sender);
	void __fastcall dbDictionaryActionUpdate(TObject *Sender);
	void __fastcall ViewSemanticEditorActionUpdate(TObject *Sender);
	void __fastcall DbSplitDatabaseExecute(TObject *Sender);
	void __fastcall FImage1MouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
	void __fastcall ModeSelectRectActionExecute(TObject *Sender);
	void __fastcall ModeSelectRectActionUpdate(TObject *Sender);
	void __fastcall ApplicationEvents1Idle(TObject *Sender, bool &Done);
	void __fastcall ImageCropActionExecute(TObject *Sender);
	void __fastcall ImageCropActionUpdate(TObject *Sender);
	void __fastcall ImageInvertActionExecute(TObject *Sender);
	void __fastcall ImageInvertActionUpdate(TObject *Sender);
	void __fastcall ImageGrayscaleActionExecute(TObject *Sender);
	void __fastcall ImageGrayscaleActionUpdate(TObject *Sender);
	void __fastcall ImageMirrorActionExecute(TObject *Sender);
	void __fastcall ImageMirrorActionUpdate(TObject *Sender);
	void __fastcall ImageFlipActionExecute(TObject *Sender);
	void __fastcall ImageFlipActionUpdate(TObject *Sender);
	void __fastcall FlipLeft1Click(TObject *Sender);
	void __fastcall ImageFlipLeftActionUpdate(TObject *Sender);
	void __fastcall ImageFlipRightActionExecute(TObject *Sender);
	void __fastcall ImageFlipRightActionUpdate(TObject *Sender);
	void __fastcall ImageRotateActionExecute(TObject *Sender);
	void __fastcall ImageRotateActionUpdate(TObject *Sender);
	void __fastcall ImageSobelActionExecute(TObject *Sender);
	void __fastcall ImageSobelActionUpdate(TObject *Sender);
	void __fastcall ImageFilterActionExecute(TObject *Sender);
	void __fastcall ImageFilterActionUpdate(TObject *Sender);
	void __fastcall ImageGaussFilterActionExecute(TObject *Sender);
	void __fastcall ImageGaussFilterActionUpdate(TObject *Sender);
	void __fastcall ImageMedianFilterActionExecute(TObject *Sender);
	void __fastcall ImageMedianFilterActionUpdate(TObject *Sender);
	void __fastcall ImageResizeActionExecute(TObject *Sender);
	void __fastcall ImageResizeActionUpdate(TObject *Sender);
	void __fastcall PageControl1Change(TObject *Sender);
	void __fastcall DbViewClick(TObject *Sender);
	void __fastcall DbViewSelectItem(TObject *Sender, TListItem *Item, bool Selected);
	void __fastcall PhImage2AfterOpen(TObject *Sender);
	void __fastcall DbCreateActionExecute(TObject *Sender);
	void __fastcall DbCreateActionUpdate(TObject *Sender);
	void __fastcall DbClearActionExecute(TObject *Sender);
	void __fastcall DbClearActionUpdate(TObject *Sender);
	void __fastcall DbViewKeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall PhImage2Paint(TObject *Sender);
	void __fastcall ModeMarkVectorActionExecute(TObject *Sender);
	void __fastcall ModeMarkVectorActionUpdate(TObject *Sender);
	void __fastcall ModeMarkPolygonActionExecute(TObject *Sender);
	void __fastcall ModeMarkPolygonActionUpdate(TObject *Sender);
	void __fastcall DbOpenActionExecute(TObject *Sender);
	void __fastcall ModeAngleActionExecute(TObject *Sender);
	void __fastcall ModeAngleActionUpdate(TObject *Sender);
	void __fastcall ModeRulerActionExecute(TObject *Sender);
	void __fastcall ModeRulerActionUpdate(TObject *Sender);
	void __fastcall DtCreateActionExecute(TObject *Sender);
	void __fastcall DtCreateActionUpdate(TObject *Sender);
	void __fastcall DtUpdateActionExecute(TObject *Sender);
	void __fastcall DtUpdateActionUpdate(TObject *Sender);
private:	// User declarations
    TPhImageMarkTool* m_markTool;
    AnsiString 		  m_strEngineName;
	TLFDetectEngine   m_ObjectEngine;

    bool              m_tableVisible;
    bool              m_fragmentsVisible;
    bool              m_markToolSelected;
	TList* m_objects;
	bool   m_detect_in_rect;
	int    m_current_rect;

    bool    m_NeedBestFit;
	bool    m_ViewSemanticOutput;
	// overlaps output
    double  m_overlaps_thr;
	double  m_nearlest_overlap;
	double  m_farthest_min_thr;
	double  m_farthest_max_thr;
	bool 	m_draw_farthest_overlaps;
	//
	bool 	m_set_nearest_overlap;

	TProgressBar*    m_ProgressBar1;          // индикатор выполнения.

    String       	 m_strEditCurrentDir;
    TLFScanner       m_scanner;

	TLFScanner* 		__fastcall GetScanner();
    ILFDetectEngine*  	__fastcall GetEngine();

	String       	 m_strLastPath;
    int              m_Selected;
    int              m_penWidth;
    int              m_deltaWidth;

    void __fastcall DrawObjects(TCanvas* cnv);
	void __fastcall DrawSemantic(TCanvas* cnv);
	void __fastcall DrawOverlaps(TCanvas* cnv);
	void __fastcall DrawFarthestOverlaps(TCanvas* cnv);
    void __fastcall DrawRois(TCanvas* cnv);
    void __fastcall LoadIniFile();

    void __fastcall ObjectDetectorHelper();
	void __fastcall Detect();

    void __fastcall UpdateSatatusBar();
    bool __fastcall RemoveMarkupHelper();
    void __fastcall ProgressHandler(int Progress, AnsiString& aComment);

    void __fastcall RepaintImage();
	void __fastcall SetSelectedIndex(int Value);
	awpRect __fastcall FindnearestOverlap(awpRect rr);

	void __fastcall ImageProcessingHelper(awpImage* pImage);
	// DbView
	void __fastcall InitDbView();
	void __fastcall UpdateDbView();


	void __fastcall ToolChange(TObject *Sender);
public:		// User declarations
	__fastcall TForm1(TComponent* Owner);
	__fastcall ~TForm1();

	__property   TProgressBar* ProgressBar1 = {read = m_ProgressBar1};
	__property   TLFScanner*   Scanner = {read = GetScanner};
	__property   ILFDetectEngine*    Engine  = {read = GetEngine};
	__property   int           SelectedIndex = {read = m_Selected, write = SetSelectedIndex};
	__property   bool 		   SetNearestOverlap = {read = m_set_nearest_overlap, write = m_set_nearest_overlap};
	__property   bool 		   DetectInRect = {read = m_detect_in_rect, write = m_detect_in_rect};

	__property bool TableVisible = {read = m_tableVisible, write = m_tableVisible};
	__property bool FragmentsVisible = {read = m_fragmentsVisible, write = m_fragmentsVisible};


	void __fastcall InitImageFile(AnsiString& strFileName);
	void __fastcall DrawScene();

	// Database
	void __fastcall OpenDatabase(const char* lpDbName);

	void ShowDockPanel(TPanel* APanel, bool MakeVisible, TControl* Client);

	TDbLabeledImages			m_db;
	TLFDetectedItem*            m_pBaseObject;
    // with this descriptor works TableViewForm and ImageViewForm
	TLFSemanticImageDescriptor  m_Descr;
	TLFSemanticImageDescriptor	m_rois;
	bool                        m_DrawOverlaps;
	AnsiString    		        m_strFileName;
	SDbCopyOptions 				copy_options;
	SDbConvertOptions 			convert_options;
	SDbExportOptions 			export_options;
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
