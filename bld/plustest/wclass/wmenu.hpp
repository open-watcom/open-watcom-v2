#ifndef wmenu_class
#define wmenu_class

extern "C" {
	#include <windows.h>
}

#include "wobject.hpp"
#include "wvlist.hpp"

WCLASS WWindow;
WCLASS WMenuItem;
WCLASS WPopupMenu;
WCLASS WMenu : public WObject
{
	public:
		WEXPORT WMenu( bool create=TRUE );
		WEXPORT ~WMenu();
		void WEXPORT setParent( WWindow* par ) { _parent = par; }

		HMENU WEXPORT handle() { return _handle; }

		void WEXPORT insertSeparator( WORD index=-1 );
		void WEXPORT insertItem( WMenuItem* item, WORD index=-1 );
		void WEXPORT insertItem( WPopupMenu* popup, WORD index=-1 );
		void WEXPORT removeItem( WMenuItem* item );
		void WEXPORT removeItem( WPopupMenu* popup );
	protected:
		HMENU	_handle;
		WWindow*_parent;
		WVList	_children;
};

#endif