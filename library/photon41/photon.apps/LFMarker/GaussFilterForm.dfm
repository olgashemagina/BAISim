object GaussFilterDlg: TGaussFilterDlg
  Left = 227
  Top = 108
  BorderStyle = bsDialog
  Caption = 'Gauss blur'
  ClientHeight = 98
  ClientWidth = 244
  Color = clBtnFace
  ParentFont = True
  Position = poMainFormCenter
  OnShow = FormShow
  TextHeight = 15
  object Bevel1: TBevel
    Left = 8
    Top = 10
    Width = 145
    Height = 80
    Shape = bsFrame
  end
  object Label1: TLabel
    Left = 16
    Top = 26
    Width = 35
    Height = 15
    Caption = 'Radius'
  end
  object Label2: TLabel
    Left = 119
    Top = 26
    Width = 10
    Height = 15
    Caption = '%'
  end
  object Label3: TLabel
    Left = 16
    Top = 59
    Width = 47
    Height = 15
    Caption = 'Sigma = '
  end
  object Label4: TLabel
    Left = 64
    Top = 59
    Width = 15
    Height = 15
    Caption = '---'
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
    MaxValue = 100
    MinValue = 1
    TabOrder = 2
    Value = 1
    OnChange = CSpinEdit1Change
  end
end
