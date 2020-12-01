/*
 * Project Capstone
 * Description: Auto theft detecton device
 * Author: Celeste Corazon
 * Date: November 30, 2020
 */

#include <I2Cdev.h>

//Declared Variablesp
byte gyro_xout_h, gyro_xout_l;
byte gyro_yout_h, gyro_yout_l;
byte gyro_zout_h, gyro_zout_l; //variables to store the individual btyes
int16_t gyro_x, gyro_y, gyro_z;
float gyro_x_g;
float gyro_y_g;
float gyro_z_g;

const int MPU_ADDR = 0X68;

void setup() {
  Serial.begin(9600);
  delay(1000);

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
  delay(2000);
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x47); // Starting with register 0x72
  Wire.endTransmission(false); // Keep active.

  Wire.requestFrom(MPU_ADDR,2,true);
  gyro_zout_h = Wire.read(); 
  gyro_zout_l = Wire.read();

  //BIT SHIFT CODE
  gyro_z = gyro_zout_h << 8 | gyro_zout_l;

  Serial.printf("Z Gyro rotation is %i \n", gyro_z);

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x43); // Starting with register 0x72
  Wire.endTransmission(false); // Keep active.

  Wire.requestFrom(MPU_ADDR,2,true);
  gyro_xout_h = Wire.read();
  gyro_xout_l = Wire.read();

  //BIT SHIFT CODE
  gyro_x = gyro_xout_h << 8 | gyro_zout_l;

  Serial.printf("X Gyro rotation is %i \n", gyro_x);

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x45); // Starting with register 0x72
  Wire.endTransmission(false); // Keep active.

  Wire.requestFrom(MPU_ADDR,2,true);
  gyro_yout_h = Wire.read();
  gyro_yout_l = Wire.read();

  //BIT SHIFT CODE
  gyro_y = gyro_xout_h << 8 | gyro_yout_l;

  Serial.printf("Y Gyro rotation is %i \n", gyro_y);

}