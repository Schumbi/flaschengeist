#include <ESP8266WiFi.h>

#include "wifi.h"

const char* ssid = "iot@schumbi.de";
const char* password = "Hoha.4wniot";

bool isConnecting = false;

void wifi_enable_AP(bool state)
{
	WiFi.softAPdisconnect(state);
}

void wifi_connect()
{
	if(! isConnecting)
	{
		isConnecting = true;
		// switch off ESPs own wifi Network
		wifi_enable_AP(false);

		// add Wifi
		WiFi.begin(ssid, password);
	}
}

bool isWifiUp()
{
	bool connected =  WiFi.status() != WL_CONNECTED;
	return  connected;
}

