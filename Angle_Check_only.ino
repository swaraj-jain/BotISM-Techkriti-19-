#include <math.h>
#include <Wire.h>
int const addr = 0x1E; //I2C Address for The HMC5883


int req_angle, init_angle;
long x, y, z;   //triple axis data Initially
float mag, angle;

void setup()
{
  Serial.begin(9600);
  //HMC I2C SETUP
  Wire.begin();
  Wire.beginTransmission(addr);     // start talking
  Wire.write(0x02);                 // Set the Register
  Wire.write(0x00);                 // Tell the HMC5883 to Continuously Measure
  Wire.endTransmission();
  Serial.println("MC");
  x = y = z = mag = angle = 0;
}
void loop()
{
  //Tell the HMC what regist to begin writing data into
  Wire.beginTransmission(addr);
  Wire.write(0x03); //start with register 3.
  Wire.endTransmission();

  //Read the initial data
  Wire.requestFrom(addr, 6);
  x = Wire.read()*256; //MSB  x
  x += Wire.read(); //LSB  x
  z = Wire.read()*256; //MSB  z
  z += Wire.read(); //LSB z
  y = Wire.read()*256; //MSB y
  y += Wire.read(); //LSB y
  mag = sqrt(x * x + y * y);
  angle = asin(x / mag);
  Serial.println("MC");
  Serial.print(x);
  Serial.print("\t");
  Serial.print(y);
  Serial.print("\t");
  Serial.print(z);
  Serial.print("\t");
  Serial.print(mag);
  Serial.print("\t");
  Serial.print(angle);
  Serial.println();

  delay(1000);

}
