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
int BOARDLED = 13;
void setup()
{
  pinMode(BOARDLED, OUTPUT); // used to show if headlight is on
  pinMode(RXLED, OUTPUT); // used to show if headlight is on
  frontLeds.begin();
  frontLeds.show();
  rearLeds.begin();
  rearLeds.begin();
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
  shineForward = !shineForward;
  digitalWrite(BOARDLED, shineForward); // status led on board
  digitalWrite(RXLED, shineForward); // for boards that don't have a status led, use rx led
  if(state != BlinkLeft && state != BlinkRight) {
    state = shineForward ? ShineStraight : NoLight;
  }
}

//////////////////////////////////////////////////////
//// Output
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
  if(!currentAnimation->scheduled) {
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

void BlinkFunc(Animation *self, int direction, float t)
{

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

