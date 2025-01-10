#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "config.h"

// MQTT Parameters
const char *mqtt_server = "192.168.1.127";
const unsigned int mqtt_port = 1883;

#define HALL_SENSOR_PIN 0
#define LED_PIN 2

unsigned int player_id = 1;
String client_ID = "Meeple-" + String(player_id);

// MQTT Topics
String meeple_hall_topic = "meeple/" + String(player_id) + "/hall_sensor";
String meeple_led_topic = "meeple/" + String(player_id) + "/led";
String meeple_debug_topic = "meeple/" + String(player_id) + "/debug";

// WiFi and MQTT Client
WiFiClient espClient;
PubSubClient client(espClient);

// Functions
void connectToWiFi();
void reconnectToMQTT();
void mqttCallback(char *topic, byte *payload, unsigned int length);
void checkHallSensor();
void debugLog(String message, bool newLine = true);
void handleLedMessage(String topic, String payload);

// Hall Sensor Parameters
const unsigned long hallSensorDebounceDelay = 100; // Debounce delay in ms
unsigned long lastHallChangeTime = 0;              // Last time the state changed
int previousHallState = HIGH;                      // Initial state (no magnet detected)

void setup()
{
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(HALL_SENSOR_PIN, INPUT);

  connectToWiFi();

  // Set up MQTT
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(mqttCallback);
}

void loop()
{
  // Check if Wi-Fi is disconnected
  if (WiFi.status() != WL_CONNECTED)
  {
    debugLog("Wi-Fi disconnected! Current status: " + String(WiFi.status()));
    debugLog("Reconnecting to Wi-Fi...");
    connectToWiFi();
  }

  // Only try to reconnect if we're actually disconnected
  if (!client.connected())
  {
    debugLog("MQTT disconnected! Current state: " + String(client.state()));
    debugLog("Reconnecting to MQTT...");
    reconnectToMQTT();
  }

  // Handle MQTT messages
  client.loop();

  // Check hall sensor state
  checkHallSensor();

  // Small delay to prevent too frequent checking
  delay(100);
}

// Connect to Wi-Fi
void connectToWiFi()
{
  Serial.print("Connecting to WiFi ..");
  WiFi.begin(ssid, password);

  unsigned long startAttemptTime = millis();
  const unsigned long wifiTimeout = 10000;

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < wifiTimeout)
  {
    Serial.print('.');
    delay(1000);
  }

  debugLog("\nConnected to Wi-Fi");
}

// Reconnect to MQTT
void reconnectToMQTT()
{
  if (!client.connected())
  { // Add this check
    debugLog("Connecting to MQTT...", false);
    if (client.connect(client_ID.c_str()))
    {
      debugLog("Connected");
      client.subscribe(meeple_led_topic.c_str());
    }
    else
    {
      debugLog("Failed, rc=" + String(client.state()) + " Trying again in 5 seconds...");
      delay(5000);
    }
  }
}

// Handle MQTT Messages
void mqttCallback(char *topic, byte *payload, unsigned int length)
{
  // Convert payload to String
  String payloadStr = "";
  for (unsigned int i = 0; i < length; i++)
  {
    payloadStr += (char)payload[i];
  }

  String topicStr = String(topic);

  // Debug message
  debugLog("Message arrived on topic: " + String(topic) + ". Message: " + payloadStr);

  // Check topic
  if (topicStr.equals(meeple_led_topic))
    handleLedMessage(topicStr, payloadStr);
  else
    debugLog("Unknown topic: " + topicStr);
}

void handleLedMessage(String topic, String payload)
{
  // Only accept "0" or "1" as valid inputs
  if (payload != "0" && payload != "1")
  {
    debugLog("Invalid LED state: " + payload);
    return;
  }

  // Convert payload to int (now safe since we validated the input)
  int ledState = atoi(payload.c_str());
  digitalWrite(LED_PIN, ledState);
  debugLog("LED state set to: " + String(ledState));
}

// Check Hall Sensor
void checkHallSensor()
{
  int currentHallState = digitalRead(HALL_SENSOR_PIN);
  debugLog("Hall sensor state: " + String(currentHallState));

  // Check if the state has changed
  if (currentHallState != previousHallState)
  {
    unsigned long currentTime = millis();

    // Debounce: Only act if enough time has passed since the last change
    if (currentTime - lastHallChangeTime > hallSensorDebounceDelay)
    {
      lastHallChangeTime = currentTime;

      if (currentHallState == LOW)
      {
        debugLog("Magnet detected: Meeple has moved to a new position");
        client.publish(meeple_hall_topic.c_str(), "1"); // Notify movement
      }
      else if (currentHallState == HIGH)
      {
        debugLog("No magnet detected: Meeple is in transit");
        client.publish(meeple_hall_topic.c_str(), "0"); // Optional, notify leaving position
      }

      // Update the previous state
      previousHallState = currentHallState;
    }
  }
}

void debugLog(String message, bool newLine)
{
  if (newLine)
    Serial.println(message);
  else
    Serial.print(message);

  if (client.connected())
    client.publish(meeple_debug_topic.c_str(), message.c_str());
}