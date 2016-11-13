#include <NeoPixelBus.h>
#include <TickerScheduler.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>

#include "network.h"
#include "web.h"
#include "strip.h"

#include "../wlan.conf"

#define LED_TICK 4
void setup();
void loop();
void update_leds();
void get_brightness();

TickerScheduler ts(5);

StaticJsonBuffer<200> jsonBuffer;

void setup()
{
	// wait to plug serial
	delay(2000);

	Serial.println("+");
	Serial.begin(115200);
	Serial.setDebugOutput(1);
	delay(10);

	// Hardware Stuff
	// A0 - ligth resistor (+>-|=L1=|-A0-|=Poti=|-D6)
	pinMode(A0, INPUT);
	// D6 - Power on/off light sensor (sink) Low, light sensor on
	pinMode(D6, OUTPUT);
	digitalWrite(D6, LOW);

	// initialize schedule
	ts.add(0, LED_TICK, update_leds);
	ts.add(1, 1000, get_brightness);

	// LED Strip acitvate
	CLedStrip* strip = CLedStrip::getStrip_ptr();
	strip->init();
	strip->getConf().ctr = 50;
	strip->getConf().min = 120;
	strip->getConf().period = 5;
	strip->switch_program(2);

	// data included from wlan.conf
	String ssid = MAKELIGHT_SSID; // makelight
	String pass = MAKELIGHT_PASS; // xxx
	String name = MAKELIGHT_NAME; // device1

	ns_net::Network::StartNetwork(ssid, pass, name );
	
	JsonObject& root = jsonBuffer.createObject();
	root["id"] = 1;
	root["name"] =  name;
}

void loop() {
	// do what, you need to do
	ts.update();
	// this is only a test function
	//int a0 = analogRead(A0);
	// busy wait
	ns_net::Network* n = ns_net::Network::GetNetwork(); 
	if(n && n->connected())
	{
		n->webwork();
		MDNS.update();
	}
}

void update_leds()
{
	// update led pattern / program
	CLedStrip::getStrip_ptr()->update();
}

void get_brightness()
{
	ns_net::Network* n = ns_net::Network::GetNetwork(); 
	if(n && n->connected())
	{
		WebSocketsClient* wsclient = n->GetSocket();
		if(wsclient)
		{
			uint16_t br = analogRead(A0);
			//Serial.printf("[Info] ws_client sends brightness %d \n", br);
			wsclient->sendTXT(String(br).c_str());
		}
	}
}

