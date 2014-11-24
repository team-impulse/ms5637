#include <Wire.h>

/* (C) Team Impulse 2014
* MS5637BA combined temperature and pressure sensor driver code 
* Made available under a modified MIT license - see GitHub repository.
* (Teensy 3.1)
* Based on datasheet at http://www.meas-spec.com/downloads/MS5637-02BA03.pdf 
*/
#define baro_addr 0x76 //ms5637 default addr
#define osr 8192
// calibration constants which we expect to read out at init.
int32_t calib[7];//the first calibration var, at 0xA0, appears to be a device ID
uint32_t d1, d2;

float pressure, int_temp;

void setup(){
  Wire.begin();
  //join I2C bus
  Serial.begin(38400);//NB serial comms at 38400 baud.
  Serial.println("Send any char to continue");
  while(!Serial.available());
  //wait for a character to be sent to begin stuff happening
  barometerCalibration();//read in calibration values from the MS5637 
}

void loop(){
  
  readRawTPValues();//reads raw temp and pressure values
  processBaroData();//compensates them.
  Serial.print("T: ");Serial.print(int_temp);Serial.println("degrees C");
  Serial.print("P: ");Serial.print(pressure);Serial.println("hPa");
  delay(500);
}

void processBaroData(){
  //calculate temperature
  int32_t dt = (int32_t)(d2- calib[5] * 256);
  int32_t temp =(int32_t)(2000+(int32_t)(dt*calib[6])/(int32_t)8388608);
  //temp in 1/100ths of a degC
  
  //Calculate pressure variables
  int64_t off = (int64_t)((int64_t)calib[2] * (int64_t)131072 + ((int64_t)calib[4]*(int64_t)dt) /(int64_t)64);
  int64_t sens =(int64_t)((int64_t)calib[1] * (int64_t)65536 + ((int64_t)calib[3] * (int64_t)dt)/(int64_t)128);
  int64_t sens2 = 0;
  int64_t off2 = 0;
  int64_t t2;
  if(temp>2000){//"second order temperature compensation"
    t2 = (int64_t)((int64_t)5 * (int64_t)((int64_t)dt*(int64_t)dt) / (int64_t)274877906944);
    //high temp  
  }
  else{
    //low temp
    t2 = (int64_t)((int64_t)3 * (int64_t)((int64_t)dt*(int64_t)dt) / (int64_t)8589934592);
    off2 = (int64_t)temp - (int64_t)2000;
    off2 = (int64_t)(off2 * off2);
    sens2 = off2;
    off2 = (int64_t)((int64_t)61 * off2 / 16);
    sens2 = (int64_t)((int64_t)(29 * sens2) / 16);
  }
  //won't be less than -15degC with electronics, so don't need to account for latter case.
  temp = (int32_t)((int64_t)temp - t2);//apply compensation
  off = off-off2;
  sens = sens-sens2;
  
  int32_t p = (int32_t)((d1 * sens / 2097152 - off) / 32768);//calculate final pressure
  
  pressure = (float)(p/100.0);//now calculate nice values. Probably not worth logging floats though!
  int_temp = (float)(temp/100.0);//NB don't log/transmit floats - use 1/100ths of degree/hPa.
}


void readRawTPValues(){
  int osr_offset = 2*((osr / 256) - 1);
  Wire.beginTransmission(baro_addr);
  
  Wire.write(0x4A);//d1 convert
  Wire.endTransmission();
  delay(25);//typ time is 16.44ms for conversion at 8192 bit oversampling ratio - would guess that this is more or less constant, but doesn't hurt to allow an extra 9ms!
  Wire.beginTransmission(baro_addr);
  Wire.write(0x00);//ADC read
  Wire.endTransmission();
  Wire.requestFrom(baro_addr,3);//24 bits
  while(!Wire.available());
  
  d1 = (Wire.read()<<16) | (Wire.read()<<8) | Wire.read();//read in all three bits and get D1

  
  Wire.beginTransmission(baro_addr);//now for d2
  Wire.write(0x5A);//d2 convert
  Wire.endTransmission();
  delay(25);//typ time is 16.44ms for conversion at 8192 bit oversampling ratio - would guess that this is more or less constant, but doesn't hurt to allow an extra 9ms!
  Wire.beginTransmission(baro_addr);
  Wire.write(0x00);//ADC read
  Wire.endTransmission();    
  Wire.requestFrom(baro_addr,3);//24 bits
  while(!Wire.available());
  
  d2 = (Wire.read()<<16) | (Wire.read()<<8) | Wire.read();//read in all three bits and get D1
  
}

void barometerCalibration(){
  //reset the barometer
  Serial.println("Resetting...");
  Wire.beginTransmission(baro_addr);
  Wire.write(0x1E);
  Wire.endTransmission();

  Serial.println("Now reading calibration variables.");
  for(int i = 0;i<7;i++){
    Serial.println(i);
    Wire.beginTransmission(baro_addr);
    int reg = 0xA0;
    reg += (2*i);//each calibration variable is 2 bytes; we only need to call the MSB address.
    Wire.write(reg);//PROM registers range from 0xA0 to 0xAE
    Wire.endTransmission();
    Wire.requestFrom(baro_addr,2);
    while(!Wire.available());
    
    calib[i]=(Wire.read()<<8) | Wire.read();//store in calibration value array 
  }
  
  Serial.println("Calibration variables read.");  
    
  }

  
    
  

