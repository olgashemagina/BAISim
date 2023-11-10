object Form1: TForm1
  Left = 615
  Top = 173
  Caption = 'Image Marker'
  ClientHeight = 542
  ClientWidth = 784
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  KeyPreview = True
  Menu = MainMenu1
  OldCreateOrder = False
  Position = poScreenCenter
  OnCreate = FormCreate
  OnKeyPress = FormKeyPress
  OnResize = FormResize
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object ToolBar1: TToolBar
    Left = 0
    Top = 0
    Width = 784
    Height = 34
    AutoSize = True
    ButtonHeight = 34
    ButtonWidth = 53
    Caption = 'ToolBar1'
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -9
    Font.Name = 'Small Fonts'
    Font.Style = []
    Images = ImageList1
    ParentFont = False
    ShowCaptions = True
    TabOrder = 0
    object ToolButton6: TToolButton
      Left = 0
      Top = 0
      Action = ModePaneAction
      Caption = 'Pane'
      ParentShowHint = False
      ShowHint = True
      Style = tbsCheck
    end
    object ToolButton10: TToolButton
      Left = 53
      Top = 0
      Action = ModeZoomAction
      Caption = 'Zoom'
      ParentShowHint = False
      ShowHint = True
      Style = tbsCheck
    end
    object ToolButton2: TToolButton
      Left = 106
      Top = 0
      Action = ModeSelectRectAction
    end
    object ToolButton4: TToolButton
      Left = 159
      Top = 0
      Action = ModeMarkRectAction
      Caption = 'Rectangle'
      Style = tbsCheck
    end
    object ToolButton3: TToolButton
      Left = 212
      Top = 0
      Action = ModeMarkVectorAction
    end
    object ToolButton5: TToolButton
      Left = 265
      Top = 0
      Action = ModeMarkPolygonAction
    end
    object ToolButton12: TToolButton
      Left = 318
      Top = 0
      Width = 8
      Caption = 'ToolButton12'
      ImageIndex = 0
      Style = tbsSeparator
    end
    object ToolButton11: TToolButton
      Left = 326
      Top = 0
      Action = ModeBestFitAction
      Grouped = True
      ParentShowHint = False
      ShowHint = True
      Style = tbsCheck
    end
    object ToolButton9: TToolButton
      Left = 379
      Top = 0
      Action = ModeActualSizeAction
      Caption = 'Actual Size'
      Grouped = True
      ParentShowHint = False
      ShowHint = True
      Style = tbsCheck
    end
    object ToolButton1: TToolButton
      Left = 432
      Top = 0
      Action = ImageDelImageAction
      Caption = 'Delete'
      ParentShowHint = False
      ShowHint = True
    end
    object ToolButton14: TToolButton
      Left = 485
      Top = 0
      Width = 8
      Caption = 'ToolButton14'
      ImageIndex = 0
      Style = tbsSeparator
    end
    object ToolButton8: TToolButton
      Left = 493
      Top = 0
      Action = DtDetectAction
      Caption = 'Objects'
      Style = tbsCheck
    end
  end
  object StatusBar1: TStatusBar
    Left = 0
    Top = 523
    Width = 784
    Height = 19
    Panels = <
      item
        Width = 300
      end
      item
        Alignment = taCenter
        Width = 100
      end
      item
        Alignment = taCenter
        Width = 120
      end
      item
        Alignment = taCenter
        Width = 200
      end
      item
        Width = 50
      end>
    OnResize = StatusBar1Resize
  end
  object Panel5: TPanel
    Left = 0
    Top = 34
    Width = 784
    Height = 489
    Align = alClient
    Caption = 'Panel5'
    TabOrder = 2
    object Splitter4: TSplitter
      Left = 1
      Top = 428
      Width = 782
      Height = 3
      Cursor = crVSplit
      Align = alBottom
      ExplicitTop = 1
      ExplicitWidth = 335
    end
    object Splitter2: TSplitter
      Left = 345
      Top = 1
      Width = 4
      Height = 427
      Beveled = True
      ExplicitLeft = 9
      ExplicitTop = 9
      ExplicitHeight = 497
    end
    object Panel7: TPanel
      Left = 1
      Top = 431
      Width = 782
      Height = 57
      Align = alBottom
      Caption = 'Panel7'
      TabOrder = 0
      object Memo1: TMemo
        Left = 1
        Top = 1
        Width = 780
        Height = 55
        Align = alClient
        TabOrder = 0
      end
    end
    object Panel1: TPanel
      Left = 1
      Top = 1
      Width = 344
      Height = 427
      Align = alLeft
      TabOrder = 1
      OnResize = Panel1Resize
      object PageControl1: TPageControl
        Left = 1
        Top = 1
        Width = 342
        Height = 425
        ActivePage = TabSheet3
        Align = alClient
        TabOrder = 0
        TabPosition = tpBottom
        OnChange = PageControl1Change
        object TabSheet1: TTabSheet
          Caption = 'File System'
          object Splitter5: TSplitter
            Left = 0
            Top = 227
            Width = 334
            Height = 10
            Cursor = crVSplit
            Align = alTop
            Beveled = True
            ExplicitTop = 223
            ExplicitWidth = 281
          end
          object PhImage1: TPhImage
            Left = 0
            Top = 237
            Width = 334
            Height = 135
            AutoMosaic = True
            ThumbWidht = 128
            ThumbHeight = 128
            SlideShowInterval = 500
            Align = alClient
            ParentColor = False
            Visible = False
          end
          object DirectoryListBox1: TDirectoryListBox
            Left = 0
            Top = 0
            Width = 334
            Height = 227
            Align = alTop
            FileList = FileListBox1
            TabOrder = 0
            OnChange = DirectoryListBox1Change
          end
          object Panel9: TPanel
            Left = 0
            Top = 372
            Width = 334
            Height = 27
            Align = alBottom
            TabOrder = 1
            object DriveComboBox1: TDriveComboBox
              Left = 1
              Top = 1
              Width = 332
              Height = 19
              Align = alTop
              DirList = DirectoryListBox1
              TabOrder = 0
            end
          end
          object FilterComboBox1: TFilterComboBox
            Left = 25
            Top = 273
            Width = 145
            Height = 21
            FileList = FileListBox1
            Filter = 'Image files|*.awp'
            TabOrder = 2
            Visible = False
          end
          object FileListBox1: TFileListBox
            Left = 0
            Top = 237
            Width = 334
            Height = 135
            Align = alClient
            Mask = '*.jpg;*awp;*.png;*.bmp;*.tif;*.tiff'
            PopupMenu = PopupMenu1
            ShowGlyphs = True
            TabOrder = 3
            OnChange = FileListBox1Change
            OnKeyUp = FileListBox1KeyUp
          end
        end
        object TabSheet2: TTabSheet
          Caption = 'Project'
          ImageIndex = 1
          object TreeView1: TTreeView
            Left = 0
            Top = 0
            Width = 334
            Height = 399
            Align = alClient
            Indent = 19
            TabOrder = 0
            Items.NodeData = {
              03010000002C0000000000000000000000FFFFFFFFFFFFFFFF00000000000000
              00020000000107700072006F006A006500630074003200000000000000000000
              00FFFFFFFFFFFFFFFF000000000000000003000000010A640069006300740069
              006F006E00610072007900280000000000000000000000FFFFFFFFFFFFFFFF00
              0000000000000003000000010577006F007200640031002C0000000000000000
              000000FFFFFFFFFFFFFFFF00000000000000000000000001077300630061006E
              006E00650072002E0000000000000000000000FFFFFFFFFFFFFFFF0000000000
              0000000000000001086400650074006500630074006F0072002E000000000000
              0000000000FFFFFFFFFFFFFFFF00000000000000000000000001087300650074
              00740069006E0067007300280000000000000000000000FFFFFFFFFFFFFFFF00
              0000000000000003000000010577006F007200640032002C0000000000000000
              000000FFFFFFFFFFFFFFFF00000000000000000000000001077300630061006E
              006E00650072002E0000000000000000000000FFFFFFFFFFFFFFFF0000000000
              0000000000000001086400650074006500630074006F0072002E000000000000
              0000000000FFFFFFFFFFFFFFFF00000000000000000000000001087300650074
              00740069006E0067007300280000000000000000000000FFFFFFFFFFFFFFFF00
              0000000000000000000000010577006F007200640033002E0000000000000000
              000000FFFFFFFFFFFFFFFF000000000000000000000000010864006100740061
              006200610073006500}
          end
        end
        object TabSheet3: TTabSheet
          Caption = 'Database'
          ImageIndex = 2
          object Panel3: TPanel
            Left = 0
            Top = 0
            Width = 334
            Height = 33
            Align = alTop
            TabOrder = 0
            object SpeedButton2: TSpeedButton
              Left = 1
              Top = 1
              Width = 35
              Height = 31
              Align = alLeft
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Small Fonts'
              Font.Style = []
              Glyph.Data = {
                36050000424D3605000000000000360400002800000010000000100000000100
                0800000000000001000000000000000000000001000000000000000000000000
                80000080000000808000800000008000800080800000C0C0C000C0DCC000F0CA
                A6000020400000206000002080000020A0000020C0000020E000004000000040
                20000040400000406000004080000040A0000040C0000040E000006000000060
                20000060400000606000006080000060A0000060C0000060E000008000000080
                20000080400000806000008080000080A0000080C0000080E00000A0000000A0
                200000A0400000A0600000A0800000A0A00000A0C00000A0E00000C0000000C0
                200000C0400000C0600000C0800000C0A00000C0C00000C0E00000E0000000E0
                200000E0400000E0600000E0800000E0A00000E0C00000E0E000400000004000
                20004000400040006000400080004000A0004000C0004000E000402000004020
                20004020400040206000402080004020A0004020C0004020E000404000004040
                20004040400040406000404080004040A0004040C0004040E000406000004060
                20004060400040606000406080004060A0004060C0004060E000408000004080
                20004080400040806000408080004080A0004080C0004080E00040A0000040A0
                200040A0400040A0600040A0800040A0A00040A0C00040A0E00040C0000040C0
                200040C0400040C0600040C0800040C0A00040C0C00040C0E00040E0000040E0
                200040E0400040E0600040E0800040E0A00040E0C00040E0E000800000008000
                20008000400080006000800080008000A0008000C0008000E000802000008020
                20008020400080206000802080008020A0008020C0008020E000804000008040
                20008040400080406000804080008040A0008040C0008040E000806000008060
                20008060400080606000806080008060A0008060C0008060E000808000008080
                20008080400080806000808080008080A0008080C0008080E00080A0000080A0
                200080A0400080A0600080A0800080A0A00080A0C00080A0E00080C0000080C0
                200080C0400080C0600080C0800080C0A00080C0C00080C0E00080E0000080E0
                200080E0400080E0600080E0800080E0A00080E0C00080E0E000C0000000C000
                2000C0004000C0006000C0008000C000A000C000C000C000E000C0200000C020
                2000C0204000C0206000C0208000C020A000C020C000C020E000C0400000C040
                2000C0404000C0406000C0408000C040A000C040C000C040E000C0600000C060
                2000C0604000C0606000C0608000C060A000C060C000C060E000C0800000C080
                2000C0804000C0806000C0808000C080A000C080C000C080E000C0A00000C0A0
                2000C0A04000C0A06000C0A08000C0A0A000C0A0C000C0A0E000C0C00000C0C0
                2000C0C04000C0C06000C0C08000C0C0A000F0FBFF00A4A0A000808080000000
                FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00000000000000
                000000980000000000000000000000000048E0E0000000000000000000000000
                98E890E0000000000000000000000000E84800E8000000000000000000000000
                E04800E8000000000000000000000000E04800E8000000000000000000000000
                E04848E8000000000000000000000000E04848E00000000000000000000000D8
                E00000E0D8000000000000000000E0E090000090E0E000000000000048E0E048
                0000000048E0E04800000090E0E00000000000000000E0E0900090E098000000
                0000000000000098E090E09800000000000000000000000098E0E09848484848
                484848484848484898E098E0E0E0E0E0E0E0E0E0E0E0E0E0E098}
              Layout = blGlyphTop
              Margin = 3
              ParentFont = False
              Spacing = 0
              ExplicitTop = 0
              ExplicitHeight = 30
            end
            object SpeedButton1: TSpeedButton
              Left = 36
              Top = 1
              Width = 35
              Height = 31
              Align = alLeft
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Small Fonts'
              Font.Style = []
              Glyph.Data = {
                36050000424D3605000000000000360400002800000010000000100000000100
                0800000000000001000000000000000000000001000000000000000000000000
                80000080000000808000800000008000800080800000C0C0C000C0DCC000F0CA
                A6000020400000206000002080000020A0000020C0000020E000004000000040
                20000040400000406000004080000040A0000040C0000040E000006000000060
                20000060400000606000006080000060A0000060C0000060E000008000000080
                20000080400000806000008080000080A0000080C0000080E00000A0000000A0
                200000A0400000A0600000A0800000A0A00000A0C00000A0E00000C0000000C0
                200000C0400000C0600000C0800000C0A00000C0C00000C0E00000E0000000E0
                200000E0400000E0600000E0800000E0A00000E0C00000E0E000400000004000
                20004000400040006000400080004000A0004000C0004000E000402000004020
                20004020400040206000402080004020A0004020C0004020E000404000004040
                20004040400040406000404080004040A0004040C0004040E000406000004060
                20004060400040606000406080004060A0004060C0004060E000408000004080
                20004080400040806000408080004080A0004080C0004080E00040A0000040A0
                200040A0400040A0600040A0800040A0A00040A0C00040A0E00040C0000040C0
                200040C0400040C0600040C0800040C0A00040C0C00040C0E00040E0000040E0
                200040E0400040E0600040E0800040E0A00040E0C00040E0E000800000008000
                20008000400080006000800080008000A0008000C0008000E000802000008020
                20008020400080206000802080008020A0008020C0008020E000804000008040
                20008040400080406000804080008040A0008040C0008040E000806000008060
                20008060400080606000806080008060A0008060C0008060E000808000008080
                20008080400080806000808080008080A0008080C0008080E00080A0000080A0
                200080A0400080A0600080A0800080A0A00080A0C00080A0E00080C0000080C0
                200080C0400080C0600080C0800080C0A00080C0C00080C0E00080E0000080E0
                200080E0400080E0600080E0800080E0A00080E0C00080E0E000C0000000C000
                2000C0004000C0006000C0008000C000A000C000C000C000E000C0200000C020
                2000C0204000C0206000C0208000C020A000C020C000C020E000C0400000C040
                2000C0404000C0406000C0408000C040A000C040C000C040E000C0600000C060
                2000C0604000C0606000C0608000C060A000C060C000C060E000C0800000C080
                2000C0804000C0806000C0808000C080A000C080C000C080E000C0A00000C0A0
                2000C0A04000C0A06000C0A08000C0A0A000C0A0C000C0A0E000C0C00000C0C0
                2000C0C04000C0C06000C0C08000C0C0A000F0FBFF00A4A0A000808080000000
                FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00525252525252
                525252525252525252525252494A4A5252494952524A4A49525252A4F6080852
                07F6F607520808F6A45252A40807B552F70707F752B50708A45252A40808F752
                F70808F752F70808A45252525B5B5252525B5B5252525B5B52525252A49B9B52
                5BA3A35B529B9BA4525252A4F6080752F70808F7520708F6A45252A40807B552
                F70808F752B50708A45252A40707F752A40707A452F70707A452525249494952
                49494949524949495252529B07F7F752A5F5F5A552F7F7079B5252A40807B552
                F70808F752B50708A45252A40807B552F70808F752B50708A452525BA4A49B52
                9BA4A49B529BA4A45B5252525252525252525252525252525252}
              Layout = blGlyphTop
              Margin = 3
              ParentFont = False
              Spacing = 0
              ExplicitLeft = 47
              ExplicitTop = 0
              ExplicitHeight = 30
            end
          end
          object DbView: TListView
            Left = 0
            Top = 33
            Width = 334
            Height = 345
            Align = alClient
            Columns = <>
            GridLines = True
            MultiSelect = True
            ReadOnly = True
            RowSelect = True
            TabOrder = 1
            ViewStyle = vsReport
            OnKeyUp = DbViewKeyUp
            OnSelectItem = DbViewSelectItem
          end
          object Panel2: TPanel
            Left = 0
            Top = 378
            Width = 334
            Height = 21
            Align = alBottom
            Caption = 'Panel2'
            TabOrder = 2
          end
        end
      end
    end
    object Panel6: TPanel
      Left = 349
      Top = 1
      Width = 434
      Height = 427
      Align = alClient
      Caption = 'Panel2'
      TabOrder = 2
      object Panel11: TPanel
        Left = 1
        Top = 1
        Width = 432
        Height = 425
        Align = alClient
        TabOrder = 0
        object HSplitter: TSplitter
          Left = 1
          Top = 421
          Width = 430
          Height = 3
          Cursor = crVSplit
          Align = alBottom
          ExplicitTop = 540
          ExplicitWidth = 557
        end
        object Splitter7: TSplitter
          Left = 423
          Top = 1
          Height = 412
          Align = alRight
          ExplicitLeft = 550
          ExplicitHeight = 539
        end
        object BottomDocPanel: TPanel
          Left = 1
          Top = 413
          Width = 430
          Height = 8
          Align = alBottom
          DockSite = True
          DragKind = dkDock
          TabOrder = 0
          OnDockDrop = BottomDocPanelDockDrop
          OnDockOver = BottomDocPanelDockOver
          OnGetSiteInfo = BottomDocPanelGetSiteInfo
          OnUnDock = BottomDocPanelUnDock
        end
        object Panel13: TPanel
          Left = 426
          Top = 1
          Width = 5
          Height = 412
          Align = alRight
          Caption = 'Panel3'
          DockSite = True
          DragKind = dkDock
          TabOrder = 1
        end
        object ValueListEditor1: TValueListEditor
          Left = 1
          Top = 1
          Width = 422
          Height = 412
          Align = alClient
          TabOrder = 2
          Visible = False
          ColWidths = (
            150
            266)
        end
        object PhImage2: TPhImage
          Left = 1
          Top = 1
          Width = 422
          Height = 412
          AutoMosaic = True
          ThumbWidht = 128
          ThumbHeight = 128
          SlideShowInterval = 500
          Align = alClient
          ParentColor = False
          OnMouseMove = FImage1MouseMove
          OnPaint = PhImage2Paint
          AfterOpen = PhImage2AfterOpen
        end
      end
    end
  end
  object OpenDialog1: TOpenDialog
    Filter = 'XML files|*.xml'
    Left = 672
    Top = 144
  end
  object MainMenu1: TMainMenu
    Images = ImageList1
    Left = 672
    Top = 48
    object File1: TMenuItem
      Caption = 'Files'
      object NewProject1: TMenuItem
        Caption = 'New Project...'
      end
      object N11: TMenuItem
        Caption = '-'
      end
      object Close1: TMenuItem
        Action = CloseAction
      end
    end
    object View1: TMenuItem
      Caption = 'View'
      object Actualsize1: TMenuItem
        Action = ModeActualSizeAction
      end
      object BestFit1: TMenuItem
        Action = ModeBestFitAction
      end
      object N14: TMenuItem
        Caption = '-'
      end
      object Toolbar2: TMenuItem
        Action = ViewToolbarAction
      end
      object StatusBar2: TMenuItem
        Action = ViewStatusbarAction
      end
      object DirectoryPane1: TMenuItem
        Action = ViewDirectoryAction
      end
      object N15: TMenuItem
        Caption = '-'
      end
      object ViewSemanticOutput1: TMenuItem
        Action = ViewSemanticOutputAction
      end
      object Overlaps1: TMenuItem
        Action = ViewOlerlapsAction
      end
      object ViewFarthestOverlaps1: TMenuItem
        Action = ViewFarthestOverlapsAction
      end
      object N13: TMenuItem
        Action = ViewSubsetAction
        Visible = False
      end
      object ViewTable1: TMenuItem
        Action = ViewTableAction
      end
      object ViewSemanticEditorAction1: TMenuItem
        Action = ViewSemanticEditorAction
      end
      object N17: TMenuItem
        Caption = '-'
      end
      object ViewSemanticThumbnail1: TMenuItem
        Action = ViewSemanticThumbinalsAction
      end
    end
    object Tool1: TMenuItem
      Caption = 'Mouse Mode'
      object N4: TMenuItem
        Action = ModePaneAction
      end
      object N5: TMenuItem
        Action = ModeZoomAction
      end
      object SetRect1: TMenuItem
        Action = ModeMarkRectAction
      end
      object ModeLenzAction1: TMenuItem
        Action = ModeLenzAction
      end
      object SelectROI1: TMenuItem
        Action = ModeSelectRectAction
      end
      object RulerRuler1: TMenuItem
        Action = ModeRulerAction
      end
      object AngleIAngle1: TMenuItem
        Action = ModeAngleAction
        Caption = 'Angle Tool'
      end
    end
    object I1: TMenuItem
      Caption = 'Image processing'
      object Invert1: TMenuItem
        Action = ImageInvertAction
        ShortCut = 16457
      end
      object Grayscale1: TMenuItem
        Action = ImageGrayscaleAction
      end
      object Mirror1: TMenuItem
        Action = ImageMirrorAction
      end
      object FlipVertical1: TMenuItem
        Action = ImageFlipAction
      end
      object FlipLeft1: TMenuItem
        Action = ImageFlipLeftAction
        OnClick = FlipLeft1Click
      end
      object FlipRight1: TMenuItem
        Action = ImageFlipRightAction
      end
      object Rotate2: TMenuItem
        Action = ImageRotateAction
      end
      object Crop1: TMenuItem
        Action = ImageCropAction
      end
      object SobelFilter1: TMenuItem
        Action = ImageSobelAction
      end
      object ImageFilterAction1: TMenuItem
        Action = ImageFilterAction
      end
      object GaussFilter1: TMenuItem
        Action = ImageGaussFilterAction
      end
      object MedianFilter1: TMenuItem
        Action = ImageMedianFilterAction
      end
      object Resize1: TMenuItem
        Action = ImageResizeAction
      end
      object N12: TMenuItem
        Caption = '-'
      end
      object N16: TMenuItem
        Action = ImageDelImageAction
        ShortCut = 16430
      end
      object Copyfiles1: TMenuItem
        Action = ImageCopyImageAction
      end
    end
    object N1: TMenuItem
      Caption = 'Database'
      object CreateDatabase1: TMenuItem
        Action = DbCreateAction
      end
      object Opendatabase1: TMenuItem
        Action = DbOpenAction
      end
      object Dictionary1: TMenuItem
        Action = dbDictionaryAction
      end
      object CopyDatabase1: TMenuItem
        Action = DbCopyAction
      end
      object ClearDatabase1: TMenuItem
        Action = DbClearAction
      end
      object N18: TMenuItem
        Caption = '-'
      end
      object Exportimages1: TMenuItem
        Action = DbExportFragmentsAction
      end
      object Exportawpimages1: TMenuItem
        Action = DbConvertDatabaseAction
      end
      object DoAnalysis1: TMenuItem
        Action = DbMarkingAction
      end
      object SplitDatabase1: TMenuItem
        Action = DbSplitDatabase
      end
      object DatabaseInfo1: TMenuItem
        Action = DbInfoAction
      end
      object N10: TMenuItem
        Caption = '-'
      end
      object SelectAll1: TMenuItem
        Action = DbSelectAllAction
        Visible = False
      end
      object ClearSelection1: TMenuItem
        Action = DbClearSelectionAction
        Visible = False
      end
      object InvertSelection1: TMenuItem
        Action = DbInvertSelectionAction
        Visible = False
      end
      object DeleteSelected1: TMenuItem
        Action = DbDeleteSelectedAction
        Visible = False
      end
      object CopySelectedTo1: TMenuItem
        Action = DbCopySelectedToAction
        Visible = False
      end
      object DbMoveSelectedTo1: TMenuItem
        Action = DbMoveSelectedToAction
        Visible = False
      end
    end
    object N2: TMenuItem
      Caption = 'Detector'
      object CreateDetector1: TMenuItem
        Action = DtCreateAction
      end
      object N9: TMenuItem
        Action = FileOpenDetectorAction
      end
      object UpdateDetector1: TMenuItem
        Action = DtUpdateAction
      end
      object N19: TMenuItem
        Caption = '-'
      end
      object Detectorinfo1: TMenuItem
        Action = DtInfoAction
      end
      object Viewdetector1: TMenuItem
        Action = DtViewAction
      end
      object N23: TMenuItem
        Action = DtDetectAction
      end
      object Engineerrors1: TMenuItem
        Action = DtErrorsAction
      end
    end
    object Tools1: TMenuItem
      Caption = 'Tools'
      object Options1: TMenuItem
        Caption = 'Options...'
        OnClick = Options1Click
      end
    end
    object N3: TMenuItem
      Caption = 'Help'
      object N25: TMenuItem
        Action = AboutAction
      end
    end
  end
  object ActionList2: TActionList
    Images = ImageList1
    Left = 704
    Top = 48
    object CloseAction: TAction
      Category = 'File'
      Caption = 'Exit'
      Hint = 'Exit from program'
      OnExecute = CloseActionExecute
    end
    object FileOpenDetectorAction: TAction
      Category = 'File'
      Caption = 'Load  object detector...'
      Hint = '|Load file detector'
      OnExecute = FileOpenDetectorActionExecute
    end
    object ModePaneAction: TAction
      Category = 'Mode'
      Caption = 'Pane Tool'
      GroupIndex = 1
      Hint = 'pane|pane'
      ImageIndex = 5
      OnExecute = ModePaneActionExecute
      OnUpdate = ModePaneActionUpdate
    end
    object ModeZoomAction: TAction
      Category = 'Mode'
      Caption = 'Zoom Tool'
      GroupIndex = 1
      Hint = 'Zoom|Zoom'
      ImageIndex = 3
      OnExecute = ModeZoomActionExecute
      OnUpdate = ModeZoomActionUpdate
    end
    object ModeBestFitAction: TAction
      Category = 'Mode'
      Caption = 'Best Fit'
      Hint = 'Best Fit|Best Fit'
      ImageIndex = 4
      ShortCut = 16450
      OnExecute = ModeBestFitActionExecute
      OnUpdate = ModeBestFitActionUpdate
    end
    object ModeActualSizeAction: TAction
      Category = 'Mode'
      Caption = 'Actual size'
      Hint = 'Actual size|Actual size of  image'
      ImageIndex = 8
      ShortCut = 16449
      OnExecute = ModeActualSizeActionExecute
      OnUpdate = ModeActualSizeActionUpdate
    end
    object ImageDelImageAction: TAction
      Category = 'Image'
      Caption = 'Delete Image'
      Hint = 'Delete image|Delete image from database'
      ImageIndex = 12
      OnExecute = ImageDelImageActionExecute
      OnUpdate = ImageDelImageActionUpdate
    end
    object DtDetectAction: TAction
      Category = 'Detector'
      Caption = 'Find Objects'
      Hint = 'Find faces|Find faces'
      ImageIndex = 1
      OnExecute = DtDetectActionExecute
      OnUpdate = DtDetectActionUpdate
    end
    object AboutAction: TAction
      Category = 'Help'
      Caption = 'About...'
      Hint = '| About'
      OnExecute = AboutActionExecute
    end
    object ModeMarkRectAction: TAction
      Category = 'Mode'
      Caption = 'Mark Tool'
      GroupIndex = 1
      ImageIndex = 6
      OnExecute = ModeMarkRectActionExecute
      OnUpdate = ModeMarkRectActionUpdate
    end
    object ViewToolbarAction: TAction
      Category = 'View'
      Caption = 'Tool Bar '
      Hint = 'View Toolbar '
      OnExecute = ViewToolbarActionExecute
      OnUpdate = ViewToolbarActionUpdate
    end
    object ViewStatusbarAction: TAction
      Category = 'View'
      Caption = 'Status Bar'
      Hint = 'View status bar'
      OnExecute = ViewStatusbarActionExecute
      OnUpdate = ViewStatusbarActionUpdate
    end
    object ViewDirectoryAction: TAction
      Category = 'View'
      Caption = 'Directory Pane'
      Hint = 'View Directory Pane'
      OnExecute = ViewDirectoryActionExecute
      OnUpdate = ViewDirectoryActionUpdate
    end
    object ViewTableAction: TAction
      Category = 'View'
      Caption = 'Sematic Table'
      Hint = 'View Table'
      OnExecute = ViewTableActionExecute
      OnUpdate = ViewTableActionUpdate
    end
    object DbInfoAction: TAction
      Category = 'Database'
      Caption = 'Database Info...'
      OnExecute = DbInfoActionExecute
      OnUpdate = DbInfoActionUpdate
    end
    object ViewOlerlapsAction: TAction
      Category = 'View'
      Caption = 'View Nearest Overlaps '
      OnExecute = ViewOlerlapsActionExecute
      OnUpdate = ViewOlerlapsActionUpdate
    end
    object DbExportFragmentsAction: TAction
      Category = 'Database'
      Caption = 'Export Image Fragments...'
      OnExecute = DbExportFragmentsActionExecute
      OnUpdate = DbExportFragmentsActionUpdate
    end
    object DtErrorsAction: TAction
      Category = 'Detector'
      Caption = 'Engine errors...'
      OnExecute = DtErrorsActionExecute
    end
    object ViewSubsetAction: TAction
      Category = 'View'
      Caption = 'Database Subset'
    end
    object ViewSemanticOutputAction: TAction
      Category = 'View'
      Caption = 'View Semantic Output'
      OnExecute = ViewSemanticOutputActionExecute
      OnUpdate = ViewSemanticOutputActionUpdate
    end
    object DbConvertDatabaseAction: TAction
      Category = 'Database'
      Caption = 'Convert Database....'
      OnExecute = DbConvertDatabaseActionExecute
    end
    object DbCopyAction: TAction
      Category = 'Database'
      Caption = 'Copy Database...'
      OnExecute = DbCopyActionExecute
      OnUpdate = DbCopyActionUpdate
    end
    object ImageCopyImageAction: TAction
      Category = 'Image'
      Caption = 'Copy file(s)...'
      ShortCut = 16465
      OnExecute = ImageCopyImageActionExecute
      OnUpdate = ImageCopyImageActionUpdate
    end
    object ViewDbTubinalsAction: TAction
      Category = 'View'
      Caption = 'View Database Thumbnail'
    end
    object EditCopy1: TEditCopy
      Category = 'Edit'
      Caption = '&Copy'
      Hint = 'Copy|Copies the selection and puts it on the Clipboard'
      ImageIndex = 15
      ShortCut = 16451
      OnExecute = EditCopy1Execute
      OnUpdate = EditCopy1Update
    end
    object ModeLenzAction: TAction
      Category = 'Mode'
      Caption = 'Lenz Tool'
      ImageIndex = 8
      OnExecute = ModeLenzActionExecute
      OnUpdate = ModeLenzActionUpdate
    end
    object ModeFaceAction: TAction
      Category = 'Mode'
      Caption = 'Face Tool'
      ImageIndex = 2
    end
    object ViewSemanticThumbinalsAction: TAction
      Category = 'View'
      Caption = 'View Semantic Thumbnail'
    end
    object ViewListAction: TAction
      Category = 'View'
      Caption = 'View File List'
    end
    object DbClearSelectionAction: TAction
      Category = 'Database'
      Caption = 'Clear Selection'
      OnExecute = DbClearSelectionActionExecute
    end
    object DbInvertSelectionAction: TAction
      Category = 'Database'
      Caption = 'Invert Selection'
    end
    object DbDeleteSelectedAction: TAction
      Category = 'Database'
      Caption = 'Delete Selected'
    end
    object DbCopySelectedToAction: TAction
      Category = 'Database'
      Caption = 'Copy Selected To...'
    end
    object DbMoveSelectedToAction: TAction
      Category = 'Database'
      Caption = 'Db Move Selected To...'
    end
    object DbSelectAllAction: TAction
      Category = 'Database'
      Caption = 'Select All'
    end
    object DtInfoAction: TAction
      Category = 'Detector'
      Caption = 'Detector info...'
      OnExecute = DtInfoActionExecute
      OnUpdate = DtInfoActionUpdate
    end
    object ViewFarthestOverlapsAction: TAction
      Category = 'View'
      Caption = 'View Farthest Overlaps '
      OnExecute = ViewFarthestOverlapsActionExecute
      OnUpdate = ViewFarthestOverlapsActionUpdate
    end
    object DbMarkingAction: TAction
      Category = 'Database'
      Caption = 'Do Analysis '
      OnExecute = DbMarkingActionExecute
      OnUpdate = DbMarkingActionUpdate
    end
    object DtViewAction: TAction
      Category = 'Detector'
      Caption = 'View detector...'
      OnExecute = DtViewActionExecute
      OnUpdate = DtViewActionUpdate
    end
    object ViewSemanticEditorAction: TAction
      Category = 'View'
      Caption = 'Semantic  Items Editor'
      OnExecute = ViewSemanticEditorActionExecute
      OnUpdate = ViewSemanticEditorActionUpdate
    end
    object dbDictionaryAction: TAction
      Category = 'Database'
      Caption = 'Edit Dictionary...'
      OnExecute = dbDictionaryActionExecute
      OnUpdate = dbDictionaryActionUpdate
    end
    object DbSplitDatabase: TAction
      Category = 'Database'
      Caption = 'Split Database...'
      OnExecute = DbSplitDatabaseExecute
    end
    object ModeSelectRectAction: TAction
      Category = 'Mode'
      Caption = 'Select ROI'
      ImageIndex = 7
      OnExecute = ModeSelectRectActionExecute
      OnUpdate = ModeSelectRectActionUpdate
    end
    object ImageCropAction: TAction
      Category = 'Image'
      Caption = 'Crop'
      ShortCut = 16464
      OnExecute = ImageCropActionExecute
      OnUpdate = ImageCropActionUpdate
    end
    object ImageInvertAction: TAction
      Category = 'Image'
      Caption = '&Invert'
      OnExecute = ImageInvertActionExecute
      OnUpdate = ImageInvertActionUpdate
    end
    object ImageGrayscaleAction: TAction
      Category = 'Image'
      Caption = '&Grayscale'
      ShortCut = 16455
      OnExecute = ImageGrayscaleActionExecute
      OnUpdate = ImageGrayscaleActionUpdate
    end
    object ImageMirrorAction: TAction
      Category = 'Image'
      Caption = '&Mirror '
      ShortCut = 16461
      OnExecute = ImageMirrorActionExecute
      OnUpdate = ImageMirrorActionUpdate
    end
    object ImageFlipAction: TAction
      Category = 'Image'
      Caption = '&Flip Vertical'
      ShortCut = 16454
      OnExecute = ImageFlipActionExecute
      OnUpdate = ImageFlipActionUpdate
    end
    object ImageFlipLeftAction: TAction
      Category = 'Image'
      Caption = 'Flip &Left '
      ShortCut = 16460
      OnUpdate = ImageFlipLeftActionUpdate
    end
    object ImageFlipRightAction: TAction
      Category = 'Image'
      Caption = 'Flip &Right'
      ShortCut = 16466
      OnExecute = ImageFlipRightActionExecute
      OnUpdate = ImageFlipRightActionUpdate
    end
    object ImageRotateAction: TAction
      Category = 'Image'
      Caption = 'R&otate...'
      ShortCut = 16463
      OnExecute = ImageRotateActionExecute
      OnUpdate = ImageRotateActionUpdate
    end
    object ImageSobelAction: TAction
      Category = 'Image'
      Caption = '&Sobel Filter'
      ShortCut = 16467
      OnExecute = ImageSobelActionExecute
      OnUpdate = ImageSobelActionUpdate
    end
    object ImageFilterAction: TAction
      Category = 'Image'
      Caption = 'Lienar Filters...'
      ShortCut = 16469
      OnExecute = ImageFilterActionExecute
      OnUpdate = ImageFilterActionUpdate
    end
    object ImageGaussFilterAction: TAction
      Category = 'Image'
      Caption = 'Gauss Fil&ter...'
      ShortCut = 16468
      OnExecute = ImageGaussFilterActionExecute
      OnUpdate = ImageGaussFilterActionUpdate
    end
    object ImageMedianFilterAction: TAction
      Category = 'Image'
      Caption = 'Media&n Filter...'
      ShortCut = 16462
      OnExecute = ImageMedianFilterActionExecute
      OnUpdate = ImageMedianFilterActionUpdate
    end
    object ImageResizeAction: TAction
      Category = 'Image'
      Caption = 'Resize...'
      ShortCut = 16474
      OnExecute = ImageResizeActionExecute
      OnUpdate = ImageResizeActionUpdate
    end
    object DbCreateAction: TAction
      Category = 'Database'
      Caption = 'Create Database...'
      OnExecute = DbCreateActionExecute
      OnUpdate = DbCreateActionUpdate
    end
    object DbClearAction: TAction
      Category = 'Database'
      Caption = 'Clear Database'
      OnExecute = DbClearActionExecute
      OnUpdate = DbClearActionUpdate
    end
    object ModeMarkVectorAction: TAction
      Category = 'Mode'
      Caption = 'Vector'
      OnExecute = ModeMarkVectorActionExecute
      OnUpdate = ModeMarkVectorActionUpdate
    end
    object ModeMarkPolygonAction: TAction
      Category = 'Mode'
      Caption = 'Polygon'
      OnExecute = ModeMarkPolygonActionExecute
      OnUpdate = ModeMarkPolygonActionUpdate
    end
    object DbOpenAction: TAction
      Category = 'Database'
      Caption = 'Open database...'
      OnExecute = DbOpenActionExecute
    end
    object ModeAngleAction: TAction
      Category = 'Mode'
      Caption = 'AngleIAngle'
      OnExecute = ModeAngleActionExecute
      OnUpdate = ModeAngleActionUpdate
    end
    object ModeRulerAction: TAction
      Category = 'Mode'
      Caption = 'Ruler Tool'
      OnExecute = ModeRulerActionExecute
      OnUpdate = ModeRulerActionUpdate
    end
    object DtCreateAction: TAction
      Category = 'Detector'
      Caption = 'Create Detector...'
      OnExecute = DtCreateActionExecute
      OnUpdate = DtCreateActionUpdate
    end
    object DtUpdateAction: TAction
      Category = 'Detector'
      Caption = 'Update Detector...'
      OnExecute = DtUpdateActionExecute
      OnUpdate = DtUpdateActionUpdate
    end
  end
  object ImageList1: TImageList
    Left = 704
    Top = 144
    Bitmap = {
      494C010110001300040010001000FFFFFFFFFF10FFFFFFFFFFFFFFFF424D3600
      0000000000003600000028000000400000005000000001002000000000000050
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000C0C0C000808080000000000000000000C0C0C00080808000C0C0C0008080
      8000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000808080000000000000000000000000000000000000000000808080008080
      8000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000C0C0C00000000000808080008080
      8000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000800000008000000080000000800000008000
      0000800000008000000080000000800000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000008080
      80000000000000000000C0C0C00000000000C0C0C00000000000808080008080
      8000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000808080000000000080808000000000000000000000000000000000000000
      000000000000000000000000000080000000FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00800000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000008080
      800000000000000000000000000000000000C0C0C00000000000808080000000
      0000000000000000000000000000000000000000000000000000000000000000
      000080808000808080008080800080808000808080008080800080808000C0C0
      C000000000000000000080808000808080000000000000000000000000000000
      000000000000000000000000000080000000FFFFFF0000000000000000000000
      00000000000000000000FFFFFF00800000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000008080800000000000808080000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00008080800000000000C0C0C000000000000000000000000000000000000000
      000000000000000000000000000080000000FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00800000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000C0C0
      C000000000008080800000000000000000008080800000000000000000008080
      8000808080000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000808080000000000000000000FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF0080000000FFFFFF0000000000000000000000
      00000000000000000000FFFFFF00800000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000C0C0
      C000000000000000000080808000C0C0C0008080800000000000000000008080
      8000808080000000000000000000000000008080800000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000C0C0C0000000000000000000FFFFFF000000
      000000000000000000000000000080000000FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00800000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00008080800000000000C0C0C00000000000C0C0C00000000000000000008080
      800080808000000000000000000000000000C0C0C00000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000080808000000000000000000000000000FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF0080000000FFFFFF000000000000000000FFFF
      FF00800000008000000080000000800000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000008080800000000000000000008080
      800000000000000000000000000000000000C0C0C00000000000000000000000
      0000000000000000000080000000808080008080800080808000C0C0C000C0C0
      C00000000000C0C0C00000000000000000000000000000000000FFFFFF000000
      000000000000000000000000000080000000FFFFFF00FFFFFF00FFFFFF00FFFF
      FF0080000000FFFFFF0080000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000080808000000000008080
      8000000000000000000000000000000000000000000000000000000000008080
      8000000000000000000000000000000000000000000000000000FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF0080000000FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00800000008000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000808080000000000000000000000000000000
      00000000000000000000000000000000000000000000C0C0C000808080000000
      0000000000000000000000000000000000000000000080808000808080000000
      0000000000000000000000000000000000000000000000000000FFFFFF000000
      000000000000FFFFFF0000000000800000008000000080000000800000008000
      0000800000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000C0C0C0000000000000000000000000000000
      0000C0C0C0000000000000000000000000000000000000000000C0C0C0000000
      0000808080008080800080808000808080008080800000000000000000000000
      0000000000000000000000000000000000000000000000000000FFFFFF00FFFF
      FF00FFFFFF00FFFFFF0000000000FFFFFF000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000808080000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000FFFFFF00FFFF
      FF00FFFFFF00FFFFFF0000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000C6C6
      C600C6C6C6000000000000000000000000000000000000000000FFFFFF00FFFF
      FF00000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000007F7F7F007F7F7F007F7F7F007F7F7F007F7F7F007F7F
      7F007F7F7F007F7F7F007F7F7F007F7F7F000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000C6C6
      C600C6C6C60000000000000000000000000000000000FFFFFF00848484000000
      000000000000C6C6C60000000000000000000000000000000000000000000000
      0000000000000000000000000000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF000000000000000000FFFFFF00FFFFFF000000
      0000FFFFFF00FFFFFF007F7F7F00FFFFFF00FFFFFF000000000000000000FFFF
      FF0000000000FFFFFF00FFFFFF007F7F7F000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000C6C6
      C600C6C6C600000000000000000000000000FFFFFF008484840084848400C6C6
      C600848484000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000FFFFFF00FFFFFF0000000000FFFF
      FF000000000000000000FFFFFF00000000007F7F7F007F7F7F00FFFFFF007F7F
      7F007F7F7F007F7F7F007F7F7F007F7F7F0000000000FFFFFF007F7F7F000000
      00007F7F7F007F7F7F00000000007F7F7F000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000FFFFFF008484
      840084848400C6C6C600FFFFFF00FFFFFF008484840084848400C6C6C600C6C6
      C600C6C6C600C6C6C6000000000000000000FFFF0000000000000000000000FF
      FF00FFFFFF0000FFFF00FFFFFF0000FFFF0000000000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00000000007F7F7F007F7F7F007F7F7F000000
      0000000000000000000000000000FFFFFF007F7F7F00FFFFFF00000000000000
      00000000000000000000FFFFFF007F7F7F000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000C6C6C600848484008484
      8400840000008400000084848400848484000000000000000000C6C6C600C6C6
      C600C6C6C600000000000000000000000000FFFF00000000000000FFFF00FFFF
      FF0000FFFF00FFFFFF00000000000000000000000000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF0000000000FFFFFF00000000007F7F7F007F7F7F00FFFFFF000000
      000000000000000000007F7F7F007F7F7F007F7F7F0000000000FFFFFF000000
      0000FFFFFF007F7F7F00000000007F7F7F0000000000000000000000000000FF
      FF00C0C0C00000FFFF00C0C0C00000FFFF00C0C0C00000FFFF00000000000000
      000000000000000000000000000000000000C6C6C60084848400FF000000C6C6
      C600C6C6C600C6C6C600FFFFFF00FFFFFF00000000000000000000000000C6C6
      C60000000000000000000000000000000000FFFF000000000000FFFFFF0000FF
      FF00FFFFFF0000FFFF00FFFFFF0000FFFF00FFFFFF0000000000FFFFFF000000
      000000000000FFFFFF00FFFFFF00000000007F7F7F007F7F7F00FFFFFF000000
      0000000000000000000000000000FFFFFF00FFFFFF007F7F7F00FFFFFF007F7F
      7F007F7F7F00FFFFFF00000000007F7F7F00000000000000000000FFFF000000
      000000FFFF00C0C0C00000FFFF00C0C0C00000FFFF00C0C0C00000FFFF000000
      0000000000000000000000000000000000008484840084848400C6C6C600C6C6
      C600C6C6C600C6C6C600FFFFFF00FFFFFF00FFFFFF0000000000848484000000
      000000000000000000000000000000000000FFFF00000000000000FFFF00FFFF
      FF0000FFFF00FFFFFF00000000000000000000000000000000000000000000FF
      FF0000000000FFFFFF00FFFFFF00000000007F7F7F007F7F7F00FFFFFF000000
      000000000000000000007F7F7F007F7F7F007F7F7F007F7F7F007F7F7F000000
      00007F7F7F0000000000000000007F7F7F00000000000000000000FFFF0000FF
      FF000000000000FFFF00C0C0C00000FFFF00C0C0C00000FFFF00C0C0C00000FF
      FF000000000000000000000000000000000000008400C6C6C600C6C6C600C6C6
      C600C6C6C600C6C6C600C6C6C600C6C6C600C6C6C600C6C6C600848484000000
      000000000000000000000000000000000000FFFF000000000000FFFFFF0000FF
      FF00FFFFFF0000FFFF00FFFFFF0000FFFF00FFFFFF0000FFFF00FFFFFF000000
      0000FFFFFF00FFFFFF00FFFFFF00000000007F7F7F007F7F7F00FFFFFF000000
      000000000000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF007F7F
      7F000000000000000000000000007F7F7F000000000000000000FFFFFF0000FF
      FF0000FFFF000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000840084008400C6C6C600C6C6
      C600C6C6C600840084000000FF000000FF000000FF000000FF00000084000000
      FF000000FF000000FF000000FF000000FF00FFFF00000000000000FFFF00FFFF
      FF0000000000000000000000000000000000000000000000000000000000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00000000007F7F7F007F7F7F00FFFFFF00FFFF
      FF007F7F7F007F7F7F007F7F7F007F7F7F007F7F7F007F7F7F007F7F7F000000
      000000000000FFFFFF00FFFFFF007F7F7F00000000000000000000FFFF00FFFF
      FF0000FFFF000000000000FFFF000000000000FFFF000000000000FFFF000000
      00000000000000000000000000000000000000008400C6C6C600C6C6C600C6C6
      C600C6C6C600C6C6C600C6C6C600C6C6C600C6C6C600C6C6C600848484000000
      00000000000000000000000000000000000000000000000000000000000000FF
      FF00FFFFFF0000FFFF00000000000000000000FFFF0000000000FFFFFF00FFFF
      FF000000000000000000FFFFFF00000000007F7F7F007F7F7F007F7F7F000000
      0000FFFFFF00FFFFFF007F7F7F007F7F7F00000000007F7F7F00000000000000
      00007F7F7F007F7F7F00000000007F7F7F000000000000000000FFFFFF0000FF
      FF00FFFFFF0000FFFF00FFFFFF0000FFFF00FFFFFF0000FFFF00000000000000
      00000000000000000000000000000000000084848400C6C6C600C6C6C600C6C6
      C600C6C6C600C6C6C600FFFFFF00FFFFFF00C6C6C600C6C6C600C6C6C6000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000FFFF0000000000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00000000000000000000000000000000007F7F
      7F007F7F7F007F7F7F007F7F7F00000000007F7F7F0000000000000000000000
      000000000000FFFFFF00FFFFFF007F7F7F00000000000000000000FFFF00FFFF
      FF0000FFFF00FFFFFF0000FFFF00FFFFFF000000000000000000000000000000
      000000000000000000000000000000000000C6C6C60084848400C6C6C600C6C6
      C600C6C6C600C6C6C600C6C6C600C6C6C600FFFFFF0084848400000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000FFFF0000000000FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00000000000000000000000000000000000000000000000000000000000000
      0000000000007F7F7F00000000007F7F7F000000000000000000FFFFFF00FFFF
      FF007F7F7F007F7F7F007F7F7F007F7F7F0000000000000000000000000000FF
      FF00FFFFFF0000FFFF00FFFFFF00000000000000000000000000000000000000
      00000000000000000000000000000000000000000000C6C6C600FF000000C6C6
      C600C6C6C600C6C6C600FFFFFF00FFFFFF0084848400FFFFFF00000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000FFFF0000000000FFFFFF00FFFFFF000000000000000000FFFF
      FF0000000000FFFFFF00FFFFFF00000000000000000000000000000000000000
      00007F7F7F00000000007F7F7F00FFFFFF00000000007F7F7F007F7F7F000000
      00007F7F7F00FFFFFF00000000007F7F7F000000000000000000808080000000
      0000000000000000000000000000808080000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000C6C6C6008400
      000084000000840000008484840084848400FFFFFF0000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000FFFF000000000000000000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF0000000000FFFFFF0000000000000000000000000000000000000000007F7F
      7F00FFFFFF007F7F7F007F7F7F00FFFFFF000000000000000000000000000000
      00007F7F7F00FFFFFF007F7F7F00000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000C6C6
      C600C6C6C600FFFFFF0000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      FF00000000000000000000000000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF000000000000000000000000000000000000000000000000007F7F7F007F7F
      7F007F7F7F00000000007F7F7F00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF007F7F7F007F7F7F0000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000C6C6
      C600C6C6C6000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000007F7F
      7F0000000000000000007F7F7F007F7F7F007F7F7F007F7F7F007F7F7F007F7F
      7F007F7F7F000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000FFFF0000FFFF0000FFFF00000000008484840000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000008484840084848400848484008484840084848400848484008484
      8400848484000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000FFFF000000000000000000000000008484840000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000084848400848484008484840084848400848484008484
      8400000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000F8F8F80000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000100
      0100000000000100010001000100000000000000000000000000000000000000
      000000FFFF0000FFFF0000FFFF00000000008484840000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000084848400848484008484840084848400848484008484
      8400000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000F8F8F800F8F8F800F8F8F800000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000100010001000100000000000000000000000000000000000000
      000000FFFF0000FFFF0000000000000000008484840000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000084848400848484008484840084848400848484008484
      8400000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000F8F8
      F800F8F8F800F8F8F80000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000FFFF0000FFFF0000FFFF00000000008484840000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000008484840084848400848484008484840084848400848484008484
      8400848484000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000F8F8
      F800F8F8F8000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000001000100000000000000000000000000000000000000
      000000FFFF000000000000000000000000008484840000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000848484008484840084848400848484008484840084848400848484008484
      8400848484000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000008484840084848400848484000000
      000000FFFF0000FFFF0000FFFF00000000008484840084848400848484008484
      8400848484008484840084848400848484000000000000000000000000008484
      8400848484008484840084848400848484008484840084848400848484008484
      8400848484008484840000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000848484008484
      8400848484008484840084848400848484008484840084848400848484008484
      8400848484008484840000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000FFFF0000FFFF0000FF
      FF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FF
      FF0000FFFF0000FFFF0000FFFF0000FFFF000000000084848400848484008484
      8400000000008484840084848400848484008484840084848400848484008484
      8400848484008484840000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000FFFF000000000000FF
      FF0000FFFF0000FFFF000000000000FFFF0000FFFF0000FFFF000000000000FF
      FF0000FFFF0000FFFF000000000000FFFF000000000084848400848484000000
      0000000000008484840084848400848484008484840084848400848484008484
      8400848484008484840084848400000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000FFFF000000000000FF
      FF000000000000FFFF000000000000FFFF000000000000FFFF000000000000FF
      FF000000000000FFFF000000000000FFFF000000000000000000000000000000
      0000000000008484840084848400848484008484840084848400848484008484
      8400000000008484840084848400000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000848484008484840000000000848484008484840000000000848484008484
      8400000000008484840084848400000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000FFFF0000FFFF0000FFFF00000000008484840000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000848484008484840000000000848484008484840000000000848484008484
      8400000000000000000084848400000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000FFFF000000000000000000000000008484840000000000000000000000
      0000000000000000000000000000000000000000000000000000000000008484
      8400848484000000000000000000848484008484840000000000848484008484
      8400000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000FFFF0000FFFF0000FFFF00000000008484840000000000000000000000
      0000000000000000000000000000000000000000000000000000000000008484
      8400848484000000000000000000848484008484840000000000848484008484
      8400000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000008484840000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000848484008484840000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000008484000084000000C6C6C6000000000000000000000000000000
      0000000000007F7F7F007F7F7F007F7F7F007F7F7F007F7F7F007F7F7F007F7F
      7F00000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000084848400848484000000
      0000848484008484840000000000848484008484840000000000848484008484
      8400000000008484840084848400000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000084840000C6C6C600FF000000848484000000000000000000000000000000
      0000000000007F7F7F00BFBFBF00BFBFBF00BFBFBF00BFBFBF00BFBFBF00BFBF
      BF00000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000008400
      8400840084008400840000000000848484000000000084008400840084008400
      8400000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000008484
      0000C6C6C600FF00000084000000000000000000000000000000000000000000
      0000000000007F7F7F00BFBFBF000000000000000000BFBFBF00BFBFBF00BFBF
      BF00000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000084848400000000000000
      0000000000008400840084008400848484008400840084008400000000000000
      0000000000000000000084848400000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000084840000C6C6
      C600FF0000008400000000000000000000000000000000000000000000000000
      0000000000007F7F7F00BFBFBF000000000000000000BFBFBF00BFBFBF00BFBF
      BF0000000000000000000000000000000000000000000000000000000000FFFF
      FF00FFFFFF000000000000000000000000000000000000000000FFFFFF00FFFF
      FF00000000000000000000000000000000000000000084848400000000000000
      0000000000008484840084848400848484008484840084848400000000000000
      0000000000000000000084848400000000000000000000000000000000000000
      0000000000000000000000000000000000000000000084848400C6C6C600FF00
      0000840000000000000000000000000000000000000000000000000000000000
      0000000000007F7F7F00BFBFBF00BFBFBF00BFBFBF00BFBFBF00BFBFBF00BFBF
      BF00000000000000000000000000000000000000000000000000FFFFFF000000
      000000000000FFFFFF00000000000000000000000000FFFFFF00000000000000
      0000FFFFFF000000000000000000000000000000000084848400000000000000
      00000000000084848400C6C6C600C6C6C600C6C6C60084848400000000000000
      000000000000000000000000000000000000000000000000000000000000C6C6
      C600848484000000000000000000848484008484840000000000848484008400
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000FFFFFF000000
      000000000000FFFFFF00000000000000000000000000FFFFFF00000000000000
      0000FFFFFF000000000000000000000000000000000000000000000000000000
      000084848400C6C6C600C6C6C600C6C6C600C6C6C600C6C6C600848484000000
      0000000000000000000084848400000000000000000000000000C6C6C600C6C6
      C600848484008484000084840000848400000000000000000000848484000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000FFFFFF000000
      000000000000FFFFFF00000000000000000000000000FFFFFF00000000000000
      0000FFFFFF00000000000000000000000000000000008484840000000000C6C6
      C600C6C6C600C6C6C600C6C6C600C6C6C600C6C6C600C6C6C600C6C6C600C6C6
      C6000000000000000000848484000000000000000000C6C6C600C6C6C6008484
      0000FFFF0000C6C6C600C6C6C600C6C6C600C6C6C60084840000848484000000
      000000000000000000000000000000000000000000000000000000000000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00000000000000000000000000000000000000000000000000FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00000000000000000000000000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF000000000000000000000000000000000084848400000000000000
      0000C6C6C600C6C6C600C6C6C600C6C6C600C6C6C600C6C6C600C6C6C6000000
      000000000000000000000000000000000000000000000000000084840000FFFF
      0000C6C6C60000000000C6C6C600C6C6C600C6C6C600FFFF0000848484000000
      000000000000000000000000000000000000000000000000000000000000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF0000000000000000000000000000000000000000000000000000000000FFFF
      FF00FFFFFF000000000000000000000000000000000000000000FFFFFF00FFFF
      FF00000000000000000000000000000000000000000000000000000000000000
      0000C6C6C6008484840084848400C6C6C6008484840084848400C6C6C6000000
      000000000000000000008484840000000000C6C6C600C6C6C60084840000C6C6
      C6000000000000000000C6C6C600C6C6C600C6C6C600FFFF0000848484000000
      000000000000000000000000000000000000000000000000000000000000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000084848400000000000000
      0000C6C6C600C6C6C600C6C6C600C6C6C600C6C6C600C6C6C600C6C6C6000000
      000000000000000000008484840000000000C6C6C60084848400FFFF00000000
      000000000000C6C6C600C6C6C600C6C6C600C6C6C600FFFF0000848484000000
      000000000000000000000000000000000000000000000000000000000000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000084848400000000000000
      000000000000C6C6C600C6C6C600C6C6C600C6C6C600C6C6C600000000000000
      000000000000000000000000000000000000C6C6C60084848400FFFF00000000
      000000000000C6C6C600C6C6C600C6C6C600C6C6C600C6C6C600848484000000
      000000000000000000000000000000000000000000000000000000000000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000C6C6
      C60000000000000000000000000000000000000000000000000000000000C6C6
      C600000000000000000084848400000000000000000000000000FFFF0000C6C6
      C60000000000C6C6C600C6C6C600C6C6C600C6C6C60000000000848484000000
      000000000000000000000000000000000000000000000000000000000000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00000000007F7F7F0000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000084848400000000000000
      0000848484000000000000000000000000000000000000000000848484000000
      000000000000000000008484840000000000000000000000000000000000FFFF
      0000C6C6C600C6C6C600FFFF000084848400C6C6C60084848400000000000000
      000000000000000000000000000000000000000000000000000000000000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000848484008484
      8400000000008484840084848400000000008484840084848400000000008484
      8400848484000000000084848400000000000000000000000000C6C6C6000000
      0000848400008484000084848400848484008484840000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      FF000000FF000000FF000000FF000000FF000000FF000000FF000000FF000000
      FF00000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000848484008484840084848400C6C6C6000000000000000000000000000000
      000000000000000000000000000000000000424D3E000000000000003E000000
      2800000040000000500000000100010000000000800200000000000000000000
      000000000000000000000000FFFFFF0000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000FFFFF30FFFFFFFFFEFFDF38FFFFFFFFF
      C7FFF30FFFFFFE00C3FBE10F9FF1FE00E3F7E30F6004FE00F1E7E30F60048000
      F8CFE10700008000FC1FE40700008000FE3FF40700018000FC1FFE0700038001
      F8CFFE078FEF8003E1E7FE078F9F8007C3F3FE07C07F807FC7FDFF9FFFFF80FF
      FFFFFF9FFFFF81FFFFFFFF9FFFFFFFFFE7CFFC00FC00FFFFE783FC009068FFFF
      E70320000092FFFFC00300001E3CC03F800700001C52801F000F00001E02800F
      001F00001C168007001F0000180E8007000000000018800F001F000010B2800F
      001FE000E178801F003FF800FAC0C0FF803FF000F492C0FFC07FE001E0F1FFFF
      E3FFC403C403FFFFE7FFEC07EC07FFFFE07FF807FFFBFFFFE07FFC0F92519249
      E07FFC0F92419249E07FFC0FFFC1FFFFE07FF8079FC39FF9E07FF0079FC19FF9
      0000E003FFFFFFFF0000C0039FF99FF9000088039FF99FF900009801FFFFFFFF
      0000F8099FF99FF90000F2499FF99FF9E07FF24DFFFFFFFFE07FE64F92499249
      E07FE64F92499249E07FFE7FFFFFFFFFFFFFFFFFFFFFFFF88003FFFF9249FFF0
      0001FFFFC287FFE10001E7CFA00DFFC30001C387B83DFF8700018103B83FE04F
      00018103F01DC01F00018103A00D801F00018103A00FC41F0001C387E00D0C1F
      0001E7CFA00D181F0001BFFBA00F181F0001C387E00D885F0001FFFFB01DC03F
      0001FFFFC925C07F8003FFFFFFFFF0FF00000000000000000000000000000000
      000000000000}
  end
  object ApplicationEvents1: TApplicationEvents
    OnIdle = ApplicationEvents1Idle
    OnHint = ApplicationEvents1Hint
    Left = 672
    Top = 96
  end
  object SaveDialog1: TSaveDialog
    DefaultExt = '.xml'
    Filter = 'XML files|*.xml'
    Options = [ofOverwritePrompt, ofHideReadOnly, ofEnableSizing]
    Left = 704
    Top = 96
  end
  object PopupMenu1: TPopupMenu
    Images = ImageList1
    Left = 736
    Top = 48
    object Copy1: TMenuItem
      Action = EditCopy1
    end
    object CopyImage1: TMenuItem
      Action = ImageCopyImageAction
    end
    object N8: TMenuItem
      Caption = '-'
    end
    object DeleteImage1: TMenuItem
      Action = ImageDelImageAction
    end
  end
  object Timer1: TTimer
    Enabled = False
    Interval = 100
    OnTimer = Timer1Timer
    Left = 736
    Top = 96
  end
  object PhZoomToRectTool1: TPhZoomToRectTool
    PhImage = PhImage2
    Left = 734
    Top = 148
  end
  object PhSelRectTool1: TPhSelRectTool
    PhImage = PhImage2
    Left = 670
    Top = 204
  end
  object PhPaneTool1: TPhPaneTool
    PhImage = PhImage2
    Left = 670
    Top = 276
  end
  object PhLenzTool1: TPhLenzTool
    PhImage = PhImage2
    Left = 710
    Top = 276
  end
  object PhRulerTool1: TPhRulerTool
    PhImage = PhImage2
    Left = 366
    Top = 44
  end
  object PhTriangleTool1: TPhTriangleTool
    PhImage = PhImage2
    Left = 366
    Top = 100
  end
end
