#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <NeoPixelBus.h>

#include "web.h"

void setup();
void loop();

// http://links2004.github.io/Arduino/d3/d58/class_e_s_p8266_web_server.html

const char* ssid = "wlan@schumbi.de";
const char* password = "Hoha.4wnwlan";
const int std_step = 100; // ms

NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(2);

void setup()
{
	// Serial Stuff
	Serial.begin(115200);
	Serial.setDebugOutput(true);
	Serial.println("Start...");
	delay(10);

	// Hardware Stuff
	// D7 - Data LEDs WS2812
	// A0 - Lichtwiderstand (+>-|=L1=|-A0-|=Poti=|-D6)
	
	// D8 - Power on/off als Verstärker High, power to bus
	pinMode(D8, OUTPUT);
	digitalWrite(D8, HIGH);

	// D6 - Power on/off light sensor (sink) Low, light sensor on
	pinMode(D6, OUTPUT);
	digitalWrite(D6, LOW);

	pinMode(A0, INPUT);

	// Connect to WiFi network
	// switch off ESPs own wifi Network
	WiFi.softAPdisconnect(true);
	Serial.print("SoftAP disabled\n");
	// add Wifi
	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.print(".");
	}
	Serial.print("Connected to ");
	Serial.println(WiFi.SSID());
	Serial.print("IP: ");
	Serial.println(WiFi.localIP());

	// activate webserver
	init_web();

	strip.Begin();
}

void loop() {
	// do what, you need to do
	webwork();

	int a0 = analogRead(A0);
	Serial.printf(" %4d", a0/4);

	RgbColor c1(a0/8,1,1);
	strip.SetPixelColor(1,c1);

	RgbColor c2(1,a0/8,1);
	strip.SetPixelColor(0,c2);

	strip.Show();
	delay(std_step);
}

