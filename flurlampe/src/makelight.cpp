#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <TickerScheduler.h>

#include "led.hpp"

#include "../wlan.conf"

#define LEDPIN 16

void setup();
void loop();
void update_leds();
void update_mqtt_status();

void setup_wifi();
void callback(char* topic, byte* payload, unsigned int length);

const char* ssid = MAKELIGHT_SSID;
const char* password = MAKELIGHT_PASS;
const char* mqtt_server = MAKELIGHT_MQTT_SERVER;


// create MQTT client
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

// Ticker to update brightness
TickerScheduler ticker(5);

// LED Stuff
long int std_step = 10;
CLed_fade blue_led(LEDPIN);

// setup LED and Client
void setup()
{
	pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output

	// Serial Stuff
	Serial.begin(115200);
	Serial.setDebugOutput(true);
	Serial.println("Start...");
	delay(10);
	// LED Stuff
	CLed_fade::brightness_t blue_brightness;
	// start value
	blue_brightness.val = 2;
	blue_brightness.max = 175;
	blue_brightness.min = blue_brightness.val;
	blue_brightness.fadeAmount = 1;
	blue_led.setUp(blue_brightness);
	blue_led.switch_on();
	Serial.println("LED initialized");
	// initialize ticker callbacks
	ticker.add(0, 10, update_leds);
	ticker.add(1, 1000, update_mqtt_status);
	// setup networking stuff
	setup_wifi();
	client.setServer(mqtt_server, 1883);
	client.setCallback(callback);
}

void setup_wifi() 
{

	digitalWrite(BUILTIN_LED, LOW);
	delay(10);
	// We start by connecting to a WiFi network
	Serial.println();
	Serial.print("Connecting to ");
	Serial.println(ssid);

	WiFi.begin(ssid, password);

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
	// Switch on the LED if an 1 was received as first character
	if ((char)payload[0] == '0')
	{
		blue_led.switch_off();
	} 
	else 
	{
		blue_led.switch_on();
	}
	Serial.println(topic);
}

void reconnect() 
{
	// Loop until we're reconnected
	while (!client.connected()) 
	{
		Serial.print("Attempting MQTT connection...");
		// Attempt to connect
		if (client.connect("flurblume")) 
		{
			Serial.println("connected");
			// Once connected, publish an announcement...
			client.publish("/home/flur/flurblume/status", String(blue_led.isOn()).c_str() );
			// ... and resubscribe
			client.subscribe("/home/flur/command");
			client.subscribe("/home/flur/flurblume/command");
			digitalWrite(BUILTIN_LED, HIGH);
		} 
		else
		{
			digitalWrite(BUILTIN_LED, LOW);
			Serial.print("failed, rc=");
			Serial.print(client.state());
			Serial.println(" try again in 2 seconds");
			delay(2000);
			digitalWrite(BUILTIN_LED, HIGH);
		}
	}
}

// ticker calls this function to update brightness level of LED
void update_leds()
{
	blue_led.fade();
}

void update_mqtt_status()
{
	if(client.connected())
		client.publish("/home/flur/flurblume/status", String(blue_led.isOn()).c_str() );
}

// arduino framework calls this function to update mqtt client and ticker
void loop() {
	if(!client.connected())
	{
		reconnect();
	}
	client.loop();
	ticker.update();
}

