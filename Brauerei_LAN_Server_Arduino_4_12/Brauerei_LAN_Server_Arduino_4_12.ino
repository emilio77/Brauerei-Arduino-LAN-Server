// Version 4.12
// Pinbelegung:
// A0 : NTC Sernsor
// A1 : Rührwerk Relais
// A2 : Pumpe Relais
// A3 : Alarm Relais
// A4 : I²C (SDA)
// A5 : I²C (SCL)
// D0 : USB - Rx
// D1 : USB - Tx
// D2 : DS18B20 Sensor
// D3 : Heizung Relais
// D4 : Ethernet SS SD Control 
// D5 : Pumpenschalter
// D6 : Alarmschalter
// D7 : Automatikschalter
// D8 : Heizungsschalter
// D9 : Ruehrwerksschalter
// D10 : Ethernet SS Ethernet Control
// D11 : Ethernet MOSI
// D12 : Ethernet MISO
// D13 : Ethernet SCK

// Arduino lauscht auf Port 5000 nach UDP Strings
// Beschreibung des seriellen Strings:
// Serieller String ist immer 19stellig
// Beispiel: C$%"dp/1----------c
// 1.    Zeichen: "C" Startzeichen
// 2.    Zeichen: Relaisstatus
// 3.    Zeichen: Programmstatus und Sensortyp
// 4.    Zeichen: Solltemperatur
// 5.+6. Zeichen: Isttemperatur
// 7.+8. Zeichen: Status der Zusatzfunktionen
// 9.-18.Zeichen: Frei für weitere Funktionen
// 19.   Zeichen: "c" Stopzeichen

// Durch Setzen der Variable "ExterneSteuerung" kann die Brauerei auf dem Rechner gestartet, gestoppt oder pausiert werden
// t= keine Funktion
// s= Start
// p= Pause
// e= Stop

#include <LiquidCrystal_I2C.h>  // Kommt von http://arduino-info.wikispaces.com/LCD-Blue-I2C
#include <math.h>               // Kommt von Arduino IDE
#include <OneWire.h>            // Kommt von http://www.pjrc.com/teensy/td_libs_OneWire.html
#include <SPI.h>                // needed for Arduino versions later than 0018
#include <Ethernet.h>           // Kommt von Arduino IDE
#include <EthernetUdp.h>        // UDP library from: bjoern@cs.stanford.edu 12/30/2008
#include <avr/wdt.h>            // Kommt von Arduino IDE
#include <Wire.h>               // Kommt von Arduino IDE

// ******* Netzwerkeinstellungen, bitte anpassen! *******
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };             // Arduino MAC Adresse
IPAddress IP(192, 168, 178, 222);                                // Arduino IP Adresse
IPAddress answerIP(192, 168, 178, 255);                          // IP Adresse an die geantwortet wird
EthernetServer server(80);                                       // Ethernet Server Port
EthernetUDP Udp;

unsigned int localPort = 5000;               // UDP Port auf dem gelauscht wird
unsigned int answerPort = 5001;              // UDP Port für die Antwort

OneWire ds(2);                                    // an pin D2

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address

char  ReplyBuffer[] = "";                    // a string to send back

const boolean aus = LOW;                           // Hier kann bei Low-Aktiven Relaiskarten einfach High und Low vertauscht werden
const boolean an = HIGH;                           // Hier kann bei Low-Aktiven Relaiskarten einfach High und Low vertauscht werden

const int Heizung = 3;                             // Im folgenden sind die Pins der Sensoren und Aktoren festgelegt
const int Ruehrwerk = 15;
const int Pumpe = 16;
const int Summer = 17;

const int Heizschalter = 8;
const int Ruehrschalter = 9;
const int Pumpschalter = 5;
const int Alarmschalter = 6;
const int Autoschalter = 7;


byte heat[8] = {  // Heizungssymbol erstellen
  B01001,
  B10010,
  B01001,
  B10010,
  B00000,
  B11111,
  B01110,
  B00000
};

