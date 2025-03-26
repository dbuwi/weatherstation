// ntp.h
#ifndef NTP_H
#define NTP_H

#include <Arduino.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <WiFi.h>

// Declare TaskHandle_t before using it
extern TaskHandle_t xNTPHandle;

// NTP Client setup
extern WiFiUDP ntpUDP;
extern NTPClient timeClient;

void setupNTP();
void taskNTPUpdate(void *parameter);

#endif // NTP_H
