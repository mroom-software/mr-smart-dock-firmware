// 
// 
// 

#include "Application.h"

/*Objects init*/
WiFiClient socketServer;
WiFiServer server(LOCAL_SERVER_PORT);
HX711 scale;
Adafruit_NeoPixel pixel = Adafruit_NeoPixel(1, LED_RGB_PIN, NEO_GRB + NEO_KHZ800);
Ticker ticker;

int lenRead = 0;
char bufferReceive[128];
char ssid[64];
char psk[64];
running_mode_t mode = MODE_RUNNING;
uint32_t buttonTimePress = 0;
uint32_t configTimeout = 0;
bool apConfigChange = false;
uint32_t timeReadLoadcell = 0;
float calibration_factor = 2230.0;
float loadcellUnits = 0.0;
uint32_t weight = 0;
uint32_t lastWeight = 0;
uint32_t weightOffset = 0;
uint32_t timeCheckInternetConnection = 0;
bool wifiConnected = false;

/*Variables for MQTT client*/
WiFiClient espClient;
PubSubClient client(espClient);
bool initMQTTClient = false;
char mqttUserName[] = "myuser";
char mqttPass[32] = MQTT_API_KEY;
char writeAPIKey[32] = MQTT_WRITE_API_KEY;
char idchannel[16];
char publishChannel[128] = MQTT_PUBLISH_CHANNEL;
char dataToPublish[12];
bool mqttConnected = false;
uint32_t timePublishData = 0;
/*For command line*/
bool forceSend = false;
bool forceConfigMode = false;

bool weightChanged = false;

bool cupRelease = false;

void Application_initialize(void)
{
	delay(1000);
	LOG.begin(115200);
	LOG.setTimeout(100);
	database_init();
	database_readSSID(&ssid[0]);
	database_readPSK(&psk[0]);
	database_readMQTT_IDChannel(&idchannel[0]);
	weightOffset = database_readWeightOffset();
	pinMode(BUTTON_CONFIG_PIN, INPUT_PULLUP);
	ticker.attach(0.1, Application_ledBlink);
	pixel.begin();
	pixel.setBrightness(100);
	pixel.show();
	WiFi.begin(&ssid[0], &psk[0]);
	LOG.println("Run...");
	LOG.println("Connect to AP " + String(&ssid[0]) + " " + String(&psk[0]));
	scale.begin(LOADCELL_DIO_PIN, LOADCELL_SCK_PIN);
	scale.set_scale(calibration_factor);
	scale.set_offset(weightOffset);
	LOG.print("Set offset done, offset: ");
	LOG.println(weightOffset);
	/* You can remove the password parameter if you want the AP to be open. */
	sprintf(&publishChannel[0], "channels/%s/publish/fields/field%d/B7QDIU6QYWK1KHX0", &idchannel[0], idchannel[0] - '0');
	LOG.println("Publish channel: " + String(&publishChannel[0]));
}