byte ruehr[8] = {  // Rührwerkssymbol erstellen
  B11111,
  B00100,
  B00100,
  B00100,
  B10101,
  B11111,
  B10101,
  B00000
};

byte alarm[8] = {  // Alarmsymbol erstellen
  B00000,
  B00100,
  B01110,
  B01110,
  B01110,
  B11111,
  B00100,
  B00000
};

byte pumpe[8] = {  // Pumpensymbol erstellen
  B11111,
  B01110,
  B10001,
  B10001,
  B10001,
  B01110,
  B11111,
  B00000
};

unsigned long jetztMillis = 0;
unsigned long letzteUDPMillis = 0;
unsigned long letzteMeldungMillis = 0;
char ExterneSteuerung = 't';
int deltaMeldungMillis = 5000; 
int solltemp = 0;
float Temp = 0.0;
float externeisttemp = 0.0;
char charVal[5];
char packetBuffer[24];   // buffer to hold incoming packet,
char temprec[24] = "";
char relais[5] = "";
char sensor= 'D';
char state[3] = "";
boolean Funktionslog[10];
float temperaturalt, temperaturneu;

int EinbringzeitMalz =0;
boolean Funktion2gestartet = false;
    
void Funktion1()           // Individuelle Funktion
{
//  ExterneSteuerung = 's'; // Beispiel für ExterneSteuerung
//  if ( Temp > 31 ) { ExterneSteuerung = 'p'; } // Beispiel für ExterneSteuerung
//  if ( Temp > 33 ) { ExterneSteuerung = 'e'; } // Beispiel für ExterneSteuerung
}

void Funktion2()           // Individuelle Funktion
{

}

void Funktion3()           // Individuelle Funktion
{

}

void Funktion4()           // Individuelle Funktion
{

}

void Funktion5()           // Individuelle Funktion
{

}

void Funktion6()           // Individuelle Funktion
{

}

void Funktion7()           // Individuelle Funktion
{

}

void Funktion8()           // Individuelle Funktion
{

}

void Funktion9()           // Individuelle Funktion
{

}

void Funktion10()           // Individuelle Funktion
{

}

void noFunktion()
{
//  ExterneSteuerung == 't'; // Beispiel für ExterneSteuerung
}
 
void setup(void)
{
  pinMode(Heizung, OUTPUT);       // Im folgenden werden die Pins als I/O definiert
  pinMode(Summer, OUTPUT);
  pinMode(Ruehrwerk, OUTPUT);
  pinMode(Pumpe, OUTPUT);
  pinMode(Heizschalter, INPUT_PULLUP);
  pinMode(Pumpschalter, INPUT_PULLUP);
  pinMode(Ruehrschalter, INPUT_PULLUP);
  pinMode(Alarmschalter, INPUT_PULLUP);
  pinMode(Autoschalter, INPUT_PULLUP);
  
  wdt_enable(WDTO_8S);
   
  Wire.begin();
 
  Ethernet.begin(mac, IP);
  server.begin();
  Udp.begin(localPort);

  for (int schleife = 0; schleife < 23; schleife++) { temprec[schleife] = ' '; }

  lcd.begin(20,4);         // initialize the lcd for 20 chars 4 lines and turn on backlight
  lcd.backlight();

  lcd.createChar(1, heat);         // Aktivierung der eigenen Zeichen
  lcd.createChar(2, ruehr);
  lcd.createChar(3, alarm);
  lcd.createChar(4, pumpe);
  
  lcd.setCursor(0, 0);                // Startbildschirm
  lcd.print("      Brauerei      ");
  lcd.setCursor(0, 1);          
  lcd.print(" Arduino LAN Server ");
  lcd.setCursor(0, 2);
  lcd.print("       V4.12        ");
  lcd.setCursor(0, 3);
  lcd.print("     by emilio      ");
  delay(3000);
  lcd.clear();
  lcd.setCursor(0, 0);              
  lcd.write("I:      ");
  lcd.write(223);
  lcd.print("C  S:    ");
  lcd.write(223);
  lcd.print("C");
}
   
