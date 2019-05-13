#include <Button.h>
#include <Adafruit_NeoPixel.h>
#include <OverAnimate.h>

//////////////////////////////////////////////////////
//// Settings
//////////////////////////////////////////////////////
static const int numberOfFrontLeds = 44;
static const int numberOfRearLeds = 8;
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
Adafruit_NeoPixel frontLeds = Adafruit_NeoPixel(numberOfFrontLeds, frontPin, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel rearLeds = Adafruit_NeoPixel(numberOfRearLeds, rearPin, NEO_GRB + NEO_KHZ800);

void BlinkFunc(Animation *self, int direction, float t);
BoundFunctionAnimation blinkLeft(BlinkFunc, -1);
BoundFunctionAnimation blinkRight(BlinkFunc, 1);
void ShineFunc(Animation *self, int _, float t);
BoundFunctionAnimation shine(ShineFunc, 0);
void BlackFunc(Animation *self, int _, float t);
BoundFunctionAnimation black(BlackFunc, 0);

BoundFunctionAnimation *anims[] = {
  &black,
  &shine,
  &blinkLeft,
  &blinkRight
};
static const int animCount = 4;


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
  
  frontLeds.begin();
  frontLeds.show();
  rearLeds.begin();
  rearLeds.show();

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

  frontLeds.show();
  rearLeds.show();
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
  Adafruit_NeoPixel *leds[] = {&frontLeds, &rearLeds};
  for(int l = 0; l < 2; l++) {
    Adafruit_NeoPixel *led = leds[l];
    int yellow = led->Color(255, 255, 0);
    int black = led->Color(0, 0, 0);
    
    int beginAtIndex = led->numPixels()/2;
    int litIndex = beginAtIndex + f*direction*led->numPixels()/2;
    for(int p = 0; p < led->numPixels(); p++) {
      if(p == litIndex-1 || p == litIndex || p == litIndex+1) {
        led->setPixelColor(p, yellow);
      }
    }
  }
}

void ShineFunc(Animation *self, int _, float t)
{
  Adafruit_NeoPixel *leds[] = {&frontLeds, &rearLeds};
  for(int l = 0; l < 2; l++) {
    Adafruit_NeoPixel *led = leds[l];
    int mid = led->numPixels()/2;
    for(int i = 0; i < led->numPixels(); i++) {
      int distance = abs(mid - i);
      int range = 10;
      int strength = clamp((range-distance)*(255/range), 0, 255);
      led->setPixelColor(i, led->Color(strength, l==0?strength:0, l==0?strength:0));
    }
  }
}

void BlackFunc(Animation *self, int _, float t)
{
  for(int i = 0; i < frontLeds.numPixels(); i++) {
    int c = 0; //a(255/frontLeds.numPixels())*i;
    frontLeds.setPixelColor(i, frontLeds.Color(c, c, c));
  }

  for(int i = 0; i < rearLeds.numPixels(); i++) {
    rearLeds.setPixelColor(i, frontLeds.Color(0, 0, 0));
  }
}

void bootBlink()
{
  rainbow(&frontLeds, 10);
  rainbow(&rearLeds, 10);
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
    return strip->Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip->Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip->Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
