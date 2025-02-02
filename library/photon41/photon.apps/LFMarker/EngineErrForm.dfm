object EngineErrDlg: TEngineErrDlg
  Left = 284
  Top = 126
  BorderStyle = bsDialog
  Caption = 'Dialog'
  ClientHeight = 552
  ClientWidth = 455
  Color = clBtnFace
  ParentFont = True
  Position = poScreenCenter
  OnShow = FormShow
  TextHeight = 15
  object Bevel1: TBevel
    Left = 7
    Top = 72
    Width = 442
    Height = 392
    Shape = bsFrame
  end
  object Label1: TLabel
    Left = 7
    Top = 21
    Width = 76
    Height = 15
    Caption = 'Test database :'
  end
  object Label2: TLabel
    Left = 7
    Top = 53
    Width = 102
    Height = 15
    Caption = 'Overlaps threshold:'
  end
  object Label3: TLabel
    Left = 192
    Top = 48
    Width = 214
    Height = 15
    Caption = 'the proportion of overlapping rectangles'
  end
  object Label16: TLabel
    Left = 8
    Top = 470
    Width = 3
    Height = 15
    Visible = False
  end
  object OKBtn: TButton
    Left = 372
    Top = 519
    Width = 75
    Height = 25
    Caption = 'Close'
    Default = True
    ModalResult = 1
    TabOrder = 0
  end
  object Edit1: TEdit
    Left = 87
    Top = 19
    Width = 361
    Height = 23
    Enabled = False
    TabOrder = 1
  end
  object Chart1: TChart
    Left = 24
    Top = 79
    Width = 408
    Height = 208
    BackWall.Brush.Style = bsClear
    Legend.Visible = False
    Title.Text.Strings = (
      'Engine error rate')
    View3D = False
    BevelOuter = bvNone
    TabOrder = 2
    DefaultCanvas = 'TGDIPlusCanvas'
    ColorPaletteIndex = 13
    object Series1: TBarSeries
      HoverElement = []
      SeriesColor = clRed
      XValues.Name = 'X'
      XValues.Order = loAscending
      YValues.Name = 'Bar'
      YValues.Order = loNone
    end
  end
  object Edit2: TEdit
    Left = 125
    Top = 46
    Width = 60
    Height = 23
    TabOrder = 3
    Text = '0,5'
  end
  object GroupBox1: TGroupBox
    Left = 24
    Top = 287
    Width = 408
    Height = 170
    Caption = 'Performance Report'
    TabOrder = 4
    object Label4: TLabel
      Left = 11
      Top = 19
      Width = 182
      Height = 15
      Caption = 'Number of images in the database'
    end
    object Label5: TLabel
      Left = 11
      Top = 38
      Width = 182
      Height = 15
      Caption = 'Number of objects in the database'
    end
    object Label6: TLabel
      Left = 223
      Top = 19
      Width = 40
      Height = 14
      Caption = '*****'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Label7: TLabel
      Left = 223
      Top = 38
      Width = 40
      Height = 14
      Caption = '*****'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Label8: TLabel
      Left = 11
      Top = 57
      Width = 173
      Height = 15
      Caption = 'False acceptance rate per objects'
    end
    object Label9: TLabel
      Left = 223
      Top = 57
      Width = 40
      Height = 14
      Caption = '*****'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Label10: TLabel
      Left = 11
      Top = 76
      Width = 184
      Height = 15
      Caption = 'False acceptance rate per fragment'
    end
    object Label11: TLabel
      Left = 223
      Top = 76
      Width = 40
      Height = 14
      Caption = '*****'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Label12: TLabel
      Left = 11
      Top = 95
      Width = 101
      Height = 15
      Caption = 'False rejection rate '
    end
    object Label13: TLabel
      Left = 223
      Top = 95
      Width = 40
      Height = 14
      Caption = '*****'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Label14: TLabel
      Left = 11
      Top = 149
      Width = 179
      Height = 15
      Caption = 'The processing time of one image'
    end
    object Label15: TLabel
      Left = 223
      Top = 153
      Width = 40
      Height = 14
      Caption = '*****'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Label17: TLabel
      Left = 11
      Top = 113
      Width = 48
      Height = 15
      Caption = 'Precision'
    end
    object Label18: TLabel
      Left = 11
      Top = 132
      Width = 31
      Height = 15
      Caption = 'Recall'
    end
    object Label19: TLabel
      Left = 223
      Top = 113
      Width = 40
      Height = 14
      Caption = '*****'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Label20: TLabel
      Left = 223
      Top = 133
      Width = 40
      Height = 14
      Caption = '*****'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
    end
  end
  object Button2: TButton
    Left = 8
    Top = 491
    Width = 407
    Height = 25
    Caption = 'calculate performance'
    TabOrder = 5
    OnClick = Button2Click
  end
  object ProgressBar1: TProgressBar
    Left = 6
    Top = 533
    Width = 360
    Height = 11
    TabOrder = 6
    Visible = False
  end
  object OpenDialog1: TOpenDialog
    Left = 384
    Top = 296
  end
end
