#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Update these with values suitable for your network.

const char* wifi_ssid     = "...";
const char* wifi_password = "...";

const char* mqtt_server   = "...";
const int   mqtt_port     = 1886;
const char* mqtt_username = "...";
const char* mqtt_password = "...";
const char* mqtt_node     = "...";

const int   temp_pin      = 2;

WiFiClient espClient;
PubSubClient client(espClient);
OneWire oneWire(temp_pin);
DallasTemperature DS18B20(&oneWire);

long lastMsg = 0;
char msg[50];
int value = 0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void loop() {
  float temp;
  char* tempStr;
  
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 60000) {
    lastMsg = now;
    ++value;
    DS18B20.requestTemperatures();
    temp = DS18B20.getTempCByIndex(0);
    dtostrf(temp, 5, 2, tempStr);
    
    Serial.print("Publish message: ");
    Serial.println(temp);
    client.publish("outTopic", tempStr);
  }
}
