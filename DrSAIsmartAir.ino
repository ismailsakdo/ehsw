#include "RTClib.h"
#include <SPI.h> // for the SD card module
#include <SD.h>  // for the SD card
#include <AHT10.h>
#include <Wire.h>

// AHT10
AHT10 myAHT20(AHT10_ADDRESS_0X38, AHT20_SENSOR);

// RTC setup
RTC_DS3231 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// SD Card
const int chipSelect = 10;
File myFile;

// Sensor
const int irSensorPin = 4;

// Variable
float t;
float h;
int irSensorValue;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  pinMode(irSensorPin, INPUT);
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  // rtc.adjust(DateTime(__DATE__, __TIME__));

  // Check the AHT20 sensor TH
  while (myAHT20.begin() != true) {
    Serial.println(F("AHT20 not connected or fail to load calibration coefficient")); //(F()) save string to flash & keeps dynamic memory free
    delay(5000);
  }
  Serial.println(F("AHT20 OK"));

  // Setup for the SD card
  Serial.print("Initializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("Initialization failed!");
    return;
  }
  Serial.println("Initialization done.");

  // Open file
  myFile = SD.open("DATA.txt", FILE_WRITE);

  // If the file opened OK, write to it:
  if (myFile) {
    Serial.println("File opened OK");
    // Print the headings for our data
    myFile.println("Date,Time,Temperature ºC");
  }
  myFile.close();
}

void loop() {
  // Function for t, h and t in F
  h = myAHT20.readHumidity();
  t = myAHT20.readTemperature();
  irSensorValue = digitalRead(irSensorPin);

  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  DateTime now = rtc.now();
  Serial.print("Date:  ");
  Serial.print(now.day() + String("-") + now.month() + String("-") + now.year());
  Serial.print(", Time:  ");
  Serial.print(String("||") + now.hour() + String(":") + now.minute() + String(":") + now.second());
  Serial.print(String(" ") + daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.print(String("|") + "Temp: " + String(t) + "°C, Hum: " + String(h));
  
  // Display IR sensor indication in Serial
  if (irSensorValue == 1) {
    Serial.print(", IR: Not Detected");
  } else {
    Serial.print(", IR: Detected");
  }
  
  Serial.println("--------------------------------------------------------");
  loggingTime();
  delay(1000);

}

void loggingTime() {
  DateTime now = rtc.now();
  myFile = SD.open("DATA.txt", FILE_WRITE);
  if (myFile) {
    myFile.print(now.year(), DEC);
    myFile.print('/');
    myFile.print(now.month(), DEC);
    myFile.print('/');
    myFile.print(now.day(), DEC);
    myFile.print(',');
    myFile.print(now.hour(), DEC);
    myFile.print(':');
    myFile.print(now.minute(), DEC);
    myFile.print(':');
    myFile.print(now.second(), DEC);
    myFile.print(",");
    myFile.print(t);
    myFile.print(",");
    myFile.print(h);
    myFile.print(",");
    myFile.println(irSensorValue);
  }
  myFile.close();
}
