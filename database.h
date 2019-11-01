// database.h

#ifndef _DATABASE_h
#define _DATABASE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include <EEPROM.h>

#define SSID_EEPROM_ADD			0
#define PSK_EEPROM_ADD			64
#define MQTT_PASSWORD_ADD		128
#define MQTT_API_KEY_ADD		160
#define MQTT_ID_CHANNEL_ADD		192
#define WEIGHT_OFFSET			208

void database_init(void);
bool database_readSSID(char *ssid);
bool database_writeSSID(const char *ssid);

bool database_readPSK(char *psk);
bool database_writePSK(const char *psk);

bool database_readMQTTUPassword(char *pass);
bool database_writeMQTTUPassword(const char *pass);

bool database_readMQTTAPIKey(char *key);
bool database_writeMQTTAPIKey(const char *key);

bool database_readMQTT_IDChannel(char *channel);
bool database_writeMQTT_IDChannel(const char *channel);

uint32_t database_readWeightOffset(void);
bool database_writeWeightOffset(uint32_t weight);

#endif

