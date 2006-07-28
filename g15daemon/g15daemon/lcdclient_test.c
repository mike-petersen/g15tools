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

#include "g15daemon_client.h"
#include "g15logo.h"

int main(int argc, char *argv[])
{
    int g15screen_fd, retval;
    char lcdbuffer[6880];

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
    printf("Sleeping for 10seconds then exiting\n",retval);
    
    sleep(10);    
    g15_close_screen(g15screen_fd);
    return 0;
}
