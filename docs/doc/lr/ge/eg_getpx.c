#include <conio.h>
#include <graph.h>
#include <stdlib.h>

main()
{
    int x, y;
    unsigned i;

    _setvideomode( _VRES16COLOR );
    _rectangle( _GBORDER, 100, 100, 540, 380 );
    for( i = 0; i <= 60000; ++i ) {
	x = 101 + rand() % 439;
	y = 101 + rand() % 279;
	_setcolor( _getpixel( x, y ) + 1 );
	_setpixel( x, y );
    }
    getch();
    _setvideomode( _DEFAULTMODE );
}
