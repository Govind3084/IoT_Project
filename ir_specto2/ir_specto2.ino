#include <math.h>

// Pin configuration      
#define LASER_PIN_650 D1      // Adjust GPIO pin for the 650nm Laser
#define PHOTODIODE_PIN A0     // ADC pin for photodiode (ESP8266 uses A0)

// Constants
#define ADC_MAX 1023          // Maximum ADC value for 10-bit resolution
#define VREF 1.0              // Reference voltage (ESP8266 ADC is 1.0V default)


float I0_phosphorus = 100.0;

void setup() {
  Serial.begin(115200);
  
  pinMode(LASER_PIN_650, OUTPUT);

  // Measure reference intensity (I0) for phosphorus
  Serial.println("Measuring reference intensity (I0)...");

  I0_phosphorus = measureIntensity(LASER_PIN_650);

  Serial.print("Reference Intensity (650nm, Phosphorus): ");
  Serial.println(I0_phosphorus, 4);
  Serial.println("Reference measurement completed.");
  delay(2000); // Pause before main loop
}

void loop() {
  Serial.println("Measuring transmitted intensity (I)...");

  // Measure transmitted intensity (I) for phosphorus
  float I_phosphorus = measureIntensity(LASER_PIN_650);

  float A_phosphorus = calculateAbsorbance(I0_phosphorus, I_phosphorus);

  Serial.print("Phosphorus (650nm): ");
  Serial.println(A_phosphorus, 4);
  Serial.println("------------------------------");

  delay(5000); // Wait for 5 seconds before the next reading
}

// Function to measure light intensity
float measureIntensity(int lightSourcePin) {
  digitalWrite(lightSourcePin, HIGH); // Turn on the light source
  delay(100);                         // Stabilize the photodiode reading
  int analogValue = analogRead(PHOTODIODE_PIN);
  float voltage = (analogValue / (float)ADC_MAX) * VREF;
  digitalWrite(lightSourcePin, LOW);  // Turn off the light source

  return voltage; // Return measured voltage as intensity
}

// Function to calculate absorbance
float calculateAbsorbance(float I0, float I) {
  if (I <= 0) I = 0.0001; // Prevent log(0) errors
  return log10(I0 / I);
}
