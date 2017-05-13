#ifndef LIGHTSOURCE_H
#define LIGHTSOURCE_H

#include <Time.h>
#include <TimeLib.h>
#include <SH1106.h>
#include <Ticker.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <FS.h>
#include <Adafruit_NeoPixel.h>
#include "font_roboto.h"

#define NTP_UDP_LOCAL_PORT	2390
#define NTP_PACKET_SIZE		48
#define	NTP_TIMEZONE		2
#define DBG_OUTPUT_PORT   	Serial
#define DBG(...)          	DBG_OUTPUT_PORT.printf( __VA_ARGS__ )

String getFileContents(String path);
bool writeFileWithContents(String file, String contents);
bool writeFileWithContents(String file, JsonObject& contents);
String getNewUniqueFilename();
void dbg(const char *fmt, ...);
String ip2str(const IPAddress& ipAddress);
void updateClockDisplay();
void updateStatusDisplay();
void sendNTPpacket(IPAddress &address);
time_t getNtpTime();
String formatBytes(size_t bytes);
int getTypeFromString(String type);
long long getColorFromHexString(const char *hexString);

void uuid(uint8_t* uuidLocation);
void memfill(char* location, int size);
char randomByte(void);
int randomBit(void);
String uuidToString(uint8_t* uuidLocation);
#endif
