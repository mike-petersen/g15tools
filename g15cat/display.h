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

#ifndef DISPLAY_H_
#define DISPLAY_H_

/* element of the linked list */
typedef struct{   
  char *data;
  struct linea* next;
  struct linea* prev;
  
} linea;


/* Linked list */
typedef struct {   
  int linelength; /* length of the line */ 
  int numlines;   /* number of the lines to keep in the linked list*/
  char* format;   /* format of text */
  linea* first;   /* pointer to the first line */
  linea* last;    /* pointer to the last one */
} display;


/* function for display.c */
int display_add(display *disp, char *val, int linelen);
display * display_create(display *disp, int linelen, int maxlines, char *format);
int free_display(display *disp);

#endif /* DISPLAY_H_ */
