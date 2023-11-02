object RotateDlg: TRotateDlg
  Left = 528
  Top = 215
  BorderStyle = bsDialog
  Caption = 'Rotate image'
  ClientHeight = 301
  ClientWidth = 313
  Color = clBtnFace
  ParentFont = True
  OldCreateOrder = True
  Position = poScreenCenter
  OnClose = FormClose
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Bevel1: TBevel
    Left = 8
    Top = 8
    Width = 297
    Height = 249
    Shape = bsFrame
  end
  object Label1: TLabel
    Left = 16
    Top = 16
    Width = 30
    Height = 13
    Caption = 'Angle '
  end
  object Label2: TLabel
    Left = 184
    Top = 16
    Width = 34
    Height = 13
    Caption = 'degree'
  end
  object OKBtn: TButton
    Left = 79
    Top = 268
    Width = 75
    Height = 25
    Caption = 'OK'
    Default = True
    ModalResult = 1
    TabOrder = 0
  end
  object CancelBtn: TButton
    Left = 159
    Top = 268
    Width = 75
    Height = 25
    Cancel = True
    Caption = 'Cancel'
    ModalResult = 2
    TabOrder = 1
  end
  object CSpinEdit1: TCSpinEdit
    Left = 56
    Top = 16
    Width = 121
    Height = 22
    MaxValue = 90
    MinValue = -90
    TabOrder = 2
    OnChange = CSpinEdit1Change
  end
  object PhImage1: TPhImage
    Left = 16
    Top = 48
    Width = 281
    Height = 201
    ThumbWidht = 128
    ThumbHeight = 128
    SlideShowInterval = 500
    ParentColor = False
  end
end