void Hauptseite()
{
  char dummy[8];
  String Antwort = "";
  int isolltemp = solltemp; 
  EthernetClient client = server.available();        // WEB-Server für Brauerei
  if (client) {
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n' && currentLineIsBlank) {
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");         // the connection will be closed after completion of the response
          client.println("Refresh: 5");                // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.print("<html>");
          client.print("<head>");
          client.print("<title>");
          client.print("Brauerei-Arduino-LAN-Server via HTML");
          client.println("</title>");
          client.println("</head>");
          client.print("<body>");
          client.println("<u><h1>Brauerei-Arduino-LAN-Server</h1></u>");
          client.print("<table cellpadding=10 border=3 frame=void>");
          client.print("<tr>");
          client.print("<td><h2>Brauereistatus:</h2></td>");
          if (state[2]=='T') {client.print("<td><h2>AUS</h2></td>");}
          else if (state[1]=='o') {client.print("<td><h2>OFFLINE</h2></td>");}        
          else if (state[1]=='x') {client.print("<td><h2>INAKTIV</h2></td>");}
          else if (state[1]=='y') {client.print("<td><h2>AKTIV</h2></td>");}
          else if (state[1]=='z') {client.print("<td><h2>PAUSIERT</h2></td>");}
          else if (digitalRead(Autoschalter)==HIGH) {client.print("<td><h2>MANUELL</h2></td>");}
          client.println("</tr>");
          client.print("<tr>");
          client.print("<td><h2>Ist-Temperatur:</h2></td>");
          client.print("<td><h2>Soll-Temperatur:</h2></td>");
          client.println("</tr>");
          client.print("<tr>");
          client.print("<td>"); if (sensor == 'd') {client.print(externeisttemp, 1);} else {client.print(Temp, 1);} client.print(" "); client.print("&#186;"); client.print("C</td>");
          client.print("<td>"); if (isolltemp == 0) {client.print("--.-");} else {client.print(solltemp,1);} client.print(" "); client.print("&#186;"); client.print("C</td>");
          client.println("</tr>");
          client.print("<tr>");
          client.print("<td>&#160;</td>");
          client.print("<td>&#160;</td>");
          client.println("</tr>");
          client.print("<tr>");
          client.print("<td><h2>Relaisstellungen:</h2></td>");
          client.print("<td>&#160;</td>");
          client.println("</tr>");
          client.print("<tr>");
          client.print("<td>Heizungsrelais</td>");
          if (digitalRead(Heizung) == an) {
            client.print("<td><b><font color=#00B200>AN</font></b></td>");
          }
          if (digitalRead(Heizung) == aus) {
            client.print("<td><b><font color=#FF0000>AUS</font></b></td>");
          }
          client.println("</tr>");
          client.print("<tr>");
          client.print("<td>R"); client.print("&#252;"); client.print("hrwerksrelais</td>");
          if (digitalRead(Ruehrwerk) == an) {
            client.print("<td><b><font color=#00B200>AN</font></b></td>");
          }
          if (digitalRead(Ruehrwerk) == aus) {
            client.print("<td><b><font color=#FF0000>AUS</font></b></td>");
          }
          client.println("</tr>");
          client.print("<tr>");
          client.print("<td>Pumpenrelais</td>");
          if (digitalRead(Pumpe) == an) {
            client.print("<td><b><font color=#00B200>AN</font></b></td>");
          }
          if (digitalRead(Pumpe) == aus) {
            client.print("<td><b><font color=#FF0000>AUS</font></b></td>");
          }
          client.println("</tr>");
          client.print("<tr>");
          client.print("<td>Alarmrelais</td>");
          if (digitalRead(Summer) == an) {
            client.print("<td><b><font color=#00B200>AN</font></b></td>");
          }
          if (digitalRead(Summer) == aus) {
            client.print("<td><b><font color=#FF0000>AUS</font></b></td>");
          }
          client.println("</tr>");
          client.print("<tr>");
          client.print("<td>&#160;</td>");
          client.print("<td>&#160;</td>");
          client.println("</tr>");
          client.print("<tr>");
          client.print("<td><h2>Sensortyp:</h2></td>");
          if (sensor == 'd') {
            client.print("<td><h2>Display</h2></td>");
          }
          if (sensor == 'D') {
            client.print("<td><h2>DS18B20</h2></td>");
          }
          if (sensor == 'N') {
            client.print("<td><h2>NTC10k</h2></td>");
          }
          client.println("</tr>");          

          client.print("<tr>");
          client.print("<td>&#160;</td>");
          client.print("<td>&#160;</td>");
          client.println("</tr>");
  

          for (int i=1; i<=10; i++) {
            client.print("<tr>");
            client.print("<td>Zusatzfunktion ");
            client.print(i);
            client.print("</td>");
            if (Funktionslog[i] == true) {
              client.print("<td><b><font color=#00B200>AN</font></b></td>");
            }
            else {
              client.print("<td><b><font color=#FF0000>AUS</font></b></td>");
            }
            client.println("</tr>");
          }         

          client.println("</table>");
          client.println("</body>");
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }
    }
    client.stop();
  }  
}

