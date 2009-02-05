#ifndef G15SCREEN_H_
#define G15SCREEN_H_

namespace G15Tools
{
	class G15Screen {
	protected:
		int g15screen_fd;
		int type;
		bool debug;
		unsigned char keys;
		void _init(int type);
		int _sendCommand(unsigned char command, unsigned char value);
	public:
		explicit G15Screen(bool debug = false);
		explicit G15Screen(int type, bool debug = false);
		G15Screen(const G15Screen& in);
		~G15Screen();
		int sendData(char *data, unsigned int len);
		int setKeyboardBacklight(unsigned char brightness);
		int setBacklight(unsigned char brightness);
		int setContrast(unsigned char contrast);
		int setM1Led(bool on = true);
		int setM2Led(bool on = true);
		int setM3Led(bool on = true);
		int setMRLed(bool on = true);
		int getKeystate();
	};
}

#endif
