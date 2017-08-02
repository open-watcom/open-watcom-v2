#include "wcontrol.hpp"

WEXPORT WControl::WControl( WWindow* parent, char* classname, const WRect& r, char* text, WStyle wstyle )
	: WWindow( parent, classname, r, text, wstyle|WS_CHILD )
{
    	WRect rr( r );
	if( rr.w() == 0 ) rr.w( 150 );
	if( rr.h() == 0 ) rr.h( 24 );
	move( rr );
}

WEXPORT WControl::~WControl()
{
}

bool WEXPORT WControl::processCmd( WORD id, WORD code )
{
	return WWindow::processCmd( id, code );
}

