/*
    This file is part of g15daemon.

    g15daemon is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    g15daemon is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with g15daemon; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
    
    (c) 2006 Mike Lampard, Philip Lawatsch, and others
    
    This daemon listens on localhost port 15550 for client connections,
    and arbitrates LCD display.  Allows for multiple simultaneous clients.
    Client screens can be cycled through by pressing the 'L1' key.
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <sys/socket.h>
#include <libdaemon/daemon.h>
#include "g15daemon.h"
#include "libg15.h"

extern int leaving;
unsigned int connected_clients = 0;

/* handy function from xine_utils.c */
void *g15_xmalloc(size_t size) {
    void *ptr;

    /* prevent xmalloc(0) of possibly returning NULL */
    if( !size )
        size++;

    if((ptr = calloc(1, size)) == NULL) {
        daemon_log(LOG_WARNING, "g15_xmalloc() failed: %s.\n", strerror(errno));
        return NULL;
    }
    return ptr;
}


lcd_t * create_lcd () {

    lcd_t *lcd = g15_xmalloc (sizeof (lcd_t));
    lcd->max_x = LCD_WIDTH;
    lcd->max_y = LCD_HEIGHT;
    lcd->backlight_state = G15_BRIGHTNESS_MEDIUM;
    lcd->mkey_state = G15_LED_MR;
    lcd->contrast_state = G15_CONTRAST_MEDIUM;
    lcd->state_changed = 1;
    
    return (lcd);
}

void quit_lcd (lcd_t * lcd) {
    free (lcd);
}


/* set a pixel in a libg15 buffer */
void setpixel(lcd_t *lcd, unsigned int x, unsigned int y, unsigned int val)
{
    unsigned int curr_row = y;
    unsigned int curr_col = x;

    unsigned int pixel_offset = curr_row * LCD_WIDTH + curr_col;
    unsigned int byte_offset = pixel_offset / 8;
    unsigned int bit_offset = 7-(pixel_offset % 8);

    if (val)
        lcd->buf[byte_offset] = lcd->buf[byte_offset] | 1 << bit_offset;
    else
        lcd->buf[byte_offset] = lcd->buf[byte_offset]  &  ~(1 << bit_offset);
}

void convert_buf(lcd_t *lcd, unsigned char * orig_buf)
{
    unsigned int x,y;
    for(x=0;x<160;x++)
        for(y=0;y<43;y++)
            setpixel(lcd,x,y,orig_buf[x+(y*160)]);
}
                                        

/* wrap the libg15 function */
void write_buf_to_g15(lcd_t *lcd)
{
    pthread_mutex_lock(&g15lib_mutex);
    writePixmapToLCD(lcd->buf);
    pthread_mutex_unlock(&g15lib_mutex);
    return;
}

void line (lcd_t * lcd, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int colour) {

    int d, sx, sy, dx, dy;
    unsigned int ax, ay;

    x1 = x1 - 1;
    y1 = y1 - 1;
    x2 = x2 - 1;
    y2 = y2 - 1;

    dx = x2 - x1;
    ax = abs (dx) << 1;
    if (dx < 0)
        sx = -1;
    else
        sx = 1;

    dy = y2 - y1;
    ay = abs (dy) << 1;
    if (dy < 0)
        sy = -1;
    else
        sy = 1;

    /* set the pixel */
    setpixel (lcd, x1, y1, colour);

    if (ax > ay)
    {
        d = ay - (ax >> 1);
        while (x1 != x2)
        {
            if (d >= 0)
            {
                y1 += sy;
                d -= ax;
            }
            x1 += sx;
            d += ay;
            setpixel (lcd, x1, y1, colour);

        }
    }
    else
    {
        d = ax - (ay >> 1);
        while (y1 != y2)
        {
            if (d >= 0)
            {
                x1 += sx;
                d -= ay;
            }
            y1 += sy;
            d += ax;
            setpixel (lcd, x1, y1, colour);
        }
    }
}


void rectangle (lcd_t * lcd, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, int filled, unsigned int colour) {

    int y;

    if (x1 != x2 && y1 != y2)
    {
        if (!filled)
        {
            line (lcd, x1, y1, x2, x1, colour);
            line (lcd, x1, y1, x1, y2, colour);
            line (lcd, x1, y2, x2, y2, colour);
            line (lcd, x2, y1, x2, y2, colour);
        }
        else
        {
            for (y = y1; y <= y2; y++)
            {
                line(lcd,x1,y,x2,y,colour);
            }
        }
    }
}


