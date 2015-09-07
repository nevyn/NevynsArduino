int DA=A0;  // Pin for Analog Output - AO
int led = 13;
int threshold = 40; //Change this
int sensorvalue=0;

void setup(){
  //Serial.begin(9600);
  pinMode(led, OUTPUT);
  setEL(1, 255);
  setEL(4, 255);
}

void setEL(int ch, int level) // set a certain EL on
{
 analogWrite(ch+3, level);
}

#define BUFFERLEN 200
static int buffer[BUFFERLEN] = {0};
static int index = 0;
void microphonicLoop() {
  sensorvalue = analogRead(DA);  //Read the analog value
  buffer[index] = sensorvalue;
  int oppositeIndex = (index + BUFFERLEN/2) % BUFFERLEN;
  index = (index + 1) % BUFFERLEN;
  
  //Serial.println(sensorvalue);  //Print the analog value
  setEL(3, sensorvalue);
  setEL(2, buffer[oppositeIndex]);
  delay(1);
}

typedef struct {
  int channel;
  int value;
  int dir;
} ElDir;
ElDir els[2] = {
  {2, 0, 1},
  {3, 128, 1},
};

void pulsateLoop() {
    for(int i = 0; i < 2; i++) {
       ElDir *el = &els[i];
       setEL(el->channel, el->value);
       el->value += el->dir;
       if(el->value == 0 || el->value == 255) {
         el->dir *= -1;
       }
    }
    delay(3);
}

void loop(){
  //microphonicLoop();
  pulsateLoop();
}
