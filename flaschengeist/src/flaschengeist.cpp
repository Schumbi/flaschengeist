#include <Arduino.h>

#include <NeoPixelBus.h>
#include <TickerScheduler.h>

#include "wifi.h"

#include "web.h"

#define NUM_LEDS 3
#define LED_TICK 333
#define WEB_TICK 10

void setup();
void loop();


void update_leds();

NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(NUM_LEDS);
TickerScheduler ts(5);
RgbColor c(0,0,0);

void setup()
{
	Serial.println("+");
	Serial.begin(115200);  
	Serial.setDebugOutput(1);
	delay(10);

	strip.Begin();
	strip.ClearTo(c);

	Serial.println("+");
	// Hardware Stuff
	// A0 - Lichtwiderstand (+>-|=L1=|-A0-|=Poti=|-D6)
	pinMode(A0, INPUT);
	// D6 - Power on/off light sensor (sink) Low, light sensor on
	pinMode(D6, OUTPUT);
	digitalWrite(D6, LOW);

	Serial.println("+");
	// activate ws2812s

	Serial.println("wifi");
	// connect to wifi
	wifi_connect();
	c.B = 128;
	strip.ClearTo(c);

	// activate webserver
	Serial.println("web");
	init_web();
	
	c.B = 128;
	c.G = 128;
	strip.ClearTo(c);
	Serial.println("sche");
	// initialize schedule
	//ts.add(0, WEB_TICK, webwork);
	ts.add(0, LED_TICK, update_leds);
	
	Serial.println("+");
	c = RgbColor(0,0,0);
	strip.ClearTo(c);

	Serial.println("+");
	strip.SetPixelColor(0, RgbColor(0,0,100)); // oben
	strip.SetPixelColor(1, RgbColor(0,50,50)); // mitte
	strip.SetPixelColor(2, RgbColor(50,0,50)); // unten
	strip.Show();
	Serial.println("+");
}

void loop() {
	// do what, you need to do
	ts.update();
	webwork();
	delay(10);
	// this is only a test function
	//int a0 = analogRead(A0);
}

void update_leds()
{
	Serial.print(".");
	strip.RotateLeft(1);
	strip.Show();
}

