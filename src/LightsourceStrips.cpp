#include "LightsourceStrips.h"

void LightsourceStrips::begin()
{
	DBG("LightsourceStrips::begin> \n");
}

bool LightsourceStrips::applyProgram(const JsonArray &programData)
{
	DBG("> LightsourceStrips::applyProgram\n");
	programData.printTo(Serial);
	DBG("\n");
	if (programData.success())
	{
		return setState(programData);
	}

	DBG("> LightsourceStrips::applyProgram programData is not vaild");
	return (false);
}

bool LightsourceStrips::loadProgram(String path)
{
	DBG("LightsourceStrips::loadProgram> %s\n", path.c_str());
	StaticJsonBuffer<2048> jsonBuffer;
	String configString = getFileContents(path);
	return applyProgram(jsonBuffer.parseArray(configString));
}

bool LightsourceStrips::setState(const JsonArray &jsonConfig)
{
	DBG("LightsourceStrips::setState> json config initialized, %d strips loaded\n", jsonConfig.size());
	for (int strip=0; strip<jsonConfig.size(); strip++)
	{
		if (jsonConfig[strip].success())
		{
			int pin = (int)jsonConfig[strip][STRIP_PIN];
			int len = (int)jsonConfig[strip][STRIP_LEN];
			uint16_t type = getTypeFromString(jsonConfig[strip][STRIP_TYPE]) + NEO_KHZ800;
			String hexColor = jsonConfig[strip][STRIP_COLOR];

			DBG("LightsourceStrips::setState> add strip: %d pin: %d, pixels: %d, type: %x, color %s\n", strip, pin, len, type, hexColor.c_str());
			pixelStrips[strip].setPin(pin);
			pixelStrips[strip].updateType(type);
			pixelStrips[strip].begin();

			for (int led=0; led<pixelStrips[strip].numPixels(); led++)
				pixelStrips[strip].setPixelColor(led, Adafruit_NeoPixel::Color(0,0,0));

			pixelStrips[strip].show();
			pixelStrips[strip].updateLength(len);

			for (int led=0; led<len; led++)
				pixelStrips[strip].setPixelColor(led, getColorFromHexString(hexColor.c_str()));

			/* there are sices in this strip */
			if (jsonConfig[strip].size() == 6)
				setSlices(pixelStrips[strip], jsonConfig[strip][5]);

			pixelStrips[strip].show();
		}
		else
		{
			DBG("Lightsource::setState> strip index %d invalid\n", strip);
			return (false);
		}
	}

	return (true);
}

void LightsourceStrips::setSlices(Adafruit_NeoPixel &strip, const JsonArray &jsonConfig)
{
	DBG("\tLightsourceStrips::setSlices> \n");
	for (int slice=0; slice<jsonConfig.size(); slice++)
	{
		int start 		= (int)jsonConfig[slice][2]-1;
		int len   		= (int)jsonConfig[slice][3];
		String hexColor	= jsonConfig[slice][1];

		DBG("\t\t%d> start: %d len: %d color: %s\n",
			slice,
			start,
			len,
			hexColor.c_str());

		for (int led=start; led<start+len; led++)
		{
			strip.setPixelColor(led, getColorFromHexString(hexColor.c_str()));
		}
	}
	DBG("\n");
}

bool LightsourceStrips::updateProgram(JsonObject &programData, String programName)
{
	DBG("LightsourceStrips::updateProgram> programName: \"%s\"\n", programName.c_str());

	StaticJsonBuffer<512> jsonBuffer;
	String programs = getFileContents("/programs.json");

	if (programs.length() <= 1)
	{
		DBG("LightsourceStrips::updateProgram> programs.json file is invalid\n");
		return (false);
	}

	JsonArray& jsonPrograms = jsonBuffer.parseArray(programs);

	if (jsonPrograms.success())
	{	
	}

	DBG("LightsourceStrips::updateProgram> can't parse programs.json file\n");
	return (false);
}

String LightsourceStrips::getFileForProgram(const String &programName)
{
	DBG("> LightsourceStrips::getFileForProgram %s", programName.c_str());
	return ("");
}
