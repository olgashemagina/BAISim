object TableForm: TTableForm
  Left = 1198
  Top = 186
  Caption = 'Image Description'
  ClientHeight = 300
  ClientWidth = 429
  Color = clBtnFace
  DragKind = dkDock
  DragMode = dmAutomatic
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 429
    Height = 41
    Align = alTop
    TabOrder = 0
    object sbDeleteRow: TSpeedButton
      Left = 6
      Top = 3
      Width = 53
      Height = 34
      Caption = 'Delete row'
      Flat = True
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -9
      Font.Name = 'Small Fonts'
      Font.Style = []
      Glyph.Data = {
        76010000424D7601000000000000760000002800000020000000100000000100
        04000000000000010000130B0000130B00001000000000000000000000000000
        800000800000008080008000000080008000808000007F7F7F00BFBFBF000000
        FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333333333
        3333333333333333FF3333333333333003333333333333377F33333333333307
        733333FFF333337773333C003333307733333777FF333777FFFFC0CC03330770
        000077777FF377777777C033C03077FFFFF077FF77F777FFFFF7CC00000F7777
        777077777777777777773CCCCC00000000003777777777777777333330030FFF
        FFF03333F77F7F3FF3F7333C0C030F00F0F03337777F7F77373733C03C030FFF
        FFF03377F77F7F3F333733C03C030F0FFFF03377F7737F733FF733C000330FFF
        0000337777F37F3F7777333CCC330F0F0FF0333777337F737F37333333330FFF
        0F03333333337FFF7F7333333333000000333333333377777733}
      Layout = blGlyphTop
      Margin = 3
      NumGlyphs = 2
      ParentFont = False
      Spacing = 0
      OnClick = sbDeleteRowClick
    end
    object sbClear: TSpeedButton
      Left = 59
      Top = 3
      Width = 53
      Height = 34
      Caption = 'Clear'
      Flat = True
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = 12
      Font.Name = 'Small Fonts'
      Font.Style = []
      Glyph.Data = {
        76010000424D7601000000000000760000002800000020000000100000000100
        04000000000000010000120B0000120B00001000000000000000000000000000
        800000800000008080008000000080008000808000007F7F7F00BFBFBF000000
        FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333000000000
        3333333777777777F3333330F777777033333337F3F3F3F7F3333330F0808070
        33333337F7F7F7F7F3333330F080707033333337F7F7F7F7F3333330F0808070
        33333337F7F7F7F7F3333330F080707033333337F7F7F7F7F3333330F0808070
        333333F7F7F7F7F7F3F33030F080707030333737F7F7F7F7F7333300F0808070
        03333377F7F7F7F773333330F080707033333337F7F7F7F7F333333070707070
        33333337F7F7F7F7FF3333000000000003333377777777777F33330F88877777
        0333337FFFFFFFFF7F3333000000000003333377777777777333333330777033
        3333333337FFF7F3333333333000003333333333377777333333}
      Layout = blGlyphTop
      Margin = 3
      NumGlyphs = 2
      ParentFont = False
      Spacing = 0
      OnClick = sbClearClick
    end
  end
  object ListView1: TListView
    Left = 0
    Top = 41
    Width = 429
    Height = 259
    Align = alClient
    Columns = <>
    GridLines = True
    ReadOnly = True
    RowSelect = True
    TabOrder = 1
    ViewStyle = vsReport
    OnChange = ListView1Change
    OnDblClick = ListView1DblClick
    OnSelectItem = ListView1SelectItem
  end
  object ActionList1: TActionList
    Left = 192
    Top = 8
    object FragmentViewAction: TAction
      GroupIndex = 1
      OnExecute = FragmentViewActionExecute
      OnUpdate = FragmentViewActionUpdate
    end
  end
end
