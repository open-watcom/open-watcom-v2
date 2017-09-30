#include "wrect.hpp"

void WEXPORT WRect::fitInside( const WRect& r )
{
	if( _x >= r.w() ) _x = r.w()-2;
	if( _y >= r.h() ) _y = r.h()-2;
	if( _x+_w >= r.w() ) _w = r.w()-_x-1;
	if( _y+_h >= r.h() ) _h = r.h()-_y-1;
}
