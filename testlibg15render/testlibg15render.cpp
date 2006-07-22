
#include <iostream>
#include <libg15.h>
#include <libg15render.h>

using namespace std;

int const NUM_BYTES = G15_LCD_WIDTH*G15_LCD_HEIGHT/8;
unsigned char test_data[NUM_BYTES];

int main(int argc, char *argv[])
{
  int ret = 0;
  cout << "Trying to init libg15" << endl;
  ret = initLibG15();
  cout << "Done, return value is " << ret << endl;
  
  g15canvas canvas;
  g15canvas *p_canvas = &canvas;
  g15r_initCanvas(p_canvas);

  int x=0, y=0;
  unsigned char character=0;
  
  for (y=0;y<5;y++)
  {
	for (x=0;x<20;x++)
	{
		g15r_renderCharacterLarge(p_canvas, x, y, character, 0, 0);
		character++;
	}
  }
  writeBufferToLCD(p_canvas->buffer);
  return 0;
}
