/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "c:/Users/Celeste/Documents/IoT/IoT_Capstone/Capstone/src/Capstone.ino"
/*
 * Project Capstone
 * Description: Auto theft detecton device
 * Author: Celeste Corazon
 * Date: November 30, 2020
 */

//Declared Variables
void setup();
void loop();
#line 9 "c:/Users/Celeste/Documents/IoT/IoT_Capstone/Capstone/src/Capstone.ino"
byte gyro_zout_h, gyro_zout_l; //variables to store the individual btyes
int16_t gyro_z;
float gyro_z_g;

const int MPU_ADDR = 0X68;

void setup() {
  Serial.begin(9600);
  delay(2000);

  //Begin 12C communications
  Wire.begin();

  //Begin transmission to MPU-6050             
  Wire.beginTransmission(MPU_ADDR);

  // Select and write to PWR_MGMT1 register
  Wire.write(0x6B);
  Wire.write(0); // set to 0 (wakes up MPU-6050)

  //End transmission and close connection
  Wire.endTransmission(true);

}

void loop() {

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x47); // Starting with register 0x72
  Wire.endTransmission(false); // Keep active.

  Wire.requestFrom(MPU_ADDR, 2, true);
  gyro_zout_h = Wire.read(); 
  gyro_zout_l = Wire.read();

  Serial.printf("Z Gyro rotation is %i \n", gyro_z);

}