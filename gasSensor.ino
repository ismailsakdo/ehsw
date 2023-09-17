#include "RTClib.h"
#include <SPI.h>
#include <SD.h>
#include <Wire.h>

#define Board ("Arduino UNO")
#define PinMQ2 (A0)   // Analog input for MQ-2 sensor
#define PinMQ135 (A1) // Analog input for MQ-135 sensor

RTC_DS3231 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

const int chipSelect = 4;
File myFile;

// Global variables for low and high values
int lowValueMQ2 = 0;
int highValueMQ2 = 1000;

int lowValueMQ135 = 0;
int highValueMQ135 = 1000;

//variable
int lpg; int alcohol;

void setup() {
  Serial.begin(115200);

  Wire.begin();
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  Serial.print("Initializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("Initialization failed!");
    return;
  }
  Serial.println("Initialization done.");

  // Open file
  myFile = SD.open("DATALOG.txt", FILE_WRITE);

  if (myFile) {
    Serial.println("File opened OK");
    // Print the headings for our data
    myFile.println("Date,Time,LPG,Alcohol");
    myFile.close();
  } else {
    Serial.println("Error opening file");
  }
}

void loop() {
  DateTime now = rtc.now();
  
  // Read MQ-2 sensor
  int lpg = map(analogRead(PinMQ2), 0, 1023, lowValueMQ2, highValueMQ2);
  
  // Read MQ-135 sensor
  int alcohol = map(analogRead(PinMQ135), 0, 1023, lowValueMQ135, highValueMQ135);
  
  logData(now, lpg, alcohol);
  printData(now, lpg, alcohol);
  delay(1000);
}

void logData(DateTime timestamp, int lpg, int alcohol) {
  // Open the data file for appending
  myFile = SD.open("DATALOG.txt", FILE_WRITE);

  if (myFile) {
    myFile.print(timestamp.year(), DEC);
    myFile.print('/');
    myFile.print(timestamp.month(), DEC);
    myFile.print('/');
    myFile.print(timestamp.day(), DEC);
    myFile.print(' ');
    myFile.print(timestamp.hour(), DEC);
    myFile.print(':');
    myFile.print(timestamp.minute(), DEC);
    myFile.print(':');
    myFile.print(timestamp.second(), DEC);
    myFile.print(", ");
    myFile.print(lpg);
    myFile.print(", ");
    myFile.print(alcohol);
    myFile.println();
    myFile.close();
  } else {
    Serial.println("Error opening file for writing");
  }
}

void printData(DateTime timestamp, int lpg, int alcohol) {
  Serial.print("Concentration Units (standard)");
  Serial.print("LPG: ");
  Serial.println(lpg);
  Serial.print("Alcohol: ");
  Serial.println(alcohol);
  Serial.println("---------------------------------------");
  Serial.print("Date: ");
  Serial.print(timestamp.year(), DEC);
  Serial.print('/');
  Serial.print(timestamp.month(), DEC);
  Serial.print('/');
  Serial.print(timestamp.day(), DEC);
  Serial.print(" Time: ");
  Serial.print(timestamp.hour(), DEC);
  Serial.print(':');
  Serial.print(timestamp.minute(), DEC);
  Serial.print(':');
  Serial.print(timestamp.second(), DEC);
  Serial.print(" PM data logged.");
  Serial.println("---------------------------------------");
}
