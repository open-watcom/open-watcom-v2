#include "wbutton.hpp"

WEXPORT WButton::WButton( WWindow* parent, const WRect& r, char* text, WStyle wstyle )
	: WControl( parent, "BUTTON", r, text, wstyle )
	, _clickClient( NIL )
	, _click( NIL )
	, _dblClickClient( NIL )
	, _dblClick( NIL )
{
}

WEXPORT WButton::~WButton()
{
}

void WEXPORT WButton::onClick( WObject* client, cb click )
{
	_clickClient = client;
	_click = click;
}

void WEXPORT WButton::onDblClick( WObject* client, cb click )
{
	_dblClickClient = client;
	_dblClick = click;
}

bool WEXPORT WButton::processCmd( WORD id, WORD code )
{
	switch( code ) {
	case BN_CLICKED:
		if( isnil( _clickClient ) || isnil( _click ) ) break;
		(_clickClient->*_click)();
		return TRUE;
	case BN_DOUBLECLICKED:
		if( isnil( _dblClickClient ) || isnil( _dblClick ) ) break;
		(_dblClickClient->*_dblClick)();
		return TRUE;
	}
	return WControl::processCmd( id, code );
}
