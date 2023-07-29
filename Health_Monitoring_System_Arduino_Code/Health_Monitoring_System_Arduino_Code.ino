#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// Replace with your Wi-Fi credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Replace with your healthcare professional's server address
const char* serverAddress = "HEALTHCARE_SERVER_ADDRESS";
const int serverPort = 80;

Adafruit_BME280 bme;
const int pulsePin = A0;

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  
  if (!bme.begin(0x76)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
}

void loop() {
  float temperature = bme.readTemperature();
  float pressure = bme.readPressure() / 100.0F;
  int heartbeat = analogRead(pulsePin);

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C, Pressure: ");
  Serial.print(pressure);
  Serial.print(" hPa, Heartbeat: ");
  Serial.println(heartbeat);

  sendDataToServer(temperature, pressure, heartbeat);

  delay(5000); // Adjust the delay as needed for your application
}

void sendDataToServer(float temperature, float pressure, int heartbeat) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client; // Create a WiFiClient object

    HTTPClient http;
    http.begin(client, "http://" + String(serverAddress) + ":" + String(serverPort) + "/update");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String postData = "temperature=" + String(temperature) +
                      "&pressure=" + String(pressure) +
                      "&heartbeat=" + String(heartbeat);

    int httpCode = http.POST(postData);
    if (httpCode > 0) {
      Serial.printf("HTTP POST request sent with status code: %d\n", httpCode);
    } else {
      Serial.println("HTTP POST request failed.");
    }

    http.end();
  } else {
    Serial.println("WiFi connection lost. Data not sent.");
  }
}
