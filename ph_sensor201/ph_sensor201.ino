#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// Wi-Fi credentials
const char* ssid = "STIC";          // Replace with your Wi-Fi SSID
const char* password = "stic@123";  // Replace with your Wi-Fi password

// ESP8266 Web Server on port 80
ESP8266WebServer server(80);

// Analog pin connected to the pH sensor
#define PH_SENSOR_PIN A0

// Calibration values
float pHCalibrationOffset = 0.0; // Adjust after calibration if necessary

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Configure the web server
  server.on("/", handleRoot); // Route for root
  server.begin();
  Serial.println("Web server started.");
}

void loop() {
  server.handleClient(); // Handle incoming client requests
}

// Function to handle the root URL
void handleRoot() {
  int sensorValue = analogRead(PH_SENSOR_PIN);
  float voltage = sensorValue * (3.3 / 1023.0); // Convert ADC value to voltage
  float pH = calculatePH(voltage);

  // Create a web page to display pH value
  String html = "<html><body>";
  html += "<h1>ESP8266 pH Sensor</h1>";
  html += "<p>pH Value: " + String(pH) + "</p>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}

// Function to calculate pH from voltage
float calculatePH(float voltage) {
  float pH = 7.0 + ((2.5 - voltage) / 0.18); // Example formula for calibration
  return pH + pHCalibrationOffset;
}

