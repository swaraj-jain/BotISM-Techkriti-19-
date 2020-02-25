/*Contains upto node detection and reading of data along with maze solver
 * 
 * These Need to be added -
 * 1. Colour Sensor and LCD Display (mode=0)       - Swaraj
 * 2. Speed Control (mode=1)                       - Jay Sharma
 * 3. Angle Detection and circular path (mode=3)   - Jefferson
 * 4. Piezo Buzzer (mode=4)                        - Jai Gupta
 */

int lms,mms,fld,frd,rld,rrd,flr,frr,rlr,rrr;
boolean active;
int mode,dec;

void setup()
{
  Serial.begin(9600);
  pinMode(4, OUTPUT);    //left motor 1
  pinMode(5, OUTPUT);    //left motor 2
  pinMode(6, OUTPUT);    //right motor 1
  pinMode(7, OUTPUT);    //right motor 2
  pinMode(8, OUTPUT);    //US Trigger pin
  pinMode(9, INPUT);     //US Echo Pin
  pinMode(10, INPUT);    //maze solver left sensor
  pinMode(11, INPUT);    //maze solver middle sensor
  pinMode(12, INPUT);    //node detecting front left sensor
  pinMode(13, INPUT);    //node detecting front right sensor
  pinMode(14, INPUT);    //node detecting rear left sensor
  pinMode(15, INPUT);    //node detecting rear right sensor
  pinMode(16, INPUT);    //node reading front left sensor
  pinMode(17, INPUT);    //node reading front right sensor
  pinMode(18, INPUT);    //node reading rear left sensor
  pinMode(19, INPUT);    //node reading rear right sensor
  active = false;
  mode=0;
}

void loop()
{
  lms = digitalRead(10);    //lms = IR Left Maze Solver
  mms = digitalRead(11);    //mms = IR Middle Maze Solver
  fld = digitalRead(12);    //fld = IR Front Left Detector
  frd = digitalRead(13);    //frd = IR Front Right Detector
  rld = digitalRead(14);    //rld = IR Rear Left Detector
  rrd = digitalRead(15);    //rrd = IR Rear Right Detector
  if(fld==LOW&&frd==LOW&&rld==LOW&&rrd==LOW)
  {
    if(!active)
    {
      active = true;
      flr = digitalRead(16);    //flr = IR Front Left Reader
      frr = digitalRead(17);    //frr = IR Front Right Reader
      rlr = digitalRead(18);    //rlr = IR Rear Left Reader
      rrr = digitalRead(19);    //rrr = IR Rear Right Reader
    }
  }
  else if(active)
  {
    if(fld==HIGH&&frd==HIGH&&rld==HIGH&&rrd==HIGH)    //once out of the node
    {
      active = false;
      dec = flr*8+frr*4+rlr*2+rrr;
      mode++;
    }
    else
    {
      digitalWrite(4, HIGH);
      digitalWrite(5, LOW);
      digitalWrite(6, HIGH);
      digitalWrite(7, LOW);
    }
  }
  else                                          //The Maze Solver here
  {
    digitalWrite(8, LOW);                       //Ultrasound begins
    delayMicroseconds(2);
    digitalWrite(8, HIGH);
    delayMicroseconds(10);
    digitalWrite(8, LOW);
    int duration = pulseIn(9, HIGH);
    double distance = duration / 58.82;         //Ultrasound ends
    if (mms == HIGH || distance < 10.0) //Taking right turns if encountered obstacles or found a dead end - higher in priority
    {
      digitalWrite(4, HIGH);
      digitalWrite(5, LOW);
      digitalWrite(6, LOW);
      digitalWrite(7, HIGH);
    }
    else if (lms == LOW)            //Taking left turn if a left path is encountered
    {
      digitalWrite(4, LOW);
      digitalWrite(5, HIGH);
      digitalWrite(6, HIGH);
      digitalWrite(7, LOW);
    }
    else                                        //Go straight otherwise
    {
      digitalWrite(4, HIGH);
      digitalWrite(5, LOW);
      digitalWrite(6, HIGH);
      digitalWrite(7, LOW);
    }
    delay(100);
    digitalWrite(4, LOW);
    digitalWrite(5, LOW);
    digitalWrite(6, LOW);
    digitalWrite(7, LOW);
    delay(10);
  }
}
