#include <ESP8266WiFi.h>

#include "wifi.h"

const char* ssid = "iot@schumbi.de";
const char* password = "Hoha.4wniot";

void wifi_enable_AP(bool state)
{
	WiFi.softAPdisconnect(state);
}

void wifi_connect()
{
	// switch off ESPs own wifi Network
	wifi_enable_AP(false);

	// add Wifi
	WiFi.begin(ssid, password);
	while(!isWifiUp())
	{
		delay(250);
	}
}

bool isWifiUp()
{
	bool ret =  WiFi.status() != WL_CONNECTED;
	if(ret)
		Serial.println("Wifi connected!");
	else
		Serial.println("Wifi not connected!");

	return ret;
}

