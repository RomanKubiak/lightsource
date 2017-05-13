#include "LightsourceUtils.h"

extern WiFiUDP Udp;
extern SH1106 display;
IPAddress ntpTimeServer(194,146,251,100);
byte ntpPacketBuffer[NTP_PACKET_SIZE];
unsigned long lastNtpUpdateInMillis, epoch;
static int lastYield = 0;

String getFileContents(String path)
{
  DBG("> getFileContents(%s)\n", path.c_str());
  String allData = "{file: \"no data\"}";
  SPIFFS.begin();
  {
    DBG("> getFileContents(%s) SPIFFS.begin\n", path.c_str());
    if (SPIFFS.exists(path))
    {
      DBG("> getFileContents(%s) try to open\n", path.c_str());
      File file = SPIFFS.open(path, "r");
      DBG("> getFileContents(%s) SPIFFS.exists/open, size: %d\n", path.c_str(), file.size());
      allData = file.readStringUntil(EOF);
      file.close();
    }
    else
    {
      DBG("> getFileContents(%s) file does not exist\n", path.c_str());
    }
  }
  return (allData);
}

bool writeFileWithContents(String filename, String contents)
{
	DBG("> writeFileWithContents file:%s contents:\n%s\n", filename.c_str(), contents.c_str());
	File file = SPIFFS.open(filename.c_str(), "w");
	if (!file)
	{
		DBG("\n\t> writeFileWithContents open() failed");
		return (false);
	}
	file.write((uint8_t *)contents.c_str(), contents.length());
	file.close();
	return (true);
}

bool writeFileWithContents(String file, JsonObject &contents)
{
	String contentAsString;
	contents.printTo(contentAsString);
	return (writeFileWithContents(file, contents));
}

String getNewUniqueFilename()
{
	uint8_t uuidBuffer[16];
	String newFileName;
	uuid(uuidBuffer);
	newFileName = uuidToString(uuidBuffer);
	DBG("> getNewUniqueFilename %s", newFileName.c_str());
	return (newFileName);
}

String formatBytes(size_t bytes)
{
  if (bytes < 1024){
    return String(bytes)+"B";
  } else if(bytes < (1024 * 1024)){
    return String(bytes/1024.0)+"KB";
  } else if(bytes < (1024 * 1024 * 1024)){
    return String(bytes/1024.0/1024.0)+"MB";
  } else {
    return String(bytes/1024.0/1024.0/1024.0)+"GB";
  }
}

String ip2str(const IPAddress& ipAddress)
{
  return String(ipAddress[0]) + String(".") +\
  String(ipAddress[1]) + String(".") +\
  String(ipAddress[2]) + String(".") +\
  String(ipAddress[3])  ;
}

time_t getNtpTime()
{
  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  DBG("> getNtpTimeNTP() request\n");
  sendNTPpacket(ntpTimeServer);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 2500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      DBG("> getNtpTime() receive NTP Response\n");
      Udp.read(ntpPacketBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)ntpPacketBuffer[40] << 24;
      secsSince1900 |= (unsigned long)ntpPacketBuffer[41] << 16;
      secsSince1900 |= (unsigned long)ntpPacketBuffer[42] << 8;
      secsSince1900 |= (unsigned long)ntpPacketBuffer[43];
      return secsSince1900 - 2208988800UL + NTP_TIMEZONE * SECS_PER_HOUR;
    }
  }
  DBG("> getNtpTime() no NTP Response\n");
  return 0; // return 0 if unable to get the time
}

