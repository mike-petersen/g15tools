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
		explicit G15Screen(const bool debug = false);
		explicit G15Screen(const int type, const bool debug = false);
		G15Screen(const G15Screen& in);
		~G15Screen();
		int sendData(const char *data, const unsigned int len);
		int setKeyboardBacklight(const unsigned char brightness);
		int setBacklight(const unsigned char brightness);
		int setContrast(const unsigned char contrast);
		int setM1Led(const bool on = true);
		int setM2Led(const bool on = true);
		int setM3Led(const bool on = true);
		int setMRLed(const bool on = true);
		int getKeystate();
	};
}

#endif
