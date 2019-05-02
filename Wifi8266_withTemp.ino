/*
 * This code is a combination of bme280test with the 8266DemoMaster file written by Theo Flex.
 * It sends temperature data in farhenight and uploads it to an AdaFruit IO page.
 * Required Libraries:
 *      Wire.h
 *      SPI.h
 *      Adafruit_Sensor.h
 *      Adafruit_BME280.h
 * 
 * -Ian Romano and Ahmad Tamali
 */

#include "Arduino.h"
#include <SoftwareSerial.h>

//BME280 required libraries, constants and setup
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; // I2C

SoftwareSerial espSerial(10,11);

String IO_USERNAME = "iromano"; //paste username here
String IO_KEY = "77790223d6874d2d9fbdf5e738e3d948"; //paste key here
String WIFI_SSID = "UD Devices"; //Only need to change if using other network, eduroam won't work
String WIFI_PASS = ""; //Blank for open network

int num = 1; //Counts up to show upload working

void setup() {
  Serial.begin(9600);
  espSerial.begin(9600);
  Serial.println("setting up");
  String resp = espData("get_macaddr",2000,false);
  Serial.println("mac: "+resp);
  resp = espData("wifi_ssid="+WIFI_SSID,2000,false);
  Serial.println("ssid: "+resp);
  resp = espData("wifi_pass="+WIFI_PASS,2000,false);
  Serial.println("pass: "+resp);
  resp = espData("io_user="+IO_USERNAME,2000,false);
  Serial.println("user: "+resp);
  resp = espData("io_key="+IO_KEY,2000,false);
  Serial.println("key: "+resp);
  resp = espData("setup_io",15000,false);
  Serial.println("setup: "+resp);
  if(resp.indexOf("connected") < 0){
    Serial.println("\nAdafruit IO Connection Failed");
    while(1);
  }
  resp = espData("setup_feed=1,test feed",2000,false);
  bool status;
  status = bme.begin();
  Serial.println("------ Setup Complete ----------");
}

void loop() {
  // Wait a few seconds between uploads
  // Adafruit IO only allows 30 uploads/minute, it discards everything else
  delay(5000);

  Serial.print("Num is: ");
  Serial.println(num);

  Serial.print("Temperature = ");
  float fTemp = bme.readTemperature() * (9.0/5.0) + 32;
  Serial.print(fTemp);
  Serial.println(" *F");

  String resp = espData("send_data=1,"+String(fTemp),2000,false);
  //Serial.println("data1: "+resp);

  num++;
}

String espData(String command, const int timeout, boolean debug)
{ 
  String response = "";
  espSerial.println(command);
  long int time = millis();
  while ( (time + timeout) > millis())
  {
    while (espSerial.available())
    {
      char c = espSerial.read();
      response += c;
      Serial.print(c);
    }
  }
  if (debug)
  {
    Serial.println("Resp: "+response);
  }
  response.trim();
  return response;
}
