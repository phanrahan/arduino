#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>

const char* ssid = "Wifihill";
const char* password = "Wifihill";
char hostname[20];

// MQTT Broker IP address
const char* mqtt_broker = "10.0.0.38"; // fiasco static ip

WiFiClient espClient;
PubSubClient client(espClient);

int lastMsg;
#define INTERVAL 10000

Adafruit_BME280 bme; // I2C


void create_hostname() {
  byte mac[6];
  WiFi.macAddress(mac);
  sprintf(hostname, "esp32c3-%02X%02X%02X", mac[3], mac[4], mac[5]);
  Serial.printf("Hostname %s\n", hostname);
}

void setup() {
  Serial.begin(115200);

  if (!bme.begin(0x77)) {
    Serial.println("Could not find a valid BME280 sensor ...");
    while (1);
  }

  create_hostname();

  setup_wifi();

  setup_mqtt();
}

void setup_wifi() {
  delay(10);
  
  Serial.printf("Connecting to %s\n", ssid);
  WiFi.mode(WIFI_STA);
  WiFi.setHostname(hostname);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup_mqtt() {
  client.setServer(mqtt_broker, 1883);
}

void connect_mqtt() {
  while (!client.connected()) {
    Serial.printf("Connecting to MQTT broker %s as client %s ...", mqtt_broker, hostname);
    if (client.connect(hostname)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void publishf(char *topic, float value) {
    char buf[8];
    dtostrf(value, 1, 2, buf);
    Serial.printf("%s %s\n", topic, buf);
    client.publish(topic, buf);
}

void loop() {
  connect_mqtt();
  client.loop();

  long now = millis();
  if (now - lastMsg > INTERVAL) {
    lastMsg = now;
    
    float temperature = bme.readTemperature();   
    temperature = 1.8 * temperature + 32; // Temperature in Fahrenheit
    publishf("exp32/temperature", temperature);

    float humidity = bme.readHumidity();
    publishf("exp32/hunidity", humidity);
  }
}
