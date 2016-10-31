// This is the Radioshack Tricolor LED strip (~$20) library. It's not very good,
// and the strip can only address the lights 3 at a time, but it works.
// The LED strip is connected to Vin, Ground and pin 1.
#import "_2760339_Program.h"

// Simple button library that works really well.
#include <Button.h>

// This is a tri-state flip switch (pin-9-on, off, pin-10-on)
Button buttonLeft = Button(9,PULLUP);
Button buttonRight = Button(10,PULLUP);

// This is a push button that toggles "headlamp", connected to pin 11
Button buttonFront = Button(11,PULLUP);

// Is the "headlamp" mode on? That is, after we stop blinking,
// should we return to NoLight or ShineStraight?
bool shineForward = false;

// What state is this app currently in?
enum {
 NoLight,
 BlinkLeft,
 BlinkRight,
 ShineStraight 
} state;

// The different Tricolor LED strip animations we'll be using.
// * Each hex value is 0xRRBBGG
// * Each line represents the entire strip for one frame
// * Each line is a frame. The duration of the frame is specified when calling send_1M_pattern.
PROGMEM const unsigned long blink_left[][10]={
  {0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000},
  {0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000},
  {0x000000,0x000000,0x000000,0xff00ff,0x000000,0x000000,0x000000,0x000000,0xff00ff,0xff00ff},
  {0x000000,0x000000,0x000000,0xff00ff,0xff00ff,0x000000,0x000000,0x000000,0xff00ff,0xff00ff},
  {0x000000,0x000000,0x000000,0xff00ff,0xff00ff,0x000000,0x000000,0x000000,0xff00ff,0xff00ff},
  {0x000000,0x000000,0x000000,0x000000,0xff00ff,0x000000,0x000000,0x000000,0x000000,0x000000},
  {0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000},
};

PROGMEM const unsigned long blink_right[][10]={
  {0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000},
  {0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000},
  {0x000000,0xff00ff,0x000000,0x000000,0x000000,0xff00ff,0xff00ff,0x000000,0x000000,0x000000},
  {0xff00ff,0xff00ff,0x000000,0x000000,0x000000,0xff00ff,0xff00ff,0x000000,0x000000,0x000000},
  {0xff00ff,0xff00ff,0x000000,0x000000,0x000000,0xff00ff,0xff00ff,0x000000,0x000000,0x000000},
  {0xff00ff,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000},
  {0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000},
};

PROGMEM const unsigned long blink_straight[][10]={
  {0x000000,0x000055,0x808080,0x000055,0x000000,0x000000,0x330000,0xff0000,0x330000,0x000000},
  {0x000000,0x000033,0xcccccc,0x000033,0x000000,0x000000,0x330000,0xff0000,0x330000,0x000000},
  {0x000000,0x000044,0xffffff,0x000044,0x000000,0x000000,0x330000,0xff0000,0x330000,0x000000},
  {0x000000,0x000033,0xcccccc,0x000033,0x000000,0x000000,0x330000,0xff0000,0x330000,0x000000},
  {0x000000,0x000055,0x808080,0x000055,0x000000,0x000000,0x330000,0xff0000,0x330000,0x000000},
};

PROGMEM const unsigned long nothing[][10]={
  {0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000},
};

void setup() {
  // Initialize LEDs
  default_setup();
  
  // Use the 'pin 13 led' on the Arduino to confirm that the headlamp button works
  pinMode(13, OUTPUT);
}

void loop() {
  // 1. Read buttons.
  
  // 1a. Since left and right use a flip switch, they will be pressed
  // for the entire duration that we should blink...
  if(buttonLeft.isPressed()){
    state = BlinkLeft;
  } else if(buttonRight.isPressed()) {
    state = BlinkRight;
  } else {
    // and thus when it isn't set, we should return to either headlamp or dark mode.
    state = shineForward ? ShineStraight : NoLight;
  }
  
  // 1b. If the headlamp button is pressed, toggle headlamp mode once (not every 'loop' iteration
  if(buttonFront.uniquePress()) {
    shineForward = !shineForward;
    digitalWrite(13, shineForward);
    if(state != BlinkLeft && state != BlinkRight) {
      state = shineForward ? ShineStraight : NoLight;
    }
  }
  // 2. Based on our state, run one animation.
  if(state == BlinkLeft) {
    send_1M_pattern(blink_left, 7, 100); 
  } else if(state == BlinkRight) {
    send_1M_pattern(blink_right, 7, 100); 
  } else if(state == ShineStraight) {
    // This is too long :/ You actually need to hold down the buttonFront because this one
    // blocks execution for too long.
    send_1M_pattern(blink_straight, 5, 70);
  } else {
    // This seems to be the only way to turn off the entire strip
    send_1M_pattern(nothing, 1, 1);
  }
  

}
