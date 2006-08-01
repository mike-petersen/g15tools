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

/* quickndirty g15daemon client example. it just connects and sends a prefab image to the server
* and remains connected until the user presses enter.. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "g15daemon_client.h"
#include "g15logo.h"
#include <errno.h>
#include <poll.h>

#include <libg15.h>


int main(int argc, char *argv[])
{
    int g15screen_fd, retval;
    char lcdbuffer[6880];
    unsigned int keystate;
    char msgbuf[256];
    
    if((g15screen_fd = new_g15_screen(G15_PIXELBUF))<0){
        printf("Sorry, cant connect to the G15daemon\n");
        return 5;
    }else
        printf("Connected to g15daemon.  sending image\n");

        if(argc<2)
            retval = g15_send(g15screen_fd,(char*)logo_data,6880);
        else {
            memset(lcdbuffer,0,6880);
            memset(lcdbuffer,1,6880/2);
            retval = g15_send(g15screen_fd,(char*)lcdbuffer,6880);
        }

        printf("checking key status - press G1 to exit\n",retval);
        
        while(1){
            keystate = 0;
            memset(msgbuf,0,256);

            if(send(g15screen_fd, "k", 1, MSG_OOB)<1) /* request key status */
                printf("Error in send\n");    
            retval = recv(g15screen_fd, &keystate , sizeof(keystate),0);
            if(keystate)
                printf("keystate = %i\n",keystate);

            if(keystate & 1) //G1 key.  See libg15.h for details on key values.
                break;

            memset(msgbuf,0,5);
            /* G2,G3 & G4 change LCD backlight */
            if(keystate & 2){
                msgbuf[0]=G15_BRIGHTNESS_DARK|G15DAEMON_BACKLIGHT;
                send(g15screen_fd,msgbuf,1,MSG_OOB);
            }
            if(keystate & 4){
                msgbuf[0]=G15_BRIGHTNESS_MEDIUM|G15DAEMON_BACKLIGHT;
                send(g15screen_fd,msgbuf,1,MSG_OOB);
            }
            if(keystate & 8){
                msgbuf[0]=G15_BRIGHTNESS_BRIGHT|G15DAEMON_BACKLIGHT;
                send(g15screen_fd,msgbuf,1,MSG_OOB);            
            }

            msgbuf[0]='v'; /* are we viewable? */
            send(g15screen_fd,msgbuf,1,MSG_OOB);            
            recv(g15screen_fd,msgbuf,1,0);
            if(msgbuf[0])
              printf("Hey, we are in the foreground, Doc\n");
            else
              printf("What dastardly wabbit put me in the background?\n");
            
            if(msgbuf[0]){ /* we've been backgrounded! */
                sleep(2); /* remain in the background for a bit */
                msgbuf[0]='p'; /* switch priorities */
                send(g15screen_fd,msgbuf,1,MSG_OOB);            
                sleep(2);
                send(g15screen_fd,msgbuf,1,MSG_OOB);            

            }
                           
            usleep(5000);
        }
        g15_close_screen(g15screen_fd);
        return 0;
}
