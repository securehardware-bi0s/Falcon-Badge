#include <Arduino.h>
#include <Wire.h>
#include "U8g2lib.h"
// graphics
#include "ship_0.h"
#include "ship_1.h"
#include "ship_2.h"
#include "ship_3.h"
#include "asteroid_0.h"

#define FRAME_RATE 16
#define BOOST_AMOUNT 4
#define SHIP_SIZE 8
#define ASTEROID_SIZE 8
#define SCREEN_HEIGHT 32
#define SCREEN_WIDTH 128
#define NUM_ASTEROIDS 12
#define ASTEROID_MAX_SPEED 6
#define ROCKET_SPEED 3

// If you want to use a different screen
// change the constructor here :), and defines above
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);


enum GAMESTATE {
  START, RUNNING, STOPPED
};

GAMESTATE gameState = START;

uint8_t loopCount = 0;

uint8_t shipY = SCREEN_HEIGHT;
uint8_t shipBoost = 0;

struct asteroid {
  asteroid() : x(SCREEN_WIDTH), y(SCREEN_HEIGHT), s(0) {}
  int16_t x;
  int16_t y;
  uint8_t s;
};

asteroid asteroids[NUM_ASTEROIDS];

uint16_t asteroidCount = 0;
uint16_t lastScore = 0;
uint16_t highScore = 0;

bool rocketFlying = false; // only one rocket
uint16_t rocketX = SCREEN_WIDTH;
uint16_t rocketY = SCREEN_HEIGHT;

void drawRocket() {
  // draw a pixel
  u8g2.drawPixel(rocketX, rocketY);
  u8g2.drawPixel(rocketX + 1, rocketY);
  u8g2.drawPixel(rocketX + 2, rocketY);

}

void fireRocket() {
  rocketFlying = true;
  rocketX = SHIP_SIZE;
  rocketY = shipY - SHIP_SIZE / 2; // center ship
}

void moveRocket() {
  if (rocketX >= SCREEN_WIDTH) {
    // stop the rocket at the screen edge
    rocketFlying = false;
  }
  rocketX += ROCKET_SPEED;
}

void drawAsteroid(asteroid& a) {
  u8g2.drawXBM(a.x, a.y - ASTEROID_SIZE, ASTEROID_SIZE, ASTEROID_SIZE, asteroid_0_bits);
}

void printAsteroid(asteroid& a) {
  Serial.print(a.x);
  Serial.print(", ");
  Serial.print(a.y);
  Serial.print(", ");
  Serial.println(a.s);
}

void moveAsteroid(asteroid& a) {
  if (a.x < 0) {
    // remove asteroid and stop it
    asteroidCount++;
    a.x = SCREEN_WIDTH;
    a.y = SCREEN_HEIGHT;
    a.s = 0;
  } else {
    a.x -= a.s;
  }
}

void drawShip() {
  if (shipBoost == 0 && shipY > 24) {
    u8g2.drawXBM(0, shipY - SHIP_SIZE, SHIP_SIZE, SHIP_SIZE, ship_0_bits);
  } else {
    switch (loopCount % 3) {
      case 0:
        u8g2.drawXBM(0, shipY - SHIP_SIZE, SHIP_SIZE, SHIP_SIZE, ship_1_bits);
        break;
      case 1:
        u8g2.drawXBM(0, shipY - SHIP_SIZE, SHIP_SIZE, SHIP_SIZE, ship_2_bits);
        break;
      case 2:
      default:
        u8g2.drawXBM(0, shipY - SHIP_SIZE, SHIP_SIZE, SHIP_SIZE, ship_3_bits);
    }
  }

}

