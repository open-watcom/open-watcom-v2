#include "wdraw.hpp"

WEXPORT WDraw::WDraw()
	: _handle( 0 )
{
}

WEXPORT WDraw::~WDraw()
{
	if( _handle ) {
		DeleteObject( _handle );
	}
}
