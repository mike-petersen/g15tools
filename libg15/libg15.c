/*
    This file is part of g15tools.

    g15tools is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    g15tools is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with g15lcd; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <pthread.h>
#include "libg15.h"
#include <stdio.h>
#include <stdarg.h>
#include <usb.h>
#include <string.h>
#include <errno.h>

static usb_dev_handle *keyboard_device = 0;
static int libg15_debugging_enabled = 0;
static int enospc_slowdown = 0;

static int found_devicetype = -1;

static pthread_mutex_t libusb_mutex;

/* to add a new device, simply create a new DEVICE() in this list */
/* Fields are: "Name",VendorID,ProductID,Capabilities */
const libg15_devices_t g15_devices[] = {
  DEVICE("Logitech G15",0x46d,0xc222,G15_LCD|G15_KEYS),
  DEVICE("Logitech G11",0x46d,0xc225,G15_KEYS),
  DEVICE(NULL,0,0,0)
};

int g15DeviceCapabilities() {
    if(found_devicetype>-1)
      return g15_devices[found_devicetype].caps;
    else
      return -1;
}

/* enable or disable debugging */
void libg15Debug(int option) {

  libg15_debugging_enabled = option;
  usb_set_debug(option);
}

/* debugging wrapper */
static int g15_log (FILE *fd, const char *fmt, ...) {

  if (libg15_debugging_enabled){
     fprintf(fd,"libg15: ");
     va_list argp;
     va_start (argp, fmt);
       vfprintf(fd,fmt,argp);
     va_end (argp);
  }
   return 0;
}

static int initLibUsb()
{
  usb_init();

  /**
   *  usb_find_busses and usb_find_devices both report the number of devices
   *  / busses added / removed since the last call. since this is the first
   *  call we have to return values != 0 or else we didnt find anything */
     
  if (!usb_find_busses())
    return G15_ERROR_OPENING_USB_DEVICE;

  if (!usb_find_devices())
    return G15_ERROR_OPENING_USB_DEVICE;

  return G15_NO_ERROR;
}

static usb_dev_handle * findAndOpenDevice(libg15_devices_t handled_device)
{
  struct usb_bus *bus = 0;
  struct usb_device *dev = 0;
  int retries=0;
  for (bus = usb_busses; bus; bus = bus->next) 
  {
    for (dev = bus->devices; dev; dev = dev->next)
    {
      if ((dev->descriptor.idVendor == handled_device.vendorid && dev->descriptor.idProduct == handled_device.productid)) 
      {
        int ret=0;
        char name_buffer[65535];
        name_buffer[0] = 0;
        usb_dev_handle *devh = 0;
        g15_log(stderr,"Found %s, trying to open it\n",handled_device.name);

        devh = usb_open(dev);
        if (!devh)
        {
          g15_log(stderr, "Error, could not open the keyboard\n");
          g15_log(stderr, "Perhaps you dont have enough permissions to access it\n");
          return 0;
        }

        usleep(50*1000);

        /* libusb functions ending in _np are not portable between OS's 
         * Non-linux users will need some way to detach the HID driver from
         * the G15 until we work out how to do this for other OS's automatically. 
         * For the moment, we just skip this code..
         */
#ifdef LIBUSB_HAS_GET_DRIVER_NP
        ret = usb_get_driver_np(devh, 0, name_buffer, 65535);
        /* some kernel versions say that a driver is attached even though there is none
            in this case the name buffer has not been changed
            thanks to RobEngle for pointing this out */
        if (!ret && name_buffer[0])
        {
          g15_log(stderr,"Trying to detach driver currently attached: \"%s\"\n",name_buffer);

          ret = usb_detach_kernel_driver_np(devh, 0);
          if (!ret)
          {
            g15_log(stderr,"Success, detached the driver\n");
          }
          else
          {
            g15_log(stderr,"Sorry, I could not detached the driver, giving up\n");
            return 0;
          }

        }
#endif  
        usleep(50*1000);

        ret = usb_set_configuration(devh, 1);
        if (ret)
        {
          g15_log(stderr,"Error setting the configuration, this is fatal\n");
          return 0;
        }
  
        usleep(50*1000);
  
        while((ret = usb_claim_interface(devh,0)) && retries <10) {
          usleep(50*1000);
          retries++;
          g15_log(stderr,"Trying to claim interface\n");
        }
  
        if (ret)
        {
          g15_log(stderr,"Error claiming interface, good day cruel world\n");
          return 0;
        }
        usleep(1000*1000); // FIXME.  I should find a way of polling the status to ensure the endpoint has woken up, rather than just waiting for a second
        g15_log(stderr,"Done opening the keyboard\n");

        return devh;
      }
    }  
  }
  return 0;
}


