#include <conio.h>
#include <graph.h>

long colors[ 16 ] = {
   _BRIGHTWHITE, _YELLOW, _LIGHTMAGENTA, _LIGHTRED,
   _LIGHTCYAN, _LIGHTGREEN, _LIGHTBLUE, _GRAY, _WHITE,
   _BROWN, _MAGENTA, _RED, _CYAN, _GREEN, _BLUE, _BLACK,
};

main()
{
    int x, y;

    _setvideomode( _VRES16COLOR );
    for( y = 0; y < 4; ++y ) {
	for( x = 0; x < 4; ++x ) {
	    _setcolor( x + 4 * y );
	    _rectangle( _GFILLINTERIOR,
		    x * 160, y * 120,
		    ( x + 1 ) * 160, ( y + 1 ) * 120 );
	}
    }
    getch();
    _remapallpalette( colors );
    getch();
    _setvideomode( _DEFAULTMODE );
}
