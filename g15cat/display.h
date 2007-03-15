/*
  g15cat is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  g15cat is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with g15daemon; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
  
  Copyright Antonio Bartolini - (c) 2007 - robynhub@gmail.com
  
*/

typedef struct{   
  char *data;
  struct linea* next;
  struct linea* prev;
  
} linea;

typedef struct {   
  int linelength;
  int maxlines;
  int numlines;
  char* format;
  linea* first;
  linea* last;
} display;

int display_add(display *disp, char *val, int linelen);
display * display_create(display *disp, int linelen, int maxlines, char *format);
int free_display(display *disp);

