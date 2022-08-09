
//creare funzione display 29/10/21 funziona
//bello/brutto 31/10/21
//Verifica buona 27/2/22
//Modifiche posizione dispay 22/3/22 da definire posizione °
//6/11/21 prove per verificare variazioni
//8/4/2022  fyrame alternati .Per uso differenti font
//Vedi Meteo_ST7789_Frame_Font
//15/6/22 fissato TX come attivo per display
//10/4/22 Aggiornamento per min-max da 
//"C:\Arduino\Sketchbook\A_Prove _Meteo\Serra\Maffucci_DHT22_Min_Max_ora_Motore\Maffucci_DHT22_Min_Max_ora_Motore.ino"
/************************************************************************
 * Tratto da https://simple-circuit.com/nodemcu-esp8266-bme280-sensor-st7789-tft-weather-station/
 * ESP8266 NodeMCU Interface with ST7789 TFT display (240x240 pixel) and
 *   BME280 barometric pressure & temperature & humidity sensor.
 * This is a free software with NO WARRANTY.
 * https://simple-circuit.com/
 *
 ************************************************************************/
 
#include <Wire.h>              // Wire library (required for I2C devices)
#include <Adafruit_GFX.h>      // Adafruit core graphics library
#include <Adafruit_ST7789.h>   // Adafruit hardware-specific library for ST7789
#include <Adafruit_BME280.h>   // Adafruit BME280 sensor library
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <EEPROM.h>
#include <Fonts/FreeMonoBoldOblique12pt7b.h>
#include <Fonts/FreeSerif9pt7b.h>
#include <Fonts/FreeSansOblique9pt7b.h> ////le librerie su AdafruitGFX
#include "C:\Users\Renzo\Documents\Arduino\libraries\Adafruit_GFX_Library\Fonts\FreeMonoBold12pt7b.h"
#include <Fonts/FreeMonoBoldOblique9pt7b.h>
#include <Fonts/FreeMono12pt7b.h> ////le librerie su AdafruitGFX
#include <Fonts/FreeMono9pt7b.h  >
#include <ESP8266WiFiMulti.h>

ESP8266WiFiMulti wifiMulti;

// WiFi connect timeout per AP. Increase when connecting takes longer.
const uint32_t connectTimeoutMs = 5000;
const char *ssid     = "AndroidAP8835";     //  WiFi name
const char *password = "caraguol7";  
//const char *ssid     = "TIM-33131861";     //  WiFi name
//const char *password = "caraguol7";    // WiFi password

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "in.pool.ntp.org", 3600, 60000);
// NTPClient(UDP& udp, const char* poolServerName, long timeOffset, unsigned long updateInterval);
char dayWeek [7][12] = {"Dom","Lun", "Mar", "Mer", "Gio", "Ven", "Sab" };

// ST7789 TFT module connections
#define TFT_DC    0    // TFT DC  pin is connected to NodeMCU pin D3 (GPIO0)
#define TFT_RST   2   // TFT RST pin is connected to NodeMCU pin D4 (GPIO2)
#define TFT_CS    15     // TFT CS  pin is connected to NodeMCU pin D8 (GPIO15)
// initialize ST7789 TFT library with hardware SPI module
 #define SCK (CLK) ---> NodeMCU pin D5 (GPIO14)// SCL
#define MOSI(DIN) ---> NodeMCU pin D7 (GPIO13)//SDA
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
 static unsigned long t1=360000;
 double dt;
