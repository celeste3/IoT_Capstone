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
byte accel_xout_h, accel_xout_l;
byte accel_yout_h, accel_yout_l;
byte accel_zout_h, accel_zout_l; //variables to store the individual btyes
int16_t accel_xout, accel_yout, accel_zout;
float accel_x_g, accel_y_g, accel_z_g;
float accelTotal;

//Declared Variables for Hall sensor
const int hallPin = D16;
int hallVal;
bool alarmState;

const int ledPin = A5;

const int MPU_ADDR = 0X68;

void setup() {
  Serial.begin(9600);
  delay(1000);
  pinMode(ledPin ,OUTPUT);
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

  delay(5000);
  getAccel();
}

void getAccel() {
 Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);  // Starting with register 0x3F
  Wire.endTransmission(false);  // Keep active.

  Wire.requestFrom(MPU_ADDR,6,true);
  accel_xout_h = Wire.read();
  accel_xout_l = Wire.read();
  accel_yout_h = Wire.read();
  accel_yout_l = Wire.read();
  accel_zout_h = Wire.read(); 
  accel_zout_l = Wire.read();

  //BIT SHIFT CODE
  accel_xout = accel_xout_h << 8 | accel_xout_l;
  accel_x_g = accel_xout / -16278.0;

  //BIT SHIFT CODE
  accel_yout = accel_yout_h << 8 | accel_yout_l;
  accel_y_g = accel_yout / -16100.0;

  //BIT SHIFT CODE
  accel_zout = accel_zout_h << 8 | accel_zout_l;
  accel_z_g = accel_zout / -17272.0; 

  // Serial.printf("X acceleration: %i \n", accel_xout);
  // Serial.printf("Y acceleration: %i \n", accel_yout);
  // Serial.printf("Z acceleration: %i \n", accel_zout);

  Serial.printf("X acceleration: %f \n", accel_x_g);
  Serial.printf("Y acceleration: %f \n", accel_y_g);
  Serial.printf("Z acceleration: %f \n", accel_z_g);

  accelTotal = sqrt(pow(accel_x_g,2) +pow(accel_z_g,2) +pow(accel_y_g,2));
  Serial.printf("Accel Total: %f\n", accelTotal);
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