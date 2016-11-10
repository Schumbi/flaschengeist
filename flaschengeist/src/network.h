#ifndef FLASCHENGEIST_NETWORK_H
#define FLASCHENGEIST_NETWORK_H

#include <Arduino.h>

class WebSocketsClient;


namespace ns_net
{
	class Network_impl;

	class Network
	{
		public:
			
			virtual ~Network();

			bool connected();
			
			void webwork();

			WebSocketsClient* GetSocket();

			static Network* GetNetwork();
			static Network* StartNetwork(String ssid, String passphrase, String mdnsname = "" );

		protected:
			Network(String ssid, String password, String mdnsname = "");
		private:
			Network_impl *d;

	};
}

#endif // FLASCHENGEIST_NETWORK_H
