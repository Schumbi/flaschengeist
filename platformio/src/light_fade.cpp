#include <Arduino.h>

#define LEDPIN 16
#define CTLPIN 2
#define ON  0
#define OFF 1

void fade_leds();


unsigned int val = 0;

int led = LEDPIN;      // the PWM pin the LED is attached to
int brightness = 2;    // how bright the LED is
int brightness_max = 175;
int brightness_min = brightness;
int fadeAmount = 1;    // how many steps to fade
long int delay_ms = 10;

void fade_leds(int max_b, int min_b, int delay_b);

void fade_leds(int led, int max_b, int min_b, int delay_b)
{
  if (val == 0)
  {
    analogWrite(led, 0);
    return;
  }

  // set the brightness of pin 9:
  analogWrite(led, brightness);

  // change the brightness for next time through the loop:
  brightness = brightness + fadeAmount;

  // reverse the direction of the fading at the ends of the fade:
  if (brightness <= min_b || brightness >= max_b) {
    fadeAmount = -fadeAmount ;
	Serial.print(".");
  }
  if(!brightness) brightness = 1;
  // wait for 30 milliseconds to see the dimming effect
  delay(delay_b * (1.0 + ((float)max_b/(float)brightness)) );
}



void setup() {
  Serial.begin(115200);
  delay(10);

  // prepare GPIO2
  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, 0);

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
    fade_leds(brightness_max, brightness_min, delay_ms);
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
    val = 0;
  else if (req.indexOf("/gpio/1") != -1)
    val = 1;
  else {
    Serial.println("invalid request");
    client.stop();
    return;
  }

  client.flush();

  // Prepare the response
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nLichterkette ist nun ";
  s += (val) ? "angeschaltet" : "ausgeschaltet";
  s += "</html>\n";

  // Send the response to the client
  client.print(s);
  delay(1);
  Serial.println("Client disonnected");

  // The client will actually be disconnected
  // when the function returns and 'client' object is detroyed
  digitalWrite(CTLPIN, OFF);
}