void Application_runInLoop(void)
{
	if (mode == MODE_CONFIG)
	{
		if (server.hasClient())
		{
			socketServer = server.available();
			socketServer.setNoDelay(true);                  // No delay server
			socketServer.setTimeout(150);        // Set timeout 150ms receive data socket server
		}
		if (socketServer)
		{
			if (socketServer.available())
			{
				lenRead = socketServer.readBytes((char*)&bufferReceive[0], 128);
				LOG.println(&bufferReceive[0]);
				StaticJsonBuffer<200> jsonBuffer;
				JsonObject& root = jsonBuffer.parseObject(&bufferReceive[0]);
				if (root.success())
				{
					if (cupRelease)
					{
						LOG.println("No cup!");
						socketServer.write("NO");
					}
					else
					{
						strncpy(&ssid[0], root["SSID"], sizeof(ssid));
						strncpy(&psk[0], root["WPA"], sizeof(psk));
						strncpy(&idchannel[0], root["ID"], sizeof(idchannel));
						LOG.println(String("SSID: ") + &ssid[0]);
						LOG.println(String("WPA: ") + &psk[0]);
						LOG.println(String("ID: ") + &idchannel[0]);
						if (database_writeSSID(&ssid[0]) && database_writePSK(&psk[0]) && database_writeMQTT_IDChannel(&idchannel[0]))
						{
							LOG.println("Write SSID and WPA to EEPROM done");
						}
						else
						{
							LOG.println("Write SSID and WPA to EEPROM failed");
						}
						/*Change publish channel*/
						sprintf(&publishChannel[0], "channels/%s/publish/fields/field%d/B7QDIU6QYWK1KHX0", &idchannel[0], idchannel[0] - '0');
						LOG.println("Publish channel: " + String(&publishChannel[0]));
						/*Send respone to client*/
						LOG.println("Send respone to client");
						socketServer.write("OK");
						delay(1000);
						scale.tare();
						weightOffset = scale.get_offset();// get offset here
						if (database_writeWeightOffset(weightOffset))
						{
							LOG.print("Write offset done, offset: ");
							LOG.println(weightOffset);
						}
						else
						{
							LOG.println("Write offset failed");
						}
						apConfigChange = true;
						mode = MODE_RUNNING;
						WiFi.disconnect();
						WiFi.softAPdisconnect(true);
						server.stop();
						LOG.println("Exit mode config");
					}
					
				}
				socketServer.flush();
			}
		}
		/*Check timeout*/
		if (labs(millis() - configTimeout) >= 300000L)
		{
			/*Return running mode*/
			LOG.println("Exit mode config");
			WiFi.disconnect();
			WiFi.softAPdisconnect(true);
			server.stop();
			mode = MODE_RUNNING;
			apConfigChange = true;
			scale.set_offset(weightOffset);// load privious offset
		}
	}
	else if (mode == MODE_RUNNING)
	{
		/*Connect to AP*/
		if (apConfigChange)
		{
			apConfigChange = false;
			LOG.println("New config, connect to AP...");
			WiFi.begin(&ssid[0], &psk[0]);
		}
#if ENABLE_COMMANDLINE == 1
		if (LOG.available() > 0)
		{
			LOG.readBytes((char*)&bufferReceive[0], 128);
			StaticJsonBuffer<200> jsonBuffer;
			JsonObject& root = jsonBuffer.parseObject(&bufferReceive[0]);
			if (root.success())
			{
				strncpy(&ssid[0], root["SSID"], sizeof(ssid));
				strncpy(&psk[0], root["WPA"], sizeof(psk));
				strncpy(&idchannel[0], root["ID"], sizeof(idchannel));
				LOG.println(String("SSID: ") + &ssid[0]);
				LOG.println(String("WPA: ") + &psk[0]);
				LOG.println(String("ID: ") + &idchannel[0]);
				if (database_writeSSID(&ssid[0]) && database_writePSK(&psk[0]) && database_writeMQTT_IDChannel(&idchannel[0]))
				{
					LOG.println("Write SSID and WPA to EEPROM done");
				}
				else
				{
					LOG.println("Write SSID and WPA to EEPROM failed");
				}
				/*Change publish channel*/
				sprintf(&publishChannel[0], "channels/%s/publish/fields/field%d/B7QDIU6QYWK1KHX0", &idchannel[0], idchannel[0] - '0');
				LOG.println("Publish channel: " + String(&publishChannel[0]));
			}
			if (strstr((char*)&bufferReceive[0], COMMAND_FORCE_SEND_DATA) != NULL)
			{
				forceSend = true;
			}
			else if (strstr((char*)&bufferReceive[0], COMMAND_CONFIG_MODE) != NULL)
			{
				forceConfigMode = true;
			}
			else if (strstr((char*)&bufferReceive[0], COMMAND_RUNNING_MODE) != NULL)
			{
				configTimeout = 0;// force timeout
			}
		}
#endif
		/*Check button*/
		if ((digitalRead(BUTTON_CONFIG_PIN) == LOW) || forceConfigMode)
		{
			if ((labs(millis() - buttonTimePress) >= BUTTON_CONFIG_LONG_PRESS) || forceConfigMode)
			{
				forceConfigMode = false;
				buttonTimePress = millis();
				WiFi.disconnect();
				server.stop();
				delay(1000);
				if (WiFi.softAP(ACCESSPOINT_SSID, ACCESSPOINT_PSK))
				{
					server.begin();
					socketServer = server.available();
					LOG.println("Server started");
					mode = MODE_CONFIG;
					configTimeout = millis();
					LOG.println("Enter mode config, timeout is 5 minutes");
					scale.tare();
				}
				else
				{
					LOG.println("Server error");
				}
			}
		}
		else
		{
			buttonTimePress = millis();
		}
		/*Check internet connection*/
		if (labs(millis() - timeCheckInternetConnection) >= 5000)
		{
			timeCheckInternetConnection = millis();
			if (WiFi.localIP() == IPAddress(0, 0, 0, 0))
			{
				wifiConnected = false;
				LOG.println("Wifi disconnected!");
			}
			else
			{
				wifiConnected = true;
			}
		}
		/**/
		if (wifiConnected)
		{
			if (!initMQTTClient)
			{
				initMQTTClient = true;
				client.setServer(MQTT_SERVER_ADD, MQTT_SERVER_PORT);
				client.setCallback(Application_MQTT_callback);
			}
			if (!client.connected())
			{
				if (client.connect("ESP8266Client", mqttUserName, mqttPass))
				{
					mqttConnected = true;
					LOG.println("MQTT connected");
					//if (client.subscribe(MQTT_SUBCRIBE_CHANNEL))
					{
						//LOG.println("MQTT subcribe OK");
						LOG.println("First time, send data to server");
						sprintf(&dataToPublish[0], "%s%d", &idchannel[0], weight);
						if (client.publish(&publishChannel[0], &dataToPublish[0]))
						{
							LOG.print("Send data OK: ");
							LOG.println(&dataToPublish[0]);
						}
					}
					//else
					{
						//LOG.println("MQTT subcribe failed!");
					}
				}
			}
			else
			{
				
			}
			/*Send data to server with period TIME_PUBLISH_DATA_TO_SERVER*/
			if ((labs(millis() - timePublishData) >= TIME_PUBLISH_DATA_TO_SERVER) || forceSend)
			{
				forceSend = false;
				timePublishData = millis();
				if (client.connected())
				{
					static uint32_t lastWeightToSend;
					if ((weight < lastWeight) && (weight > 0) && (!cupRelease))
					{
						lastWeight = weight;
						sprintf(&dataToPublish[0], "%s%d", &idchannel[0], weight);
						LOG.println("Send data to server");
						if (client.publish(&publishChannel[0], &dataToPublish[0]))
						{
							LOG.print("Send data OK: ");
							LOG.println(&dataToPublish[0]);
						}
					}
				}
				else
				{
					LOG.print("MQTT disconnected!");
				}
			}
		}
	}
	/*Read loadcell*/
	if (labs(millis() - timeReadLoadcell) >= 1000)
	{
		static uint8_t weightCalculateCount = 0;
		timeReadLoadcell = millis();
		scale.set_scale(calibration_factor);
		loadcellUnits = scale.get_units(10) * 5;
		if (loadcellUnits < 0.0)
		{
			loadcellUnits = 0.0;
		}
		weight = (uint32_t)loadcellUnits;
		if (labs(weight - lastWeight) >= WEIGHT_CHANGED_AT_LEAST)
		{
			lastWeight = weight;
			weightChanged = true;
			LOG.println(String("Weight changed, now: ") + weight + " grams");
		}
		if (weight <= 5)
		{
			weightCalculateCount++;
			if (weightCalculateCount >= 3)
			{
				weightCalculateCount = 0;
				cupRelease = true;
				//LOG.println("Cup has just left for 3 sec");
			}
		}
		else
		{
			cupRelease = false;
		}
	}

	client.loop();
}

