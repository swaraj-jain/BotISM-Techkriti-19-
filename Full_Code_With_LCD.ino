/*This is the complete code with LCD Display
  Only This Needs to be added - Piezo Buzzer
  Note - This still needs to be tested
*/

/*Modes -
  Mode 0 - Colour Sensor and Maze Solver
  Mode 1 - Speed Control
  Mode 2 - After Speed control and before Angle Turning
  Mode 3 - Angle Turning
  Mode 4 - Finish
*/

#include <math.h>
#include <Wire.h>
#include <LiquidCrystal.h>
#define addr 0x1E           //I2C Address for The HMC5883L

int lms, mms, fld, frd, rld, rrd, flr, frr, rlr, rrr;
boolean active;
int mode, dec;
int frequency;
int th;
const double dia = 0.073;
unsigned long before,after;
int counter;
int currpwm;
int req_angle, init_angle;
int xi, yi, zi, anglei;     //triple axis data Initially
int x, y, z, angle;         //triple axis data and Net Magnitude Along Plane
int mag, turned;
boolean firstencounter;

LiquidCrystal lcd(25,26,27,28,29,30);       //(rs,enable,d4,d5,d6,d7)

void setup()
{
  Serial.begin(9600);
  pinMode(2, INPUT);     //Hall Effect Sensor Output
  pinMode(3, OUTPUT);    //Motor Driver Enable Pin
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
  pinMode(20, OUTPUT);   //output scalling for colour sensor
  pinMode(21, OUTPUT);
  pinMode(22, OUTPUT);   //colour we want to detect
  pinMode(23, OUTPUT);
  pinMode(24, INPUT);    //frequency measurement
  digitalWrite(20, HIGH);     // Setting frequency-scaling to 20%
  digitalWrite(21, LOW);      // Setting frequency-scaling to 20%
  frequency = 0;
  active = false;
  //LCD Setup
  lcd.begin(16,2);
  //HMC I2C SETUP
  Wire.begin();
  Wire.beginTransmission(addr);     // start talking
  Wire.write(0x02);                 // Set the Register
  Wire.write(0x00);                 // Tell the HMC5883L to Continuously Measure
  Wire.endTransmission();
  mode = counter = firstencounter = turned = 0;
  currpwm = 255;
  analogWrite(3,255);
}

