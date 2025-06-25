#include <WiFi.h>
#include <WebSocketsClient.h>  // Include WebSocket library
#include <HTTPClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// WiFi credentials
const char* ssid = "Block-G";
const char* password = "Rise@2023";

// Firebase Firestore details (optional if you need to send data directly to Firebase from the server)
const char* apiKey = "AIzaSyBiVWzxSgMjib_BxqCBiycbh9x9YFQ0uZw";
const char* projectID = "hosting-c74b1";

// OLED display configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// WebSocket server details
const char* websocket_server = "ws://192.168.118.17:8080"; // Replace with your WebSocket server URL
WebSockets webSocket;

// Variables for sensors
float calibration_value = 21.34 - 0.7;
int buffer_arr[10], temp;
unsigned long int avgval;
float ph_act;

const int soil_sensor_pin = 34; // Soil moisture sensor pin
const int ph_sensor_pin = 36;   // pH sensor pin

// Define photodiode pins for N, P, and K
const int photodiode1 = 34; // Photodiode 1 for N (Nitrogen)
const int photodiode2 = 35; // Photodiode 2 for P (Phosphorus)
const int photodiode3 = 39; // Photodiode 3 for K (Potassium)

const int redLed = 15;    // Red LED connected to GPIO 15
const int yellowLed = 16; // Yellow LED connected to GPIO 16
const int greenLed = 4;   // Green LED connected to GPIO 4

void setup() {
  Serial.begin(115200);

  // Initialize WiFi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");

  // Initialize WebSocket
  webSocket.begin(websocket_server);
  webSocket.onEvent(webSocketEvent);

  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    while (true);
  }
  display.clearDisplay();
  display.display();

  // Initialize LED pins as outputs
  pinMode(redLed, OUTPUT);
  pinMode(yellowLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
}

void loop() {
  webSocket.loop();  // Keep WebSocket connection alive
  calculate_pH();
  int moisture = getSoilMoisture();

  // Simulate photodiode readings and generate random values for N, P, K
  float nitrogen = generateRandomValue(0.10, 0.19);  // N (Nitrogen) between 0.10 to 0.19
  float phosphorus = generateRandomValue(0.60, 0.70); // P (Phosphorus) between 0.60 to 0.70
  float potassium = generateRandomValue(0.01, 0.10);  // K (Potassium) between 0.01 to 0.10

  // Display data on OLED
  display_data(moisture, nitrogen, phosphorus, potassium);

  // Send data to servers
  sendDataToServer(ph_act, moisture, nitrogen, phosphorus, potassium);
  sendDataToWebSocket(ph_act, moisture, nitrogen, phosphorus, potassium);

  delay(5000); // Send data every 5 seconds
}

void calculate_pH() {
  for (int i = 0; i < 10; i++) {
    buffer_arr[i] = analogRead(ph_sensor_pin);
    delay(30);
  }

  // Sort the buffer array
  for (int i = 0; i < 9; i++) {
    for (int j = i + 1; j < 10; j++) {
      if (buffer_arr[i] > buffer_arr[j]) {
        temp = buffer_arr[i];
        buffer_arr[i] = buffer_arr[j];
        buffer_arr[j] = temp;
      }
    }
  }

  avgval = 0;
  for (int i = 2; i < 8; i++) // Exclude outliers
    avgval += buffer_arr[i];

  float volt = (float)avgval * 3.3 / 1023 / 6; // Use 3.3V reference
  ph_act = -5.70 * volt + calibration_value;

  Serial.print("pH Value: ");
  Serial.println(ph_act);
}

int getSoilMoisture() {
  int sensor_analog = analogRead(soil_sensor_pin);
  int moisture_percentage = (sensor_analog / 4095.0) * 100;
  moisture_percentage = 100 - moisture_percentage; // Invert if necessary
  Serial.print("Moisture Percentage = ");
  Serial.print(moisture_percentage);
  Serial.println("%");
  return moisture_percentage;
}

float generateRandomValue(float min, float max) {
  return min + (max - min) * (rand() / (float)RAND_MAX); // Generate random float within range
}

void display_data(int moisture, float nitrogen, float phosphorus, float potassium) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);

  // Display pH value
  display.setCursor(10, 0);
  display.print("pH: ");
  display.println(ph_act, 2);

  // Display moisture percentage
  display.setCursor(10, 30);
  display.print("Moisture: ");
  display.print(moisture);
  display.println("%");

  // Display Nitrogen, Phosphorus, Potassium values
  display.setCursor(10, 50);
  display.print("N: ");
  display.print(nitrogen, 2);
  display.print(" P: ");
  display.print(phosphorus, 2);
  display.print(" K: ");
  display.println(potassium, 2);

  display.display();
}

void sendDataToServer(float ph, int moisture, float nitrogen, float phosphorus, float potassium) {
  if (WiFi.status() == WL_CONNECTED) {
    String jsonPayload = "{\"ph\": " + String(ph, 2) + ", \"moisture\": " + String(moisture) + 
                         ", \"nitrogen\": " + String(nitrogen, 2) + ", \"phosphorus\": " + String(phosphorus, 2) +
                         ", \"potassium\": " + String(potassium, 2) + "}";

    // Send data to the ESP32 server
    sendHttpPost("http://192.168.118.17:3000/api/sensor-data", jsonPayload);

    // Send data to the ngrok server
    sendHttpPost("https://8554-49-37-215-65.ngrok-free.app/", jsonPayload);
  } else {
    Serial.println("WiFi not connected");
  }
}

void sendHttpPost(String serverUrl, String jsonPayload) {
  HTTPClient http;

  http.begin(serverUrl);
  http.addHeader("Content-Type", "application/json");

  int httpResponseCode = http.POST(jsonPayload);

  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println("POST to " + serverUrl);
    Serial.println("Response Code: " + String(httpResponseCode));
    Serial.println("Response: " + response);
  } else {
    Serial.println("Error sending POST to " + serverUrl + ": " + String(http.errorToString(httpResponseCode).c_str()));
  }
  http.end();
}

void sendDataToWebSocket(float ph, int moisture, float nitrogen, float phosphorus, float potassium) {
  if (WiFi.status() == WL_CONNECTED) {
    String jsonPayload = "{\"ph\": " + String(ph, 2) + ", \"moisture\": " + String(moisture) + 
                         ", \"nitrogen\": " + String(nitrogen, 2) + ", \"phosphorus\": " + String(phosphorus, 2) +
                         ", \"potassium\": " + String(potassium, 2) + "}";

    // Send JSON payload to WebSocket server
    webSocket.sendTXT(jsonPayload);
  } else {
    Serial.println("WiFi not connected");
  }
}

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.println("Disconnected from WebSocket");
      break;
    case WStype_CONNECTED:
      Serial.println("Connected to WebSocket");
      break;
    case WStype_TEXT:
      Serial.printf("Received: %s\n", payload);
      break;
    default:
      break;
  }
}