void UDPRead()
{
  int packetSize = Udp.parsePacket();
  if (packetSize)
  {
    for (int schleife = 0; schleife < 23; schleife++) { temprec[schleife] = ' '; }
    // read the packet into packetBufffer
    Udp.read(packetBuffer, packetSize);
    for (int schleife = 0; schleife < 23; schleife++) { temprec[schleife] = packetBuffer[schleife]; }
    letzteUDPMillis = millis();
    packetAuswertung();
  }
}    

void UDPOut()
{
  dtostrf(Temp, 4, 1, charVal);
  Udp.beginPacket(answerIP, answerPort);
  Udp.write('T');
  for (int i = 0; i < sizeof(charVal)-1; i++) { Udp.write(charVal[i]); }
  if (ExterneSteuerung != 'p' && ExterneSteuerung != 's' && ExterneSteuerung != 'e') { ExterneSteuerung = 't'; }
  Udp.write(ExterneSteuerung);
  Udp.println(); 
  Udp.endPacket();
}

void OfflineCheck()
{
  if (jetztMillis > letzteUDPMillis+10000) 
  {
    relais[1]='h';      
    relais[2]='r';               
    relais[3]='p'; 
    relais[4]='a';       
    state[1]='o';
    solltemp=0;
    for (int i=1; i<=10; i++) {Funktionslog[i]=false;}
  }
}

void ManuellerStatus()
{
  if (digitalRead(Autoschalter)==HIGH) 
  {
    solltemp=0;
    state[1]='m';
    if (digitalRead(Heizschalter)==HIGH) {relais[1] = 'h';} else {relais[1] = 'H';}
    if (digitalRead(Ruehrschalter)==HIGH) {relais[2] = 'r';} else {relais[2] = 'R';}
    if (digitalRead(Pumpschalter)==HIGH) {relais[3] = 'p';} else {relais[3] = 'P';}
    if (digitalRead(Alarmschalter)==HIGH) {relais[4] = 'a';} else {relais[4] = 'A';}
  } 
}

