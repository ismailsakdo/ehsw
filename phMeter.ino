#include "RTClib.h"
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <SimpleTimer.h>

SimpleTimer timer;

float calibration_value = 21.34 - 0.7;
int phval = 0;
unsigned long int avgval;
int buffer_arr[10], temp;

float ph_act;

RTC_DS3231 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

const int chipSelect = 10;
File myFile;

void setup()
{
  Serial.begin(115200);
  timer.setInterval(500L, logData);

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
  timer.run(); // Initiates SimpleTimer
  for (int i = 0; i < 10; i++)
  {
    buffer_arr[i] = analogRead(A0);
    delay(30);
  }
  for (int i = 0; i < 9; i++)
  {
    for (int j = i + 1; j < 10; j++)
    {
      if (buffer_arr[i] > buffer_arr[j])
      {
        temp = buffer_arr[i];
        buffer_arr[i] = buffer_arr[j];
        buffer_arr[j] = temp;
      }
    }
  }
  avgval = 0;
  for (int i = 2; i < 8; i++)
    avgval += buffer_arr[i];
  float volt = (float)avgval * 5.0 / 1024 / 6;
  ph_act = -5.70 * volt + calibration_value;

  logData(now, ph_act);
  printData(now, ph_act);
  delay(1000);
}

void logData(DateTime timestamp, float ph_act) {
  // Check if the date values are valid
  if (timestamp.year() >= 2000 && timestamp.month() >= 1 && timestamp.month() <= 12 && timestamp.day() >= 1 && timestamp.day() <= 31) {
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
      myFile.print(ph_act);
      myFile.println();
      myFile.close();
    } else {
      Serial.println("Error opening file for writing");
    }
  } else {
    Serial.println("Invalid date values, data not logged.");
  }
}


void printData(DateTime timestamp, float ph_act) {
  Serial.print("Concentration Units (standard)");
  Serial.print("PH: ");
  Serial.println(ph_act);
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
  Serial.print(" pH data logged.");
  Serial.println("---------------------------------------");
}
