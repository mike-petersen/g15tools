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

#include "g15cat.h"

/* config variables */
char *version = G15CAT_VERSION;      /* version of this program */
char *format = "S";                  /* default format of text  */
char *buffer;                        /* buffer to render        */
display *disp;                       /* linked list for storage data */
g15canvas *canvas;		     /* g15canvas struct for screen data */
int slow_mode = 0;                   /* slowdown mode also for debug */
int topdown = 0;                     /* direction of the text   */
int g15fd;                           /* file descriptor of the g15 display */
int seconds = 2;                     /* seconds to wait until exit */
int slow;                            /* If >0 wait. Else go wild  */
int shell_mode = 0;                  /* if 1 don't cut final chars but \n and continue */
int cut = 0;                         /* number of chars that need to be cutted */
int dim =  G15_TEXT_SMALL;           /* size of the text */
FILE * fd;                           /* file descriptor of input */
pid_t pid;


struct sigaction action; /* struct to handle signals */

/* init routine to handle signals */
void siginit(void){
  
  /* signal assignment: SIGINT to client_exit() */
  IFERROR3(sigaction(SIGINT,NULL,&action),"sigaction",exit(errno));
  action.sa_handler=client_exit;
  IFERROR3(sigaction(SIGINT,&action,NULL),"sigaction",exit(errno));
  /* signal assignment: SIGQUIT to client_exit() */
  IFERROR3(sigaction(SIGQUIT,NULL,&action),"sigaction",exit(errno));
  action.sa_handler=client_exit;
  IFERROR3(sigaction(SIGQUIT,&action,NULL),"sigaction",exit(errno));
  /* signal assignment: SIGTERM to client_exit() */
  IFERROR3(sigaction(SIGTERM,NULL,&action),"sigaction",exit(errno));
  action.sa_handler=client_exit;
  IFERROR3(sigaction(SIGTERM,&action,NULL),"sigaction",exit(errno));
}

/* clean exit on signals */
void client_exit(int st){
  free_display(disp);
  exit(st);
}


/* print the main usage */
void main_usage(void){
  printf("g15cat ver. %s - GNU Software - (c) 2007 Antonio Bartolini\n", version);
  printf("part of G15tools - http://g15tools.sourceforge.net/\n\nUsage:\n");
  printf("  g15cat [-F FILENAME] [-s SECONDS] [-f FORMAT] [-c NUMCHARS] [-t] [-S] [-d SECONDS]\n\n  Where possibile options are:\n");
  printf("  -F FILENAME = Read Filename as Input. [default STDIN]\n");
  printf("  -s SECONDS = Wait n. SECONDS until exit. [default 5]\n");
  printf("  -S  = Shell mode (see man). \n");
  printf("  -f FORMAT = S, M or L. [default S]\n");
  printf("  -c NUMCHARS = Cut first NUMCHARS characters from the begin of each line.\n");
  printf("  -t = Display data in Top-Down mode.\n");
  printf("  -d SECONDS = Wait n. SECONDS before printing each line (Slow mode).\n");
  printf("  -h = Display this help.\n\n");
  printf("Report bugs to robynhub@gmail.com\n");
  /* exit 1 anyway */
  exit(1); 
}


/* refresh routine */
int refresh(display *buf){
  linea *pointer;
  int lineno=0;
  
  /* Clear the buffer in the g15canvas struct */
  g15r_clearScreen (canvas, 0);
  
  if (topdown)
    pointer = buf->last;
  else
    pointer = buf->first;
  do{  
    /* rendering data */
    g15r_renderString (canvas, (unsigned char *)pointer->data, lineno, dim, 0, 0);
    lineno++;
    if (topdown)
      pointer = (linea *) pointer->prev;
    else
      pointer = (linea *) pointer->next;
  } while (pointer!= NULL); /* until the buffer end */
  
  /* send data to the display*/
  g15_send(g15fd,(char *)canvas->buffer,G15_BUFFER_LEN);
  return TRUE;
}

