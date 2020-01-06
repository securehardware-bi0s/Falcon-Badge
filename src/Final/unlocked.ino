/**
 * Init-Firmware
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 by Securehardware@bi0s
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */
// Pin Declarations for the modules in the Kit
const int laserPin = 0;
const int dataPin = 13;  
const int loadPin = 15;  
const int clockPin = 14;
const int cntrl_1Pin = 12; // Pin D6
const int cntrl_2Pin = 16; // Pin D0

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
// Libraries Included for modules
#include <Wire.h>
#include "SSD1306Wire.h"
#include <FS.h>

SSD1306Wire display(0x3c, 4, 5, GEOMETRY_128_32);
ESP8266WebServer server(8266);

// Custom Header Files
#include "images.h"

/////////////////////////////////////////////////////////////////////
#ifndef STASSID
#define STASSID "AMRITA-LAN"
#endif
////////////////////////////////////////////////////////////////////
const char* mqttServer = "10.112.7.254";
const int mqttPort = 1883;
///////////////////////////////////////////////////////////////////
IPAddress gateway(10, 112, 0, 1);   //IP Address of your WiFi Router (Gateway)
IPAddress subnet(255, 255, 248, 0);  //Subnet mask
////////////////////////////////////////////////////////////////////
WiFiClient espClient;
PubSubClient client(espClient);
////////////////////////////////////////////////////////////////////
#define DEMO_DURATION 4000
typedef void (*Demo)(void);

int demoMode = 0;
int counter = 1;
int randX = 0;
int randY = 0;
////////////////////////////////////////////////////////////////////
const char* ssid = STASSID;
const char* upad;
const char* upasswd;
const char* mqttClientID;
const char* char_ip;
String name;
String cred;
String id;
String s_ip;

///////////////////////////////////////////////////////////////////
void LED_Matrix() {
  byte data = B00001111;
  digitalWrite(loadPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, data);
  digitalWrite(loadPin, HIGH);
  digitalWrite(laserPin, HIGH);
  delay(100);  
  data = B00000000;
  digitalWrite(loadPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, data);
  digitalWrite(loadPin, HIGH);
  digitalWrite(laserPin, HIGH);
  delay(50);  
  Serial.println("Bleh Bleh");
}
///////////////////////////////////////////////////////////////////
void particleDraw()
{
  for(int j=0;j<20;j++)
  {
    yield();
    randX=random(0,128);
    randY=random(0,32);
    display.setPixel(randX,randY);
  }
  LED_Matrix();
}
///////////////////////////////////////////////////////////////////
void inctf_logo() {
    display.drawXbm(0, 0,Logo_width,Logo_height,inctf);
}
/////////////////////////////////////////////////////////////////////
void disp_name() {
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0, "Hi, My name is ");
    display.setFont(ArialMT_Plain_16);
    display.drawString(0,10,name);
}
/////////////////////////////////////////////////////////////////////
void disp_id() {
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 0, "ID");
    display.setFont(ArialMT_Plain_10);
    display.drawString(0,16,id);
}
/////////////////////////////////////////////////////////////////////
void handcraft() {
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0, "Handcrafted by");
    display.setFont(ArialMT_Plain_16);
    display.drawString(0,10,"bi0s");
}
/////////////////////////////////////////////////////////////////////
void callback(char* topic, byte* payload, unsigned int length) {
 
  String message;
  for (int i = 0; i < length; ++i) {
    Serial.print((char)payload[i]);
  }
 
  message = (char*)payload;
  if ((char)payload[0]) {
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0, message);
    display.setFont(ArialMT_Plain_16);
    display.drawString(0,10 , "-----");
    display.setFont(ArialMT_Plain_10);
    display.display();
    delay(5000);
    display.clear();
    display.display();
    message = "                                                                                   ";
 
  }
 
  Serial.println();
  Serial.println("-----------------------");
 
}
//////////////////////////////////////////////////////////////////////////
void parseBytes(const char* str, char sep, byte* bytes, int maxBytes, int base) {    

for (int i = 0; i < maxBytes; i++) {        
bytes[i] = strtoul(str, NULL, base);  
 str = strchr(str, sep);          
if (str == NULL || *str == '\0') {          
  break;                        
}    
    str++;                    
    }}

//////////////////////////////////////////////////////////////////////////
void setup()
{
  pinMode(laserPin,OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(loadPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  // Debug Enable
  Serial.begin(115200);
  digitalWrite(laserPin, HIGH);

  // Display Init
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  // FS Init
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
// Calling FS Values

  File  spiffs_name = SPIFFS.open("/name", "r");
   if (!spiffs_name) {
      Serial.println("file open failed");
  }   Serial.println("---Reading from Name---");
  name=spiffs_name.readStringUntil('\n');
  spiffs_name.close();

  File  spiffs_ip = SPIFFS.open("/ip", "r");
   if (!spiffs_ip) {
      Serial.println("file open failed");
  }   Serial.println("---Reading from IP---");
  s_ip=spiffs_ip.readStringUntil('\n');
  spiffs_ip.close();
 
  File  spiffs_id= SPIFFS.open("/id", "r");
   if (!spiffs_id) {
      Serial.println("file open failed");
  }   Serial.println("---Reading from ID---");
  id=spiffs_id.readStringUntil('\n');
  spiffs_id.close();

  File  spiffs_pass= SPIFFS.open("/pass", "r");
   if (!spiffs_pass) {
      Serial.println("file open failed");
  }   Serial.println("---Reading from Creds---");
  cred=spiffs_pass.readStringUntil('\n');
  spiffs_pass.close();

 upad=name.c_str();
 upasswd=cred.c_str();
 mqttClientID=id.c_str();
 char_ip=s_ip.c_str();
// Debug Purposes
// Serial.println(upad);
// Serial.println(upasswd);
// Serial.println(mqttClientID);

// String IP conversion
  byte dec_ip[4];
  parseBytes(char_ip,'.',dec_ip,4,10);
  dec_ip[0] = (uint8_t)dec_ip[0];          
  dec_ip[1] = (uint8_t)dec_ip[1];          
  dec_ip[2] = (uint8_t)dec_ip[2];          
  dec_ip[3] = (uint8_t)dec_ip[3];            
 IPAddress ip_static(dec_ip[0],dec_ip[1],dec_ip[2],dec_ip[3]);

Demo demos[] = {disp_name,particleDraw,inctf_logo,handcraft};
int demoLength = (sizeof(demos) / sizeof(Demo));
long timeSinceLastModeSwitch = 0;

/////////////////////////////////////////////////////////////////////
void loop()
{

   server.handleClient();
   display.clear();
  // draw the current demo method
  demos[demoMode]();

  display.setFont(ArialMT_Plain_10);
  // write the buffer to the display
  display.display();


  if (millis() - timeSinceLastModeSwitch > DEMO_DURATION) {
    demoMode = (demoMode + 1)  % demoLength;
    timeSinceLastModeSwitch = millis();

  }
  counter++;
  delay(100);
 
}
/////////////////////////////////////////////////////////////////////
