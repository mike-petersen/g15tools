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

#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <poll.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <errno.h>

#include "g15daemon.h"
#include <libdaemon/daemon.h>
extern int leaving;
extern unsigned int current_key_state;

/* create and open a socket for listening */
int init_sockserver(){
    int listening_socket;
    int yes=1;

    struct    sockaddr_in servaddr; 

    if ((listening_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
        daemon_log(LOG_WARNING, "couldnt create socket.\n");
        return -1;
    }

    setsockopt(listening_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    inet_aton (LISTEN_ADDR, &servaddr.sin_addr);
    servaddr.sin_port        = htons(LISTEN_PORT);

    if (bind(listening_socket, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0 ) {
        daemon_log(LOG_WARNING, "error calling bind()\n");
        return -1;
    }

    if (listen(listening_socket, MAX_CLIENTS) < 0 ) {
        daemon_log(LOG_WARNING, "error calling listen()\n");
        return -1;
    }

    return listening_socket;
}


int g15_send(int sock, char *buf, unsigned int len)
{
    int total = 0;
    int retval = 0;
    int bytesleft = len;
    struct pollfd pfd[1];
    
    while(total < len && !leaving) {
        memset(pfd,0,sizeof(pfd));
        pfd[0].fd = sock;
        pfd[0].events = POLLOUT;
        if(poll(pfd,1,500)>0) {
            if(pfd[0].revents & POLLOUT) {
                retval = send(sock, buf+total, bytesleft, 0);
                if (retval == -1) { 
                    break; 
                }
                bytesleft -= retval;
                total += retval;
            }
        }
    }
    return retval==-1?-1:0;
} 

int g15_recv(lcdnode_t *lcdnode, int sock, char *buf, unsigned int len)
{
    int total = 0;
    int retval = 0;
    int msgret = 0;
    int bytesleft = len; 
    struct pollfd pfd[1];
    unsigned int msgbuf[20];

    while(total < len  && !leaving) {
        memset(pfd,0,sizeof(pfd));
        pfd[0].fd = sock;
        pfd[0].events = POLLIN | POLLPRI;
        if(poll(pfd,1,500)>0){
            if(pfd[0].revents & POLLPRI) { /* receive out-of-band request from client and deal with it */
                memset(msgbuf,0,sizeof(msgbuf));
                msgret = recv(sock, msgbuf, sizeof(msgbuf) , MSG_OOB);
                if (msgret < 1) {
                    break;
                }

                if(msgbuf[0] == 'k') { /* client wants keypresses */
                    if(lcdnode->list->current == lcdnode){
                        if((msgret=send(sock,(void *)&current_key_state,sizeof(current_key_state),0))<0) /* send the keystate inband back to the client */
                           daemon_log(LOG_WARNING,"Error in send: %s\n",strerror(errno));
                    }
                    else{
                        memset(msgbuf,0,4); /* client isn't currently being displayed.. tell them nothing */
                        send(sock,(void *)msgbuf,sizeof(current_key_state),0);
                    }
                }
            }
            else if(pfd[0].revents & POLLIN) {
                retval = recv(sock, buf+total, bytesleft, 0);
                if (retval < 1) { 
                    break; 
                }
                total += retval;
                bytesleft -= retval;
            }
        }
    }
    return total;
} 