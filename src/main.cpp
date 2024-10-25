#include <Arduino.h>

#define HALL_SENSOR_PIN 0
#define LED_PIN 2

void setup()
{
  pinMode(LED_PIN, OUTPUT);
  pinMode(HALL_SENSOR_PIN, INPUT);
}

void loop()
{
  int sensorValue = digitalRead(HALL_SENSOR_PIN);
  if (sensorValue == LOW)
  {
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
    delay(200);
  }
  else
  {
    digitalWrite(LED_PIN, LOW);
  }
}
