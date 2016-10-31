#define LOG_OUT 1 // use the log output function
#define FHT_N 256 // set to 256 point fht
#include <FHT.h> 
//#include <OverAnimate.h>

void setEL(int ch, int level) // set a certain EL on
{
 analogWrite(ch+2, level);
}

void setup() {
  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT);
  pinMode(4,OUTPUT);
  pinMode(5,OUTPUT);
  pinMode(6,OUTPUT);
  pinMode(7,OUTPUT);
  pinMode(8,OUTPUT);
  pinMode(9,OUTPUT);
  
  // Monitor FHT over serial
    Serial.begin(115200); // use the serial port
    TIMSK0 = 0; // turn off timer0 for lower jitter
    ADCSRA = 0xe5; // set the adc to free running mode
    ADMUX = 0x42; // use adc0 (0x42 is A2, 0x40 is the pin A0)
    DIDR0 = 0x01; // turn off the digital input for adc0
}

void collectData() {
  cli();  // UDRE interrupt slows this way down on arduino1.0
    for (int i = 0 ; i < FHT_N ; i++) { // save 256 samples
      while(!(ADCSRA & 0x10)); // wait for adc to be ready
      ADCSRA = 0xf5; // restart adc
      byte m = ADCL; // fetch adc data
      byte j = ADCH;
      int k = (j << 8) | m; // form into an int
      k -= 0x0200; // form into a signed int
      k <<= 6; // form into a 16b signed int
      fht_input[i] = k; // put real data into bins
    }
    fht_window(); // window the data for better frequency response
    fht_reorder(); // reorder the data before doing the fht
    fht_run(); // process the data in the fht
    fht_mag_log(); // take the output of the fht
    sei();
}

class Bin {
public:
  int index;
  int limit;
};
Bin bins[] = {
  {2, 200},
  {20, 200},
  {100, 200},
};
int binCount = 3;

void makeItBlink()
{
  Bin bin;
  for(int binIndex = 0; binIndex < binCount; binIndex++) {
     bin = bins[binIndex];
     setEL(binIndex, fht_log_out[bin.index]);
  }
}

void loop() {
  while(1) { // reduces jitter
    collectData();
    
    // Monitor signal
    Serial.write(255); // send a start byte
    Serial.write(fht_log_out, FHT_N/2); // send out the data for debug
    
    makeItBlink();
  }
}
