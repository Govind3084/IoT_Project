#include <math.h>

// Pin configuration      
#define LASER_PIN_650 26      // GPIO pin for 650nm Laser
#define PHOTODIODE_PIN 36     // ADC pin for photodiode (VP)

// Constants
#define ADC_MAX 4095          // Maximum ADC value for 12-bit resolution
#define VREF 3.3              // Reference voltage (ESP32 ADC is 3.3V)


float I0_phosphorus = 100.0;

void setup() {
  Serial.begin(115200);
  
  
  pinMode(LASER_PIN_650, OUTPUT);

  // Measure reference intensity (I0) for both wavelengths
  Serial.println("Measuring reference intensities (I0)...");
 
  I0_phosphorus = measureIntensity(LASER_PIN_650);

 
  Serial.print("Reference Intensity (650nm, Phosphorus): ");
  Serial.println(I0_phosphorus, 4);
  Serial.println("Reference measurement completed.");
  delay(2000); // Pause before main loop
}

void loop() {
  Serial.println("Measuring transmitted intensities (I)...");

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
  // Turn on the light source
  digitalWrite(lightSourcePin, HIGH);
 // Read photodiode value
  int analogValue = analogRead(PHOTODIODE_PIN);
  float voltage = (analogValue / (float)ADC_MAX) * VREF;


  // Return measured voltage as intensity
  return voltage;
}

// Function to calculate absorbance
float calculateAbsorbance(float I0, float I) {
  if (I <= 0) I = 0.0001; // Prevent log(0) errors
  return log10(I0 / I);
}