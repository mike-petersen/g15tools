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


#include "lcd.h"
#include "keys.h"

using namespace std;

static string fifo_filename;
static string uinput_device_filename;

static unsigned char lcd_buffer[0x03e0];
static int lcd_buffer_changed=0;
static pthread_mutex_t mutex;

static bool do_key_handling=true;
static bool do_lcd_handling=true;

void *lcd_worker_thread_func(void *arg)
{
   lcdProcessingWorkflow(lcd_buffer, &mutex, &lcd_buffer_changed, fifo_filename);
   return 0;
}

void usb_worker(usb_dev_handle *devh)
{
   
   if (do_key_handling)
      initKeyHandling(uinput_device_filename);

   
   while (1)
   {
      if (do_key_handling)
      {
         unsigned char buffer[9];
   
         int ret = usb_bulk_read(devh, 0x81, (char*)buffer, 9, 100);
         if (ret == 9)
         {
            processKeyEvent(buffer);
         }
      }
      else
      {
         usleep(100 * 1000); // sleep 100ms to avoid busy looping
      }
      
      if (do_lcd_handling)
      {      
         pthread_mutex_lock(&mutex);
            if (lcd_buffer_changed)
            {
               lcd_buffer[0] = 3;
               int ret = usb_interrupt_write(devh, 2, (char *)lcd_buffer, 0x03e0, 1000);
               if (ret != 0x03e0)
               {
                  cout << "Error writing to the lcd, return value is " << ret << endl;
                  cout << "The returnvalue should have been 0x03e0 however" << endl;
               }
               lcd_buffer_changed = 0;
            }
         pthread_mutex_unlock(&mutex);
      }
      
         
   }
   
}

void work(usb_dev_handle *devh)
{
   pthread_mutex_init(&mutex,0);
  
   if (fifo_filename != "" && do_lcd_handling)
   {
      pthread_attr_t attr;
      pthread_attr_init(&attr);
      pthread_attr_setdetachstate(&attr,0);
      
      pthread_t lcd_thread;
      
      pthread_create(&lcd_thread, &attr, &lcd_worker_thread_func, 0);
   }
   
   usb_worker(devh);
  
}
void printUsage()
{
   cout << "Call me like this: g15lcd path_to_fifo " << endl;
   cout << "Parameters understood:" << endl;
   cout << "\t\t -h ... guess what" << endl;
   cout << "\t\t --help .. another wild guess please" << endl;
   cout << "\t\t -d /dev/.... path to the uinput device" << endl;
   cout << "\t\t --nokeys ... disable key handling" << endl;
   cout << "\t\t --nolcd ... disable lcd handling" << endl;  
}

int main(int argc, char *argv[])
{
   fifo_filename = "";
   uinput_device_filename = "/dev/input/uinput";
   
   int i;
   
   for (i=1;i<argc;++i)
   {
      string arg(argv[i]);
      if (arg == "-h" || arg == "--help")
      {
         printUsage();
         return 0;
      }
      else if (arg == "-d" || arg == "--dev")
      {
         if (i+1 < argc)
         {
            uinput_device_filename = argv[i+1];
            ++i;
         }
         else
         {
            cout << "Dont try to fool me, " << arg << " as the last parameter does not make much sense" << endl;
            return 0;
         }
      }
      else if (arg == "--nokeys")
      {
         do_key_handling=false;
      }
      else if (arg == "--nolcd")
      {
         do_lcd_handling=false;
      }
      else
      {
         fifo_filename = argv[i];
      }
   }
   if (fifo_filename == "")
   {
      cout << "You didnt specify a fifo filename, I'm this disabling writing text to the lcd" << endl;
      cout << "This is a change in behaviour from previous version, I will _NOT_ read data from stdin anymore" << endl;
      do_lcd_handling = false;
   }
   if (fifo_filename == "-")
   {
      cout << "Dont try to trick me, I wont read input from stdin anymore" << endl;
      do_lcd_handling = false;
   }
   
   if (!do_lcd_handling && !do_key_handling)
   {
      cout << "Make up your mind, I should at least have to do something dont you think?" << endl;
      return 0;
   }
   
   
   cout << "Starting g15lcd with the following settings:" << endl;
   cout << "\t fifo:          \"" << fifo_filename << "\"" << endl;
   cout << "\t uinput device: \"" << uinput_device_filename << "\"" << endl;
      
   struct usb_bus *bus;
   
   usb_init();
   
   usb_find_busses();
   usb_find_devices();

   usb_dev_handle *devh = 0;
   
   for (bus = usb_busses; bus; bus = bus->next) 
   {
      
      struct usb_device *dev;
      
      for (dev = bus->devices; dev; dev = dev->next)
      {
         // the other one is 49697 .... (and the other one is 49698) 
         // 8 is the only one with endpoint 2 so lets try this one
         // btw, i REALLY have to disable this fucking g keys
         if (dev->descriptor.idVendor == 1133 && dev->descriptor.idProduct == 49698)
         {
            int ret;
            devh = usb_open(dev);
            cout << "Found keyboard, trying to open it ..." << endl;
            
            if (!devh)
            {
               cout << "Error, could not open the keyboard" << endl;
               cout << "Perhaps you dont have enough permissions to access it" << endl;
               abort();
            }

            
            usleep(25*1000);

            char name_buffer[65535];
            ret = usb_get_driver_np(devh, 0, name_buffer, 65535);
            
            if (!ret)
            {
               cout << "There already seems to be a driver attached (\"" << name_buffer << "\"), trying to detach it" << endl;
               
               ret = usb_detach_kernel_driver_np(devh, 0);
               if (!ret)
               {
                  cout << "Success, detached the driver" << endl;
               }
               else
               {
                  cout << "Sorry, I could not detached the driver, giving up" << endl;
                  abort();
               }
            }
            
            
            ret = usb_set_configuration(devh, 1);
            if (ret)
            {
               cout << "Error setting the configuration, this is fatal" << endl;
               abort();
            }
            
            usleep(25*1000);
            
            ret = usb_claim_interface(devh,0);
            
            if (ret)
            {
               cout << "Error claiming interface, good day cruel world" << endl;
               abort();
            }
            usleep(25*1000);
            cout << "Done opening the keyboard" << endl;
         }
      }  
   }
   
   if (!devh)
   {
      cout << "How about plugging your keyboard in?" << endl;
      cout << "At least I couldnt find the keyboard :(" << endl;
      cout << "Sorry, I'm really sorry" << endl;
      abort();
   }
   
   work(devh);
   
   return 0;
}
