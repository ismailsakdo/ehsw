#include "RTClib.h"
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <SoftwareSerial.h>

SoftwareSerial pmsSerial(2, 3);

RTC_DS3231 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

const int chipSelect = 4;
File myFile;

struct pms5003data {
  uint16_t framelen;
  uint16_t pm10_standard, pm25_standard, pm100_standard;
  uint16_t pm10_env, pm25_env, pm100_env;
  uint16_t particles_03um, particles_05um, particles_10um, particles_25um, particles_50um, particles_100um;
  uint16_t unused;
  uint16_t checksum;
};

struct pms5003data data;

void setup() {
  Serial.begin(115200);

  // Sensor PMS
  pmsSerial.begin(9600);

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
    myFile.println("Date,Time,Temperature ºC, PM1.0, PM2.5, PM10");
    myFile.close();
  } else {
    Serial.println("Error opening file");
  }
}

void loop() {
  DateTime now = rtc.now();

  if (readPMSdata(&pmsSerial)) {
    logData(now);
    printData(now);
  }
}

void logData(DateTime timestamp) {
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
    myFile.print(data.pm10_standard);
    myFile.print(", ");
    myFile.print(data.pm25_standard);
    myFile.print(", ");
    myFile.println(data.pm100_standard);
    myFile.close();
  } else {
    Serial.println("Error opening file for writing");
  }
}

void printData(DateTime timestamp) {
  Serial.println();
  Serial.println("---------------------------------------");
  Serial.println("Concentration Units (standard)");
  Serial.print("PM 1.0: "); Serial.print(data.pm10_standard);
  Serial.print("\t\tPM 2.5: "); Serial.print(data.pm25_standard);
  Serial.print("\t\tPM 10: "); Serial.println(data.pm100_standard);
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
  Serial.println(" PM data logged.");
  Serial.println("---------------------------------------");
}

boolean readPMSdata(Stream *s) {
  if (!s->available()) {
    return false;
  }

  if (s->peek() != 0x42) {
    s->read();
    return false;
  }

  if (s->available() < 32) {
    return false;
  }

  uint8_t buffer[32];
  uint16_t sum = 0;
  s->readBytes(buffer, 32);

  for (uint8_t i = 0; i < 30; i++) {
    sum += buffer[i];
  }

  uint16_t buffer_u16[15];
  for (uint8_t i = 0; i < 15; i++) {
    buffer_u16[i] = buffer[2 + i * 2 + 1];
    buffer_u16[i] += (buffer[2 + i * 2] << 8);
  }

  memcpy((void *)&data, (void *)buffer_u16, 30);

  if (sum != data.checksum) {
    Serial.println("Checksum failure");
    return false;
  }
  return true;
}
