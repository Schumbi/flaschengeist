#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <PubSubClient.h>

#include <Hash.h>

#include "strip.h"
#include "strings.h"
#include "network.h"

const char* mqttServerUrl = "cubier.local";
const int mqttServerPort = 1883;

const char* inChannel = "/makelight/flaschengeist/in";
const char* outChannel = "/makelight/flaschengeist/out";
const char* statusChannel = "/makelight/flaschengeist/status";

namespace ns_net
{
	static Network* net = NULL;
	static ESP8266WebServer* staticServer = NULL;
	static PubSubClient* staticMqttClient = NULL;
	
	void www_404();
	void www_led();
	void www_std();
	void callback(char* topic, byte* payload, size_t length);

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
				_server(NULL),
				_mqttClient(NULL) {}

			~Network_impl()
			{
				if(_server)
				{
					_server->stop();
					delete(_server);
					delete(_mqttClient);

					staticServer = NULL;
					staticMqttClient = NULL;
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
					_init = true;

				}
				return _server;
			}

			PubSubClient* getMqttClient()
			{
				if(_mqttClient == NULL)
				{
					_mqttClient = new PubSubClient(_espClient);
					staticMqttClient = _mqttClient;
				}
				return _mqttClient;
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

				_mqttClient = getMqttClient();
				_mqttClient->setServer(mqttServerUrl, mqttServerPort);
				_mqttClient->setCallback(callback);
				Serial.println("[Debug] MQTT client started ");

				if(!MDNS.begin("flaschengeist")) //mdnsname.c_str()))
				{
					Serial.println("[Failure] MDNS not started");
				}
				delay(100);

				MDNS.addService("http", "tcp", 80);
				MDNS.addService("mqtt", "tcp", 1883);
				//MDNS.addService("mqtt/ws", "tcp", 8000);

				Serial.println("[Debug] MDNS started");
				delay(1000);
				_started = true;
			}

			void stopWeb()
			{
				ESP8266WebServer* s = getServer();
				s->stop();
				_started = false;
			}

			void update()
			{
				if(_started)
				{
					getServer()->handleClient();

					if(tryConnectMqtt())
					{
						_mqttClient->loop();
					}
				}
			}

			bool init() {return _init;}
			bool started() {return _started;}

		private:
			bool _init;
			bool _started;
			ESP8266WebServer* _server;
			PubSubClient* _mqttClient;

			ESP8266HTTPUpdateServer _httpUpdater;
			WiFiClient _espClient;


			bool tryConnectMqtt()
			{
				if(!_mqttClient->connected())
				{
					if(_mqttClient->connect("flaschengeist"))
					{
						Serial.println("connected");
						// TODO Ontologie des Systems entwicklen und dann hier eintragen
						_mqttClient->publish(statusChannel, "flaschengeist connected");
						_mqttClient->subscribe(inChannel);
						_mqttClient->publish(outChannel, "0");
					}
					else
					{
						Serial.print("failed, rc=");
						Serial.print(_mqttClient->state());
					}
				}
				return _mqttClient->connected();
			}

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
			if(staticMqttClient and staticMqttClient->connected())
				staticMqttClient->publish(outChannel, "2");
		}
		else
		{
			CLedStrip::getStrip_ptr()->switch_program(0);
			if(staticMqttClient and staticMqttClient->connected())
				staticMqttClient->publish(outChannel, "0");
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

	void callback(char* topic, byte* payload, size_t length)
	{
		String topicStr = topic;
		if(topicStr == String(inChannel))
		{
			Serial.print("Message arrived [");
			Serial.print(topic);
			Serial.print("] ");
			for (size_t i = 0; i < length; i++) {
				Serial.print((char)payload[i]);
			}
			Serial.println();

			CLedStrip* strip = CLedStrip::getStrip_ptr();
			// Switch on the LED if an 1 was received as first character
			if ((char)payload[0] == '1') {
				if(strip->getConf().old_prog != 2)
					strip->switch_program(2);
				staticMqttClient->publish(outChannel, "2");
				digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
			}
			else
			{
				strip->switch_program(0);
				staticMqttClient->publish(outChannel, "0");
				digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
			}
			staticMqttClient->publish(statusChannel, "updated");

		}
		else
			staticMqttClient->publish(statusChannel, String(String("invalid topic: ") + String(topic)).c_str());

	}
}

