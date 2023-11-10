object FilterDlg: TFilterDlg
  Left = 528
  Top = 215
  BorderStyle = bsDialog
  Caption = 'Linear Filtering'
  ClientHeight = 301
  ClientWidth = 426
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
    Width = 410
    Height = 249
    Shape = bsFrame
  end
  object OKBtn: TButton
    Left = 134
    Top = 268
    Width = 75
    Height = 25
    Caption = 'OK'
    Default = True
    ModalResult = 1
    TabOrder = 0
  end
  object CancelBtn: TButton
    Left = 215
    Top = 268
    Width = 75
    Height = 25
    Cancel = True
    Caption = 'Cancel'
    ModalResult = 2
    TabOrder = 1
  end
  object ListBox1: TListBox
    Left = 16
    Top = 16
    Width = 122
    Height = 233
    ItemHeight = 13
    Items.Strings = (
      'Blur'
      'Blur more'
      'Sharpen'
      'Sharpen More '
      'Sharpen Edges '
      'Find Edges '
      'Find Edges 1'
      'Emobss North'
      'Emboss North Ost'
      'Emboss Ost '
      'Emboss South Ost'
      'Emboss South'
      'Emboss South West '
      'Emboss West '
      'Emboss North West '
      'Previtt  Vertical'
      'Previtt Horisontal '
      'Sobel Vertical '
      'Sobel Horizontal')
    TabOrder = 2
    OnClick = ListBox1Click
  end
  object PhImage1: TPhImage
    Left = 144
    Top = 16
    Width = 265
    Height = 233
    ThumbWidht = 128
    ThumbHeight = 128
    SlideShowInterval = 500
    ParentColor = False
  end
end
