/*
  Reading CO2, humidity and temperature from the SCD30
  By: Nathan Seidle
  SparkFun Electronics
  Date: May 22nd, 2018
  License: MIT. See license file for more information but you can
  basically do whatever you want with this code.

  Feel like supporting open source hardware?
  Buy a board from SparkFun! https://www.sparkfun.com/products/15112

  This example prints the current CO2 level, relative humidity, and temperature in C.

  Hardware Connections:
  Attach RedBoard to computer using a USB cable.
  Connect SCD30 to RedBoard using Qwiic cable.
  Open Serial Monitor at 115200 baud.
*/

#include <Wire.h>
#include <WiFi.h>
#include <NTPClient.h>

#include "SparkFun_SCD30_Arduino_Library.h" //Click here to get the library: http://librarymanager/All#SparkFun_SCD30
SCD30 airSensor;
#define PIN_LORA GPIO_NUM_2

WiFiUDP ntpUDP;
NTPClient time_client(ntpUDP);

int co2_offset = 400;
int count = 0;
bool isCalibrated = false;

//Wifi Inputs*****************************************************
//Set SSID & Password for WiFi network
const char* ssid = "doodlehouse";
const char* password = "f1nndog!";
//const char* ssid = "Intravision Welland";
//const char* password = "Intravision2020";
//**************************************************************


void connectToWifi(){

  Serial.println("Is Wifi Connected");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  //WiFi.setHostname(getHostname());

  Serial.println();
  Serial.print("Waiting for WiFi... ");

  int count = 0;
  while (WiFi.status() != WL_CONNECTED){
    count++;
    delay(500);
    Serial.print(".");
    if (count == 30){
      Serial.print("count = 30");
      break;
    }
  }
}

int getTimeUntilCalibration(){

    while(!time_client.update()) {
    time_client.forceUpdate();
    Serial.print("How long");
  }

  int current_time = time_client.getEpochTime();
  
  int now_hour = time_client.getHours();
  int now_minute = time_client.getMinutes();
  Serial.println("Now minute: " + String(now_minute));
  int time_until_calibration = ((60 - now_minute)*60) - time_client.getSeconds();

  return time_until_calibration;
  
}


void setup()
{
  Serial.begin(115200);
  Serial.println("Calibration Program");
  
  setCpuFrequencyMhz(80);
  pinMode(GPIO_NUM_5,OUTPUT);
  digitalWrite(GPIO_NUM_5, HIGH);
  delay(1000);
  Wire.begin(GPIO_NUM_21,GPIO_NUM_22);

  if (airSensor.begin(Wire, false) == false)
  {
    Serial.println("Air sensor not detected. Please check wiring. Freezing...");
    while (1)
      ;
  }

  airSensor.setMeasurementInterval(5);
  airSensor.setAutoSelfCalibration(false);
  airSensor.setTemperatureOffset(0);
  airSensor.beginMeasuring();

  connectToWifi();


  //The SCD30 has data ready every two seconds
}

void loop()
{
  int time_remaining = getTimeUntilCalibration();
  Serial.println("Time until calibration :  " + String(time_remaining));
  Serial.println("Current Time : " + String(millis()/1000));
  bool isCalibrated = false;
  
  while(!isCalibrated){
    
    if (airSensor.dataAvailable())
    {
      count = count + 1;
      Serial.print("co2(ppm):");
      Serial.print(airSensor.getCO2());
    
      Serial.print(" temp(C):");
      Serial.print(airSensor.getTemperature(), 1);
    
      Serial.print(" humidity(%):");
      Serial.print(airSensor.getHumidity(), 1);
    
      Serial.println();
    
      if (millis()/1000 >= time_remaining){
        airSensor.setForcedRecalibrationFactor(co2_offset);
        Serial.println("Forced Value is: ");
        isCalibrated = true;   
      }
    }
    else
      Serial.println("Waiting for new data");
      Serial.println(millis()/1000);
    
    delay(500);

    if(isCalibrated){
      while(true){
        //
      }
    }


  }

}
