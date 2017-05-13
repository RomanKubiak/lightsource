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

class LightsourceStrips
{
	public:
    	LightsourceStrips(){}
    	~LightsourceStrips(){}
    	void begin();
		void applyConfig(String path);
		bool updateProgram(JsonObject &programData, String programName="default");

	private:

    	Adafruit_NeoPixel pixelStrips[6];
};
#endif
