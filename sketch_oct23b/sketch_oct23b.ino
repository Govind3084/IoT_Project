#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>

String command;  // String to store app command state.
const char* ssid = "GRP 5G";  // Wi-Fi SSID.
ESP8266WebServer server(80);

void setup() {
  
  // Serial Monitor for debugging.
  Serial.begin(115200);
  
  // Setting up Wi-Fi in Access Point (AP) mode.
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid);

  // Print the IP address of the ESP8266.
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  
  // Start Web Server.
  server.on("/", HTTP_handleRoot);  // Route for root.
  server.onNotFound(HTTP_handleRoot);  // Handle undefined routes.
  server.begin();    
}

void HTTP_handleRoot(void) {
  if (server.hasArg("State")) {  // Check if the "State" argument is present.
    command = server.arg("State");
    Serial.println(command);  // Print the command to the serial monitor.
  }
  
  // Send a basic HTML response to the web client.
  String html = "<html><body><h1>ESP8266 Web Server</h1></body></html>";
  server.send(200, "text/html", html);
}

void loop() {
  server.handleClient();  // Handle incoming client requests.
}

