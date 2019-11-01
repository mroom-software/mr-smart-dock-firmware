#ifndef _CONFIG_H
#define _CONFIG_H

/*For command line*/
#define ENABLE_COMMANDLINE				1
#define COMMAND_FORCE_SEND_DATA			"fsend"
#define COMMAND_CONFIG_MODE				"cfgmode"
#define COMMAND_RUNNING_MODE			"exit"

#define LOG								Serial

#define LOCAL_SERVER_PORT				4567

#define BUTTON_CONFIG_PIN				D3
#define BUTTON_CONFIG_LONG_PRESS		3000
#define LED_RGB_PIN						D4
#define LED_RGB_ID						0

#define ACCESSPOINT_SSID				"SmartDock"
#define ACCESSPOINT_PSK					"12345678"

#define LOADCELL_SCK_PIN				D1
#define LOADCELL_DIO_PIN				D2

#define TIME_CHECK_INTERNET_CONNECTION	30000L
#define TIME_PUBLISH_DATA_TO_SERVER		60000L


#define RED_COLOR						0x00FF0000
#define GREEN_COLOR						0x0000FF00
#define BLUE_COLOR						0x000000FF
#define BLACK_COLOR						0x00000000
#define WHITE_COLOR						0x00FFFFFF

#define WEIGHT_CHANGED_AT_LEAST			10


#endif
