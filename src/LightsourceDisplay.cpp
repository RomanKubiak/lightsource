#include "LightsourceDisplay.h"
#include <SH1106.h>
#include <OLEDDisplayUi.h>
#include "images.h"

SH1106 display(0x3c, D3, D5);
OLEDDisplayUi ui(&display);

void drawNetStatus(OLEDDisplay *d, OLEDDisplayUiState* state)
{
	char buf[64];
	d->setTextAlignment(TEXT_ALIGN_RIGHT);
	d->setFont(Droid_Sans_Mono_8);
	sprintf(buf, "%s\0", ip2str(WiFi.localIP()).c_str());
	d->drawString(128,0,buf);
}

void drawTime(OLEDDisplay *d, OLEDDisplayUiState* state)
{
	char buf[64];
	d->setTextAlignment(TEXT_ALIGN_LEFT);
    d->setFont(Droid_Sans_Mono_8);
	sprintf(buf, "%02d:%02d:%02d", hour(), minute(), second());
	d->setFont(Droid_Sans_Mono_8);
	d->drawString(0,0,buf);
}

void drawMainFrame(OLEDDisplay *d,
	OLEDDisplayUiState* state,
	int16_t x,
	int16_t y)
{
	d->drawXbm(x + 34, y + 12, pentagram_width, pentagram_height, pentagram_bits);
}

void drawSettingsFrame(OLEDDisplay *d,
	OLEDDisplayUiState* state,
	int16_t x,
	int16_t y)
{
	d->drawXbm(x, y + 14, cog_width, cog_height, cog_bits);
}

FrameCallback frames[] = { drawMainFrame, drawSettingsFrame };
OverlayCallback overlays[] = { drawTime, drawNetStatus };

void LightsourceDisplay::init()
{
	DBG("LightsourceDisplay::init> display init\n");
	ui.init();
	ui.setTargetFPS(24);
	ui.setOverlays(overlays, 2);
	ui.setFrames(frames, 2);
	ui.disableAutoTransition();
	ui.disableIndicator();
	ui.setTimePerTransition(250);
    display.flipScreenVertically();
}

void LightsourceDisplay::next()
{
	ui.nextFrame();
}
void LightsourceDisplay::previous()
{
	ui.previousFrame();
}

void LightsourceDisplay::confirm()
{
}

void LightsourceDisplay::update()
{
	ui.update();
}



/*
void updateStatusDisplay()
{
	char buf[64];
	display.setFont(Droid_Sans_Mono_8);
	sprintf(buf, "%s\0",
		ip2str(WiFi.localIP()).c_str());
	display.drawString(0,0,buf);
}

void updateClockDisplay()
{
	char buf[64];
	sprintf(buf, "%02d:%02d:%02d", hour(), minute(), second());
	display.setFont(Droid_Sans_Mono_8);
	display.drawString(80,0,buf);
}
*/
