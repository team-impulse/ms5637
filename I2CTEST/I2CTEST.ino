#include <Wire.h>

void setup(){
  Wire.begin();
  Serial.begin(38400);
  while(!Serial.available());
  
  Wire.beginTransmission(0x76);
  Wire.write(0x1E);
  Wire.endTransmission();
  delay(100);
  Wire.beginTransmission(0x76); 
  Wire.write(0xA0);
 Wire.endTransmission();
Wire.requestFrom(0x76,6);
while(Wire.available() < 14);
Serial.print("1 ");
Serial.println((Wire.read()<<8)|Wire.read());
Serial.print("2");
Serial.println((Wire.read()<<8)|Wire.read());
Serial.print("3");
Serial.println((Wire.read()<<8)|Wire.read());
Serial.print("4 ");
Serial.println((Wire.read()<<8)|Wire.read());
Serial.print("5");
Serial.println((Wire.read()<<8)|Wire.read());
Serial.print("6");
Serial.println((Wire.read()<<8)|Wire.read());
Serial.print("7");
Serial.println((Wire.read()<<8)|Wire.read());

}
void loop(){

}