static usb_dev_handle * findAndOpenG15() {
  int i;
  for (i=0; g15_devices[i].name !=NULL  ;i++){
    g15_log(stderr,"Trying to find %s\n",g15_devices[i].name);
    if(keyboard_device = findAndOpenDevice(g15_devices[i])){
        found_devicetype = i;
        break;
    }
    else
      g15_log(stderr,"%s not found\n",g15_devices[i].name);
  }
  return keyboard_device;
}


int re_initLibG15()
{

  usb_init();

  /**
   *  usb_find_busses and usb_find_devices both report the number of devices
   *  / busses added / removed since the last call. since this is the first
   *  call we have to return values != 0 or else we didnt find anything */
     
  if (!usb_find_devices())
    return G15_ERROR_OPENING_USB_DEVICE;
  
  keyboard_device = findAndOpenG15();
  if (!keyboard_device)
    return G15_ERROR_OPENING_USB_DEVICE;
 
  return G15_NO_ERROR;
}

int initLibG15()
{
  int retval = G15_NO_ERROR;
  retval = initLibUsb();
  if (retval)
    return retval;
  
  keyboard_device = findAndOpenG15();
  if (!keyboard_device)
    return G15_ERROR_OPENING_USB_DEVICE;

  pthread_mutex_init(&libusb_mutex, NULL); 
  
  return retval;
}

/* reset the keyboard, returning it to a known state */
int exitLibG15()
{
  int retval = G15_NO_ERROR;
  if (keyboard_device){
    retval = usb_release_interface (keyboard_device, 0);
    usleep(50*1000);
    retval = usb_reset(keyboard_device);
    usleep(50*1000);
    usb_close(keyboard_device);
    keyboard_device=0;
    pthread_mutex_destroy(&libusb_mutex);
    return retval;
  }
  return -1;
}


static void dumpPixmapIntoLCDFormat(unsigned char *lcd_buffer, unsigned char const *data)
{
  unsigned int offset_from_start = G15_LCD_OFFSET;
  unsigned int curr_row = 0;
  unsigned int curr_col = 0;
  
  for (curr_row=0;curr_row<G15_LCD_HEIGHT;++curr_row)
  {
    for (curr_col=0;curr_col<G15_LCD_WIDTH;++curr_col)
    {
      unsigned int pixel_offset = curr_row*G15_LCD_WIDTH + curr_col;
      unsigned int byte_offset = pixel_offset / 8;
      unsigned int bit_offset = pixel_offset % 8;
      unsigned int val = data[byte_offset] & 1<<(7-bit_offset);
      
      unsigned int row = curr_row / 8;
      unsigned int offset = G15_LCD_WIDTH*row + curr_col;
      unsigned int bit = curr_row % 8;
    
/*
      if (val)
        printf("Setting pixel at row %d col %d to %d offset %d bit %d\n",curr_row,curr_col, val, offset, bit);
      */
      if (val)
        lcd_buffer[offset_from_start + offset] = lcd_buffer[offset_from_start + offset] | 1 << bit;
      else
        lcd_buffer[offset_from_start + offset] = lcd_buffer[offset_from_start + offset]  &  ~(1 << bit);
    }
  }
}

int handle_usb_errors(const char *prefix, int ret) {
  switch (ret){
    case -ETIMEDOUT:
     return G15_ERROR_READING_USB_DEVICE;  /* backward-compatibility */
     break;
    case -ENOSPC: /* the we dont have enough bandwidth, apparently.. something has to give here.. */
      g15_log(stderr,"usb error: ENOSPC.. reducing speed\n");
      enospc_slowdown = 1;
      break;
    case -ENODEV: /* the device went away - we probably should attempt to reattach */
    case -ENXIO: /* host controller bug */
    case -EINVAL: /* invalid request */
    case -EAGAIN: /* try again */
    case -EFBIG: /* too many frames to handle */
    case -EMSGSIZE: /* msgsize is invalid */
     g15_log(stderr,"usb error: %s (%i)\n",prefix,ret);     
     break;
    case -EPIPE: /* endpoint is stalled */
     g15_log(stderr,"usb error: %s EPIPE! clearing...\n",prefix);     
     pthread_mutex_lock(&libusb_mutex);
     usb_clear_halt(keyboard_device, 0x81);
     pthread_mutex_unlock(&libusb_mutex);
     break;
  default: /* timed out */
     g15_log(stderr,"Unknown usb error: %s !! (err is %i)\n",prefix,ret);     
  }
  return ret;
}

