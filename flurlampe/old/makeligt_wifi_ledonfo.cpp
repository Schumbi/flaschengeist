#include <Arduino.h>
#include <ESP8266WiFi.h>
//#include <ArduinoJson.h>
//#include <NeoPixelBus.h>
#include <WebSocketsClient.h>
#include <Hash.h>

#include "led.hpp"

#define LEDPIN 16
#define CTLPIN 2
#define ON  0
#define OFF 1

void setup();
void loop();


// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

const char* ssid = "wlan@schumbi.de";
const char* password = "Hoha.4wnwlan";

// LED Stuff
long int std_step = 10;
CLed_fade blue_led(LEDPIN);

void setup()
{
	// Serial Stuff
	Serial.begin(115200);
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
	Serial.println("LED initialized");
	// Blue Status lED on Device
	pinMode(CTLPIN, OUTPUT);
	digitalWrite(CTLPIN, 1);
	Serial.println("Blue LED initialized!");

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

	// Start the server
	server.begin();
}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    digitalWrite(CTLPIN, OFF);
	delay(std_step);
	blue_led.fade();
    return;
  }
  else
    digitalWrite(CTLPIN, ON);

  // Wait until the client sends some data
  Serial.println("new client");
  while (!client.available()) {
    delay(1);
  }

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();

  // Match the request
  if (req.indexOf("/gpio/0") != -1)
    blue_led.switch_off();
  else if (req.indexOf("/gpio/1") != -1)
    blue_led.switch_on();
  else {
    Serial.println("invalid request");
    client.stop();
    return;
  }

  client.flush();

  // Prepare the response
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nLichterkette ist nun ";
  s += (blue_led.isOn()) ? "angeschaltet" : "ausgeschaltet";
  s += "</html>\n";

  // Send the response to the client
  client.print(s);
  delay(1);
  Serial.println("Client disonnected");

  // The client will actually be disconnected
  // when the function returns and 'client' object is detroyed
  digitalWrite(CTLPIN, OFF);
}

