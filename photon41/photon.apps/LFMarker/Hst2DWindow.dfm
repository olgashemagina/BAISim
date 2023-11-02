object Hsv2DForm: THsv2DForm
  Left = 0
  Top = 0
  BorderStyle = bsToolWindow
  Caption = '2D Histogramm'
  ClientHeight = 301
  ClientWidth = 450
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  Position = poScreenCenter
  OnHide = FormHide
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 8
    Top = 199
    Width = 30
    Height = 13
    Caption = #1055#1086#1088#1086#1075
  end
  object FImage1: TFImage
    Left = 152
    Top = 8
    Width = 289
    Height = 278
    Cursor = 1
    BorderStyle = bsFSingle
    ParentColor = False
    PopupMenu = PopupMenu1
  end
  object LoadButton: TButton
    Left = 8
    Top = 40
    Width = 138
    Height = 25
    Action = hsLoadModelAction
    TabOrder = 1
  end
  object SaveButton: TButton
    Left = 8
    Top = 72
    Width = 138
    Height = 25
    Action = hsSaveModelAction
    TabOrder = 2
  end
  object ApplyButton: TButton
    Left = 8
    Top = 103
    Width = 138
    Height = 25
    Action = hsPredictAction
    TabOrder = 3
  end
  object NewButton: TButton
    Left = 8
    Top = 8
    Width = 138
    Height = 25
    Action = hsNewModelAction
    TabOrder = 4
  end
  object SpinEdit1: TSpinEdit
    Left = 8
    Top = 218
    Width = 138
    Height = 22
    MaxValue = 255
    MinValue = 0
    TabOrder = 5
    Value = 0
    OnChange = SpinEdit1Change
  end
  object CheckBox1: TCheckBox
    Left = 8
    Top = 246
    Width = 97
    Height = 17
    Caption = 'Process'
    TabOrder = 6
  end
  object CheckBox2: TCheckBox
    Left = 8
    Top = 269
    Width = 97
    Height = 17
    Caption = 'ViewSource'
    TabOrder = 7
    OnClick = CheckBox2Click
  end
  object Button1: TButton
    Left = 9
    Top = 136
    Width = 137
    Height = 25
    Action = hsBuildModelAction
    TabOrder = 8
  end
  object Button2: TButton
    Left = 8
    Top = 168
    Width = 138
    Height = 25
    Action = hsEstErrorAction
    TabOrder = 9
  end
  object OpenDialog1: TOpenDialog
    Filter = '2d histogramm|*.awp'
    Left = 256
    Top = 40
  end
  object SaveDialog1: TSaveDialog
    Filter = '2d histogramm|*.awp'
    Left = 208
    Top = 40
  end
  object ActionList1: TActionList
    Left = 296
    Top = 40
    object hsNewModelAction: TAction
      Caption = #1053#1086#1074#1072#1103' '#1084#1086#1076#1077#1083#1100
      OnExecute = hsNewModelActionExecute
    end
    object hsSaveModelAction: TAction
      Caption = #1057#1086#1093#1088#1072#1085#1080#1090#1100' '#1084#1086#1076#1077#1083#1100'...'
      OnExecute = hsSaveModelActionExecute
      OnUpdate = hsSaveModelActionUpdate
    end
    object hsLoadModelAction: TAction
      Caption = #1047#1072#1075#1088#1091#1079#1080#1090#1100' '#1084#1086#1076#1077#1083#1100'...'
      OnExecute = hsLoadModelActionExecute
    end
    object hsBuildModelAction: TAction
      Caption = #1055#1086#1089#1090#1088#1086#1080#1090#1100' '#1084#1086#1076#1077#1083#1100
      OnExecute = hsBuildModelActionExecute
      OnUpdate = hsBuildModelActionUpdate
    end
    object hsEstErrorAction: TAction
      Caption = #1054#1094#1077#1085#1080#1090#1100' '#1086#1096#1080#1073#1082#1091
      OnExecute = hsEstErrorActionExecute
      OnUpdate = hsEstErrorActionUpdate
    end
    object hsPredictAction: TAction
      Caption = #1050#1083#1072#1089#1089#1080#1092#1080#1094#1080#1088#1086#1074#1072#1090#1100' '
      OnExecute = hsPredictActionExecute
      OnUpdate = hsPredictActionUpdate
    end
    object hsAddSampleAction: TAction
      Caption = #1044#1086#1073#1072#1074#1080#1090#1100' '#1086#1073#1088#1072#1079#1077#1094
    end
    object viewSourceHSTAction: TAction
      Category = 'view'
      AutoCheck = True
      Caption = #1056#1072#1089#1087#1088#1077#1076#1083#1077#1085#1080#1077' '#1074' '#1079#1072#1076#1072#1085#1085#1086#1081' '#1086#1073#1083#1072#1089#1090#1080
      Checked = True
      GroupIndex = 1
      OnExecute = viewSourceHSTActionExecute
      OnUpdate = viewSourceHSTActionUpdate
    end
    object viewSourceCondHSTAction: TAction
      Category = 'view'
      AutoCheck = True
      Caption = #1056#1072#1089#1087#1088#1077#1076#1077#1083#1077#1085#1080#1077' '#1074' '#1079#1072#1076#1072#1085#1085#1086#1081' '#1086#1073#1083#1072#1089#1090#1080'('#1087#1086#1088#1086#1075') '
      GroupIndex = 1
      OnExecute = viewSourceCondHSTActionExecute
      OnUpdate = viewSourceCondHSTActionUpdate
    end
    object viewOutsideAction: TAction
      Category = 'view'
      AutoCheck = True
      Caption = #1056#1072#1089#1087#1088#1077#1076#1077#1083#1077#1085#1080#1077' '#1074#1085#1077' '#1079#1072#1076#1072#1085#1085#1086#1081' '#1086#1073#1083#1072#1089#1090#1080
      GroupIndex = 1
      OnExecute = viewOutsideActionExecute
      OnUpdate = viewOutsideActionUpdate
    end
    object viewCondOutsideAction: TAction
      Category = 'view'
      AutoCheck = True
      Caption = #1056#1072#1089#1087#1088#1077#1076#1077#1083#1077#1085#1080#1077' '#1074#1085#1077' '#1079#1072#1076#1072#1085#1085#1086#1081' '#1086#1073#1083#1072#1089#1090#1080' ('#1087#1086#1088#1086#1075')'
      GroupIndex = 1
      OnExecute = viewCondOutsideActionExecute
      OnUpdate = viewCondOutsideActionUpdate
    end
    object viewPredictorAction: TAction
      Category = 'view'
      AutoCheck = True
      Caption = #1056#1077#1096#1072#1102#1097#1077#1077' '#1087#1088#1072#1074#1080#1083#1086
      GroupIndex = 1
      OnExecute = viewPredictorActionExecute
      OnUpdate = viewPredictorActionUpdate
    end
  end
  object PopupMenu1: TPopupMenu
    Left = 344
    Top = 40
    object N1: TMenuItem
      Action = viewSourceHSTAction
      AutoCheck = True
    end
    object N2: TMenuItem
      Action = viewSourceCondHSTAction
      AutoCheck = True
    end
    object N3: TMenuItem
      Action = viewOutsideAction
      AutoCheck = True
    end
    object N4: TMenuItem
      Action = viewCondOutsideAction
      AutoCheck = True
    end
    object N5: TMenuItem
      Action = viewPredictorAction
      AutoCheck = True
    end
  end
end
