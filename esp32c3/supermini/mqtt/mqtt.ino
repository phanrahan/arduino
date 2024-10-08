#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>

const char* ssid = "Wifihill";
const char* password = "Wifihill";
char hostname[20];

// MQTT Broker IP address
const char* mqtt_broker = "10.0.0.38";  // fiasco static ip

WiFiClient espClient;
PubSubClient client(espClient);

// 10 m
#define INTERVAL 10*60

char *location = "minoca";
char *room = "studio";

Adafruit_BME280 bme;  // I2C

void create_hostname() {
  byte mac[6];
  WiFi.macAddress(mac);
  sprintf(hostname, "esp32c3-%02X%02X%02X", mac[3], mac[4], mac[5]);
  Serial.printf("Hostname %s\n", hostname);
}

void setup() {
  Serial.begin(115200);

  setup_sensor();

  create_hostname();

  setup_wifi();

  setup_mqtt();

  publish_sensor();

  delay(1000); // wait for mqtt message to be sent
  
  Serial.println("going into deep sleep");
  ESP.deepSleep(INTERVAL * 1000000);
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

void publishf(char* location, char *room, char *sensor, float value) {
  char topic[100];
  char buf[8];
  dtostrf(value, 1, 2, buf);
  sprintf(topic, "%s/%s/%s", location, room, sensor);
  Serial.printf("%s %s\n", topic, buf);
  client.publish(topic, buf);
}

void publish_sensor() {
  connect_mqtt();
  client.loop();

  float temperature = bme.readTemperature();
  temperature = 1.8 * temperature + 32;  // Temperature in Fahrenheit
  publishf(location, room, "temperature", temperature);

  float humidity = bme.readHumidity();
  publishf(location, room, "humidity", humidity);
}

void setup_sensor() {
  if (!bme.begin(0x77)) {
    Serial.println("Could not find a valid BME280 sensor ...");
    while (1)
      ;
  }
}

void loop() {
}
