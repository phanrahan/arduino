#include <WiFi.h>

const IPAddress ip(10, 0, 0, 40);
const String ssid("Wifihill");
const String password("Wifihill");
const char* const hostname("esp32c3");

WiFiServer server(80);

//const int pinIn = D3;
const int pinOut = D10;
bool outValue;

void setup() {
  Serial.begin(115200);

  // Initialize pins.
  //pinMode(pinIn, INPUT);
  pinMode(pinOut, OUTPUT);
  digitalWrite(pinOut, LOW);
  outValue = false;

  // Connect to WiFi.
  Serial.println("Connecting to " + ssid + " as " + hostname);
  WiFi.mode(WIFI_STA);
  WiFi.config(ip);  // Force this static IP address.
  WiFi.setHostname(hostname);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("WiFi connected. IP address: ");
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
  if (s.indexOf("GET /turnoff") >= 0) {
    digitalWrite(pinOut, LOW);
    outValue = false;
  } else if (s.indexOf("GET /turnon") >= 0) {
    digitalWrite(pinOut, HIGH);
    outValue = true;
  }
}

void respond(WiFiClient& client) {
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
    client.println("<p>");
      client.println(hostname);
    client.println("</p>");
    client.println("<p>LED (D10) is ");
      client.println(outValue ? "on" : "off");
    client.println("</p>");
    client.println("<p><a href=\"/turnoff\">Turn LED off.</a></p>");
    client.println("<p><a href=\"/turnon\">Turn LED on.</a></p>");
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
