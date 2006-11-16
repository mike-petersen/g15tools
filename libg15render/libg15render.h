#ifndef LIBG15RENDER_H_
#define LIBG15RENDER_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <string.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef TTF_SUPPORT
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_BITMAP_H
#endif

#define BYTE_SIZE 		8
#define G15_BUFFER_LEN  	1048
#define G15_LCD_OFFSET  	32
#define G15_LCD_HEIGHT  	43
#define G15_LCD_WIDTH   	160
#define G15_COLOR_WHITE 	0
#define G15_COLOR_BLACK 	1
#define G15_TEXT_SMALL  	0
#define G15_TEXT_MED    	1
#define G15_TEXT_LARGE  	2
#define G15_PIXEL_NOFILL 	0
#define G15_PIXEL_FILL  	1
#define G15_MAX_FACE		5

/** \brief This structure holds the data need to render objects to the LCD screen.*/
  typedef struct g15canvas
  {
/** g15canvas::buffer[] is a buffer holding the pixel data to be sent to the LCD.*/
    unsigned char buffer[G15_BUFFER_LEN];
/** g15canvas::mode_xor determines whether xor processing is used in g15r_setPixel.*/
    int mode_xor;
/** g15canvas::mode_cache can be used to determine whether caching should be used in an application.*/
    int mode_cache;
/** g15canvas::mode_reverse determines whether color values passed to g15r_setPixel are reversed.*/
    int mode_reverse;
#ifdef TTF_SUPPORT
    FT_Library ftLib;
    FT_Face ttf_face[G15_MAX_FACE][sizeof (FT_Face)];
    int ttf_fontsize[G15_MAX_FACE];
#endif
  } g15canvas;

/** \brief Fills an area bounded by (x1, y1) and (x2, y2)*/
  void g15r_pixelReverseFill (g15canvas * canvas, int x1, int y1, int x2,
			      int y2, int fill, int color);
/** \brief Overlays a bitmap of size width x height starting at (x1, y1)*/
  void g15r_pixelOverlay (g15canvas * canvas, int x1, int y1, int width,
			  int height, short colormap[]);
/** \brief Draws a line from (px1, py1) to (px2, py2)*/
  void g15r_drawLine (g15canvas * canvas, int px1, int py1, int px2, int py2,
		      const int color);
/** \brief Draws a box bounded by (x1, y1) and (x2, y2)*/
  void g15r_pixelBox (g15canvas * canvas, int x1, int y1, int x2, int y2,
		      int color, int thick, int fill);
/** \brief Draws a circle centered at (x, y) with a radius of r*/
  void g15r_drawCircle (g15canvas * canvas, int x, int y, int r, int fill,
			int color);
/** \brief Draws a box with rounded corners bounded by (x1, y1) and (x2, y2)*/
  void g15r_drawRoundBox (g15canvas * canvas, int x1, int y1, int x2, int y2,
			  int fill, int color);
/** \brief Draws a completion bar*/
  void g15r_drawBar (g15canvas * canvas, int x1, int y1, int x2, int y2,
		     int color, int num, int max, int type);
/** \brief Draw a splash screen from 160x43 wbmp file*/
int g15r_loadWbmpSplash(g15canvas *canvas, char *filename);
/** \brief Draw an icon to the screen from a wbmp buffer*/
void g15r_drawIcon(g15canvas *canvas, char *buf, int my_x, int my_y, int width, int height);
/** \brief Draw a sprite to the screen from a wbmp buffer*/
void g15r_drawSprite(g15canvas *canvas, char *buf, int my_x, int my_y, int width, int height, int start_x, int start_y, int total_width);
/** \brief Load a wbmp file into a buffer*/
int g15r_loadWbmpToBuf(char *buf, char *filename, int *img_width, int *img_height, int maxlen);
/** \brief Draw a large number*/
void g15r_drawBigNum (g15canvas * canvas, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, int num);

/** \brief Gets the value of the pixel at (x, y)*/
  int g15r_getPixel (g15canvas * canvas, unsigned int x, unsigned int y);
/** \brief Sets the value of the pixel at (x, y)*/
  void g15r_setPixel (g15canvas * canvas, unsigned int x, unsigned int y,
		      int val);
/** \brief Fills the screen with pixels of color*/
  void g15r_clearScreen (g15canvas * canvas, int color);
/** \brief Clears the canvas and resets the mode switches*/
  void g15r_initCanvas (g15canvas * canvas);

/** \brief Font data for the large (8x8) font*/
  extern unsigned char fontdata_8x8[];
/** \brief Font data for the medium (7x5) font*/
  extern unsigned char fontdata_7x5[];
/** \brief Font data for the small (6x4) font*/
  extern unsigned char fontdata_6x4[];

/** \brief Renders a character in the large font at (x, y)*/
  void g15r_renderCharacterLarge (g15canvas * canvas, int x, int y,
				  unsigned char character, unsigned int sx,
				  unsigned int sy);
/** \brief Renders a character in the meduim font at (x, y)*/
  void g15r_renderCharacterMedium (g15canvas * canvas, int x, int y,
				   unsigned char character, unsigned int sx,
				   unsigned int sy);
/** \brief Renders a character in the small font at (x, y)*/
  void g15r_renderCharacterSmall (g15canvas * canvas, int x, int y,
				  unsigned char character, unsigned int sx,
				  unsigned int sy);
/** \brief Renders a string with font size in row*/
  void g15r_renderString (g15canvas * canvas, unsigned char stringOut[],
			  int row, int size, unsigned int sx,
			  unsigned int sy);

#ifdef TTF_SUPPORT
/** \brief Loads a font through the FreeType2 library*/
  void g15r_ttfLoad (g15canvas * canvas, char *fontname, int fontsize,
		     int face_num);
/** \brief Prints a string in a given font*/
  void g15r_ttfPrint (g15canvas * canvas, int x, int y, int fontsize,
		      int face_num, int color, int center,
		      char *print_string);
#endif

#ifdef __cplusplus
}
#endif

#endif				/*LIBG15RENDER_H_ */
