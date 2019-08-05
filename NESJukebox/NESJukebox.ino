/** By nevyn.jpg@gmail.com
  * 2019-08-05
  * 
  * Plays NES songs in vgz format when a button on pin 14
  * is pressed.
  * 
  * Requires an ESP32. I'm using an Adafruit Feather HUZZAH32.
  * 
  * Upload songs to the data folder using SPIFFS.
  * Uploading guide: https://randomnerdtutorials.com/install-esp32-filesystem-uploader-arduino-ide/
  */

#include <Cartridge.h>
#include <driver/dac.h>
#include <Button.h>
#include "SPIFFS.h"

Cartridge cart(27, 33, 15, 32);

Button buttonShuffle = Button(14, PULLUP);
Button buttonStop = Button(32, PULLUP);
Button buttonPlay = Button(15, PULLUP);

int current_song_index = -1;

const int max_song_length = 32*1024;
const int max_filename_length = 32;
char songs[max_filename_length][64];
uint8_t currentSong[max_song_length];
int songCount = 0;

void setup()
{
  Serial.begin(9600);
  dac_output_enable(DAC_CHANNEL_1);
  dac_output_voltage(DAC_CHANNEL_1, 200);
  SPIFFS.begin();

  File songsDir = SPIFFS.open("/");
  File song;
  while(song = songsDir.openNextFile()) {
    strncpy(songs[songCount], song.name(), max_filename_length);
    song.close();
    songCount++;
  }
  

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
  //Serial.println(v);
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
    
    current_song_index = random(songCount);
    Serial.print("Playing ");
    Serial.print(current_song_index);
    Serial.print(": ");
    Serial.println(songs[current_song_index]);

    File f = SPIFFS.open(songs[current_song_index], "r");
    int bytesRead = f.readBytes((char*)currentSong, max_song_length);
    f.close();
    
    Serial.print("Loaded ");
    Serial.print(bytesRead);
    Serial.println(" bytes, now playing...");
    
    cart.play_nes(currentSong);
    playing = 0;
  }
}

void loop() {
  check();
  delay(50);
}
