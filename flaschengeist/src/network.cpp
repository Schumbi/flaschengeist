#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>

#include <Hash.h>
#include <WebSocketsClient.h>

#include "strings.h"

#include "network.h"

namespace ns_net
{
	static Network* net = NULL;
	static ESP8266WebServer* staticServer = NULL;
	static WebSocketsClient* staticWs = NULL;
	static String staticMDNS = "";

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
					MDNS.begin(mdnsname.c_str());

					staticMDNS = mdnsname;

					_httpUpdater.setup(_server);
					_init = true;
				}
				return _server;
			}

			// maybey set an array here or struct here
			void startWeb()
			{
				ESP8266WebServer* s = getServer();
				s->onNotFound(www_404);
				s->on("/", HTTP_GET, www_std);
				s->on("/led", HTTP_GET, www_led);

				s->begin();
				MDNS.addService("http2led", "tcp", 80);

				_wsclient.begin(wsgateway.c_str(), wsport);
				_wsclient.onEvent(ws_Callback);

				_started = true;
			}

			void stopWeb()
			{
				ESP8266WebServer* s = getServer();
				s->stop();
				_wsclient.disconnect();
				MDNS.update();
				_started = false;
			}

			void update()
			{
				if(_started)
				{
					getServer()->handleClient();
					MDNS.update();
					_wsclient.loop();
				}
			}

			bool init() {return _init;}
			bool started() {return _started;}

		private:
			bool _init;
			bool _started;
			ESP8266WebServer* _server;
			ESP8266HTTPUpdateServer _httpUpdater;
			WebSocketsClient _wsclient;

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
				d->startWeb();

			if(d->started())
				d->update();
		}
		else
		{
			if(d->started())
				d->stopWeb();
		}
	}

	WebSocketsClient*
		Network::GetSocket()
		{
			return NULL;
		}

	// call backs for web server
	void www_std()
	{
		Response = html_anfang;
		Response += form + html_ende;

		staticServer->send(200, "text/html", Response);
	}

	void www_led()
	{
		int state = staticServer->arg("state").toInt();
		// Hier anschalten, was auch immer
		// blue_led.power(state);
		if(state)
			digitalWrite(D8, HIGH);
		else
			digitalWrite(D8, LOW);

		staticServer->sendHeader("Location", String("/"), true);
		staticServer->send ( 302, "text/plain", "");
	}

	void www_404()
	{
		Response = html_anfang + "Not found!" + html_ende;
		staticServer->send(404, "text/html", Response);
	}

	void ws_Callback(WStype_t type, uint8_t * payload, size_t lenght)
	{
		switch(type) 
		{
			case WStype_DISCONNECTED:
				break;
			case WStype_CONNECTED:
				staticWs->sendTXT(String(staticMDNS).c_str());
				break;
			case WStype_TEXT:
				if(String((const char*)payload) == "brigthness")
				{
					staticWs->sendTXT(String(analogRead(A0)).c_str());
				}
				break;
			case WStype_BIN:
				break;
			default:
				lenght = 1;
		}
	}

}
