/*
    ESP32 Flappybird Game
    Copyright (C) 2023  Sam Warr

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

///////////////////////////////////////////////////////////
/////////////////////// LIBRARIES /////////////////////////
///////////////////////////////////////////////////////////
#include <Arduino.h>
#include <EEPROM.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include "images.h"
///////////////////////////////////////////////////////////
/////////////////////// CONFIGS  //////////////////////////
///////////////////////////////////////////////////////////
//#define clear_hiscore         //uncomment to clear hi score
//#define flash_led             //uncomment to flash led on death
#define button 2                //button pin
#define led 13                  //led pin
#define oled_i2c 0x3c           //oled i2c address
///////////////////////////////////////////////////////////
/////////////////////// DEFINES ///////////////////////////
///////////////////////////////////////////////////////////
#define space 32
#define fx 30
#define speed 0.01
#define oled_w 128
#define oled_h 64
#define flappy_addr 0
#define xbm_h 9
#define xbm_w 14
#define map_buffer 4
#define speed_u 0.03
#define speed_d 0.01
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire, -1);///////////////////////////////////////////////////////////
/////////////////////// VARIABLES /////////////////////////
///////////////////////////////////////////////////////////
unsigned long current = 0;
float wallx[map_buffer];
float fy = 22.00;
bool playing = false;
bool played = false;
bool up = false;
bool printed = false;
bool debouncer = false;
int gap[map_buffer];
int gap_size[map_buffer];
int hiscore = 0;
int score = 0;
int last = 0;
int led_state = LOW;
///////////////////////////////////////////////////////////
/////////////////////// FUNCTIONS /////////////////////////
///////////////////////////////////////////////////////////
void wait4start(){                  //wait for button press
    if(digitalRead(button) == 0){
      playing = true;
      printed = false;
  }
}

void generate_map(){               //generate map of pipes
  for(int i=0; i<map_buffer; i++){
    wallx[i] = oled_w + ((i + 1) * space);
    gap[i] = random(0, 33);
    gap_size[i] = random(27, (61 - gap[i]));
  }
}

void homescreen(){                 //draw homescreen
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 4);
  display.print("Flappy");
  display.drawXBitmap(0, 0, background, oled_w, oled_h, SSD1306_WHITE);
  display.drawXBitmap(55, 7, bird, xbm_w, xbm_h, SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 47);
  display.print("Press to Start");
  if(not played){
    display.setCursor(0, 28);
    display.print("Hi score: " + String(hiscore));
  }
  if(played){
    display.setCursor(0, 24);
    display.print("Scored: " + String(last));
    display.setCursor(0, 34);
    display.print("Hi score: " + String(hiscore));
  }
  display.display();
  printed = true;
  generate_map();
}

void clear_hiscore(){            //clear hi score from EEPROM
  EEPROM.write(flappy_addr, 0);
}

void draw_map(){                 //draw map of pipes
  for(int j=0; j<map_buffer; j++){
    display.fillRect(wallx[j], 0, 6, oled_h, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.fillRect(wallx[j], gap[j], 6, gap_size[j], SSD1306_BLACK);
    display.setTextColor(SSD1306_WHITE);
  }
}

void draw_bird(){               //draw bird image
  display.drawXBitmap(fx, fy, bird, xbm_w, xbm_h, SSD1306_WHITE);
}

void draw_score(){              //draw scorebox
  display.fillRect(0, 0, 15, 13, SSD1306_WHITE);
  display.setTextColor(SSD1306_BLACK);
  display.setCursor(2, 0);
  display.print(String(score));
  display.setTextColor(SSD1306_WHITE);
}

void death(){                  //death screen and clear game
  playing = false;
  fy = 22;
  last = score;
  score = 0;
  played = true;
  digitalWrite(led, HIGH);
  delay(500);
  digitalWrite(led, LOW);
  if(last > hiscore){
    hiscore = last;
    EEPROM.write(flappy_addr, hiscore);
  }
}

void setup(){
  pinMode(button, INPUT_PULLUP);
  EEPROM.begin();
  hiscore = EEPROM.read(flappy_addr);
  display.begin(SSD1306_SWITCHCAPVCC, oled_i2c);
  display.setTextSize(1);
  #ifdef flash_led
    pinMode(led, OUTPUT);
  #endif
  #ifdef clear_hiscore
    clear_hiscore();
  #endif
}

void loop(){
  if(not playing && not printed){
    homescreen();
  }
  if(not playing){
    wait4start();
  }
  if(playing){
    display.clearDisplay();
    if(digitalRead(button) == 0){
      if(not debouncer){
        current = millis();
        up = true;
        led_state = HIGH;
        debouncer = true;
      }
    }
      else{
        debouncer = false;
      }
    for(int j=0; j<map_buffer; j++){
      wallx[j] = wallx[j] - speed;
      if(wallx[j] < -7){
        score ++;
        gap[j] = random(8, 32);
        wallx[j] = oled_w;
      }
    }
    if((current + 185) < millis()){
      up = false;
    }
    if((current + 40) < millis()){
      led_state = LOW;
    }
    if(not up){
      fy = fy + speed_d;
    }
    else{
      fy = fy - speed_u;
    }
    digitalWrite(led, led_state);
    if(fy > oled_h || fy < 0){
      death();
    }
    for(int m=0; m<map_buffer; m++){
      if(wallx[m] <= fx + 7 && fx + 7 <= wallx[m] + 6){
        if(fy < gap[m] || fy + 8 > gap[m] + gap_size[m]){
          death();
        }
      }
    }
    draw_map();
    draw_bird();
    draw_score();
    display.display();
  }
}