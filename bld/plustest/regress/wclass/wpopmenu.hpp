#ifndef wpopupmenu_class
#define wpopupmenu_class

#include "wmenu.hpp"
#include "wstring.hpp"

WCLASS WPopupMenu : public WMenu
{
	public:
		WEXPORT WPopupMenu( char* text );
		WEXPORT ~WPopupMenu();
		void WEXPORT setParent( WMenu* par ) { _parent = par; }

		char* WEXPORT text() { return (char*)_text; }
	private:
		WMenu*	_parent;
		WString	_text;
};

#endif
