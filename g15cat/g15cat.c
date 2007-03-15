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

/* header standard */
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <g15daemon_client.h>
#include <libg15.h>
#include <libg15render.h>

#include "display.h"
#include "g15cat.h"

char *version="1.0";
char *format="S";
char *buffer;
display *disp;
int debug=0;
int topdown=0;
int g15fd;
int seconds=5;
int dim =  G15_TEXT_SMALL;

int refresh(display *);



struct sigaction action; /* struct to handle signals */

/* init routine to handle signals */
void siginit(void){
  
  /* assegnamento gestione segnale SIGINT a client_exit() */
  IFERROR3(sigaction(SIGINT,NULL,&action),"sigaction",exit(errno));
  action.sa_handler=client_exit;
  IFERROR3(sigaction(SIGINT,&action,NULL),"sigaction",exit(errno));
  /* assegnamento gestione segnale SIGQUIT a client_exit() */
  IFERROR3(sigaction(SIGQUIT,NULL,&action),"sigaction",exit(errno));
  action.sa_handler=client_exit;
  IFERROR3(sigaction(SIGQUIT,&action,NULL),"sigaction",exit(errno));
  /* assegnamento gestione segnale SIGTERM a client_exit() */
  IFERROR3(sigaction(SIGTERM,NULL,&action),"sigaction",exit(errno));
  action.sa_handler=client_exit;
  IFERROR3(sigaction(SIGTERM,&action,NULL),"sigaction",exit(errno));
}

void client_exit(int st){
  free_display(disp);
  /* DEBUG("bye. \n");*/
  exit(st);
}


/* print the main usage */
void main_usage(void){
  /* print help */
  
  printf("g15cat ver.%s - GNU Software - (c) 2007 Antonio Bartolini\n\nUsage:\n", version);
  printf("  g15cat [-s SECONDS] [-f FORMAT] [-d]\n\n  Where possibile options are:\n");
  printf("  -f FORMAT = S, M or L [default S]\n");
  printf("  -t = Display data in Top-Down mode\n");
  printf("  -s SECONDS = Wait n. SECONDS until exit [default 5]\n");
  printf("  -d = Debug mode\n\n");
  printf("  Report bugs to robynhub@gmail.com\n");
  /* exit */
  exit(0); 
}


/* refresh routine */

int refresh(display *buf){
  char *buffer;
  linea *pointer;
  g15canvas *canvas;
  int lineno=0;
  
  /* empty buffer */
  strcpy(buffer," ");
  canvas = (g15canvas *) malloc (sizeof (g15canvas));
  if (canvas != NULL) {
    memset(canvas->buffer, 0, G15_BUFFER_LEN);
    canvas->mode_cache = 0;
    canvas->mode_reverse = 0;
    canvas->mode_xor = 0;
  }
  
  if (topdown)
    pointer = buf->last;
  else
    pointer = buf->first;
  do{  
    g15r_renderString (canvas, (unsigned char *)pointer->data, lineno, dim, 0, 0);
    lineno++;
    if (topdown)
      pointer = (linea *) pointer->prev;
    else
      pointer = (linea *) pointer->next;
  } while (pointer!= NULL);
  
  /* send data */
  g15_send(g15fd,(char *)canvas->buffer,G15_BUFFER_LEN);
  free(canvas);
  return 0;
}

/* main loop */

void main_loop(void){
  int num_chars;
  int num_lines;
  int counter;
  char c;
  
  if(*format == 'S'){
    num_chars = TS_CHARS;
    num_lines = TS_LINES;
    dim =  G15_TEXT_SMALL; 
  } else if(*format == 'M'){
    num_chars = TM_CHARS;
    num_lines = TM_LINES;
    dim =  G15_TEXT_MED;
  } else if(*format == 'L'){
    num_chars = TL_CHARS;
    num_lines = TL_LINES;
    dim =  G15_TEXT_LARGE;
  } else {
    main_usage();
  }
  
  buffer = malloc((num_chars + 2) * sizeof(char)); 
  
  disp = display_create(disp, num_chars,num_lines,format);
  g15fd = new_g15_screen(G15_G15RBUF);
  
  if(g15fd < 0){
    printf("Can't connect to daemon!\n");
    client_exit(255);
  }
  
  while ((c = getchar()) && !feof(stdin) ){
    if (c == '\n' || counter == num_chars){
      /* add the line */
      display_add(disp,buffer,num_chars);
      /* refresh display */
      refresh(disp);
      /* empty buffer */
      strcpy(buffer,"");
      
      /* ignore rest of line */
      if (counter == num_chars)
	do { c = getchar(); } while (c != '\n' || feof(stdin) );
      counter = 0;
      
    } else {
      strncat(buffer, &c, 1);
      counter++;
    }
  }
  sleep(seconds);
  free(buffer);
  client_exit(0);
  
}


int main(int argc,char **argv){
  
  int option;
  
  siginit();
  while ((option = getopt(argc,argv,"tdhf:s:")) != -1){
    
    switch(option){
    case 's':
      seconds=atoi(optarg);
      break;       
    case 'f':
      format=optarg;
      break;      
    case 'd':
      debug = 1;
      break;
    case 't':
      topdown = 1;
      break;
    case 'h':
      main_usage();
    }
  }
  
  main_loop(); 
  return 0;  /* never reached */
}




