#ifndef weditbox_class
#define weditbox_class

#include "wcontrol.hpp"

WCLASS WEditBox : public WControl
{
	public:
		WEXPORT WEditBox( WWindow* parent, const WRect& r, char* text=NIL, WStyle wstyle=ES_LEFT );
		WEXPORT ~WEditBox();
		virtual bool WEXPORT processCmd( WORD id, WORD code );
};

#endif //weditbox_class
