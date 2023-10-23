object EngineViewForm: TEngineViewForm
  Left = 0
  Top = 0
  Caption = 'EngineView'
  ClientHeight = 399
  ClientWidth = 591
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  OnClose = FormClose
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Splitter1: TSplitter
    Left = 185
    Top = 0
    Height = 399
    ExplicitLeft = 200
    ExplicitTop = 248
    ExplicitHeight = 100
  end
  object Panel1: TPanel
    Left = 188
    Top = 0
    Width = 403
    Height = 399
    Align = alClient
    TabOrder = 0
    OnResize = Panel1Resize
    object PaintBox1: TPaintBox
      AlignWithMargins = True
      Left = 4
      Top = 4
      Width = 395
      Height = 329
      Align = alClient
      Color = clBlack
      ParentColor = False
      OnMouseDown = PaintBox1MouseDown
      OnMouseMove = PaintBox1MouseMove
      OnMouseUp = PaintBox1MouseUp
      OnPaint = PaintBox1Paint
      ExplicitLeft = 6
      ExplicitTop = 1
    end
    object Panel2: TPanel
      Left = 1
      Top = 336
      Width = 401
      Height = 62
      Align = alBottom
      BevelOuter = bvNone
      TabOrder = 0
      object Label1: TLabel
        Left = 8
        Top = 8
        Width = 27
        Height = 13
        Caption = 'Name'
      end
      object Label2: TLabel
        Left = 8
        Top = 24
        Width = 56
        Height = 13
        Caption = 'Num stages'
      end
      object Label3: TLabel
        Left = 8
        Top = 40
        Width = 24
        Height = 13
        Caption = 'Type'
      end
      object Label4: TLabel
        Left = 80
        Top = 8
        Width = 31
        Height = 13
        Caption = 'Label4'
      end
      object Label5: TLabel
        Left = 80
        Top = 24
        Width = 31
        Height = 13
        Caption = 'Label5'
      end
      object Label6: TLabel
        Left = 80
        Top = 40
        Width = 31
        Height = 13
        Caption = 'Label6'
      end
    end
  end
  object Panel3: TPanel
    Left = 0
    Top = 0
    Width = 185
    Height = 399
    Align = alLeft
    Caption = 'Panel3'
    TabOrder = 1
    object CheckListBox1: TCheckListBox
      Left = 1
      Top = 177
      Width = 183
      Height = 221
      OnClickCheck = CheckListBox1Click
      Align = alClient
      ItemHeight = 13
      TabOrder = 0
      OnClick = CheckListBox1Click
    end
    object CheckListBox2: TCheckListBox
      Left = 1
      Top = 113
      Width = 183
      Height = 64
      OnClickCheck = CheckListBox2ClickCheck
      Align = alTop
      ItemHeight = 13
      Items.Strings = (
        'layer 0'
        'layer 1'
        'layer 2'
        'layer 3')
      TabOrder = 1
      OnClick = CheckListBox2Click
    end
    object RadioGroup1: TRadioGroup
      Left = 1
      Top = 41
      Width = 183
      Height = 72
      Align = alTop
      Caption = 'View'
      ItemIndex = 0
      Items.Strings = (
        '2D View'
        '3D View')
      TabOrder = 2
      OnClick = RadioGroup1Click
    end
    object GroupBox1: TGroupBox
      Left = 1
      Top = 1
      Width = 183
      Height = 40
      Align = alTop
      TabOrder = 3
      object CheckBox1: TCheckBox
        Left = 11
        Top = 12
        Width = 97
        Height = 17
        Caption = 'Draw 3D cube'
        TabOrder = 0
        OnClick = CheckBox1Click
      end
    end
  end
  object PopupMenu1: TPopupMenu
    Left = 488
    Top = 72
    object LoadEngine1: TMenuItem
      Caption = 'Load Engine...'
      OnClick = LoadEngine1Click
    end
    object N1: TMenuItem
      Caption = '-'
    end
    object Exit1: TMenuItem
      Caption = 'Exit'
      OnClick = Exit1Click
    end
  end
  object OpenDialog1: TOpenDialog
    Filter = 'Xml files|*.xml'
    Left = 488
    Top = 128
  end
end
