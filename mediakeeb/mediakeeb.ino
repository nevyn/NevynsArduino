#include <USB.h>
#include <USBHIDKeyboard.h>
#include <USBHIDConsumerControl.h>
#include <M5Unified.h>

#if !defined(ARDUINO_USB_MODE)
#error Needs an M5AtomS3, which has USB support
#endif

USBHIDKeyboard kbd;
USBHIDConsumerControl cons;
static const unsigned long debounce = 50;
// if I wanted to do a bluetooth keyboard: https://gist.github.com/manuelbl/66f059effc8a7be148adb1f104666467 or https://hackaday.com/2020/02/13/emulating-a-bluetooth-keyboard-with-the-esp32/
// Based on https://github.com/espressif/arduino-esp32/blob/master/libraries/USB/examples/Keyboard/KeyboardMessage/KeyboardMessage.ino

class MediaKey {
public:
  MediaKey(int pin, int equiv)
    : pin(pin), equiv(equiv), lastReading(HIGH), currentState(HIGH), lastDebounce(0)
  {
  }
  void setup() {
    pinMode(pin, INPUT_PULLUP);
  }
  void update() {
    int reading = digitalRead(pin);
    unsigned long now = millis();
    if(reading != lastReading) {
      lastDebounce = now;
    }
    if((now - lastDebounce) > debounce) {
      if(reading != currentState) {
        currentState = reading;
        if(currentState == LOW) on(); else off();
      }
    }
    lastReading = reading;
  }
  void on() {
    cons.press(equiv);
  }
  void off() {
    cons.release();
  }
  int pin;
  int equiv;

  int lastReading;
  int currentState;
  unsigned long lastDebounce;
};
MediaKey prev(G5,  CONSUMER_CONTROL_SCAN_PREVIOUS),
         next(G38, CONSUMER_CONTROL_SCAN_NEXT),
         play(G39, CONSUMER_CONTROL_PLAY_PAUSE);
MediaKey *keys[] = {&prev, &next, &play};

void setup() {
  M5.begin();
  for(int i = 0; i < 3;  i++)
    keys[i]->setup();
  
  //kbd.begin();
  cons.begin();
  USB.begin();
  //Serial.begin(9600);
}

void loop() {
  M5.update();
  for(int i = 0; i < 3;  i++)
    keys[i]->update();
  if(M5.BtnA.isPressed()) {
    cons.press(CONSUMER_CONTROL_SLEEP);
    cons.release();
  }
}
