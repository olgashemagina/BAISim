object RectToolForm: TRectToolForm
  Left = 614
  Top = 111
  BorderIcons = [biSystemMenu, biMinimize, biMaximize, biHelp]
  BorderStyle = bsToolWindow
  Caption = 'Rectangle properties'
  ClientHeight = 133
  ClientWidth = 231
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  OnClose = FormClose
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 8
    Top = 8
    Width = 52
    Height = 13
    Caption = 'Base width'
  end
  object Label2: TLabel
    Left = 103
    Top = 8
    Width = 56
    Height = 13
    Caption = 'Base height'
  end
  object Label3: TLabel
    Left = 8
    Top = 56
    Width = 54
    Height = 13
    Caption = 'Object type'
  end
  object Label4: TLabel
    Left = 104
    Top = 56
    Width = 34
    Height = 13
    Caption = 'Racurs'
  end
  object ComboBox1: TComboBox
    Left = 8
    Top = 27
    Width = 89
    Height = 21
    Style = csDropDownList
    Enabled = False
    TabOrder = 0
    OnChange = ComboBox1Change
    Items.Strings = (
      '')
  end
  object ComboBox2: TComboBox
    Left = 103
    Top = 27
    Width = 120
    Height = 21
    Style = csDropDownList
    Enabled = False
    TabOrder = 1
    OnChange = ComboBox2Change
    Items.Strings = (
      '')
  end
  object ComboBox3: TComboBox
    Left = 8
    Top = 75
    Width = 81
    Height = 21
    Style = csDropDownList
    TabOrder = 2
    OnChange = ComboBox3Change
    OnExit = ComboBox3Exit
  end
  object ComboBox4: TComboBox
    Left = 103
    Top = 77
    Width = 122
    Height = 21
    Style = csDropDownList
    ItemIndex = 0
    TabOrder = 3
    Text = 'Front'
    OnChange = ComboBox4Change
    Items.Strings = (
      'Front'
      'Left semi-profile'
      'Left profile '
      'Left back semi-profile'
      'Back '
      'Right back semi-profile'
      'Right profile'
      'Right semi-profile'
      'Bottom front '
      'Bottom left semi-propfile '
      'Bottom left profile'
      'Bottom back left semi-profile'
      'Bottom back '
      'Bottom back right semi profile'
      'Bottom right profile'
      'Bottom right semi propfile'
      'Bottom'
      'Up front '
      'Up left semi profile'
      'Up left profile'
      'Up left back semi profile'
      'Up back '
      'Up back right semi profile'
      'Up right profile'
      'Up right semi profile '
      'Up')
  end
  object CheckBox1: TCheckBox
    Left = 8
    Top = 108
    Width = 151
    Height = 17
    Caption = 'Set nearest overlaped rect'
    TabOrder = 4
    OnClick = CheckBox1Click
  end
end
