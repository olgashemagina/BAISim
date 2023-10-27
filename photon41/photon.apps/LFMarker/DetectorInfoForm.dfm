object DetectorForm: TDetectorForm
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = 'Detector info'
  ClientHeight = 529
  ClientWidth = 355
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 355
    Height = 89
    Align = alTop
    TabOrder = 0
    object Label1: TLabel
      Left = 16
      Top = 64
      Width = 25
      Height = 13
      Caption = 'View '
    end
    object Label3: TLabel
      Left = 16
      Top = 8
      Width = 27
      Height = 13
      Caption = 'Name'
    end
    object Label4: TLabel
      Left = 16
      Top = 27
      Width = 56
      Height = 13
      Caption = 'Num stages'
    end
    object Label5: TLabel
      Left = 93
      Top = 8
      Width = 76
      Height = 13
      Caption = '-------------------'
    end
    object Label6: TLabel
      Left = 93
      Top = 27
      Width = 76
      Height = 13
      Caption = '-------------------'
    end
    object Label7: TLabel
      Left = 93
      Top = 45
      Width = 76
      Height = 13
      Caption = '-------------------'
    end
    object Label8: TLabel
      Left = 16
      Top = 45
      Width = 61
      Height = 13
      Caption = 'Num sensors'
    end
    object ComboBox1: TComboBox
      Left = 80
      Top = 62
      Width = 249
      Height = 21
      Style = csDropDownList
      ItemIndex = 0
      TabOrder = 0
      Text = 'Num sensors in the stage'
      OnChange = ComboBox1Change
      Items.Strings = (
        'Num sensors in the stage'
        'Stages threshold'
        'Srages weight ')
    end
    object Button1: TButton
      Left = 264
      Top = 24
      Width = 75
      Height = 25
      Caption = 'Button1'
      TabOrder = 1
      OnClick = Button1Click
    end
  end
  object Panel2: TPanel
    Left = 0
    Top = 339
    Width = 355
    Height = 86
    Align = alTop
    TabOrder = 1
    object Label2: TLabel
      Left = 16
      Top = 53
      Width = 28
      Height = 13
      Caption = 'Stage'
    end
    object ComboBox2: TComboBox
      Left = 50
      Top = 51
      Width = 279
      Height = 21
      Style = csDropDownList
      ItemIndex = 0
      TabOrder = 0
      Text = 'All'
      OnChange = ComboBox2Change
      Items.Strings = (
        'All')
    end
    object CheckBox1: TCheckBox
      Left = 16
      Top = 30
      Width = 153
      Height = 17
      Caption = 'Use weight of engine sensors '
      TabOrder = 1
      OnClick = CheckBox1Click
    end
    object CheckBox2: TCheckBox
      Left = 16
      Top = 8
      Width = 97
      Height = 17
      Caption = 'Fill sensors area'
      TabOrder = 2
      OnClick = CheckBox2Click
    end
  end
  object PhImage1: TPhImage
    Left = 0
    Top = 425
    Width = 355
    Height = 104
    ThumbWidht = 128
    ThumbHeight = 128
    SlideShowInterval = 500
    Align = alClient
    ParentColor = False
  end
  object Chart1: TChart
    Left = 0
    Top = 89
    Width = 355
    Height = 250
    BackWall.Brush.Gradient.Direction = gdBottomTop
    BackWall.Brush.Gradient.EndColor = clWhite
    BackWall.Brush.Gradient.StartColor = 15395562
    BackWall.Brush.Gradient.Visible = True
    BackWall.Transparent = False
    Foot.Font.Color = clBlue
    Foot.Font.Name = 'Verdana'
    Gradient.Direction = gdBottomTop
    Gradient.EndColor = clWhite
    Gradient.MidColor = 15395562
    Gradient.StartColor = 15395562
    Gradient.Visible = True
    LeftWall.Color = 14745599
    Legend.Font.Name = 'Verdana'
    Legend.Shadow.Transparency = 0
    Legend.Visible = False
    RightWall.Color = 14745599
    Title.Font.Name = 'Verdana'
    Title.Text.Strings = (
      'TChart')
    BottomAxis.Axis.Color = 4210752
    BottomAxis.Grid.Color = 11119017
    BottomAxis.LabelsFormat.Font.Name = 'Verdana'
    BottomAxis.TicksInner.Color = 11119017
    BottomAxis.Title.Font.Name = 'Verdana'
    DepthAxis.Axis.Color = 4210752
    DepthAxis.Grid.Color = 11119017
    DepthAxis.LabelsFormat.Font.Name = 'Verdana'
    DepthAxis.TicksInner.Color = 11119017
    DepthAxis.Title.Font.Name = 'Verdana'
    DepthTopAxis.Axis.Color = 4210752
    DepthTopAxis.Grid.Color = 11119017
    DepthTopAxis.LabelsFormat.Font.Name = 'Verdana'
    DepthTopAxis.TicksInner.Color = 11119017
    DepthTopAxis.Title.Font.Name = 'Verdana'
    LeftAxis.Axis.Color = 4210752
    LeftAxis.Grid.Color = 11119017
    LeftAxis.LabelsFormat.Font.Name = 'Verdana'
    LeftAxis.TicksInner.Color = 11119017
    LeftAxis.Title.Font.Name = 'Verdana'
    RightAxis.Axis.Color = 4210752
    RightAxis.Grid.Color = 11119017
    RightAxis.LabelsFormat.Font.Name = 'Verdana'
    RightAxis.TicksInner.Color = 11119017
    RightAxis.Title.Font.Name = 'Verdana'
    TopAxis.Axis.Color = 4210752
    TopAxis.Grid.Color = 11119017
    TopAxis.LabelsFormat.Font.Name = 'Verdana'
    TopAxis.TicksInner.Color = 11119017
    TopAxis.Title.Font.Name = 'Verdana'
    Align = alTop
    TabOrder = 3
    DefaultCanvas = 'TGDIPlusCanvas'
    ColorPaletteIndex = 13
    object Series1: TBarSeries
      XValues.Name = 'X'
      XValues.Order = loAscending
      YValues.Name = 'Bar'
      YValues.Order = loNone
    end
  end
end
