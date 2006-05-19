#    This file is part of g15lcd.
#
#    g15lcd is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 2 of the License, or
#    (at your option) any later version.
#
#    g15lcd is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with g15lcd; if not, write to the Free Software
#    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

all:
	g++ -O2 -Wall -pedantic -c font_8x8.cpp
	g++ -O2 -Wall -pedantic -c font_7x5.cpp
	g++ -O2 -Wall -pedantic -c font_6x4.cpp
	g++ -O2 -Wall -pedantic -c lcd.cpp
	g++ -O2 -Wall -pedantic -c keys.cpp
	g++ -O2 -Wall -pedantic -c main.cpp
	g++ main.o lcd.o keys.o font_8x8.o font_7x5.o font_6x4.o -o g15lcd -lusb -lpthread
	
clean:
	rm -f *.o
	rm -f g15lcd
