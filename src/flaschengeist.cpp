#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <TickerScheduler.h>

#include "strip.h"

#include "../../wlan.conf"

#define LED_TICK 4

void setup();
void loop();
void update_leds();
void get_brightness();
void update_mqtt_status();

void setup_wifi();
void callback(char* topic, byte* payload, unsigned int length);

const char* myssid = MAKELIGHT_SSID;
const char* mypass = MAKELIGHT_PASS;
const char* mqtt_server = MAKELIGHT_MQTT_SERVER;

// create MQTT client
WiFiClient espClient;
PubSubClient client(espClient);

static volatile bool autoSwitchOnEnabled = false;
long lastMsg = 0;
char msg[50];
int value = 0;

// Ticker to update brightness
TickerScheduler ticker(5);

void setup()
{
	pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output

	// Serial Stuff
	Serial.begin(115200);
	Serial.setDebugOutput(true);
	Serial.println("Start...");
	delay(10);
	// Hardware Stuff
	// A0 - ligth resistor (+>-|=L1=|-A0-|=Poti=|-D6)
	pinMode(A0, INPUT);
	// D6 - Power on/off light sensor (sink) Low, light sensor on
	pinMode(D6, OUTPUT);
	digitalWrite(D6, LOW);

	// initialize schedule
	ticker.add(0, LED_TICK, update_leds);
	ticker.add(1, 1000, update_mqtt_status);
	ticker.add(2, 1000, get_brightness);

	// LED Strip acitvate
	CLedStrip* strip = CLedStrip::getStrip_ptr();
	strip->init();
	strip->getConf().ctr = 50;
	strip->getConf().min = 120;
	strip->getConf().period = 5;
	strip->switch_program(2);

	setup_wifi();

	client.setServer("192.168.3.1" , 1883);
	client.setCallback(callback);
}

void setup_wifi() 
{

	digitalWrite(BUILTIN_LED, LOW);
	delay(10);
	// We start by connecting to a WiFi network
	Serial.println();
	Serial.print("Connecting to ");
	Serial.println(myssid);

	WiFi.begin(myssid, mypass);

	while (WiFi.status() != WL_CONNECTED)
	{
		digitalWrite(BUILTIN_LED, LOW);
		delay(500);
		Serial.print(".");
		digitalWrite(BUILTIN_LED, HIGH);
	}
	Serial.println("");
	Serial.println("WiFi connected");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());
	digitalWrite(BUILTIN_LED, HIGH);
	delay(100);
}

void callback(char* topic, byte* payload, size_t length) {
	Serial.println(topic);

	if(String(topic) == String("/home/wohnzimmer/flaschengeist/command"))
	{
		if(length > 0)
		{
			// Switch on the LED if an 1 was received as first character
			if ((char)payload[0] == '0')
			{
				CLedStrip* strip = CLedStrip::getStrip_ptr();
				strip->switch_program(0);
			} 
			else 
			{
				CLedStrip* strip = CLedStrip::getStrip_ptr();
				strip->switch_program(2);
			}
		}
	}

	if(String(topic) == String("/home/wohnzimmer/flaschengeist/auto"))
	{
		if ((char)payload[0] == '0')
			autoSwitchOnEnabled = false;
		else
			autoSwitchOnEnabled = true;
	}
}

void reconnect() 
{
	// Loop until we're reconnected
	while (!client.connected()) 
	{
		Serial.print("Attempting MQTT connection...");
		// Attempt to connect
		if (client.connect("flaschengeist")) 
		{
			Serial.println("connected");
			// Once connected, publish an announcement...
			client.publish("/home/wohnzimmer/flaschengeist/status",
					String(CLedStrip::getStrip_ptr()->getConf()
						.old_prog).c_str());
			// ... and resubscribe
			client.subscribe("/home/wohnzimmer/command");
			client.subscribe("/home/wohnzimmer/flaschengeist/command");
			client.subscribe("/home/wohnzimmer/flaschengeist/auto");
			digitalWrite(BUILTIN_LED, HIGH);
		} 
		else
		{
			digitalWrite(BUILTIN_LED, LOW);
			Serial.print("failed, rc=");
			Serial.print(client.state());
			Serial.println(" try again in 2 seconds");
			delay(1000);
			digitalWrite(BUILTIN_LED, HIGH);
			delay(1000);
		}
	}
}



void loop() {
	// do what, you need to do
	ticker.update();
	// this is only a test function
	//int a0 = analogRead(A0);
	// busy wait
	//	ns_net::Network::GetNetwork()->webwork();
	if(!client.connected())
	{
		reconnect();
	}
	client.loop();
}

void update_leds()
{
	// update led pattern / program
	CLedStrip::getStrip_ptr()->update();
}

void get_brightness()
{
	int br = analogRead(A0);
	if(client.connected())
	{
		client.publish("/home/wohnzimmer/flaschengeist/brightness",
				String(br).c_str());
	}
	if( autoSwitchOnEnabled)
	{ 
		CLedStrip* strip = CLedStrip::getStrip_ptr();
		if (br < 100)
			strip->switch_program(2);
		if (br > 120)
			strip->switch_program(0);
	}

	// ... and resubscribe
}

void update_mqtt_status()
{
	if(client.connected())
	{
		client.publish("/home/wohnzimmer/flaschengeist/status",
				String(CLedStrip::getStrip_ptr()->getConf().old_prog).c_str());

		client.publish("/home/wohnzimmer/flaschengeist/auto", String(autoSwitchOnEnabled).c_str() );
	}
}

