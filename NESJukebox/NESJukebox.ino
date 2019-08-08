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
#include "SPIFFS.h"

extern "C" {
#include "uzlib/uzlib.h"
#include "uzlib/adler32.c"
#include "uzlib/crc32.c"
#include "uzlib/genlz77.c"
#include "uzlib/tinfgzip.c"
#include "uzlib/tinflate.c"
#include "uzlib/tinfzlib.c"
#include "uzlib/defl_static.c"
}

Cartridge cart(27, 33, 15, 32);

int current_song_index = -1;

const int max_compressed_song_length = 32*1024;
const int max_song_length = 74*1024;
const int max_filename_length = 32;
const int max_songs = 64;

char songs[max_songs][max_filename_length];
uint8_t currentCompressedSong[max_compressed_song_length];
uint8_t currentSong[max_song_length];
int songCount = 0;

void setup()
{
  Serial.begin(9600);
  dac_output_enable(DAC_CHANNEL_1);
  dac_output_voltage(DAC_CHANNEL_1, 200);
  SPIFFS.begin();
  uzlib_init();

  File songsDir = SPIFFS.open("/");
  File song;
  while(song = songsDir.openNextFile()) {
    strncpy(songs[songCount], song.name(), max_filename_length);
    song.close();
    songCount++;
    if(songCount == max_songs) {
      break;
      Serial.println("Max song count reached.");
    }
  }
  
  // set button to use internal pullup so they can be connected between signal pin and GND
  // https://www.arduino.cc/en/Tutorial/InputPullupSerial
  pinMode (14, INPUT_PULLUP);
  
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
  if ( playing ) {
    Serial.println("Button pressed: stopping.");
    playing = 0;
    cart.reset_nes();
    delay(100);
  } else {
    Serial.println("Button pressed: starting.");
    
    playing = 1;
    
    current_song_index = random(songCount);
    Serial.print("Playing ");
    Serial.print(current_song_index);
    Serial.print(": ");
    Serial.println(songs[current_song_index]);

    File f = SPIFFS.open(songs[current_song_index], "r");
    int bytesRead = f.readBytes((char*)currentCompressedSong, max_compressed_song_length);
    if(f.available()) {
      Serial.println("Failed to read entire file, will now crash");
    }
    f.close();
    
    Serial.print("Loaded ");
    Serial.print(bytesRead);
    Serial.println(" bytes, decompressing...");

    struct uzlib_uncomp d;
    uzlib_uncompress_init(&d, NULL, 0);
    d.source = currentCompressedSong;
    d.source_limit = currentCompressedSong+max_compressed_song_length;
    d.source_read_cb = NULL;
  
    int res = TINF_OK;
    res = uzlib_gzip_parse_header(&d);
    if (res != TINF_OK) {
        Serial.print("Failed to read zlib header");
    }
    
    d.dest_start = d.dest = currentSong;
    d.dest_limit = currentSong+max_song_length;

    res = TINF_OK;
    while (res == TINF_OK) {
        res = uzlib_uncompress_chksum(&d);
    }

    Serial.print("Decompressed ");
    Serial.print(d.curlen);
    Serial.print(" bytes (status ");
    Serial.print(res);
    Serial.println("), playing...");
    
    cart.play_nes(currentSong);
    Serial.println("Song ended, stopping.");
    playing = 0;
  }
}

void loop() {
  check();
  delay(50);
}
