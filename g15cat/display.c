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

#include "display.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int display_add(display *disp, char *val, int linelen){
  linea *l, *u;
  
  /* free the first data */
  free(disp->first->data);
  u = disp->first;
  disp->first = (linea *) disp->first->next;
  disp->first->prev = NULL;
  /* free frist linea */
  free(u);

  /* add the last */
  l = (linea *) malloc(sizeof(linea));
  l->data = (char *) malloc(sizeof(char) * linelen + 2);
  strcpy(l->data,val);
  disp->last->next = (struct linea *) l; 
  l->prev = (struct linea *) disp->last;
  l->next = NULL;
  disp->last = l;
  return 0;
}

display * display_create(display *disp, int linelen, int maxlines, char *format){
  int i;
  linea *prec;
  linea *latest;
  
  disp = (display *) malloc(sizeof(display));
  disp->first = (linea *) malloc(sizeof(linea));
  disp->first->data = (char *) malloc(sizeof(char) * linelen + 2);
  
  /* initialing data */
  strcpy(disp->first->data," ");
  disp->first->prev = NULL;
  latest = disp->first;

  for(i=0;i<maxlines;i++){
    /* moving pointer */
    prec = latest;
    /* latest = (linea *) latest->next; */

    /* fill data */
    latest = malloc(sizeof(linea));
    latest->prev = (struct linea *) prec;
    prec->next =  (struct linea *) latest; 
    /* initialing data */   
    latest->data = malloc(sizeof(char) * linelen + 2);
    strcpy(latest->data," ");
  }
  /* last one */
  disp->last = latest;
  disp->last->next = NULL;
  
  return disp;
}

int free_display(display *disp){
  linea *pointer;
  
  /* free data */
  pointer = disp->first;
  while (pointer->next != NULL){
    free(pointer->data);
     pointer = (linea *)pointer->next;
  }
  free(pointer->data);
  
  /* free lines */
  while (pointer->next != NULL){
    pointer = (linea *)pointer->prev;
    free(pointer->next);
  }
  /* free the last one */
  free(pointer);
  /* free display */
  free(disp);
  
  return 0;
}
