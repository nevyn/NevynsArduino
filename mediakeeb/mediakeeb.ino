#include <Button.h>
#include <USB.h>
#include <USBHIDKeyboard.h>
#include <M5Unified.h>

#if !defined(ARDUINO_USB_MODE)
#error Needs an M5AtomS3, which has USB support
#endif

// if I wanted to do a bluetooth keyboard: https://gist.github.com/manuelbl/66f059effc8a7be148adb1f104666467 or https://hackaday.com/2020/02/13/emulating-a-bluetooth-keyboard-with-the-esp32/
// Based on https://github.com/espressif/arduino-esp32/blob/master/libraries/USB/examples/Keyboard/KeyboardMessage/KeyboardMessage.ino

Button buttonPrev = Button(G5, PULLUP);
Button buttonNext = Button(G38, PULLUP);
Button buttonPlay = Button(G39, PULLUP);

// TODO: Better key debounce

USBHIDKeyboard kbd;

void setup() {
  M5.begin();
  pinMode(G5, INPUT_PULLUP);
  pinMode(G38, INPUT_PULLUP);
  pinMode(G39, INPUT_PULLUP);
  kbd.begin();
  USB.begin();

}

void loop() {
  M5.update();

  if(buttonPrev.uniquePress())
  {
    kbd.print("prev");
  }
  if(buttonNext.uniquePress())
  {
    kbd.print("buttonNext");
  }
  if(buttonPlay.uniquePress())
  {
    kbd.print("buttonPlay");
  }
}
