#include "RTClib.h"
#include <SPI.h>
#include <SD.h>
#include <Wire.h>

const int trigPin = 6; // Define the trigger pin of the ultrasonic sensor
const int echoPin = 7; // Define the echo pin of the ultrasonic sensor
const int irSensorPin = 2; // Define the IR sensor pin

RTC_DS3231 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

const int chipSelect = 10;
File myFile;

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
    myFile.println("Date,Time,Distance,IR_Status");
    myFile.close();
  } else {
    Serial.println("Error opening file");
  }

  pinMode(trigPin, OUTPUT); // Set the trigger pin as an OUTPUT
  pinMode(echoPin, INPUT);  // Set the echo pin as an INPUT
  pinMode(irSensorPin, INPUT); // Set the IR sensor pin as an INPUT
}

void loop() {
  DateTime now = rtc.now();
  
  // Trigger an ultrasonic pulse
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Read the duration of the echo pulse
  long duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance in centimeters
  int distance = duration * 0.0343 / 2; // Speed of sound is 343 m/s or 0.0343 cm/µs
  
  // Read the status of the IR sensor
  int irStatus = digitalRead(irSensorPin);
  
  logData(now, distance, irStatus);
  printData(now, distance, irStatus);
  delay(1000);
}

void logData(DateTime timestamp, int distance, int irStatus) {
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
    myFile.print(distance);
    myFile.print(", ");
    myFile.print(irStatus);
    myFile.println();
    myFile.close();
  } else {
    Serial.println("Error opening file for writing");
  }
}

void printData(DateTime timestamp, int distance, int irStatus) {
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  Serial.print("IR Status: ");
  Serial.println(irStatus == HIGH ? "Far" : "Near");
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
  Serial.print(" Data logged.");
  Serial.println("---------------------------------------");
}
