#include <WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DS2423.h>
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>

const char* ssid = "Wifihill";
const char* password = "Wifihill";
char hostname[20];

// MQTT Broker IP address
const char* mqtt_broker = "10.0.0.38";  // fiasco static ip

WiFiClient espClient;
PubSubClient client(espClient);

// 2 m
#define INTERVAL 2*60

char *location = "minoca";
//char *room = "studio";
//char *room = "bedroom";
//char *room = "shop";
char *room = "roof";

// define the Arduino digital I/O pin to be used for the 1-Wire network here
const uint8_t ONE_WIRE_PIN = 10;

// define the 1-Wire address of the DS2423 rain gauge counter
uint8_t DS2423_address[] = { 0x1D, 0xD0, 0xE6, 0x0D, 0x00, 0x00, 0x00, 0xDC };

OneWire ow(ONE_WIRE_PIN);
DS2423 ds2423(&ow, DS2423_address);

Adafruit_BME280 bme;  // I2C

void create_hostname() {
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.printf("MAC %02x:%02x:%02x:%02x:%02x:%02x\n",
              mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  sprintf(hostname, "esp32c3-%02X%02X%02X", mac[3], mac[4], mac[5]);
}

void setup_wifi() {
  Serial.printf("Connecting to %s as %s", ssid, hostname);
  WiFi.mode(WIFI_STA);
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
  WiFi.setHostname(hostname);
  WiFi.begin(ssid, password);
  WiFi.setTxPower(WIFI_POWER_19_5dBm);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.print(" connected as ");
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

  ds2423.update();
  if (ds2423.isError()) {
    Serial.println("Error reading counter");
  } else {
    int count = ds2423.getCount(DS2423_COUNTER_B);
    //Serial.printf("Count = %d\n", count);
    publishf(location, room, "rain", count/100.0);
  }

    float temperature = bme.readTemperature();
  temperature = 1.8 * temperature + 32;  // Temperature in Fahrenheit
  publishf(location, room, "temperature", temperature);

  float pressure = bme.readPressure();
  pressure /= 100.0;
  publishf(location, room, "pressure", pressure);

  float humidity = bme.readHumidity();
  publishf(location, room, "humidity", humidity);

}

void setup_sensor() {
  ds2423.begin(DS2423_COUNTER_B);

  if (!bme.begin()) {
    Serial.println("Could not find a BME280 sensor ...");
    while (1)
      ;
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial)
    ;   

  Serial.println();

  setup_sensor();

  create_hostname();

  setup_wifi();

  setup_mqtt();

  publish_sensor();

  delay(1000); // wait for mqtt message to be sent
  
  Serial.println("going into deep sleep");
  ESP.deepSleep(INTERVAL * 1000000UL);
}

void loop() {
}
