/*
 * Project Capstone
 * Description: Auto theft detection/baby left in backseat device
 * Author: Celeste Corazon
 * Date: November 30, 2020
 */

#include "credentials.h"

#include <neopixel.h>

#include <Adafruit_MQTT.h>

#include "Adafruit_MQTT/Adafruit_MQTT.h"
#include "Adafruit_MQTT/Adafruit_MQTT_SPARK.h"

#include <colors.h>

#include <HX711.h> //Scale library
HX711 myScale(D6,D5);

#include <math.h>

// IMPORTANT: Set pixel COUNT, PIN and TYPE
#define PIXEL_PIN D2
#define PIXEL_COUNT 4 
#define PIXEL_TYPE WS2812B

Adafruit_NeoPixel pixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

/************ Global State (you don't need to change this!) ******************/ 
TCPClient TheClient; 
 
// Setup the MQTT client class by passing in the WiFi client, MQTT server and login details. 
Adafruit_MQTT_SPARK mqtt(&TheClient,AIO_SERVER,AIO_SERVERPORT,AIO_USERNAME,AIO_KEY); 

/****************************** Feeds ***************************************/ 
// Setup Feeds to publish or subscribe 
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname> 
//Adafruit_MQTT_Subscribe Capstone = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/capstone"); 
Adafruit_MQTT_Publish alarmData = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/alarmData");
Adafruit_MQTT_Publish weightData = Adafruit_MQTT_Publish(&mqtt,AIO_USERNAME "/feeds/weightData");

unsigned long lastTime;

/****Delcare Variables for scale*****/
const int cal_factor= -1050;
const int samples=1;
float weight, rawData, cailbration;
int offset;
const int baby = 145; //Theshold for back seat weight
int i;
int c;

/***Declared Variables for accel***/
byte accel_xout_h, accel_xout_l;
byte accel_yout_h, accel_yout_l;
byte accel_zout_h, accel_zout_l; //variables to store the individual btyes
int16_t accel_xout, accel_yout, accel_zout;
float accel_x_g, accel_y_g, accel_z_g;
float accelTotal;

const float threshold = 1.1;
int setAlarm = 0;

/***Declared Variables for Hall sensor and button***/
const int buttonPin = D15;
const int hallPin = D16;
int buttonVal;
bool hallVal;
bool hallState;
bool alarmState;

const int MPU_ADDR = 0X68;

// SYSTEM_MODE(SEMI_AUTOMATIC); //Uncomment if using wifi

void setup() {
  Serial.begin(9600);
  myScale.set_scale();
  delay(1000);
  myScale.tare(); // set the tares weight (or zero)
  myScale.set_scale(cal_factor); //adjust calibration
  pixel.begin();
  pixel.show();
  
  // // Setup MQTT subscription for onoff feed.
  // mqtt.subscribe(&NAMETHIS);

  pinMode(buttonPin, INPUT_PULLDOWN); //INPUT_PULLDOWN uses Argon resister
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
    MQTT_connect();
    MQTTping();
    getAccel();
    getHallState();
    alarmEnabled(); //Use to be named alarmNotTrigger 

    //***Publish on Adafruit***//
    if((millis()-lastTime > 15000)) {
      if(mqtt.Update()) {
        weightData.publish(weight);
        Serial.printf("Publish Weight %f \n", weight);
        alarmData.publish(accelTotal);
        Serial.printf("Publish Alarm Accel Total %f \n", accelTotal);
      }
    }

    //Using data from loadcell
    weight = myScale.get_units(samples); // return weight in units set by set_scale ();

    Serial.printf("Weight: %f \n", weight);
   
    babyInBack();

  if(alarmState) {
    Serial.printf("hallVal %i, accellTotal %f \n", hallVal, accelTotal);
   if(accelTotal > threshold || hallVal == 0) {
    alarmIsOn();
   }
   else {
     alarmIsOff();
   }
  }
  else {
     alarmIsOff();
  }
  //delay(2000); // Might be a good idea to take out for demo
}

void MQTT_connect() {
 int8_t ret;

    // Stop if already connected.
    if (mqtt.connected()) {
      return;
    }

    Serial.print("Connecting to MQTT... ");

    while ((ret = mqtt.connect()) !=0) { // connect will return 0 for connected
      Serial.println(mqtt.connectErrorString(ret));
      Serial.println("Retrying MQTT connection in 5 seconds...");
      mqtt.disconnect();
      delay(5000); // wait 5 seconds
    }
    Serial.println("MQTT Connected!"); 

    while ((ret = mqtt.connect()) != 0) { //connect will return 0 for connected
      Serial.println(mqtt.connectErrorString(ret));
      Serial.println("Retrying MQTT connection in 5 secounds....");
      mqtt.disconnect();
      delay(5000); //wait 5 seconds
      }
      Serial.println("MQTT Connected!");
    }

void MQTTping() {
  static unsigned int lastTime=0;
  if((millis()-lastTime)>12000) {
    Serial.printf("Pinging MQTT \n");
    if(! mqtt.ping()) {
     Serial.printf("Pinging MQTT \n");
     mqtt.disconnect();
  }
  lastTime = millis();
 }
}

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

  delay(500); //Not sure why, but lights are more stable with a delay
}

void getHallState() {
 hallVal = digitalRead(hallPin);
}

void alarmIsOn() {
  Serial.printf("Alarm is on \n");
    if(accelTotal > threshold) {
      publish();
      while(alarmState) { //Anytime you have curly braces endent code
        Serial.printf("***AccelTotal: %i \n", accelTotal);
        pixel.setPixelColor(0,255,0,0); // Red alarm on
        pixel.setBrightness(25);
        pixel.show();
        delay(100);
        pixel.setColor(0,0,0,0);
        pixel.show();
        delay(100);
      }
    }
    else if (hallVal == 0) {
      //Serial.printf("\n");
      while(alarmState) {
          pixel.setPixelColor(1,255,255,0); // Yellow alarm 
          pixel.setBrightness(25);
          pixel.show();
      }
    }
   }

void alarmEnabled() {
  if(alarmState == true) {
    pixel.setPixelColor(2,0,255,255); // Cyan alarm is enabled
    pixel.setBrightness(25);
    pixel.show();
    delay(100);
    pixel.setPixelColor(2,0,0,0);
    pixel.show();
    delay(100);
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
  Serial.printf("Baby in back %i \n", weight);
  if(weight > baby) {
     for(c=0; c <6; c++){
      for(i=0; i < 4; i++) {
        pixel.setPixelColor(i, rainbow[c]);
        pixel.setBrightness(25);
        pixel.show();  
      }
      delay(200);
    }
   }
  }

  void publish() {
    if((millis()-lastTime > 15000)) {
      if(mqtt.Update()) {
       alarmData.publish(alarmState);
      }
    lastTime = millis();
    }
  }