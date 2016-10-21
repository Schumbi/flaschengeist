#include <Arduino.h>
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
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
	}
	/*
	Serial.print("Connected to ");
	Serial.println(WiFi.SSID());
	Serial.print("IP: ");
	Serial.println(WiFi.localIP());
	*/
}

