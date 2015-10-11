unit Form;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  StdCtrls, ExtCtrls,
  synaser, IdUDPServer, IdBaseComponent, IdComponent, IdUDPBase,
  IdUDPClient, IdSocketHandle;

type
  TMainForm = class(TForm)
    Memo1: TMemo;
    Tmr_Rcv: TTimer;
    Label1: TLabel;
    Label2: TLabel;
    ComboBox2: TComboBox;
    Button1: TButton;
    Edit1: TEdit;
    Label3: TLabel;
    Edit2: TEdit;
    Label4: TLabel;
    Timer1: TTimer;
    CheckBox1: TCheckBox;
    IdUDPClient1: TIdUDPClient;
    IdUDPServer1: TIdUDPServer;
    Label5: TLabel;
    Edit3: TEdit;
    ComboBox1: TComboBox;
    ComboBox3: TComboBox;
    Label6: TLabel;
    procedure Tmr_RcvTimer(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure Memo1Change(Sender: TObject);
    procedure Button1Click(Sender: TObject);
    procedure Timer1Timer(Sender: TObject);
    procedure CheckBox1Click(Sender: TObject);
    procedure IdUDPServer1UDPRead(Sender: TObject; AData: TStream;
      ABinding: TIdSocketHandle);
    procedure Edit3Change(Sender: TObject);
    procedure ComboBox1Change(Sender: TObject);
    procedure ComboBox3Change(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  MainForm: TMainForm;
  ser: TBlockSerial;
  myFile,mySettings: Text;
  XonOff : boolean ; { manual XonOff handling ... I want to see it }
  RecText, x, logfilename, displayfilename: string;
  floattempalt,floattemp,tempdelta: extended;
  timestamp: Array[1..3] of TDateTime;
  logfilecounter: integer;

implementation

{$R *.DFM}


function GetFileModifyDate(FileName: string): TDateTime;
var
  h: THandle;
  Struct: TOFSTRUCT;
  lastwrite: Integer;
  t: TDateTime;
begin
  h := OpenFile(PChar(FileName), Struct, OF_SHARE_DENY_NONE);
  try
    if h <> HFILE_ERROR then
    begin
      lastwrite := FileGetDate(h);
      Result    := FileDateToDateTime(lastwrite);
    end;
  finally
    CloseHandle(h);
  end;
end;


procedure TMainForm.Tmr_RcvTimer(Sender: TObject);
var solltemp, isttemp, line2, temp: string;
    itemp,iisttemp,isolltemp,i:integer;
begin
  Tmr_Rcv.Enabled := false ;
  DecimalSeparator := '.';
  timestamp[1]:= GetFileModifyDate(displayfilename);
  if FileExists(displayfilename) then
  begin
    try
      AssignFile(myFile, displayfilename);
      Reset(myFile);
      ReadLn(myFile, isttemp); iisttemp:=round(strtofloat(isttemp)*10);
      ReadLn(myFile, solltemp); isolltemp:=round(strtofloat(solltemp));
      ReadLn(myFile, temp); if temp='1' then itemp:=1 else itemp:=0;
      ReadLn(myFile, temp); if temp='1' then itemp:=itemp+2;
      ReadLn(myFile, temp); if temp='1' then itemp:=itemp+4;
      ReadLn(myFile, temp); if temp='1' then itemp:=itemp+8;
      line2:='C'+char(itemp);
      ReadLn(myFile, temp); if temp='aktiv' then itemp:=1 else if temp='pausiert' then itemp:=2 else itemp:=4;
      if timestamp[1]=timestamp[3] then itemp:=itemp+8 else itemp:=itemp+16;
      if ComboBox2.Text='Display' then itemp:=itemp+32 else if ComboBox2.Text='DS18B20' then itemp:=itemp+64 else itemp:=itemp+128;
      if iisttemp>255 then repeat begin iisttemp:=iisttemp-256; i:=i+1; end until iisttemp<256;
      line2:=line2+char(itemp)+char(isolltemp)+char(i)+char(iisttemp);
      ReadLn(myFile, temp); if temp<>'0' then itemp:=128 else itemp:=0;
      ReadLn(myFile, temp); if temp<>'0' then itemp:=itemp+64;
      ReadLn(myFile, temp); if temp<>'0' then itemp:=itemp+32;
      ReadLn(myFile, temp); if temp<>'0' then itemp:=itemp+16;
      ReadLn(myFile, temp); if temp<>'0' then itemp:=itemp+8;
      ReadLn(myFile, temp); if temp<>'0' then itemp:=itemp+4;
      ReadLn(myFile, temp); if temp<>'0' then itemp:=itemp+2;
      ReadLn(myFile, temp); if temp<>'0' then itemp:=itemp+1;
      line2:=line2+char(itemp);
      ReadLn(myFile, temp); if temp<>'0' then itemp:=2 else itemp:=0;
      ReadLn(myFile, temp); if temp<>'0' then itemp:=itemp+1;
      line2:=line2+char(itemp)+('----------c');
      CloseFile(myFile);
      IdUDPClient1.Active := true;
      IdUDPClient1.Send(line2);
      IdUDPClient1.Active := false;
    except
      Tmr_Rcv.Enabled := true ;
      exit;
    end;
  end
  else
  begin
    if Memo1.Lines.Strings[Memo1.Lines.Count-2]<>'Displaydatei existiert nicht' then
    begin
      Memo1.Lines.Append('');
      Memo1.Lines.Strings[Memo1.Lines.Count-2]:='Displaydatei existiert nicht';
    end;
  end;
  Tmr_Rcv.Enabled := true ;
end;

procedure TMainForm.FormCreate(Sender: TObject);
var i: integer;
begin
  for i:=1 to 3 do timestamp[i]:=0;
  if FileExists('settings.txt') then
  begin
    AssignFile(mySettings, 'settings.txt');
    Reset(mySettings);
    ReadLn(mySettings, x); ComboBox1.Text:=x;
    ReadLn(mySettings, x); ComboBox2.Text:=x;
    ReadLn(mySettings, x); ComboBox3.Text:=x;
    ReadLn(mySettings, x); Edit3.Text:=x;
    ReadLn(mySettings, logfilename); Edit1.Text:=logfilename;
    ReadLn(mySettings, displayfilename); Edit2.Text:=displayfilename;
    CloseFile(mySettings);
    MainForm.ComboBox1Change(Sender);
    MainForm.ComboBox3Change(Sender);
    MainForm.Edit3Change(Sender);
  end
  else
  begin
    logfilename:=Edit1.Text;
    displayfilename:=Edit2.Text;
  end;
  if DeleteFile(logfilename) then
  begin
    AssignFile(myFile, logfilename);
    ReWrite(myFile);
    Writeln(myFile, '01-01-2000 00:00:00;21.0');
    CloseFile(myFile);
  end;
  Label2.Caption:=DateTimeToStr(Now);
  ser:=TBlockserial.Create;
  ser.RaiseExcept:=False;
end;

procedure TMainForm.Memo1Change(Sender: TObject);
begin
  Label1.Caption:=DateTimeToStr(Now);
end;

procedure TMainForm.Button1Click(Sender: TObject);
begin
  try
  AssignFile(mySettings, 'settings.txt');
  ReWrite(mySettings);
  WriteLn(mySettings, ComboBox1.Text);
  WriteLn(mySettings, ComboBox2.Text);
  WriteLn(mySettings, ComboBox3.Text);
  WriteLn(mySettings, Edit3.Text);
  WriteLn(mySettings, Edit1.Text);
  WriteLn(mySettings, Edit2.Text);
  CloseFile(mySettings);
  logfilename:=Edit1.Text;
  displayfilename:=Edit2.Text;
  except
  end;
end;

procedure TMainForm.Timer1Timer(Sender: TObject);
begin
  timestamp[3]:=timestamp[2];
  timestamp[2]:=timestamp[1];
end;

procedure TMainForm.CheckBox1Click(Sender: TObject);
begin
  if CheckBox1.Checked=true then Timer1.Enabled:=true else Timer1.Enabled:=false;
  timestamp[2]:=0;
  timestamp[3]:=0;
end;

procedure TMainForm.IdUDPServer1UDPRead(Sender: TObject; AData: TStream;
  ABinding: TIdSocketHandle);
var
  sl: TStringList;
  tfs: string;
begin
  SetLength(RecText, AData.Size);
  AData.ReadBuffer(PChar(RecText)^, AData.Size);
  DecimalSeparator := '.';
  if (((ComboBox2.Text='NTC10000') or (ComboBox2.Text='DS18B20')) and (RecText[1]='T') and (RecText[6]='t'))then
  begin
    Memo1.Lines.Append('');
    sl:=TStringList.Create; //Objekt erzeugen
    try
      floattempalt:=floattemp;
      try tfs:= copy(RecText, 2, 4); floattemp:=strtofloat(tfs); except end;
      tempdelta:=floattemp-floattempalt;
      if timestamp[1]=timestamp[3] then
      begin
        Memo1.Lines.Strings[Memo1.Lines.Count-1]:='Logdatei seit >2 min. unverändert';
      end
      else if (floattemp<0) or (floattemp>=100) or (tempdelta<-5) or (tempdelta>5) then
      begin
        Memo1.Lines.Strings[Memo1.Lines.Count-1]:='Temperaturwert unplausibel';
      end
      else
      begin
        try
          AssignFile (MyFile, logfilename);
          if logfilecounter>100 then begin DeleteFile(logfilename); logfilecounter:=0; end;
          if FileExists(logfilename) then
          try Append(MyFile); except Memo1.Lines.Strings[Memo1.Lines.Count-1]:='Datei konnte nicht geschrieben werden'; exit; end
          else
          try ReWrite(MyFile); except Memo1.Lines.Strings[Memo1.Lines.Count-1]:='Datei konnte nicht geschrieben werden'; exit; end;
          tfs:=(DateTimeToStr(Now)+';'+tfs);
          writeln(MyFile, tfs);
          CloseFile(MyFile);
          logfilecounter:=logfilecounter+1;
          Memo1.Lines.Strings[Memo1.Lines.Count-1]:=(tfs); //Text hinzufügen
          except
            Tmr_Rcv.Enabled := true;
            Memo1.Lines.Strings[Memo1.Lines.Count-1]:='Datei konnte nicht geschrieben werden';
          end;
        end;
        finally
          sl.free; //Objekt wieder freigeben
      end;
  end;
end;

procedure TMainForm.Edit3Change(Sender: TObject);
begin
  IdUDPClient1.Active:=false;
  IdUDPClient1.Host:=Edit3.Text;
  IdUDPClient1.Active:=true;
end;

procedure TMainForm.ComboBox1Change(Sender: TObject);
begin
  IdUDPClient1.Active:=False;
  IdUDPClient1.Port:=strtoint(ComboBox1.Text);
  IdUDPClient1.Active:=True;
end;

procedure TMainForm.ComboBox3Change(Sender: TObject);
begin
  IdUDPServer1.Active:=False;
  IdUDPServer1.Bindings.Clear;
  IdUDPServer1.DefaultPort:= strtoint(ComboBox3.Text);         //Electronic device port
  IdUDPServer1.Active:=True;
end;

end.
