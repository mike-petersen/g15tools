/*
    This file is part of g15lcd.

    g15lcd is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    g15lcd is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with g15lcd; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef _LCD_H_
#define _LCD_H_

#include <pthread.h>
#include <string>

void lcdProcessingWorkflow(unsigned char *buffer, pthread_mutex_t *mutex, int *changed, std::string const &filename);

#endif
