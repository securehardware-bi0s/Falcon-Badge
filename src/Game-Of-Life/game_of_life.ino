#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <FS.h>
String name;
String cred;
String id;
String s_ip;
// STASSID is 128x64
#define OLED_RESET -1
Adafruit_SSD1306 display(128,32,&Wire,OLED_RESET);

//Current grid, max is 64x32 for 2048 byte Uno...
// width must be multiple of 8
// 8.6 iterations / second
const uint8_t width = 64;
const uint8_t height = 16;
const uint8_t pixelSize = 2;
const boolean displayIterations = true;
const uint16_t maxGenerations = 1000;

uint8_t grid[width / 8 * height];
// should be possible to do without full copy buffer, but how ?
uint8_t newgrid[width / 8 * height];

static const uint8_t PROGMEM
        GFXsetBit[] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 },
        GFXclrBit[] = { 0x7F, 0xBF, 0xDF, 0xEF, 0xF7, 0xFB, 0xFD, 0xFE };

inline void setPixel(uint8_t* ptr, uint8_t x, uint8_t y, uint8_t color) {
  uint16_t idx = (x + y * width)  / 8;  
  if (color == 1) {
    //     ptr[idx] |= (0x80 >> (x & 7));
    // >> is slow on AVR (Adafruit)
    ptr[idx] |= pgm_read_byte(&GFXsetBit[x & 7]);
  } else {
    //     ptr[idx] &= ~(0x80 >> (x & 7));
    ptr[idx] &= pgm_read_byte(&GFXclrBit[x & 7]);
  }
}

inline uint8_t getPixel(uint8_t* ptr, uint8_t x, uint8_t y) {
  uint16_t idx = (x + y * width) / 8;
//  uint8_t value = ptr[idx] & (0x80 >> (x & 7));
  uint8_t value = ptr[idx] & pgm_read_byte(&GFXsetBit[x & 7]);
  return value == 0 ? 0 : 1;
}

//Draws the grid on the display
void drawGrid(uint16_t gen) {
  display.clearDisplay();
  uint8_t offsetx = (128 - (width * pixelSize)) / 2;
  uint8_t offsety = (32 - (height * pixelSize)) / 2;
 #if 1
    for (uint8_t y = 1; y < (height - 1); y++) {
      for (uint8_t x = 1; x < (width - 1); x++) {
        uint8_t color = getPixel(grid, x, y); 
        if (color == WHITE) {
          //if (pixelSize == 1) // does not seem any faster
          //  display.drawPixel(offsetx + pixelSize*x, offsety + pixelSize*y, WHITE);
         // else 
            display.fillRect(offsetx + pixelSize*x, offsety + pixelSize*y, pixelSize, pixelSize, WHITE);
        }
    }
  }
  #else
  // works for size 1, draw directly the bitmap into the frame buffer
    display.drawBitmap(offsetx, offsety, grid, width, height, WHITE, BLACK);
  #endif
  if (displayIterations && gen) {
    display.setTextSize(1);
     display.setCursor(0,0);
     display.print(gen);
  }
  display.display();
}

//Initialize Grid
void initGrid() {
    for (uint8_t y = 0; y < height; y++) {
       for (uint8_t x = 0; x < width; x++) {
        if (x==0 || x==(width - 1) || y==0 || y== (height - 1)){
          setPixel(grid, x, y, 0);
        } else {
            setPixel(grid, x, y, random(2)); 
        }
    }   
  }
}
// order makes faster code ?
inline uint8_t getNumberOfNeighbors(uint8_t x, uint8_t y){
    return getPixel(grid, x-1, y)+
  getPixel(grid, x+1, y)+
  getPixel(grid, x-1, y-1)+
  getPixel(grid, x, y-1)+
  getPixel(grid, x+1, y-1)+
  getPixel(grid, x-1, y+1)+
  getPixel(grid, x, y+1)+
  getPixel(grid, x+1, y+1);
}

void computeNewGeneration(){
    for (uint8_t y = 1; y < (height - 1); y++) {
      for (uint8_t x = 1; x < (width - 1); x++) {
        uint8_t neighbors = getNumberOfNeighbors(x, y);
        uint8_t current = getPixel(grid, x, y);
        if (current == 1 && (neighbors == 2 || neighbors == 3 )) {
          setPixel(newgrid, x, y, 1);
        } else if (current==1)  
          setPixel(newgrid, x, y, 0);
        if (current==0 && (neighbors==3)) {
          setPixel(newgrid, x, y, 1);
        } else if (current==0) 
          setPixel(newgrid, x, y, 0);
    }  
  }
  // copy newgrid into grid
  memcpy(grid, newgrid, width * height / 8);
}

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
void setup()   {                
  Serial.begin(9600); //use only for debugging purposes
  randomSeed(analogRead(A0));
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); //initialize with the I2C addr 0x3C (128x64)
  display.display();
  delay(2000);
  display.setTextColor(WHITE, BLACK);

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

// Debug Purposes
// Serial.println(upad);
// Serial.println(upasswd);
// Serial.println(mqttClientID);

}

void loop() {       
    //Displaying a simple splash screen
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,15);
  display.print(F("Game of Life"));       
  display.display();
  delay(3500);

  display.clearDisplay();
  initGrid();
  drawGrid(0);
  delay(8000);    
//  int t = millis();
  for (uint16_t gen = 0; gen < maxGenerations; gen++) {
     computeNewGeneration();
     drawGrid(gen);
//     delay(20);
  } 
//  Serial.println(millis() - t);  
}
