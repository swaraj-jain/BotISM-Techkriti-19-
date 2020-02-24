
void setup() 
{
    Serial.begin(9600);
    pinMode(10,INPUT);     //left sensor
    pinMode(11,INPUT);     //middle sensor
    pinMode(4,OUTPUT);    //left motor 1
    pinMode(5,OUTPUT);    //left motor 2
    pinMode(6,OUTPUT);    //right motor 1
    pinMode(7,OUTPUT);    //right motor 2
    pinMode(8,OUTPUT);    //US Trigger pin
    pinMode(9,INPUT);     //US Echo Pin
}

void loop() 
{
    digitalWrite(8,LOW);
    delayMicroseconds(2);
    digitalWrite(8,HIGH);
    delayMicroseconds(10);
    digitalWrite(8,LOW);
    int duration = pulseIn(9,HIGH);
    double distance = duration/58.82;           //Ultrasound 
    if(digitalRead(11)==HIGH||distance<10.0)
    {
        digitalWrite(4,HIGH);
        digitalWrite(5,LOW);
        digitalWrite(6,LOW);
        digitalWrite(7,HIGH);
    }
    else if(digitalRead(10)==LOW)
    {
        digitalWrite(4,LOW);
        digitalWrite(5,HIGH);
        digitalWrite(6,HIGH);
        digitalWrite(7,LOW);
    }
    else
    {
        digitalWrite(4,HIGH);
        digitalWrite(5,LOW);
        digitalWrite(6,HIGH);
        digitalWrite(7,LOW);
    }
    delay(100);
    digitalWrite(4,LOW);
    digitalWrite(5,LOW);
    digitalWrite(6,LOW);
    digitalWrite(7,LOW);
    delay(20);
}
