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
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <libdaemon/daemon.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <fcntl.h>
#include <unistd.h>

#include "libg15.h"
#include "g15daemon.h"

static int uinp_fd = -1;

extern int leaving;
extern unsigned int connected_clients;

int g15_init_uinput() {
    
    int i=0;
    struct uinput_user_dev uinp;
    static const char *uinput_device_fn[] = { "/dev/uinput", "/dev/input/uinput",0};
    
    while (uinput_device_fn[i] && (uinp_fd = open(uinput_device_fn[i],O_RDWR))<0){
        ++i;
    }
    if(uinp_fd<0){
        daemon_log(LOG_ERR,"Couldnt open uinput device.  Please ensure the uinput driver is loaded into the kernel and that you have permission to open the device.");
        return -1;
    }

    memset(&uinp,0,sizeof(uinp));
    strncpy(uinp.name, "G15 Extra Keys", UINPUT_MAX_NAME_SIZE);
    uinp.id.version = 4;
    uinp.id.bustype = BUS_USB;

    ioctl(uinp_fd, UI_SET_EVBIT, EV_KEY);

    for (i=0; i<256; ++i)
        ioctl(uinp_fd, UI_SET_KEYBIT, i);

    write(uinp_fd, &uinp, sizeof(uinp));
    
    if (ioctl(uinp_fd, UI_DEV_CREATE))
    {
        daemon_log(LOG_ERR,"Couldnt create UINPUT device.");
        return -1;
    }
    return 0;
}

void g15_exit_uinput(){
    ioctl(uinp_fd, UI_DEV_DESTROY);
    close(uinp_fd);
}


static void g15_uinput_keydown(unsigned char code)
{
    struct input_event event;
    memset(&event, 0, sizeof(event));

    event.type = EV_KEY;
    event.code = code;
    event.value = G15KEY_DOWN;
    
    write (uinp_fd, &event, sizeof(event));
}

static void g15_uinput_keyup(unsigned char code)
{
    struct input_event event;
    memset(&event, 0, sizeof(event));

    event.type = EV_KEY;
    event.code = code;
    event.value = G15KEY_UP;
    
    write (uinp_fd, &event, sizeof(event));
}

