#ifndef LIBG15RENDER_H_
#define LIBG15RENDER_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <string.h>

#ifndef _LIBG15_H_
enum {
   BYTE_SIZE = 8,
   G15_LCD_OFFSET = 32,
   G15_LCD_HEIGHT = 43,
   G15_LCD_WIDTH = 160
};

enum {
   G15_BUFFER_LEN = 0x03e0
};
#endif /*_LIBG15_H_*/
  
typedef struct g15canvas {
    unsigned char buffer[G15_LCD_WIDTH * G15_LCD_HEIGHT];
    int mode_xor;
    int mode_cache;
    int mode_reverse;
} g15canvas;

void g15r_pixelReverseFill(g15canvas *, int, int, int, int, int, int);
void g15r_pixelOverlay(g15canvas *, int, int, int, int, int []);
void g15r_drawLine(g15canvas *, int, int, int, int, const int);
void g15r_pixelBox(g15canvas *, int, int, int, int, int, int, int);

int g15r_getPixel(g15canvas *, unsigned int, unsigned int);
void g15r_setPixel(g15canvas *, unsigned int, unsigned int, int);
void g15r_clearScreen(g15canvas *, int);
void g15r_initCanvas(g15canvas *);

extern unsigned char fontdata_8x8[];
extern unsigned char fontdata_7x5[];
extern unsigned char fontdata_6x4[];
void g15r_renderCharacterLarge(g15canvas *, int, int, unsigned char, unsigned int, unsigned int);
void g15r_renderCharacterMedium(g15canvas *, int, int, unsigned char, unsigned int, unsigned int);
void g15r_renderCharacterSmall(g15canvas *, int, int, unsigned char, unsigned int, unsigned int);
void g15r_renderString(g15canvas *, unsigned char [], int, int, unsigned int, unsigned int);

#ifdef __cplusplus
}
#endif

#endif /*LIBG15RENDER_H_*/
