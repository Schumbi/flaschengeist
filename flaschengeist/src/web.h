#ifndef MAKELIGTH_WEB_H
#define MAKELIGTH_WEB_H

#include <ESP8266WebServer.h>

// initialize webserver
void init_web();

// std processing of requests
void www_std();

// processing of requests for LED
void www_led();

// not found processing
void www_404();

// webserver loop
void webwork();

#endif
