#define PinMQ2 (A0) // Analog input for MQ-2 sensor

void setup() {
  Serial.begin(9600);
}

void loop() {
  // Read the analog value from MQ-2 sensor
  int sensorValue = analogRead(PinMQ2);

  // Display the raw analog value
  Serial.print("Raw Sensor Value: ");
  Serial.println(sensorValue);

  delay(1000); // Delay for 1 second before the next reading
}
