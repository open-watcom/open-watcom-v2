#include <conio.h>
#include <graph.h>
#include <stdlib.h>

main()
{
    int x, y;

    _setvideomode( _VRES16COLOR );
    while( _grstatus() == _GROK ) {
	x = rand() % 700;
	y = rand() % 500;
	_setpixel( x, y );
    }
    getch();
    _setvideomode( _DEFAULTMODE );
}
