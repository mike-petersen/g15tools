#ifndef G15CANVAS_H_
#define G15CANVAS_H_

#include <G15Screen.h>
#include <G15Wbmp.h>
#include <libg15render.h>
#include <string>

namespace G15Tools
{
	class G15Canvas {
	protected:
		g15canvas *canvas;
		bool debug;

	public:
		explicit G15Canvas(bool debug = false);
		G15Canvas(const G15Canvas& in);
		~G15Canvas();
		void render(G15Screen &screen);
		void clearScreen(int color);
		void drawBar(int x1, int y1, int x2, int y2, int color, int num, int max, int type);
		void drawBigNum(int x1, int y1, int x2, int y2, int color, int num);
		void drawCircle(int x, int y, int r, bool fill, int color);
		void drawLine(int x1, int y1, int x2, int y2, int color);
		void drawRoundBox(int x1, int y1, int x2, int y2, bool fill, int color);
		void drawBox(int x1, int y1, int x2, int y2, int color, int thick, bool fill);
		void drawOverlay(int x, int y, int width, int height, short colormap[]);
		void drawCharacter(int size, int x, int y, unsigned char character, int sx = 0, int sy = 0);
		void drawString(std::string stringOut, int row, int size, int sx = 0, int sy = 0);
		void drawSplash(std::string filename);
		void drawSprite(G15Wbmp &wbmp, int x, int y, int width, int height, int sx = 0, int sy = 0);
		void drawIcon(G15Wbmp &wbmp, int x, int y);

		void setPixel(int x, int y, int color);
		int getPixel(int x, int y);
	};
}

#endif