void draw_bignum (lcd_t * lcd, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int colour, int num) {
    x1 += 2;
    x2 -= 2;

    switch(num){
        case 45: 
            rectangle (lcd, x1, y1+((y2/2)-2), x2, y1+((y2/2)+2), 1, BLACK);
            break;
        case 46:
            rectangle (lcd, x2-5, y2-5, x2, y2 , 1, BLACK);
            break;
        case 48:
            rectangle (lcd, x1, y1, x2, y2 , 1, BLACK);
            rectangle (lcd, x1 +5, y1 +5, x2 -5, y2 - 6, 1, WHITE);
            break;
        case 49: 
            rectangle (lcd, x2-5, y1, x2, y2 , 1, BLACK);
            rectangle (lcd, x1, y1, x2 -5, y2, 1, WHITE);
            break;
        case 50:
            rectangle (lcd, x1, y1, x2, y2 , 1, BLACK);
            rectangle (lcd, x1, y1+5, x2 -5, y1+((y2/2)-3), 1, WHITE);
            rectangle (lcd, x1+5, y1+((y2/2)+3), x2 , y2-6, 1, WHITE);
            break;
        case 51:
            rectangle (lcd, x1, y1, x2, y2 , 1, BLACK);
            rectangle (lcd, x1, y1+5, x2 -5, y1+((y2/2)-3), 1, WHITE);
            rectangle (lcd, x1, y1+((y2/2)+3), x2-5 , y2-6, 1, WHITE);
            break;
        case 52:
            rectangle (lcd, x1, y1, x2, y2 , 1, BLACK);
            rectangle (lcd, x1, y1+((y2/2)+3), x2 -5, y2, 1, WHITE);
            rectangle (lcd, x1+5, y1, x2-5 , y1+((y2/2)-3), 1, WHITE);
            break;
        case 53:
            rectangle (lcd, x1, y1, x2, y2 , 1, BLACK);
            rectangle (lcd, x1+5, y1+5, x2 , y1+((y2/2)-3), 1, WHITE);
            rectangle (lcd, x1, y1+((y2/2)+3), x2-5 , y2-6, 1, WHITE);
            break;
        case 54:
            rectangle (lcd, x1, y1, x2, y2 , 1, BLACK);
            rectangle (lcd, x1+5, y1+5, x2 , y1+((y2/2)-3), 1, WHITE);
            rectangle (lcd, x1+5, y1+((y2/2)+3), x2-5 , y2-6, 1, WHITE);
            break;
        case 55:
            rectangle (lcd, x1, y1, x2, y2 , 1, BLACK);
            rectangle (lcd, x1, y1+5, x2 -5, y2, 1, WHITE);
            break;
        case 56:
            rectangle (lcd, x1, y1, x2, y2 , 1, BLACK);
            rectangle (lcd, x1+5, y1+5, x2-5 , y1+((y2/2)-3), 1, WHITE);
            rectangle (lcd, x1+5, y1+((y2/2)+3), x2-5 , y2-6, 1, WHITE);
            break;
        case 57:
            rectangle (lcd, x1, y1, x2, y2 , 1, BLACK);
            rectangle (lcd, x1+5, y1+5, x2-5 , y1+((y2/2)-3), 1, WHITE);
            rectangle (lcd, x1, y1+((y2/2)+3), x2-5 , y2, 1, WHITE);
            break;
        case 58: 
            rectangle (lcd, x2-5, y1+5, x2, y1+10 , 1, BLACK);
            rectangle (lcd, x2-5, y2-10, x2, y2-5 , 1, BLACK);
            break;

    }
}

/* initialise a new displaylist, and add an initial node at the tail (used for the clock) */
lcdlist_t *lcdlist_init () {
    
    lcdlist_t *displaylist = NULL;
    
    pthread_mutex_init(&lcdlist_mutex, NULL);
    pthread_mutex_lock(&lcdlist_mutex);
    
    displaylist = g15_xmalloc(sizeof(lcdlist_t));
    
    displaylist->head = g15_xmalloc(sizeof(lcdnode_t));
    
    displaylist->tail = displaylist->head;
    displaylist->current = displaylist->head;
    
    displaylist->head->lcd = create_lcd();
    displaylist->head->lcd->mkey_state = 0;
    
    displaylist->head->prev = displaylist->head;
    displaylist->head->next = displaylist->head;
    displaylist->head->list = displaylist;
    
    pthread_mutex_unlock(&lcdlist_mutex);
    return displaylist;
}

