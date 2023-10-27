object LongProcDlg: TLongProcDlg
  Left = 0
  Top = 0
  BorderStyle = bsNone
  Caption = 'LongProcDlg'
  ClientHeight = 83
  ClientWidth = 504
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -13
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  OnShow = FormShow
  PixelsPerInch = 120
  TextHeight = 16
  object Label1: TLabel
    Left = 8
    Top = 59
    Width = 37
    Height = 16
    Caption = 'Label1'
  end
  object Label2: TLabel
    Left = 8
    Top = 8
    Width = 4
    Height = 18
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -15
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object ProgressBar1: TProgressBar
    Left = 8
    Top = 30
    Width = 470
    Height = 21
    TabOrder = 0
  end
  object Timer1: TTimer
    Enabled = False
    OnTimer = Timer1Timer
    Left = 232
    Top = 8
  end
end
