/*
    This file is part of g15lcd.

    g15lcd is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    g15lcd is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with g15lcd; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <iostream>
#include <cstdlib>
#include <usb.h>
#include <string>
#include <fstream>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>
#include <linux/uinput.h>

#include "keys.h"



/**** SCANCODES SENT BY ME
   G1 - G18 = 167  ....  184
   M1 - M4 (MR is M4) = 185 ... 188
   L1 - L5 (L1 is the round one, L2 - L4 are the flat ones) = 189 ... 203   
   */// 
   
unsigned char g_key_states[18];
unsigned char m_key_states[4];
unsigned char l_key_states[5];


using namespace std;

static int uinput_fd;

// FIXXME, we have to close the device even on ctrl c and alike
void closeDevice()
{
   cout << "closing device" << endl;
   ioctl(uinput_fd, UI_DEV_DESTROY);
   close(uinput_fd);
}

void initKeyHandling(string const &device_filename)
{
   cout << "Key Handling init" << endl;
   struct uinput_user_dev device;
   uinput_fd = open(device_filename.c_str(), O_RDWR);
   
   if (uinput_fd < 0)
   {
      cout << "Error, could not open the uinput device" << endl;
      cout << "Compile your kernel for uinput, calling it a day now" << endl;
      cout << "mknod uinput c 10 223" << endl;
      abort();
   }
   memset(&device,0,sizeof(device));
   strncpy(device.name, "G15 Keys", UINPUT_MAX_NAME_SIZE);
   device.id.bustype = BUS_USB;
   device.id.version = 4;
   
   ioctl(uinput_fd, UI_SET_EVBIT, EV_KEY);
   
   int i;
   for (i=0;i<256;++i)
      ioctl(uinput_fd, UI_SET_KEYBIT, i);
   
   write(uinput_fd, &device, sizeof(device));
   
   if (ioctl(uinput_fd, UI_DEV_CREATE)) 
   {
      cout <<"Failed to create input device" << endl;
      abort();
   }
   atexit(&closeDevice);
   
   memset(g_key_states, 0, sizeof(g_key_states));
   memset(m_key_states, 0, sizeof(m_key_states));
   memset(l_key_states, 0, sizeof(l_key_states));
}

void keyDown(unsigned char scancode)
{
   struct input_event event;
	memset(&event, 0, sizeof(event));
	
	event.type = EV_KEY;
	event.code = scancode;
	event.value = 1;
	write (uinput_fd, &event, sizeof(event));   
}
void keyUp(unsigned char scancode)
{
   struct input_event event;
	memset(&event, 0, sizeof(event));
	
	event.type = EV_KEY;
	event.code = scancode;
	event.value = 0;
	write (uinput_fd, &event, sizeof(event));   
}
void keyDownUp(unsigned char scancode)
{
   keyDown(scancode);
   keyUp(scancode);
   
}
unsigned char g15KeyToLogitechKeyCode(int key)
{
   // first 12 G keys produce F1 - F12, thats 0x3a + key
   if (key < 12)
   {
      return 0x3a + key;
   }
   // the other keys produce Key '1' (above letters) + key, thats 0x1e + key
   else
   {
      return 0x1e + key - 12; // sigh, half an hour to find  -12 ....
   }
}

void processKeyEvent(unsigned char *buffer)
{
 //  printf("\n");
 //  printf("\n");
   
   const int g_scancode_offset = 167;
   const int m_scancode_offset = 185;
   const int l_scancode_offset = 189;
   int i;
   
   if (buffer[0] == 0x01)
   {
//      cout << "Checking keys: " << endl;

      for (i=0;i<18;++i)
      {
         unsigned char orig_scancode = g15KeyToLogitechKeyCode(i);
         bool is_set = false;
         
         if (buffer[1] == orig_scancode || buffer[2] == orig_scancode || buffer[3] == orig_scancode ||
               buffer[4] == orig_scancode || buffer[5] == orig_scancode)
            is_set = true;
         
         if (!is_set && g_key_states[i] != 0)
         {
            // key was pressed but is no more
            keyUp( g_scancode_offset + i);
            g_key_states[i] = 0;
//            cout << "G" << (i+1) << " going up" << endl;
         }
         else if (is_set && g_key_states[i] == 0)
         {
            keyDown ( g_scancode_offset + i);
            g_key_states[i] = 1;
//            cout << "G" << (i+1) << " going down" << endl;
         }
      }
      
      
   }
   else
   {
      if (buffer[0] == 0x02)
      {
         unsigned char m_key_new_states[4];
         memset(m_key_new_states,0,sizeof(m_key_new_states));
         
         if (buffer[6]&0x01)
            m_key_new_states[0] = 1;
         if (buffer[7]&0x02)
            m_key_new_states[1] = 1;
         if (buffer[8]&0x04)
            m_key_new_states[2] = 1;
         if (buffer[7]&0x40)
            m_key_new_states[3] = 1;
         
         for (i=0;i<4;++i)
         {
            if (!m_key_new_states[i] && m_key_states[i] != 0)
            {
               // key was pressed but is no more
               keyUp( m_scancode_offset + i);
               m_key_states[i] = 0;
//               cout << "M" << (i+1) << " going up" << endl;
            }
            else if (m_key_new_states[i] && m_key_states[i] == 0)
            {
               keyDown ( m_scancode_offset + i);
               m_key_states[i] = 1;
//               cout << "M" << (i+1) << " going down" << endl;
            }
         }
         
         unsigned char l_key_new_states[5];
         memset(l_key_new_states,0,sizeof(l_key_new_states));
         if (buffer[8]&0x80)
            l_key_new_states[0] = 1;
         if (buffer[2]&0x80)
            l_key_new_states[1] = 1;
         if (buffer[3]&0x80)
            l_key_new_states[2] = 1;
         if (buffer[4]&0x80)
            l_key_new_states[3] = 1;
         if (buffer[5]&0x80)
            l_key_new_states[4] = 1;
         
         for (i=0;i<5;++i)
         {
            if (!l_key_new_states[i] && l_key_states[i] != 0)
            {
               // key was pressed but is no more
               keyUp( l_scancode_offset + i);
               l_key_states[i] = 0;
//               cout << "L" << (i+1) << " going up" << endl;
            }
            else if (l_key_new_states[i] && l_key_states[i] == 0)
            {
               keyDown ( l_scancode_offset + i);
               l_key_states[i] = 1;
//               cout << "L" << (i+1) << " going down" << endl;
            }
         }
                  
         
         
      }
//      printf("%hhx %hhx %hhx %hhx %hhx %hhx %hhx %hhx %hhx \n",buffer[0],buffer[1],buffer[2],buffer[3],buffer[4],buffer[5],buffer[6],buffer[7],buffer[8]);   
   }


}
