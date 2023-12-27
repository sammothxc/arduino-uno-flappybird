# Arduino Uno Flappybird Game
## Description
This is a Flappy Bird clone for the Arduino Uno. It is written in C++ and uses the Arduino framework.

## Gameplay
The bird is controlled by a single button. Pressing the button causes the bird to jump. The goal is to get the bird through as many pipes as possible. The bird will fall if it hits a pipe or the ground. The game ends when the bird hits a pipe or the ground. The high score is saved in EEPROM. The game can be reset by pressing the button when the game is over.

## Features
- EEPROM high score save persists across resets and power cycles
- Circuit and hardware simplified to a single button and an OLED display
- Configurable for different boards and displays
- Difficulty increases as the game progresses

## Configurations
The code defines several configurations, including:
- `clear_hiscore`: If defined, the high score is cleared.
- `flash_led`: If defined, an LED flashes on jump and upon death in the game.
- `button, led, oled_i2c`: These define various pins and the I2C address for the OLED display.

## Prerequisites
You will need the following libraries installed on your Arduino IDE or PlatformIO:
- Adafruit SSD1306
- Adafruit GFX Library

## Installation
- Clone the repository to your local machine.
- Open the project in your IDE.
- Install the necessary libraries.
- Upload the code to your Uno.

## License
This project is licensed under the GNU General Public License v3.0 - see the LICENSE.md file for details.

## TODO
- Test on hardware