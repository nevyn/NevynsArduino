#import "_2760339_Program.h"

const unsigned long blob_sprite[5]={
  0x330000,0x666600,0xffffff,0x666600,0x330000
};

PROGMEM const unsigned long buf[][10] = {
  {0x00ff00,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000},
  {0x000000,0x00ff00,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000},
  {0x000000,0x000000,0x00ff00,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000},
  {0x000000,0x000000,0x000000,0x00ff00,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000},
  {0x000000,0x000000,0x000000,0x000000,0x00ff00,0x000000,0x000000,0x000000,0x000000,0x000000},
  {0x000000,0x000000,0x000000,0x000000,0x000000,0x00ff00,0x000000,0x000000,0x000000,0x000000},
  {0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x00ff00,0x000000,0x000000,0x000000},
  {0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x00ff00,0x000000,0x000000},
  {0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x00ff00,0x000000},
  {0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x00ff00},
};

float pos = 0.0;

void blit(unsigned long *dest, int dest_len, const unsigned long *src, int src_len, float where) {
  for(int src_i = 0; src_i < src_len; src_i++) {
    int dest_i = where + src_i;
    if(dest_i < 0) continue;
    if(dest_i >= dest_len) break;
    
    static char b[128] = {0};
    sprintf(b, "%d %d\n", src_i, dest_i);
    Serial.print(b);
    
    dest[dest_i] = src[src_i];
  }
}

void setup() {
  Serial.begin(9600);

  // put your setup code here, to run once:
  default_setup();
}

float vel = 1;

void loop() {
  
  /*if(pos > 10 || pos < 0)
    vel *= -1;
  pos = pos + vel;
  
  //memset(buf[0], 0, 10);
  //blit(buf[0], 10, blob_sprite, 5, pos);
  //send_1M_pattern(buf, 10, 300);
  
  //send_1M_pattern(pattern_test_red, 10, 10);
  */
  
  // put your main code here, to run repeatedly:
  send_1M_pattern(pattern_test_red, 10, 200);
  delay(500);
  send_1M_pattern(pattern_test_blue, 10, 200);
  send_1M_pattern(pattern_test_green, 10, 200);
  send_1M_pattern(pattern_test_white, 10, 200);
  for(int i = 0; i < 4; i++)
    send_1M_pattern(pattern_test_comet1, 10, 70);
  for(int i = 0; i < 4; i++)
    send_1M_pattern(pattern_test_comet2, 10, 70);
  for(int i = 0; i < 8; i++)
    send_1M_pattern(pattern_test_comet3, 10, 70);
  delay(500);

   
  for(int i = 280; i /= 2; i < 5) {
    send_1M_pattern(pattern_test_rainbow, 10, i);
  }
  delay(500);
  
  

}
