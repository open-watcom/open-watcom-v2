#ifndef wtext_class
#define wtext_class

#include "wcontrol.hpp"

WCLASS WText : public WControl
{
	public:
		WEXPORT WText( WWindow* parent, const WRect& r, char* text=NIL, DWORD style=SS_LEFT );
		WEXPORT ~WText();
};

#endif //wtext_class
