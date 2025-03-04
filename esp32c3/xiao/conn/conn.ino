#include <WiFi.h>

const char* ssid     = "Wifihill";
const char* password = "Wifihill";   
char hostname[100];

void create_hostname() {
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.printf("MAC %02x:%02x:%02x:%02x:%02x:%02x\n",
                  mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  snprintf(hostname, 100, "esp32c3-%02X%02X%02X", mac[3], mac[4], mac[5]);
}

void setup()
{
    Serial.begin(115200);
    delay(10);

    Serial.println();

    create_hostname();

    Serial.printf("Connecting to %s as %s\n", ssid, hostname);

    WiFi.mode(WIFI_STA);
    WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
    WiFi.setHostname(hostname);
    WiFi.begin(ssid, password);
    WiFi.setTxPower(WIFI_POWER_19_5dBm);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.println(WiFi.localIP());
    Serial.print("RRSI: ");
    Serial.println(WiFi.RSSI());

    Serial.println("going into deep sleep");
    ESP.deepSleep(5*1000000UL); // 5 seconds (argument is in us)
}  

void loop()
{
}
