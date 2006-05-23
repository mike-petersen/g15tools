
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
  
  /* this one return G15_NO_ERROR on success, something
   * else otherwise (for instance G15_ERROR_OPENING_USB_DEVICE */
  int initLibG15();
  
  int writePixmapToLCD(char const *data);
  

#ifdef __cplusplus
}
#endif
	
#endif
