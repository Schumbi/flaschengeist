#include <Arduino.h>

#include <NeoPixelBus.h>

#include "wifi.h"

#include "web.h"



const int std_step = 100; // ms

void setup();
void loop();


NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(3);

void setup()
{
	Serial.begin(115200);  
	Serial.setDebugOutput(1);
	Serial.setDebugOutput(0);
	delay(10);

	// Hardware Stuff
	// D7 - Data LEDs WS2812
	// A0 - Lichtwiderstand (+>-|=L1=|-A0-|=Poti=|-D6)
	
	// D8 - Power on/off als Verstärker High, power to bus
	pinMode(D8, OUTPUT);
	digitalWrite(D8, HIGH);

	//pinMode(D7, OUTPUT);
	//digitalWrite(D7, LOW);
	strip.Begin();

	// D6 - Power on/off light sensor (sink) Low, light sensor on
	pinMode(D6, OUTPUT);
	digitalWrite(D6, LOW);

	//pinMode(A0, INPUT);

	// activate ws2812s
	strip.ClearTo(RgbColor(128,128,128));
	strip.Show();

	wifi_connect();
	// activate webserver
	init_web();

	// initial pixel count
	const int l = 64;
	strip.SetPixelColor(0,RgbColor(0,0,l)); // oben rechts (vom Auge aus reingesehen)
	strip.SetPixelColor(1,RgbColor(0,l,0)); // oben links
	strip.SetPixelColor(2,RgbColor(l,0,0)); // unten
	strip.Show();
}

void loop() {

	webwork();
	// do what, you need to do
	// this is only a test function
	//int a0 = analogRead(A0);
	
	strip.RotateLeft(1);
	//strip.SetPixelColor(0,RgbColor(0,0,12)); // oben rechts (vom Auge aus reingesehen)
	//strip.SetPixelColor(1,RgbColor(0,12,0)); // oben links
	//strip.SetPixelColor(2,RgbColor(12,0,0)); // unten
	strip.Show();

	delay(100);
}

