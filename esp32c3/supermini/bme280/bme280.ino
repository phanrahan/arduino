#include <WiFi.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; // I2C

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
      respond(client);
    }

    // Disconnect.
    client.stop();
    Serial.println("Client disconnected");
  }
}

void processRequest(const String& s) {
}

void respond(WiFiClient& client) {
  int temperature = bme.readTemperature();
  int pressure = bme.readPressure();
  int humidity = bme.readHumidity();
  int altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println("Connection: close");
  client.println();
  client.println("<!DOCTYPE html>");
  client.println("<html>");
  client.println("<head>");
    client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
  client.println("</head>");
  client.println("<body>");
    client.println("<p>Temperature ");
      client.print(temperature);
      client.println("C");
    client.println("</p>");
    client.println("<p>Pressure ");
      client.print(pressure);
      client.println("Pa");
    client.println("</p>");
        client.println("<p>Humidity ");
      client.print(humidity);
      client.println("%");
    client.println("</p>");
    client.println("</p>");
        client.println("<p>Altitude ");
      client.print(altitude);
      client.println("m");
    client.println("</p>");
  client.println("</body>");
  client.println("</html>");
  client.println();  // End response with blank line.
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
