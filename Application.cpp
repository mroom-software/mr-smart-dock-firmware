// 
// 
// 

#include "Application.h"

WiFiClient socketServer;
WiFiServer server(4567);
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


void Application_initialize(void)
{
	delay(1000);
	Serial.begin(115200);
	Serial.setTimeout(100);
	EEPROM.begin(1024);
	EEPROM.get(0, ssid);
	EEPROM.get(64, psk);
	ssid[63] = '\0';
	psk[63] = '\0';
	pinMode(BUTTON_CONFIG_PIN, INPUT_PULLUP);
	ticker.attach(0.1, Application_ledBlink);
	pixel.begin();
	pixel.setBrightness(2);
	pixel.show();
	WiFi.begin(&ssid[0], &psk[0]);
	Serial.println("Run...");
	Serial.println("Connect to AP " + String(&ssid[0]) + " " + String(&psk[0]));
	scale.begin(LOADCELL_DIO_PIN, LOADCELL_SCK_PIN);
	scale.set_scale(calibration_factor);
	scale.tare();
	
	/* You can remove the password parameter if you want the AP to be open. */
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
				Serial.println(&bufferReceive[0]);
				StaticJsonBuffer<200> jsonBuffer;
				JsonObject& root = jsonBuffer.parseObject(&bufferReceive[0]);
				if (root.success())
				{
					strncpy(&ssid[0], root["SSID"], sizeof(ssid));
					strncpy(&psk[0], root["WPA"], sizeof(psk));
					Serial.println(String("SSID: ") + &ssid[0]);
					Serial.println(String("WPA: ") + &psk[0]);
					EEPROM.put(0, ssid);
					EEPROM.put(64, psk);
					if (EEPROM.commit())
					{
						Serial.println("Write SSID and WPA to EEPROM done");
					}
					else
					{
						Serial.println("Write SSID and WPA to EEPROM failed");
					}
					apConfigChange = true;
					mode = MODE_RUNNING;
					Serial.println("Exit mode config");
				}
				socketServer.flush();
			}
		}
		/*Check timeout*/
		if (millis() - configTimeout >= 300000L)
		{
			/*Return running mode*/
			Serial.println("Exit mode config");
			WiFi.disconnect();
			server.stop();
			mode = MODE_RUNNING;
			apConfigChange = true;
		}
	}
	else if (mode == MODE_RUNNING)
	{
		/*Connect to AP*/
		if (apConfigChange)
		{
			apConfigChange = false;
			Serial.println("New config, connect to AP...");
			WiFi.begin(&ssid[0], &psk[0]);
		}
		if (Serial.available() > 0)
		{
			Serial.readBytes((char*)&bufferReceive[0], 128);
			StaticJsonBuffer<200> jsonBuffer;
			JsonObject& root = jsonBuffer.parseObject(&bufferReceive[0]);
			if (root.success())
			{
				strcpy(&ssid[0], root["SSID"]);
				strcpy(&psk[0], root["PSK"]);
				Serial.print(String("SSID: ") + &ssid[0]);
				Serial.println(String("PSK: ") + &psk[0]);
			}
		}
		/*Check button*/
		if (digitalRead(BUTTON_CONFIG_PIN) == LOW)
		{
			if (millis() - buttonTimePress >= BUTTON_CONFIG_LONG_PRESS)
			{
				buttonTimePress = millis();
				WiFi.disconnect();
				server.stop();
				delay(1000);
				if (WiFi.softAP(ACCESSPOINT_SSID, ACCESSPOINT_PSK))
				{
					server.begin();
					socketServer = server.available();
					Serial.println("Server started");
					mode = MODE_CONFIG;
					configTimeout = millis();
					Serial.println("Enter mode config, timeout is 5 minutes");
				}
				else
				{
					Serial.println("Server error");
				}
			}
		}
		else
		{
			buttonTimePress = millis();
		}

		/*Read loadcell*/
		if (millis() - timeReadLoadcell >= 1000)
		{
			timeReadLoadcell = millis();
			scale.set_scale(calibration_factor);
			loadcellUnits = scale.get_units(10);
			if (loadcellUnits < 0.0)
			{
				loadcellUnits = 0.0;
			}
			Serial.println(String("Units: ") + loadcellUnits);
		}
		
	}
}

void Application_ledBlink(void)
{
	static bool toggle;
	toggle ^= true;
	if (toggle)
	{
		pixel.setPixelColor(0, 0x00FF0000);
	}
	else
	{
		pixel.setPixelColor(0, 0x00000000);
	}
	
	pixel.show();
}