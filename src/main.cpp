#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "config.h"

void mqttCallback(char *topic, byte *payload, unsigned int length);

#define HALL_SENSOR_PIN 0
#define LED_PIN 2

// Wi-Fi credentials
// in config.h

// MQTT broker information
const char *mqttServer = "80.102.7.215";
const int mqttPort = 1883;

unsigned int player_id = 1;
String client_ID = "Meeple-" + String(player_id);

// MQTT Topics
String meeple_hall_topic = "meeple/" + String(player_id) + "/hall_sensor";
String meeple_led_topic = "meeple/" + String(player_id) + "/led";
String meeple_debug_topic = "meeple/" + String(player_id) + "/debug";

WiFiClient espClient;
PubSubClient client(espClient);

// Hall sensor pin and movement logic variables
bool isAboveMagnet = false;

void connectToWiFi() {
  Serial.print("Connecting to Wi-Fi");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  digitalWrite(LED_PIN, HIGH);
  delay(1000);
  digitalWrite(LED_PIN, LOW);
}

void connectToMQTT() {
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect(client_ID.c_str())) {
      Serial.println("Connected to MQTT");
      client.publish(meeple_debug_topic.c_str(), "Connected!");
      client.subscribe(meeple_led_topic.c_str());
    }
    else {
      Serial.print("Failed to connect. State: ");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(HALL_SENSOR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);

  connectToWiFi();
  client.setServer(mqttServer, mqttPort);
  client.setCallback(mqttCallback);
  connectToMQTT();
}

void loop() {
  if (!client.connected()) {
    connectToMQTT();
  }
  client.loop();

  // Read the hall sensor value
  bool currentAboveMagnet = digitalRead(HALL_SENSOR_PIN);

  // Check if the player moved from one magnet to another
  if (currentAboveMagnet != isAboveMagnet) {
    if (currentAboveMagnet) {
      Serial.println("Has moved");
      client.publish(meeple_hall_topic.c_str(), "1");
    }
    isAboveMagnet = currentAboveMagnet;
  }

  delay(500);  // Add delay to debounce
}


void mqttCallback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);

  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if (String(topic) == meeple_led_topic) {
    if ((char)payload[0] == '1') {
      digitalWrite(LED_PIN, HIGH);
    }
    else {
      digitalWrite(LED_PIN, LOW);
    }
  }
}
