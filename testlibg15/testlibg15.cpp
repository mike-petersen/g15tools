
#include <iostream>
#include <libg15.h>

using namespace std;

int const NUM_BYTES = G15_LCD_WIDTH*G15_LCD_HEIGHT/8;
unsigned char test_data[NUM_BYTES];

int main(int argc, char *argv[])
{
  int ret = 0;
  cout << "Trying to init libg15" << endl;
  ret = initLibG15();
  cout << "Done, return value is " << ret << endl;
  
  int i;
  for (i=0;i<NUM_BYTES;++i) 
    test_data[i] = 0xFF;
  writePixmapToLCD(test_data);
  
  sleep(1);

  cout << "Size of buffer: " <<   NUM_BYTES << endl;
  
  ret = setLCDContrast(G15_CONTRAST_LOW);
  cout << "Setting contrast, ret is " << ret << endl;
  sleep(1);
  ret = setLCDContrast(G15_CONTRAST_MEDIUM);
  cout << "Setting contrast, ret is " << ret << endl;
  sleep(1);
  ret = setLCDContrast(G15_CONTRAST_HIGH);
  cout << "Setting contrast, ret is " << ret << endl;
  sleep(1);

  
  ret = setLCDBrightness(G15_BRIGHTNESS_DARK);
  cout << "Setting the brightness: " << ret << endl;
  sleep(1);
  ret = setLCDBrightness(G15_BRIGHTNESS_MEDIUM);
  cout << "Setting the brightness: " << ret << endl;
  sleep(1);
  ret = setLCDBrightness(G15_BRIGHTNESS_BRIGHT);
  cout << "Setting the brightness: " << ret << endl;
  sleep(1);
  

  cout << "Going to play with leds now" << endl;
  ret = setLEDs(G15_LED_M1);
  cout << "Setting leds returned: " << ret << endl;
  sleep(1);
  ret = setLEDs(G15_LED_M2);
  cout << "Setting leds returned: " << ret << endl;
  sleep(1);
  ret = setLEDs(G15_LED_M3);
  cout << "Setting leds returned: " << ret << endl;
  sleep(1);
  ret = setLEDs(G15_LED_MR);
  cout << "Setting leds returned: " << ret << endl;
  sleep(1);
  ret = setLEDs(G15_LED_M1|G15_LED_MR);
  cout << "Setting leds returned: " << ret << endl;
  sleep(1);
  ret = setLEDs(G15_LED_M2|G15_LED_M3);
  cout << "Setting leds returned: " << ret << endl;
  sleep(1);
  ret = setLEDs(G15_LED_M1|G15_LED_M3);
  cout << "Setting leds returned: " << ret << endl;
  sleep(1);
  ret = setLEDs(G15_LED_M2|G15_LED_MR);
  cout << "Setting leds returned: " << ret << endl;
  sleep(1);
  ret = setLEDs(G15_LED_M1|G15_LED_M2|G15_LED_M3|G15_LED_MR);
  cout << "Setting leds returned: " << ret << endl;
  sleep(1);

 
}
