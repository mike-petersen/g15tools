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


/* Add data to the end of linked list and free the first one */

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
  l->data = (char *) calloc(linelen + 2, sizeof(char));
  strcpy(l->data,val);
  disp->last->next = (struct linea *) l; 
  l->prev = (struct linea *) disp->last;
  l->next = NULL;
  disp->last = l;
  return 0;
}

int display_newline(display *disp, int linelen){
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
  l->data = (char *) calloc(linelen + 2, sizeof(char));
  disp->last->next = (struct linea *) l; 
  l->prev = (struct linea *) disp->last;
  l->next = NULL;
  disp->last = l;
  return 0;
}

int display_remline(display *disp, int linelen){
  linea *l, *u;
  /* free the last data */
  free(disp->last->data);
  u = disp->last;
  disp->last = (linea *) disp->last->prev;
  disp->last->next = NULL;
  /* free last linea */
  free(u);
   /* add the first */
  l = (linea *) malloc(sizeof(linea));
  l->data = (char *) calloc(linelen + 2, sizeof(char));
  disp->first->prev = (struct linea *) l; 
  l->next = (struct linea *) disp->first;
  l->prev = NULL;
  disp->first = l;
  return 0; 

}

int display_add_char(display *disp, char c){
  strncat(disp->last->data,&c,1);
  return 0;
}

int display_len(display *disp){
  return strlen(disp->last->data);
}

int display_rem_char(display *disp, int linelen){
  disp->last->data[display_len(disp) - 1] = '\0';
   if (display_len(disp) == 0)
     display_remline(disp, linelen);
  return 0;
}

/* create a new linked list of lines */
display * display_create(display *disp, int linelen, int numlines, char *format){
  int i;
  linea *prec;
  linea *latest;
  
  disp = (display *) malloc(sizeof(display));
  disp->first = (linea *) malloc(sizeof(linea));
  disp->first->data = (char *) calloc(linelen + 2, sizeof(char));
  
  /* initialing data */
  disp->first->prev = NULL;
  latest = disp->first;
  
  for(i=0;i<numlines;i++){
    /* moving pointer */
    prec = latest;
    /* fill pointers data */
    latest = malloc(sizeof(linea));
    latest->prev = (struct linea *) prec;
    prec->next =  (struct linea *) latest; 
    /* fill with empty data */   
    latest->data = calloc(linelen + 2, sizeof(char));
  }
  /* last one */
  disp->last = latest;
  disp->last->next = NULL;

   /* maybe for future use */
  disp->numlines = numlines; 
  return disp;
}


/* free world with free memory */
int free_display(display *disp){
  linea *pointer;
  
  /* check for correct args */
  if (disp == NULL)
    return -1;
  
  /* free data */
  pointer = disp->first;
  /* clean each data until ends */
  while (pointer->next != NULL){
    free(pointer->data);
    pointer = (linea *)pointer->next;
  }
  /* cleaning the last one */
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
