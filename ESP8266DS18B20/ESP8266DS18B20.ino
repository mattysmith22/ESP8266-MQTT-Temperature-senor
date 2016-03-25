#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Update these with values suitable for your network.

const char* wifi_ssid     = "..."; // The name of the wifi
const char* wifi_password = "..."; // The password of the wifi

const char* mqtt_server   = "..."; // The IP for the server
const int   mqtt_port     = 1886 ; // The port for the server
const char* mqtt_username = "..."; // The username for the server
const char* mqtt_password = "..."; // The password for the server
const char* mqtt_node     = "..."; // Where to mount the temperature sensor

const int   misc_sens_pin = 4;     // The GPIO pin that the temperature sensor is attatched to
const bool  misc_debug    = true;  // Whether to output the debugging info to serial
const int   mqtt_interval = 60000; // Time (milliseconds) between polls

// Declarations

WiFiClient espClient;
PubSubClient client(espClient);
OneWire oneWire(misc_sens_pin);
DallasTemperature DS18B20(&oneWire);

long lastMsg = 0;
char msg[50];
int value = 0;

void debug(String message)
{
  if(misc_debug)
  {
    Serial.print(message);
  }
}

void debugln(String message)
{
  if(misc_debug)
  {
    Serial.println(message);
  }
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  debugln("");
  debug("Connecting to ");
  debugln(wifi_ssid);

  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    debug(".");
  }

  debugln("");
  debugln("WiFi connected");
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    debug("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      debugln("connected");
    } else {
      debug("failed, rc=");
      debug((char*)client.state());
      debugln(" try again in 5 seconds");
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
  char tempStr[5];
  
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > mqtt_interval) {
    lastMsg = now;
    ++value;
    DS18B20.requestTemperatures();
    temp = DS18B20.getTempCByIndex(0); 

    debug("Publish message: ");
    String(temp).toCharArray(tempStr, 2);
    debugln((String)temp);
    String(temp).toCharArray(tempStr, 5);
    client.publish("outTopic", tempStr);
    debug("Done");
  }
}
