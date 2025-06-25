#define SensorPin A0           // Analog pin connected to pH sensor
unsigned long avgValue;        // To store the average value of sensor readings
int sensorBuffer[10];          // Buffer to store 10 readings for smoothing
int temp;

void setup() {
  Serial.begin(9600);          // Initialize Serial communication
  Serial.println("pH Sensor Ready");
}

void loop() {
  for (int i = 0; i < 10; i++) {   // Read sensor data 10 times
    sensorBuffer[i] = analogRead(SensorPin);
    delay(30);                     // Short delay between readings
  }

  for (int i = 0; i < 9; i++) {    // Sort sensorBuffer values (ascending order)
    for (int j = i + 1; j < 10; j++) {
      if (sensorBuffer[i] > sensorBuffer[j]) {
        temp = sensorBuffer[i];
        sensorBuffer[i] = sensorBuffer[j];
        sensorBuffer[j] = temp;
      }
    }
  }

  avgValue = 0;                   // Reset average value
  for (int i = 2; i < 8; i++) {   // Average 6 middle values for stability
    avgValue += sensorBuffer[i];
  }
  float voltage = (float)avgValue * 5.0 / 1024 / 6;  // Convert ADC value to voltage
  float pHValue = 3.5 * voltage;                     // Convert voltage to pH (calibration)

  Serial.print("pH Value: ");
  Serial.println(pHValue, 2);    // Print pH value with 2 decimal places

  delay(1000);                   // Delay before next measurement
}

