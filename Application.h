// Application.h

#ifndef _APPLICATION_h
#define _APPLICATION_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "HX711.h"
#include "config.h"
#include "server_config.h"
#include "ArduinoJson\ArduinoJson.h"
#include <ArduinoJson.hpp>
#include <Adafruit_NeoPixel.h>
#include <PubSubClient.h>
#include <EEPROM.h>
#include <Ticker.h>
#include "database.h"

typedef enum
{
	MODE_RUNNING = 0,
	MODE_CONFIG,
	MODE_ERROR,
}
running_mode_t;

void Application_initialize(void);
void Application_runInLoop(void);
void Application_ledBlink(void);
void Application_MQTT_callback(char* topic, byte* payload, unsigned int length);
#endif

