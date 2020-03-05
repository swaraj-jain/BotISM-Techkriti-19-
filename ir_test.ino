void setup() {
  Serial.begin(9600);
 pinMode(3,INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
Serial.println(digitalRead(3));
}
