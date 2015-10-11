object MainForm: TMainForm
  Left = 488
  Top = 137
  Width = 505
  Height = 295
  Caption = 'Brauerei Arduino LAN Server'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poDefaultPosOnly
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 234
    Top = 232
    Width = 3
    Height = 13
  end
  object Label2: TLabel
    Left = 234
    Top = 216
    Width = 3
    Height = 13
  end
  object Label3: TLabel
    Left = 232
    Top = 120
    Width = 169
    Height = 13
    Caption = 'Speicherort der Temperaturlogdatei:'
  end
  object Label4: TLabel
    Left = 232
    Top = 168
    Width = 191
    Height = 13
    Caption = 'Speicherort der Displaxinformationsdatei:'
  end
  object Label5: TLabel
    Left = 232
    Top = 16
    Width = 50
    Height = 13
    Caption = 'Sensortyp:'
  end
  object Label6: TLabel
    Left = 232
    Top = 72
    Width = 213
    Height = 13
    Caption = 'IP-OUT:                      Port-OUT:         Port-IN'
  end
  object Memo1: TMemo
    Left = 8
    Top = 8
    Width = 209
    Height = 241
    ReadOnly = True
    ScrollBars = ssVertical
    TabOrder = 0
    OnChange = Memo1Change
  end
  object ComboBox2: TComboBox
    Left = 232
    Top = 32
    Width = 105
    Height = 21
    ItemHeight = 13
    TabOrder = 1
    Text = 'NTC10000'
    Items.Strings = (
      'NTC10000'
      'DS18B20'
      'Display')
  end
  object Button1: TButton
    Left = 352
    Top = 8
    Width = 129
    Height = 49
    Caption = 'Settings speichern'
    TabOrder = 2
    OnClick = Button1Click
  end
  object Edit1: TEdit
    Left = 232
    Top = 136
    Width = 241
    Height = 21
    TabOrder = 3
    Text = 'C:\Brauerei\Temperatur\log.txt'
  end
  object Edit2: TEdit
    Left = 232
    Top = 184
    Width = 241
    Height = 21
    TabOrder = 4
    Text = 'C:\Brauerei\Display\display.txt'
  end
  object CheckBox1: TCheckBox
    Left = 360
    Top = 232
    Width = 113
    Height = 17
    Caption = 'Log-'#220'berwachung'
    Checked = True
    State = cbChecked
    TabOrder = 5
    OnClick = CheckBox1Click
  end
  object Edit3: TEdit
    Left = 232
    Top = 88
    Width = 97
    Height = 21
    TabOrder = 6
    Text = '192.168.178.255'
    OnChange = Edit3Change
  end
  object ComboBox1: TComboBox
    Left = 336
    Top = 88
    Width = 65
    Height = 21
    ItemHeight = 13
    TabOrder = 7
    Text = '5000'
    OnChange = ComboBox1Change
    Items.Strings = (
      '5000'
      '5001'
      '5002'
      '5003'
      '5004'
      '5005'
      '5006'
      '5007'
      '5008'
      '5009')
  end
  object ComboBox3: TComboBox
    Left = 408
    Top = 88
    Width = 65
    Height = 21
    ItemHeight = 13
    TabOrder = 8
    Text = '5001'
    OnChange = ComboBox3Change
    Items.Strings = (
      '5000'
      '5001'
      '5002'
      '5003'
      '5004'
      '5005'
      '5006'
      '5007'
      '5008'
      '5009')
  end
  object Tmr_Rcv: TTimer
    OnTimer = Tmr_RcvTimer
    Left = 56
    Top = 16
  end
  object Timer1: TTimer
    Interval = 60000
    OnTimer = Timer1Timer
    Left = 96
    Top = 16
  end
  object IdUDPClient1: TIdUDPClient
    Active = True
    BroadcastEnabled = True
    Host = '192.168.178.255'
    Port = 5001
    Left = 56
    Top = 56
  end
  object IdUDPServer1: TIdUDPServer
    Active = True
    BroadcastEnabled = True
    Bindings = <>
    DefaultPort = 5000
    OnUDPRead = IdUDPServer1UDPRead
    Left = 96
    Top = 56
  end
end