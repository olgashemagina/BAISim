object MedianFilterDlg: TMedianFilterDlg
  Left = 227
  Top = 108
  BorderStyle = bsDialog
  Caption = 'Median filter'
  ClientHeight = 73
  ClientWidth = 242
  Color = clBtnFace
  ParentFont = True
  OldCreateOrder = True
  Position = poMainFormCenter
  PixelsPerInch = 96
  TextHeight = 13
  object Bevel2: TBevel
    Left = 8
    Top = 10
    Width = 145
    Height = 53
    Shape = bsFrame
  end
  object Label1: TLabel
    Left = 16
    Top = 26
    Width = 32
    Height = 13
    Caption = 'Radius'
  end
  object OKBtn: TButton
    Left = 159
    Top = 8
    Width = 75
    Height = 25
    Caption = 'OK'
    Default = True
    ModalResult = 1
    TabOrder = 0
  end
  object CancelBtn: TButton
    Left = 159
    Top = 38
    Width = 75
    Height = 25
    Cancel = True
    Caption = 'Cancel'
    ModalResult = 2
    TabOrder = 1
  end
  object CSpinEdit1: TCSpinEdit
    Left = 56
    Top = 24
    Width = 57
    Height = 22
    MaxValue = 5
    MinValue = 1
    TabOrder = 2
    Value = 1
  end
end
