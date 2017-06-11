#include "LightsourceStrips.h"

void LightsourceStrips::begin()
{
	DBG("LightsourceStrips::begin> \n");
	loadProgram("/default.json");
}

/*
 * Load a program from a file on the FLASH
 */
bool LightsourceStrips::loadProgram(String path)
{
	DBG("LightsourceStrips::loadProgram> %s\n", path.c_str());
	StaticJsonBuffer<2048> jsonBuffer;
	String configString = getFileContents(path);
	return setState(jsonBuffer.parseArray(configString));
}

/*
 * Set the "live" state of the strip based on a JSON description
 */
bool LightsourceStrips::setState(const JsonArray &jsonConfig)
{
	DBG("LightsourceStrips::setState> json config initialized, %d strips loaded\n", jsonConfig[PROGRAM_DATA].size());
	jsonConfig.printTo(Serial);
	String programName = jsonConfig[PROGRAM_NAME];
	JsonArray &program = jsonConfig[PROGRAM_DATA];
	DBG("\n");
	for (int strip=0; strip<program.size(); strip++)
	{
		DBG("LightsourceStrips::setState> process strip %d\n", strip);
		if (program[strip].success())
		{
			int pin = (int)program[strip][STRIP_PIN];
			int len = (int)program[strip][STRIP_LEN];
			uint16_t type = getTypeFromString(program[strip][STRIP_TYPE]) + NEO_KHZ800;
			String hexColor = program[strip][STRIP_COLOR];

			DBG("LightsourceStrips::setState> add strip: %d pin: %d, pixels: %d, type: %x, color %s\n",
				strip,
				pin,
				len,
				type,
				hexColor.c_str());

			pixelStrips[strip].setPin(pin);
			pixelStrips[strip].updateType(type);
			pixelStrips[strip].begin();

			/* we need to turn all pixels off, this is tricky
				since if it's a new strip we don't know what
				was it's previous length so we can't turn off
				all the pixels, shoot in the dark, literaly */
			int pixelsToTurnOff = (pixelStrips[strip].numPixels() > 32) ? pixelStrips[strip].numPixels() : 128;
			DBG("Lightsource::setState> turning off %d pixels\n", pixelsToTurnOff);
			for (int led=0; led<pixelsToTurnOff; led++)
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

bool LightsourceStrips::updateProgram(const JsonArray &programData)
{
	const String programName = programData[PROGRAM_NAME];
	if (programName.length() <= 0)
	{
		DBG("LightsourceStrips::updateProgram> programName is empty\n");
		return (false);
	}

	DBG("LightsourceStrips::updateProgram> programName: \"%s\"\n", programName.c_str());
	String fileName = getFileForProgram(programName);

	if (fileName.length() == 0)
	{
		DBG("LightsourceStrips::updateProgram> can't parse programs.json file\n");
		return (false);
	}

	DBG("LightsourceStrips::updateProgram> writing program data to %s\n",
		fileName.c_str());

	bool ret = writeFileWithContents(fileName, programData);
	DBG("LightsourceStrips::updateProgram> get out of function with ret:%d\n", ret);
	return (ret);
}

String LightsourceStrips::getFileForProgram(const String &programName)
{
	DynamicJsonBuffer jsonBuffer;
	DBG("> LightsourceStrips::getFileForProgram %s\n", programName.c_str());
	String programFile;
	String programs = getFileContents("/programs.json");

	if (programs.length() <= 1)
	{
		DBG("LightsourceStrips::getFileForProgram> programs.json file is invalid\n");
		return ("");
	}
	JsonArray& jsonPrograms = jsonBuffer.parseArray(programs);

	if (jsonPrograms.success())
	{
		DBG("LightsourceStrips::getFileForProgram> parsed programs\n");
		jsonPrograms.printTo(Serial);
		DBG("\n");

		for (int i=0; i<jsonPrograms.size(); i++)
		{
			if (jsonPrograms[i][0] == programName)
			{
				programFile = (const char *)jsonPrograms[i][1];

				DBG("LightsourceStrips::getFileForProgram> program match %s in file %s\n",
					programName.c_str(),
					programFile.c_str());
				break;
			}
		}

		if (programFile.length() <= 0)
		{
			DBG("LightsourceStrips::getFileForProgram> can't find program or program filename corrupted\n");
			programFile = getNewUniqueFilename();
			JsonArray &newProgram = jsonPrograms.createNestedArray();
			newProgram.add(programName.c_str());
			newProgram.add(programFile.c_str());
			updateProgramsFile(jsonPrograms);
		}
	}

	DBG("LightsourceStrips::getFileForProgram> file to write program to: %s\n", programFile.c_str());
	return (programFile);
}

void LightsourceStrips::updateProgramsFile(const JsonArray &programs)
{
	DBG("LightsourceStrips::updateProgramsFile> \n");
	if (!writeFileWithContents("/programs.json", programs))
		DBG("LightsourceStrips::updateProgramsFile> FAILED\n");
	else
		DBG("LightsourceStrips::updateProgramsFile> SUCCESS\n");
}