void LCDOut()
{
  lcd.setCursor(3, 0);
  if (sensor=='d') {dtostrf(externeisttemp, 4, 1, charVal);} else {dtostrf(Temp, 4, 1, charVal);}  
  lcd.print(charVal);      

  lcd.setCursor(15, 0);         
  if (solltemp >= 0 && solltemp < 10) { lcd.print(" "); }; 
  if (solltemp > 0 && solltemp < 100) { lcd.print(solltemp); }  
  else { lcd.print("-"); }
  
  lcd.setCursor(13, 1);              
  if (relais[1]=='H') { lcd.write(1); lcd.print(" "); } else { lcd.print("  "); } 
  lcd.setCursor(15, 1);     
  if (relais[2]=='R') { lcd.write(2); lcd.print(" "); } else { lcd.print("  "); } 
  lcd.setCursor(17, 1);              
  if (relais[3]=='P') { lcd.write(4); lcd.print(" "); } else { lcd.print("  "); } 
  lcd.setCursor(19, 1);     
  if (relais[4]=='A') { lcd.write(3); } else { lcd.print(" "); } 

  lcd.setCursor(0, 1);
  if (state[1]=='o') { lcd.print("OFFLINE "); }        
  else if (state[1]=='x') { lcd.print("INAKTIV "); }
  else if (state[1]=='y') { lcd.print("AKTIV   "); }
  else if (state[1]=='z') { lcd.print("PAUSIERT"); }
  else if (state[1]=='m') { lcd.print("MANUELL "); }

  lcd.setCursor(0, 2);
  if (state[1]=='o') { lcd.print("OFF seit: "); }
  else { lcd.print("Up seit: "); }

  long hours=0;
  long mins=0;
  long secs=0;
  if (state[1]=='o') { secs=jetztMillis-letzteUDPMillis-10000; }
  else { secs=jetztMillis; }; 
  secs = secs/1000; //convect milliseconds to seconds
  mins=secs/60; //convert seconds to minutes
  hours=mins/60; //convert minutes to hours
  secs=secs-(mins*60); //subtract the coverted seconds to minutes in order to display 59 secs max
  mins=mins-(hours*60); //subtract the coverted minutes to hours in order to display 59 minutes max

  lcd.setCursor(12, 2);
  if (hours < 10) { lcd.print("0"); }; 
  lcd.print(hours);
  lcd.print(":");
  if (mins < 10) { lcd.print("0"); }; 
  lcd.print(mins);
  lcd.print(":");
  if (secs < 10) { lcd.print("0"); }; 
  lcd.print(secs);
  
  lcd.setCursor(0, 3);
  lcd.print("Funktion: ");
  for (int i=1; i<=10; i++) { if (Funktionslog[i]==false) {lcd.print("0");} else {lcd.print("1");} }
    
}

void RelaisOut()
{
  if (relais[1] == 'H') { digitalWrite(Heizung,an); } else { digitalWrite(Heizung,aus); }
  if (relais[2] == 'R') { digitalWrite(Ruehrwerk,an); } else { digitalWrite(Ruehrwerk,aus); }
  if (relais[3] == 'P') { digitalWrite(Pumpe,an); } else { digitalWrite(Pumpe,aus); }
  if (relais[4] == 'A') { digitalWrite(Summer,an); } else { digitalWrite(Summer,aus); }
}