void g15_uinput_process_keys(lcdlist_t *displaylist, unsigned int currentkeys, unsigned int lastkeys){
    
    /* 'G' keys */
    if((currentkeys & G15_KEY_G1) && !(lastkeys & G15_KEY_G1))
        g15_uinput_keydown(GKEY_OFFSET);
    else if(!(currentkeys & G15_KEY_G1) && (lastkeys & G15_KEY_G1))
        g15_uinput_keyup(GKEY_OFFSET);

    if((currentkeys & G15_KEY_G2) && !(lastkeys & G15_KEY_G2))
        g15_uinput_keydown(GKEY_OFFSET+1);
    else if(!(currentkeys & G15_KEY_G2) && (lastkeys & G15_KEY_G2))
        g15_uinput_keyup(GKEY_OFFSET+1);

    if((currentkeys & G15_KEY_G3) && !(lastkeys & G15_KEY_G3))
        g15_uinput_keydown(GKEY_OFFSET+2);
    else if(!(currentkeys & G15_KEY_G3) && (lastkeys & G15_KEY_G3))
        g15_uinput_keyup(GKEY_OFFSET+2);

    if((currentkeys & G15_KEY_G4) && !(lastkeys & G15_KEY_G4))
        g15_uinput_keydown(GKEY_OFFSET+3);
    else if(!(currentkeys & G15_KEY_G4) && (lastkeys & G15_KEY_G4))
        g15_uinput_keyup(GKEY_OFFSET+3);

    if((currentkeys & G15_KEY_G5) && !(lastkeys & G15_KEY_G5))
        g15_uinput_keydown(GKEY_OFFSET+4);
    else if(!(currentkeys & G15_KEY_G5) && (lastkeys & G15_KEY_G5))
        g15_uinput_keyup(GKEY_OFFSET+4);

    if((currentkeys & G15_KEY_G6) && !(lastkeys & G15_KEY_G6))
        g15_uinput_keydown(GKEY_OFFSET+5);
    else if(!(currentkeys & G15_KEY_G6) && (lastkeys & G15_KEY_G6))
        g15_uinput_keyup(GKEY_OFFSET+5);

    if((currentkeys & G15_KEY_G7) && !(lastkeys & G15_KEY_G7))
        g15_uinput_keydown(GKEY_OFFSET+6);
    else if(!(currentkeys & G15_KEY_G7) && (lastkeys & G15_KEY_G7))
        g15_uinput_keyup(GKEY_OFFSET+6);

    if((currentkeys & G15_KEY_G8) && !(lastkeys & G15_KEY_G8))
        g15_uinput_keydown(GKEY_OFFSET+7);
    else if(!(currentkeys & G15_KEY_G8) && (lastkeys & G15_KEY_G8))
        g15_uinput_keyup(GKEY_OFFSET+7);

    if((currentkeys & G15_KEY_G9) && !(lastkeys & G15_KEY_G9))
        g15_uinput_keydown(GKEY_OFFSET+8);
    else if(!(currentkeys & G15_KEY_G9) && (lastkeys & G15_KEY_G9))
        g15_uinput_keyup(GKEY_OFFSET+8);

    if((currentkeys & G15_KEY_G10) && !(lastkeys & G15_KEY_G10))
        g15_uinput_keydown(GKEY_OFFSET+9);
    else if(!(currentkeys & G15_KEY_G10) && (lastkeys & G15_KEY_G10))
        g15_uinput_keyup(GKEY_OFFSET+9);

    if((currentkeys & G15_KEY_G11) && !(lastkeys & G15_KEY_G11))
        g15_uinput_keydown(GKEY_OFFSET+10);
    else if(!(currentkeys & G15_KEY_G11) && (lastkeys & G15_KEY_G11))
        g15_uinput_keyup(GKEY_OFFSET+10);

    if((currentkeys & G15_KEY_G12) && !(lastkeys & G15_KEY_G12))
        g15_uinput_keydown(GKEY_OFFSET+11);
    else if(!(currentkeys & G15_KEY_G12) && (lastkeys & G15_KEY_G12))
        g15_uinput_keyup(GKEY_OFFSET+11);

    if((currentkeys & G15_KEY_G13) && !(lastkeys & G15_KEY_G13))
        g15_uinput_keydown(GKEY_OFFSET+12);
    else if(!(currentkeys & G15_KEY_G13) && (lastkeys & G15_KEY_G13))
        g15_uinput_keyup(GKEY_OFFSET+12);

    if((currentkeys & G15_KEY_G14) && !(lastkeys & G15_KEY_G14))
        g15_uinput_keydown(GKEY_OFFSET+13);
    else if(!(currentkeys & G15_KEY_G14) && (lastkeys & G15_KEY_G14))
        g15_uinput_keyup(GKEY_OFFSET+13);

    if((currentkeys & G15_KEY_G15) && !(lastkeys & G15_KEY_G15))
        g15_uinput_keydown(GKEY_OFFSET+14);
    else if(!(currentkeys & G15_KEY_G15) && (lastkeys & G15_KEY_G15))
        g15_uinput_keyup(GKEY_OFFSET+14);

    if((currentkeys & G15_KEY_G16) && !(lastkeys & G15_KEY_G16))
        g15_uinput_keydown(GKEY_OFFSET+15);
    else if(!(currentkeys & G15_KEY_G16) && (lastkeys & G15_KEY_G16))
        g15_uinput_keyup(GKEY_OFFSET+15);

    if((currentkeys & G15_KEY_G17) && !(lastkeys & G15_KEY_G17))
        g15_uinput_keydown(GKEY_OFFSET+16);
    else if(!(currentkeys & G15_KEY_G17) && (lastkeys & G15_KEY_G17))
        g15_uinput_keyup(GKEY_OFFSET+16);

    if((currentkeys & G15_KEY_G18) && !(lastkeys & G15_KEY_G18))
        g15_uinput_keydown(GKEY_OFFSET+17);
    else if(!(currentkeys & G15_KEY_G18) && (lastkeys & G15_KEY_G18))
        g15_uinput_keyup(GKEY_OFFSET+17);

    /* 'M' keys */

    if((currentkeys & G15_KEY_M1) && !(lastkeys & G15_KEY_M1))
        g15_uinput_keydown(MKEY_OFFSET);
    else if(!(currentkeys & G15_KEY_M1) && (lastkeys & G15_KEY_M1))
        g15_uinput_keyup(MKEY_OFFSET);

    if((currentkeys & G15_KEY_M2) && !(lastkeys & G15_KEY_M2))
        g15_uinput_keydown(MKEY_OFFSET+1);
    else if(!(currentkeys & G15_KEY_M2) && (lastkeys & G15_KEY_M2))
        g15_uinput_keyup(MKEY_OFFSET+1);

    if((currentkeys & G15_KEY_M3) && !(lastkeys & G15_KEY_M3))
        g15_uinput_keydown(MKEY_OFFSET+2);
    else if(!(currentkeys & G15_KEY_M3) && (lastkeys & G15_KEY_M3))
        g15_uinput_keyup(MKEY_OFFSET+2);
    
    if(!connected_clients) {
        if((currentkeys & G15_KEY_MR) && !(lastkeys & G15_KEY_MR))
            g15_uinput_keydown(MKEY_OFFSET+3);
        else if(!(currentkeys & G15_KEY_MR) && (lastkeys & G15_KEY_MR))
            g15_uinput_keyup(MKEY_OFFSET+3);
    }else{
        /* cycle through connected client displays if L1 is pressed */
        if((currentkeys & G15_KEY_MR) && !(lastkeys & G15_KEY_MR))
        {
            pthread_mutex_lock(&lcdlist_mutex);
            if(displaylist->tail == displaylist->current) {
                displaylist->current = displaylist->head;
            } else {
                displaylist->current = displaylist->current->prev;
            }
            displaylist->current->lcd->state_changed = 1;
            pthread_mutex_unlock(&lcdlist_mutex);
        }
    }
    
    /* 'L' keys...  */
    
    if((currentkeys & G15_KEY_L1) && !(lastkeys & G15_KEY_L1))
    g15_uinput_keydown(LKEY_OFFSET);
    else if(!(currentkeys & G15_KEY_L1) && (lastkeys & G15_KEY_L1))
    g15_uinput_keyup(LKEY_OFFSET);
    
    if((currentkeys & G15_KEY_L2) && !(lastkeys & G15_KEY_L2))
        g15_uinput_keydown(LKEY_OFFSET+1);
    else if(!(currentkeys & G15_KEY_L2) && (lastkeys & G15_KEY_L2))
        g15_uinput_keyup(LKEY_OFFSET+1);

    if((currentkeys & G15_KEY_L3) && !(lastkeys & G15_KEY_L3))
        g15_uinput_keydown(LKEY_OFFSET+2);
    else if(!(currentkeys & G15_KEY_L3) && (lastkeys & G15_KEY_L3))
        g15_uinput_keyup(LKEY_OFFSET+2);

    if((currentkeys & G15_KEY_L4) && !(lastkeys & G15_KEY_L4))
        g15_uinput_keydown(LKEY_OFFSET+3);
    else if(!(currentkeys & G15_KEY_L4) && (lastkeys & G15_KEY_L4))
        g15_uinput_keyup(LKEY_OFFSET+3);

    if((currentkeys & G15_KEY_L5) && !(lastkeys & G15_KEY_L5))
        g15_uinput_keydown(LKEY_OFFSET+4);
    else if(!(currentkeys & G15_KEY_L5) && (lastkeys & G15_KEY_L5))
        g15_uinput_keyup(LKEY_OFFSET+4);

}
