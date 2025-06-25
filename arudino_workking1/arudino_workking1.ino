#define SensorPin A0  // pH sensor connected to analog pin A0
unsigned long int avgValue;  
float voltage, pHValue;
int buf[10], temp;

float offset = 0.0; // Calibration offset to adjust the pH value
float calibrationSlope = -5.7; // Adjust this value based on your sensor's datasheet or calibration

void setup() {
  pinMode(13, OUTPUT);  
  Serial.begin(9600);  
  Serial.println("Ready");    
}

void loop() {
  for (int i = 0; i < 10; i++) {  
    buf[i] = analogRead(SensorPin);  
    delay(10);
  }
  
  for (int i = 0; i < 9; i++) {  
    for (int j = i + 1; j < 10; j++) {  
      if (buf[i] > buf[j]) {  
        temp = buf[i];  
        buf[i] = buf[j];  
        buf[j] = temp;  
      }
    }
  }
  
  avgValue = 0;
  for (int i = 2; i < 8; i++) {  
    avgValue += buf[i];
  }
  
  voltage = (float)avgValue * 5.0 / 1024 / 6; // Convert the analog value to voltage
  pHValue = voltage * calibrationSlope + offset; // Apply calibration formula
  
  Serial.print("Voltage: ");
  Serial.print(voltage, 2);
  Serial.print(" V, pH: ");
  Serial.println(pHValue, 2);
  
  digitalWrite(13, HIGH);  
  delay(800);
  digitalWrite(13, LOW);
}

