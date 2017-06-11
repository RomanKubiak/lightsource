#ifndef LIGHTSOURCE_STRIPS_H
#define LIGHTSOURCE_STRIPS_H

#include <vector>
#include <stdlib.h>
#include "LightsourceUtils.h"

#define STRIP_PIN	0
#define STRIP_NAME 	1
#define STRIP_COLOR 2
#define STRIP_TYPE	3
#define STRIP_LEN	4

#define PROGRAM_NAME 0
#define PROGRAM_DATA 1

/*
static const uint8_t D0   = 16;
static const uint8_t D1   = 5;
static const uint8_t D2   = 4;
static const uint8_t D3   = 0;
static const uint8_t D4   = 2;
static const uint8_t D5   = 14;
static const uint8_t D6   = 12;
static const uint8_t D7   = 13;
static const uint8_t D8   = 15;
static const uint8_t D9   = 3;
static const uint8_t D10  = 1;
*/
class LightsourceStrips
{
	public:
    	LightsourceStrips(){}
    	~LightsourceStrips(){}
    	void begin();
		bool loadProgram(String path);
		bool setState(const JsonArray &jsonConfig);
		bool updateProgram(const JsonArray &programData);
		String getFileForProgram(const String &programName);
	private:
		void setSlices(Adafruit_NeoPixel &strip, const JsonArray &jsonConfig);

		void updateProgramsFile(const JsonArray &programs);
    	Adafruit_NeoPixel pixelStrips[10];
};
#endif
