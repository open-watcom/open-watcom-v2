#ifndef wdraw_class
#define wdraw_class

#include "wwindow.hpp"

WCLASS WDraw : public WObject
{
	public:
		WEXPORT WDraw();
		WEXPORT ~WDraw();

		void WEXPORT setHandle( HANDLE handle ) { _handle = handle; }

	private:
		HANDLE		_handle;
};

#endif





