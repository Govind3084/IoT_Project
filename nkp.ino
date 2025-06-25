#include <AltSoftSerial.h>
#include <Ethernet.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>  // Include the LCD library

// Define LCD address, change it to your specific LCD address
LiquidCrystal_I2C lcd(0x27, 16, 2);

// RS485 connection
#define RE 10
#define DE 10

const byte nitro[] = { 0x01, 0x03, 0x00, 0x1E, 0x00, 0x01, 0xE4, 0x0C };
const byte phos[] = { 0x01, 0x03, 0x00, 0x1F, 0x00, 0x01, 0xB5, 0xCC };
const byte pota[] = { 0x01, 0x03, 0x00, 0x20, 0x00, 0x01, 0x85, 0xC0 };

byte values[11];
AltSoftSerial mod;

byte val5 = 0, val6 = 0, val7 = 0;

void setup() {
  Serial.begin(9600);
  mod.begin(9600);
  pinMode(RE, OUTPUT);
  pinMode(DE, OUTPUT);

  // Initialize LCD
  lcd.begin();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Soil Nutrients");

  // Put RS-485 into receive mode
  digitalWrite(DE, LOW);
  digitalWrite(RE, LOW);

  delay(3000);
}

void loop() {
  val5 = nitrogen();
  delay(1000);
  val6 = phosphorous();
  delay(1000);
  val7 = potassium();
  delay(1000);

  // Serial output
  Serial.print("Nitrogen: "); Serial.print(val5); Serial.println(" mg/kg");
  Serial.print("Phosphorous: "); Serial.print(val6); Serial.println(" mg/kg");
  Serial.print("Potassium: "); Serial.print(val7); Serial.println(" mg/kg");
  Serial.println();

  // LCD display output
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("N: "); lcd.print(val5); lcd.print(" mg/kg");

  lcd.setCursor(0, 1);
  lcd.print("P: "); lcd.print(val6); lcd.print(" mg/kg");

  lcd.setCursor(9, 1);
  lcd.print("K: "); lcd.print(val7); lcd.print(" mg/kg");

  delay(3000);
}

byte nitrogen() {
  return readSensor(nitro);
}

byte phosphorous() {
  return readSensor(phos);
}

byte potassium() {
  return readSensor(pota);
}

byte readSensor(const byte* command) {
  // Clear the receive buffer
  mod.flushInput();

  // Switch RS-485 to transmit mode
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(1);

  // Write out the message
  for (uint8_t i = 0; i < 8; i++) mod.write(command[i]);

  // Wait for the transmission to complete
  mod.flush();

  // Switch RS-485 to receive mode
  digitalWrite(DE, LOW);
  digitalWrite(RE, LOW);

  // Delay to allow response bytes to be received
  delay(200);

  // Read in the received bytes
  for (byte i = 0; i < 7; i++) {
    values[i] = mod.read();
  }
  return values[4];
}
