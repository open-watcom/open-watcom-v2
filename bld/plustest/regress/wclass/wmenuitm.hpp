#ifndef wmenuitem_class
#define wmenuitem_class

#include "windows.h"
#include "wobject.hpp"
#include "wstring.hpp"

WCLASS WMenu;
WCLASS WMenuItem : public WObject
{
	public:
		WEXPORT WMenuItem( char* text, WObject* obj=NIL, cb pick=NIL );
		WEXPORT ~WMenuItem();
		void WEXPORT setParent( WMenu* par ) { _parent = par; }
		WORD WEXPORT id() { return _id; }
		char* WEXPORT text() { return (char*)_text; }
		void WEXPORT picked();

	private:
		WMenu*		_parent;
		WORD		_id;
		WString		_text;
		WObject*	_client;
		cb		_pick;
};

#endif