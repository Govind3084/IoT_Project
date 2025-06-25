#define ENA   14          // Enable/speed motors Right        GPIO14(D5)
#define ENB   12          // Enable/speed motors Left         GPIO12(D6)
#define IN_1  15          // L298N in1 motors Right           GPIO15(D8)
#define IN_2  13          // L298N in2 motors Right           GPIO13(D7)
#define IN_3  2           // L298N in3 motors Left            GPIO2(D4)
#define IN_4  0           // L298N in4 motors Left            GPIO0(D3)

#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <DHT.h>
#include <SoftwareSerial.h>

// DHT11 sensor
#define DHTPIN 5         // DHT11 connected to GPIO5 (D1)
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Ultrasonic sensor
#define TRIG_PIN 4       // Trigger pin for ultrasonic sensor (D2)
#define ECHO_PIN 16      // Echo pin for ultrasonic sensor (D0)

// GPS module (GY-NEO6MV2)
SoftwareSerial gpsSerial(2, 3); // RX, TX (D4, D3)

String command;             // String to store app command state
int speedCar = 800;         // 400 - 1023
int speed_Coeff = 3;

const char* ssid = "esp-12";
const char* password = "1111111"; 
ESP8266WebServer server(80);

void setup() {
    pinMode(ENA, OUTPUT);
    pinMode(ENB, OUTPUT);  
    pinMode(IN_1, OUTPUT);
    pinMode(IN_2, OUTPUT);
    pinMode(IN_3, OUTPUT);
    pinMode(IN_4, OUTPUT); 
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
  
    Serial.begin(115200);
    gpsSerial.begin(9600); // GPS module
    dht.begin();           // Initialize DHT11 sensor
  
    // Connecting WiFi
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid);
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
  
    // Starting WEB-server 
    server.on("/", HTTP_handleRoot);
    server.onNotFound(HTTP_handleRoot);
    server.begin();    
}

float getTemperature() {
    return dht.readTemperature();
}

float getHumidity() {
    return dht.readHumidity();
}

long getDistance() {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    long duration = pulseIn(ECHO_PIN, HIGH);
    return duration * 0.034 / 2; // Distance in cm
}

void getGPSData() {
    while (gpsSerial.available()) {
        Serial.write(gpsSerial.read());
    }
}

void goAhead() {
    digitalWrite(IN_1, LOW);
    digitalWrite(IN_2, HIGH);
    analogWrite(ENA, speedCar);
    digitalWrite(IN_3, LOW);
    digitalWrite(IN_4, HIGH);
    analogWrite(ENB, speedCar);
}

void stopRobot() {  
    digitalWrite(IN_1, LOW);
    digitalWrite(IN_2, LOW);
    analogWrite(ENA, 0);  // Stop motor on Right

    digitalWrite(IN_3, LOW);
    digitalWrite(IN_4, LOW);
    analogWrite(ENB, 0);  // Stop motor on Left
}

void loop() {
    server.handleClient();
    command = server.arg("State");
    if (command == "F") goAhead();
    // Other commands as in original code...
    else if (command == "S") stopRobot();
  
    // Display sensor data in Serial Monitor
    Serial.print("Temperature: ");
    Serial.print(getTemperature());
    Serial.print(" C, Humidity: ");
    Serial.print(getHumidity());
    Serial.print(" %, Distance: ");
    Serial.print(getDistance());
    Serial.println(" cm");
  
    // GPS data
    getGPSData();
    delay(1000); // Adjust the delay as needed
}

void HTTP_handleRoot() {
    if (server.hasArg("State")) {
        Serial.println(server.arg("State"));
    }
    server.send(200, "text/html", "");
    delay(1);
}
