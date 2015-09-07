void setup() {
  // put your setup code here, to run once:
  pinMode(A1, INPUT);
  pinMode(A0, INPUT);
  pinMode(5, OUTPUT);
  pinMode(9, OUTPUT);
  Serial.begin(9600);
}

bool differs(int a, int b, int delta)
{
   return abs(a-b) > delta; 
}

int values[3], lastRead, lastLastRead, lastWrite;

void loop() {
  // put your main code here, to run repeatedly:
  lastRead = lastLastRead = lastWrite = 0;
  while(1) {
    //int v = analogRead(1) / 4;
    //analogWrite(9, v);
    //break;;
    
    int newValue = analogRead(1) / 4;
    if(newValue != 0 && differs(newValue, values[lastRead], 2) && differs(newValue, values[lastLastRead], 2)) {
      lastLastRead = lastRead;
      lastRead = (lastRead + 1) % 3;
      values[lastLastRead] = newValue;
    }
    
    Serial.print("values ");
    Serial.print(values[0], DEC);
    Serial.print(" ");
    Serial.print(values[1], DEC);
    Serial.print(" ");
    Serial.print(values[2], DEC);
    Serial.println("");
    
    lastWrite = (lastWrite + 1) % 3;
    analogWrite(5, values[lastWrite]);  
    analogWrite(9, values[lastWrite]);    
    delay(analogRead(0));
  }
}
