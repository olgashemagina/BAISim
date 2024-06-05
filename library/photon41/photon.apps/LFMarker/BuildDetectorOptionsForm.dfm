object CSBuildOptions: TCSBuildOptions
  Left = 0
  Top = 0
  Caption = 'Build Detector options'
  ClientHeight = 291
  ClientWidth = 413
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Panel2: TPanel
    Left = 0
    Top = 263
    Width = 417
    Height = 33
    TabOrder = 0
    object CancelButton: TButton
      Left = 24
      Top = 0
      Width = 89
      Height = 25
      Caption = 'Cancel'
      TabOrder = 0
      OnClick = CancelButtonClick
    end
    object NextButton: TButton
      Left = 290
      Top = 0
      Width = 92
      Height = 25
      Caption = 'Next'
      TabOrder = 1
      OnClick = NextButtonClick
    end
    object BackButton: TButton
      Left = 192
      Top = 0
      Width = 92
      Height = 25
      Caption = 'Back'
      TabOrder = 2
      OnClick = BackButtonClick
    end
  end
  object PageControl1: TPageControl
    Left = 0
    Top = 0
    Width = 417
    Height = 257
    ActivePage = DB_page
    TabOrder = 1
    OnChange = PageControl1Change
    object DB_page: TTabSheet
      Caption = 'Database'
      object Panel1: TPanel
        Left = -4
        Top = -24
        Width = 505
        Height = 282
        TabOrder = 0
        object Label1: TLabel
          Left = 24
          Top = 48
          Width = 239
          Height = 13
          Caption = #1050#1072#1090#1072#1083#1086#1075' '#1080#1079#1086#1073#1088#1072#1078#1077#1085#1080#1081', '#1085#1077' '#1089#1086#1076#1077#1088#1078#1072#1097#1080#1093' '#1086#1073#1098#1077#1082#1090
        end
        object Label2: TLabel
          Left = 24
          Top = 158
          Width = 138
          Height = 13
          Caption = #1050#1072#1090#1072#1083#1086#1075' '#1086#1073#1088#1072#1079#1094#1086#1074' '#1086#1073#1098#1077#1082#1090#1072
        end
        object Label3: TLabel
          Left = 24
          Top = 102
          Width = 303
          Height = 13
          Caption = #1050#1072#1090#1072#1083#1086#1075' '#1076#1083#1103' '#1089#1086#1093#1088#1072#1085#1077#1085#1080#1103' '#1086#1073#1088#1072#1079#1094#1086#1074', '#1085#1077' '#1089#1086#1076#1077#1088#1078#1072#1097#1080#1081' '#1086#1073#1098#1077#1082#1090
        end
        object bgr: TEdit
          Left = 24
          Top = 67
          Width = 303
          Height = 21
          TabOrder = 0
          Text = 'select folder'
        end
        object neg: TEdit
          Left = 24
          Top = 121
          Width = 303
          Height = 21
          TabOrder = 1
          Text = 'select folder'
        end
        object pos: TEdit
          Left = 24
          Top = 177
          Width = 303
          Height = 21
          TabOrder = 2
          Text = 'select folder'
        end
        object Sel_bgr: TButton
          Left = 343
          Top = 65
          Width = 27
          Height = 25
          Caption = '...'
          TabOrder = 3
          OnClick = Sel_bgrClick
        end
        object Sel_neg: TButton
          Left = 343
          Top = 119
          Width = 27
          Height = 25
          Caption = '...'
          TabOrder = 4
          OnClick = negClick
        end
        object Sel_pos: TButton
          Left = 343
          Top = 175
          Width = 27
          Height = 25
          Caption = '...'
          TabOrder = 5
          OnClick = DatabaseClick
        end
      end
    end
    object Scanner_page: TTabSheet
      Caption = 'Scanner_page'
      ImageIndex = 1
      object Label4: TLabel
        Left = 20
        Top = 24
        Width = 56
        Height = 13
        Caption = 'Base height'
      end
      object Label5: TLabel
        Left = 163
        Top = 24
        Width = 52
        Height = 13
        Caption = 'Base width'
      end
      object Label6: TLabel
        Left = 20
        Top = 64
        Width = 208
        Height = 13
        Caption = 'Shift as a percentage of baseline resolution'
      end
      object Label7: TLabel
        Left = 20
        Top = 112
        Width = 49
        Height = 13
        Caption = 'Scale step'
      end
      object Label8: TLabel
        Left = 19
        Top = 153
        Width = 126
        Height = 13
        Caption = 'Min size of scanned object'
      end
      object Label9: TLabel
        Left = 227
        Top = 153
        Width = 61
        Height = 13
        Caption = 'x Base width'
      end
      object Label10: TLabel
        Left = 19
        Top = 192
        Width = 130
        Height = 13
        Caption = 'Max size of scanned object'
      end
      object Label11: TLabel
        Left = 227
        Top = 192
        Width = 61
        Height = 13
        Caption = 'x Base width'
      end
      object H_CSpinEdit: TCSpinEdit
        Left = 96
        Top = 21
        Width = 49
        Height = 22
        MaxValue = 1024
        MinValue = 8
        TabOrder = 0
        Value = 24
      end
      object CSpinEdit2: TCSpinEdit
        Left = 248
        Top = 21
        Width = 49
        Height = 22
        MaxValue = 1024
        MinValue = 8
        TabOrder = 1
        Value = 24
      end
      object CSpinEdit1: TCSpinEdit
        Left = 248
        Top = 61
        Width = 49
        Height = 22
        MaxValue = 100
        TabOrder = 2
        Value = 10
      end
      object CSpinEdit4: TCSpinEdit
        Left = 167
        Top = 150
        Width = 54
        Height = 22
        MaxValue = 4
        MinValue = 1
        TabOrder = 3
        Value = 1
      end
      object CSpinEdit5: TCSpinEdit
        Left = 167
        Top = 189
        Width = 54
        Height = 22
        MaxValue = 100
        MinValue = 1
        TabOrder = 4
        Value = 50
      end
      object ComboBox1: TComboBox
        Left = 85
        Top = 109
        Width = 136
        Height = 21
        ItemIndex = 0
        TabOrder = 5
        Text = '1.1'
        Items.Strings = (
          '1.1'
          '1.2'
          '1.3'
          '1.4'
          '1.5'
          '1.6'
          '1.7'
          '1.8'
          '1.9'
          '2')
      end
    end
    object Learning_page: TTabSheet
      Caption = 'Learning'
      ImageIndex = 2
      object Label12: TLabel
        Left = 20
        Top = 24
        Width = 141
        Height = 13
        Caption = 'Number of samples per image'
      end
      object Label13: TLabel
        Left = 20
        Top = 88
        Width = 172
        Height = 13
        Hint = 'if less, the learning process will be stopped'
        Caption = 'Minimal number of negative samples'
      end
      object Label14: TLabel
        Left = 20
        Top = 56
        Width = 227
        Height = 13
        Caption = 'Number of negative samples necessary to start'
      end
      object Label15: TLabel
        Left = 20
        Top = 107
        Width = 213
        Height = 13
        Hint = 'if less, the learning process will be stopped'
        Caption = '(if less, the learning process will be stopped)'
      end
      object Label16: TLabel
        Left = 20
        Top = 144
        Width = 166
        Height = 13
        Caption = 'Maximal number of learning stages'
      end
      object Label17: TLabel
        Left = 20
        Top = 176
        Width = 138
        Height = 13
        Caption = 'FAR value to finish stage, %'
      end
      object CSpinEdit3: TCSpinEdit
        Left = 262
        Top = 15
        Width = 65
        Height = 22
        MaxValue = 10000
        MinValue = 100
        TabOrder = 0
        Value = 2000
      end
      object CSpinEdit6: TCSpinEdit
        Left = 262
        Top = 53
        Width = 65
        Height = 22
        MaxValue = 10000
        MinValue = 100
        TabOrder = 1
        Value = 2500
      end
      object CSpinEdit7: TCSpinEdit
        Left = 262
        Top = 98
        Width = 65
        Height = 22
        MaxValue = 1000
        MinValue = 50
        TabOrder = 2
        Value = 200
      end
      object CSpinEdit8: TCSpinEdit
        Left = 262
        Top = 141
        Width = 65
        Height = 22
        MaxValue = 3000
        MinValue = 500
        TabOrder = 3
        Value = 1000
      end
      object CSpinEdit9: TCSpinEdit
        Left = 262
        Top = 173
        Width = 65
        Height = 22
        MaxValue = 100
        MinValue = 1
        TabOrder = 4
        Value = 10
      end
      object SpinEdit1: TSpinEdit
        Left = 472
        Top = 312
        Width = 121
        Height = 22
        MaxValue = 0
        MinValue = 0
        TabOrder = 5
        Value = 0
      end
    end
    object Features_page: TTabSheet
      Caption = 'Features'
      ImageIndex = 3
      object RadioGroup1: TRadioGroup
        Left = 20
        Top = 16
        Width = 269
        Height = 185
        Caption = 'Select feature'
        Columns = 2
        ItemIndex = 5
        Items.Strings = (
          'TLFHFeature'
          'TLFDAFeature'
          'TLFHAFeature'
          'TLFCAFeature'
          'TLFVAFeature'
          'CSFeature'
          'TLFVFeature'
          'TLFCFeature'
          'YLFLBPFeature')
        TabOrder = 0
      end
    end
    object Detector_page: TTabSheet
      Caption = 'Detector'
      ImageIndex = 4
      object Label18: TLabel
        Left = 48
        Top = 51
        Width = 71
        Height = 13
        Caption = 'Detector name'
      end
      object Label19: TLabel
        Left = 48
        Top = 99
        Width = 46
        Height = 13
        Caption = 'Log name'
      end
      object Edit1: TEdit
        Left = 144
        Top = 48
        Width = 121
        Height = 21
        TabOrder = 0
        Text = '*.xml'
      end
      object Edit2: TEdit
        Left = 144
        Top = 96
        Width = 121
        Height = 21
        TabOrder = 1
        Text = '*.txt'
      end
    end
  end
  object SaveDialog1: TSaveDialog
    DefaultExt = 'xml'
    Filter = 'xml|*.xml'
    Left = 368
    Top = 216
  end
end
