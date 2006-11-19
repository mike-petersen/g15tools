#define TTF_SUPPORT 1
#include <iostream>
#include <libg15.h>
#include <g15daemon_client.h>
#include <libg15render.h>
#include "g15logo.h"

using namespace std;

int const NUM_BYTES = G15_LCD_WIDTH * G15_LCD_HEIGHT / 8;
unsigned char test_data[NUM_BYTES];
extern short logo_data[];
int g15screen_fd = 0;
g15canvas *canvas;

void
updateScreen (void)
{
  if (!canvas->mode_cache)
    g15_send (g15screen_fd, (char *) canvas->buffer, 1048);
}

void
connectToScreen (void)
{
  if ((g15screen_fd = new_g15_screen (G15_G15RBUF)) < 0)
    {
      cout << "Sorry, cant connect to the G15daemon" << endl;
      exit (-1);
    }

  canvas = (g15canvas *) malloc (sizeof (g15canvas));

  g15r_initCanvas (canvas);
}

int
main (int argc, char *argv[])
{
  connectToScreen ();

  g15r_setPixel (canvas, 20, 20, 1);
  int ret = g15r_getPixel (canvas, 20, 20);
  cout << "g15r_getPixel(20, 20) returns " << ret << " which should be 1" <<
    endl;
  ret = g15r_getPixel (canvas, 21, 20);
  cout << "g15r_getPixel(21, 20) returns " << ret << " which should be 0" <<
    endl;

  g15r_setPixel (canvas, 20, 20, 0);
  ret = g15r_getPixel (canvas, 20, 20);
  cout << "g15r_getPixel(20, 20) returns " << ret << " which should be 0" <<
    endl;
  ret = g15r_getPixel (canvas, 21, 20);
  cout << "g15r_getPixel(21, 20) returns " << ret << " which should be 0" <<
    endl;

  int i = 0, x = 0, y = 0;
  unsigned char character = 0;

  g15r_clearScreen (canvas, 0);

  for (i = 0; i < 3;)
    {
      for (y = 0; y < 5; y++)
	{
	  for (x = 0; x < 20; x++)
	    {
	      g15r_renderCharacterLarge (canvas, x, y, character, 0, 0);
	      character++;
	    }
	}
      updateScreen ();
      i++;
      character = 100 * i;
      sleep (3);
    }

  g15r_clearScreen (canvas, 0);
  character = 0;

  for (i = 0; i < 3;)
    {
      for (y = 0; y < 6; y++)
	{
	  for (x = 0; x < 32; x++)
	    {
	      g15r_renderCharacterMedium (canvas, x, y, character, 0, 0);
	      character++;
	    }
	}
      updateScreen ();
      i++;
      character = 192 * i;
      sleep (3);
    }

  g15r_clearScreen (canvas, 0);
  character = 0;

  for (y = 0; y < 7; y++)
    {
      for (x = 0; x < 42; x++)
	{
	  g15r_renderCharacterSmall (canvas, x, y, character, 0, 0);
	  character++;
	}
    }
  updateScreen ();
  sleep (3);

  g15r_clearScreen (canvas, 0);

  g15r_pixelBox (canvas, 40, 20, 90, 40, 1, 1, 1);
  g15r_pixelBox (canvas, 10, 10, 50, 30, 1, 3, 0);
  updateScreen ();
  sleep (3);

  g15r_clearScreen (canvas, 0);

  g15r_drawCircle (canvas, 10, 21, 5, 0, 1);
  g15r_drawCircle (canvas, 110, 21, 15, 1, 1);
  updateScreen ();
  sleep (3);

  g15r_clearScreen (canvas, 0);

  g15r_drawRoundBox (canvas, 10, 21, 50, 40, 0, 1);
  g15r_drawRoundBox (canvas, 80, 15, 140, 30, 1, 1);
  updateScreen ();
  sleep (3);

  g15r_clearScreen (canvas, 0);

  g15r_drawBar (canvas, 10, 10, 150, 20, 1, 50, 100, 1);
  g15r_drawBar (canvas, 10, 22, 150, 32, 1, 75, 100, 2);
  g15r_drawBar (canvas, 10, 34, 150, 42, 1, 315, 900, 3);
  updateScreen ();
  sleep (3);

  g15r_clearScreen (canvas, 0);

  g15r_ttfLoad (canvas, "/usr/share/fonts/ttf-bitstream-vera/Vera.ttf", 16,
		1);
  g15r_ttfPrint (canvas, 0, 0, 16, 1, 1, 0, "This   is a     test");
  g15r_pixelBox (canvas, 0, 22, 159, 42, 1, 1, 1);
  g15r_ttfPrint (canvas, 0, 25, 16, 1, 0, 1, "This   is a     test");
  updateScreen ();
  sleep (3);

  g15r_clearScreen (canvas, 0);
  g15r_loadWbmpSplash (canvas, "./splash.wbmp");
  updateScreen ();
  sleep (3);

  g15r_clearScreen (canvas, 0);
  char *buf = (char *)malloc (G15_BUFFER_LEN);
  int width, height;
  g15r_loadWbmpToBuf (buf, "./splash.wbmp", &width, &height, G15_BUFFER_LEN);
  g15r_drawSprite (canvas, buf, 30, 10, 43, 20, 30, 10, width);
  g15r_drawSprite (canvas, buf, 10, 20, 15, 20, 10, 20, width);
  g15r_drawSprite (canvas, buf, 100, 20, 25, 20, 100, 20, width);
  updateScreen ();
  sleep (3);

  g15r_clearScreen (canvas, 0);
  g15r_pixelOverlay (canvas, 0, 0, 160, 43, logo_data);
  updateScreen ();
  sleep (3);

  g15r_clearScreen (canvas, 0);
  g15r_drawIcon (canvas, buf, 0, 0, width, height);
  updateScreen ();
  sleep (3);

  g15r_clearScreen (canvas, 0);
  g15r_drawBigNum (canvas, 0, 0, 20, 22, '3');
  g15r_drawBigNum (canvas, 25, 0, 45, 42, '0');
  updateScreen ();
  sleep (3);

  return 0;
}
