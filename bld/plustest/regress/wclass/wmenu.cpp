#include "wmenu.hpp"

#include "wwindow.hpp"
#include "wmenuitm.hpp"
#include "wpopmenu.hpp"

WEXPORT WMenu::WMenu( bool create )
	: _handle( 0 )
	, _parent( NIL )
{
	if( create ) {
		_handle = CreateMenu();
	}
}

WEXPORT WMenu::~WMenu()
{
	while( _children.count() > 0 ) {
		delete _children.last();
	}
	ifptr( _parent ) {
		_parent->clearMenu();
	}
	DestroyMenu( _handle );
}

void WEXPORT WMenu::insertSeparator( WORD index )
{
	InsertMenu( _handle, index, MF_SEPARATOR, 0, 0 );
}

void WEXPORT WMenu::insertItem( WMenuItem* item, WORD index )
{
	InsertMenu( _handle, index, MF_BYPOSITION, item->id(), item->text() );
	_children.add( item );
	item->setParent( this );
}

void WEXPORT WMenu::insertItem( WPopupMenu* popup, WORD index )
{
	InsertMenu( _handle, index, MF_BYPOSITION|MF_POPUP, popup->handle(), popup->text() );
	_children.add( popup );
	popup->setParent( this );
}

void WEXPORT WMenu::removeItem( WMenuItem* item )
{
	int index = _children.indexOfSame( item );
	if( index >= 0 ) {
		RemoveMenu( _handle, index, MF_BYPOSITION );
		_children.removeAt( index );
		item->setParent( NIL );
	}
}

void WEXPORT WMenu::removeItem( WPopupMenu* popup )
{
	int index = _children.indexOfSame( popup );
	if( index >= 0 ) {
		RemoveMenu( _handle, index, MF_BYPOSITION );
		_children.removeAt( index );
		popup->setParent( NIL );
	}
}
