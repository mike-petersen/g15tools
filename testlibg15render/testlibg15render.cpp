
#include <iostream>
#include <libg15.h>
#include <libg15render.h>
#include "g15logo.h"

using namespace std;

int const NUM_BYTES = G15_LCD_WIDTH*G15_LCD_HEIGHT/8;
unsigned char test_data[NUM_BYTES];
extern short logo_data[];

int main(int argc, char *argv[])
{
  int ret = 0;
  cout << "Trying to init libg15" << endl;
  ret = initLibG15();
  cout << "Done, return value is " << ret << endl;
  
  g15canvas canvas;
  g15canvas *p_canvas = &canvas;
  g15r_initCanvas(p_canvas);

  g15r_setPixel(p_canvas, 20, 20, 1);
  ret = g15r_getPixel(p_canvas, 20, 20);
  cout << "g15r_getPixel(20, 20) returns " << ret << " which should be 1" << endl;
  ret = g15r_getPixel(p_canvas, 21, 20);
  cout << "g15r_getPixel(21, 20) returns " << ret << " which should be 0" << endl;

  g15r_setPixel(p_canvas, 20, 20, 0);
  ret = g15r_getPixel(p_canvas, 20, 20);
  cout << "g15r_getPixel(20, 20) returns " << ret << " which should be 0" << endl;
  ret = g15r_getPixel(p_canvas, 21, 20);
  cout << "g15r_getPixel(21, 20) returns " << ret << " which should be 0" << endl;

  int i=0, x=0, y=0;
  unsigned char character=0;
  
  g15r_clearScreen(p_canvas, 0);

  for (i=0;i<3;)
  {
  	for (y=0;y<5;y++)
  	{
		for (x=0;x<20;x++)
		{
			g15r_renderCharacterLarge(p_canvas, x, y, character, 0, 0);
			character++;
		}
  	}
  	writePixmapToLCD(p_canvas->buffer);
	i++;
	character = 100 * i;
	sleep(3);
  }

  g15r_clearScreen(p_canvas, 0);
  character = 0;

  for (i=0;i<3;)
  {
  	for (y=0;y<6;y++)
  	{
		for (x=0;x<32;x++)
		{
			g15r_renderCharacterMedium(p_canvas, x, y, character, 0, 0);
			character++;
		}
  	}
  	writePixmapToLCD(p_canvas->buffer);
	i++;
	character = 192 * i;
	sleep(3);
  }

  g15r_clearScreen(p_canvas, 0);
  character = 0;

  for (y=0;y<7;y++)
  {
	for (x=0;x<42;x++)
	{
		g15r_renderCharacterSmall(p_canvas, x, y, character, 0, 0);
		character++;
	}
  }
  writePixmapToLCD(p_canvas->buffer);
  sleep(3);

  g15r_clearScreen(p_canvas, 0);

  g15r_pixelBox(p_canvas, 40, 20, 90, 40, 1, 1, 1);
  g15r_pixelBox(p_canvas, 10, 10, 50, 30, 1, 3, 0);
  writePixmapToLCD(p_canvas->buffer);
  sleep(3);

  g15r_clearScreen(p_canvas, 0);
  g15r_pixelOverlay(p_canvas, 0, 0, 160, 43, logo_data);
  writePixmapToLCD(p_canvas->buffer);
  sleep(3);

  return 0;
}
