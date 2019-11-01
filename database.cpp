// 
// 
// 

#include "database.h"

void database_init(void)
{
	EEPROM.begin(1024);
}
bool database_readSSID(char *ssid)
{
	char tmp[64] = { 0 };
	EEPROM.get(SSID_EEPROM_ADD, tmp);
	memcpy(ssid, &tmp[0], sizeof(tmp));
	return true;
}

bool database_writeSSID(const char *ssid)
{
	char tmp[64] = { 0 };
	memcpy(&tmp[0], ssid, sizeof(tmp));
	EEPROM.put(SSID_EEPROM_ADD, tmp);
	return EEPROM.commit();
}

bool database_readPSK(char *psk)
{
	char tmp[64] = { 0 };
	EEPROM.get(PSK_EEPROM_ADD, tmp);
	memcpy(psk, &tmp[0], sizeof(tmp));
	return true;
}

bool database_writePSK(const char *psk)
{
	char tmp[64] = { 0 };
	memcpy(&tmp[0], psk, sizeof(tmp));
	EEPROM.put(PSK_EEPROM_ADD, tmp);
	return EEPROM.commit();
}

bool database_readMQTTUPassword(char *pass)
{
	char tmp[32] = { 0 };
	EEPROM.get(MQTT_PASSWORD_ADD, tmp);
	memcpy(pass, &tmp[0], sizeof(tmp));
	return true;
}

bool database_writeMQTTUPassword(const char *pass)
{
	char tmp[32] = { 0 };
	memcpy(&tmp[0], pass, sizeof(tmp));
	EEPROM.put(MQTT_PASSWORD_ADD, tmp);
	return EEPROM.commit();
}

bool database_readMQTTAPIKey(char *key)
{
	char tmp[32] = { 0 };
	EEPROM.get(MQTT_API_KEY_ADD, tmp);
	memcpy(key, &tmp[0], sizeof(tmp));
	return true;
}

bool database_writeMQTTAPIKey(const char *key)
{
	char tmp[32] = { 0 };
	memcpy(&tmp[0], key, sizeof(tmp));
	EEPROM.put(MQTT_API_KEY_ADD, tmp);
	return EEPROM.commit();
}

bool database_readMQTT_IDChannel(char *channel)
{
	char tmp[16] = { 0 };
	EEPROM.get(MQTT_ID_CHANNEL_ADD, tmp);
	memcpy(channel, &tmp[0], sizeof(tmp));
	return true;
}

bool database_writeMQTT_IDChannel(const char *channel)
{
	char tmp[16] = { 0 };
	memcpy(&tmp[0], channel, sizeof(tmp));
	EEPROM.put(MQTT_ID_CHANNEL_ADD, tmp);
	return EEPROM.commit();
}

uint32_t database_readWeightOffset(void)
{
	uint32_t tmp = 0;
	tmp = EEPROM.read(WEIGHT_OFFSET);
	tmp <<= 8;
	tmp |= EEPROM.read(WEIGHT_OFFSET + 1);
	tmp <<= 8;
	tmp |= EEPROM.read(WEIGHT_OFFSET + 2);
	tmp <<= 8;
	tmp |= EEPROM.read(WEIGHT_OFFSET + 3);
	return tmp;
}

bool database_writeWeightOffset(uint32_t weight)
{
	EEPROM.write(WEIGHT_OFFSET, weight >> 24);
	EEPROM.write(WEIGHT_OFFSET + 1, weight >> 16);
	EEPROM.write(WEIGHT_OFFSET + 2, weight >> 8);
	EEPROM.write(WEIGHT_OFFSET + 3, weight);
	return EEPROM.commit();
}

