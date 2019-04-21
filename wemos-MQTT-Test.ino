// This code connects a Wemos device to wifi and performs an MQTT publish every few seconds
// For test purposes it increments a counter by one each loop, which starts at 1
// Replace the counter int and increment with your own values from sensors connected to the Wemos
// Tested on a Wemos D1 Mini

// Inspired by https://www.instructables.com/id/Remote-Temperature-Monitoring-Using-MQTT-and-ESP82/

// Requires Nick O'Leary's Arduino Client for MQTT
// https://github.com/knolleary/pubsubclient

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// WIFI SSID and password
const char* ssid = "YourSSIDHere";
const char* password = "secret";

// Address of MQTT Broker
const char* mqtt_server = "192.168.1.1";

// How often to publish MQTT, in milliseconds
int freq = 5000;

// MQTT topic to publish to
String topic = "mqtttesttext";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;

// This int is incremented by one for every loop so we can send a distinct message each time.
// It's for test purposes and can be removed if not in use.
int counter = 0;

void setup_wifi() {
  delay(10);
  // Connect to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.print(ssid);
  Serial.print(" ..");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected. IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("");
}

void reconnect() {
  // Loop until reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("Wemos_Client")) {
      Serial.println(" connected");
    } else {
      Serial.print(" failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
 
void setup()
{
  Serial.begin(115200);
  setup_wifi(); 
  client.setServer(mqtt_server, 1883);
  pinMode(BUILTIN_LED, OUTPUT); // Setup builtin LED
  digitalWrite(LED_BUILTIN, HIGH); // LED Off
}

void loop()
{
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > freq) {
    digitalWrite(LED_BUILTIN, LOW); // LED On
    counter++;
    lastMsg = now;
    
    Serial.print("Count: ");
    Serial.println(counter);
    Serial.print("Publishing... ");

    client.publish(topic.c_str(), String(counter).c_str());
    
    Serial.println("Done");
    digitalWrite(BUILTIN_LED, HIGH); // LED Off
  }
}
