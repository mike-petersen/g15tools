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
    along with g15tools; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef G15BASE_H_
#define G15BASE_H_

#include <iostream>
#include <cstdlib>
#include <string>
#include <getopt.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <string>
#include <vector>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

using namespace std;

class G15Base
{
public:
	G15Base();
	G15Base(string filename);
	virtual ~G15Base();
	string filename() {return fifo_filename;}
	void filename(string filename) {fifo_filename = filename;}
	
protected:
	int doOpen();
	int get_params(int*, std::string const &, int, int);
	string fifo_filename;
};

#endif /*G15BASE_H_*/
