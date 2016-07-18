#include <Arduino.h>
#include <ESP8266WiFi.h>
//#include <ArduinoJson.h>
//#include <NeoPixelBus.h>

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


long int std_step = 10;

CLed_fade blue_led(LEDPIN);

void setup() {

	CLed_fade::brightness_t blue_brightness;
	// start value
	blue_brightness.val = 2;
	blue_brightness.max = 175;
	blue_brightness.min = blue_brightness.val;
	blue_brightness.fadeAmount = 1;
	blue_led.setUp(blue_brightness);

  Serial.begin(115200);
  delay(std_step);


  pinMode(CTLPIN, OUTPUT);
  digitalWrite(CTLPIN, 1);
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  // switch off ESPs own wifi Network
  WiFi.softAPdisconnect(true);
  Serial.print("SoftAP disabled");
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());
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

