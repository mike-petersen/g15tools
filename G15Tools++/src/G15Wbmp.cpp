#include <libg15.h>
#include <libg15render.h>
#include "G15Wbmp.h"
#include <iostream>
#include <string>

using namespace G15Tools;

G15Wbmp::G15Wbmp(const char *filename, const bool debug) : debug(debug)
{
	if (this->debug)
	{
		std::cerr << "G15Wbmp(" << this << "): ";
		std::cerr << "Created from file " << filename << "." << std::endl;
	}
	this->buffer = g15r_loadWbmpToBuf((char *)filename, &this->width, &this->height);
}

G15Wbmp::G15Wbmp(const G15Wbmp& in)
{
	this->debug = in.debug;
	if (this->debug)
	{
		std::cerr << "G15Wbmp(" << this << "): ";
		std::cerr << "Created as copy from G15Wbmp(" << &in << ")." << std::endl;
	}
	this->width = in.width;
	this->height = in.height;
	// FIXME: This is likely a bad calculation of the size of this->buffer
	memcpy(this->buffer, in.buffer, (this->width * this->height) + 5);
}

G15Wbmp::~G15Wbmp()
{
	if (this->debug)
	{
		std::cerr << "G15Wbmp(" << this << "): ";
		std::cerr << "Destroyed." << std::endl;
	}
	free(this->buffer);
}
