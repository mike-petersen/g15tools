#include "G15Screen.h"
#include <libg15.h>
#include <g15daemon_client.h>
#include <iostream>

using namespace G15Tools;

G15Screen::G15Screen(const bool debug) : debug(debug)
{
	this->_init(G15_G15RBUF);
}

G15Screen::G15Screen(const int type, const bool debug) : debug(debug)
{
	this->_init(type);
}

G15Screen::G15Screen(const G15Screen& in)
{
	this->debug = in.debug;
	if (this->debug)
	{
		std::cerr << "G15Screen(" << this << "): ";
		std::cerr << "Created as copy from G15Screen(" << &in << ")." << std::endl;
	}
	this->_init(in.type);
	this->keys = in.keys;
}

G15Screen::~G15Screen()
{
	if (this->debug)
	{
		std::cerr << "G15screen(" << this << "): ";
		std::cerr << "Destroyed." << std::endl;
	}
	g15_close_screen(this->g15screen_fd);
}

void G15Screen::_init(const int type)
{
	this->g15screen_fd = new_g15_screen(type);
	this->keys = 0;
	this->type = type;
	if (this->debug)
	{
		std::cerr << "G15screen(" << this << "): ";
		std::cerr << "Created with g15screen_fd=" << this->g15screen_fd << "." << std::endl;
	}
}

int G15Screen::_sendCommand(const unsigned char command, const unsigned char value)
{
	if (this->debug)
	{
		std::cerr << "G15screen(" << this << "): ";
		std::cerr << "Sending command: " << (int)command << " with value: " << (int)value << "." << std::endl;
	}
	return g15_send_cmd(this->g15screen_fd, command, value);
}

int G15Screen::sendData(const char *data, const unsigned int len)
{
	if (this->debug)
	{
		std::cerr << "G15screen(" << this << "): ";
		std::cerr << "Sending " << len << " bytes." << std::endl;
	}
	return g15_send(this->g15screen_fd, (char *)data, len);
}

int G15Screen::setKeyboardBacklight(const unsigned char brightness)
{
	return this->_sendCommand(G15DAEMON_KB_BACKLIGHT, brightness);
}

int G15Screen::setBacklight(const unsigned char brightness)
{
	return this->_sendCommand(G15DAEMON_BACKLIGHT, brightness);
}

int G15Screen::setContrast(const unsigned char contrast)
{
	return this->_sendCommand(G15DAEMON_CONTRAST, contrast);
}

int G15Screen::setM1Led(const bool on)
{
	if (on)
	{
		this->keys = this->keys | G15_LED_M1;
	}
	else
	{
		this->keys = this->keys & ~G15_LED_M1;
	}
	return this->_sendCommand(G15DAEMON_MKEYLEDS, this->keys);
}

int G15Screen::setM2Led(const bool on)
{
	if (on)
	{
		this->keys = this->keys | G15_LED_M2;
	}
	else
	{
		this->keys = this->keys & ~G15_LED_M2;
	}
	return this->_sendCommand(G15DAEMON_MKEYLEDS, this->keys);
}

int G15Screen::setM3Led(const bool on)
{
	if (on)
	{
		this->keys = this->keys | G15_LED_M3;
	}
	else
	{
		this->keys = this->keys & ~G15_LED_M3;
	}
	return this->_sendCommand(G15DAEMON_MKEYLEDS, this->keys);
}

int G15Screen::setMRLed(const bool on)
{
	if (on)
	{
		this->keys = this->keys | G15_LED_MR;
	}
	else
	{
		this->keys = this->keys & ~G15_LED_MR;
	}
	return this->_sendCommand(G15DAEMON_MKEYLEDS, this->keys);
}

int G15Screen::getKeystate()
{
	return this->_sendCommand(G15DAEMON_GET_KEYSTATE, 0);
}
