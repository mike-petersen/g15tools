
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
  
  unsigned int key_state = 0;
for (;;)
{
  int ret = getPressedKeys(&key_state,0);
  if (ret == G15_ERROR_TRY_AGAIN)
    continue;
  
  cout << "Getting key states: " << ret << endl;
  cout << "Key state: " << key_state << endl;
  
  cout << "Pressed Keys: "  ;
  
  if (key_state & G15_KEY_G1)
    cout << "G1 ";
  if (key_state & G15_KEY_G2)
    cout << "G2 ";
  if (key_state & G15_KEY_G3)
    cout << "G3 ";
  if (key_state & G15_KEY_G4)
    cout << "G4 ";
  if (key_state & G15_KEY_G5)
    cout << "G5 ";
  if (key_state & G15_KEY_G6)
    cout << "G6 ";
  if (key_state & G15_KEY_G7)
    cout << "G7 ";
  if (key_state & G15_KEY_G8)
    cout << "G8 ";
  if (key_state & G15_KEY_G9)
    cout << "G9 ";
  if (key_state & G15_KEY_G10)
    cout << "G10 ";
  if (key_state & G15_KEY_G11)
    cout << "G11 ";
  if (key_state & G15_KEY_G12)
    cout << "G12 ";
  if (key_state & G15_KEY_G13)
    cout << "G13 ";
  if (key_state & G15_KEY_G14)
    cout << "G14 ";
  if (key_state & G15_KEY_G15)
    cout << "G15 ";
  if (key_state & G15_KEY_G16)
    cout << "G16 ";
  if (key_state & G15_KEY_G17)
    cout << "G17 ";
  if (key_state & G15_KEY_G18)
    cout << "G18 ";
  if (key_state & G15_KEY_G19)
    cout << "G19 ";
  if (key_state & G15_KEY_G20)
    cout << "G20 ";
  if (key_state & G15_KEY_G21)
    cout << "G21 ";
  if (key_state & G15_KEY_G22)
    cout << "G22 "; 
  
  if (key_state & G15_KEY_M1)
    cout << "M1 " ;
  if (key_state & G15_KEY_M2)
    cout << "M2 " ;
  if (key_state & G15_KEY_M3)
    cout << "M3 " ;
  if (key_state & G15_KEY_MR)
    cout << "MR " ;
  
  if (key_state & G15_KEY_L1)
    cout << "L1 " ;
  if (key_state & G15_KEY_L2)
    cout << "L2 " ;
  if (key_state & G15_KEY_L3)
    cout << "L3 " ;
  if (key_state & G15_KEY_L4)
    cout << "L4 " ;
  if (key_state & G15_KEY_L5)
    cout << "L5 " ;
  if (key_state & G15_KEY_LIGHT)
    cout << "LIGHT ";
/*  if (key_state & G15_KEY_JOYBL)
    cout << "JOYBL ";
  if (key_state & G15_KEY_JOYBD)
    cout << "JOYBD ";
  if (key_state & G15_KEY_JOYBS)
    cout << "JOYBS ";
  */
  cout << endl;
}
  
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
