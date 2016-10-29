#include "web.h"

#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>
// http://links2004.github.io/Arduino/d3/d58/class_e_s_p8266_web_server.html

#include "strings.h"

// Create an instance of the server
// specify the port to listen on as an argument
static ESP8266WebServer server(80);
static ESP8266HTTPUpdateServer httpUpdater;

void init_web()
{
	MDNS.begin("flaschengeist");
	httpUpdater.setup(&server);

	server.on("/", HTTP_GET, www_std);
	server.on("/led", HTTP_GET, www_led);
	server.onNotFound(www_404);

	// Start the server
	server.begin();
	MDNS.addService("http", "tcp", 80);
}

void webwork()
{
	server.handleClient();
	MDNS.update();
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
	if(state)
		digitalWrite(D8, HIGH);
	else
		digitalWrite(D8, LOW);

	server.sendHeader("Location", String("/"), true);
	server.send ( 302, "text/plain", "");
}

void www_404()
{
	Response = html_anfang + "Not found!" + html_ende;
	server.send(404, "text/html", Response);
}

