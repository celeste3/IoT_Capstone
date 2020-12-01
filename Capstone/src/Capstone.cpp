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

#include <math.h>

//Declared Variables for gyro
void setup();
void loop();
void getAccel();
void alarm();
#line 11 "c:/Users/Celeste/Documents/IoT/IoT_Capstone/Capstone/src/Capstone.ino"
byte accel_x_h, accel_x_l;
byte accel_y_h, accel_y_l;
byte accel_z_h, accel_z_l; //variables to store the individual btyes
int16_t accel_x, accel_y, accel_z;
float accel_x_g, accel_y_g, accel_z_g;
int gyroTotal;

//Declared Variables for Hall sensor
const int hallPin = D16;
int hallVal;
bool alarmState;

const int ledPin = A5;

const int MPU_ADDR = 0X68;

void setup() {
  Serial.begin(9600);
  delay(1000);
  pinMode(hallPin, INPUT);

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
  
  hallVal = digitalRead(hallPin);
  Serial.printf("hallPin %i\n", hallVal);
 
  if(alarmState == TRUE) {
    alarm();

  }
  else {
    digitalWrite(ledPin,LOW);
  }

  delay(2000);
  getAccel();
}

void getAccel() {
 Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x47); // Starting with register 0x72
  Wire.endTransmission(false); // Keep active.

  Wire.requestFrom(MPU_ADDR,2,true);
  accel_z_h = Wire.read(); 
  accel_z_l = Wire.read();

  //BIT SHIFT CODE
  accel_z = accel_z_h << 8 | accel_z_l;

  Serial.printf("Z acceleration: %i \n", accel_z);

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x43); // Starting with register 0x72
  Wire.endTransmission(false); // Keep active.

  Wire.requestFrom(MPU_ADDR,2,true);
  accel_x_h = Wire.read();
  accel_x_l = Wire.read();

  //BIT SHIFT CODE
  accel_x = accel_x_h << 8 | accel_x_l;

  Serial.printf("X acceleration: %i \n", accel_x);

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x45); // Starting with register 0x72
  Wire.endTransmission(false); // Keep active.

  Wire.requestFrom(MPU_ADDR,2,true);
  accel_y_h = Wire.read();
  accel_y_l = Wire.read();

  //BIT SHIFT CODE
  accel_y = accel_y_h << 8 | accel_y_l;

  Serial.printf("Y acceleration: %i \n", accel_y);

 gyroTotal = sqrt(pow(accel_x,2) +pow(accel_z,2) +pow(accel_y,2));
 Serial.printf("Gyro Total: %i\n", gyroTotal);
}

void alarm() {
  if (hallVal == 0) {
    digitalWrite(ledPin,LOW);
  }
  else {
    digitalWrite(ledPin, HIGH);
    digitalWrite(ledPin,LOW);
    digitalWrite(ledPin,HIGH);
    digitalWrite(ledPin,LOW);
  }
}