/*
 * Project Capstone
 * Description: Auto theft detecton device
 * Author: Celeste Corazon
 * Date: November 30, 2020
 */

#include "credentials.h"

#include <neopixel.h>

#include <Adafruit_MQTT.h>

#include <math.h>

// IMPORTANT: Set pixel COUNT, PIN and TYPE
#define PIXEL_PIN D2
#define PIXEL_COUNT 3 //Might add a fourth later
#define PIXEL_TYPE WS2812B

Adafruit_NeoPixel strip(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

/************ Global State (you don't need to change this!) ***   ***************/ 
// TCPClient TheClient; 

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details. 
// Adafruit_MQTT_SPARK mqtt(&TheClient,AIO_SERVER,AIO_SERVERPORT,AIO_USERNAME,AIO_KEY); 

/****************************** Feeds ***************************************/ 
// Setup Feeds to publish or subscribe 
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname> 
// Adafruit_MQTT_Subscribe buttonOnOf = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/buttonOnOf"); 
// Adafruit_MQTT_Publish randomPub = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/randomPub");


//Declared Variables for gyro
byte accel_xout_h, accel_xout_l;
byte accel_yout_h, accel_yout_l;
byte accel_zout_h, accel_zout_l; //variables to store the individual btyes
int16_t accel_xout, accel_yout, accel_zout;
float accel_x_g, accel_y_g, accel_z_g;
float accelTotal;

const int threshold = 1.2;
int setAlarm = 0;

//Declared Variables for Hall sensor
const int buttonPin = D15;
const int hallPin = D16;
bool hallVal;
bool hallState;
bool alarmState = true; //True tempory hard wire for testing

const int MPU_ADDR = 0X68;

SYSTEM_MODE(SEMI_AUTOMATIC); //Uncomment if using wifi

void setup() {
  Serial.begin(9600);
  delay(1000);
  pixel.begin();
  pixel.show();
  
  // // Setup MQTT subscription for onoff feed.
  // //mqtt.subscribe(&TempF);
  // mqtt.subscribe(&buttonOnOf);

  pinMode(buttonPin, INPUT);
  pinMode(hallPin, INPUT);

  attachInterrupt(buttonPin, buttonClick, RISING);

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
    // MQTT_connect();
    getAccel();
    getHallState();
    babyInBack();

  if(alarmState) {
    hallVal = 0; //This is temporary
    Serial.printf("hallVal %i, accellTotal %f \n", hallVal, accelTotal);
   if(accelTotal > threshold || hallVal == true) {
    alarmIsOn();
   }
   else {
     alarmIsOff();
   }
  }
  else {
     alarmIsOff();
  }
}

// void MQTT_connect() {
      // int8 
    
// }

void getAccel() {
 Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);  // Starting with register 0x3B
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

  Serial.printf("X acceleration: %f \n", accel_x_g);
  Serial.printf("Y acceleration: %f \n", accel_y_g);
  Serial.printf("Z acceleration: %f \n", accel_z_g);

  accelTotal = sqrt(pow(accel_x_g,2) +pow(accel_z_g,2) +pow(accel_y_g,2));
  Serial.printf("Accel Total: %f\n", accelTotal);
}

void getHallState() {
 hallVal = digitalRead(hallPin);
}

void alarmIsOn() {
    Serial.printf("Alarm is on \n");
    if (hallVal == 0) {
      pixel.setPixelColor(255,255,0,0); // Yellow alarm neutral
      pixel.setBrightness(100);
      pixel.show
    }
    else {
      pixel.setPixelColor(0,255,0,0): // Red alarm on
      pixel.setBrightness(100);
      pixel.show();
      delay(100);
      pixel.clear();
      pixel.show();
      delay();
    }
  }

void alarmIsOff() {
  pixel.clear();
  pixel.show();
}

void buttonClick() {
  alarmState = !alarmState;
}

void babyInBack() {
  
}