#ifndef LIGHTSOURCE_DISPLAY_H
#define LIGHTSOURCE_DISPLAY_H

#include "LightsourceUtils.h"
#include <OLEDDisplayUi.h>
class LightsourceDisplay
{
	public:
		LightsourceDisplay(){}
		void init();
		void update();
		void next();
		void previous();
		void confirm();
		void showLoadingScreen();
		
		static void loadingDraw(OLEDDisplay *display, LoadingStage* stage, uint8_t progress);
		static void loadingCallback();
	private:
		LoadingStage loadingStages[100];
};

#endif
