#include <Arduino.h>
#include "LightsourceUtils.h"
#include "LightsourceHTTPHandlers.h"
#include "LightsourceDisplay.h"
#include <queue>
#include "images.h"

const char* ssid = "FAST3764-ED38";
const char* password = "54dkl182";

WiFiUDP Udp;
Ticker ntp;
extern ESP8266WebServer httpServer;
extern ESP8266HTTPUpdateServer httpUpdater;
LightsourceRpc lightsourceRpc;
LightsourceStrips lightsourceStrips;
LightsourceDisplay lightsourceDisplay;
ClickButton button1(A0, HIGH);
bool wifOk;
/*
 *  MAIN PROGRAM
 */
void setup(void){
  DBG_OUTPUT_PORT.begin(115200);
  delay(50);
  DBG("\n\n> starting\n");
  DBG("> display\n");
  lightsourceDisplay.init();
  DBG("> wifi init\n");
  WiFi.begin(ssid, password);
  bool blink = true;
  byte wifiCheck = 0;

  while (WiFi.status() != WL_CONNECTED)
  {
    DBG(".");
    delay(50);
    wifiCheck++;

    if (wifiCheck == 600)
    {
      /* we waited for 30 seconds, give up */
      wifOk = false;
      break;
    }
  }
  DBG("\n");
  if (wifOk)
  {
    DBG("> connected to ssid: \"%s\" ip: %s\n", ssid, ip2str(WiFi.localIP()).c_str());
    registerHTTPHandlers();
    DBG("> start web server\n");
    httpServer.begin();
  }
  else
  {
    DBG("> wifi connect timed out after 30 seconds\n");
  }
  
  DBG("> init spiffs, all files:\n");
  SPIFFS.begin();
  {
    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) {
      String fileName = dir.fileName();
      size_t fileSize = dir.fileSize();
      DBG(">\t%s [%s]\n", fileName.c_str(), formatBytes(fileSize).c_str());
    }
  }

	Udp.begin(NTP_UDP_LOCAL_PORT);
	DBG("> start UDP for NTP protocol, local port: %d\n", NTP_UDP_LOCAL_PORT);
	setSyncProvider(getNtpTime);
	DBG("> button setup");
	button1.debounceTime   = 20;   // Debounce timer in ms
  	button1.multiclickTime = 250;  // Time limit for multi clicks
  	button1.longClickTime  = 1000; // time until "held-down clicks" register
	DBG("> main program start\n");
	lightsourceRpc.begin();
	delay(500);
	lightsourceStrips.begin();
}

/*
 * Main Loop
 */
void loop(void)
{
  	httpServer.handleClient();
	button1.Update();
	if(button1.clicks != 0)
	{
		if (button1.clicks == 1)
			lightsourceDisplay.next();
		if (button1.clicks == 2)
			lightsourceDisplay.previous();
		if (button1.clicks == -1)
				lightsourceDisplay.confirm();
	}
	lightsourceDisplay.update();
	delay(5);
}
