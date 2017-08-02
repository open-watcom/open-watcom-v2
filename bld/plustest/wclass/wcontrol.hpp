#ifndef wcontrol_class
#define wcontrol_class

#include "wwindow.hpp"
#include "wrect.hpp"

WCLASS WControl : public WWindow
{
	public:
		WEXPORT WControl( WWindow* parent, char* classname, const WRect& r, char* text, WStyle wstyle );
		WEXPORT ~WControl();

		virtual bool WEXPORT processCmd( WORD id, WORD code );
};

#endif //wcontrol_class
