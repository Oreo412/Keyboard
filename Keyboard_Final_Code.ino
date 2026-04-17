#include <Adafruit_GFX.h>
#include <Adafruit_GrayOLED.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>

#include <Adafruit_SSD1306.h>
#include <splash.h>

#include <HID-Project.h>
#include <HID-Settings.h>
#include <Encoder.h>
#include <Adafruit_MCP23017.h>
#include "modified_font.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

//

Encoder volumeKnob(0, 1);
int ePin = 19;
int encoderState = 0;
float backlight = 255;
volatile bool eOn;

bool caps;
bool bOngo = false;
int bTime = 0;

Adafruit_MCP23017 mcp;
byte inputs[] = {4,5,6,7,8,9};          //declaring inputs and outputs
const int inCount = sizeof(inputs)/sizeof(inputs[0]);
byte outputs[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14};
const int outCount = sizeof(outputs)/sizeof(outputs[0]);
/*
  {Esc,F1,F2,F3,F4,F5,F6,F7,F8,F9,F10,F11,F12},
  {'`','1','2','3','4','5','6','7','8','9','0','-','=',BACKSPACE},
  {TAB,'q','w','e','r','t','y','u','i','o','p','[',']','\\'},
  {CAPS_LOCK,'a','s','d','f','g','h','j','k','l',';','\'',ENTER},
  {LEFT_SHIFT,'z','x','c','v','b','n','m',',','.','/',RIGHT_SHIFT},
  {LEFT_CTRL,LEFT_GUI,LEFT_ALT,SPACE,RIGHT_ALT,RIGHT_CTRL,RIGHT_GUI,LEFT_ARROW,DOWN_ARROW,RIGHT_ARROW,UP_ARROW,END,HOME,DELETE}
 */

int keys[6][14] = {
  {KEY_ESC,KEY_F1,KEY_F2,KEY_F3,KEY_F4,KEY_F5,KEY_F6,KEY_F7,KEY_F8,KEY_F9,KEY_F10,KEY_F11,KEY_F12},
  {KEY_TILDE,KEY_1,KEY_2,KEY_3,KEY_4,KEY_5,KEY_6,KEY_7,KEY_8,KEY_9,KEY_0,KEY_MINUS,KEY_EQUAL,KEY_BACKSPACE},
  {KEY_TAB,KEY_Q,KEY_W,KEY_E,KEY_R,KEY_T,KEY_Y,KEY_U,KEY_I,KEY_O,KEY_P,KEY_LEFT_BRACE,KEY_RIGHT_BRACE,KEY_BACKSLASH},
  {KEY_CAPS_LOCK,KEY_A,KEY_S,KEY_D,KEY_F,KEY_G,KEY_H,KEY_J,KEY_K,KEY_L,KEY_SEMICOLON,KEY_QUOTE,KEY_ENTER},
  {KEY_LEFT_SHIFT,KEY_Z,KEY_X,KEY_C,KEY_V,KEY_B,KEY_N,KEY_M,KEY_COMMA,KEY_PERIOD,KEY_SLASH,KEY_RIGHT_SHIFT},
  {KEY_LEFT_CTRL,KEY_LEFT_GUI,KEY_LEFT_ALT,KEY_SPACE,KEY_RIGHT_ALT,KEY_RIGHT_CTRL,KEY_RIGHT_GUI,KEY_LEFT_ARROW,KEY_DOWN_ARROW,KEY_RIGHT_ARROW,KEY_UP_ARROW,KEY_END,KEY_HOME,KEY_DELETE}
};

bool keysDown[6][14] = {
  {false,false,false,false,false,false,false,false,false,false,false,false,false},
  {false,false,false,false,false,false,false,false,false,false,false,false,false,false},
  {false,false,false,false,false,false,false,false,false,false,false,false,false,false},
  {false,false,false,false,false,false,false,false,false,false,false,false,false},
  {false,false,false,false,false,false,false,false,false,false,false,false},
  {false,false,false,false,false,false,false,false,false,false,false,false,false,false}
};

// 



