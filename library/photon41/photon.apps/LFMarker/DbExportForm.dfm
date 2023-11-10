object DbExportDialog: TDbExportDialog
  Left = 349
  Top = 107
  BorderStyle = bsDialog
  Caption = 'Export images'
  ClientHeight = 563
  ClientWidth = 313
  Color = clBtnFace
  ParentFont = True
  OldCreateOrder = True
  Position = poScreenCenter
  PixelsPerInch = 96
  TextHeight = 13
  object Bevel1: TBevel
    Left = 8
    Top = 39
    Width = 297
    Height = 485
    Shape = bsFrame
  end
  object Label1: TLabel
    Left = 16
    Top = 12
    Width = 37
    Height = 13
    Caption = 'To path'
  end
  object Label5: TLabel
    Left = 24
    Top = 44
    Width = 181
    Height = 13
    Caption = 'Export data with following class labels'
  end
  object OKBtn: TButton
    Left = 76
    Top = 530
    Width = 75
    Height = 25
    Caption = 'OK'
    Default = True
    ModalResult = 1
    TabOrder = 0
  end
  object CancelBtn: TButton
    Left = 157
    Top = 530
    Width = 75
    Height = 25
    Cancel = True
    Caption = 'Cancel'
    ModalResult = 2
    TabOrder = 1
  end
  object Edit1: TEdit
    Left = 64
    Top = 8
    Width = 210
    Height = 21
    Enabled = False
    TabOrder = 2
  end
  object Button1: TButton
    Left = 280
    Top = 8
    Width = 25
    Height = 25
    Caption = '...'
    TabOrder = 3
    OnClick = Button1Click
  end
  object GroupBox1: TGroupBox
    Left = 16
    Top = 119
    Width = 281
    Height = 114
    Caption = 'Boxes to export '
    TabOrder = 4
    object Label6: TLabel
      Left = 9
      Top = 94
      Width = 166
      Height = 13
      Caption = 'Or with overlapped with threshold '
    end
    object Edit2: TEdit
      Left = 189
      Top = 86
      Width = 76
      Height = 21
      TabOrder = 0
      Text = '0,7'
    end
    object CheckBox3: TCheckBox
      Left = 8
      Top = 45
      Width = 257
      Height = 17
      Caption = 'Nearlest overlapped with marking '
      TabOrder = 1
    end
    object ComboBox1: TComboBox
      Left = 9
      Top = 18
      Width = 256
      Height = 21
      Style = csDropDownList
      ItemIndex = 1
      TabOrder = 2
      Text = 'Human markerd items'
      Items.Strings = (
        'Scanned items '
        'Human markerd items')
    end
    object CheckBox8: TCheckBox
      Left = 8
      Top = 68
      Width = 250
      Height = 17
      Caption = 'Boxes covered marking box'
      TabOrder = 3
    end
  end
  object CheckListBox1: TCheckListBox
    Left = 24
    Top = 63
    Width = 257
    Height = 50
    ItemHeight = 13
    TabOrder = 5
  end
  object GroupBox2: TGroupBox
    Left = 16
    Top = 233
    Width = 281
    Height = 50
    Caption = 'Image format to export data '
    TabOrder = 6
    object ComboBox2: TComboBox
      Left = 9
      Top = 24
      Width = 256
      Height = 21
      Style = csDropDownList
      ItemIndex = 0
      TabOrder = 0
      Text = 'Awp team'#39's image file format (preffered)'
      Items.Strings = (
        'Awp team'#39's image file format (preffered)'
        'JPEG image file format ')
    end
  end
  object GroupBox3: TGroupBox
    Left = 16
    Top = 284
    Width = 281
    Height = 51
    Caption = 'View angle to export '
    TabOrder = 7
    object ComboBox4: TComboBox
      Left = 8
      Top = 16
      Width = 264
      Height = 21
      Style = csDropDownList
      ItemIndex = 0
      TabOrder = 0
      Text = 'Front'
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
  end
  object GroupBox4: TGroupBox
    Left = 16
    Top = 334
    Width = 281
    Height = 67
    Caption = 'Image processing options'
    TabOrder = 8
    object Label3: TLabel
      Left = 77
      Top = 44
      Width = 52
      Height = 13
      Caption = 'Base width'
    end
    object CheckBox1: TCheckBox
      Left = 8
      Top = 17
      Width = 161
      Height = 17
      Caption = 'Flip the image horizontally'
      TabOrder = 0
    end
    object Edit3: TEdit
      Left = 135
      Top = 39
      Width = 130
      Height = 21
      TabOrder = 1
      Text = '128'
    end
    object CheckBox7: TCheckBox
      Left = 8
      Top = 43
      Width = 57
      Height = 17
      Caption = 'Resize'
      TabOrder = 2
    end
  end
  object GroupBox5: TGroupBox
    Left = 16
    Top = 400
    Width = 281
    Height = 113
    Caption = 'Additional options'
    TabOrder = 9
    object Label4: TLabel
      Left = 168
      Top = 26
      Width = 11
      Height = 13
      Caption = '%'
    end
    object CheckBox5: TCheckBox
      Left = 8
      Top = 45
      Width = 97
      Height = 17
      Caption = 'Copy iEye files '
      Enabled = False
      TabOrder = 0
    end
    object Edit4: TEdit
      Left = 117
      Top = 18
      Width = 45
      Height = 21
      TabOrder = 1
      Text = '50'
    end
    object CheckBox2: TCheckBox
      Left = 8
      Top = 91
      Width = 273
      Height = 17
      Caption = 'Open the exported images in Windows Explorer'
      TabOrder = 2
    end
    object CheckBox6: TCheckBox
      Left = 8
      Top = 22
      Width = 97
      Height = 17
      Caption = 'Copy random'
      TabOrder = 3
    end
    object CheckBox4: TCheckBox
      Left = 8
      Top = 67
      Width = 250
      Height = 17
      Caption = 'Copy negative samples '
      TabOrder = 4
    end
  end
  object ApplicationEvents1: TApplicationEvents
    OnIdle = ApplicationEvents1Idle
    Left = 248
    Top = 69
  end
end