// define device I2C address: 0x76 or 0x77 (0x77 is library default address)
#define BME280_I2C_ADDRESS  0x76
// initialize Adafruit BME280 library
Adafruit_BME280  bme280;
float pressmb,pres,temp;// presD e tempD per dispay
int addr = 0;
int humi;
int volt;
float Min = 50.1;
float Max = 0.1;
String now,Omin,Omax;
float ho, minu;
int i,Index;
float Media=0;
float med;
int Display=3; // RX
unsigned int raw=0;  //per calcolo voltaggio batteria
unsigned long ti;
void setup(void) {
  Serial.begin(115200);
  Serial.println("");  
  Serial.println("OK");  
  EEPROM.begin(512);
  WiFi.mode(WIFI_STA);
  
  wifiMulti.addAP("TIM-33131861", "caraguol7");
  wifiMulti.addAP("Home&Life SuperWiFi-47E1", "XC4JYFFXX3QXMJF3");
  wifiMulti.addAP("AndroidAP8835", "caraguol7");
  wifiMulti.addAP("TIM-26619725", "azW4fb3WXmyF2US9kxnkJI6Q");
  // More is possible
  WiFi.begin(ssid, password);
  pinMode(Display,OUTPUT);
  digitalWrite(Display,HIGH);
  /*while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }*/
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  timeClient.setTimeOffset(7200);//Ora legale
 
  // initialize the ST7789 display (240x240 pixel)
  // if the display has CS pin try with SPI_MODE0
  tft.init(240, 240, SPI_MODE2);
  tft.setFont(&FreeSerif9pt7b);
  // if the screen is flipped, remove this command
  tft.setRotation(2);
  // fill the screen with black color
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextWrap(false);         // turn off text wrap option
  tft.setFont(&FreeMono12pt7b);
  tft.setFont(&FreeSansOblique9pt7b);
  tft.setFont(&FreeMonoBoldOblique12pt7b);
  tft.setFont(&FreeMonoBoldOblique9pt7b);
  //<Fonts/FreeMono12pt7b.h>
  // initialize the BME280 sensor
  Wire.begin(4, 5);  // set I2C pins [SDA = D2, SCL = D1], default clock is 100kHz
  if( bme280.begin(BME280_I2C_ADDRESS) == 0 )
  {  // connection error or device address wrong!
    tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);   // set text color to white and black background
    tft.setTextSize(4);      // text size = 4
    tft.setCursor(3, 88);    // move cursor to position (3, 88) pixel
    tft.print("Connection");
    tft.setCursor(63, 126);  // move cursor to position (63, 126) pixel
    tft.print("Error");
    while(1) delay(1000);  // stay here
    
  }
  //Per lettura volt *********
 pinMode(A0, INPUT);
  raw = analogRead(A0);
  volt=raw/1023;
  volt=volt*4.2;
  Serial.print("Volt Rigo 137: ");
  Serial.println(volt);
  //***********
 }
 