#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  // put your setup code here, to run once:
  display.clearDisplay();
  eOn = false;
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  
  mcp.begin();
  for(int i=0; i<outCount; i++){    //declaring all the outputs and setting them high
    mcp.pinMode(outputs[i],OUTPUT);
    mcp.digitalWrite(outputs[i],LOW);
  }
  for(int i=0; i<inCount; i++){     //declaring all the inputs and activating the internal pullup resistor
    pinMode(inputs[i],INPUT_PULLUP);
  }
  Serial.begin(9600);
  BootKeyboard.begin();
  pinMode(ePin, INPUT_PULLUP);
  encoderState = 0;
  mcp.digitalWrite(1, LOW);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.clearDisplay();
  display.setFont(&Dialog_bold_14);
}


void loop() {
  // put your main code here, to run repeatedly:
  capsCheck();
  oled();
  keyCheck();
  encoderStuff();
  eButton();
  f();
  ledBrightness();
}

volatile long oldState = -999;

void encoderStuff()
{
  volatile long newState;
  newState = volumeKnob.read();
  if(newState > oldState +4 || newState < oldState -4)
  {
    if(encoderState == 0)
    {
      if(newState > oldState)
        {
          Consumer.write(MEDIA_VOLUME_UP);
          Serial.print("Volume up");
          Serial.println();
        }else if(newState < oldState)
        {
          Consumer.write(MEDIA_VOLUME_DOWN);
          Serial.print("Volume down");
          Serial.println();
        }
      }
    else if(encoderState == 1)
    {
      if(newState < oldState && backlight > 0)
      {
        backlight = backlight -20;
        if(backlight < 0)
        {
          backlight = 0;
        }
        Serial.print("Light Down: ");
        Serial.print(backlight);
        Serial.println();
      }else if(newState > oldState && backlight < 255)
      {
        backlight = backlight +20;
        if( backlight > 255)
        {
          backlight = 255;
        }
        Serial.print("Light Up: ");
        Serial.print(backlight);
        Serial.println();
      }
      bTime = 1000;
    }
    oldState = newState;
  }
}

void eButton()
{
  if(digitalRead(ePin) == LOW && eOn == false)
  {
    encoderState = encoderState + 1;
    Serial.print("New Encoder State");
    Serial.println();
    if(encoderState == 2)
    {
      encoderState = 0;
    }
    eOn = true;
    Serial.print("Toggle on");
    Serial.println();
  }
  if (eOn == true && digitalRead(ePin) == HIGH)
  {
    eOn = false;
    Serial.print("Toggle off");
    Serial.println();
  }
}

void oled()
{ 
  display.clearDisplay();
  if(bOngo == true)
  {
    int backP = (backlight / 255) * 100;
    String backPp = String(backP);
    String backPpp = backPp + "%";
    display.setCursor(20,25);
    display.print(backPpp);
    Serial.print("backP: ");
    Serial.print(backP);
    Serial.println();
  }else{
    if(caps == true){
      display.setCursor(37, 28);
      display.print("CAPS");
    }
    if(encoderState == 0){
      display.setCursor(0, 28);
      display.print("V");
    }else if(encoderState == 1){
      display.setCursor(0, 28);
      display.print("B");
    }
  }
  display.display();
}

void ledBrightness()
{
  analogWrite(10, backlight);
}

void keyCheck()
{
  for (int i=0; i<14; i++){
    mcp.digitalWrite(outputs[i],LOW);
    for (int j=0; j<6; j++)
    {
      if(digitalRead(inputs[j]) == LOW && keysDown[j][i] == false)
      {      
        Serial.print("Row: ");
        Serial.print(j);
        Serial.println();
        Serial.print("Col: ");
        Serial.print(i);
        Serial.println();
        Serial.print(keys[j][i]);
        Serial.println();
        Keyboard.press(KeyboardKeycode(keys[j][i]));
        keysDown[j][i] = true;
        Serial.print("KeysDown set to true");
        Serial.println();
      }else if(digitalRead(inputs[j]) == HIGH && keysDown[j][i] == true)
      {
        Keyboard.release(KeyboardKeycode(keys[j][i]));
        keysDown[j][i] = false;
        Serial.print("KeysDown set to false");
        Serial.println();
        Serial.println();
      }
    }
    mcp.digitalWrite(outputs[i], HIGH);
  }
}

void capsCheck(){
  if (BootKeyboard.getLeds() && LED_CAPS_LOCK){
    caps = true;
  }else{
    caps = false;
  }
}

void f()
{
  if(bTime > 0)
  {
    bTime = bTime - 30;
    bOngo = true;
  }else{
    bOngo = false;
  }
}
