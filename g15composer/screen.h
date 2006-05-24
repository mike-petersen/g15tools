/*
    This file is part of g15tools.

    g15tools is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    g15tools is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with g15lcd; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef G15COMPOSER_SCREEN_H_
#define G15COMPOSER_SCREEN_H_

#include <string>

int getPixel(unsigned int x, unsigned int y);
void setPixel(unsigned int x, unsigned int y, int val);

void clearScreen(int color = 0);
void updateScreen(bool force = false);

void handleModeCommand(std::string const &input_line);

#endif
