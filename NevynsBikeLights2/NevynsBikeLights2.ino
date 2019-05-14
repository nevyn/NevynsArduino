#include <Button.h>
#include <Adafruit_NeoPixel.h>
#include "LedStrip.h"
#include <OverAnimate.h>

//////////////////////////////////////////////////////
//// Settings
//////////////////////////////////////////////////////
static const int numberOfFrontLeds = 44;
static const int numberOfRearLeds = 28;
static const int numberOfDashLeds= 5;
static const int frontPin = 6;
static const int rearPin = 5;
// You can also change the pins for the buttons down under `buttonLeft` etc

// If true: left and right are implemented as a on-off-on rocker switch
// If false: left and right are push buttons, with a "stop blinking" switch.
static const bool kBlinkerButtonsAreSticky = false;


//////////////////////////////////////////////////////
//// Global app state
//////////////////////////////////////////////////////


AnimationSystem sys;
Adafruit_NeoPixel frontLedStrip = Adafruit_NeoPixel(numberOfFrontLeds + numberOfDashLeds, frontPin, NEO_GRB + NEO_KHZ800);
LedStrip frontAndDashLeds = LedStrip(&frontLedStrip, 0, numberOfFrontLeds + numberOfDashLeds);
LedStrip frontLeds = LedStrip(&frontLedStrip, 0, numberOfFrontLeds);
LedStrip dashLeds = LedStrip(&frontLedStrip, numberOfFrontLeds, numberOfDashLeds);

Adafruit_NeoPixel rearLedStrip = Adafruit_NeoPixel(numberOfRearLeds, rearPin, NEO_GRB + NEO_KHZ800);
LedStrip rearLeds = LedStrip(&rearLedStrip, 0, numberOfRearLeds);

void BlinkFunc(Animation *self, int direction, float t);
BoundFunctionAnimation blinkLeft(BlinkFunc, -1);
BoundFunctionAnimation blinkRight(BlinkFunc, 1);
void ShineFunc(Animation *self, int _, float t);
BoundFunctionAnimation shine(ShineFunc, 0);
void BlackFunc(Animation *self, int _, float t);
BoundFunctionAnimation black(BlackFunc, 0);

void PowerLedFunc(Animation *self, int _, float t);
BoundFunctionAnimation powerLed(PowerLedFunc, 0);

BoundFunctionAnimation *anims[] = {
  &black,
  &shine,
  &blinkLeft,
  &blinkRight,
  &powerLed
};
static const int animCount = sizeof(anims)/sizeof(BoundFunctionAnimation*);


Button buttonLeft = Button(9, PULLUP); // blink left
Button buttonRight = Button(11, PULLUP); // blink right
Button buttonStopBlinking = Button(8, PULLUP); // stop blinking
Button buttonFront = Button(10, PULLUP);  // toggle headlight

template <typename T> T clamp(T value, T low, T high)
{
    return (value < low) ? low : ((value > high) ? high : value);
}

//////////////////////////////////////////////////////
//// Setup and run
//////////////////////////////////////////////////////

int RXLED = 17;
void setup()
{
  pinMode(LED_BUILTIN, OUTPUT); // used to show if headlight is on
  pinMode(RXLED, OUTPUT); // used to show if headlight is on

  // set buttons to use internal pullup so they can be connected between signal pin and GND
  // https://www.arduino.cc/en/Tutorial/InputPullupSerial
  for(int i = 8; i < 14; i++) {
    pinMode (i, INPUT_PULLUP);
  }
  
  frontLedStrip.begin();
  frontLedStrip.show();
  rearLedStrip.begin();
  rearLedStrip.show();

  for(int i = 0; i < animCount; i++) {
    BoundFunctionAnimation *anim = anims[i];
    anim->beginTime = sys.now();
    anim->duration = 1.0;
    anim->repeats = true;
    anim->enabled = false;
    sys.addAnimation(anim);
  }
  // this one clears out the background every frame
  black.enabled = true;
  powerLed.duration = 3.0;
  powerLed.enabled = true;
  
  // hello!
  bootBlink();
}

unsigned long lastMillis;

