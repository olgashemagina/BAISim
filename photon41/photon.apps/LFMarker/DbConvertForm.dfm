object DbConvertDlg: TDbConvertDlg
  Left = 685
  Top = 367
  BorderStyle = bsDialog
  Caption = 'Convert database settings '
  ClientHeight = 182
  ClientWidth = 208
  Color = clBtnFace
  ParentFont = True
  OldCreateOrder = True
  Position = poScreenCenter
  PixelsPerInch = 96
  TextHeight = 13
  object Bevel1: TBevel
    Left = 8
    Top = 8
    Width = 193
    Height = 137
    Shape = bsFrame
  end
  object Label1: TLabel
    Left = 88
    Top = 17
    Width = 55
    Height = 13
    Caption = 'Base width '
  end
  object Label2: TLabel
    Left = 16
    Top = 88
    Width = 100
    Height = 13
    Caption = 'Output image format'
  end
  object OKBtn: TButton
    Left = 23
    Top = 148
    Width = 75
    Height = 25
    Caption = 'OK'
    Default = True
    ModalResult = 1
    TabOrder = 0
  end
  object CancelBtn: TButton
    Left = 103
    Top = 148
    Width = 75
    Height = 25
    Cancel = True
    Caption = 'Cancel'
    ModalResult = 2
    TabOrder = 1
  end
  object CheckBox1: TCheckBox
    Left = 16
    Top = 16
    Width = 57
    Height = 17
    Caption = 'Resize '
    TabOrder = 2
  end
  object Edit1: TEdit
    Left = 152
    Top = 16
    Width = 33
    Height = 21
    TabOrder = 3
    Text = '320'
  end
  object CheckBox2: TCheckBox
    Left = 16
    Top = 40
    Width = 169
    Height = 17
    Caption = 'Use interpolation when resize '
    TabOrder = 4
  end
  object CheckBox3: TCheckBox
    Left = 16
    Top = 64
    Width = 161
    Height = 17
    Caption = 'Rename to UUID '
    TabOrder = 5
  end
  object ComboBox1: TComboBox
    Left = 16
    Top = 112
    Width = 145
    Height = 21
    Style = csDropDownList
    ItemIndex = 0
    TabOrder = 6
    Text = 'jpeg'
    Items.Strings = (
      'jpeg'
      'awp')
  end
end
