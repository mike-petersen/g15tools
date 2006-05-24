
#ifndef _LIBG15_H_
#define _LIBG15_H_

#ifdef __cplusplus
extern "C"
{
#endif
	
  enum
  {
    G15_NO_ERROR = 0,
    G15_ERROR_OPENING_USB_DEVICE,
    G15_ERROR_WRITING_PIXMAP
  };
  
  enum
  {
    G15_LCD_HEIGHT = 43,
    G15_LCD_WIDTH = 160
  };
  
  enum
  {
    G15_LED_M1=1<<0,
    G15_LED_M2=1<<1,
    G15_LED_M3=1<<2,
    G15_LED_MR=1<<3
  };
  
  enum
  {
    G15_CONTRAST_LOW=0,
    G15_CONTRAST_MEDIUM,
    G15_CONTRAST_HIGH    
  };
  
  enum
  {
    G15_BRIGHTNESS_DARK=0,
    G15_BRIGHTNESS_MEDIUM,
    G15_BRIGHTNESS_BRIGHT
  };
  
  /* this one return G15_NO_ERROR on success, something
   * else otherwise (for instance G15_ERROR_OPENING_USB_DEVICE */
  int initLibG15();
  
  int writePixmapToLCD(unsigned char const *data);
  int setLCDContrast(unsigned int level);
  int setLEDs(unsigned int leds);
  int setLCDBrightness(unsigned int level);
  

#ifdef __cplusplus
}
#endif
	
#endif
