#include "wpopmenu.hpp"

WEXPORT WPopupMenu::WPopupMenu( char* text )
	: WMenu( FALSE )
	, _text( text )
	, _parent( NIL )
{
	_handle = CreatePopupMenu();
}

WEXPORT WPopupMenu::~WPopupMenu()
{
	ifptr( _parent ) {
		_parent->removeItem( this );
	}
}