int writePixmapToLCD(unsigned char const *data)
{
  int ret = 0;
  int transfercount=0;
  unsigned char lcd_buffer[G15_BUFFER_LEN];
  memset(lcd_buffer,0,G15_BUFFER_LEN);
  dumpPixmapIntoLCDFormat(lcd_buffer, data);

  if(!(g15_devices[found_devicetype].caps & G15_LCD))
    return 0;
  
  /* the keyboard needs this magic byte */
  lcd_buffer[0] = 0x03;
  /* in an attempt to reduce peak bus utilisation, we break the transfer into 32 byte chunks and sleep a bit in between.
     It shouldnt make much difference, but then again, the g15 shouldnt be flooding the bus enough to cause ENOSPC, yet 
     apparently does on some machines...
     I'm not sure how successful this will be in combatting ENOSPC, but we'll give it try in the real-world. */

  if(enospc_slowdown != 0){
    pthread_mutex_lock(&libusb_mutex);  
    for(transfercount = 0;transfercount<=31;transfercount++){
      ret = usb_interrupt_write(keyboard_device, 2, (char*)lcd_buffer+(32*transfercount), 32, 1000);
      if (ret != 32)
      {
        handle_usb_errors ("LCDPixmap Slow Write",ret);
        return G15_ERROR_WRITING_PIXMAP;
      }
      usleep(100);
    }
    pthread_mutex_unlock(&libusb_mutex);
  }else{
      /* transfer entire buffer in one hit */
      pthread_mutex_lock(&libusb_mutex);
      ret = usb_interrupt_write(keyboard_device, 2, (char*)lcd_buffer, G15_BUFFER_LEN, 1000);
      pthread_mutex_unlock(&libusb_mutex);
      if (ret != G15_BUFFER_LEN)
      {
        handle_usb_errors ("LCDPixmap Write",ret);
        return G15_ERROR_WRITING_PIXMAP;
      }
      usleep(100);
  }

  return 0;
}

int setLCDContrast(unsigned int level)
{
  int retval = 0;
  unsigned char usb_data[] = { 2, 32, 129, 0 };

  switch(level) 
  {
    case 1: 
      usb_data[3] = 22; 
      break;
    case 2: 
      usb_data[3] = 26;
      break;
    default:
      usb_data[3] = 18;
  }  
  pthread_mutex_lock(&libusb_mutex);
  retval = usb_control_msg(keyboard_device, USB_TYPE_CLASS + USB_RECIP_INTERFACE, 9, 0x302, 0, (char*)usb_data, 4, 10000);
  pthread_mutex_unlock(&libusb_mutex);
  return retval;
}

int setLEDs(unsigned int leds)
{
  int retval = 0;
  unsigned char m_led_buf[4] = { 2, 4, 0, 0 };
  m_led_buf[2] = ~(unsigned char)leds;
  pthread_mutex_lock(&libusb_mutex);
  retval = usb_control_msg(keyboard_device, USB_TYPE_CLASS + USB_RECIP_INTERFACE, 9, 0x302, 0, (char*)m_led_buf, 4, 10000); 
  pthread_mutex_unlock(&libusb_mutex);
  return retval;
}

int setLCDBrightness(unsigned int level)
{
  int retval = 0;
  unsigned char usb_data[] = { 2, 2, 0, 0 };

  switch(level) 
  {
    case 1 : 
      usb_data[2] = 0x10; 
      break;
    case 2 : 
      usb_data[2] = 0x20; 
      break;
    default:
      usb_data[2] = 0x00;
  }
  pthread_mutex_lock(&libusb_mutex);
  retval = usb_control_msg(keyboard_device, USB_TYPE_CLASS + USB_RECIP_INTERFACE, 9, 0x302, 0, (char*)usb_data, 4, 10000); 
  pthread_mutex_unlock(&libusb_mutex);
  return retval;
}

