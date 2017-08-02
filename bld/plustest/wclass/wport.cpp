#include "wport.hpp"

extern "C" {
	#include <string.h>
}

WEXPORT WPort::WPort( WWindow* host )
	: _dc( 0 )
	, _host( host )
	, _font( NIL )
	, _brush( NIL )
	, _pen( NIL )
	, _palette( NIL )
{
	_dc = GetDC( _host->handle() );
}

WEXPORT WPort::~WPort()
{
	ReleaseDC( _host->handle(), _dc );
}

void WEXPORT WPort::textOut( const WRect& r, char* text )
{
	TextOut( _dc, r.x(), r.y(), text, strlen( text ) );
}
