#include "web.h"

#include "strings.h"

// Create an instance of the server
// specify the port to listen on as an argument
static ESP8266WebServer server(80);

void init_web()
{
	server.on("/", HTTP_GET, www_std);
	server.on("/led", HTTP_GET, www_led);
	server.onNotFound(www_404);

	// Start the server
	server.begin();
}

void webwork()
{
	server.handleClient();
}

// call backs for web server
void www_std()
{
	Response = html_anfang;
	Response += form + html_ende;

	server.send(200, "text/html", Response);
}

void www_led()
{
	int state = server.arg("state").toInt();
	// Hier anschalten, was auch immer
	// blue_led.power(state);
	server.sendHeader("Location", String("/"), true);
	server.send ( 302, "text/plain", "");
}

void www_404()
{
	Serial.printf("404 - not found");
	Response = html_anfang + "Not found!" + html_ende;
	server.send(404, "text/html", Response);
}