void loop()
{
  lms = digitalRead(10);    //lms = IR Left Maze Solver
  mms = digitalRead(11);    //mms = IR Middle Maze Solver
  fld = digitalRead(12);    //fld = IR Front Left Detector
  frd = digitalRead(13);    //frd = IR Front Right Detector
  rld = digitalRead(14);    //rld = IR Rear Left Detector
  rrd = digitalRead(15);    //rrd = IR Rear Right Detector
  if (fld == LOW && frd == LOW && rld == LOW && rrd == LOW)
  {
    if (!active)
    {
      active = true;
      flr = digitalRead(16);    //flr = IR Front Left Reader
      frr = digitalRead(17);    //frr = IR Front Right Reader
      rlr = digitalRead(18);    //rlr = IR Rear Left Reader
      rrr = digitalRead(19);    //rrr = IR Rear Right Reader
    }
  }
  else if (active)
  {
    if (fld == HIGH && frd == HIGH && rld == HIGH && rrd == HIGH)       //once out of the node
    {
      active = false;
      dec = flr * 8 + frr * 4 + rlr * 2 + rrr;          //Convert Data to Decimal
      lcd.clear();
      lcd.print(dec);
      if(mode!=4)
      {
        mode++;
        if(mode==2)                 //Reset Back to Top Speed once out of the Mode 1
        {
          analogWrite(3,255);
        }
      }
    }
    else
    {
      digitalWrite(4, HIGH);
      digitalWrite(5, LOW);
      digitalWrite(6, HIGH);
      digitalWrite(7, LOW);
    }
  }
  else                                        
  {
    if (mode == 0 || mode == 2)                    //The Maze Solver here with traffic lights
    {
      digitalWrite(22, LOW);                       // Setting red filtered photodiodes to be read
      digitalWrite(23, LOW);
      frequency = pulseIn(24, LOW);                // Reading the output frequency
      frequency = map(frequency, 30, 90, 255, 0);  //maping the value of the frequency to the Red of 0 to 255
      delay(100);
      if (frequency < th && mode == 0)             //bot will stop if it dectes red light
      {
        digitalWrite(4, LOW);
        digitalWrite(5, LOW);
        digitalWrite(6, LOW);
        digitalWrite(7, LOW);
      }
      else
      {
        digitalWrite(8, LOW);                       //Ultrasound begins
        delayMicroseconds(2);
        digitalWrite(8, HIGH);
        delayMicroseconds(10);
        digitalWrite(8, LOW);
        int duration = pulseIn(9, HIGH);
        double distance = duration / 58.82;         //Ultrasound ends
        if (mms == HIGH || distance <= 50.0)        //Taking right turns if encountered obstacles or found a dead end - higher in priority
        {
          while (mms == HIGH || distance <= 50.0)
          {
            digitalWrite(4, HIGH);
            digitalWrite(5, LOW);
            digitalWrite(6, LOW);
            digitalWrite(7, HIGH);
            mms = digitalRead(11);
            lms = digitalRead(10);
            digitalWrite(8, LOW);                       //Ultrasound begins
            delayMicroseconds(2);
            digitalWrite(8, HIGH);
            delayMicroseconds(10);
            digitalWrite(8, LOW);
            duration = pulseIn(9, HIGH);
            distance = duration / 58.82;         //Ultrasound ends
            delay(5);
          }
        }
        else if (lms == LOW)            //Taking left turn if a left path is encountered
        {
          while (lms == LOW)
          {
            digitalWrite(4, LOW);
            digitalWrite(5, HIGH);
            digitalWrite(6, HIGH);
            digitalWrite(7, LOW);
            lms = digitalRead(10);
            mms = digitalRead(11);
            delay(5);
          }
        }
        else                                        //Go straight otherwise
        {
          digitalWrite(4, HIGH);
          digitalWrite(5, LOW);
          digitalWrite(6, HIGH);
          digitalWrite(7, LOW);
        }
        delay(10);
        digitalWrite(4, LOW);
        digitalWrite(5, LOW);
        digitalWrite(6, LOW);
        digitalWrite(7, LOW);
        delay(5);
      }
    }
    else if (mode == 1)                               //Speed Control Part
    {
      if(digitalRead(2)==HIGH&&counter == 0)          //When the North Pole is Encountered
      {
        before  = millis();                           //Read the Current Time
        counter = 1;
      }
      else if(digitalRead(2)==LOW&&counter==1)        //When the South Pole is Encountered
      {
        counter = 2;
      }
      else if(digitalRead(2)==HIGH&&counter==2)       //When the North Pole is Encountered the Second Time
      {
        after = millis();                             //Read the Current Time
        unsigned int duration = after-before;
        double sp = PI*dia*1000.0/duration;           //Calculate Current Speed
        if(sp>(dec/5.0))                              //If Current Speed > Required Speed, reduce it to half
        {
          currpwm = currpwm/2;
          analogWrite(3,currpwm);
        }
        else                                          //Speed Controlling is complete
        {
          counter = 3;
        }
        counter=0;
      }
    }
    else if (mode == 3)         //Angle Turning Part
    {
      req_angle = dec*10;
      lms = digitalRead(10);    //lms = IR Left Maze Solver
      mms = digitalRead(11);    //mms = IR Middle Maze Solver
      if (mms == HIGH)          //Taking right turns if encountered obstacles or found a dead end - higher in priority
      {
        digitalWrite(4, HIGH);
        digitalWrite(5, LOW);
        digitalWrite(6, LOW);
        digitalWrite(7, HIGH);
      }
      else if (lms == LOW && !firstencounter)            //Taking left turn if a left path is encountered
      {
        //Tell the HMC what regist to begin writing data into
        Wire.beginTransmission(addr);
        Wire.write(0x03); //start with register 3.
        Wire.endTransmission();

        //Read the initial data
        Wire.requestFrom(addr, 6);
        if (6 <= Wire.available())
        {
          xi = Wire.read() << 8; //MSB  x
          xi |= Wire.read(); //LSB  x
          zi = Wire.read() << 8; //MSB  z
          zi |= Wire.read(); //LSB z
          yi = Wire.read() << 8; //MSB y
          yi |= Wire.read(); //LSB y
          mag = sqrt(x * x + y * y);
          anglei = 180*asin(x / mag)/PI;
        }

        while (lms == LOW) //Taking A Left Till The Bot is On the Circle Without Re-Encountering the initial Path
        {
          digitalWrite(4, LOW);
          digitalWrite(5, HIGH);
          digitalWrite(6, HIGH);
          digitalWrite(7, LOW);
          lms = digitalRead(10);
        }
        firstencounter = 1;
      }
      else if (lms == LOW && firstencounter)    //Encounters a path in the circle
      {
        digitalWrite(4, LOW);
        digitalWrite(5, LOW);
        digitalWrite(6, LOW);
        digitalWrite(7, LOW);

        //Tell the HMC what regist to begin writing data into
        Wire.beginTransmission(addr);
        Wire.write(0x03); //start with register 3.
        Wire.endTransmission();

        //Read the current data
        Wire.requestFrom(addr, 6);
        if (6 <= Wire.available())
        {
          x = Wire.read() << 8; //MSB  x
          x |= Wire.read(); //LSB  x
          z = Wire.read() << 8; //MSB  z
          z |= Wire.read(); //LSB z
          y = Wire.read() << 8; //MSB y
          y |= Wire.read(); //LSB y
          mag = sqrt(x * x + y * y);
          angle = 180*asin(x / mag)/PI;
        }

        int diff = anglei - angle;

        if (diff > 85 - req_angle && diff < 95 - req_angle) //If its the path with error tolerance 10 turn left
        {
          digitalWrite(4, LOW);
          digitalWrite(5, HIGH);
          digitalWrite(6, HIGH);
          digitalWrite(7, LOW);
          int turned = 1;
        }
        else                                               //Avoid This Path And Continue The Circle
        {
          while (lms == LOW)
          {
            digitalWrite(4, HIGH);
            digitalWrite(5, LOW);
            digitalWrite(6, HIGH);
            digitalWrite(7, LOW);
            lms = digitalRead(10);
          }
        }
      }
      if(turned)
      {
        flr = digitalRead(16);
        frr = digitalRead(17);
        if(flr==LOW&&frr==LOW)
        mode = 4;
      }
    }
    if(mode==4)
    {
      lcd.clear();
      lcd.print("FINISH");
      //Add the buzzer part here
    }
  }
}
