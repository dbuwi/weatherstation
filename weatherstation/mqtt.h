// mqtt.h
#ifndef MQTT_H
#define MQTT_H

#include <Arduino.h>
#include <PubSubClient.h>

// Define MQTT broker details
const char* mqttServer = "broker.emqx.io";
const int mqttPort = 1883;

// MQTT Client instance
WiFiClient espClient;
PubSubClient mqtt(espClient);

// Define MQTT subscription topics
//const char* subtopic[] = { "620162321_sub","/elet2415" }; // Ensure these are defined
//const uint8_t size = sizeof(subtopic) / sizeof(subtopic[0]);

void mqttCallback(char* topic, byte* payload, unsigned int length);
void setupMQTT();
void reconnectMQTT();

#endif // MQTT_H