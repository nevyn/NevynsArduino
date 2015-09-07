// EL test code 

void setup()
{
 setEL(1, 255);
 setEL(4, 255);
} 

void setEL(int ch, int level) // set a certain EL on
{
 analogWrite(ch+3, level);
}


int count = 2, level = 0, oldCount = 2; 

void loop() {
setEL(count, level); //(level%2)*255);
delay(1);
if(level++ == 255) {
  level = 0;
  if(count++ == 3) {
    count = 2;
  }
  setEL(oldCount, 0);
  oldCount = count;
}
}
