#include <Arduino.h>

#include "LightsourceUtils.h"
#include "LightsourceHTTPHandlers.h"

const char* ssid = "FAST3764-ED38";
const char* password = "54dkl182";

WiFiUDP Udp;
Ticker ntp;
SH1106 display(0x3c, D3, D5);
extern ESP8266WebServer httpServer;
extern ESP8266HTTPUpdateServer httpUpdater;
time_t prevDisplay = 0; // when the digital clock was displayed
int function = 0;
LightsourceRpc lightsourceRpc;
LightsourceStrips lightsourceStrips;
ClickButton button1(A0, HIGH);

/*
 *  MAIN PROGRAM
 */
void setup(void){
  DBG_OUTPUT_PORT.begin(115200);
  delay(50);
  DBG("\n\n> starting\n");
  DBG("> display init\n");
  display.init();
  display.flipScreenVertically();
  display.setFont(Droid_Sans_Mono_12);
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  DBG("> wifi init\n");
  WiFi.begin(ssid, password);
  bool blink = true;
  while (WiFi.status() != WL_CONNECTED)
  {
    display.clear();
    display.drawString(0, 0, "->" + String(ssid));
    if (blink)
      display.drawString(48,26, "-.-");
    else
      display.drawString(48,26, "#.#");
    blink = !blink;
    display.display();
    DBG(".");
    delay(250);
  }
  DBG("\n");
  DBG("> connected to ssid: \"%s\" ip: %s\n", ssid, ip2str(WiFi.localIP()).c_str());
  registerHTTPHandlers();
  DBG("> start web server\n");
  httpServer.begin();

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
	display.clear();
    updateStatusDisplay();
	display.display();
	delay(500);
	lightsourceStrips.begin();
}

/*
 * Main Loop
 */
void loop(void)
{
  	httpServer.handleClient();
  	if (timeStatus() != timeNotSet)
	{
    	if (now() != prevDisplay)
		{
      		prevDisplay = now();
      		display.clear();
      		updateStatusDisplay();
      		updateClockDisplay();
      		display.display();
    	}
  	}

	button1.Update();
	if(button1.clicks != 0)
	{
		function = button1.clicks;
		DBG("> clicks: %d\n", function);
		function = 0;
	}

	delay(5);
}