void sendNTPpacket(IPAddress &address)
{
  // set all bytes in the buffer to 0
  memset(ntpPacketBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  ntpPacketBuffer[0] = 0b11100011;   // LI, Version, Mode
  ntpPacketBuffer[1] = 0;     // Stratum, or type of clock
  ntpPacketBuffer[2] = 6;     // Polling Interval
  ntpPacketBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  ntpPacketBuffer[12]  = 49;
  ntpPacketBuffer[13]  = 0x4E;
  ntpPacketBuffer[14]  = 49;
  ntpPacketBuffer[15]  = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(ntpPacketBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}

void updateStatusDisplay()
{
	char buf[64];
	display.setFont(Orbitron_Light_8);
	sprintf(buf, "%s\0", ip2str(WiFi.localIP()).c_str());
	display.drawString(0,0,buf);
}

void updateClockDisplay()
{
	char buf[64];
	sprintf(buf, "%02d.%02d %02d:%02d:%02d", day(), month(), hour(), minute(), second());
	display.setFont(Orbitron_Light_12);
	display.drawString(8,24,buf);
}

int getTypeFromString(String type)
{
if (strcmp(type.c_str(), "NEO_RGB") == 0) return NEO_RGB;
if (strcmp(type.c_str(), "NEO_RBG") == 0) return NEO_RBG;
if (strcmp(type.c_str(), "NEO_GRB") == 0) return NEO_GRB;
if (strcmp(type.c_str(), "NEO_GBR") == 0) return NEO_GBR;
if (strcmp(type.c_str(), "NEO_BRG") == 0) return NEO_BRG;
if (strcmp(type.c_str(), "NEO_BGR") == 0) return NEO_BGR;
if (strcmp(type.c_str(), "NEO_WRGB") == 0) return NEO_WRGB;
if (strcmp(type.c_str(), "NEO_WRBG") == 0) return NEO_WRBG;
if (strcmp(type.c_str(), "NEO_WGRB") == 0) return NEO_WGRB;
if (strcmp(type.c_str(), "NEO_WGBR") == 0) return NEO_WGBR;
if (strcmp(type.c_str(), "NEO_WBRG") == 0) return NEO_WBRG;
if (strcmp(type.c_str(), "NEO_WBGR") == 0) return NEO_WBGR;
if (strcmp(type.c_str(), "NEO_RWGB") == 0) return NEO_RWGB;
if (strcmp(type.c_str(), "NEO_RWBG") == 0) return NEO_RWBG;
if (strcmp(type.c_str(), "NEO_RGWB") == 0) return NEO_RGWB;
if (strcmp(type.c_str(), "NEO_RGBW") == 0) return NEO_RGBW;
if (strcmp(type.c_str(), "NEO_RBWG") == 0) return NEO_RBWG;
if (strcmp(type.c_str(), "NEO_RBGW") == 0) return NEO_RBGW;
if (strcmp(type.c_str(), "NEO_GWRB") == 0) return NEO_GWRB;
if (strcmp(type.c_str(), "NEO_GWBR") == 0) return NEO_GWBR;
if (strcmp(type.c_str(), "NEO_GRWB") == 0) return NEO_GRWB;
if (strcmp(type.c_str(), "NEO_GRBW") == 0) return NEO_GRBW;
if (strcmp(type.c_str(), "NEO_GBWR") == 0) return NEO_GBWR;
if (strcmp(type.c_str(), "NEO_GBRW") == 0) return NEO_GBRW;
if (strcmp(type.c_str(), "NEO_BWRG") == 0) return NEO_BWRG;
if (strcmp(type.c_str(), "NEO_BWGR") == 0) return NEO_BWGR;
if (strcmp(type.c_str(), "NEO_BRWG") == 0) return NEO_BRWG;
if (strcmp(type.c_str(), "NEO_BRGW") == 0) return NEO_BRGW;
if (strcmp(type.c_str(), "NEO_BGWR") == 0) return NEO_BGWR;
if (strcmp(type.c_str(), "NEO_BGRW") == 0) return NEO_BGRW;
return NEO_RGB;
}

long long getColorFromHexString(const char *hexString)
{
	return (strtol( &hexString[1], NULL, 16));
}

int randomBit(void)
{
  // Needed to keep wifi stack running smoothly
  // And to avoid wdt reset
  if (lastYield == 0 || millis() - lastYield >= 50) {
    yield();
    lastYield = millis();
  }
  uint8_t bit = (int)RANDOM_REG32;

  return bit & 1;
}

char randomByte(void)
{
  char result = 0;
  uint8_t i;
  for (i=8; i--;) result += result + randomBit();
  return result;
}

void memfill(char* location, int size)
{
  for (;size--;) *location++ = randomByte();
}

void uuid(uint8_t* uuidLocation) {
  // Generate a Version 4 UUID according to RFC4122
  memfill((char*)uuidLocation,16);
  // Although the UUID contains 128 bits, only 122 of those are random.
  // The other 6 bits are fixed, to indicate a version number.
  uuidLocation[6] = 0x40 | (0x0F & uuidLocation[6]);
  uuidLocation[8] = 0x80 | (0x3F & uuidLocation[8]);
}

String uuidToString(uint8_t* uuidLocation) {
  String string = "";
  int i;
  for (i=0; i<16; i++) {
    if (i==4) string += "-";
    if (i==6) string += "-";
    if (i==8) string += "-";
    if (i==10) string += "-";
    int topDigit = uuidLocation[i] >> 4;
    int bottomDigit = uuidLocation[i] & 0x0f;
    // High hex digit
    string += "0123456789abcdef"[topDigit];
    // Low hex digit
    string += "0123456789abcdef"[bottomDigit];
  }

  return string;
}
