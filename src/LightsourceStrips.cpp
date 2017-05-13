#include "LightsourceStrips.h"

void LightsourceStrips::begin()
{
	DBG("LightsourceStrips::begin> \n");
}

bool LightsourceStrips::applyConfig(const JsonVariant &programData)
{
	return (applyConfig(getFileForProgram(programData[0])));
}

bool LightsourceStrips::applyConfig(String path)
{
	DBG("LightsourceStrips::applyConfig> \n");
	StaticJsonBuffer<2048> jsonBuffer;
	String configString = getFileContents(path);
	JsonArray& jsonConfig = jsonBuffer.parseArray(configString);

	if (jsonConfig.success())
	{
		DBG("LightsourceStrips::applyConfig> json config initialized, %d strips loaded\n", jsonConfig.size());
		for (int strip=0; strip<jsonConfig.size(); strip++)
		{
			if (jsonConfig[strip].success())
			{
				int pin = (int)jsonConfig[strip][STRIP_PIN];
				int len = (int)jsonConfig[strip][STRIP_LEN];
				uint16_t type = getTypeFromString(jsonConfig[strip][STRIP_TYPE]) + NEO_KHZ800;
				String hexColor = jsonConfig[strip][STRIP_COLOR];

				DBG("LightsourceStrips::applyConfig> add strip: %d pin: %d, pixels: %d, type: %x, color %s\n", strip, pin, len, type, hexColor.c_str());
				pixelStrips[strip].setPin(pin);
				pixelStrips[strip].updateLength(len);
				pixelStrips[strip].updateType(type);
				pixelStrips[strip].begin();

				for (int led=0; led<len; led++)
					pixelStrips[strip].setPixelColor(led, getColorFromHexString(hexColor.c_str()));

				pixelStrips[strip].show();
			}
			else
			{
				DBG("Lightsource::applyConfig> strip index %d invalid\n", strip);
			}
		}
	}
	else
	{
		DBG("LightsourceStrips::applyConfig> json config failed to load: \n%s\n", configString.c_str());
	}

	return (true);
}

bool LightsourceStrips::updateProgram(JsonObject &programData, String programName)
{
	StaticJsonBuffer<1024> jsonBuffer;
	String fileTowriteTo;
	JsonObject& jsonPrograms = jsonBuffer.parseObject(getFileContents("/programs.json"));
	if (jsonPrograms.success())
	{
		if (jsonPrograms.containsKey(programName))
		{
			DBG("LightsourceStrips::updateProgram> program exits, replace and write");
			fileTowriteTo = (const char *)jsonPrograms[programName];
			return (writeFileWithContents(fileTowriteTo, jsonPrograms));
		}
		else
		{
			DBG("LightsourceStrips::updateProgram> program is new");
			fileTowriteTo = getNewUniqueFilename();
			jsonPrograms[programName] = fileTowriteTo;
			if (!writeFileWithContents(fileTowriteTo, programData))
			{
				DBG("LightsourceStrips::updateProgram> failed to write program data to file: %s", fileTowriteTo.c_str());
				return (false);
			}
			if (!writeFileWithContents("/programs.json", jsonPrograms))
			{
				DBG("LightsourceStrips::updateProgram> failed to write programs.json");
				return (false);
			}
		}

		return (true);
	}
	return (false);
}

String LightsourceStrips::getFileForProgram(const String &programName)
{
	DBG("> LightsourceStrips::getFileForProgram %s", programName.c_str());
	return ("");
}