// main loop
void loop() {
  t1=millis();
   // Maintain WiFi connection
   
  if (wifiMulti.run(connectTimeoutMs) == WL_CONNECTED) {
    Serial.print("WiFi connected: ");
    Serial.print(WiFi.SSID());
    Serial.print(" ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("WiFi not connected!");
  }
  /* 
  index 0     ;ora Min 4    ;Min 8
  ora Max 12  ;Max  16      ;pres  20*/
  Serial.print("Default: Riga 148: ");
  Serial.print(Min);
  
  Min = EEPROM.get(0,Min);
  Max = EEPROM.get(16,Max);
  Serial.print("Lettura EEPROM ");
  Serial.print(Min);
  Serial.print(" MAX ");
  Serial.println(Max);
   
  if (Min > 100) {
    Min = Min - 256;
  }
   if(Max>100){
    Max=Max-255;
    }
  
  
  delay(100);
  
  timeClient.update();
  temp = bme280.readTemperature();  // get temperature in °C
  Serial.print("TEMPERATURA ATTUALE; ");
  Serial.println(temp);
  humi = bme280.readHumidity();     // get humidity in %
  pres = bme280.readPressure()+10.12;
    char x[8]; //conversione pressione
      dtostrf((pres/100-pressmb),5,1,x);
  Serial.print("Press Ridotta; ");
  Serial.println(x);
  pres=(pres/100)+10.12,1;
 
   tft.fillScreen(ST77XX_BLACK);
  display0(&temp,&humi,&pres,&volt);
  display1();
  Data();
  Min_Max ();
  int e=now.length()-3;
    now=(now.substring(0, e));
    tft.setCursor(35, 210);
    tft.println(now); 
    //Serial.print("Risultato ");
   // Serial.println(now);   
  delay(20000);
  
   Data();
    //Serial.print("Risultato ");
    //Serial.println(now); 
  digitalWrite(Display,LOW);
  tft.setCursor(300, 150);
  tft.print("Batt: ");
  tft.println(volt);
  Serial.print("Volt: ");
  Serial.println(volt);
  //delay(10000);
  Serial.print("DURATA: ");
  Serial.print((millis()-t1)/1000);
  Serial.println(" Sec");
  ESP.deepSleep(1800e6);
  }

 void display0(float*t,int*h,float*p,int*volt){ 
  tft.setFont(&FreeMonoBoldOblique12pt7b);
  tft.setTextSize(3);  
  //tft.setTextSize(8);
  //tft.drawRoundRect(10, 10, 30, 50, 2, 150);  
  tft.setTextColor(ST77XX_RED, ST77XX_BLACK);  // set text color to yellow and black background
  tft.setCursor(20, 110);
  if(t < 0)    // if temperature < 0
    tft.print(*t,1);
    //tft.printf( "-%01u.%1u", (int)abs(*t), (int)(abs(*t) * 100) % 100 );
  else            // temperature >= 0
    //tft.printf( " %01u.%1u", (int)temp, (int)(temp * 100) % 100 );
  tft.println(*t,1);
  tft.drawCircle(220, 70, 8, ST77XX_RED); // x,y,diam, colore
  // 2: print humidity
  tft.setFont(&FreeMonoBoldOblique9pt7b);
  tft.setTextColor(ST77XX_CYAN, ST77XX_BLACK);  // set text color to cyan and black background
  
  tft.setCursor(5, 30);
  tft.setTextSize(2);
  for(int i=0;i<5;i++){
  tft.drawRoundRect(i,i , 105, 35, 5, 150);  
  }
   tft.printf( "%02u %%", (int)*h,(int)(*h*100)% 100 );
   tft.setFont(&FreeMonoBoldOblique9pt7b);
  tft.setTextColor(ST77XX_YELLOW, ST77XX_BLACK);  // set text color to yellow and black background
  for(int i=0;i<5;i++){
  tft.drawRoundRect(i+110,i , 125, 35, 5, 150);  
  }
  tft.setCursor(115, 30);
  tft.setTextSize(1); 
    
  tft.printf( "%04u.%01d h ", (int)(*p), (int)((uint32_t)pres % 100) );
 
  }

 void display1(){ 
  //****Modifiche 23/7/22 da rivedere*****
 
  float alt;
  float i=1/5.255;
  pres = bme280.readPressure()/100;
  Serial.print("PRESSIONE ASSOLUTA: ");
  Serial.println(pres);
  float p0=1013.25;
   alt=44330*(1-(pow((pres/p0),(1/5.255))));
  
  //***********************
  tft.setCursor(5, 150);
  tft.setTextSize(1); 
  tft.print(pres,1);
  tft.print(" : ");
  tft.setTextSize(2); 
  tft.print(alt,1);
   
    Min=EEPROM.get(0,Min);
    tft.setTextSize(1); 
    tft.setCursor(35, 190);
    tft.drawRoundRect(0,190 ,10,2,1, 0);
    tft.println(Min,1);
    Max=EEPROM.get(16,Max);
    tft.setCursor(140, 190);
    tft.println(Max,1);
    now=EEPROM.get(20,now);
    tft.setCursor(140, 210);
    tft.println(now);
    now=EEPROM.get(10,Min);
    tft.setCursor(15, 210);
    tft.println(now); 
 
  
 
 
  }
void Data(){
  now=timeClient.getFormattedTime();
  
  tft.setFont(&FreeMonoBoldOblique12pt7b);
  tft.setTextColor(ST77XX_YELLOW, ST77XX_BLACK);  // set text color to green and black background
      tft.setTextSize(1);        // text size = 3
      tft.drawRoundRect(0,160 ,35,2,1, 150); 
      tft.drawRoundRect(90,160 ,30,2,1, 150);
      tft.drawLine(0, 160, 0, 212, 150);
      tft.drawLine(0, 212, 115, 212, 150);
      tft.drawLine(115, 212, 115, 160, 150);
       
      //tft.drawLine(0, 160, 40, 160, 150);
      
      tft.setCursor(45, 170);
      tft.print("Min ");
     // tft.setCursor(35, 190);//su 246
     // tft.drawRoundRect(0,190 ,10,2,1, 0);
      //tft.println(Min,1);
      tft.drawRoundRect(120,160 ,30,2,1, 150); 
      tft.drawRoundRect(200,160 ,20,2,1, 150);
      tft.drawLine(120, 160, 120, 213, 150);
      tft.drawLine(120, 213, 230, 213, 150);
      tft.drawLine(230, 213, 230, 160, 150);
      tft.setCursor(155, 170);
      tft.print("Max ");
      tft.setCursor(158, 190);
      tft.println("");
     // tft.setCursor(158, 190); //Su 252
      //tft.println(Max,1);
      // tft.setCursor(130, 210);
     // tft.println(now); 
      
   
      tft.setCursor(30, 230);      // move cursor to position (17, 31) pixel
      tft.print(dayWeek[timeClient.getDay()]);
      tft.print("  ");
      tft.print(timeClient.getFormattedTime());}

 int Min_Max () {
  //EEPROM.put(16,0);
  
  //Max = EEPROM.get(16,Max);
 // EEPROM.put(1,50);
 // EEPROM.commit();
 Serial.print("Dati su Min_Max: ");
  Serial.print(Min);
  Serial.print(" MAX ");
  Serial.println(Max);
 
  
  Serial.print("Lettura Temp su MinMax: ");
  Serial.println(temp);
  Serial.print("Min,Max,temp; ");
  Serial.print(Min);
  Serial.print(": ");
  Serial.print(Max);
  Serial.print("; ");
  Serial.println(temp);
  
 if (temp <= Min) {
    Min = temp;
    delay (50);
    Serial.print("Minima attuale:");
    Serial.println(Min);
    EEPROM.put(0,Min);
    int e=now.length()-2;
    now=(now.substring(0, e));
    tft.setCursor(15, 210);
    tft.println(now);
    EEPROM.put(10,now); 
    EEPROM.commit();
    
    Serial.print("Ora Minima  ");
    Serial.println(now);   
  }
      if (temp >=Max) {
      Max = temp;
      Serial.print("Massima Scattata:");
      EEPROM.put(16, Max);
      int e=now.length()-3;
      now=(now.substring(0, e));
     // tft.setCursor(130, 210); Su 252 e seg
     // tft.println(now);
      EEPROM.put(20, now);
      //Serial.print(",");
      //Serial.print(now);
      EEPROM.commit();//righe aggiunte 29/4/22
     // tft.setCursor(130, 210);
      //tft.println(now); 
     
    }
  ho = timeClient.getHours(); //riprendo ora e minuti per salvataggio dati
  minu = timeClient.getMinutes(); // a mezzanotte
  minu = minu / 60; //Converto ora/minuti in decimale
  ho = ho + minu;
if (ho > 0.0 && (ho < 0.45)) {
    Max = temp;
    Min = temp;
    Serial.println(Max);
    EEPROM.put(0, Min);
    EEPROM.put(15, Max);
    EEPROM.commit();
  }
   i++;
  Media=Media +temp;
  med=Media/i;
  /*Serial.print("La media è: ");
  Serial.print(Media);
  Serial.print(" La media Calcolata: ");
  Serial.println(med);
  Serial.print(" i: ");
  Serial.println(i);*/
  return Min;
  return Max;
  return med;
}
