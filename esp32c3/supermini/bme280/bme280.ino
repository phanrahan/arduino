#include <WiFi.h>
#include "ArduinoJson.h"

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

Adafruit_BME280 bme; // I2C
int temperature;
int pressure;
int humidity;

const String ssid("Wifihill");
const String password("Wifihill");
const IPAddress ip(10, 0, 0, 243);
//const char* const hostname("esp32c3");

WiFiServer server(80);

void setup() {
  Serial.begin(115200);

  bool status = bme.begin();  
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }

  // Connect to WiFi.
  Serial.println("Connecting to " + ssid);
  WiFi.mode(WIFI_STA);
  WiFi.config(ip);  // Force this static IP address.
  //WiFi.setHostname(hostname);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  //Serial.print("WiFi connected. IP address: ");
  Serial.println(WiFi.localIP());

  // Initialize WiFi server.
  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (!client) {
    printDot();
  } else {
    Serial.println("\nNew Client.");

    while (client.connected() && client.available()) {
      // Get request.
      Serial.println("Read string");
      const String s = client.readString();
      Serial.print(s);

      processRequest(s);
      //respond_html(client);
      respond_json(client);
    }

    // Disconnect.
    client.stop();
    Serial.println("Client disconnected");
  }
}

void read_bme() {
  temperature = bme.readTemperature();
  pressure = bme.readPressure();
  humidity = bme.readHumidity();
}

void processRequest(const String& s) {
  read_bme();
}

void respond_html(WiFiClient& client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println("Connection: close"); // default in HTTP 1.1 is open
  client.println();
  client.println("<!DOCTYPE html>");
  client.println("<html>");
  client.println("<head>");
    client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
  client.println("</head>");
  client.println("<body>");
    client.printf("<p>Temperature %dC</p>\n", temperature);
    client.printf("<p>Pressure %dhPa</p>\n", pressure/100);  // 1 hectopascal (hPa) equals 100 Pa, which equals 1 millibar.
    client.printf("<p>Humidity %d%%</p>\n", humidity);
  client.println("</body>");
  client.println("</html>");
  client.println();  // End response with blank line.
}

void respond_json(WiFiClient& client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type: application/json");
  //client.println("Content-length: 19");
  client.println("Connection: close");
  client.println();
  //client.println("{\"success\":\"true\"}");
  StaticJsonDocument<200> data;
  data["temperature"] = temperature;
  data["pressure"] = pressure;
  data["humidity"] = humidity;
  String response;
  serializeJson(data, response);
  client.println(response);
}

void printDot() {
  static int loopCount = 0;
  delay(200);
  Serial.print(".");
  loopCount += 1;
  if (loopCount >= 80) {
    loopCount = 0;
    Serial.println("");
  }
}