lcdnode_t *lcdnode_add(lcdlist_t **display_list) {
    
    lcdnode_t *new = NULL;
    
    pthread_mutex_lock(&lcdlist_mutex);
    
    new = g15_xmalloc(sizeof(lcdnode_t));
    new->prev = (*display_list)->current;
    new->next = NULL; 
    new->lcd = create_lcd();
    
    (*display_list)->current->next=new;
    (*display_list)->current = new;
    (*display_list)->head = new;
    (*display_list)->head->list = *display_list;
    
    pthread_mutex_unlock(&lcdlist_mutex);
    
    return new;
}

void lcdnode_remove (lcdnode_t *oldnode) {
    
    lcdlist_t **display_list = NULL;
    lcdnode_t **prev = NULL;
    lcdnode_t **next = NULL;
    
    pthread_mutex_lock(&lcdlist_mutex);
    
    display_list = &oldnode->list;
    prev = &oldnode->prev;
    next = &oldnode->next;
    
    quit_lcd(oldnode->lcd);
    
    if((*display_list)->current == oldnode) {
        (*display_list)->current = oldnode->prev;
    	(*display_list)->current->lcd->state_changed = 1;
    }
    
    if(oldnode->next!=NULL){
        (*next)->prev = oldnode->prev;
    }else{
        (*prev)->next = NULL;
        (*display_list)->head = oldnode->prev;
    }

    free(oldnode);
    
    pthread_mutex_unlock(&lcdlist_mutex);
}

void lcdlist_destroy(lcdlist_t **displaylist) {
    
    int i = 0;
    
    while ((*displaylist)->head != (*displaylist)->tail) {
        i++;
        lcdnode_remove((*displaylist)->head);
    }
    
    if(i)
        daemon_log(LOG_INFO,"removed %i stray client nodes",i);
    
    free((*displaylist)->tail->lcd);
    free((*displaylist)->tail);
    free(*displaylist);
    
    pthread_mutex_destroy(&lcdlist_mutex);
}

/* Sleep routine (hackish). */
void pthread_sleep(int seconds) {
    pthread_mutex_t dummy_mutex;
    static pthread_cond_t dummy_cond = PTHREAD_COND_INITIALIZER;
    struct timespec timeout;

    /* Create a dummy mutex which doesn't unlock for sure while waiting. */
    pthread_mutex_init(&dummy_mutex, NULL);
    pthread_mutex_lock(&dummy_mutex);

    timeout.tv_sec = time(NULL) + seconds;
    timeout.tv_nsec = 0;

    pthread_cond_timedwait(&dummy_cond, &dummy_mutex, &timeout);

    /*    pthread_cond_destroy(&dummy_cond); */
    pthread_mutex_unlock(&dummy_mutex);
    pthread_mutex_destroy(&dummy_mutex);
}

/* millisecond sleep routine. */
int pthread_msleep(int milliseconds) {
    
    struct timespec timeout;
    if(milliseconds>999)
        milliseconds=999;
    timeout.tv_sec = 0;
    timeout.tv_nsec = milliseconds*1000000;

    return nanosleep (&timeout, NULL);
}


void lcdclock(lcd_t *lcd)
{
    unsigned int col = 0;
    unsigned int len=0;
    int narrows=0;
    int totalwidth=0;
    char buf[10];
    
    time_t currtime = time(NULL);
    
    if(lcd->ident < currtime - 60) {	
        memset(lcd->buf,0,1024);
        memset(buf,0,10);
        strftime(buf,6,"%H:%M",localtime(&currtime));

        if(buf[0]==49) 
            narrows=1;

        len = strlen(buf); 

        if(narrows)
            totalwidth=(len*20)+(15);
        else
            totalwidth=len*20;

        for (col=0;col<len;col++) 
        {
            draw_bignum (lcd, (80-(totalwidth)/2)+col*20, 1,(80-(totalwidth)/2)+(col+1)*20, LCD_HEIGHT, BLACK, buf[col]);

        }
        lcd->ident = currtime;
    }
}


