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

#ifndef G15CAT_H_
#define G15CAT_H_

#include "display.h"

#define TM_CHARS 32
#define TS_CHARS 40
#define TL_CHARS 20
#define TM_LINES 5
#define TS_LINES 6
#define TL_LINES 4
#define G15CAT_VERSION "1.2"

#define TRUE 1
#define FALSE 0


/* if error clean exit */
#define IFERROR(s,m) if((s)==-1) {perror(m); client_exit(errno);}
/* if error print a message and execute variable */
#define IFERROR3(s,m,c) if((s)==-1) {perror(m); c;} 
/* if error clean abort without messages */              
#define IFERRORQ(s) if((s)==-1) {client_exit(status);}
/* debug routine */
#define DEBUG(m) if(debug) {printf("%s",m);}

/* functions declaration */
void client_exit(int);  
void siginit(void);
void main_usage(void);
void main_loop(void);
int refresh(display *);

#endif /* G15CAT_H_ */
