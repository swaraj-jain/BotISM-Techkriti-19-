
int frequency = 0;
int min;
int S0=20,S1=21,S2=22,S3=23,sensorop=24;

void setup() {
   pinMode(4, OUTPUT);    //left motor 1
  pinMode(5, OUTPUT);    //left motor 2
  pinMode(6, OUTPUT);    //right motor 1
  pinMode(7, OUTPUT);    //right motor 2
  pinMode(S0, OUTPUT);   //output scalling
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);    //colour we want to detect
  pinMode(S3, OUTPUT);
  pinMode(sensorop, INPUT);   //frequency measure
  
  
  digitalWrite(S0,HIGH);      // Setting frequency-scaling to 30%
  digitalWrite(S1,LOW);       // Setting frequency-scaling to 30%
  
  Serial.begin(9600);
}
void loop()
{
    digitalWrite(S2,LOW);     // Setting red filtered photodiodes to be read
    digitalWrite(S3,LOW);

    frequency = pulseIn(sensorop, LOW);        // Reading the output frequency
    frequency = map(frequency, 30,90,255,0);     //maping the value of the frequency to the Red of 0 to 255
    delay(100);
    if(frequency <min)
    { //bot will stop if it dectes red light
      digitalWrite(4,LOW);
      digitalWrite(5,LOW);
      digitalWrite(6,LOW);
      digitalWrite(7,LOW);
    }
}
  


    
