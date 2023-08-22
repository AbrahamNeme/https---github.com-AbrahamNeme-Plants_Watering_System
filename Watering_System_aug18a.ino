#include "arduino_secrets.h"
/* 
  Sketch generated by the Arduino IoT Cloud Thing "Untitled"
  https://create.arduino.cc/cloud/things/ff59705a-4c9a-4db4-898b-1424e30a4a01 

  Arduino IoT Cloud Variables description

  The following variables are automatically generated and updated when changes are made to the Thing

  float humidity;
  float temperature;
  int moisture;
  int trigger;
  bool pump;

  Variables which are marked as READ/WRITE in the Cloud Thing will also have functions
  which are called when their values are changed from the Dashboard.
  These functions are generated with the Thing and added at the end of this sketch.
*/

// DHT sensor library - Version: Latest
#include <DHT.h>
#include <DHT_U.h>
 
// Wire Library for I2C
#include <Wire.h>

// Define DHT11 Parameters
#define DHTPIN 8
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
 
// Define variables for Temperature and Humidity
float temp;
float hum;
  
// Constant for dry sensor
const int DryValue = 3044;
 
// Constant for wet sensor
const int WetValue = 1130;
 
// Variables for soil moisture
int soilMoistureValue;
int soilMoisturePercent;
 
// Analog input port
#define SENSOR_IN 0
 
// Relay Port
#define RELAY_OUT 3
 
// Variable for pump trigger
int pump_trigger = 30;

#include "thingProperties.h"

void setup() {
  // Initialize serial and wait for port to open:
  Serial.begin(9600);
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(1500); 

  // Defined in thingProperties.h
  initProperties();

  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  
  /*
     The following function allows you to obtain more information
     related to the state of network and IoT Cloud connection and errors
     the higher number the more granular information you’ll get.
     The default is 0 (only errors).
     Maximum is 4
 */
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

  // Initialize DHT11
  dht.begin();
 
  // Set ADC to use 12 bits
  analogReadResolution(12);
 
  // Set Relay as Output
  pinMode(RELAY_OUT, OUTPUT);
 
  // Turn off relay
  digitalWrite(RELAY_OUT, LOW);
 
  // Set Pump Status to Off
  pump = false;
}

void loop() {
  // Call Arduino IoT Cloud for updates
  ArduinoCloud.update();
  // Your code here 
  
  // Get temperature and Humidity
  temp = dht.readTemperature();
  hum = dht.readHumidity();
 
  // Pass temperature and humidity values to cloud variables
  temperature = temp;
  humidity = hum;
 
  // Get soil mositure value
  soilMoistureValue = analogRead(SENSOR_IN);
 
  // Determine soil moisture percentage value
  soilMoisturePercent = map(soilMoistureValue, DryValue, WetValue, 0, 100);
 
  // Keep values between 0 and 100
  soilMoisturePercent = constrain(soilMoisturePercent, 0, 100);
 
  // Print raw value to serial monitor for sensor calibration
  Serial.println(soilMoistureValue);
 
  // Pass soil moisture to cloud variable
  moisture = soilMoisturePercent;
 
  // See if pump should be triggered
  // See if moisture is below or equal to threshold
  if (soilMoisturePercent <= pump_trigger) {
    // Turn pump on
    pumpOn();
    delay(8000);
    // Turn pump off
    digitalWrite(RELAY_OUT, LOW);
    delay(3000);
    // Turn pump on
    digitalWrite(RELAY_OUT, HIGH);
    delay(8000);
    // Turn pump off
    digitalWrite(RELAY_OUT, LOW);
 
  } else {
    // Turn pump off
    pumpOff();
  }
}


void pumpOn() {
  // Turn pump on
  digitalWrite(RELAY_OUT, HIGH);
  pump = true;
 
}
 
void pumpOff() {
  // Turn pump off
  digitalWrite(RELAY_OUT, LOW);
  pump = false;
 
}

/*
  Since Trigger is READ_WRITE variable, onTriggerChange() is
  executed every time a new value is received from IoT Cloud.
*/
void onTriggerChange()  {
  // Changes when Pump Trigger Level slider is activated
  pump_trigger = trigger;
}