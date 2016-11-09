#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>

#include "strings.h"

#include "network.h"

namespace ns_net
{
	static Network* net = NULL;
	static ESP8266WebServer* staticServer = NULL;

	void www_404();
	void www_led();
	void www_std();

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

			ESP8266WebServer* getServer()
			{
				if(_server == NULL)
				{
					_server = new ESP8266WebServer(80);
					staticServer = _server;
					MDNS.begin(mdnsname.c_str());
					httpUpdater.setup(_server);
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
				_started = true;
			}

			void stopWeb()
			{
				ESP8266WebServer* s = getServer();
				s->stop();
				MDNS.update();
				_started = false;
			}

			void update()
			{
				getServer()->handleClient();
				MDNS.update();
			}

			bool init() {return _init;}
			bool started() {return _started;}

		private:
			bool _init;
			bool _started;
			ESP8266WebServer* _server;
			ESP8266HTTPUpdateServer httpUpdater;

	};

	Network::Network( String ssid, String password, String mdnsname)
		: d(new Network_impl)
	{
		d->ssid = ssid;	
		d->password = password;
		d->mdnsname = mdnsname;

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


}
