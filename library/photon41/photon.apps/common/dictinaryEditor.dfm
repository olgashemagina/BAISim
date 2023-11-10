object dictinaryEditDlg: TdictinaryEditDlg
  Left = 227
  Top = 108
  BorderStyle = bsDialog
  Caption = 'Dictionary editor '
  ClientHeight = 202
  ClientWidth = 330
  Color = clBtnFace
  ParentFont = True
  OldCreateOrder = True
  Position = poMainFormCenter
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object OKBtn: TButton
    Left = 199
    Top = 167
    Width = 122
    Height = 25
    Caption = 'Ok'
    Default = True
    ModalResult = 1
    TabOrder = 0
  end
  object AddWordButton: TButton
    Left = 199
    Top = 8
    Width = 122
    Height = 25
    Caption = 'Add Class Label...'
    TabOrder = 1
    OnClick = AddWordButtonClick
  end
  object DeleteWordButton: TButton
    Left = 199
    Top = 70
    Width = 122
    Height = 25
    Caption = 'Delete Class Label...'
    TabOrder = 2
    OnClick = DeleteWordButtonClick
  end
  object EditWordButton: TButton
    Left = 199
    Top = 39
    Width = 122
    Height = 25
    Caption = 'Edit Class Label...'
    TabOrder = 3
    OnClick = EditWordButtonClick
  end
  object ListBox1: TListBox
    AlignWithMargins = True
    Left = 3
    Top = 3
    Width = 185
    Height = 196
    Align = alLeft
    Columns = 2
    ItemHeight = 13
    TabOrder = 4
  end
  object Button1: TButton
    Left = 200
    Top = 136
    Width = 122
    Height = 25
    Cancel = True
    Caption = 'Cancel'
    ModalResult = 2
    TabOrder = 5
  end
end
