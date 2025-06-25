#define SensorPin A0          // Analog pin connected to pH sensor
float voltage;                // To store the sensor voltage
float pHValue;                // To store the calculated pH value

// Calibration parameters (adjust based on your calibration)
float pH7_voltage = 2.5;      // Voltage from the sensor at pH 7.0
float pH_slope = -0.18;       // Voltage change per pH unit (calculated during calibration)

void setup() {
  Serial.begin(115200);       // Start Serial Monitor at 115200 baud rate
  Serial.println("pH Sensor Calibration Ready");
}

void loop() {
  int sensorValue = analogRead(SensorPin);  // Read raw analog value
  voltage = (float)sensorValue * 3.3 / 1024;  // Convert to voltage (ESP8266 ADC range is 0-3.3V)

  // Calculate pH based on calibration parameters
  pHValue = (pH7_voltage - voltage) / pH_slope + 7.0;

  Serial.print("Voltage: ");
  Serial.print(voltage, 2);
  Serial.print(" V, pH Value: ");
  Serial.println(pHValue, 2);  // Display pH with 2 decimal places

  delay(1000);  // Delay for 1 second before next reading
}