/* main loop */

void main_loop(void){
  int num_chars = TS_LINES;
  int num_lines = TS_CHARS;
  int counter = 0;
  char c;
  
  /* parse dimension of the text and take the values */
  if(*format == 'S'){
    num_chars = TS_CHARS;
    num_lines = TS_LINES;
    dim =  G15_TEXT_SMALL; 
  } else if(*format == 'M'){
    num_chars = TM_CHARS;
    num_lines = TM_LINES;
    dim =  7;
  } else if(*format == 'L'){
    num_chars = TL_CHARS;
    num_lines = TL_LINES;
    dim =  11;
  } else {
    main_usage();
  }
  
  /* security check */
  if (cut < 0)
    cut = 0;
  
  
  /* create new canvas */
  canvas = (g15canvas *) calloc (1, sizeof (g15canvas));
  /* create display*/
  disp = display_create(disp, num_chars,num_lines,format);
  /* catch the screen */
  g15fd = new_g15_screen(G15_G15RBUF);
  
  if(g15fd < 0){
    printf("Can't connect to daemon!\n");
    client_exit(255);
  }
  /* fetch character while a new charater exist or EOF reached */
  while ((c = fgetc(fd))  && !feof(fd)){

    /* If there is some cutted characters skip it */
    if (counter < cut  && shell_mode == 0)
      counter++;
    else {

      /* ignore rest of line */
      if (display_len(disp) == num_chars && shell_mode == 0)
	do { c = fgetc(fd); } while (c != '\n' || feof(fd) );

      /* if it's at the end of the line or max number of charatcter reached 
	 and char isn't a meta char (8) */
      if (c == '\n' || ((display_len(disp) >= num_chars) && c != 8) ){
	/* add the new line */
	display_newline(disp,num_chars);
	counter = 0;
	/* if it was a normal char re-put it in the new line */
	if (c != '\n')
	  display_add_char(disp,c);
	/* slow mode */
	if(slow_mode)
	  sleep(slow);
      } else {
	/* add a new char */
	switch((int)c){
	case 7:
	  /* just ignore it */
	  break;
	case 8:
	  /* tryin to fight shell macro - sorry guys */
	  /* backspace */
	  c = fgetc(fd);
	  if (c == 27){
	    c = fgetc(fd);
	    if (c == 91){
	      c = fgetc(fd);
	      if (c == 75)	    
		display_rem_char(disp,num_chars);
	    }
	  }
	  break;
	default:
	  display_add_char(disp,c);
	}
      }
      /* refresh display anyway */
      refresh(disp);
    }
  } /* end main loop */
  /* wait until exit */
  sleep(seconds);
  /* free memory */
  free(canvas);
  /* close input fd */
  fclose(fd);
  /* clean exit */
  client_exit(0);
  
}


int main(int argc,char **argv){
  int option;
  
  /* catch the signals */
  siginit(); 
  /* default fd */
  fd = stdin;
  
  while ((option = getopt(argc,argv,"td:hf:s:c:F:S")) != -1){    
    switch(option){
    case 'F':
      fd = fopen(optarg, "r");
      if (fd == NULL){
	/* something goes wrong */
	printf("ERROR: Can't open file: %s\n",optarg);
	main_usage();
      }
      break;
    case 's':
      seconds = atoi(optarg);
      break; 
    case 'S':
      shell_mode = 1;
      break;
    case 'f':
      format = optarg;
      break;      
    case 'd':
      slow = atoi(optarg);
      slow_mode = TRUE;
      break;
    case 't':
      topdown = TRUE;
      break;
    case 'c':
      cut = atoi(optarg);
      break;
    case 'h':
      main_usage();
    default:
      /* if something goes wrong */
      main_usage();
    }
  }
  
  main_loop(); 
  return TRUE;  /* never reached */
  
}