void loop()
{
  unsigned long now = millis();
  if(!lastMillis) {
    lastMillis = now;
  }
  unsigned long diff = now - lastMillis;
  lastMillis = now;
  TimeInterval delta = diff/1000.0;
  
  handleButtons();
  sys.playElapsedTime(delta);

  frontLedStrip.show();
  rearLedStrip.show();
}

//////////////////////////////////////////////////////
//// Input
//////////////////////////////////////////////////////

void handleButtons()
{
  // Blinker buttons
  if(buttonLeft.isPressed()){
    blinkLeft.enabled = true;
    if(buttonLeft.uniquePress()) {
      blinkLeft.beginTime = sys.now();
    }
    blinkRight.enabled = false;
  } else if(buttonRight.isPressed()) {
    blinkRight.enabled = true;
    if(buttonRight.uniquePress()) {
      blinkRight.beginTime = sys.now();
    }
    blinkLeft.enabled = false;
  } else if((blinkLeft.enabled || blinkRight.enabled) && 
    (kBlinkerButtonsAreSticky || buttonStopBlinking.isPressed()
  )) {
    blinkRight.enabled = false;
    blinkLeft.enabled = false;
  }

  // Headlight toggle button
  if(buttonFront.uniquePress()) {
    shine.enabled = !shine.enabled;
    shine.beginTime = sys.now();
  }
}

//////////////////////////////////////////////////////
//// Output
//////////////////////////////////////////////////////

void BlinkFunc(Animation *self, int direction, float f)
{
  LedStrip *leds[] = {&frontLeds, &rearLeds};
  for(int l = 0; l < 2; l++) {
    LedStrip *led = leds[l];
    int yellow = Adafruit_NeoPixel::Color(255, 255, 0);
    int black = Adafruit_NeoPixel::Color(0, 0, 0);
    
    int beginAtIndex = led->numPixels()/2;
    int litIndex = beginAtIndex + f*direction*led->numPixels()/2;
    for(int p = 0; p < led->numPixels(); p++) {
      if(p == litIndex-1 || p == litIndex || p == litIndex+1) {
        led->setPixelColor(p, yellow);
      }
    }
  }

  int c = frontLedStrip.sine8(f*255)/2;
  dashLeds.setPixelColor((direction == 1) ? 0 : 4, Adafruit_NeoPixel::Color(c, c, 0));

}

void ShineFunc(Animation *self, int _, float t)
{
  LedStrip *leds[] = {&frontLeds, &rearLeds};
  for(int l = 0; l < 2; l++) {
    LedStrip *led = leds[l];
    int mid = led->numPixels()/2;
    for(int i = 0; i < led->numPixels(); i++) {
      int distance = abs(mid - i);
      int range = led->numPixels()/4;
      int strength = frontLedStrip.gamma8(clamp((range-distance)*(255/range), 0, 255));
      led->setPixelColor(i, Adafruit_NeoPixel::Color(strength, l==0?strength:0, l==0?strength:0));
    }
  }

  dashLeds.setPixelColor(2, Adafruit_NeoPixel::Color(0, 0, 1));
}

void BlackFunc(Animation *self, int _, float t)
{
  for(int i = 0; i < frontAndDashLeds.numPixels(); i++) {
    int c = 0;
    frontAndDashLeds.setPixelColor(i, Adafruit_NeoPixel::Color(c, c, c));
  }

  for(int i = 0; i < rearLeds.numPixels(); i++) {
    rearLeds.setPixelColor(i, Adafruit_NeoPixel::Color(0, 0, 0));
  }
}

void PowerLedFunc(Animation *self, int _, float t)
{
  dashLeds.setPixelColor(1, Adafruit_NeoPixel::Color(2, 0, 0));
}

void bootBlink()
{
  rainbow(&frontLedStrip, 10);
  rainbow(&rearLedStrip, 10);
}

void rainbow(Adafruit_NeoPixel *strip, uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<64; j++) {
    for(i=0; i<strip->numPixels(); i++) {
      strip->setPixelColor(i, Wheel(strip, (i+j) & 255));
    }
    strip->show();
    delay(wait);
  }

  // reset afterwards
  for(i=0; i<strip->numPixels(); i++) {
    strip->setPixelColor(i, 0);
  }
  strip->show();
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(Adafruit_NeoPixel *strip, byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return Adafruit_NeoPixel::Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return Adafruit_NeoPixel::Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return Adafruit_NeoPixel::Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
