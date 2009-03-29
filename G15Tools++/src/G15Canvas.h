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
		explicit G15Canvas(const bool debug = false);
		G15Canvas(const G15Canvas& in);
		~G15Canvas();
		void render(G15Screen &screen);
		void clearScreen(const int color);
		void drawBar(const int x1, const int y1, const int x2, const int y2, const int color, const int num, const int max, const int type);
		void drawBigNum(const int x1, const int y1, const int x2, const int y2, const int color, const int num);
		void drawCircle(const int x, const int y, const int r, const bool fill, const int color);
		void drawLine(const int x1, const int y1, const int x2, const int y2, const int color);
		void drawRoundBox(const int x1, const int y1, const int x2, const int y2, const bool fill, const int color);
		void drawBox(const int x1, const int y1, const int x2, const int y2, const int color, const int thick, const bool fill);
		void drawOverlay(const int x, const int y, const int width, const int height, short colormap[]);
		void drawCharacter(const int size, const int x, const int y, const unsigned char character, const int sx = 0, const int sy = 0);
		void drawString(const std::string stringOut, const int row, const int size, const int sx = 0, const int sy = 0);
		void drawSplash(const std::string filename);
		void drawSprite(G15Wbmp &wbmp, const int x, const int y, const int width, const int height, const int sx = 0, const int sy = 0);
		void drawIcon(G15Wbmp &wbmp, const int x, const int y);

		void setPixel(const int x, const int y, const int color);
		int getPixel(const int x, const int y);
	};
}

#endif
