/* Contains upto node detection and reading of data along with maze solver
 * These Need to be added -
 * 1. Colour Sensor and LCD Display (mode=0)       - Swaraj Jain
 * 2. Speed Control (mode=1)                       - Jay Sharma
 * 3. Angle Detection and circular path (mode=3)   - Jefferson
 * 4. Piezo Buzzer (mode=4)                        - Jai Gupta
 */
#include <math.h> 
#define addr 0x1E //I2C Address for The HMC5883

int lms,mms,fld,frd,rld,rrd,flr,frr,rlr,rrr;
boolean active;
int mode,dec;

int req_angle, init_angle;
int xi, yi, zi, anglei;   //triple axis data Initially
int x,y,z,anglef;   //triple axis data and Net Magnitude Along Plane
int mag;
boolean firstencounter;


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

                                    //HMC I2C SETUP       
  Wire.begin();
  Wire.beginTransmission(addr);     // start talking
  Wire.write(0x02);                 // Set the Register
  Wire.write(0x00);                 // Tell the HMC5883 to Continuously Measure
  Wire.endTransmission();
  
  active = false;
  mode = firstencounter = 0;
}

void loop()
{ 
  lms = digitalRead(10);    //lms = IR Left Maze Solver
  mms = digitalRead(11);    //mms = IR Middle Maze Solver

  if (mms == HIGH) //Taking right turns if encountered obstacles or found a dead end - higher in priority
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
      if(6 <= Wire.available()) 
      {
        xi = Wire.read()<<8; //MSB  x 
        xi |= Wire.read(); //LSB  x
        zi = Wire.read()<<8; //MSB  z
        zi |= Wire.read(); //LSB z
        yi = Wire.read()<<8; //MSB y
        yi |= Wire.read(); //LSB y
        mag = sqrt(x*x + y*y);
        anglei = arcsin(x/mag);
      }
      
      while(lms == LOW) //Taking A Left Till The Bot is On the Circle Without Re-Encountering the initial Path
      {
        digitalWrite(4, LOW);
        digitalWrite(5, HIGH);
        digitalWrite(6, HIGH);
        digitalWrite(7, LOW);
      }
      firstencounter = 1;
    }
    else if(lms == LOW && firstencounter)     //Encounters a path in the circle
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
      if(6 <= Wire.available())
      {
        x = Wire.read()<<8; //MSB  x 
        x |= Wire.read(); //LSB  x
        z = Wire.read()<<8; //MSB  z
        z |= Wire.read(); //LSB z
        y = Wire.read()<<8; //MSB y
        y |= Wire.read(); //LSB y
        mag = sqrt(x*x + y*y);
        angle = arcsin(x/mag);
      }

      int diff = anglei - angle;
      
      if(diff > 80-req_angle && diff < 100-req_angle)    //If its the path with error tolerance 10 turn left
      {
        digitalWrite(4, LOW);
        digitalWrite(5, HIGH);
        digitalWrite(6, HIGH);
        digitalWrite(7, LOW);
      }
      else                                               //Avoid This Path And Continue The Circle
      {
        digitalWrite(4, HIGH);
        digitalWrite(5, LOW);
        digitalWrite(6, HIGH);
        digitalWrite(7, LOW);
      }
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
