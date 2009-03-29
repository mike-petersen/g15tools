#ifndef G15WBMP_H_
#define G15WBMP_H_

#include <G15Screen.h>
#include <libg15render.h>
#include <string>

namespace G15Tools
{
	class G15Wbmp {
	protected:
		int width;
		int height;
		bool debug;
		char *buffer;

	public:
		explicit G15Wbmp(char *filename, bool debug = false);
		G15Wbmp(const G15Wbmp& in);
		~G15Wbmp();
		inline int getWidth() { return this->width; };
		inline int getHeight() { return this->height; };
		inline const char* getBuffer() { return this->buffer; };
	};
}

#endif