/* set the keyboard backlight. doesnt affect lcd backlight. 0==off,1==medium,2==high */
int setKBBrightness(unsigned int level)
{
  int retval = 0;
  unsigned char usb_data[] = { 2, 1, 0, 0 };

  switch(level) 
  {
    case 1 : 
      usb_data[2] = 0x1; 
      break;
    case 2 : 
      usb_data[2] = 0x2; 
      break;
    default:
      usb_data[2] = 0x0;
  }
  pthread_mutex_lock(&libusb_mutex);
  retval = usb_control_msg(keyboard_device, USB_TYPE_CLASS + USB_RECIP_INTERFACE, 9, 0x302, 0, (char*)usb_data, 4, 10000); 
  pthread_mutex_unlock(&libusb_mutex);
  return retval;
}

static unsigned char g15KeyToLogitechKeyCode(int key)
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

static void processKeyEvent(unsigned int *pressed_keys, unsigned char *buffer)
{
  int i;
  
  *pressed_keys = 0;
  /*printf("Buffer: %x, %x, %x, %x, %x, %x, %x, %x, %x\n",buffer[0],buffer[1],buffer[2],buffer[3],buffer[4],buffer[5],buffer[6],buffer[7],buffer[8]);
  */
  if (buffer[0] == 0x02)
  {
    if (buffer[1]&0x01)
      *pressed_keys |= G15_KEY_G1;
    
    if (buffer[2]&0x02)
      *pressed_keys |= G15_KEY_G2;

    if (buffer[3]&0x04)
      *pressed_keys |= G15_KEY_G3;
    
    if (buffer[4]&0x08)
      *pressed_keys |= G15_KEY_G4;
    
    if (buffer[5]&0x10)
      *pressed_keys |= G15_KEY_G5;

    if (buffer[6]&0x20)
      *pressed_keys |= G15_KEY_G6;

    
    if (buffer[2]&0x01)
      *pressed_keys |= G15_KEY_G7;
    
    if (buffer[3]&0x02)
      *pressed_keys |= G15_KEY_G8;
    
    if (buffer[4]&0x04)
      *pressed_keys |= G15_KEY_G9;
    
    if (buffer[5]&0x08)
      *pressed_keys |= G15_KEY_G10;
    
    if (buffer[6]&0x10)
      *pressed_keys |= G15_KEY_G11;
    
    if (buffer[7]&0x20)
      *pressed_keys |= G15_KEY_G12;
    
    if (buffer[1]&0x04)
      *pressed_keys |= G15_KEY_G13;
    
    if (buffer[2]&0x08)
      *pressed_keys |= G15_KEY_G14;
    
    if (buffer[3]&0x10)
      *pressed_keys |= G15_KEY_G15;
    
    if (buffer[4]&0x20)
      *pressed_keys |= G15_KEY_G16;
    
    if (buffer[5]&0x40)
      *pressed_keys |= G15_KEY_G17;
    
    if (buffer[8]&0x40)
      *pressed_keys |= G15_KEY_G18;
    
    if (buffer[6]&0x01)
      *pressed_keys |= G15_KEY_M1;
    if (buffer[7]&0x02)
      *pressed_keys |= G15_KEY_M2;
    if (buffer[8]&0x04)
      *pressed_keys |= G15_KEY_M3;
    if (buffer[7]&0x40)
      *pressed_keys |= G15_KEY_MR;

    if (buffer[8]&0x80)
      *pressed_keys |= G15_KEY_L1;
    if (buffer[2]&0x80)
      *pressed_keys |= G15_KEY_L2;
    if (buffer[3]&0x80)
      *pressed_keys |= G15_KEY_L3;
    if (buffer[4]&0x80)
      *pressed_keys |= G15_KEY_L4;
    if (buffer[5]&0x80)
      *pressed_keys |= G15_KEY_L5;

    if (buffer[1]&0x80)
      *pressed_keys |= G15_KEY_LIGHT;

  }
}

int getPressedKeys(unsigned int *pressed_keys, unsigned int timeout)
{
  unsigned char buffer[9];
  int ret = 0;
  pthread_mutex_lock(&libusb_mutex);
    ret = usb_interrupt_read(keyboard_device, 0x81, (char*)buffer, 9, timeout);
  pthread_mutex_unlock(&libusb_mutex);
   
  if (ret == 9)
  {
    if (buffer[0] == 1)
      return G15_ERROR_TRY_AGAIN;
    
    processKeyEvent(pressed_keys, buffer);
    
    return G15_NO_ERROR;
  }
 return handle_usb_errors("Keyboard Read", ret); /* allow the app to deal with errors */
}
