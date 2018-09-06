#include <Button.h>
#include <Adafruit_NeoPixel.h>
#include <OverAnimate.h>

//////////////////////////////////////////////////////
//// Settings
//////////////////////////////////////////////////////
static const int numberOfFrontLeds = 8;
static const int numberOfRearLeds = 8;
static const int frontPin = 5;
static const int rearPin = 6;
// You can also change the pins for the buttons down under `buttonLeft` etc

// If true: left and right are implemented as a on-off-on rocker switch
// If false: left and right are push buttons, with a "stop blinking" switch.
static const bool kBlinkerButtonsAreSticky = false;


//////////////////////////////////////////////////////
//// Global app state
//////////////////////////////////////////////////////

// Is the "headlamp" mode on? That is, after we stop blinking,
// should we return to NoLight or ShineStraight?
bool shineForward = false;

// What state is this app currently in?
enum {
 NoLight,
 BlinkLeft,
 BlinkRight,
 ShineStraight,

 StateCount
} state;

/// ----

AnimationSystem anims;
Adafruit_NeoPixel frontLeds = Adafruit_NeoPixel(numberOfFrontLeds, frontPin, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel rearLeds = Adafruit_NeoPixel(numberOfRearLeds, rearPin, NEO_GRB + NEO_KHZ800);

void BlinkFunc(Animation *self, int direction, float t);
BoundFunctionAnimation blinkLeft(BlinkFunc, -1);
BoundFunctionAnimation blinkRight(BlinkFunc, 1);
void ShineFunc(Animation *self, int _, float t);
BoundFunctionAnimation shine(ShineFunc, 0);

Button buttonLeft = Button(7, PULLUP); // blink left
Button buttonRight = Button(8, PULLUP); // blink right
Button buttonStopBlinking = Button(9, PULLUP); // stop blinking
Button buttonFront = Button(10, PULLUP);  // toggle headlight

//////////////////////////////////////////////////////
//// Setup and run
//////////////////////////////////////////////////////

int RXLED = 17;
void setup()
{
  pinMode(LED_BUILTIN, OUTPUT); // used to show if headlight is on
  pinMode(RXLED, OUTPUT); // used to show if headlight is ons
  frontLeds.begin();
  frontLeds.show();
  rearLeds.begin();
  rearLeds.show();

  // hello!
  setHeadlight(true); delay(100); setHeadlight(false);
  bootBlink();
  setHeadlight(true); delay(100); setHeadlight(false);
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
  handleState();
  anims.playElapsedTime(delta);
}

//////////////////////////////////////////////////////
//// Input
//////////////////////////////////////////////////////

void handleButtons()
{
  // Blinker buttons
  if(buttonLeft.isPressed()){
    state = BlinkLeft;
  } else if(buttonRight.isPressed()) {
    state = BlinkRight;
  } else if(!kBlinkerButtonsAreSticky || buttonStopBlinking.isPressed()) {
    state = shineForward ? ShineStraight : NoLight;
  }

  // Headlight toggle button
  if(buttonFront.uniquePress()) {
    setHeadlight(!shineForward);
  }
}

void setHeadlight(bool headlight)
{
  shineForward = headlight;
  digitalWrite(LED_BUILTIN, shineForward?HIGH:LOW); // status led on board
  digitalWrite(RXLED, shineForward?LOW:HIGH); // for boards that don't have a status led, use rx led. it's pulldown.
  if(state != BlinkLeft && state != BlinkRight) {
    state = shineForward ? ShineStraight : NoLight;
  }
}

//////////////////////////////////////////////////////
//// State
//////////////////////////////////////////////////////

Animation *stateMap[] = {
  [NoLight] = NULL,
  [BlinkLeft] = &blinkLeft,
  [BlinkRight] = &blinkRight,
  [ShineStraight] = &shine
};

void handleState()
{
  Animation *currentAnimation = stateMap[state];
  if(currentAnimation && !currentAnimation->scheduled) {
    currentAnimation->beginTime = anims.now();
    currentAnimation->duration = 1.0;
    currentAnimation->repeats = true;
    anims.addAnimation(currentAnimation);
    for(int i = BlinkLeft; i < StateCount; i++) {
      if(stateMap[i] != currentAnimation) {
        anims.removeAnimation(stateMap[i]);
      }
    }
  }
  frontLeds.show();
  rearLeds.show();
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
    
    int beginAtIndex = direction>0 ? 0 : led->numPixels();
    int litIndex = beginAtIndex + f*direction*led->numPixels();
    for(int p = 0; p < led->numPixels(); p++) {
      led->setPixelColor(p, p == litIndex ? yellow : black);
    }
  }
}

void ShineFunc(Animation *self, int _, float t)
{
  for(int i = 0; i < frontLeds.numPixels(); i++) {
    int c = (i%2==0)?255:128;
    frontLeds.setPixelColor(i, frontLeds.Color(c, c, c));
  }

  for(int i = 0; i < rearLeds.numPixels(); i++) {
    rearLeds.setPixelColor(i, frontLeds.Color((i%2==0)?255:128, 0, 0));
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
