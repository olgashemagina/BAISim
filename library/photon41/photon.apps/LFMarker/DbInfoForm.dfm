object DbInfoDialog: TDbInfoDialog
  Left = 912
  Top = 512
  BorderStyle = bsDialog
  Caption = 'Current database information'
  ClientHeight = 355
  ClientWidth = 335
  Color = clBtnFace
  ParentFont = True
  OldCreateOrder = True
  Position = poScreenCenter
  PixelsPerInch = 96
  TextHeight = 13
  object Bevel1: TBevel
    Left = 8
    Top = 8
    Width = 319
    Height = 308
    Shape = bsFrame
  end
  object Label1: TLabel
    Left = 16
    Top = 16
    Width = 63
    Height = 13
    Caption = 'Database: '
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label2: TLabel
    Left = 19
    Top = 43
    Width = 103
    Height = 13
    Caption = 'Number of Images'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object labelTotalImages: TLabel
    Left = 262
    Top = 43
    Width = 30
    Height = 13
    Caption = '*****'
  end
  object Label5: TLabel
    Left = 20
    Top = 62
    Width = 139
    Height = 13
    Caption = 'Number of Markup Files:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label6: TLabel
    Left = 20
    Top = 103
    Width = 153
    Height = 13
    Caption = 'Number of Marked Objects:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object labelTotalXmlFiles: TLabel
    Left = 262
    Top = 62
    Width = 30
    Height = 13
    Caption = '*****'
  end
  object labelTotalXmlItems: TLabel
    Left = 262
    Top = 103
    Width = 30
    Height = 13
    Caption = '*****'
  end
  object Bevel2: TBevel
    Left = 19
    Top = 126
    Width = 296
    Height = 9
    Shape = bsTopLine
  end
  object Label3: TLabel
    Left = 20
    Top = 82
    Width = 178
    Height = 13
    Caption = 'Number of classes to recognize:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object labelClasses: TLabel
    Left = 262
    Top = 84
    Width = 30
    Height = 13
    Caption = '*****'
  end
  object Label7: TLabel
    Left = 24
    Top = 134
    Width = 129
    Height = 13
    Caption = 'Number of objects by class'
  end
  object labelDatabase: TLabel
    Left = 80
    Top = 16
    Width = 68
    Height = 13
    Caption = 'labelDatabase'
  end
  object OKBtn: TButton
    Left = 123
    Top = 322
    Width = 75
    Height = 25
    Caption = 'OK'
    Default = True
    ModalResult = 1
    TabOrder = 0
  end
  object ValueListEditor1: TValueListEditor
    Left = 21
    Top = 153
    Width = 294
    Height = 152
    DrawingStyle = gdsGradient
    Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goColSizing, goRowSelect, goThumbTracking]
    TabOrder = 1
    TitleCaptions.Strings = (
      'class name'
      'count marked items')
    ColWidths = (
      109
      179)
  end
end
