

#include <Cartridge.h>
#include "songs.h"
#include <driver/dac.h>
#include <Button.h>

Cartridge cart(27, 33, 15, 32);

Button buttonShuffle = Button(14, PULLUP);
Button buttonStop = Button(32, PULLUP);
Button buttonPlay = Button(15, PULLUP);

uint8_t *songs[] = {
  smb2_overworld,
  mm3_magnetman,
  iceclimber,
//  smb2_select,
  //smb3_die,
};

  //,
  //kirby_plains,
  //mario_1,

  //smb3_athletic,
  //smb3_overworld,

int current_song_index = -1;


void setup()
{
  Serial.begin(9600);
  dac_output_enable(DAC_CHANNEL_1);
  dac_output_voltage(DAC_CHANNEL_1, 200);

  // set buttons to use internal pullup so they can be connected between signal pin and GND
  // https://www.arduino.cc/en/Tutorial/InputPullupSerial
  pinMode (14, INPUT_PULLUP);
  pinMode (32, INPUT_PULLUP);
  pinMode (15, INPUT_PULLUP);
  
  cart.init();
  cart.frame_counter_cb(check);

}

int playing = 0;
int btn = 1;

void check() {
  int v = digitalRead(14);
  Serial.println(v);
  int st = btn && !v;
  btn = v;
  if ( st ) {
    clicked();
  }
  
}

void clicked() {
   Serial.println("click");
  if ( playing ) {
    playing = 0;
    cart.reset_nes();
    delay(100);
  } else {
    playing = 1;
    
    int new_index = current_song_index;
    while(new_index == current_song_index) {
      new_index = random(sizeof(songs)/sizeof(*songs));
    }
    current_song_index = new_index;
    Serial.print("Playing ");
    Serial.println(current_song_index);
    cart.play_nes(songs[current_song_index]);
    playing = 0;
  }
}

void loop() {
  check();
  delay(50);
}
