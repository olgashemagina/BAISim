object OKBottomDlg: TOKBottomDlg
  Left = 720
  Top = 424
  BorderStyle = bsDialog
  Caption = 'Convert database '
  ClientHeight = 226
  ClientWidth = 297
  Color = clBtnFace
  ParentFont = True
  OldCreateOrder = True
  Position = poScreenCenter
  PixelsPerInch = 96
  TextHeight = 13
  object OKBtn: TButton
    Left = 71
    Top = 196
    Width = 75
    Height = 25
    Caption = 'OK'
    Default = True
    ModalResult = 1
    TabOrder = 0
  end
  object CancelBtn: TButton
    Left = 151
    Top = 196
    Width = 75
    Height = 25
    Cancel = True
    Caption = 'Cancel'
    ModalResult = 2
    TabOrder = 1
  end
  object GroupBox1: TGroupBox
    Left = 8
    Top = 8
    Width = 281
    Height = 177
    Caption = 'Convert options'
    TabOrder = 2
    object Label1: TLabel
      Left = 8
      Top = 152
      Width = 67
      Height = 13
      Caption = 'Output Format'
    end
    object Label3: TLabel
      Left = 8
      Top = 24
      Width = 43
      Height = 13
      Caption = 'To path: '
    end
    object Label2: TLabel
      Left = 24
      Top = 80
      Width = 90
      Height = 13
      Caption = 'Image Base Width '
    end
    object CheckBox1: TCheckBox
      Left = 8
      Top = 123
      Width = 153
      Height = 17
      Caption = 'Rename Files to UUID '
      TabOrder = 0
    end
    object ComboBox1: TComboBox
      Left = 88
      Top = 144
      Width = 177
      Height = 21
      Style = csDropDownList
      ItemHeight = 13
      ItemIndex = 0
      TabOrder = 1
      Text = '*.awp image files'
      Items.Strings = (
        '*.awp image files'
        '*.jpg  image files')
    end
    object Edit2: TEdit
      Left = 56
      Top = 20
      Width = 169
      Height = 21
      TabOrder = 2
      Text = 'Edit2'
    end
    object Button1: TButton
      Left = 232
      Top = 16
      Width = 33
      Height = 25
      Caption = '...'
      TabOrder = 3
    end
    object CheckBox2: TCheckBox
      Left = 8
      Top = 56
      Width = 97
      Height = 17
      Caption = 'Resize Images '
      TabOrder = 4
    end
    object CSpinEdit1: TCSpinEdit
      Left = 120
      Top = 75
      Width = 145
      Height = 22
      TabOrder = 5
    end
    object CheckBox3: TCheckBox
      Left = 8
      Top = 102
      Width = 153
      Height = 17
      Caption = 'Use beelinear interpolation'
      TabOrder = 6
    end
  end
end
