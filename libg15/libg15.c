
#include "libg15.h"
#include <stdio.h>
#include <usb.h>
#include <string.h>

static usb_dev_handle *keyboard_device = 0;

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

static usb_dev_handle * findAndOpenG15()
{
  struct usb_bus *bus = 0;
  struct usb_device *dev = 0;
  for (bus = usb_busses; bus; bus = bus->next) 
  {
    for (dev = bus->devices; dev; dev = dev->next)
    {

      if (dev->descriptor.idVendor == 1133 && dev->descriptor.idProduct == 49698)
      {
        int ret=0;
        char name_buffer[65535];
        usb_dev_handle *devh = 0;
        printf("Found g15, trying to open it\n");
        devh = usb_open(dev);
        
  
        if (!devh)
        {
          fprintf(stderr, "Error, could not open the keyboard\n");
          fprintf(stderr, "Perhaps you dont have enough permissions to access it\n");
          return 0;
        }
  
  
        usleep(25*1000);
  
        
        ret = usb_get_driver_np(devh, 0, name_buffer, 65535);
        
        if (!ret)
        {
          printf("Trying to detach drive currentl attached: \"%s\"\n",name_buffer);

          ret = usb_detach_kernel_driver_np(devh, 0);
          if (!ret)
          {
            printf("Success, detached the driver\n");
          }
          else
          {
            fprintf(stderr,"Sorry, I could not detached the driver, giving up\n");
            return 0;
          }
        }
        printf("Debug: %s\n",name_buffer);
  
        ret = usb_set_configuration(devh, 1);
        if (ret)
        {
          fprintf(stderr,"Error setting the configuration, this is fatal\n");
          return 0;
        }
  
        usleep(25*1000);
  
        ret = usb_claim_interface(devh,0);
        
        if (ret)
        {
          fprintf(stderr,"Error claiming interface, good day cruel world\n");
          return 0;
        }
        usleep(25*1000);
        printf("Done opening the keyboard\n");
        return devh;
      }
    }  
  }
  fprintf(stderr, "Error, keyboard not found, is it plugged in?\n");
  return 0;
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
  
  
	return retval;
}

static void dumpPixmapIntoLCDFormat(char *lcd_buffer, char const *data)
{
  unsigned int offset_from_start = 32;
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
    

      //if (val)
      //  printf("Setting pixel at row %d col %d to %d offset %d bit %d\n",curr_row,curr_col, val, offset, bit);
      
      if (val)
        lcd_buffer[offset_from_start + offset] = lcd_buffer[offset_from_start + offset] | 1 << bit;
      else
        lcd_buffer[offset_from_start + offset] = lcd_buffer[offset_from_start + offset]  &  ~(1 << bit);
    }
  }
}

int writePixmapToLCD(char const *data)
{
  int ret = 0;
  char lcd_buffer[0x03e0];
  memset(lcd_buffer,0,0x03e0);
  dumpPixmapIntoLCDFormat(lcd_buffer, data);
  
  /* the keyboard needs this magic byte */
  lcd_buffer[0] = 0x03;
  
  ret = usb_interrupt_write(keyboard_device, 2, lcd_buffer, 0x03e0, 10000);
  if (ret != 0x03e0)
  {
    fprintf(stderr, "Error writing pixmap to lcd, return value is %d instead of %d\n",ret,0x03e0);
    return G15_ERROR_WRITING_PIXMAP;
  }
  return 0;
}