void moveShip() {
  if (shipY < SCREEN_HEIGHT) {
    shipY++;
  }

  // only boost if we dont go out side the screen
  if (shipY > SHIP_SIZE) {
    // only remove as much height as screen is available
    shipY -= shipY - SHIP_SIZE > shipBoost ? shipBoost : shipY;
  }

  // make sure ship keeps inside screen (remove?)
  shipY = shipY < SHIP_SIZE ? SHIP_SIZE : shipY;

  if (shipBoost > 0) {
    shipBoost--;
  }
}

void boostShip() {
  shipBoost = BOOST_AMOUNT;
}


int buttonState1 = 0;
int buttonState2 = 0;

void setup() {
  Serial.begin(9600);
  pinMode(12, INPUT);
  pinMode(16, INPUT);
  u8g2.begin();
  u8g2.setContrast(0);
  Serial.println("");

}


void loop() {
  if (gameState == START) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_profont12_tf);
    u8g2.drawStr(12, 8, "Falcon Fire");
    u8g2.drawStr(6, 32, "~~ Press Button ~~");
    u8g2.sendBuffer();

    delay(250);

    buttonState1 = digitalRead(12);
    if (buttonState1 == HIGH) {
      gameState = RUNNING;
      rocketFlying = false;
    }
    return;
  } else if (gameState == STOPPED) {
    if (highScore < lastScore) {
      highScore = lastScore; // remember new highscore
    }
    for (uint8_t i = 0; i < NUM_ASTEROIDS; i++) {
      asteroids[i].x = SCREEN_WIDTH;
      asteroids[i].s = 0;
    }

    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_profont12_tf);
    u8g2.drawStr(24, 8, String("Highscore: " + String(highScore)).c_str());
    u8g2.drawStr(34, 20, String("Score: " + String(lastScore)).c_str());
    u8g2.drawStr(6, 32, "~~ Press Button ~~");
    u8g2.sendBuffer();

    delay(250);

    buttonState1 = digitalRead(12);
    if (buttonState1 == HIGH) {
      gameState = START;
    }
    return;
  }

  // main game loop starts here

  // input handling for button D5 and D6
  loopCount++;
  buttonState1 = digitalRead(12);
  if (buttonState1 == HIGH) {
    boostShip();
  }

  buttonState2 = digitalRead(12);
  if (buttonState2 == HIGH) {
    // only
    if (!rocketFlying) {
      fireRocket();
    }
  }

  moveShip();
  moveRocket();

  for (uint8_t i = 0; i < NUM_ASTEROIDS; i++) {

    // collision detection vs ship
    if (asteroids[i].x <= SHIP_SIZE / 2        // asteroid is close to ship
        && abs(asteroids[i].y - shipY) <= SHIP_SIZE / 2 // and is a hit
       ) {
      gameState = STOPPED;
      lastScore = asteroidCount;
      asteroidCount = 0;
    }

    // collision detection vs rocket
    if (rocketFlying) {
      // check if rocket x/y is close to asteroid
      if (asteroids[i].x > rocketX
          && asteroids[i].x < rocketX + ASTEROID_SIZE
          && asteroids[i].y > rocketY
          && asteroids[i].y < rocketY + ASTEROID_SIZE) {
        // we have hit
        rocketFlying = false;
        asteroids[i].x = -1;
      }
    }

    uint8_t r = random(1, max(1, 2000 / (10 + asteroidCount))); // randomly add an asteroid
    if (asteroids[i].x >= SCREEN_WIDTH && r == 1) {
      asteroids[i].s = random(1, ASTEROID_MAX_SPEED);
      asteroids[i].y = random(ASTEROID_SIZE, SCREEN_HEIGHT);
    }
    moveAsteroid(asteroids[i]);
  }

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_profont12_tf);
  u8g2.drawStr(110, 10, String(asteroidCount).c_str());

  drawShip();
  for (uint8_t i = 0; i < NUM_ASTEROIDS; i++) {
    drawAsteroid(asteroids[i]);
  }

  if (rocketFlying) {
    drawRocket();
  }

  u8g2.sendBuffer();

  delay(1000 / FRAME_RATE);
}