void packetAuswertung()
{
  int temp = 0;
  int temp2 = 0;
  if ((temprec[0]=='C') && (temprec[18]=='c'))             // Begin der Decodierung des seriellen Strings  
  { 
    temp=(int)temprec[1];
    if ( temp < 0 ) { temp = 256 + temp; }
    if ( temp > 7) {relais[4]='A';temp=temp-8;} else {relais[4]='a';} 
    if ( temp > 3) {relais[3]='P';temp=temp-4;} else {relais[3]='p';} 
    if ( temp > 1) {relais[2]='R';temp=temp-2;} else {relais[2]='r';}
    if ( temp > 0) {relais[1]='H';temp=temp-1;} else {relais[1]='h';}   

    temp=(int)temprec[2];
    if ( temp < 0 ) { temp = 256 + temp; }
    if ( temp > 127) {sensor='N';temp=temp-128;}  
    if ( temp > 63) {sensor='D';temp=temp-64;}
    if ( temp > 31) {sensor='d';temp=temp-32;}    
    if ( temp > 15) {state[2]='t';temp=temp-16;}  
    if ( temp > 7) {state[2]='T';temp=temp-8;}  
    if ( temp > 3) {state[1]='x';temp=temp-4;} 
    else if ( temp > 1) {state[1]='z';temp=temp-2;}  
    else if ( temp > 0) {state[1]='y';temp=temp-1;}    
  
    temp=(int)temprec[3];
    if ( temp < 0 ) { temp = 256 + temp; }
    solltemp=temp;

    temp=(int)temprec[5];
    if ( temp < 0 ) { temp = 256 + temp; }
    temp2=temp;
    temp=(int)temprec[4];
    if ( temp < 0 ) { temp = 256 + temp; }
    temp=temp*256;
    temp2=temp2+temp;
    externeisttemp=temp2;
    externeisttemp=externeisttemp/10;

    temp=(int)temprec[6];
    if ( temp < 0 ) { temp = 256 + temp; }  
    temp2=(int)temprec[7];
    if ( temp2 < 0 ) { temp2 = 256 + temp2; }
    for (int i=1; i<=10; i++) {Funktionslog[i]=false;}
    if ( (temp == 0) && (temp2 == 0) ) {noFunktion();} 
    if ( temp > 127) {Funktion1();Funktionslog[1]=true;temp=temp-128;} 
    if ( temp > 63) {Funktion2();Funktionslog[2]=true;temp=temp-64;} 
    if ( temp > 31) {Funktion3();Funktionslog[3]=true;temp=temp-32;} 
    if ( temp > 15) {Funktion4();Funktionslog[4]=true;temp=temp-16;}    
    if ( temp > 7) {Funktion5();Funktionslog[5]=true;temp=temp-8;}  
    if ( temp > 3) {Funktion6();Funktionslog[6]=true;temp=temp-4;} 
    if ( temp > 1) {Funktion7();Funktionslog[7]=true;temp=temp-2;}  
    if ( temp > 0) {Funktion8();Funktionslog[8]=true;temp=temp-1;}  
    if ( temp2 > 1) {Funktion9();Funktionslog[9]=true;temp2=temp2-2;}    
    if ( temp2 > 0) {Funktion10();Funktionslog[10]=true;temp2=temp2-1;}  

  }
}

float temperaturmessungDS18B20()            // Glättungsroutine für DS18B20-Messung
{
  temperaturneu = DS18B20lesen();
  if (temperaturalt - temperaturneu < 2 and temperaturneu - temperaturalt < 2)
  {
    Temp = temperaturneu * 2 + temperaturalt * 8;
    Temp = Temp / 10;
  }
  temperaturalt = temperaturneu;
  Temp;
}

float DS18B20lesen()
{
  int HighByte, LowByte, TReading, SignBit, Tc_100, Whole, Fract;
  byte i;
  byte present = 0;
  byte data[12];
  byte addr[8];
  if ( !ds.search(addr))  { ds.search(addr); } // Wenn keine weitere Adresse vorhanden, von vorne anfangen
  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        // start Konvertierung, mit power-on am Ende
  delay(750);               // 750ms sollten ausreichen
  present = ds.reset();
  ds.select(addr);
  ds.write(0xBE);           // Wert lesen
  for ( i = 0; i < 9; i++) { data[i] = ds.read(); }
  LowByte = data[0];
  HighByte = data[1];
  TReading = (HighByte << 8) + LowByte;
  SignBit = TReading & 0x8000;  // test most sig bit
  if (SignBit)                  // negative
  {
    TReading = (TReading ^ 0xffff) + 1; // 2's comp
  }
  Temp = TReading * 0.0625;       // Für DS18S20  temperatur = TReading*0.5
  if (SignBit) // negative Werte ermitteln
  {
    Temp = Temp * -1;
  }
  return Temp;
}

void loop(void)
{
  jetztMillis = millis();

  UDPRead();
  OfflineCheck();
  ManuellerStatus();
  LCDOut();
  RelaisOut();
   
  if(jetztMillis - letzteMeldungMillis > deltaMeldungMillis)
  {
    letzteMeldungMillis = jetztMillis;
    temperaturmessungDS18B20();
    UDPOut();    
  }
  
  Hauptseite();
  
  if (jetztMillis < 100000000) {wdt_reset();}             // WatchDog Reset  
}
