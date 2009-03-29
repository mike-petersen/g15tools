#include <libg15.h>
#include "G15Canvas.h"
#include <iostream>
#include <string>

using namespace G15Tools;

G15Canvas::G15Canvas(bool debug) : debug(debug)
{
	if (this->debug)
	{
		std::cerr << "G15Canvas(" << this << "): ";
		std::cerr << "Created." << std::endl;
	}
	this->canvas = new g15canvas;
}

G15Canvas::G15Canvas(const G15Canvas& in)
{
	this->debug = in.debug;
	if (this->debug)
	{
		std::cerr << "G15Canvas(" << this << "): ";
		std::cerr << "Created as copy from G15Canvas(" << &in << ")." << std::endl;
	}
	this->canvas = new g15canvas;
	memcpy(this->canvas->buffer, in.canvas->buffer, G15_BUFFER_LEN);
}

G15Canvas::~G15Canvas()
{
	if (this->debug)
	{
		std::cerr << "G15Canvas(" << this << "): ";
		std::cerr << "Destroyed." << std::endl;
	}
	delete this->canvas;
}

void G15Canvas::render(G15Screen &screen)
{
	if (!this->canvas->mode_cache)
	{
		if (this->debug)
		{
			std::cerr << "G15Canvas(" << this << "): ";
			std::cerr << "Rendering to G15Screen(" << &screen << ")." << std::endl;
		}
		screen.sendData((char *) this->canvas->buffer, G15_BUFFER_LEN);
	}
}

void G15Canvas::clearScreen(int color)
{
	if (this->debug)
	{
		std::cerr << "G15Canvas(" << this << "): ";
		std::cerr << "Clearing screen to " << ((color == G15_COLOR_BLACK) ? "black" : "white") << "." << std::endl;
	}
	g15r_clearScreen(this->canvas, color);
}

void G15Canvas::drawBar(int x1, int y1, int x2, int y2, int color, int num, int max, int type)
{
	if (this->debug)
	{
		std::cerr << "G15Canvas(" << this << "): ";
		std::cerr << "Drawing " << ((color == G15_COLOR_BLACK) ? "black" : "white") << " bar ";
		std::cerr << "of type " << type << " ";
		std::cerr << "from (" << x1 << "," << y1 << ") ";
		std::cerr << "to (" << x2 << "," << y2 << ") ";
		std::cerr << "filled to " << num << " of max " << max << "." << std::endl;
	}
	g15r_drawBar(this->canvas, x1, y1, x2, y2, color, num, max, type);
}

void G15Canvas::drawBigNum(int x1, int y1, int x2, int y2, int color, int num)
{
	if (this->debug)
	{
		std::cerr << "G15Canvas(" << this << "): ";
		std::cerr << "Drawing large " << ((color == G15_COLOR_BLACK) ? "black" : "white") << " number " << num;
		std::cerr << " from (" << x1 << "," << y1 << ") ";
		std::cerr << "to (" << x2 << "," << y2 << ")." << std::endl;
	}
	g15r_drawBigNum(this->canvas, x1, y1, x2, y2, color, num);
}

void G15Canvas::drawCircle(int x, int y, int r, bool fill, int color)
{
	if (this->debug)
	{
		std::cerr << "G15Canvas(" << this << "): ";
		std::cerr << "Drawing " << ((color == G15_COLOR_BLACK) ? "black" : "white") << " circle centered at (" << x << "," << y << ") with radius " << r << ", ";
		if (!fill)
		{
			std::cerr << "not ";
		}
		std::cerr << "filled." << std::endl;
	}
	g15r_drawCircle(this->canvas, x, y, r, fill, color);
}

void G15Canvas::drawLine(int x1, int y1, int x2, int y2, int color)
{
	if (this->debug)
	{
		std::cerr << "G15Canvas(" << this << "): ";
		std::cerr << "Drawing " << ((color == G15_COLOR_BLACK) ? "black" : "white") << " line ";
		std::cerr << "from (" << x1 << "," << y1 << ") ";
		std::cerr << "to (" << x2 << "," << y2 << ")." << std::endl;
	}
	g15r_drawLine(this->canvas, x1, y1, x2, y2, color);
}

void G15Canvas::drawRoundBox(int x1, int y1, int x2, int y2, bool fill, int color)
{
	if (this->debug)
	{
		std::cerr << "G15Canvas(" << this << "): ";
		std::cerr << "Drawing round " << ((color == G15_COLOR_BLACK) ? "black" : "white") << " box ";
		std::cerr << "from (" << x1 << "," << y1 << ") ";
		std::cerr << "to (" << x2 << "," << y2 << "), ";
		if (!fill)
		{
			std::cerr << "not ";
		}
		std::cerr << "filled." << std::endl;
	}
	g15r_drawRoundBox(this->canvas, x1, y1, x2, y2, fill, color);
}

void G15Canvas::drawBox(int x1, int y1, int x2, int y2, int color, int thick, bool fill)
{
	if (this->debug)
	{
		std::cerr << "G15Canvas(" << this << "): ";
		std::cerr << "Drawing " << ((color == G15_COLOR_BLACK) ? "black" : "white") << " box ";
		std::cerr << "from (" << x1 << "," << y1 << ") ";
		std::cerr << "to (" << x2 << "," << y2 << "), ";
		if (!fill)
		{
			std::cerr << "with edges " << thick << " pixels thick, not ";
		}
		std::cerr << "filled." << std::endl;
	}
	g15r_pixelBox(this->canvas, x1, y1, x2, y2, color, thick, fill);
}

