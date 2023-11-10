object dictinaryItemDlg: TdictinaryItemDlg
  Left = 227
  Top = 108
  BorderStyle = bsDialog
  Caption = 'Class label editor'
  ClientHeight = 257
  ClientWidth = 319
  Color = clBtnFace
  ParentFont = True
  OldCreateOrder = True
  Position = poOwnerFormCenter
  PixelsPerInch = 96
  TextHeight = 13
  object Bevel1: TBevel
    Left = 8
    Top = 8
    Width = 225
    Height = 241
    Shape = bsFrame
  end
  object Label1: TLabel
    Left = 16
    Top = 19
    Width = 53
    Height = 13
    Caption = 'Class Label'
  end
  object Label2: TLabel
    Left = 18
    Top = 82
    Width = 74
    Height = 13
    Caption = 'Aperture Width'
  end
  object Label3: TLabel
    Left = 18
    Top = 113
    Width = 77
    Height = 13
    Caption = 'Aperture Height'
  end
  object Label4: TLabel
    Left = 18
    Top = 142
    Width = 53
    Height = 13
    Caption = 'Class Color'
  end
  object Label5: TLabel
    Left = 18
    Top = 50
    Width = 60
    Height = 13
    Caption = 'Class shape '
  end
  object OKBtn: TButton
    Left = 239
    Top = 8
    Width = 75
    Height = 25
    Caption = 'OK'
    Default = True
    ModalResult = 1
    TabOrder = 0
  end
  object CancelBtn: TButton
    Left = 239
    Top = 38
    Width = 75
    Height = 25
    Cancel = True
    Caption = 'Cancel'
    ModalResult = 2
    TabOrder = 1
  end
  object Edit1: TEdit
    Left = 96
    Top = 16
    Width = 121
    Height = 21
    TabOrder = 2
    Text = 'new item'
  end
  object SpinEdit1: TSpinEdit
    Left = 155
    Top = 78
    Width = 62
    Height = 22
    Increment = 2
    MaxValue = 64
    MinValue = 8
    TabOrder = 3
    Value = 24
  end
  object SpinEdit2: TSpinEdit
    Left = 155
    Top = 110
    Width = 62
    Height = 22
    Increment = 2
    MaxValue = 64
    MinValue = 8
    TabOrder = 4
    Value = 24
  end
  object ColorGrid1: TColorGrid
    Left = 113
    Top = 142
    Width = 104
    Height = 88
    ClickEnablesColor = True
    ForegroundIndex = 2
    BackgroundIndex = 3
    BackgroundEnabled = False
    TabOrder = 5
    OnClick = ColorGrid1Click
  end
  object Panel1: TPanel
    Left = 18
    Top = 161
    Width = 89
    Height = 69
    Color = clRed
    ParentBackground = False
    ShowCaption = False
    TabOrder = 6
  end
  object ComboBox1: TComboBox
    Left = 96
    Top = 48
    Width = 121
    Height = 21
    Style = csDropDownList
    ItemIndex = 0
    TabOrder = 7
    Text = 'Rectangle'
    Items.Strings = (
      'Rectangle'
      'Vector '
      'Polygon ')
  end
end
