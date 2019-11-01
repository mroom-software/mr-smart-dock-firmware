// server_config.h

#ifndef _SERVER_CONFIG_h
#define _SERVER_CONFIG_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
/* Change this to your MQTT API Key from Account > MyProfile.*/
#define MQTT_API_KEY					"<API_KEY>"
/* Change to your channel write API key.*/
#define MQTT_WRITE_API_KEY				"<API_WRITE_KEY>"
#define MQTT_SERVER_ADD					"mqtt.thingspeak.com"
#define MQTT_SERVER_PORT				1883
#define MQTT_SUBCRIBE_CHANNEL			"channels/<channel_id>/subscribe/fields/<field_number>/<API_KEY>"
#define MQTT_PUBLISH_CHANNEL			"channels/<channel_id>/publish/fields/<field_number>/<API_KEY>"

#endif

