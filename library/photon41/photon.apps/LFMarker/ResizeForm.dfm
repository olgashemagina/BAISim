object ResizeDlg: TResizeDlg
  Left = 227
  Top = 108
  BorderStyle = bsDialog
  Caption = 'Resize image'
  ClientHeight = 242
  ClientWidth = 384
  Color = clBtnFace
  ParentFont = True
  OldCreateOrder = True
  Position = poMainFormCenter
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Bevel1: TBevel
    Left = 8
    Top = 8
    Width = 281
    Height = 199
    Shape = bsFrame
  end
  object Label1: TLabel
    Left = 32
    Top = 59
    Width = 59
    Height = 13
    Caption = 'Image width'
  end
  object Label2: TLabel
    Left = 32
    Top = 86
    Width = 64
    Height = 13
    Caption = 'Image Height'
  end
  object Label3: TLabel
    Left = 32
    Top = 24
    Width = 41
    Height = 13
    Caption = 'Change:'
  end
  object OKBtn: TButton
    Left = 300
    Top = 8
    Width = 75
    Height = 25
    Caption = 'OK'
    Default = True
    ModalResult = 1
    TabOrder = 0
  end
  object CancelBtn: TButton
    Left = 300
    Top = 38
    Width = 75
    Height = 25
    Cancel = True
    Caption = 'Cancel'
    ModalResult = 2
    TabOrder = 1
  end
  object CheckBox1: TCheckBox
    Left = 32
    Top = 111
    Width = 121
    Height = 17
    Caption = 'Keep aspect ratio'
    Checked = True
    State = cbChecked
    TabOrder = 2
    OnClick = CheckBox1Click
  end
  object RadioButton1: TRadioButton
    Left = 120
    Top = 23
    Width = 73
    Height = 17
    Caption = 'Persents'
    Checked = True
    TabOrder = 3
    TabStop = True
    OnClick = RadioButton1Click
  end
  object RadioButton2: TRadioButton
    Left = 199
    Top = 22
    Width = 50
    Height = 17
    Caption = 'Pixels'
    TabOrder = 4
    OnClick = RadioButton2Click
  end
  object SpinEdit1: TSpinEdit
    Left = 120
    Top = 56
    Width = 121
    Height = 22
    MaxValue = 500
    MinValue = 1
    TabOrder = 5
    Value = 100
    OnChange = SpinEdit1Change
  end
  object SpinEdit2: TSpinEdit
    Left = 120
    Top = 83
    Width = 121
    Height = 22
    MaxValue = 500
    MinValue = 1
    TabOrder = 6
    Value = 100
    OnChange = SpinEdit2Change
  end
  object GroupBox1: TGroupBox
    Left = 32
    Top = 138
    Width = 113
    Height = 57
    Caption = 'Current Image'
    TabOrder = 7
    object currentHeightLabel: TLabel
      Left = 8
      Top = 34
      Width = 91
      Height = 13
      Caption = 'currentHeightLabel'
    end
    object currenWidthLabel: TLabel
      Left = 8
      Top = 18
      Width = 84
      Height = 13
      Caption = 'currenWidthLabel'
    end
  end
  object GroupBox2: TGroupBox
    Left = 151
    Top = 138
    Width = 113
    Height = 57
    Caption = 'New Image'
    TabOrder = 8
    object newWidthLabel: TLabel
      Left = 8
      Top = 20
      Width = 73
      Height = 13
      Caption = 'newWidthLabel'
    end
    object newHeightLabel: TLabel
      Left = 8
      Top = 35
      Width = 76
      Height = 13
      Caption = 'newHeightLabel'
    end
  end
  object CheckBox2: TCheckBox
    Left = 16
    Top = 216
    Width = 273
    Height = 17
    Caption = 'Keep user choise '
    TabOrder = 9
  end
end
