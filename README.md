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
esptool.py write_flash 0x00000 intial.bin
```
### To-Do
* [ ] Add Source Code
* [ ] Add Write-Ups
