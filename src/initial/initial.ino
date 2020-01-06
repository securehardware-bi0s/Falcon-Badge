/**
 * Initial-Firmware
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
int c = 0;
int flag = 0;
int t;

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
// Libraries Included for modules
#include <Wire.h>
#include "SSD1306Wire.h"
#include <FS.h>

SSD1306Wire display(0x3c, 4, 5, GEOMETRY_128_32);

// Custom Header Files
#include "images.h"

String name;
String cred;
String id;
String s_ip;
///////////////////////////////////////////////////////////////////
void inctf_logo() {
    display.drawXbm(0, 0,Logo_width,Logo_height,inctf);
}
void drawWarning() {
    display.drawXbm(0, 0,Logo_width,Logo_height,attack);
}
void drawDamage() {
    display.drawXbm(0, 0,Logo_width,Logo_height,damage);
}
/////////////////////////////////////////////////////////////////////
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
}
/////////////////////////////////////////////////////////////////////
void LED_Repair() {
  byte data = B00001111;
  digitalWrite(loadPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, data);
  digitalWrite(loadPin, HIGH);
  digitalWrite(laserPin, HIGH);
  delay(10000);
  File f = SPIFFS.open("/flag", "w");
  if (!f) {
      Serial.println("file open failed");
  }
    f.println("0");
  f.close();
  data = B00000000;
  digitalWrite(loadPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, data);
  digitalWrite(loadPin, HIGH);
  digitalWrite(laserPin, HIGH);
  delay(50);
}
/////////////////////////////////////////////////////////////////////
void attack_notif() {
  display.clear();
  drawWarning();
  display.display();
  LED_Matrix();
  delay(1000);
}
/////////////////////////////////////////////////////////////////////
void repair() {
  display.clear();
  drawDamage();
  display.display();
  flag = 0;
  c = 0;
  LED_Repair();
}
/////////////////////////////////////////////////////////////////////
void disp_name() {
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0, "Hi, My name is ");
    Serial.println("Hi, My name is");
    display.setFont(ArialMT_Plain_16);
    display.drawString(0,10,name);
    Serial.println(name);
}
/////////////////////////////////////////////////////////////////////
void disp_id() {
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 0, "ID");
    Serial.println("ID");
    display.setFont(ArialMT_Plain_10);
    display.drawString(0,16,id);
    Serial.println(id);
}
/////////////////////////////////////////////////////////////////////
void handcraft() {
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0, "Handcrafted by");
    Serial.println("Handcrafted by bi0s");
    display.setFont(ArialMT_Plain_16);
    display.drawString(0,10,"bi0s");
}
void check() {
 //Laser Logic
  int sensorValue = analogRead(A0);  
  if (sensorValue > 200 && sensorValue < 390) {
    c = c + 1;
    display.clear();
    Serial.println("Warning.... Falcon Under Attack");
    attack_notif();
    display.display(); 
  }
  if (c >= 3) {
    flag = 1;

    File f = SPIFFS.open("/flag", "w");
    if (!f) {
      Serial.println("file open failed");
    }
//    Serial.println("====== Writing to flag=========");
    f.println("1");
//    Serial.println(millis());
    f.close();
    delay(3000);

    display.clear();
    Serial.println("Repairing");
    display.display();
    repair();
  } 
}
//////////////////////////////////////////////////////////////////////////
void setup() {
  pinMode(laserPin,OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(loadPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  digitalWrite(laserPin, HIGH);
  // Debug Enable
  Serial.begin(9600);
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
  } //  Serial.println("---Reading from Name---");
  name=spiffs_name.readStringUntil('\n');
  spiffs_name.close();

  File  spiffs_ip = SPIFFS.open("/ip", "r");
   if (!spiffs_ip) {
      Serial.println("file open failed");
  }  // Serial.println("---Reading from IP---");
  s_ip=spiffs_ip.readStringUntil('\n');
  spiffs_ip.close();
  
  File  spiffs_id= SPIFFS.open("/id", "r");
   if (!spiffs_id) {
      Serial.println("file open failed");
  }  // Serial.println("---Reading from ID---");
  id=spiffs_id.readStringUntil('\n');
  spiffs_id.close();

  File  spiffs_pass= SPIFFS.open("/pass", "r");
   if (!spiffs_pass) {
      Serial.println("file open failed");
  }  // Serial.println("---Reading from Creds---");
  cred=spiffs_pass.readStringUntil('\n');
  spiffs_pass.close();

  File f = SPIFFS.open("/flag", "r");
  if (!f) {
     Serial.println("file open failed");
  }  
//  Serial.println("====== Reading from SPIFFS file =======");
  String s=f.readStringUntil('\n');
  t = s.toInt();
  Serial.print(t);
  delay(500);
  if (t == 1) {
     repair();
  }
  f.close();

// Debug Purposes
// Serial.println(upad); 
// Serial.println(upasswd);
// Serial.println(mqttClientID);
  Serial.println("falcon{g3t_start3d_w!th_h4rdw4r3}");

}


/////////////////////////////////////////////////////////////////////
void loop() {
  display.clear();
  check();
  display.setFont(ArialMT_Plain_10);
  display.clear();
  disp_name();
  display.display();
  delay(1000);
  check();
  display.clear();
  disp_id();
  display.display();
  delay(1000);
  check();
  display.clear();
  inctf_logo();
  display.display();
  delay(1000);
  check();
  display.clear();
  handcraft();
  display.display();
  delay(1000);
 
}
/////////////////////////////////////////////////////////////////////
