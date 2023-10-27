object DbCopyDlg: TDbCopyDlg
  Left = 1049
  Top = 302
  BorderStyle = bsDialog
  Caption = 'Copy database settings '
  ClientHeight = 363
  ClientWidth = 313
  Color = clBtnFace
  ParentFont = True
  OldCreateOrder = True
  Position = poScreenCenter
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Bevel1: TBevel
    Left = 8
    Top = 8
    Width = 297
    Height = 313
    Shape = bsFrame
  end
  object Label1: TLabel
    Left = 16
    Top = 18
    Width = 44
    Height = 13
    Caption = 'To path: '
  end
  object OKBtn: TButton
    Left = 80
    Top = 327
    Width = 75
    Height = 25
    Caption = 'OK'
    Default = True
    ModalResult = 1
    TabOrder = 0
  end
  object CancelBtn: TButton
    Left = 160
    Top = 327
    Width = 75
    Height = 25
    Cancel = True
    Caption = 'Cancel'
    ModalResult = 2
    TabOrder = 1
  end
  object Edit1: TEdit
    Left = 64
    Top = 16
    Width = 233
    Height = 21
    ReadOnly = True
    TabOrder = 2
    Text = 'Edit1'
    OnChange = Edit1Change
  end
  object CheckBox1: TCheckBox
    Left = 16
    Top = 248
    Width = 97
    Height = 17
    Caption = 'Copy images '
    TabOrder = 3
  end
  object CheckBox2: TCheckBox
    Left = 16
    Top = 272
    Width = 145
    Height = 17
    Caption = 'Copy semantic description'
    TabOrder = 4
  end
  object CheckBox5: TCheckBox
    Left = 16
    Top = 295
    Width = 145
    Height = 17
    Caption = 'Open databse after copy'
    TabOrder = 5
  end
  object DirectoryListBox1: TDirectoryListBox
    Left = 16
    Top = 72
    Width = 281
    Height = 170
    TabOrder = 6
    OnChange = DirectoryListBox1Change
  end
  object DriveComboBox1: TDriveComboBox
    Left = 16
    Top = 43
    Width = 281
    Height = 19
    DirList = DirectoryListBox1
    TabOrder = 7
  end
end
