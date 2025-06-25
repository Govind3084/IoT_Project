#define SensorPin A0           // Analog pin connected to pH sensor
float voltage;                 // Store voltage from sensor
float pHValue;                 // Store calculated pH value

void setup() {
  Serial.begin(9600);          // Initialize Serial communication
  Serial.println("Calibrating pH Sensor...");
}

void loop() {
  int sensorValue = analogRead(SensorPin);         // Read raw analog value
  voltage = (float)sensorValue * 5.0 / 1024.0;    // Convert analog value to voltage (0-5V)

  // Adjust the calibration formula as needed based on your sensor
  pHValue = 7.0 + ((2.5 - voltage) / 0.18);       // Use this formula for accurate calibration

  Serial.print("Voltage: ");
  Serial.print(voltage, 2);                       // Print voltage
  Serial.print(" V, pH Value: ");
  Serial.println(pHValue, 2);                     // Print pH value with 2 decimal places

  delay(1000);                                    // Delay before next reading
}