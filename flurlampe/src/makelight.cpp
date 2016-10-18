#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include "led.hpp"

#define LEDPIN 16
#define CTLPIN 2
#define ON  0
#define OFF 1

void setup();
void loop();

// http://links2004.github.io/Arduino/d3/d58/class_e_s_p8266_web_server.html

// Create an instance of the server
// specify the port to listen on as an argument
//WiFiServer server(80);
ESP8266WebServer server(80);

String Response ="";

const char* ssid = "wlan@schumbi.de";
const char* password = "Hoha.4wnwlan";

// LED Stuff
long int std_step = 10;
CLed_fade blue_led(LEDPIN);

String html_anfang = "<!DOCTYPE html>\r\n<html>\r\n\
<head>\r\n<meta content=\"text/html; charset=ISO-8859-1\" http-equiv=\"content-type\">\r\n\
<title>WebSchalter</title>\r\n<body><p>";
String html_ende = "</p></body>\r\n</html>";

String redirect = "<!DOCTYPE html>\r\n<html>\r\n\
<head>\r\n\
<meta content=\"text/html; charset=ISO-8859-1\" \
<meta http-equiv=\"refresh\" content=\"0; URL='./'\" /> \
</head><body/></html>";

String form = "<form action='led'><input type='radio' name='state' value='1' checked>On<input type='radio' name='state' value='0'>Off<input type='submit' value='Submit'></form>";

void www_std()
{
	String state_led;
	if(blue_led.isOn())
		state_led = "an";
	else
		state_led = "aus";

	Response = html_anfang + "LED ist " + state_led + "!<br/>";
	Response += form + html_ende;

	server.send(200, "text/html", Response);
}

void www_led()
{
	int state = server.arg("state").toInt();
	blue_led.power(state);
	server.sendHeader("Location", String("/"), true);
	server.send ( 302, "text/plain", "");
}

void www_404()
{
	Serial.printf("404 - not found");
	Response = html_anfang + "Not found!" + html_ende;
	server.send(404, "text/html", Response);
}

void setup()
{
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

	server.on("/", HTTP_GET, www_std);
	server.on("/led", HTTP_GET, www_led);
	server.onNotFound(www_404);

	// Start the server
	server.begin();
    blue_led.switch_on();
}

void loop() {
	blue_led.fade();
	server.handleClient();
	delay(std_step);
/**
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    digitalWrite(CTLPIN, OFF);
	blue_led.fade();
	webSocket.loop();
	delay(std_step);
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
  {
    blue_led.switch_off();
	webSocket.sendTXT("Switched off");
  }
  else if (req.indexOf("/gpio/1") != -1)
  {
    blue_led.switch_on();
	webSocket.sendTXT("Switched on");
  }
  else {
    Serial.println("invalid request");
	webSocket.sendTXT("Invalid request");
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
  **/
}

