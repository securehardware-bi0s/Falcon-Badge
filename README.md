# Falcon-Badge

**Work in progress.**

## Layout
```
├── bin
│   ├── falcon-fire.bin
│   ├── final-unlocked.bin
│   ├── game_of_life.bin
│   ├── intial.bin
│   └── serial-exploitation.bin
└── README.md

```
### Software Pre-requisities
Software to install:
* For Windows:
    * [USB-UART Driver](https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers)
* On Linux/Windows:
    * pyserial -> ```pip install pyserial```
    * esptool -> ```pip install esptool```
        * For more info visit [esptool](https://github.com/espressif/esptool).
    * miniterm.py (Installed with pyserial)

### Binaries

* falcon-fire.bin
    * A version of the 80s *Asteroid* Game.
* final-unlocked.bin
    * Firmware with unlocked LEDs and defence against laser attacks.
* game_of_life.bin
    * A version of *Conway's Game of Life* on Falcon.
* initial.bin
    * The basic firmware everyone's badge was loaded with.
* serial-exploitation.bin
    * Firmware that was designed to teach you serial exploitation.


### How to flash the firmware?

Make sure you've installed the correct drivers and the ```esptool.py```
Clone the repository and open a terminal in the same directory.
Run the following command with the desired file in the same terminal.
```
esptool.py write_flash 0x00000 initial.bin
```
### Source Code
The code is written with the help of [Arduino](https://github.com/esp8266/Arduino) SDK, considering it to be beginner-friendly platform. You can run [MicroPython](https://docs.micropython.org/en/latest/esp8266/tutorial/intro.html) on the same board as well.
#### The structure
The badge has the personal data of the users stored in the SPIFFS system. Hence we use the 4MB flash memory in NodeMCU with 1MB specified for filesystem.
If you're interested in learning more about the [Filesystem](https://arduino-esp8266.readthedocs.io/en/latest/filesystem.html) used in ESP8266 Arduino. 
This filesystem consists of the following files:
```
data/
├── chall.png -> The forensics challenge desgined by @_f41c0n
├── flag -> For the game 
├── id -> Cadet/Padawan/Rebel ID
├── ip -> Static IP Assigned to each badge
├── name -> Username
├── pass -> 5 char of username and 5 char of the phone number

```
#### Some of the used libraries include
* [ThingPulse SSD1306](https://github.com/ThingPulse/esp8266-oled-ssd1306)
* [Adafruit GFX](https://github.com/adafruit/Adafruit-GFX-Library)
* [Adafruit SSD1306](https://github.com/adafruit/Adafruit_SSD1306)
* [u8g2](https://github.com/olikraus/u8g2)
* Already included in the base installation
    * Wire
    * FS
#### How to compile? 
* If you have successfully installed Arduino IDE , you can just clone the code to get started.
![Warning](https://i.ibb.co/0JGRqHv/error.png)
You might face an issue of creating a new folder, just press "OK" and continue.
* If you like the makefile approach then I suggest you look at [makeEspArduino](https://github.com/plerup/makeEspArduino) project.
    * To build 
        ```make -f makeEspArduino.mk all FLASH_DEF=4M1M```
    * For flashing the code 
        ```make -f makeEspArduino.mk flash FLASH_DEF=4M1M```
    * For flashing the filesystem 
        ```make -f makeEspArduino.mk flash_fs FLASH_DEF=4M1M```
### To-Do

* [x] Add Source Code
* [ ] Add OTA and MQTT Code
* [ ] Add Write-Ups

