#include "wmenuitm.hpp"
#include "wwindow.hpp"
#include "wobjmap.hpp"
#include "wmenu.hpp"

static WORD _menuIds = 0;

WEXPORT WMenuItem::WMenuItem( char* text, WObject* obj, cb pick )
	: _parent( NIL )
	, _text( text )
	, _client( obj )
	, _pick( pick )
{
	_id = _menuIds++;
	WWindow::_objMap.setThis( this, _id );
}

WEXPORT WMenuItem::~WMenuItem()
{
	ifptr( _parent ) {
		_parent->removeItem( this );
	}
	WWindow::_objMap.clearThis( this );
}

void WEXPORT WMenuItem::picked()
{
	if( isptr( _client ) && isptr( _pick ) ) {
		(_client->*_pick)();
	}
}
