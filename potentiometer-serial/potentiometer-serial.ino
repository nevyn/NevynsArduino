void setup() {
  Serial.begin(9600);
}

void loop() {
  int val1 = analogRead(1);
  int val2 = analogRead(2);
  Serial.print(val1);
  Serial.print(" ");
  Serial.print(val2);
  Serial.println("");
  delay(150);
}