void Application_ledBlink(void)
{
	static bool toggle;
	if (mode == MODE_CONFIG)
	{
		toggle ^= true;
		if (toggle)
		{
			if (!cupRelease)
			{
				pixel.setPixelColor(LED_RGB_ID, BLUE_COLOR);
			}
			else
			{
				pixel.setPixelColor(LED_RGB_ID, GREEN_COLOR);
			}
		}
		else
		{
			pixel.setPixelColor(LED_RGB_ID, BLACK_COLOR);
		}
		pixel.show();
	}
	else if(mode == MODE_RUNNING)
	{
		if (!wifiConnected)
		{
			toggle ^= true;
			if (toggle)
			{
				pixel.setPixelColor(LED_RGB_ID, RED_COLOR);
			}
			else
			{
				pixel.setPixelColor(LED_RGB_ID, BLACK_COLOR);
			}
			pixel.show();
		}
		else
		{
			if (!cupRelease)
			{
				pixel.setPixelColor(LED_RGB_ID, BLACK_COLOR);
				pixel.show();
			}
			else
			{
				if (pixel.getPixelColor(LED_RGB_ID) != GREEN_COLOR)
				{
					pixel.setPixelColor(LED_RGB_ID, GREEN_COLOR);
					pixel.show();
				}
			}
			
		}

	}
	else
	{

	}
}

void Application_MQTT_callback(char* topic, byte* payload, unsigned int length)
{
	LOG.println("MQTT data receive: ");
	LOG.println(String("Topic: ") + topic);
	LOG.println(String("Data: ") + (char*)payload);
}