/* the client must send 6880 bytes for each lcd screen.  This thread will continue to copy data
* into the clients LCD buffer for as long as the connection remains open. 
* so, the client should open a socket, check to ensure that the server is a g15daemon,
* and send multiple 6880 byte packets (1 for each screen update) 
* once the client disconnects by closing the socket, the LCD buffer is 
* removed and will no longer be displayed.
*/
void *lcd_client_thread(void *display) {

    lcdnode_t *g15node = display;
    lcd_t *client_lcd = g15node->lcd;
    int retval;
    int i,y,x;
    unsigned int width, height, buflen,header=4;

    int client_sock = client_lcd->connection;
    char helo[]=SERV_HELO;
    unsigned char *tmpbuf=g15_xmalloc(6880);
    
    if(!connected_clients)
        setLEDs(G15_LED_MR); /* turn on the MR backlight to show that it's now being used for lcd-switching */
    connected_clients++;
    
    if(g15_send(client_sock, (char*)helo, strlen(SERV_HELO))<0){
        goto exitthread;
    }
    /* check for requested buffer type.. we only handle pixel buffers atm */
    if(g15_recv(g15node, client_sock,(char*)tmpbuf,4)<4)
        goto exitthread;

    /* we will in the future handle txt buffers gracefully but for now we just hangup */
    if(tmpbuf[0]=='G') {
        while(!leaving) {
            retval = g15_recv(g15node, client_sock,(char *)tmpbuf,6880);
            if(retval!=6880){
                break;
            }
            pthread_mutex_lock(&lcdlist_mutex);
            memset(client_lcd->buf,0,1024);      
            convert_buf(client_lcd,tmpbuf);
            client_lcd->ident = random();
            pthread_mutex_unlock(&lcdlist_mutex);
        }
    }
    else if (tmpbuf[0]=='R') { /* libg15render buffer */
        while(!leaving) {
            retval = g15_recv(g15node, client_sock, (char *)tmpbuf, 1048);
            if(retval != 1048) {
                break;
            }
            pthread_mutex_lock(&lcdlist_mutex);
            memcpy(client_lcd->buf,tmpbuf,sizeof(client_lcd->buf));
            client_lcd->ident = random();
            pthread_mutex_unlock(&lcdlist_mutex);
        }
    }
    else if (tmpbuf[0]=='W'){ /* wbmp buffer - we assume (stupidly) that it's 160 pixels wide */
        while(!leaving) {
            retval = g15_recv(g15node, client_sock,(char*)tmpbuf, 865);
            if(!retval)
                break;

            if (tmpbuf[2] & 1) {
                width = ((unsigned char)tmpbuf[2] ^ 1) | (unsigned char)tmpbuf[3];
                height = tmpbuf[4];
                header = 5;
            } else {
                width = tmpbuf[2];
                height = tmpbuf[3];
                header = 4;
            }
            
            buflen = (width/8)*height;

            if(buflen>860){ /* grab the remainder of the image and discard excess bytes */
                /*  retval=g15_recv(client_sock,(char*)tmpbuf+865,buflen-860);  */
                retval=g15_recv(g15node, client_sock,NULL,buflen-860); 
                buflen = 860;
            }
            
            if(width!=160) /* FIXME - we ought to scale images I suppose */
                goto exitthread;
            
            pthread_mutex_lock(&lcdlist_mutex);
            memcpy(client_lcd->buf,tmpbuf+header,buflen+header);
            client_lcd->ident = random();
            pthread_mutex_unlock(&lcdlist_mutex);
        }
    }
exitthread:
        close(client_sock);
    free(tmpbuf);
    lcdnode_remove(display);
    connected_clients--;
    if(!connected_clients)
        setLEDs(0);
    pthread_exit(NULL);
}

/* poll the listening socket for connections, spawning new threads as needed to handle clients */
int g15_clientconnect (lcdlist_t **g15daemon, int listening_socket) {

    int conn_s;
    struct pollfd pfd[1];
    pthread_t client_connection;
    pthread_attr_t attr;
    lcdnode_t *clientnode;

    memset(pfd,0,sizeof(pfd));
    pfd[0].fd = listening_socket;
    pfd[0].events = POLLIN;


    if (poll(pfd,1,500)>0){
        if (!(pfd[0].revents & POLLIN)){
            return 0;
        }

        if ( (conn_s = accept(listening_socket, NULL, NULL) ) < 0 ) {
            if(errno==EWOULDBLOCK || errno==EAGAIN){
            }else{
                daemon_log(LOG_WARNING, "error calling accept()\n");
                return -1;
            }
        }

        clientnode = lcdnode_add(g15daemon);
        clientnode->lcd->connection = conn_s;

        memset(&attr,0,sizeof(pthread_attr_t));
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
        pthread_attr_setstacksize(&attr,256*1024); /* set stack to 768k - dont need 8Mb - this is probably rather excessive also */
        if (pthread_create(&client_connection, &attr, lcd_client_thread, clientnode) != 0) {
            daemon_log(LOG_WARNING,"Couldnt create client thread.");
            if (close(conn_s) < 0 ) {
                daemon_log(LOG_WARNING, "error calling close()\n");
                return -1;
            }

        }
        
        pthread_detach(client_connection);
    }
    return 0;
}















