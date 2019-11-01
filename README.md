#Hardware descriptions:

##1. ESP8266 Wifi module:

![](https://images-na.ssl-images-amazon.com/images/I/71efjnKymHL._SX342_.jpg)
##2. Loadcell 5kg:

![](https://nshopvn.com/wp-content/uploads/2019/03/065271871760_loadcell-5kg.jpg)
##3. HX711 ADC module:

![](https://www.makerlab-electronics.com/my_uploads/2016/12/hx711-load-cell-amplifier-1.jpg)

##4. LED RGB W2812B module:

![](https://www.artekit.eu/wp-content/uploads/ak-ws2812b-single-main.jpg)

##5. Button module:

![](https://robu.in/wp-content/uploads/2019/01/Momentary-Tactile-Push-Button-Module-1.jpg)

#Hardware connections:
	| Arduino pin | Peripherals     |
	|-------------|-----------------|
	| D1          | HX711_CLK_PIN   |
	| D2          | HX711_DIO_PIN   |
	| D3          | BUTTON_PIN      |
	| D4          | LED_RGB_PIN     |

#Firmware libraries:

##1. [PubsubClient](https://github.com/knolleary/pubsubclient) to use MQTT client.
##2. HX711 to read gravity from Loadcell 5Kg.
##3. Database to access EEPROM from ESP8266.

#Firmware configurations:

##All configurations can be found in config.h file. We can change some configurations:

###1. Change Accesspoint configurations:

    #define ACCESSPOINT_SSID				"SmartDock"

    #define ACCESSPOINT_PSK					"12345678"

###2. Change period time to check internet connection(time in millisecond):

    #define TIME_CHECK_INTERNET_CONNECTION	30000L

Default is 30 seconds.

###3. Change period time to publish data to MQTT server(time in millisecond):

    #define TIME_PUBLISH_DATA_TO_SERVER		60000L

Default is 60 senconds.

##Specially, server configurations can be changed in server_config.h:

    #define MQTT_API_KEY					"<API_KEY>"

    #define MQTT_WRITE_API_KEY				"<WRITE_API_KEY>"

    #define MQTT_SERVER_ADD					"mqtt.thingspeak.com"

    #define MQTT_SERVER_PORT				1883

    #define MQTT_SUBCRIBE_CHANNEL			"channels/<channel_id>/subscribe/fields/<field_number>/<API_KEY>"

    #define MQTT_PUBLISH_CHANNEL			"channels/<channel_id>/publish/fields/<field_number>/<API_KEY>"

The channel\_id, field\_number API\_KEY and WRITE\_API\_KEY we can find in Channels > My Profile in [www.thingspeak.com](http://thingspeak.com).