#include <HardwareSerial.h>
#include <WiFi.h>
#include "ArduinoJson.h"

#define ESP32
#include <SDS011.h>

const String ssid("Wifihill");
const String password("Wifihill");
const IPAddress ip(10, 0, 0, 244);

WiFiServer server(80);

HardwareSerial uart0(0);

SDS011 sds;
float p10, p25;

void setup() {
  Serial.begin(115200);

  sds.begin(&uart0);
  //sds.begin(&uart0, RX, TX);
  //sds.wakeup();
  //sds.continuous_mode();

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

void processRequest(const String& s) {
	int err = sds.read(&p25, &p10);
  if( err )
    Serial.println("SDS011 read error");
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
    client.printf("<p>p10 %fC</p>\n", p10);
    client.printf("<p>p25 %fC</p>\n", p25);
  client.println("</body>");
  client.println("</html>");
  client.println();  // End response with blank line.
}

void respond_json(WiFiClient& client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type: application/json");
  client.println("Connection: close");
  client.println();
  StaticJsonDocument<200> data;
  data["p10"] = p10;
  data["p25"] = p25;
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
