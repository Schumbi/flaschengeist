#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>

#include <Hash.h>
#include <WebSocketsClient.h>

#include "strings.h"

#include "network.h"

#include "strip.h"

namespace ns_net
{
	static Network* net = NULL;
	static ESP8266WebServer* staticServer = NULL;
	static WebSocketsClient* staticWs = NULL;

	void www_404();
	void www_led();
	void www_std();
	void ws_Callback(WStype_t type, uint8_t * payload, size_t lenght);

	Network* 
		Network::GetNetwork()
		{
			return net;
		}

	Network* 
		Network::StartNetwork(String ssid, String passphrase, String mdnsname )
		{
			if(net == NULL)
			{
				net = new ns_net::Network(ssid, passphrase, mdnsname);
			}
			return net;
		}

	class Network_impl
	{
		public:
			Network_impl() : 
				_init(false),
				_started(false),
				_server(NULL) {}

			~Network_impl()
			{
				if(_server)
				{
					_server->stop();
					delete(_server);
				}
			}

			String ssid;
			String password;
			String mdnsname;
			String wsgateway;
			uint16_t wsport;

			ESP8266WebServer* getServer()
			{
				if(_server == NULL)
				{
					_server = new ESP8266WebServer(80);
					staticServer = _server;
					staticWs = &_wsclient;
					_init = true;

				}
				return _server;
			}

			// maybe set an array here or struct here
			void startWeb()
			{
				ESP8266WebServer* s = getServer();

				s->begin();
				_httpUpdater.setup(s);
				delay(1000);
				Serial.println("[Debug] updater started");

				s->onNotFound(www_404);
				s->on("/", HTTP_GET, www_std);
				s->on("/led", HTTP_GET, www_led);

				delay(100);
				Serial.println("[Debug] Server callbacks set ");

				_wsclient.begin(wsgateway.c_str(), wsport);
				_wsclient.onEvent(ws_Callback);
				delay(500);
				Serial.println("[Debug] WS started callbacks set ");


				if(!MDNS.begin("flaschengeist")) //mdnsname.c_str()))
				{
					Serial.println("[Failure] MDNS not started");
				}
				delay(1000);

				MDNS.addService("http", "tcp", 80);
				MDNS.addService("ws", "tcp", 2301);

				Serial.println("[Debug] MDNS started");
				_started = true;
			}

			void stopWeb()
			{
				ESP8266WebServer* s = getServer();
				s->stop();
				_wsclient.disconnect();
				_started = false;
			}

			void update()
			{
				if(_started)
				{
					getServer()->handleClient();
				}
			}

			bool init() {return _init;}
			bool started() {return _started;}

			WebSocketsClient _wsclient;

		private:
			bool _init;
			bool _started;
			ESP8266WebServer* _server;
			ESP8266HTTPUpdateServer _httpUpdater;

	};

	Network::Network( String ssid, String password, String mdnsname)
		: d(new Network_impl)
	{
		d->ssid = ssid;	
		d->password = password;
		d->mdnsname = mdnsname;

		// setup websocket TODO mach hübsch
		d->wsport = 2301;
		d->wsgateway = "192.168.3.1";

		WiFi.softAPdisconnect(true);
		WiFi.begin( d->ssid.c_str(), d->password.c_str());

	}

	Network::~Network()
	{
		if(d != NULL)
			delete d;
	}

	bool Network::connected()
	{
		return WiFi.status() == WL_CONNECTED;
	}

	void Network::webwork()
	{
		if(connected())
		{
			if(!d->started())
			{
				d->startWeb();
				Serial.println("[Debug] web started");
			}

			if(d->started())
				d->update();
		}
		else
		{
			if(d->started())
			{
				d->stopWeb();
				Serial.println("[Debug] web stoped");
			}
		}
	}

	WebSocketsClient*
		Network::GetSocket()
		{
			return &(d->_wsclient);
		}

	// call backs for web server
	void www_std()
	{
		Serial.println("[Debug] www_std");
		Response = html_anfang;
		Response += form + html_ende;

		staticServer->send(200, "text/html", Response);
	}

	void www_led()
	{
		Serial.println("[Debug] www_led");
		int state = staticServer->arg("state").toInt();
		// Hier anschalten, was auch immer
		// blue_led.power(state);
		if(state)
		{
			CLedStrip::getStrip_ptr()->switch_program(2);
		}
		else
		{
			CLedStrip::getStrip_ptr()->switch_program(0);
		}

		staticServer->sendHeader("Location", String("/"), true);
		staticServer->send ( 302, "text/plain", "");
		
	}

	void www_404()
	{
		Serial.println("[Debug] www_404");
		Response = html_anfang + "Not found!" + html_ende;
		staticServer->send(404, "text/html", Response);
	}

	void ws_Callback(WStype_t type, uint8_t * payload, size_t lenght)
	{
		Serial.println("[Debug] ws callback Type" + String(type));
		switch(type) 
		{
			case WStype_DISCONNECTED:
				Serial.println("[Debug] ws disconnected ");
				break;
			case WStype_CONNECTED:
				staticWs->sendTXT(String("Flaschengeist").c_str());
				Serial.println("[Debug] ws connected");
				break;
			case WStype_TEXT:
				if(String((const char*)payload) == "brigthness")
				{
					Serial.println("[Debug] ws got text");
					staticWs->sendTXT(String(analogRead(A0)).c_str());
				}
				break;
			case WStype_BIN:
				Serial.println("[Debug] ws got bin");
				break;
			default:
				lenght = 1;
				Serial.println("[Debug] ws default " + String(type));
		}
	}
}

