#include <WiFi.h>

const char* ssid = "Wifihill";
const char* password = "Wifihill";
char hostname[100];

void create_hostname() {
  byte mac[6];
  WiFi.macAddress(mac);
  snprintf(hostname, 100, "esp32c3-%02X%02X%02X", mac[3], mac[4], mac[5]);
}

void setup() {
  Serial.begin(115200);
  while(!Serial) ;

  create_hostname();

  Serial.println();
  Serial.printf("Connecting to %s as %s\n", ssid, hostname);

  WiFi.mode(WIFI_STA);
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
  // WiFi.setHostname(hostname);
  WiFi.begin(ssid, password);
  //Serial.printf("Tx power %d\n", WiFi.getTxPower());
  // WIFI_POWER_15dBm, WIFI_POWER_8_5dBm, WIFI_POWER_5dBm
  WiFi.setTxPower(WIFI_POWER_19_5dBm);
  //WiFi.setTxPower(WIFI_POWER_15dBm);
  //WiFi.setTxPower(WIFI_POWER_8_5dBm);
  //WiFi.setTxPower(WIFI_POWER_5dBm);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println(WiFi.localIP());
}

void loop() {
}
