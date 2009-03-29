#include <libg15.h>
#include "G15Canvas.h"
#include "G15Screen.h"
#include "G15Wbmp.h"
#include "g15logo.h"
#include <unistd.h>

using namespace G15Tools;

int main()
{
	bool debug = true;
	G15Screen screen = G15Screen(debug);
	screen.setKeyboardBacklight(G15_BRIGHTNESS_DARK);
	screen.setBacklight(G15_BRIGHTNESS_MEDIUM);
	screen.setContrast(G15_CONTRAST_HIGH);
	screen.setM1Led();
	screen.setM2Led();
	screen.setMRLed();
	G15Canvas canvas = G15Canvas(debug);
	G15Canvas c2 = canvas;
	canvas.clearScreen(G15_COLOR_WHITE);
	canvas.drawCircle(80, 27, 15, true, G15_COLOR_BLACK);
	canvas.drawCircle(90, 22, 15, true, G15_COLOR_WHITE);
	canvas.drawBar(10, 5, 150, 10, G15_COLOR_BLACK, 69, 100, 1);
	canvas.drawBox(95, 30, 140, 42, G15_COLOR_BLACK, 4, false);
	canvas.render(screen);
	sleep(3);
	canvas.clearScreen(G15_COLOR_WHITE);
	canvas.drawBigNum(10, 5, 35, 40, G15_COLOR_BLACK, 3);
	canvas.drawRoundBox(40, 15, 135, 40, true, G15_COLOR_BLACK);
	canvas.drawLine(40, 15, 135, 40, G15_COLOR_WHITE);
	canvas.drawLine(40, 40, 135, 15, G15_COLOR_WHITE);
	canvas.getPixel(41, 41);
	canvas.setPixel(44, 41, G15_COLOR_WHITE);
	canvas.render(screen);
	sleep(3);
	canvas.clearScreen(G15_COLOR_WHITE);
	canvas.drawOverlay(0, 0, 160, 43, logo_data);
	canvas.render(screen);
	sleep(3);
	canvas.clearScreen(G15_COLOR_WHITE);
	canvas.drawCharacter(G15_TEXT_LARGE, 0, 0, (unsigned char)'H');
	canvas.drawString(std::string("Hello World!"), 1, G15_TEXT_LARGE);
	canvas.render(screen);
	sleep(3);
	canvas.clearScreen(G15_COLOR_WHITE);
	canvas.drawSplash("./splash.wbmp");
	canvas.render(screen);
	sleep(3);

	G15Wbmp splash = G15Wbmp("./splash.wbmp", debug);
	canvas.clearScreen(G15_COLOR_WHITE);
	canvas.drawSprite(splash, 30, 10, 43, 20, 30, 10);
	canvas.drawSprite(splash, 10, 20, 15, 20, 10, 20);
	canvas.drawSprite(splash, 100, 20, 25, 20, 100, 20);
	canvas.render(screen);
	sleep(3);

	canvas.clearScreen(G15_COLOR_WHITE);
	canvas.drawIcon(splash, 0, 0);
	canvas.render(screen);
	sleep(3);
	return 0;
}