void G15Canvas::drawOverlay(int x, int y, int width, int height, short colormap[])
{	
	if (this->debug)
	{
		std::cerr << "G15Canvas(" << this << "): ";
		std::cerr << "Drawing " << width << "x" << height << " overlay ";
		std::cerr << "at (" << x << "," << y << ")." << std::endl;
	}
	g15r_pixelOverlay(this->canvas, x, y, width, height, colormap);
}

void G15Canvas::drawCharacter(int size, int x, int y, unsigned char character, int sx, int sy)
{
	if (this->debug)
	{
		std::string textSize;
		switch (size)
		{
			case G15_TEXT_SMALL:
			{
				textSize = "small";
				break;
			}
			case G15_TEXT_MED:
			{
				textSize = "medium";
				break;
			}
			case G15_TEXT_LARGE:
			{
				textSize = "large";
				break;
			}

		}
		std::cerr << "G15Canvas(" << this << "): ";
		std::cerr << "Drawing " << textSize << " character \"" << character << "\" ";
		std::cerr << "at (" << x << "," << y << ")";
		if (sx > 0 || sy > 0)
		{
			std::cerr << " with offset (" << sx << "," << sy << ")";
		}
		std::cerr << "." << std::endl;
	}

	switch (size)
	{
		case G15_TEXT_SMALL:
		{
			g15r_renderCharacterSmall(this->canvas, x, y, character, sx, sy);
			break;
		}
		case G15_TEXT_MED:
		{
			g15r_renderCharacterMedium(this->canvas, x, y, character, sx, sy);
			break;
		}
		case G15_TEXT_LARGE:
		{
			g15r_renderCharacterLarge(this->canvas, x, y, character, sx, sy);
			break;
		}

	}
}

void G15Canvas::drawString(std::string stringOut, int row, int size, int sx, int sy)
{
	if (this->debug)
	{
		std::string textSize;
		switch (size)
		{
			case G15_TEXT_SMALL:
			{
				textSize = "small";
				break;
			}
			case G15_TEXT_MED:
			{
				textSize = "medium";
				break;
			}
			case G15_TEXT_LARGE:
			{
				textSize = "large";
				break;
			}

		}
		std::cerr << "G15Canvas(" << this << "): ";
		std::cerr << "Drawing " << textSize << " string \"" << stringOut << "\" ";
		std::cerr << "in row " << row;
		if (sx > 0 || sy > 0)
		{
			std::cerr << " with offset (" << sx << "," << sy << ")";
		}
		std::cerr << "." << std::endl;
	}
	g15r_renderString(this->canvas, (unsigned char*)stringOut.c_str(), row, size, sx, sy);
}

void G15Canvas::drawSplash(std::string filename)
{
	if (filename.length() < 1)
	{
		if (this->debug)
		{
		std::cerr << "G15Canvas(" << this << "): ";
		std::cerr << "Invalid filename provided to drawSplash: " << filename << "." << std::endl;
		}
		return;
	}

	if (this->debug)
	{
		std::cerr << "G15Canvas(" << this << "): ";
		std::cerr << "Drawing splashscreen from file: " << filename << "." << std::endl;
	}
	
	g15r_loadWbmpSplash(this->canvas, (char *)filename.c_str());
}

void G15Canvas::drawSprite(G15Wbmp &wbmp, int x, int y, int width, int height, int sx, int sy)
{
	if (this->debug)
	{
		std::cerr << "G15Canvas(" << this << "): ";
		std::cerr << "Drawing (" << width << "," << height << ") sprite ";
		std::cerr << "at (" << x << "," << y << ")";
		if (sx > 0 || sy > 0)
		{
			std::cerr << " with offset (" << sx << "," << sy << ")";
		}
		std::cerr << "." << std::endl;
	}

	g15r_drawSprite(this->canvas, (char *)wbmp.getBuffer(), x, y, width, height, sx, sy, wbmp.getWidth());
}

void G15Canvas::drawIcon(G15Wbmp &wbmp, int x, int y)
{
	if (this->debug)
	{
		std::cerr << "G15Canvas(" << this << "): ";
		std::cerr << "Drawing (" << wbmp.getWidth() << "," << wbmp.getHeight() << ") icon ";
		std::cerr << "at (" << x << "," << y << ")";
		std::cerr << "." << std::endl;
	}

	g15r_drawIcon(this->canvas, (char *)wbmp.getBuffer(), x, y, wbmp.getWidth(), wbmp.getHeight());
}

int G15Canvas::getPixel(int x, int y)
{
	int r = g15r_getPixel(this->canvas, x, y);
	if (this->debug)
	{
		std::cerr << "G15Canvas(" << this << "): ";
		std::cerr << "The pixel at (" << x << "," << y << ") has value " << r << "." << std::endl;
	}
	return r;
}

void G15Canvas::setPixel(int x, int y, int color)
{
	if (this->debug)
	{
		std::cerr << "G15Canvas(" << this << "): ";
		std::cerr << "Setting pixel at (" << x << "," << y << ") to " << color << "." << std::endl;
	}
	g15r_setPixel(this->canvas, x, y, color);
}
