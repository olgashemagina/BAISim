object SelectBuildConfForm: TSelectBuildConfForm
  Left = 0
  Top = 0
  Caption = 'Select configuration file'
  ClientHeight = 171
  ClientWidth = 258
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Choose: TRadioGroup
    Left = 8
    Top = 8
    Width = 233
    Height = 81
    ItemIndex = 0
    Items.Strings = (
      'Create new configuration file'
      'Use existing configuration file')
    TabOrder = 0
    OnClick = ChooseClick
  end
  object Edit1: TEdit
    Left = 8
    Top = 95
    Width = 202
    Height = 21
    TabOrder = 1
    Visible = False
  end
  object Button1: TButton
    Left = 216
    Top = 93
    Width = 25
    Height = 25
    Caption = '...'
    TabOrder = 2
    Visible = False
    OnClick = Button1Click
  end
  object OKButton: TButton
    Left = 166
    Top = 137
    Width = 75
    Height = 25
    Caption = 'OK'
    TabOrder = 3
    OnClick = OKButtonClick
  end
  object CancelButton: TButton
    Left = 84
    Top = 137
    Width = 76
    Height = 25
    Caption = 'Cancel'
    TabOrder = 4
    OnClick = CancelButtonClick
  end
  object OpenDialog1: TOpenDialog
    Filter = 'xml|*.xml'
    Left = 200
    Top = 32
  end
end
