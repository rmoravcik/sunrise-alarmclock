#ifndef COMMON_H
#define COMMON_H

#include <ESP8266WebServer.h>
#include <DNSServer.h>

// #define SERIAL_DEBUG 1

extern ESP8266WebServer server;
extern DNSServer dns;
extern WiFiUDP udp;

#endif

