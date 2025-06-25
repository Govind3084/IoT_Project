const int pHSensorPin = A0; // Analog pin connected to the pH sensor

void setup() {
  Serial.begin(9600); // Initialize serial communication
}

void loop() {
  int sensorValue = analogRead(pHSensorPin); // Read the analog value from the pH sensor
  float voltage = sensorValue * (3.3 / 1023.0); // Convert analog value to voltage

  // Convert voltage to pH value
  // Calibration values will vary based on your specific sensor
  float pHValue = 7 + ((2.5 - voltage) / 0.18);

  // Print the pH value to the Serial Monitor
  Serial.print("pH Value: ");
  Serial.println(pHValue);

  delay(2000); // Wait for 2 seconds before taking another reading
}