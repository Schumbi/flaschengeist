#include <NeoPixelBus.h>
#include <TickerScheduler.h>

#include "wifi.h"
#include "web.h"
#include "strip.h"

#define LED_TICK 1
#define WEB_TICK 10

void setup();
void loop();
void update_leds();

TickerScheduler ts(5);

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

	// connect to wifi
	wifi_connect();

	// activate webserver
	init_web();

	// initialize schedule
	ts.add(0, LED_TICK, update_leds);

	// LED Strip acitvate
	CLedStrip* strip = CLedStrip::getStrip_ptr();
	strip->init();
	strip->getConf().max = 33;
	strip->getConf().min = 0;
	strip->switch_program(1);
}

void loop() {
	// do what, you need to do
	ts.update();
	// this is only a test function
	//int a0 = analogRead(A0);
	webwork();
	// busy wait
	delay(WEB_TICK);
}

void update_leds()
{
	// update led pattern / program
	CLedStrip::getStrip_ptr()->update();
}

